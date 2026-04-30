#ifndef TASK_TEMPLATE_H
#define TASK_TEMPLATE_H

#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

class TaskTemplate {
private:
  TaskHandle_t task_handle;

public:
  virtual void task();
  virtual ~TaskTemplate() = 0;
  virtual void register_task(const char *name, uint16_t stack_depth,
                             UBaseType_t priority);
};

#endif // TASK_TEMPLATE_H
