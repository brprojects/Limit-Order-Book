#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

#include "Limit.hpp"
#include "Order.hpp"

class Book {
private:
    std::vector<Limit> buyLimits;
    std::vector<Limit> sellLimits;
    std::vector<Limit> stopBuyLimits;
    std::vector<Limit> stopSellLimits;
    std::unordered_map<int, Order*> orderMap;
    Limit& getOrCreateLimit(std::vector<Limit>& limits, int price, bool descending, bool createIfNotFound = true);
    void removeEmptyLimit(std::vector<Limit>& limits, size_t index);
    void triggerStopOrders();

    int crossLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
    int crossStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
    int crossMarketLimitOrder(Order *order);
    int crossStopLimit(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
    void executeMarketOrder(int orderId, bool buyOrSell, int shares);
    void convertStopLimitToLimit(Order *order, bool buyOrSell);

public:
    Book();
    ~Book();

    // Counts used in order book perforamce visualisations
    int executedOrdersCount=0;

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

    const std::vector<Limit>& getBuyLimits() const {return buyLimits;}
    const std::vector<Limit>& getSellLimits() const {return sellLimits;}
    const std::vector<Limit>& getStopBuyLimits() const {return stopBuyLimits;}
    const std::vector<Limit>& getStopSellLimits() const {return stopSellLimits;}
    Order* searchOrderMap(int orderId) const;

    // Functions for visualising the order book
    void printOrder(int orderId) const;
    void printBookEdges() const;
    void printOrderBook() const;

    // Functions and data structures needed for generating sample data
    Order* getRandomOrder(int key, std::mt19937 gen) const;
    std::unordered_set<Order*> limitOrders;
    std::unordered_set<Order*> stopOrders;
    std::unordered_set<Order*> stopLimitOrders;

    int getBestBidPrice() const {
        return buyLimits.empty() ? 0 : buyLimits.front().getLimitPrice();
    }

    int getBestAskPrice() const {
        return sellLimits.empty() ? 0 : sellLimits.front().getLimitPrice();
    }

    int getAVLTreeBalanceCount() const {
        return 0;  // No AVL tree anymore
    }
};

#endif