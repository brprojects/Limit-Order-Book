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
    std::unordered_map<int, Limit*> limitBuyMap;
    std::unordered_map<int, Limit*> limitSellMap;

public:
    Book();
    ~Book();
    void addOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    void addLimit(int limitPrice, bool buyOrSell);
    Limit* insert(Limit* root, Limit* limit, Limit* parent=nullptr);
    void updateBookEdge(Limit* newLimit, bool buyOrSell);
    void cancelOrder(int orderId);
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    Order* searchOrderMap(int orderId) const;
    Limit* searchLimitBuyMap(int limitPrice) const;
    Limit* searchLimitSellMap(int limitPrice) const;
};

#endif