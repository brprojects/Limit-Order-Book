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

public:
    Book();
    ~Book();

    Limit* getBuyTree() const;
    Limit* getSellTree() const;

    void addOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    void addLimit(int limitPrice, bool buyOrSell);
    Limit* insert(Limit* root, Limit* limit, Limit* parent=nullptr);
    void updateBookEdgeInsert(Limit* newLimit);
    void updateBookEdgeRemove(Limit* limit);
    void cancelOrder(int orderId);
    Order* searchOrderMap(int orderId) const;
    void deleteFromOrderMap(int orderId);
    Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
    void deleteFromLimitMaps(int LimitPrice, bool buyOrSell);
    void updateLimitHeightsOnInsert(Limit* limit);
    
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    void printBookEdges() const;

    std::vector<int> inOrderTreeTraversal(Limit* root);
    std::vector<int> preOrderTreeTraversal(Limit* root);
    std::vector<int> postOrderTreeTraversal(Limit* root);
    int limitHeightDifference(Limit *);
    Limit* rr_rotate(Limit *);
    Limit* ll_rotate(Limit *);
    Limit* lr_rotate(Limit*);
    Limit* rl_rotate(Limit *);
    Limit* balance(Limit *);

    void removeLimit(Limit*);
};

#endif