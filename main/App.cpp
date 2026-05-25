/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "NoiseDetectionTask.h"
#include "SharedRingContext.h"
#include "SignalEvaluator.h"
#include "SignalProcessor.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "include/GPIOTask.h"
#include "sdkconfig.h"
#include "soc/gpio_num.h"
#include <inttypes.h>
#include <stdio.h>

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

  static SharedRingCtxt<int, 100> reed_shared_ring_ctxt{};
  static SharedRingCtxt<int, 100> infrared_shared_ring_ctxt{};

  GPIOTask reed_task(GPIO_NUM_39, reed_shared_ring_ctxt);
  reed_task.register_task("reed_task", 2048, 5);

  GPIOTask infrared_task(GPIO_NUM_21, infrared_shared_ring_ctxt);
  infrared_task.register_task("infrared_task", 2048, 5);

  IsEqualToZeroEvaluator<int> is_zero_evaluator;
  SignalProcessor signal_processor(reed_shared_ring_ctxt,
                                   infrared_shared_ring_ctxt, is_zero_evaluator,
                                   is_zero_evaluator);

  signal_processor.register_task("signal_processor", 4096, 10);

  NoiseDetectionTask noise_detection_task;
  NoiseDetectionI2SConfig noise_config = {
      .sample_rate = 16000,
      .SD_pin = GPIO_NUM_16,
      .BCKL_pin = GPIO_NUM_17,
      .WS_pin = GPIO_NUM_18,
      .bits_per_sample = I2S_DATA_BIT_WIDTH_24BIT,
  };
  noise_detection_task.setup(noise_config, I2S_NUM_0);
  noise_detection_task.register_task("noise_detection_task", 4096, 10);

  while (1) {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
