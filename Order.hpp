#ifndef ORDER_HPP
#define ORDER_HPP

#include <memory>
#include "Limit.hpp"

class Order {
private:
    int idNumber;
    bool buyOrSell;
    int shares;
    int limit;
    std::unique_ptr<Order> nextOrder;
    std::unique_ptr<Order> prevOrder;
    Limit *parentLimit;
public:
    Order(int _idNumber, bool _buyOrSell, int _shares, int _limit);
};

#endif