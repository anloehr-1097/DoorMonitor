#include "include/ListenerTask.h"
#include "esp_attr.h"
#include <span>

// init static member
DMA_ATTR uint8_t ListenerTask::buffer[1024] = {0};
