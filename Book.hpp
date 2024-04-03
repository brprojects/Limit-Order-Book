#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>

class Limit;
class Order;

class Book {
private:
    Limit *buyTree;
    Limit *sellTree;
    Limit *lowestSell;
    Limit *highestBuy;
    std::unordered_map<int, Order*> orderMap;
    std::unordered_map<int, Limit*> limitMap;

public:
    Book();
    ~Book();
    void addOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    void addLimit(int limitPrice);
    void printLimit(int limitPrice);
    void printOrder(int orderId);
};

#endif