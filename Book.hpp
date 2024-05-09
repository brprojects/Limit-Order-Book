#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <vector>
#include <random>

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

    void addLimit(int limitPrice, bool buyOrSell);
    Limit* insert(Limit* root, Limit* limit, Limit* parent=nullptr);
    void updateBookEdgeInsert(Limit* newLimit);
    void updateBookEdgeRemove(Limit* limit);
    void changeBookRoots(Limit* limit);
    void deleteLimit(Limit* limit);
    void deleteFromOrderMap(int orderId);
    void deleteFromLimitMaps(int LimitPrice, bool buyOrSell);
    int limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice);

    int limitHeightDifference(Limit* limit);
    Limit* rr_rotate(Limit* limit);
    Limit* ll_rotate(Limit* limit);
    Limit* lr_rotate(Limit* limit);
    Limit* rl_rotate(Limit* limit);
    Limit* balance(Limit* limit);

public:
    Book();
    ~Book();

    Limit* getBuyTree() const;
    Limit* getSellTree() const;
    Limit* getLowestSell() const;
    Limit* getHighestBuy() const;

    void marketOrder(int orderId, bool buyOrSell, int shares);
    void addOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    // void addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
    // void addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
    void cancelOrder(int orderId);
    // void cancelStopOrder(int orderId);
    // void cancelStopLimitOrder(int orderId);
    void modifyOrder(int orderId, int newShares, int newLimit);
    // void modifyStopOrder(int orderId, int newShares, int newStopPrice);
    // void modifyStopLimitOrder(int orderId, int newShares, int newLimit, int newStopPrice);

    int getLimitHeight(Limit* limit) const;
    Order* searchOrderMap(int orderId) const;
    Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
    Order* getRandomOrder(std::mt19937 gen) const;
    
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    void printBookEdges() const;
    std::vector<int> inOrderTreeTraversal(Limit* root);
    std::vector<int> preOrderTreeTraversal(Limit* root);
    std::vector<int> postOrderTreeTraversal(Limit* root);
};

#endif