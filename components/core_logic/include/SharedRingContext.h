#ifndef SHARED_RING_CONTEXT_H
#define SHARED_RING_CONTEXT_H

#include "RingBuffer.h"
#include <optional>
#include <semaphore>

// Interface for shared ring context to be used in none-template tasks
template <typename T> struct ISharedRingCtxt {
  virtual ~ISharedRingCtxt() = default;
  virtual void push(const T &item) = 0;
  virtual std::optional<T> pop() = 0;
  virtual bool empty() = 0;
};

// Encapsulate a RingBuffer and a binary semaphore to control
// access to buffer.
template <typename T, unsigned int N>
struct SharedRingCtxt : public ISharedRingCtxt<T> {
  RingBuffer<T, N> buffer;
  std::binary_semaphore access_sem;

  SharedRingCtxt() : buffer(), access_sem{1} {}

  void push(const T &item) override {
    access_sem.acquire();
    buffer.push(item);
    access_sem.release();
  }

  std::optional<T> pop() override {
    access_sem.acquire();
    std::optional<T> elem = buffer.pop();
    access_sem.release();
    return elem;
  }

  bool empty() override {
    access_sem.acquire();
    bool is_empty = buffer.empty();
    access_sem.release();
    return is_empty;
  }
};

#endif // SHARED_RING_CONTEXT_H
