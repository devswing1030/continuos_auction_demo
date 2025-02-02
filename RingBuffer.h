
#ifndef AUCTION_RINGBUFFER_H
#define AUCTION_RINGBUFFER_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template <typename T>
class DisruptorHandler {
public:
    virtual void onEvent(T& event, int sequence) = 0;
};
template <typename T>
class RingBuffer;

template <typename T>
class Sequencer;

template <typename T>
class Consumer {
private:
    DisruptorHandler<T>* handler;
    int position = -1;
    bool stopped = false;
    std::thread thread;
    RingBuffer<T>* ringBuffer;
    int sequencer_idx;


    void run(RingBuffer<T>* ringBuffer) {
        while (!stopped) {
            auto [event, nextPosition] = ringBuffer->get(sequencer_idx, position);
            if (nextPosition == -1) {
                break;
            }
            handler->onEvent(*event, nextPosition);
            ringBuffer->notify(sequencer_idx, nextPosition, this);
        }
    }
public:
    explicit Consumer(DisruptorHandler<T>* handler, int sequencer_idx, RingBuffer<T>* ringBuffer)
            : handler(handler), sequencer_idx(sequencer_idx), ringBuffer(ringBuffer) {
    }

    void Start() {
        thread = std::thread(&Consumer::run, this, ringBuffer);
    }

    void Stop() {
        stopped = true;
    }

    int GetPosition() {
        return position;
    }

    void SetPosition(int position) {
        this->position = position;
    }

    void Join() {
        thread.join();
    }

    ~Consumer() {
        stopped = true;
        thread.join();
    }
};

template <typename T>
class Sequencer {
private:
    std::vector<Consumer<T>*> consumers;
    std::condition_variable position_released;
    bool stopped = false;
public:
    explicit Sequencer(std::vector<DisruptorHandler<T>*> handlers, int idx, RingBuffer<T>* ringBuffer) {
        for (auto handler : handlers) {
            consumers.push_back(new Consumer<T>(handler, idx, ringBuffer));
        }
    }

    void Start() {
        for (auto& consumer : consumers) {
            consumer->Start();
        }
    }

    void Stop() {
        stopped = true;
        for (auto& consumer : consumers) {
            consumer->Stop();
        }
        position_released.notify_all();
        for (auto& consumer : consumers) {
            consumer->Join();
        }
    }

    bool IsReleased(int head, int position, int capacity) {
        for (int i = 0; i < consumers.size(); i++) {
            if (consumers[i]->GetPosition() == -1) {
                return false;
            }
            int next_position = (consumers[i]->GetPosition() + 1) % capacity;
            if (next_position >= head && position >= head) {
                if (next_position <= position) {
                    return false;
                }
            }
            else if (next_position >= head && position < head) {
                return false;
            }
            else if (next_position < head && position >= head) {
            }
            else if (next_position < head && position < head) {
                if (next_position <= position) {
                    return false;
                }
            }
        }
        return true;
    }

    void notify() {
        position_released.notify_all();
    }

    void wait(int position, std::unique_lock<std::mutex>& lock, int head, int capacity) {
        position_released.wait(lock, [this, head, position, capacity]() { return IsReleased(head, position, capacity) || stopped;});
    }
};

// A thread-safe ring buffer implementation
template <typename T>
class RingBuffer {
private:
    std::vector<T> buffer;                // Internal buffer to hold elements
    size_t capacity;                      // Maximum capacity of the buffer
    size_t head;                          // Index of the front element
    size_t tail;                          // Index for inserting the next element
    size_t size;                          // Current number of elements in the buffer
    std::mutex mtx;                       // Mutex to protect shared data
    std::condition_variable not_full;     // Condition variable for "not full" state
    std::condition_variable not_empty;    // Condition variable for "not empty" state

    std::vector<Sequencer<T>*> sequencers;

    bool stopped = false;

public:
    // Constructor: Initialize the ring buffer with a given capacity
    explicit RingBuffer(size_t cap)
            : buffer(cap), capacity(cap), head(0), tail(0), size(0) {}

    void ChainHandler(std::vector<DisruptorHandler<T>*> handlers) {
        std::unique_lock<std::mutex> lock(mtx);
        sequencers.push_back(new Sequencer<T>(handlers, sequencers.size(), this));
    }

    void Start() {
        for (auto& sequencer : sequencers) {
            sequencer->Start();
        }
    }

    void Stop() {
        stopped = true;
        not_empty.notify_all();
        for (auto& sequencer : sequencers) {
            sequencer->Stop();
        }

    }

    // Push an element into the ring buffer (blocks if the buffer is full)
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until the buffer is not full
        not_full.wait(lock, [this]() { return size < capacity - 1; });

        // Add the element to the buffer
        buffer[tail] = item;
        tail = (tail + 1) % capacity;
        ++size;

        // Notify a consumer that new data is available
        not_empty.notify_all();
    }

    // Pop an element from the ring buffer (blocks if the buffer is empty)
    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until the buffer is not empty
        not_empty.wait(lock, [this]() { return size > 0; });

        // Retrieve the front element
        T item = buffer[head];
        head = (head + 1) % capacity;
        --size;

        // Notify a producer that there is space available
        not_full.notify_one();

        return item;
    }

    void notify(int sequencer_idx, int position, Consumer<T>* consumer) {
        std::unique_lock<std::mutex> lock(mtx);
        consumer->SetPosition(position);
        if (this->sequencers[sequencer_idx]->IsReleased(head, position, capacity)) {
            if (sequencer_idx == sequencers.size() - 1) {
                head = (head + 1) % capacity;
                --size;

                // Notify a producer that there is space available
                not_full.notify_one();
            } else {
                sequencers[sequencer_idx]->notify();
            }
        }
    }

    std::pair<T*, int> get(int sequencer_idx, int position) {
        std::unique_lock<std::mutex> lock(mtx);
        //std::cout << "get" << sequencer_idx << " " << position << std::endl;
        if (sequencer_idx == 0) {
            if (position == -1) {
                not_empty.wait(lock, [this]() { return size > 0 || stopped; });
            }
            else {
                not_empty.wait(lock, [this, position]() { return ((size > 0) && ((position + 1) % capacity != tail)) || stopped; });
            }
        }
        else {
            sequencers[sequencer_idx -1]->wait((position + 1) % capacity, lock, head, capacity);
        }
        if (stopped) {
            return std::pair(nullptr, -1);
        }
        return std::pair(&buffer[(position + 1) % capacity], (position + 1) % capacity);
    }
};

#endif //AUCTION_RINGBUFFER_H
