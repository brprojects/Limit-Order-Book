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
    friend class Book;
public:
    Limit(int _limitPrice, int _size=0, int _totalVolume=0);
    Order* getHeadOrder() const;
    void append(Order *_order);
    void printForward() const;
    void printBackward() const;
    void print() const;
};

#endif