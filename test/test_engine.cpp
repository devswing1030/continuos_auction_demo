//
//

#include <gtest/gtest.h>
#include "Engine.h"
#include <random>

TEST(Engine, performance) {
    Engine engine;
    std::vector<SecurityInfo> securities;
    for (int i = 0; i < 100; i++) {
        SecurityInfo security;
        std::string id = std::to_string(i);
        security.securityId.fill(' ');
        std::copy(id.begin(), id.end(), security.securityId.begin());
        securities.push_back(security);
    }
    engine.Init(securities, 1);

    // Function to get a random number between min and max
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> security_distr(0, securities.size() - 1); // Define the range
    std::uniform_int_distribution<> price_distr(1, 1000); // Define the range
    std::uniform_int_distribution<> qty_distr(1, 1000); // Define the range
    std::uniform_int_distribution<> side_distr(1, 2); // Define the range


    engine.Start();

    // calculate time elapse
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; i++) {
        Order* order = new Order(i, 1, securities[security_distr(gen)].securityId, price_distr(gen), qty_distr(gen), side_distr(gen));
        engine.SubmitOrder(order);
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    engine.Stop(false);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

}

