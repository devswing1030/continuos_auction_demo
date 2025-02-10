
#ifndef AUCTION_CONTINUOSAUCTION_H
#define AUCTION_CONTINUOSAUCTION_H

#include "OrderBook.h"

template<typename Order, typename ExecutionCallback>
class ContinuosAuction {
    OrderBook<Order>& buyOrderBook;
    OrderBook<Order>& sellOrderBook;

public:
    ContinuosAuction(OrderBook<Order>& buyOrderBook, OrderBook<Order>& sellOrderBook) :
            buyOrderBook(buyOrderBook), sellOrderBook(sellOrderBook) {}

    void processNewOrder(Order* order, ExecutionCallback* callback) {
        OrderBook<Order>* orderBook = nullptr;
        OrderBook<Order>* couterOrderBook = nullptr;
        if (order->getSide() == Order::BUY) {
            orderBook = &buyOrderBook;
            couterOrderBook = &sellOrderBook;
        } else {
            orderBook = &sellOrderBook;
            couterOrderBook = &buyOrderBook;
        }

        auto upper = couterOrderBook->upper(order->getPrice());
        while (true) {
            auto it = couterOrderBook->begin();
            if (it == upper) {
                break;
            }
            while(true) {
                auto result = it->second.consumeOneOrder(order->getLeavesQty());
                if (!result) {
                    break;
                }
                int match_qty = result->second;
                order->execute(match_qty);
                callback->onExecution(order, result->first, match_qty, result->first->getPrice());
                if(order->getLeavesQty() == 0) {
                    break;
                }
            }
            if(order->getLeavesQty() == 0) {
                break;
            }
        }
        if(order->getLeavesQty() > 0) {
            orderBook->addOrder(order);
        }
    }

    void cancelOrder(Order* order) {
        if (order->getSide() == Order::BUY) {
            buyOrderBook.cancelOrder(order);
        } else {
            sellOrderBook.cancelOrder(order);
        }
    }
};

#endif //AUCTION_CONTINUOSAUCTION_H
