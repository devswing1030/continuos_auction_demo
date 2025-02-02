
#ifndef AUCTION_ORDERBOOK_H
#define AUCTION_ORDERBOOK_H

#include <map>
#include "OrderQueue.h"

template<typename Order>
class OrderBook {
    using Iterator = typename std::map<int, OrderQueue<Order>>::iterator;
    std::map<int, OrderQueue<Order>> book;
    bool isHighPriceFirst;

public:
    OrderBook(bool isHighPriceFirst) : isHighPriceFirst(isHighPriceFirst) {}

    void addOrder(Order *order) {
        int price = order->getPrice();
        if (isHighPriceFirst) {
            price = -price;
        }
        auto it = book.find(price);
        if (it == book.end()) {
            book.emplace(price,OrderQueue<Order>(order->getPrice()));
            it = book.find(price);
        }
        it->second.insert(*order);
    }

    bool cancelOrder(Order* order) {
        int price = order->getPrice();
        if (isHighPriceFirst) {
            price = -price;
        }
        auto it = book.find(price);
        if (it != book.end()) {
            bool re = it->second.cancel(order->getOrderId());
            if (it->second.getQty() == 0) {
                book.erase(it);
            }
            return re;
        } else {
            return false;
        }
    }

    Iterator begin() {
        while(true) {
            auto it = book.begin();
            if (it == book.end()) {
                return book.end();
            }
            if (it->second.getQty() > 0) {
                return it;
            }
            book.erase(it);
        }
    }

    Iterator end() {
        return book.end();
    }

    Iterator upper(int price) {
        if (isHighPriceFirst) {
            price = -price;
        }
        while(true) {
            auto it = book.upper_bound(price);
            if (it == book.end()) {
                return book.end();
            }
            if (it->second.getQty() > 0) {
                return it;
            }
            book.erase(it);
        }
    }

    Iterator lower(int price) {
        if (isHighPriceFirst) {
            price = -price;
        }
        while(true) {
            auto it = book.lower_bound(price);
            if (it == book.end()) {
                return book.end();
            }
            if (it->second.getQty() > 0) {
                return it;
            }
            book.erase(it);
        }
    }
};

#endif //AUCTION_ORDERBOOK_H
