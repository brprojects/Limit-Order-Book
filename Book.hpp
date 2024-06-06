#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

class Limit;
class Order;

class Book {
private:
    Limit *buyTree;
    Limit *sellTree;
    Limit *lowestSell;
    Limit *highestBuy;

    Limit *stopBuyTree;
    Limit *stopSellTree;
    Limit *highestStopSell;
    Limit *lowestStopBuy;

    std::unordered_map<int, Order*> orderMap;
    std::unordered_map<int, Limit*> limitBuyMap;
    std::unordered_map<int, Limit*> limitSellMap;
    std::unordered_map<int, Limit*> stopMap;

    void addLimit(int limitPrice, bool buyOrSell);
    void addStop(int stopPrice, bool buyOrSell);
    Limit* insert(Limit* root, Limit* limit, Limit* parent=nullptr);
    Limit* insertStop(Limit* root, Limit* limit, Limit* parent=nullptr);
    void updateBookEdgeInsert(Limit* newLimit);
    void updateStopBookEdgeInsert(Limit* newStop);
    void updateBookEdgeRemove(Limit* limit);
    void updateStopBookEdgeRemove(Limit* stopLevel);
    void changeBookRoots(Limit* limit);
    void changeStopBookRoots(Limit* stopLevel);
    void deleteLimit(Limit* limit);
    void deleteStopLevel(Limit* limit);
    void deleteFromOrderMap(int orderId);
    void deleteFromLimitMaps(int LimitPrice, bool buyOrSell);
    void deleteFromStopMap(int StopPrice);
    int limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    int stopOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
    int existingOrderAsMarketOrder(Order* headOrder, bool buyOrSell);
    int stopLimitOrderAsLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
    void executeStopOrders(bool buyOrSell);
    void stopLimitOrderToLimitOrder(Order* headOrder, bool buyOrSell);
    void marketOrderHelper(int orderId, bool buyOrSell, int shares);

    // Functions to balance AVL tree
    int limitHeightDifference(Limit* limit);
    Limit* rr_rotate(Limit* limit);
    Limit* ll_rotate(Limit* limit);
    Limit* lr_rotate(Limit* limit);
    Limit* rl_rotate(Limit* limit);
    Limit* balance(Limit* limit);
    Limit* rr_rotateStop(Limit* limit);
    Limit* ll_rotateStop(Limit* limit);
    Limit* lr_rotateStop(Limit* limit);
    Limit* rl_rotateStop(Limit* limit);
    Limit* balanceStop(Limit* limit);

public:
    Book();
    ~Book();

    // Counts used in order book perforamce visualisations
    int executedOrdersCount=0;
    int AVLTreeBalanceCount=0;

    // Getter and setter functions
    Limit* getBuyTree() const;
    Limit* getSellTree() const;
    Limit* getLowestSell() const;
    Limit* getHighestBuy() const;
    Limit* getStopBuyTree() const;
    Limit* getStopSellTree() const;
    Limit* getHighestStopSell() const;
    Limit* getLowestStopBuy() const;

    // Functions for different types of orders
    void marketOrder(int orderId, bool buyOrSell, int shares);
    void addLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    void cancelLimitOrder(int orderId);
    void modifyLimitOrder(int orderId, int newShares, int newLimit);
    void addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
    void cancelStopOrder(int orderId);
    void modifyStopOrder(int orderId, int newShares, int newStopPrice);
    void addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
    void cancelStopLimitOrder(int orderId);
    void modifyStopLimitOrder(int orderId, int newShares, int newLimitPrice, int newStopPrice);

    // Functions that needed to be public for testing purposes
    int getLimitHeight(Limit* limit) const;
    Order* searchOrderMap(int orderId) const;
    Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
    Limit* searchStopMap(int stopPrice) const;

    // Functions for visualising the order book
    void printLimit(int limitPrice, bool buyOrSell) const;
    void printOrder(int orderId) const;
    void printBookEdges() const;
    void printOrderBook() const;
    std::vector<int> inOrderTreeTraversal(Limit* root) const;
    std::vector<int> preOrderTreeTraversal(Limit* root) const;
    std::vector<int> postOrderTreeTraversal(Limit* root) const;

    // Functions and data structures needed for generating sample data
    Order* getRandomOrder(int key, std::mt19937 gen) const;
    std::unordered_set<Order*> limitOrders;
    std::unordered_set<Order*> stopOrders;
    std::unordered_set<Order*> stopLimitOrders;
};

#endif