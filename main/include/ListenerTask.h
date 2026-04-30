#ifndef LISTENER_TASK_H
#define LISTENER_TASK_H

#include "TaskTemplate.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

class ListenerTask : public TaskTemplate {
private:
  static void static_task_wrapper(void *pvParameter) {
    ListenerTask *run_task = static_cast<ListenerTask *>(pvParameter);
    run_task->task();
  }

public:
  static DMA_ATTR uint8_t
      buffer[1024]; // use this to store the data read from the serial port
  explicit ListenerTask();
};

#endif // LISTENER_TASK_H
