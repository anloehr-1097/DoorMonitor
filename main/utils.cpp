#include "utils.h"
#include <iostream>

void handle_error(esp_err_t err) {
  if (err != ESP_OK) {
    // Handle the error, e.g., print an error message or take appropriate action
    // For example:
    std::cout << "Encountered ESP error " << err << std::endl;
    // You can also add more specific error handling based on the error code
  }
}
