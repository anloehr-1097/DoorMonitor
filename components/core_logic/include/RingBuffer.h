#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <array>
#include <optional>
#include <type_traits>

template <class T>
concept IsTriviallyCopyable = std::is_trivially_copyable<T>::value;

// A simple ring buffer implementation that can hold up to N elements of type T.
// The buffer is designed to be used in a single producer, single consumer
// scenario. Elements of the buffer should be trivially copyable to ensure that
// the push and pop operations are efficient.
template <IsTriviallyCopyable T, unsigned int N> class RingBuffer {
  std::array<T, N> buffer;
  bool full = false;
  unsigned int read_pos = 0;

  /*
   * The var ~write_pos~ is the index where the last recent update was written.
   * The next update will be written to (write_pos + 1) % N
   */
  unsigned int write_pos = 0;

public:
  RingBuffer() : buffer{} {};
  void push(T val) {
    if (full) {
      // overwrite oldest data
      read_pos = (read_pos + 1) % N;
    }

    buffer[write_pos] = val;
    write_pos = (write_pos + 1) % N;
    full = write_pos == read_pos;
  }

  std::optional<T> pop() {
    if (empty()) {
      return std::nullopt;
    }
    T val = buffer[read_pos];
    read_pos = (read_pos + 1) % N;
    full = false;
    return val;
  }

  bool empty() const { return read_pos == write_pos && !full; }
};

#endif // RINGBUFFER_H
