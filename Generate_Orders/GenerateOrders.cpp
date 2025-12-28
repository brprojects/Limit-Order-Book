#include "GenerateOrders.hpp"
#include "../Limit_Order_Book/Book.hpp"
#include "../Limit_Order_Book/Limit.hpp"
#include "../Limit_Order_Book/Order.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <functional>

GenerateOrders::GenerateOrders(Book* _book) : book(_book), gen(rd()) {}

void GenerateOrders::market()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::uniform_int_distribution<> buyOrSellDist(0, 1);

    int shares = sharesDist(gen);
    bool buyOrSell = buyOrSellDist(gen);

    file << "Market " << orderId << " " << buyOrSell << " " << shares << std::endl;
    book->marketOrder(orderId, buyOrSell, shares);
    orderId++;
}

void GenerateOrders::addLimit()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::normal_distribution<> limitPriceDist(300, 50);
    std::uniform_int_distribution<> buyOrSellDist(0, 1);

    int shares = sharesDist(gen);
    bool buyOrSell = buyOrSellDist(gen);
    int limitPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            limitPrice = static_cast<int>(limitPriceDist(gen));
        } while (bestAsk != 0 && limitPrice >= bestAsk);
    } else {
        do {
            limitPrice = static_cast<int>(limitPriceDist(gen));
        } while (bestBid != 0 && limitPrice <= bestBid);
    }

    file << "AddLimit " << orderId << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    book->addLimitOrder(orderId, buyOrSell, shares, limitPrice);
    orderId++;
}

void GenerateOrders::cancelLimit()
{
    Order* order = book->getRandomOrder(0, gen);

    if (order == nullptr) {
        addLimit();
        return;
    }
    int orderId = order->getOrderId();
    file << "CancelLimit " << orderId << std::endl;
    book->cancelLimitOrder(orderId);
}

void GenerateOrders::modifyLimit()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    int basePrice = book->getBestBidPrice();
    if (basePrice == 0) basePrice = 10000;
    std::normal_distribution<> limitPriceDist(basePrice, 50);

    int shares = sharesDist(gen);

    Order* order = book->getRandomOrder(0, gen);

    if (order == nullptr) {
        addLimit();
        return;
    }
    int orderId = order->getOrderId();
    bool buyOrSell = order->getBuyOrSell();
    int limitPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            limitPrice = static_cast<int>(limitPriceDist(gen));
        } while (bestAsk != 0 && limitPrice >= bestAsk);
    } else {
        do {
            limitPrice = static_cast<int>(limitPriceDist(gen));
        } while (bestBid != 0 && limitPrice <= bestBid);
    }
    file << "ModifyLimit " << orderId << " " << shares << " " << limitPrice << std::endl;
    book->modifyLimitOrder(orderId, shares, limitPrice);
}

void GenerateOrders::addLimitMarket()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::uniform_int_distribution<> buyOrSellDist(0, 1);

    int shares = sharesDist(gen);
    bool buyOrSell = buyOrSellDist(gen);
    int limitPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        limitPrice = (bestAsk == 0 ? 10000 : bestAsk) + 1;
    } else {
        limitPrice = (bestBid == 0 ? 9900 : bestBid) - 1;
    }
    
    file << "AddMarketLimit " << orderId << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    book->addLimitOrder(orderId, buyOrSell, shares, limitPrice);
    orderId++;
}

void GenerateOrders::addStop()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    int basePrice = book->getBestBidPrice();
    if (basePrice == 0) basePrice = 10000;
    std::normal_distribution<> stopPriceDist(basePrice, 50);
    std::uniform_int_distribution<> buyOrSellDist(0, 1);

    int shares = sharesDist(gen);
    bool buyOrSell = buyOrSellDist(gen);
    int stopPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestAsk != 0 && stopPrice <= bestAsk);
    } else {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestBid != 0 && stopPrice >= bestBid);
    }

    file << "AddStop " << orderId << " " << buyOrSell << " " << shares << " " << stopPrice << std::endl;
    book->addStopOrder(orderId, buyOrSell, shares, stopPrice);
    orderId++;
}

void GenerateOrders::cancelStop()
{
    Order* order = book->getRandomOrder(1, gen);

    if (order == nullptr) {
        addStop();
        return;
    }
    int orderId = order->getOrderId();
    file << "CancelStop " << orderId << std::endl;
    book->cancelStopOrder(orderId);
}

void GenerateOrders::modifyStop()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    int basePrice = book->getBestBidPrice();
    if (basePrice == 0) basePrice = 10000;
    std::normal_distribution<> stopPriceDist(basePrice, 50);

    int shares = sharesDist(gen);

    Order* order = book->getRandomOrder(1, gen);

    if (order == nullptr) {
        addStop();
        return;
    }
    int orderId = order->getOrderId();
    bool buyOrSell = order->getBuyOrSell();
    int stopPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestAsk != 0 && stopPrice <= bestAsk);
    } else {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestBid != 0 && stopPrice >= bestBid);
    }
    file << "ModifyStop " << orderId << " " << shares << " " << stopPrice << std::endl;
    book->modifyStopOrder(orderId, shares, stopPrice);
}

