#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"

// 单元测试
TEST(OrderBookTest, add) {
    const std::string tmp = "2001";
    tSecurityId securityId;
    std::copy(tmp.begin(), tmp.end(), securityId.begin());

    Order order1(1, 1001, securityId, 97, 100, Order::BUY);
    Order order2(2, 1001, securityId, 98, 200, Order::BUY);
    Order order3(3, 1001, securityId, 97, 300, Order::BUY);
    Order order4(4, 1001, securityId, 96, 400, Order::BUY);


    OrderBook<Order> orderBook(false);
    orderBook.addOrder(&order1);
    orderBook.addOrder(&order2);
    orderBook.addOrder(&order3);
    orderBook.addOrder(&order4);

    auto it = orderBook.begin();
    EXPECT_EQ(it->second.getQty(), 400);
    EXPECT_EQ(it->second.getPrice(), 96);
    ++it;

    EXPECT_EQ(it->second.getQty(), 400);
    EXPECT_EQ(it->second.getPrice(), 97);

    ++it;
    EXPECT_EQ(it->second.getQty(), 200);
    EXPECT_EQ(it->second.getPrice(), 98);

    ++it;
    EXPECT_EQ(it, orderBook.end());

    OrderBook<Order> orderBook1(true);
    orderBook1.addOrder(&order1);
    orderBook1.addOrder(&order2);
    orderBook1.addOrder(&order3);
    orderBook1.addOrder(&order4);

    auto it1 = orderBook1.begin();
    EXPECT_EQ(it1->second.getQty(), 200);
    EXPECT_EQ(it1->second.getPrice(), 98);
    ++it1;

    EXPECT_EQ(it1->second.getQty(), 400);
    EXPECT_EQ(it1->second.getPrice(), 97);

    ++it1;
    EXPECT_EQ(it1->second.getQty(), 400);
    EXPECT_EQ(it1->second.getPrice(), 96);

}

TEST(OrderBookTest, cancel) {
    const std::string tmp = "2001";
    tSecurityId securityId;
    std::copy(tmp.begin(), tmp.end(), securityId.begin());

    Order order1(1, 1001, securityId, 97, 100, Order::BUY);
    Order order2(2, 1001, securityId, 98, 200, Order::BUY);
    Order order3(3, 1001, securityId, 97, 300, Order::BUY);
    Order order4(4, 1001, securityId, 96, 400, Order::BUY);

    OrderBook<Order> orderBook(false);
    orderBook.addOrder(&order1);
    orderBook.addOrder(&order2);
    orderBook.addOrder(&order3);
    orderBook.addOrder(&order4);

    orderBook.cancelOrder(&order1);
    orderBook.cancelOrder(&order2);

    auto it = orderBook.begin();
    EXPECT_EQ(it->second.getQty(), 400);
    EXPECT_EQ(it->second.getPrice(), 96);
    ++it;

    EXPECT_EQ(it->second.getQty(), 300);
    EXPECT_EQ(it->second.getPrice(), 97);

    ++it;
    EXPECT_EQ(it, orderBook.end());

}
//
