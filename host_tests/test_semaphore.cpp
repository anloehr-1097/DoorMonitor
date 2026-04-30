#include "SharedRingContext.h"
#include <gtest/gtest.h>
#include <set>
#include <thread>

void produce(SharedRingCtxt<float, 8> &context) {}

TEST(SemaphoreTest, Basic) {
  SharedRingCtxt<float, 8> context;
  ASSERT_TRUE(true);
}

TEST(SemaphoreTest, TestAccess) {
  /*
   * Test that the semaphore correctly allows access from either one - produces
   * or consumer.
   */

  SharedRingCtxt<int, 4> context;
  // Simulate producer acquiring the SemaphoreTest

  context.access_sem.acquire();

  std::atomic<bool> thread_finished{false};
  std::atomic<bool> thread_acquired{false};

  std::thread t([&]() {
    // will return false as main thread holds semaphore
    thread_acquired = context.access_sem.try_acquire();
    thread_finished = true;
  });
  while (!thread_finished) {
    std::this_thread::yield();
  }
  EXPECT_FALSE(thread_acquired);
  context.access_sem.release();

  t.join();
  EXPECT_TRUE(context.access_sem.try_acquire());
  context.access_sem.release();
}

TEST(SemaphoreTest, ProvokeRaceCondition) {

  std::atomic<bool> start{false};
  const int NUM_ITEMS = 10000;
  SharedRingCtxt<int, NUM_ITEMS * 2> context;

  std::thread t1([&]() {
    while (!start) {
      std::this_thread::yield();
    }
    for (int i = 1; i <= NUM_ITEMS; i++) {
      context.push(i);
    }
  });

  std::thread t2([&]() {
    while (!start) {
      std::this_thread::yield();
    }
    for (int i = NUM_ITEMS + 1; i <= NUM_ITEMS * 2; i++) {
      context.push(i);
    }
  });

  start = true;
  t1.join();
  t2.join();

  // Test no values were lost
  int actual_count = 0;
  while (!context.buffer.empty()) {
    auto val = context.buffer.pop();
    if (val.has_value()) {
      actual_count++;
    }
  }
  EXPECT_EQ(NUM_ITEMS * 2, actual_count);
}

TEST(SemaphoreTest, ProvokeRaceCondition_RawBuffer_Fails) {
  std::atomic<bool> start{false};
  const int NUM_ITEMS = 10000;

  // Use the raw RingBuffer without semaphore protection
  RingBuffer<int, NUM_ITEMS * 2> buffer;

  std::thread t1([&]() {
    while (!start) {
      std::this_thread::yield();
    }
    for (int i = 1; i <= NUM_ITEMS; i++) {
      buffer.push(i);
    }
  });

  std::thread t2([&]() {
    while (!start) {
      std::this_thread::yield();
    }
    for (int i = NUM_ITEMS + 1; i <= NUM_ITEMS * 2; i++) {
      buffer.push(i);
    }
  });

  start = true;
  t1.join();
  t2.join();

  int actual_count = 0;
  while (!buffer.empty()) {
    auto val = buffer.pop();
    if (val.has_value()) {
      actual_count++;
    }
  }
  // We use EXPECT_NE here because the race condition on the unprotected buffer
  // will cause data loss due to concurrent overwrites of write_pos.
  EXPECT_NE(NUM_ITEMS * 2, actual_count);
}
