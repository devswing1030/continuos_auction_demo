//
//

#ifndef AUCTION_ENGINE_H
#define AUCTION_ENGINE_H

#include <vector>
#include "type.h"
#include "Disruptor.h"
#include "AuctionProcessor.h"
#include "ReportService.h"

class SecurityInfo {
public:
    tSecurityId securityId;
    int price;
    int qty;
};

class Engine {
private:
    Disruptor<AuctionEvent> disruptor;
    ReportService reportService;
public:
    Engine() : disruptor(1024) {
    }
    void Init(std::vector<SecurityInfo>& securities, int maxThreads);
    void Start();
    void Stop(bool force);
    void SubmitOrder(Order* order);

};


#endif //AUCTION_ENGINE_H
