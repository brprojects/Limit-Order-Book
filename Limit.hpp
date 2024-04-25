#ifndef LIMIT_HPP
#define LIMIT_HPP

class Order;

class Limit {
private:
    int limitPrice;
    int size;
    int totalVolume;
    bool buyOrSell;
    int height;
    Limit *parent;
    Limit *leftChild;
    Limit *rightChild;
    Order *headOrder;
    Order *tailOrder;

    friend class Order;
public:
    Limit(int _limitPrice, bool _buyOrSell, int _size=0, int _totalVolume=0);
    ~Limit();

    Order* getHeadOrder() const;
    int getLimitPrice() const;
    int getSize() const;
    int getTotalVolume() const;
    bool getBuyOrSell() const;
    int getHeight() const;
    Limit* getParent() const;
    Limit* getLeftChild() const;
    Limit* getRightChild() const;
    void setParent(Limit* newParent);
    void setLeftChild(Limit* newLeftChild);
    void setRightChild(Limit* newRightChild);
    void setHeight(int newHeight);

    void append(Order *_order);

    void printForward() const;
    void printBackward() const;
    void print() const;
};

#endif