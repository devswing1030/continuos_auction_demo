//
//

#ifndef AUCTION_SECURITYSESSION_H
#define AUCTION_SECURITYSESSION_H

#include "Order.h"
#include "OrderBook.h"
#include "ContinuosAuction.h"
#include "Report.h"
#include "AuctionEvent.h"


class SecuritySession {
private:
    OrderBook<Order> buyOrderBook;
    OrderBook<Order> sellOrderBook;
    ContinuosAuction<Order, SecuritySession> auction;
    AuctionEvent* currentEvent = nullptr;

    long long getUtcTime() {
        time_t now = time(0);
        tm* gmtm = gmtime(&now);
        long long day = (gmtm->tm_year + 1900) * 10000 + (gmtm->tm_mon + 1) * 100 + gmtm->tm_mday;
        long long time = gmtm->tm_hour * 10000000 + gmtm->tm_min * 100000 + gmtm->tm_sec * 1000 + (long)(now % 1000);
        return day * 1000000000 + time;
    }
public:
    SecuritySession();
    void processAuctionEvent(AuctionEvent& event);
    void processNewOrder(Order* order);
    void onExecution(Order* order, Order* couterOrder, int qty, int price);
};


#endif //AUCTION_SECURITYSESSION_H
