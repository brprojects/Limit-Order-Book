#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <vector>

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

    void addOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    void marketOrder(int orderId, bool buyOrSell, int shares);
    void cancelOrder(int orderId);
    int getLimitHeight(Limit* limit) const;
    Order* searchOrderMap(int orderId) const;
    Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
    
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    void printBookEdges() const;
    std::vector<int> inOrderTreeTraversal(Limit* root);
    std::vector<int> preOrderTreeTraversal(Limit* root);
    std::vector<int> postOrderTreeTraversal(Limit* root);
};

#endif