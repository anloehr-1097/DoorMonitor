#include "include/InfraredTask.h"
#include "SharedRingContext.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "hal/gpio_types.h"
#include <cstdio>

GPIOTask::GPIOTask(const gpio_num_t gpio_num, ISharedRingCtxt<int> &ctxt)
    : gpio_num(gpio_num), shared_ring_ctxt(ctxt) {
  // reset pin
  //
  esp_err_t err;
  err = gpio_reset_pin(gpio_num);
  ESP_ERROR_CHECK(err);
  if (err != ESP_OK) {
    printf("Failed to reset GPIO%d, error code: %d\n", gpio_num, err);
    return;
  }
  // pull up / down
  err = gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
  ESP_ERROR_CHECK(err);
  if (err != ESP_OK) {
    printf("Failed to reset GPIO%d, error code: %d\n", gpio_num, err);
    return;
  }

  // direction
  err = gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
  ESP_ERROR_CHECK(err);
  if (err != ESP_OK) {
    printf("Failed to reset GPIO%d, error code: %d\n", gpio_num, err);
    return;
  }
}

void GPIOTask::task() {
  while (true) {
    // Toggle GPIO pin
    int level = gpio_get_level(this->gpio_num);
    printf("GPIO%d level: %d\n", this->gpio_num, level);
    shared_ring_ctxt.push(level);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void GPIOTask::register_task(const char *name, uint16_t stack_depth,
                             UBaseType_t priority) {
  xTaskCreate(static_task_wrapper, name, stack_depth, this, priority,
              &task_handle);
}
