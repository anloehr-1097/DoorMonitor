#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "TaskTemplate.h"

class SensorTask : public TaskTemplate {
private:
  static void static_task_wrapper(void *pvParameter) {
    SensorTask *run_task = static_cast<SensorTask *>(pvParameter);
    run_task->task();
  }

public:
  explicit SensorTask();
}

#endif // SENSOR_TASK_H
