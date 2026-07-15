/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "NoiseDetectionTask.h"
#include "SharedRingContext.h"
#include "SignalEvaluator.h"
#include "SignalProcessor.h"
#include "WebsocketClient.h"
#include "WebsocketParser.h"
#include "WifiConnectTask.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_event_base.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "hal/i2s_types.h"
#include "helpers.h"
#include "include/GPIOTask.h"
#include "include/helpers.h"
#include "nvs_flash.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include "secrets.h"
#include "soc/gpio_num.h"
#include <cstdint>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>

static EventGroupHandle_t wifi_event_group;
constexpr bool noise_detect{false};
constexpr bool reed_gpio_process{false};

extern "C" void app_main(void) {
  /* Print chip information */
  esp_chip_info_t chip_info;
  uint32_t flash_size;
  esp_chip_info(&chip_info);
  printf("This is %s chip with %d CPU core(s), %s%s%s%s, ", CONFIG_IDF_TARGET,
         chip_info.cores,
         (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
         (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
         (chip_info.features & CHIP_FEATURE_IEEE802154)
             ? ", 802.15.4 (Zigbee/Thread)"
             : "");

  unsigned major_rev = chip_info.revision / 100;
  unsigned minor_rev = chip_info.revision % 100;
  printf("silicon revision v%d.%d, ", major_rev, minor_rev);
  if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    printf("Get flash size failed");
    return;
  }

  printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                       : "external");

  printf("Minimum free heap size: %" PRIu32 " bytes\n",
         esp_get_minimum_free_heap_size());

  esp_err_t nvs_ret = nvs_flash_init();
  if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    nvs_ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(nvs_ret);

  wifi_event_group = xEventGroupCreate();
  WifiConnectTask wifi_task(WIFI_SSID, WIFI_PASSWORD, wifi_event_group);
  wifi_task.register_task("wifi_task", 4096, 11);

  auto wifi_bits =
      xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdTRUE, pdFALSE,
                          6000 / portTICK_PERIOD_MS);
  if ((wifi_bits & WIFI_CONNECTED_BIT) != 0) {
    std ::cout << "Wifi connect success.";
  } else {
    std::cout << "Wifi Connect Failure.";
    esp_restart();
  }

  WebsocketClient ws_client{"ws://192.168.178.55"};
  ws_client.register_handler([](void *handler_args, esp_event_base_t ev_base,
                                int32_t event_id, void *event_data) {
    esp_websocket_event_data_t *data =
        static_cast<esp_websocket_event_data_t *>(event_data);

    auto *conn = static_cast<WebsocketClient *>(handler_args);

    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
      ESP_LOGI("WS Handler", "WEBSOCKET_EVENT_CONNECTED");
      conn->on_connected();
      break;
    case WEBSOCKET_EVENT_DISCONNECTED:
      ESP_LOGI("WS Handler", "WEBSOCKET_EVENT_DISCONNECTED");
      break;
    case WEBSOCKET_EVENT_DATA:
      ESP_LOGI("WS Handler", "WEBSOCKET_EVENT_DATA");
      ESP_LOGI("WS Handler", "Received opcode=%d", data->op_code);
      ESP_LOGW("WS Handler", "Received=%.*s", data->data_len,
               static_cast<const char *>(data->data_ptr));
      ESP_LOGW(
          "WS Handler",
          "Total payload length=%d, data_len=%d, current payload offset=%d\r\n",
          data->payload_len, data->data_len, data->payload_offset);
      break;
    case WEBSOCKET_EVENT_ERROR:
      ESP_LOGI("WS Handler", "WEBSOCKET_EVENT_ERROR");
      break;
    }
  });

  GenericWSParser<const char *> WsParser;
  ws_client.start();
  auto parsed_v = WsParser.parse("Parsed value").value();
  ws_client.send_co(parsed_v);

  if constexpr (reed_gpio_process) {
    static SharedRingCtxt<int, 100> reed_shared_ring_ctxt{};
    static SharedRingCtxt<int, 100> infrared_shared_ring_ctxt{};

    GPIOTask reed_task(GPIO_NUM_39, reed_shared_ring_ctxt);
    reed_task.register_task("reed_task", 2048, 5);

    GPIOTask infrared_task(GPIO_NUM_21, infrared_shared_ring_ctxt);
    infrared_task.register_task("infrared_task", 2048, 5);

    IsEqualToZeroEvaluator<int> is_zero_evaluator;
    SignalProcessor signal_processor(reed_shared_ring_ctxt,
                                     infrared_shared_ring_ctxt,
                                     is_zero_evaluator, is_zero_evaluator);

    signal_processor.register_task("signal_processor", 4096, 10);
  }

  if constexpr (noise_detect) {
    NoiseDetectionTask noise_detection_task;
    NoiseDetectionI2SConfig noise_config = {
        .sample_rate = 16000, // 16kHz sampling rate
        .SD_pin = GPIO_NUM_16,
        .BCKL_pin = GPIO_NUM_17,
        .WS_pin = GPIO_NUM_18,
        .bits_per_sample = I2S_DATA_BIT_WIDTH_32BIT,
    };
    noise_detection_task.setup(noise_config, I2S_NUM_0);
    noise_detection_task.register_task("noise_detection_task", 4096, 10);
  }

  while (1) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
