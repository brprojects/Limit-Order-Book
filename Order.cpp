#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>

Order::Order(int _idNumber, bool _buyOrSell, int _shares, int _limit)
    : idNumber(_idNumber), buyOrSell(_buyOrSell), shares(_shares), limit(_limit), 
    nextOrder(nullptr), prevOrder(nullptr), parentLimit(nullptr) {}

Order::~Order()
{
    parentLimit->totalVolume -= shares;
    parentLimit->size -= 1;
}

void Order::cancel()
{
    if (prevOrder == nullptr)
    {
        parentLimit->headOrder = nextOrder;
    } else
    {
        prevOrder->nextOrder = nextOrder;
    }
    if (nextOrder == nullptr)
    {
        parentLimit->tailOrder = prevOrder;
    } else
    {
    nextOrder->prevOrder = prevOrder;
    }
}

void Order::print() const
{
    std::cout << "Order ID: " << idNumber 
    << ", Order Type: " << (buyOrSell == 1 ? "buy" : "sell") 
    << ", Order Size: " << shares
    << ", Order Limit: " << limit 
    << std::endl;
    if (parentLimit != nullptr) {
        parentLimit->print();
    } else {
        std::cout << "No parent limit assigned" << std::endl;
    }
}
