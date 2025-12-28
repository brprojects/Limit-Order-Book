#ifndef LIMIT_HPP
#define LIMIT_HPP

class Order;

class Limit {
private:
    int limitPrice;
    int size;
    int totalVolume; // removed parent, child, and buyOrSell since there will be 2 separate vectors storing buy and sell orders
    Order *headOrder;
    Order *tailOrder;

    friend class Order;

public:
    Limit(int _limitPrice, int _size=0, int _totalVolume=0);
    ~Limit() = default;

    Order* getHeadOrder() const;
    int getLimitPrice() const;
    int getSize() const;
    int getTotalVolume() const;
    void partiallyFillTotalVolume(int orderedShares);

    void appendOrder(Order *order);
    void removeOrder(Order *order);
    bool isEmpty() const;

    void printForward() const;
    void printBackward() const;
    void print() const;
};

#endif