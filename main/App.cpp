/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "SharedRingContext.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "include/InfraredTask.h"
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
  //
  // for (int i = 10; i >= 0; i--) {
  //     printf("Restarting in %d seconds...\n", i);
  //     vTaskDelay(1000 / portTICK_PERIOD_MS);
  // }
  // printf("Restarting now.\n");
  // fflush(stdout);
  //
  //
  // esp_restart();

  static SharedRingCtxt<int, 100> shared_ring_ctxt{};
  esp_err_t res = gpio_reset_pin(GPIO_NUM_39);

  if (res != ESP_OK) {
    printf("Failed to reset GPIO32, error code: %d\n", res);
    return;
  }

  esp_err_t pures = gpio_set_pull_mode(GPIO_NUM_39, GPIO_PULLUP_ONLY);

  if (pures != ESP_OK) {
    printf("Failed to set GPIO39 pull mode, error code: %d\n", pures);
    return;
  }
  esp_err_t setd = gpio_set_direction(GPIO_NUM_39, GPIO_MODE_INPUT);
  if (setd != ESP_OK) {
    printf("Failed to set GPIO39 direction, error code: %d\n", setd);
    return;
  }

  GPIOTask gpio_task(GPIO_NUM_21, shared_ring_ctxt);
  gpio_task.register_task("gpio_task", 2048, 5);

  RingBuffer<float, 20> signal_buffer;

  while (1) {
    printf("GPIO39 state: %d\n", gpio_get_level(GPIO_NUM_39));
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
