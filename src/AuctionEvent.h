//
//

#ifndef AUCTION_AUCTIONEVENT_H
#define AUCTION_AUCTIONEVENT_H

#include <string>
#include <vector>
#include "Order.h"

class AuctionEvent {
private:
    Order* order = nullptr;
    std::vector<std::string> executions;

public:
    void setOrder(Order* order) {
        this->order = order;
    }
    Order* getOrder() {
        return order;
    }
    void addExecution(std::string reportJson) {
        executions.push_back(reportJson);
    }
    std::vector<std::string>& getExecutions() {
        return executions;
    }
    void clear() {
        executions.clear();
        order = nullptr;
    }
};

#endif //AUCTION_AUCTIONEVENT_H
