#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>

Limit::Limit(int _limitPrice, int _size, int _totalVolume)
    : limitPrice(_limitPrice), size(_size), totalVolume(_totalVolume),
    parent(nullptr), leftChild(nullptr), rightChild(nullptr),
    headOrder(nullptr), tailOrder(nullptr) {}

void Limit::append(Order *order)
{
        if (headOrder == nullptr) {
            headOrder = tailOrder = order;
        } else {
            tailOrder->nextOrder = order;
            order->prevOrder = tailOrder;
            tailOrder = order;
        }
        size += 1;
        totalVolume += order->shares;
        order->parentLimit = this;
}

void Limit::printForward()
{
    Order* current = headOrder;
    while (current != nullptr) {
        std::cout << current->idNumber << " ";
        current = current->nextOrder;
    }
    std::cout << std::endl;
}

void Limit::printBackward()
{
    Order* current = tailOrder;
    while (current != nullptr) {
        std::cout << current->idNumber << " ";
        current = current->prevOrder;
    }
    std::cout << std::endl;
}

void Limit::print()
{
    std::cout << totalVolume << std::endl;
}
