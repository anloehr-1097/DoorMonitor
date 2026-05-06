#include "include/SignalProcessor.h"
#include "SharedRingContext.h"
#include <iostream>

SignalProcessor::SignalProcessor(ISharedRingCtxt<int> &first_ctxt,
                                 ISharedRingCtxt<int> &second_ctxt)
    : q1(first_ctxt), q2(second_ctxt) {
  // Constructor implementation (if needed)
}

void SignalProcessor::task() {
  while (true) {
    // Process signals from q1 and q2
    std::optional<int> signal1 = q1.pop();
    std::optional<int> signal2 = q2.pop();

    if (signal1.has_value()) {
      std::cout << "Processed signal from q1: " << signal1.value() << std::endl;
    }

    if (signal2.has_value()) {
      std::cout << "Processed signal from q2: " << signal2.value() << std::endl;
    }

    // Add a delay to prevent busy waiting
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
