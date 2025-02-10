//
//

#ifndef AUCTION_REPORT_H
#define AUCTION_REPORT_H

#include <string>
#include <Order.h>

class ExecutionReport{
    static const std::string MESSAGE_TYPE;
private:
    static std::string getCommonJson(Order* order, long long transactTime) {
        std::string json = "{";
        json += "\"type\": \"" + MESSAGE_TYPE + "\","; // FIX type
        json += "\"securityId\": \"" + std::string(order->getSecurityId().begin(), order->getSecurityId().end()) + "\",";
        json += "\"orderId\": \"" + std::to_string(order->getOrderId()) + "\",";
        json += "\"accountId\": \"" + std::to_string(order->getAccountId()) + "\",";
        json += "\"side\": \"" + std::to_string(order->getSide()) + "\",";
        json += "\"price\": " + std::to_string(order->getPrice()) + ",";
        json += "\"orderQty\": " + std::to_string(order->getQty()) + ",";
        json += "\"transactTime\": " + std::to_string(transactTime) + ",";
        return json;
    }
public:


    static std::string getAcceptReportJson(Order* order, long long transactTime) {
        std::string execType = "0"; // FIX '0' = 'New'
        std::string ordStatus = "0"; // FIX '0' = 'New'

        std::string json = getCommonJson(order, transactTime);
        json += "\"execType\": \"" +  execType + "\",";
        json += "\"ordStatus\": \"" + ordStatus + "\"";
        json += "}";
        return json;
    }

    static std::string getRejectReportJson(Order* order, long long transactTime, int rejectReason) {
        std::string execType = "8"; // FIX '8' = 'Rejected'
        std::string ordStatus = "8"; // FIX '8' = 'Rejected'

        std::string json = getCommonJson(order, transactTime);
        json += "\"execType\": \"" + execType + "\",";
        json += "\"ordStatus\": \"" + ordStatus + "\",";
        json += "\"ordRejReason\": " + std::to_string(rejectReason);
        json += "}";
        return json;
    }

    static std::string getCancelReportJson(Order* order, long long transactTime) {
        std::string execType = "4"; // FIX '4' = 'Canceled'
        std::string ordStatus = "4"; // FIX '4' = 'Canceled'

        std::string json = getCommonJson(order, transactTime);
        json += "\"execType\": \"" + execType + "\",";
        json += "\"ordStatus\": \"" + ordStatus + "\"";
        json += "}";
        return json;
    }

    static std::string getTradeReportJson(Order* order, int qty, int price, long long transactTime) {
        std::string execType = "F"; // FIX 'F' = 'Trade'
        std::string ordStatus = "2"; // FIX '2' = 'Filled'
        if (order->getLeavesQty() > 0) {
            ordStatus = "1"; // FIX '1' = 'Partially filled'
        }

        std::string json = getCommonJson(order, transactTime);
        json += "\"lastPx\": " + std::to_string(price) + ",";
        json += "\"lastQty\": " + std::to_string(qty) + ",";
        json += "\"leavesQty\": " + std::to_string(order->getLeavesQty()) + ",";
        json += "\"execType\": \"" + execType + "\",";
        json += "\"ordStatus\": \"" + ordStatus + "\",";
        json += "}";
        return json;
    }
};

#endif //AUCTION_REPORT_H
