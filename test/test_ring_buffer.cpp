#include <gtest/gtest.h>
#include "RingBuffer.h"
#include <thread>

class Event {
public:
    int value = 0;
    int* value1 = nullptr;
    int value2 = 0;
    Event() = default;
    Event(int value) : value(value) {}
};

class Handler1 : public DisruptorHandler<Event> {
public:
    void onEvent(Event& event, int sequence) override {
        event.value1 = new int(event.value);
    }
};

class Handler2 : public DisruptorHandler<Event> {
public:
    void onEvent(Event& event, int sequence) override {
        event.value2 = *event.value1;
    }
};

class Handler3 : public DisruptorHandler<Event> {
private:
    int value = 0;
public:
    void onEvent(Event& event, int sequence) override {
        EXPECT_EQ(event.value2, value);
        value++;
    }
};

    void test() {
        RingBuffer<Event> buffer(1024);

        std::vector<DisruptorHandler<Event>*> handlers1;
        handlers1.push_back(new Handler1());
        std::vector<DisruptorHandler<Event>*> handlers2;
        handlers2.push_back(new Handler2());
        std::vector<DisruptorHandler<Event>*> handlers3;
        handlers3.push_back(new Handler3());

        buffer.ChainHandler(handlers1);
        buffer.ChainHandler(handlers2);
        buffer.ChainHandler(handlers3);

        buffer.Start();

        // calculate time elapse
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 20000; i++) {
            Event event(i);
            buffer.push(event);
        }

        buffer.Stop();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    }
TEST(RingBufferTest, performance) {
        for (int i = 0; i < 1; i++) {
            test();
            std::cout << "test " << i << " done\n";
        }
    }

/*
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
 */


