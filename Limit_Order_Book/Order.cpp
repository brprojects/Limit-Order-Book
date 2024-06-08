#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>

Order::Order(int _idNumber, bool _buyOrSell, int _shares, int _limit)
    : idNumber(_idNumber), buyOrSell(_buyOrSell), shares(_shares), limit(_limit), 
    nextOrder(nullptr), prevOrder(nullptr), parentLimit(nullptr) {}

int Order::getShares() const
{
    return shares;
}

int Order::getOrderId() const
{
    return idNumber;
}

bool Order::getBuyOrSell() const
{
    return buyOrSell;
}

int Order::getLimit() const
{
    return limit;
}

Limit* Order::getParentLimit() const
{
    return parentLimit;
}

void Order::partiallyFillOrder(int orderedShares)
{
    shares -= orderedShares;
    parentLimit->partiallyFillTotalVolume(orderedShares);
}

// Remove order from its parent limit
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

    parentLimit->totalVolume -= shares;
    parentLimit->size -= 1;
}

// Execute head order
void Order::execute()
{
    parentLimit->headOrder = nextOrder;
    if (nextOrder == nullptr)
    {
        parentLimit->tailOrder = nullptr;
    } else
    {
        nextOrder->prevOrder = nullptr;
    }
    nextOrder = nullptr;
    prevOrder = nullptr;

    parentLimit->totalVolume -= shares;
    parentLimit->size -= 1;
}

void Order::modifyOrder(int newShares, int newLimit)
{
    shares = newShares;
    limit = newLimit;
    nextOrder = nullptr;
    prevOrder = nullptr;
    parentLimit = nullptr;
}

void Order::setShares(int newShares)
{
    shares = newShares;
}

void Order::print() const
{
    std::cout << "Order ID: " << idNumber 
    << ", Order Type: " << (buyOrSell == 1 ? "buy" : "sell") 
    << ", Order Size: " << shares
    << ", Order Limit: " << limit 
    << std::endl;
}