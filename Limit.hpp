#ifndef LIMIT_HPP
#define LIMIT_HPP

#include "Order.hpp"

class Limit {
private:
    int limitPrice;
    int size;
    int totalVolume;
    Limit *parent;
    Limit *leftChild;
    Limit *rightChild;
    Order *headOrder;
    Order *tailOrder;
public:
    Limit();
    void append();
};

#endif