void GenerateOrders::addStopLimit()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    int basePrice = book->getBestBidPrice();
    if (basePrice == 0) basePrice = 10000;
    std::normal_distribution<> stopPriceDist(basePrice, 50);
    std::uniform_int_distribution<> limitPriceDist(1, 5);
    std::uniform_int_distribution<> buyOrSellDist(0, 1);

    int shares = sharesDist(gen);
    bool buyOrSell = buyOrSellDist(gen);
    int stopPrice;
    int limitPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestAsk != 0 && stopPrice <= bestAsk);
        limitPrice = stopPrice + limitPriceDist(gen);
    } else {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestBid != 0 && stopPrice >= bestBid);
        limitPrice = stopPrice - limitPriceDist(gen);
    }

    file << "AddStopLimit " << orderId << " " << buyOrSell << " " << shares << " " << limitPrice << " " << stopPrice << std::endl;
    book->addStopLimitOrder(orderId, buyOrSell, shares, limitPrice, stopPrice);
    orderId++;
}

void GenerateOrders::cancelStopLimit()
{
    Order* order = book->getRandomOrder(2, gen);

    if (order == nullptr) {
        addStopLimit();
        return;
    }
    int orderId = order->getOrderId();
    file << "CancelStopLimit " << orderId << std::endl;
    book->cancelStopLimitOrder(orderId);
}

void GenerateOrders::modifyStopLimit()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    int basePrice = book->getBestBidPrice();
    if (basePrice == 0) basePrice = 10000;
    std::normal_distribution<> stopPriceDist(basePrice, 50);
    std::uniform_int_distribution<> limitPriceDist(1, 5);

    int shares = sharesDist(gen);

    Order* order = book->getRandomOrder(2, gen);

    if (order == nullptr) {
        addStopLimit();
        return;
    }
    int orderId = order->getOrderId();
    bool buyOrSell = order->getBuyOrSell();
    int stopPrice;
    int limitPrice;

    int bestAsk = book->getBestAskPrice();
    int bestBid = book->getBestBidPrice();

    if (buyOrSell) {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestAsk != 0 && stopPrice <= bestAsk);
        limitPrice = stopPrice + limitPriceDist(gen);
    } else {
        do {
            stopPrice = static_cast<int>(stopPriceDist(gen));
        } while (bestBid != 0 && stopPrice >= bestBid);
        limitPrice = stopPrice - limitPriceDist(gen);
    }
    file << "ModifyStopLimit " << orderId << " " << shares << " " << limitPrice << " " << stopPrice << std::endl;
    book->modifyStopLimitOrder(orderId, shares, limitPrice, stopPrice);
}

void GenerateOrders::createOrders(int numberOfOrders)
{
    file.open("./Generate_Orders/orders.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    std::uniform_real_distribution<> dis(0.0, 1.0);

    std::vector<double> probabilities = {0.025, 0, 0.195, 0.295, 0.025, 0, 0.12, 0.12, 0, 0.12, 0.12};
    std::vector<std::function<void()>> actions = {
        std::bind(&GenerateOrders::market, this),
        std::bind(&GenerateOrders::addLimit, this),
        std::bind(&GenerateOrders::cancelLimit, this),
        std::bind(&GenerateOrders::modifyLimit, this),
        std::bind(&GenerateOrders::addLimitMarket, this),
        std::bind(&GenerateOrders::addStop, this),
        std::bind(&GenerateOrders::cancelStop, this),
        std::bind(&GenerateOrders::modifyStop, this),
        std::bind(&GenerateOrders::addStopLimit, this),
        std::bind(&GenerateOrders::cancelStopLimit, this),
        std::bind(&GenerateOrders::modifyStopLimit, this),
    };

    std::partial_sum(probabilities.begin(), probabilities.end(), probabilities.begin());

    for (size_t i = 1; i <= numberOfOrders; ++i) {
        double randNum = dis(gen);
        auto it = std::lower_bound(probabilities.begin(), probabilities.end(), randNum);
        int selectedAction = std::distance(probabilities.begin(), it);

        if (selectedAction < actions.size()) {
            actions[selectedAction]();
        } else {
            std::cerr << "Error: No action selected!" << std::endl;
        }
    }
    file.close();
    std::cout << "Orders written to orders.txt successfully!" << std::endl;
}

void GenerateOrders::createInitialOrders(int numberOfOrders, int centreOfBook)
{
    std::ofstream file("./Generate_Orders/initialOrders.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::normal_distribution<> limitPriceDist(centreOfBook, 50);

    for (int order = 1; order <= numberOfOrders; ++order) {
        int shares = sharesDist(gen);
        int limitPrice = static_cast<int>(limitPriceDist(gen));
        bool buyOrSell = limitPrice < centreOfBook;

        file << "AddLimit " << order << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    }

    std::uniform_int_distribution<> stopLimitPriceDist(1, 5);
    std::uniform_int_distribution<> stopOrStopLimitDist(0, 1);

    for (int order = numberOfOrders + 1; order <= static_cast<int>(numberOfOrders * 1.1); ++order) {
        int shares = sharesDist(gen);
        int stopPrice = static_cast<int>(limitPriceDist(gen));
        bool buyOrSell = stopPrice > centreOfBook;
        bool stopOrStopLimit = stopOrStopLimitDist(gen);

        if (stopOrStopLimit) {
            file << "AddStop " << order << " " << buyOrSell << " " << shares << " " << stopPrice << std::endl;
        } else {
            int limitPrice = buyOrSell ? stopPrice + stopLimitPriceDist(gen) : stopPrice - stopLimitPriceDist(gen);
            file << "AddStopLimit " << order << " " << buyOrSell << " " << shares << " " << limitPrice << " " << stopPrice << std::endl;
        }
    }

    file.close();
    std::cout << "Orders written to initialOrders.txt successfully!" << std::endl;
}