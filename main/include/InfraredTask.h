#ifndef MAIN_INCLUDE_GPIOTASK_H_
#define MAIN_INCLUDE_GPIOTASK_H_

#include "SharedRingContext.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "portmacro.h"
#include "soc/gpio_num.h"

class GPIOTask {
private:
  TaskHandle_t task_handle;
  gpio_num_t gpio_num;
  ISharedRingCtxt<int> &shared_ring_ctxt;

  static void static_task_wrapper(void *pvParameter) {
    GPIOTask *run_task = static_cast<GPIOTask *>(pvParameter);
    run_task->task();
  }

public:
  GPIOTask(const gpio_num_t gpio_num, ISharedRingCtxt<int> &ctxt);
  void task();
  void register_task(const char *name, uint16_t stack_depth,
                     UBaseType_t priority);
};

#endif // MAIN_INCLUDE_GPIOTASK_H_
