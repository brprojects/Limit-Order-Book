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
    void changeBookRoots(Limit* limit);
    void deleteLimit(Limit* limit);
    void cancelOrder(int orderId);
    Order* searchOrderMap(int orderId) const;
    void deleteFromOrderMap(int orderId);
    Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
    void deleteFromLimitMaps(int LimitPrice, bool buyOrSell);

    int getLimitHeight(Limit* limit) const;
    int limitHeightDifference(Limit* limit);
    Limit* rr_rotate(Limit* limit);
    Limit* ll_rotate(Limit* limit);
    Limit* lr_rotate(Limit* limit);
    Limit* rl_rotate(Limit* limit);
    Limit* balance(Limit* limit);
    
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    void printBookEdges() const;
    std::vector<int> inOrderTreeTraversal(Limit* root);
    std::vector<int> preOrderTreeTraversal(Limit* root);
    std::vector<int> postOrderTreeTraversal(Limit* root);
};

#endif