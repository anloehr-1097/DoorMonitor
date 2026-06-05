#ifndef MAIN_INCLUDE_HELPERS_H_
#define MAIN_INCLUDE_HELPERS_H_

#include "esp_bit_defs.h"

const int WIFI_CONNECTED_BIT = BIT0;
const int WIFI_FAIL_BIT = BIT1;

constexpr unsigned int MAX_WIFI_RETRY_NUM = 10;

#endif // MAIN_INCLUDE_HELPERS_H_
