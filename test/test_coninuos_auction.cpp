#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Order.h"
#include "ContinuosAuction.h"

class TestAuctionSession  {
public:
    virtual void onExecution(Order* order, Order* couterOrder, int qty, int price) = 0;
};

class MockAuctionSession : public TestAuctionSession {
public:
    //MOCK_METHOD(void , onExecution, (Order* order, Order* couterOrder, int qty, int price), (override));
    virtual void onExecution(Order* order, Order* couterOrder, int qty, int price) {
        std::cout << "mock onExecution, qty:" << qty << ", price:" << price << std::endl;
    }
};

// 单元测试
TEST(ContinuosAuctionTest, process) {
    OrderBook<Order> buyOrderBook(true);
    OrderBook<Order> sellOrderBook(false);
    ContinuosAuction<Order, TestAuctionSession> auction(buyOrderBook, sellOrderBook);
    MockAuctionSession session;

    Order order1(1, 1001, 1002, 2001, 99, 100, Order::BUY);
    Order order2(2, 1001, 1002, 2001, 98, 200, Order::BUY);
    Order order3(3, 1001, 1002, 2001, 98, 300, Order::BUY);
    Order order4(4, 1001, 1002, 2001, 96, 400, Order::BUY);
    auction.processNewOrder(&order1, &session);
    auction.processNewOrder(&order2, &session);
    auction.processNewOrder(&order3, &session);
    auction.processNewOrder(&order4, &session);


    Order order5(5, 1001, 1002, 2001, 97, 400, Order::SELL);

    //EXPECT_CALL(session, onExecution(&order5, &order1, 100, 96));
    //EXPECT_CALL(session, onExecution(&order5, &order2, 200, 97));
    //EXPECT_CALL(session, onExecution(&order5, &order3, 100, 97));
    auction.processNewOrder(&order5, &session);

    Order order6(6, 1001, 1002, 2001, 95, 800, Order::SELL);
    auction.processNewOrder(&order6, &session);
}
//
