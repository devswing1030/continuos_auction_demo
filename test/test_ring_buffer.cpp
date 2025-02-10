#include <gtest/gtest.h>
#include "Disruptor.h"
#include <thread>

class Event {
public:
    int value = 0;
    int* value1 = nullptr;
    int value2 = 0;
    Event() = default;
    Event(int value) : value(value) {}
};

class Handler1 : public EventHandler<Event> {
private:
    int tag;
public:
    Handler1(int tag) : tag(tag) {
    }
    void onEvent(Event& event, int sequence) override {
        if (sequence % 3 != tag)
            return;
        event.value1 = new int(event.value);
    }
};

class Handler2 : public EventHandler<Event> {
private:
    int tag;
public:
    Handler2(int tag) : tag(tag) {
    }
    void onEvent(Event& event, int sequence) override {
        if (sequence % 3 != tag)
            return;
        event.value2 = *event.value1;
    }
};

class Handler3 : public EventHandler<Event> {
private:
    int value = 0;
public:
    void onEvent(Event& event, int sequence) override {
        EXPECT_EQ(event.value2, value);
        value++;
    }
};

    void test() {
        Disruptor<Event> buffer(1024);

        std::vector<EventHandler<Event>*> handlers1;
        for (int i = 0; i < 3; i++) {
            handlers1.push_back(new Handler1(i));
        }
        std::vector<EventHandler<Event>*> handlers2;
        for (int i = 0; i < 3; i++) {
            handlers2.push_back(new Handler2(i));
        }
        std::vector<EventHandler<Event>*> handlers3;
        handlers3.push_back(new Handler3());

        buffer.Chain(handlers1);
        buffer.Chain(handlers2);
        buffer.Chain(handlers3);

        buffer.Start();

        // calculate time elapse
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 2000000; i++) {
            Event event(i);
            buffer.Push(event);
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
    void consumer(Disruptor<int>& buffer) {
        for (int i = 0; i < 10; i++) {
            int value = buffer.pop();
            EXPECT_EQ(value, i);
        }
    }

TEST(RingBufferTest1, func) {
    Disruptor<int> buffer(1024);

    std::thread t1(consumer, std::ref(buffer));
    for (int i = 0; i < 10; i++) {
        buffer.Push(i);
    }
    t1.join();
}
 */


