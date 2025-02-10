
#ifndef AUCTION_ORDER_H
#define AUCTION_ORDER_H

#include "type.h"

class Order {
    tSecurityId securityId;
    int orderId;
    int accountId;
    int price;
    int qty;
    int leavesQty;
    short side;

public:
    static const short BUY = 1;
    static const short SELL = 2;
    Order(int orderId, int accountId, tSecurityId securityId, int price, int qty, short side) :
            orderId(orderId), accountId(accountId), securityId(securityId), price(price),
            qty(qty), side(side) {
        leavesQty = qty;
    }

    int getOrderId() const {
        return orderId;
    }

    int getAccountId() const {
        return accountId;
    }

    const tSecurityId &getSecurityId() const {
        return securityId;
    }

    int getPrice() const {
        return price;
    }

    int getQty() const {
        return qty;
    }

    short getSide() const {
        return side;
    }

    int getLeavesQty() const {
        return leavesQty;
    }

    void execute(int qty) {
        leavesQty -= qty;
    }
};

#endif //AUCTION_ORDER_H
