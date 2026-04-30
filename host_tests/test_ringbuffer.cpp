#include "RingBuffer.h"
#include <gtest/gtest.h>

// Define a test suite named 'RingBufferTest'
TEST(RingBufferTest, TestCreation) {
  // Note: Since I don't know the exact interface of your RingBuffer,
  // this is a generic placeholder test to show GTest syntax.

  RingBuffer<int, 10> buffer;
  // Example:
  // RingBuffer buffer;
  // EXPECT_TRUE(buffer.isEmpty());

  // GTest assertion examples:
  EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, TestPushAndPop) {
  RingBuffer<int, 3> buffer;

  buffer.push(1);
  buffer.push(2);
  buffer.push(3);

  EXPECT_FALSE(buffer.empty());

  auto val1 = buffer.pop();
  EXPECT_TRUE(val1.has_value());
  EXPECT_EQ(val1.value(), 1);
  EXPECT_FALSE(buffer.empty());

  auto val2 = buffer.pop();
  EXPECT_TRUE(val2.has_value());
  EXPECT_EQ(val2.value(), 2);
  EXPECT_FALSE(buffer.empty());

  auto val3 = buffer.pop();
  EXPECT_TRUE(val3.has_value());
  EXPECT_EQ(val3.value(), 3);
  EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, TestOverwrite) {
  RingBuffer<int, 2> buffer;

  buffer.push(1);
  buffer.push(2);
  buffer.push(3); // should overwrite '1'

  auto val1 = buffer.pop();
  EXPECT_TRUE(val1.has_value());
  EXPECT_EQ(val1.value(), 2); // '2' should be the first value now

  auto val2 = buffer.pop();
  EXPECT_TRUE(val2.has_value());
  EXPECT_EQ(val2.value(), 3);

  auto val3 = buffer.pop();
  EXPECT_FALSE(val3.has_value());
  EXPECT_TRUE(buffer.empty());
}
