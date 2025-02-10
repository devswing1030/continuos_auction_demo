//
//

#include "SecuritySession.h"
#include "Report.h"

SecuritySession::SecuritySession() :
    buyOrderBook(true),
    sellOrderBook(false),
    auction(buyOrderBook, sellOrderBook) {
}

void SecuritySession::processAuctionEvent(AuctionEvent &event) {
    currentEvent = &event;
    processNewOrder(event.getOrder());
}

void SecuritySession::processNewOrder(Order *order) {
    long long transactTime = getUtcTime();
    currentEvent->addExecution(ExecutionReport::getAcceptReportJson(order, transactTime));
    auction.processNewOrder(order, this);
}

void SecuritySession::onExecution(Order *order, Order *couterOrder, int qty, int price) {
    long long transactTime = getUtcTime();
    currentEvent->addExecution(ExecutionReport::getTradeReportJson(order, qty, price, transactTime));
    currentEvent->addExecution(ExecutionReport::getTradeReportJson(couterOrder, qty, price, transactTime));
}
