//
// Created by 曹兆勇 on 2024/10/6.
//

#ifndef AUCTION_ORDER_H
#define AUCTION_ORDER_H

class Order {
    // 订单编号
    int orderId;
    // 买家编号
    int buyerId;
    // 卖家编号
    int sellerId;
    // 商品编号
    int securityId;
    // 价格
    int price;
    // 数量
    int qty;
    // 剩余数量
    int leavesQty;
    // 类型
    short side;

public:
    static const short BUY = 1;
    static const short SELL = 2;
    // 构造函数
    Order(int orderId, int buyerId, int sellerId, int securityId, int price, int qty, short side) :
            orderId(orderId), buyerId(buyerId), sellerId(sellerId), securityId(securityId), price(price),
            qty(qty), side(side) {
        leavesQty = qty;
    }

    // 获取订单编号
    int getOrderId() const {
        return orderId;
    }

    // 获取买家编号
    int getBuyerId() const {
        return buyerId;
    }

    // 获取卖家编号
    int getSellerId() const {
        return sellerId;
    }

    // 获取商品编号
    int getSecurityId() const {
        return securityId;
    }

    // 获取价格
    int getPrice() const {
        return price;
    }

    // 获取数量
    int getQty() const {
        return qty;
    }

    short getSide() const {
        return side;
    }

    // 获取剩余数量
    int getLeavesQty() const {
        return leavesQty;
    }

    void execute(int qty) {
        leavesQty -= qty;
    }

    // 设置订单编号
    void setOrderId(int orderId) {
        Order::orderId = orderId;
    }

    // 设置买家编号
    void setBuyerId(int buyerId) {
        Order::buyerId = buyerId;
    }

    // 设置卖家编号
    void setSellerId(int sellerId) {
        Order::sellerId = sellerId;
    }

};

#endif //AUCTION_ORDER_H
