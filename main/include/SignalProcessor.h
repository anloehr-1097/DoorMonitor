#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include "SharedRingContext.h"
#include "TaskTemplate.h"

class SignalProcessor : public TaskTemplate {

private:
  ISharedRingCtxt<int> &q1;
  ISharedRingCtxt<int> &q2;

public:
  // TODO(al) deal with constness
  explicit SignalProcessor();
  SignalProcessor(ISharedRingCtxt<int> &first_ctxt,
                  ISharedRingCtxt<int> &second_ctxt);
  void task() override;
};

#endif // SIGNALPROCESSOR_H
