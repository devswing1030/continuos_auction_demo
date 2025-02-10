//
//

#ifndef AUCTION_AUCTIONPROCESSOR_H
#define AUCTION_AUCTIONPROCESSOR_H

#include <map>
#include <time.h>
#include "type.h"
#include "SecuritySession.h"
#include "Disruptor.h"


class AuctionProcessor : public EventHandler<AuctionEvent>  {
private:
    std::map<tSecurityId, SecuritySession*>& securitySessions;
public:
    AuctionProcessor(std::map<tSecurityId, SecuritySession*>& securitySessions)
        : securitySessions(securitySessions) {
    }
    void onEvent(AuctionEvent& event, int sequence) override {
        Order* order = event.getOrder();
        auto it = securitySessions.find(order->getSecurityId());
        if (it != securitySessions.end()) {
            it->second->processAuctionEvent(event);
        }
    }
};


#endif //AUCTION_AUCTIONPROCESSOR_H
