//
//

#ifndef AUCTION_REPORTSERVICE_H
#define AUCTION_REPORTSERVICE_H

#include <string>
#include <deque>
#include <mutex>
#include "Disruptor.h"
#include "AuctionEvent.h"
#include <httplib.h>

const auto html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>SSE demo</title>
</head>
<body>
<script>
const ev1 = new EventSource("report");
ev1.onmessage = function(e) {
  console.log('ev1', e.data);
}
</script>
</body>
</html>
)";

class ReportService : public EventHandler<AuctionEvent> {
    std::deque<std::string> reports;
    std::mutex mutex;
    httplib::Server server;
    std::condition_variable cv;
    size_t lastReport = 0;
    std::thread worker;
public:
    void start() {
        worker = std::thread([this] { createServer(); });
    }

private:
    void createServer() {

        server.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
            res.set_content(html, "text/html");
        });

        server.Get("/report", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_chunked_content_provider("text/event-stream",
                                             [&, this](size_t /*offset*/, httplib::DataSink &sink) {
                                                 std::stringstream ss;
                                                 ss << "data: " << getReport(lastReport) << "\n\n";
                                                 std::string report = ss.str();
                                                 sink.write(report.data(), report.size());
                                                 lastReport++;
                                                 return true;
                                             });
        });
        server.listen("localhost", 8089);
    }

    void onEvent(AuctionEvent& event, int sequence) override {
        std::unique_lock<std::mutex> lock(mutex);
        for (auto& report : event.getExecutions()) {
            reports.push_back(report);
        }
        cv.notify_all();
    }


    const std::string& getReport(size_t idx) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this, idx] { return reports.size() > idx; });
        return reports.at(idx);
    }


};


#endif //AUCTION_REPORTSERVICE_H
