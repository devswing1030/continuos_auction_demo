//
//

#include "Engine.h"
#include "ReportService.h"

void Engine::Init(std::vector<SecurityInfo> &securities, int maxThreads) {
    std::vector<EventHandler<AuctionEvent>*> processors;
    for (int i = 0; i < maxThreads; i++) {
        std::map<tSecurityId, SecuritySession*>* securitySessions = new std::map<tSecurityId, SecuritySession*>();
        int end = (i + 1) * securities.size() / maxThreads;
        if (i == maxThreads - 1) {
            end = securities.size();
        }
        for (int j = i * securities.size() / maxThreads; j < end; j++) {
            SecurityInfo &security = securities[j];
            securitySessions->insert(std::make_pair(security.securityId, new SecuritySession()));
        }
        processors.push_back(new AuctionProcessor(*securitySessions));
    }
    disruptor.Chain(processors);

    processors.clear();
    processors.push_back(&reportService);
    disruptor.Chain(processors);
}

void Engine::Start() {
    disruptor.Start();
    reportService.start();
}

void Engine::Stop(bool force) {
    disruptor.Stop(force);
}

void Engine::SubmitOrder(Order *order) {
    AuctionEvent event;
    event.setOrder(order);
    disruptor.Push(event);
}
