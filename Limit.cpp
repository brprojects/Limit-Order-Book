#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>
using namespace std;

Limit::Limit(int _limitPrice, int _size, int _totalVolume)
    : limitPrice(_limitPrice), size(_size), totalVolume(_totalVolume),
    parent(nullptr), leftChild(nullptr), rightChild(nullptr),
    headOrder(nullptr), tailOrder(nullptr) {}

void Limit::append(Order *order){
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

void Limit::printForward(){
    Order* current = headOrder;
    while (current != nullptr) {
        cout << current->idNumber << " ";
        current = current->nextOrder;
    }
    cout << endl;
}

void Limit::printBackward(){
    Order* current = tailOrder;
    while (current != nullptr) {
        cout << current->idNumber << " ";
        current = current->prevOrder;
    }
    cout << endl;
}

void Limit::print(){
    cout << totalVolume << endl;
}
