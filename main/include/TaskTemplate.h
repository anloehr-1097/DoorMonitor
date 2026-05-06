#ifndef TASK_TEMPLATE_H
#define TASK_TEMPLATE_H

#include "freertos/idf_additions.h"
#include "freertos/task.h"

class TaskTemplate {
private:
  TaskHandle_t task_handle;

  // Provide a single static wrapper in the base class
  static void base_task_wrapper(void *pvParameter) {
    TaskTemplate *task_instance = static_cast<TaskTemplate *>(pvParameter);
    task_instance->task(); // Virtual dispatch calls the derived class's task()
  }

public:
  virtual void task() = 0;
  virtual ~TaskTemplate() = default;
  virtual void register_task(const char *name, uint16_t stack_depth,
                             UBaseType_t priority) {
    xTaskCreate(base_task_wrapper, name, stack_depth, this, priority,
                &task_handle);
  }
};

#endif // TASK_TEMPLATE_H
