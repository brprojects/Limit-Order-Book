#include "OrderPipeline.hpp"
#include "Book.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>

OrderPipeline::OrderPipeline(Book* book) : book(book) {
    orderFunctions = {
        {"Market", &OrderPipeline::processMarketOrder},
        {"Add", &OrderPipeline::processAddOrder},
        {"Cancel", &OrderPipeline::processCancelOrder},
        {"Modify", &OrderPipeline::processModifyOrder}
    };
}

void OrderPipeline::processOrdersFromFile(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string orderType;
        iss >> orderType;

        auto it = orderFunctions.find(orderType);
            if (it != orderFunctions.end()) {
                (this->*(it->second))(iss);
            } else {
                std::cerr << "Unknown order type: " << orderType << std::endl;
            }
    }
    file.close();
}

void OrderPipeline::processMarketOrder(std::istringstream& iss) {
    int orderId, shares;
    bool buyOrSell;
    iss >> orderId >> buyOrSell >> shares;
    book->marketOrder(orderId, buyOrSell, shares);
}

void OrderPipeline::processAddOrder(std::istringstream& iss) {
    int orderId, shares, limitPrice;
    bool buyOrSell;
    iss >> orderId >> buyOrSell >> shares >> limitPrice;
    book->addOrder(orderId, buyOrSell, shares, limitPrice);
}

void OrderPipeline::processCancelOrder(std::istringstream& iss) {
    int orderId;
    iss >> orderId;
    book->cancelOrder(orderId);
}

void OrderPipeline::processModifyOrder(std::istringstream& iss) {
    int orderId, newShares, newLimit;
    iss >> orderId >> newShares >> newLimit;
    book->modifyOrder(orderId, newShares, newLimit);
}
