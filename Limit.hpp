#ifndef LIMIT_HPP
#define LIMIT_HPP

class Order;

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

    friend class Order;
public:
    Limit(int _limitPrice, int _size=0, int _totalVolume=0);
    void append(Order *_order);
    void printForward();
    void printBackward();
    void print();
};

#endif