#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>

Limit::Limit(int _limitPrice, bool _buyOrSell, int _size, int _totalVolume)
    : limitPrice(_limitPrice), buyOrSell(_buyOrSell), size(_size), totalVolume(_totalVolume),
    height(1), parent(nullptr), leftChild(nullptr), rightChild(nullptr),
    headOrder(nullptr), tailOrder(nullptr) {}

Limit::~Limit()
{
    if (parent == nullptr)
    {
        return;
    }
    if (parent->leftChild = this)
    {
        parent->leftChild = nullptr;
    } else
    {
        parent->rightChild = nullptr;
    }
}

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

bool Limit::getBuyOrSell() const
{
    return buyOrSell;
}

int Limit::getHeight() const
{
    return height;
}

Limit* Limit::getParent() const
{
    return parent;
}

Limit* Limit::getLeftChild() const
{
    return leftChild;
}

Limit* Limit::getRightChild() const
{
    return rightChild;
}

void Limit::setParent(Limit* newParent)
{
    parent = newParent;
}

void Limit::setLeftChild(Limit* newLeftChild)
{
    leftChild = newLeftChild;
}

void Limit::setRightChild(Limit* newRightChild)
{
    rightChild = newRightChild;
}

void Limit::setHeight(int newHeight)
{
    height = newHeight;
}

// Add an order to the limit
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
        totalVolume += order->getOrderSize();
        order->parentLimit = this;
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
