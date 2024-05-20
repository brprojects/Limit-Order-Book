#include "GenerateOrders.hpp"
#include "Book.hpp"
#include "Limit.hpp"
#include "Order.hpp"

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
    Order* order = book->getRandomOrder(gen);

    if (order != nullptr)
    {
        file << "Market " << orderId << " " << buyOrSell << " " << shares << std::endl;
        book->marketOrder(orderId, buyOrSell, shares);
        orderId ++;
    } else {
        add();
    }
}

void GenerateOrders::modify()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::normal_distribution<> limitPriceDist(book->getHighestBuy()->getLimitPrice(), 50);

    int shares = sharesDist(gen);

    Order* order = book->getRandomOrder(gen);

    if (order != nullptr)
    {
        int orderId = order->getOrderId();
        bool buyOrSell = order->getBuyOrSell();

        int limitPrice;
        if (buyOrSell)
        {
            do {
                limitPrice = limitPriceDist(gen);
            } while (limitPrice >= book->getLowestSell()->getLimitPrice());  
        } else {
            do {
                limitPrice = limitPriceDist(gen);
            } while (limitPrice <= book->getHighestBuy()->getLimitPrice());  
        }
        
        file << "Modify " << orderId << " " << shares << " " << limitPrice << std::endl;
        book->modifyLimitOrder(orderId, shares, limitPrice);
    } else {
        add();
    }
}

void GenerateOrders::add()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::normal_distribution<> limitPriceDist(300, 50);

    int shares = sharesDist(gen);
    int limitPrice = limitPriceDist(gen);
    bool buyOrSell = limitPrice < 300;

    file << "Add " << orderId << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    book->addLimitOrder(orderId, buyOrSell, shares, limitPrice);
    orderId ++;
}

void GenerateOrders::cancel()
{
    Order* order = book->getRandomOrder(gen);

    if (order != nullptr)
    {
        int orderId = order->getOrderId();

        file << "Cancel " << orderId << std::endl;
        book->cancelLimitOrder(orderId);
    } else {
        add();
    }
    
}

void GenerateOrders::limitMarket()
{
    std::uniform_int_distribution<> sharesDist(1, 1000);
    std::normal_distribution<> limitPriceDist(book->getHighestBuy()->getLimitPrice(), 50);

    int shares = sharesDist(gen);
    int limitPrice = limitPriceDist(gen);
    bool buyOrSell = limitPrice > book->getHighestBuy()->getLimitPrice();

    file << "Add " << orderId << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    book->addLimitOrder(orderId, buyOrSell, shares, limitPrice);
    orderId ++;
}

void GenerateOrders::createOrders(int numberOfOrders)
{
    // Open a file named "orders.txt" for writing
    file.open("../orders.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Define the probabilities and actions
    std::vector<double> probabilities = {0.15, 0.2, 0.4, 0.2, 0.05};
    std::vector<std::function<void()>> actions = {
        std::bind(&GenerateOrders::market, this),
        std::bind(&GenerateOrders::modify, this),
        std::bind(&GenerateOrders::add, this),
        std::bind(&GenerateOrders::cancel, this),
        std::bind(&GenerateOrders::limitMarket, this)
    };

    // Calculate the cumulative probabilities
    std::partial_sum(probabilities.begin(), probabilities.end(), probabilities.begin());

    for (size_t i = 1; i < numberOfOrders + 1; i++)
    {
        // Generate a random number between 0 and 1 
        double randNum = dis(gen);

        // Use std::lower_bound to find the first element greater than randNum
        auto it = std::lower_bound(probabilities.begin(), probabilities.end(), randNum);

        // Calculate the index of the selected action
        int selectedAction = std::distance(probabilities.begin(), it);

        // Perform the selected action
        if (selectedAction < probabilities.size()) {
            actions[selectedAction]();
        } else {
            std::cerr << "Error: No action selected!" << std::endl;
        }
    }
    file.close();
    std::cout << "Orders written to Orders.txt successfully!" << std::endl;
}

void GenerateOrders::createInitialOrders(int numberOfOrders, int centreOfBook)
{
    // Open a file named "initialOrders.txt" for writing
    std::ofstream file("../initialOrders.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    // Define the range for shares and limitPrice
    std::uniform_int_distribution<> sharesDist(1, 1000);

    // Define the normal distribution for limitPrice (Mean, SD)
    std::normal_distribution<> limitPriceDist(centreOfBook, 50);

    // Writing orders
    for (int order = 1; order <= numberOfOrders; ++order) {
        int shares = sharesDist(gen);
        int limitPrice = limitPriceDist(gen);
        bool buyOrSell = limitPrice < centreOfBook;

        // Writing order to the file
        file << "Add " << order << " " << buyOrSell << " " << shares << " " << limitPrice << std::endl;
    }

    // Close the file
    file.close();

    std::cout << "Orders written to initialOrders.txt successfully!" << std::endl;
}