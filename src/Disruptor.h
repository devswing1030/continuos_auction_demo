
#ifndef AUCTION_DISRUPTOR_H
#define AUCTION_DISRUPTOR_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

/**
 * Event handler interface
 *
 * @tparam T Event type
 */
template <typename T>
class EventHandler {
public:
    /**
     * Handle an event. All event handlers must implement this method.
     * Every event handler will receive all events.
     *
     * @param event Event to handle
     * @param sequence Sequence number of the event in the ring buffer
     */
    virtual void onEvent(T& event, int sequence) = 0;
};


template <typename T>
class Disruptor;


template <typename T>
class Consumer {
private:
    EventHandler<T>* handler;
    int last_consumed = -1;   // Last consumed position, and the event at this position has been processed
    bool stopped = false;
    std::thread thread;
    Disruptor<T>* disruptor;
    int sequencer_idx;


    void run(Disruptor<T>* disruptor) {
        while (!stopped) {
            auto [event, retrieved] = disruptor->Get(sequencer_idx, last_consumed);
            if (retrieved == -1) {
                break;
            }
            handler->onEvent(*event, retrieved);
            // update last consumed position in the Notify function, ensure update with the same lock
            disruptor->Notify(sequencer_idx, retrieved, this);
        }
    }
public:
    explicit Consumer(EventHandler<T>* handler, int sequencer_idx, Disruptor<T>* disruptor)
            : handler(handler), sequencer_idx(sequencer_idx), disruptor(disruptor) {
    }

    void Start() {
        thread = std::thread(&Consumer::run, this, disruptor);
    }

    void Stop() {
        stopped = true;
    }

    int GetLastConsumed() {
        return last_consumed;
    }

    void SetLastConsumed(int position) {
        this->last_consumed = position;
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
public:
    std::vector<Consumer<T>*> consumers;
    std::condition_variable position_released;
public:
    explicit Sequencer(std::vector<EventHandler<T>*> handlers, int idx, Disruptor<T>* disruptor) {
        for (auto handler : handlers) {
            consumers.push_back(new Consumer<T>(handler, idx, disruptor));
        }
    }

    void Start() {
        for (auto& consumer : consumers) {
            consumer->Start();
        }
    }

    void Stop() {
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
            if (consumers[i]->GetLastConsumed() == -1) {
                return false;
            }
            int next_position = (consumers[i]->GetLastConsumed() + 1) % capacity;
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
};

// A thread-safe ring buffer implementation
template <typename T>
class Disruptor {
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
    friend class Consumer<T>;

public:
    /**
     * Constructor: Initialize the ring buffer with a given capacity
     *
     * @param cap Maximum capacity of the buffer
     */
    explicit Disruptor(size_t cap)
            : buffer(cap), capacity(cap), head(0), tail(0), size(0) {}

    /**
     * Add a chain of event handlers to the disruptor. The handlers in one chain will process events concurrently.
     * The chains will process events sequentially in the order they are added.
     *
     * @param handlers A vector of event handlers
     */
    void Chain(std::vector<EventHandler<T>*> handlers) {
        std::unique_lock<std::mutex> lock(mtx);
        sequencers.push_back(new Sequencer<T>(handlers, sequencers.size(), this));
    }

    /**
     * Start the disruptor. This will start all event handlers in the disruptor.
     */
    void Start() {
        for (auto& sequencer : sequencers) {
            sequencer->Start();
        }
    }

    /**
     * Stop the disruptor. This will stop all event handlers in the disruptor.
     */
    void Stop(bool force = true) {
        if (!force) {
            while (size > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        stopped = true;
        not_empty.notify_all();
        for (auto& sequencer : sequencers) {
            sequencer->Stop();
        }
    }

    /**
     * Push an element to the buffer. This method will block if the buffer is full.
     *
     * @param item Element to push
     */
    void Push(const T& item) {
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

protected:
    void Notify(int sequencer_idx, int position, Consumer<T>* consumer) {
        std::unique_lock<std::mutex> lock(mtx);
        consumer->SetLastConsumed(position);
        if (this->sequencers[sequencer_idx]->IsReleased(head, position, capacity)) {
            if (sequencer_idx == sequencers.size() - 1) {
                head = (head + 1) % capacity;
                --size;

                // Notify a producer that there is space available
                not_full.notify_one();
            } else {
                sequencers[sequencer_idx]->position_released.notify_all();
            }
        }
    }

    std::pair<T*, int> Get(int sequencer_idx, int position) {
        std::unique_lock<std::mutex> lock(mtx);
        //std::cout << "get" << sequencer_idx << " " << last_consumed << std::endl;
        if (sequencer_idx == 0) {
            if (position == -1) {
                not_empty.wait(lock, [this]() { return size > 0 || stopped; });
            }
            else {
                not_empty.wait(lock, [this, position]() { return ((size > 0) && ((position + 1) % capacity != tail)) || stopped; });
            }
        }
        else {
            sequencers[sequencer_idx -1]->position_released.wait(lock, [this, position, sequencer_idx]() {
                int next_position = (position + 1) % capacity;
                return sequencers[sequencer_idx -1]->IsReleased(head, next_position, capacity) || stopped;});
        }
        if (stopped) {
            return std::pair(nullptr, -1);
        }
        return std::pair(&buffer[(position + 1) % capacity], (position + 1) % capacity);
    }
};

#endif //AUCTION_DISRUPTOR_H
