#ifndef WIFI_CONNECT_TASK_H
#define WIFI_CONNECT_TASK_H

#include <string>

#include "TaskTemplate.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "helpers.h"

class WifiConnectTask : public TaskTemplate {
private:
  std::string ssid;
  std::string password;
  unsigned int max_retry_num = MAX_WIFI_RETRY_NUM;
  EventGroupHandle_t event_group;

  void event_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data);

public:
  WifiConnectTask(std::string ssid, std::string password,
                  EventGroupHandle_t event_group);

  void task() override;
};

#endif // WIFI_CONNECT_TASK_H
