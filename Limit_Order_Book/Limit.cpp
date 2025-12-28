#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>
#include <cassert>

Limit::Limit(int _limitPrice, int _size, int _totalVolume)
    : limitPrice(_limitPrice), size(_size), totalVolume(_totalVolume),
    headOrder(nullptr), tailOrder(nullptr) {
}

// removed original destructor, default destructor is fine

Order* Limit::getHeadOrder() const
{
    return headOrder;
}

int Limit::getLimitPrice() const
{
    return limitPrice;
}

int Limit::getSize() const
{
    return size;
}

int Limit::getTotalVolume() const
{
    return totalVolume;
}

void Limit::partiallyFillTotalVolume(int orderedShares)
{
    totalVolume -= orderedShares;
    assert(totalVolume >= 0); // guarantee no over filling
}

// Add an order to the limit
void Limit::appendOrder(Order* order)
{
    // assert(order != nullptr);
    // if (headOrder == nullptr) {
    //     headOrder = tailOrder = order;
    // } else {
    //     tailOrder->nextOrder = order;
    //     order->prevOrder = tailOrder;
    //     order->nextOrder = nullptr;
    //     tailOrder = order;
    // }
    // size += 1;
    // totalVolume += order->getShares();
    // order->parentLimit = this;

    assert(order != nullptr);

    order->nextOrder = nullptr;
    order->prevOrder = nullptr;
    order->parentLimit = this;

    if (headOrder == nullptr) {
        headOrder = tailOrder = order;
    }
    else {
        tailOrder->nextOrder = order;
        order->prevOrder = tailOrder;
        tailOrder = order;
    }

    size++;
    totalVolume += order->getShares();
}

// ADD: remove specific order, handling case like cancel order, modify order etc.
void Limit::removeOrder(Order* order)
{
    if (!order || order->parentLimit != this) {
        return;   // already removed or invalid � SAFE EXIT
    }

    if (order->prevOrder) {
        order->prevOrder->nextOrder = order->nextOrder;
    }
    else {
        headOrder = order->nextOrder;
    }

    if (order->nextOrder) {
        order->nextOrder->prevOrder = order->prevOrder;
    }
    else {
        tailOrder = order->prevOrder;
    }

    size--;
    totalVolume -= order->getShares();

    order->parentLimit = nullptr;
    order->prevOrder = nullptr;
    order->nextOrder = nullptr;
}

// ADD: helper function on checking if the limit is empty
bool Limit::isEmpty() const {
    return headOrder == nullptr;
}

void Limit::printForward() const
{
    Order* current = headOrder;
    while (current != nullptr) {
        std::cout << current->getOrderId() << " ";
        current = current->nextOrder;
    }
    std::cout << std::endl;
}

void Limit::printBackward() const
{
    Order* current = tailOrder;
    while (current != nullptr) {
        std::cout << current->getOrderId() << " ";
        current = current->prevOrder;
    }
    std::cout << std::endl;
}

void Limit::print() const
{
    std::cout << "Limit Price: " << limitPrice
        << ", Limit Volume: " << totalVolume
        << ", Limit Size: " << size
        << std::endl;
}
