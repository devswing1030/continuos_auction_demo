//
// Created by 曹兆勇 on 2024/10/6.
#include <gtest/gtest.h>
#include "Order.h"
#include "OrderQueue.h"

// 单元测试
TEST(OrderQueueTest, insert) {
    OrderQueue<Order> orderQueue(97);
    Order order1(1, 1001, 1002, 2001, 97, 100, Order::BUY);
    Order order2(2, 1001, 1002, 2001, 97, 300, Order::BUY);
    orderQueue.insert(order1);
    orderQueue.insert(order2);
    EXPECT_EQ(orderQueue.getQty(), 400);
}

TEST(OrderQueueTest, cancel) {
    OrderQueue<Order> orderQueue(97);
    Order order1(1, 1001, 1002, 2001, 97, 100, Order::BUY);
    Order order2(2, 1001, 1002, 2001, 97, 300, Order::BUY);
    Order order3(3, 1001, 1002, 2001, 97, 500, Order::BUY);
    Order order4(4, 1001, 1002, 2001, 97, 700, Order::BUY);
    orderQueue.insert(order1);
    orderQueue.insert(order2);
    orderQueue.insert(order3);
    orderQueue.insert(order4);
    orderQueue.cancel(order2.getOrderId());
    EXPECT_EQ(orderQueue.getQty(), 1300);
    orderQueue.cancel(order3.getOrderId());
    EXPECT_EQ(orderQueue.getQty(), 800);
    orderQueue.cancel(order1.getOrderId());
    EXPECT_EQ(orderQueue.getQty(), 700);
    orderQueue.cancel(order4.getOrderId());
    EXPECT_EQ(orderQueue.getQty(), 0);

}

TEST(OrderQueueTest, iterator) {
    OrderQueue<Order> orderQueue(97);
    Order order1(1, 1001, 1002, 2001, 97, 100, Order::BUY);
    Order order2(2, 1001, 1002, 2001, 97, 200, Order::BUY);
    Order order3(3, 1001, 1002, 2001, 97, 300, Order::BUY);
    Order order4(4, 1001, 1002, 2001, 97, 400, Order::BUY);
    orderQueue.insert(order1);
    orderQueue.insert(order2);
    orderQueue.insert(order3);
    orderQueue.insert(order4);
    auto it = orderQueue.begin();
    EXPECT_EQ((*it)->getOrderId(), 1);
    EXPECT_EQ((*it)->getQty(), 100);
    ++it;
    EXPECT_EQ((*it)->getOrderId(), 2);
    EXPECT_EQ((*it)->getQty(), 200);
    ++it;
    EXPECT_EQ((*it)->getOrderId(), 3);
    EXPECT_EQ((*it)->getQty(), 300);
    ++it;
    EXPECT_EQ((*it)->getOrderId(), 4);
    EXPECT_EQ((*it)->getQty(), 400);
    ++it;
    EXPECT_EQ(it, orderQueue.end());

    orderQueue.cancel(order2.getOrderId());
    auto it2 = orderQueue.begin();
    EXPECT_EQ((*it2)->getOrderId(), 1);
    EXPECT_EQ((*it2)->getQty(), 100);
    ++it2;
    EXPECT_EQ((*it2)->getOrderId(), 3);
    EXPECT_EQ((*it2)->getQty(), 300);
    ++it2;
    EXPECT_EQ((*it2)->getOrderId(), 4);
    EXPECT_EQ((*it2)->getQty(), 400);
    ++it2;
}

TEST(OrderQueueTest, consume) {
    OrderQueue<Order> orderQueue(97);
    Order order1(1, 1001, 1002, 2001, 97, 100, Order::BUY);
    Order order2(2, 1001, 1002, 2001, 97, 200, Order::BUY);
    Order order3(3, 1001, 1002, 2001, 97, 300, Order::BUY);
    Order order4(4, 1001, 1002, 2001, 97, 400, Order::BUY);
    orderQueue.insert(order1);
    orderQueue.insert(order2);
    orderQueue.insert(order3);
    orderQueue.insert(order4);

    auto result = orderQueue.consumeOneOrder(30);
    EXPECT_EQ((*result).first->getOrderId(), 1);
    EXPECT_EQ((*result).first->getLeavesQty(), 70);
    EXPECT_EQ(orderQueue.getQty(), 970);

    result = orderQueue.consumeOneOrder(70);
    EXPECT_EQ((*result).first->getOrderId(), 1);
    EXPECT_EQ((*result).first->getLeavesQty(), 0);
    EXPECT_EQ(orderQueue.getQty(), 900);

    result = orderQueue.consumeOneOrder(300);
    EXPECT_EQ((*result).first->getOrderId(), 2);
    EXPECT_EQ((*result).first->getLeavesQty(), 0);
    EXPECT_EQ(orderQueue.getQty(), 700);

    result = orderQueue.consumeOneOrder(300);
    EXPECT_EQ((*result).first->getOrderId(), 3);
    EXPECT_EQ((*result).first->getLeavesQty(), 0);
    EXPECT_EQ(orderQueue.getQty(), 400);

    result = orderQueue.consumeOneOrder(500);
    EXPECT_EQ((*result).first->getOrderId(), 4);
    EXPECT_EQ((*result).first->getLeavesQty(), 0);
    EXPECT_EQ(orderQueue.getQty(), 0);

    result = orderQueue.consumeOneOrder(500);
    EXPECT_EQ(result, std::nullopt);
}

