#ifndef SIGNAL_EVALUATOR_H
#define SIGNAL_EVALUATOR_H

template <typename T> struct SignalEvaluator {
  virtual bool operator()(T signal) const = 0;
};

template <typename T>
struct IsEqualToZeroEvaluator : public SignalEvaluator<T> {
  bool operator()(T signal) const override { return signal == 0; }
};

#endif // SIGNAL_EVALUATOR_H
