#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>

Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr){}

Book::~Book()
{
    for (auto& [id, order] : orderMap) {
        order->print();
        delete order;
    }
    orderMap.clear();

    for (auto& [limitPrice, limit] : limitMap) {
        delete limit;
    }
    limitMap.clear();
}

void Book::addOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
    orderMap.emplace(orderId, newOrder);

    if (limitMap.find(limitPrice) != limitMap.end())
    {
    limitMap.at(limitPrice)->append(newOrder);
    }
}

void Book::addLimit(int limitPrice)
{
    Limit* newLimit = new Limit(limitPrice);
    limitMap.emplace(limitPrice, newLimit);
}

void Book::printLimit(int limitPrice)
{
    if (limitMap.find(limitPrice) != limitMap.end())
    {
        std::cout << "Limit forwards:" << std::endl;
        limitMap.at(limitPrice)->printForward();
        std::cout << "Limit backwards:" << std::endl;
        limitMap.at(limitPrice)->printBackward();
        limitMap.at(limitPrice)->print();
    }
}

void Book::printOrder(int orderId)
{
    if (orderMap.find(orderId) != orderMap.end())
    {
        orderMap.at(orderId)->print();
    }
}