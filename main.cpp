
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
public:
    void onEvent(Event& event, int sequence) override {
        event.value1 = new int(event.value);
    }
};

class Handler2 : public EventHandler<Event> {
public:
    void onEvent(Event& event, int sequence) override {
        event.value2 = *event.value1;
    }
};

class Handler3 : public EventHandler<Event> {
private:
    int value = 0;
    bool passed = true;
public:
    void onEvent(Event& event, int sequence) override {
        if (event.value2 != value) {
            passed = false;
        }
        delete event.value1;
        event.value1 = nullptr;
        value = (value + 1) % 1000000000;
    }

    bool Passed() {
        return passed;
    }
};

void test() {
    Disruptor<Event> buffer(1024);

    std::vector<EventHandler<Event>*> handlers1;
    handlers1.push_back(new Handler1());
    std::vector<EventHandler<Event>*> handlers2;
    handlers2.push_back(new Handler2());
    std::vector<EventHandler<Event>*> handlers3;
    Handler3* handler3 = new Handler3();
    handlers3.push_back(handler3);

    buffer.Chain(handlers1);
    buffer.Chain(handlers2);
    buffer.Chain(handlers3);

    buffer.Start();

    int i = 0;
    while(handler3->Passed()) {
        Event event(i);
        buffer.Push(event);
        i = (i + 1) % 1000000000;
        if (i % 1000000 == 0) {
            std::cout << "i:" << i << std::endl;
        }

    }

    buffer.Stop();
}
int main() {
    test();
}
