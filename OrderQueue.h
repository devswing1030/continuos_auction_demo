
#ifndef AUCTION_ORDERQUEUE_H
#define AUCTION_ORDERQUEUE_H

#include <list>
#include <unordered_map>

template<typename Order>
class OrderQueue {
    using Iterator = typename std::list<Order*>::iterator;
    std::list<Order*> orders;
    std::unordered_map<int, typename std::list<Order*>::iterator> orderMap;
    int qty;
    int price;
public:
    OrderQueue(int price) : qty(0), price(price) {}
    void insert(Order& order) {
        orders.push_back(&order);
        if (orderMap.find(order.getOrderId()) == orderMap.end()) {
            orderMap[order.getOrderId()] = --orders.end();
        }
        else {
            throw std::runtime_error("Duplicate order ID");
        }
        qty += order.getLeavesQty();
    }
    bool cancel(int orderId) {
        auto it = orderMap.find(orderId);
        if (it != orderMap.end()) {
            this->qty -= (*(it->second))->getLeavesQty();
            orders.erase(it->second);
            orderMap.erase(it);
            return true;
        }
        return false;
    }

    int getPrice() const {
        return price;
    };

    int getQty() const {
        return qty;
    }

    Iterator begin() {
        return orders.begin();
    }

    Iterator end() {
        return orders.end();
    }

    std::optional<std::pair<Order*, int>> consumeOneOrder(int qty) {
        if (orders.empty()) {
            return std::nullopt;
        }
        Order* order = orders.front();
        int orderQty = order->getLeavesQty();
        if (orderQty <= qty) {
            orderMap.erase(order->getOrderId());
            orders.pop_front();
            order->execute(orderQty);
            this->qty -= orderQty;
            return std::make_pair(order, orderQty);
        }
        else {
            order->execute(qty);
            this->qty -= qty;
            return std::make_pair(order, qty);
        }
    }

    virtual ~OrderQueue() = default;
};

#endif //AUCTION_ORDERQUEUE_H
