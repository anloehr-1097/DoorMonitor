#include "include/SignalProcessor.h"
#include "SharedRingContext.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include <iostream>

SignalProcessor::SignalProcessor(ISharedRingCtxt<int> &first_ctxt,
                                 ISharedRingCtxt<int> &second_ctxt,
                                 SignalEvaluator<int> &first_eval,
                                 SignalEvaluator<int> &second_eval)
    : q1(first_ctxt), q2(second_ctxt), se1(first_eval), se2(second_eval) {
  // Constructor implementation (if needed)
}

void SignalProcessor::task() {
  while (true) {

    // If one of the queues is empty and the other is not, we need to wait to
    // not mess up the signal sync. This combined with the fact that we pop the
    // head of the queue ensures  that the signals processed are indeed from the
    // same 'read cycle'.

    if (q1.empty() || q2.empty()) {
      // Add a delay to prevent busy waiting
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    // Process signals from q1 and q2
    std::optional<int> signal1 = q1.pop();
    std::optional<int> signal2 = q2.pop();

    if (signal1.has_value() && signal2.has_value()) {
      std::cout << "Processed signal from q1: " << signal1.value() << std::endl;
      std::cout << "Processed signal from q2: " << signal2.value() << std::endl;

      if (se1(signal1.value()) && se2(signal2.value())) {
        std::cout << "Both signals turned ON" << std::endl;
      }
    }

    // Add a delay to prevent busy waiting
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
