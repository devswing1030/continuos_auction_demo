//
//
#include <gtest/gtest.h>
#include "RingBuffer.h"
#include <thread>

void consumer(RingBuffer<int>& buffer) {
    for (int i = 0; i < 10; i++) {
        int value = buffer.pop();
        EXPECT_EQ(value, i);
    }
}

TEST(RingBufferTest1, func) {
    RingBuffer<int> buffer(1024);

    std::thread t1(consumer, std::ref(buffer));
    for (int i = 0; i < 10; i++) {
        buffer.push(i);
    }
    t1.join();
}
