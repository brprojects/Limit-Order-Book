#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>
#include <cassert>
//Test
Book::Book() = default;
// When deleting the book need to ensure all used memory is freed
Book::~Book()
{
    for (auto& pair: orderMap) {
        delete pair.second;
    }
}

Limit& Book::getOrCreateLimit(std::vector<Limit>& limits, int price, bool descending, bool createIfNotFound) {
    if (limits.empty()) {
        if (!createIfNotFound) {
            throw std::runtime_error("Limit not found and createIfNotFound = false");
        }
        limits.emplace_back(price);
        return limits.back();
    }

    auto cmp = [descending](const Limit& l, int p) {
        return descending ? (l.getLimitPrice() > p) : (l.getLimitPrice() < p);
    };
    auto it = std::lower_bound(limits.begin(), limits.end(), price, cmp);
    if (it != limits.end() && it->getLimitPrice() == price) {
        return *it;
    }

    if (!createIfNotFound) {
        throw std::runtime_error("Limit not found");
    }
    return *limits.insert(it, Limit(price));
}

void Book::removeEmptyLimit(std::vector<Limit>& limits, size_t index) {
    if (index < limits.size()) {
        limits.erase(limits.begin() + index);
    }
}

int Book::crossLimitOrder(int orderId, bool buyOrSell, int shares, int LimitPrice) {
    std::vector<Limit>& opposite = buyOrSell ? sellLimits : buyLimits;
    size_t i = 0;
    while (shares > 0 && i < opposite.size()) {
        Limit& level = opposite[i];
        int priceLevel = level.getLimitPrice();
        if ((buyOrSell && priceLevel > LimitPrice) || (!buyOrSell && priceLevel < LimitPrice)) {
            break;
        }

        Order* current = level.getHeadOrder();
        while (current && shares > 0) {
            int fillSize = std::min(shares, current->getShares());
            current->partiallyFillOrder(fillSize);
            level.partiallyFillTotalVolume(fillSize);
            shares -= fillSize;
            executedOrdersCount++;

            if (current->getShares() == 0) {
                Order* next = current->nextOrder;
                level.removeOrder(current);
                orderMap.erase(current->getOrderId());
                delete current;
                current = next;
            } else {
                current = current->nextOrder;
            }
        }

        if (level.isEmpty()) {
            removeEmptyLimit(opposite, i);
        } else {
            ++i;
        }
    }
    return shares;
}

int Book::crossStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice) {
    Limit* bestAsk = sellLimits.empty() ? nullptr : &sellLimits.front();
    Limit* bestBid = buyLimits.empty() ? nullptr : &buyLimits.front();

    if (buyOrSell) { // Buy stop
        if (bestAsk && stopPrice <= bestAsk->getLimitPrice()) {
            executeMarketOrder(orderId, true, shares);
            return 0;
        }
    } else { // Sell stop
        if (bestBid && stopPrice >= bestBid->getLimitPrice()) {
            executeMarketOrder(orderId, false, shares);
            return 0;
        }
    }
    return shares;
}

int Book::crossMarketLimitOrder(Order* order) {
    bool buyOrSell = order->getBuyOrSell();
    int shares = order->getShares();
    int limitPrice = order->getLimit();

    return crossLimitOrder(order->getOrderId(), buyOrSell, shares, limitPrice);
}

// Immediate check for stop-limit trigger
int Book::crossStopLimit(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice) {
    Limit* bestAsk = sellLimits.empty() ? nullptr : &sellLimits.front();
    Limit* bestBid = buyLimits.empty() ? nullptr : &buyLimits.front();

    if (buyOrSell) {
        if (bestAsk && stopPrice <= bestAsk->getLimitPrice()) {
            // Triggered: treat as limit order
            addLimitOrder(orderId, true, shares, limitPrice);
            return 0;
        }
    } else {
        if (bestBid && stopPrice >= bestBid->getLimitPrice()) {
            addLimitOrder(orderId, false, shares, limitPrice);
            return 0;
        }
    }
    return shares;
}

void Book::executeMarketOrder(int orderId, bool buyOrSell, int shares) {
    std::vector<Limit>& opposite = buyOrSell ? sellLimits : buyLimits;

    size_t i = 0;
    while (shares > 0 && i < opposite.size()) {
        Limit& level = opposite[i];
        Order* current = level.getHeadOrder();

        while (current && shares > 0) {
            int fillSize = std::min(shares, current->getShares());
            current->partiallyFillOrder(fillSize);
            level.partiallyFillTotalVolume(fillSize);
            shares -= fillSize;
            executedOrdersCount++;

            if (current->getShares() == 0) {
                Order* next = current->nextOrder;
                level.removeOrder(current);
                orderMap.erase(current->getOrderId());
                delete current;
                current = next;
            } else {
                current = current->nextOrder;
            }
        }

        if (level.isEmpty()) {
            removeEmptyLimit(opposite, i);
        } else {
            ++i;
        }
    }
}

void Book::convertStopLimitToLimit(Order* order, bool buyOrSell) {
    int remaining = crossMarketLimitOrder(order);

    if (remaining > 0) {
        order->setShares(remaining);
        std::vector<Limit>& side = buyOrSell ? buyLimits : sellLimits;
        Limit& level = getOrCreateLimit(side, order->getLimit(), buyOrSell);
        level.appendOrder(order);
    } else {
        // Fully filled
        orderMap.erase(order->getOrderId());
        delete order;
    }
}

void Book::triggerStopOrders() {
    Limit* bestAsk = sellLimits.empty() ? nullptr : &sellLimits.front();
    Limit* bestBid = buyLimits.empty() ? nullptr : &buyLimits.front();

    // Trigger buy stops
    while (!stopBuyLimits.empty()) {
        Limit& level = stopBuyLimits.front();
        if (bestAsk == nullptr || level.getLimitPrice() > bestAsk->getLimitPrice()) break;

        Order* head = level.getHeadOrder();
        Order* next = head->nextOrder;
        level.removeOrder(head);

        if (head->getLimit() == 0) {
            // Stop market
            executeMarketOrder(0, true, head->getShares());
            orderMap.erase(head->getOrderId());
            delete head;
        } else {
            // Stop-limit
            convertStopLimitToLimit(head, true);
        }

        if (level.isEmpty()) {
            stopBuyLimits.erase(stopBuyLimits.begin());
        }
        head = next;
    }

    // Trigger sell stops
    while (!stopSellLimits.empty()) {
        Limit& level = stopSellLimits.front();
        if (bestBid == nullptr || level.getLimitPrice() < bestBid->getLimitPrice()) break;

        Order* head = level.getHeadOrder();
        Order* next = head->nextOrder;
        level.removeOrder(head);

        if (head->getLimit() == 0) {
            executeMarketOrder(0, false, head->getShares());
            orderMap.erase(head->getOrderId());
            delete head;
        } else {
            convertStopLimitToLimit(head, false);
        }

        if (level.isEmpty()) {
            stopSellLimits.erase(stopSellLimits.begin());
        }
        head = next;
    }
}

void Book::marketOrder(int orderId, bool buyOrSell, int shares) {
    executedOrdersCount = 0;
    executeMarketOrder(orderId, buyOrSell, shares);
    triggerStopOrders();
}

void Book::addLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice) {
    executedOrdersCount = 0;
    int remaining = crossLimitOrder(orderId, buyOrSell, shares, limitPrice);

    if (remaining > 0) {
        Order* newOrder = new Order(orderId, buyOrSell, remaining, limitPrice);
        orderMap[orderId] = newOrder;

        std::vector<Limit>& side = buyOrSell ? buyLimits : sellLimits;
        Limit& level = getOrCreateLimit(side, limitPrice, buyOrSell);
        level.appendOrder(newOrder);
    }

    if (remaining < shares) {
        triggerStopOrders();
    }
}

void Book::cancelLimitOrder(int orderId) {
    executedOrdersCount = 0;
    Order* order = searchOrderMap(orderId);
    if (!order || !order->parentLimit) return;

    Limit* level = order->parentLimit;
    level->removeOrder(order);
    orderMap.erase(orderId);
    delete order;

    if (level->isEmpty()) {
        std::vector<Limit>* side = nullptr;

        if (std::find_if(buyLimits.begin(), buyLimits.end(), 
                         [level](const Limit& l) { return &l == level; }) != buyLimits.end()) {
            side = &buyLimits;
        } else if (std::find_if(sellLimits.begin(), sellLimits.end(), 
                                [level](const Limit& l) { return &l == level; }) != sellLimits.end()) {
            side = &sellLimits;
        }

        if (side) {
            auto it = std::find_if(side->begin(), side->end(), 
                                   [level](const Limit& l) { return &l == level; });
            removeEmptyLimit(*side, std::distance(side->begin(), it));
        }
    }
}

void Book::modifyLimitOrder(int orderId, int newShares, int newLimit) {
    executedOrdersCount = 0;
    Order* order = searchOrderMap(orderId);
    if (!order || !order->parentLimit) return;

    Limit* oldLevel = order->parentLimit;
    bool isBuy = order->getBuyOrSell();

    // Remove from old level (keep object alive)
    oldLevel->removeOrder(order);

    // Clean up empty level
    if (oldLevel->isEmpty()) {
        std::vector<Limit>& oldSide = isBuy ? buyLimits : sellLimits;
        auto it = std::find_if(oldSide.begin(), oldSide.end(), [oldLevel](const Limit& l) { return &l == oldLevel; });
        if (it != oldSide.end()) {
            removeEmptyLimit(oldSide, std::distance(oldSide.begin(), it));
        }
    }

    // Update using existing method
    order->modifyOrder(newShares, newLimit);

    // Add to new level
    std::vector<Limit>& newSide = isBuy ? buyLimits : sellLimits;
    Limit& newLevel = getOrCreateLimit(newSide, newLimit, isBuy);
    newLevel.appendOrder(order);

    triggerStopOrders();
}

void Book::addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice) {
    executedOrdersCount = 0;
    int remaining = crossStopOrder(orderId, buyOrSell, shares, stopPrice);

    if (remaining > 0) {
        Order* newOrder = new Order(orderId, buyOrSell, remaining, 0); // limit = 0 for market stop
        orderMap[orderId] = newOrder;

        std::vector<Limit>& side = buyOrSell ? stopBuyLimits : stopSellLimits;
        Limit& level = getOrCreateLimit(side, stopPrice, buyOrSell);
        level.appendOrder(newOrder);
    }
}

void Book::cancelStopOrder(int orderId) {
    cancelLimitOrder(orderId); // Same logic — stops use same parentLimit
}

void Book::modifyStopOrder(int orderId, int newShares, int newStopPrice) {
    executedOrdersCount = 0;
    Order* order = searchOrderMap(orderId);
    if (!order || !order->parentLimit) return;

    Limit* oldLevel = order->parentLimit;
    bool isBuy = order->getBuyOrSell();

    oldLevel->removeOrder(order);

    if (oldLevel->isEmpty()) {
        std::vector<Limit>& oldSide = isBuy ? stopBuyLimits : stopSellLimits;
        auto it = std::find_if(oldSide.begin(), oldSide.end(), [oldLevel](const Limit& l) { return &l == oldLevel; });
        if (it != oldSide.end()) {
            removeEmptyLimit(oldSide, std::distance(oldSide.begin(), it));
        }
    }

    order->modifyOrder(newShares, newStopPrice);  // stop price goes into limit field

    std::vector<Limit>& newSide = isBuy ? stopBuyLimits : stopSellLimits;
    Limit& newLevel = getOrCreateLimit(newSide, newStopPrice, isBuy);
    newLevel.appendOrder(order);
}

void Book::addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice) {
    executedOrdersCount = 0;
    int remaining = crossStopLimit(orderId, buyOrSell, shares, limitPrice, stopPrice);

    if (remaining > 0) {
        Order* newOrder = new Order(orderId, buyOrSell, remaining, limitPrice);
        orderMap[orderId] = newOrder;

        std::vector<Limit>& side = buyOrSell ? stopBuyLimits : stopSellLimits;
        Limit& level = getOrCreateLimit(side, stopPrice, buyOrSell);
        level.appendOrder(newOrder);
    }
}

void Book::cancelStopLimitOrder(int orderId) {
    cancelLimitOrder(orderId);
}

void Book::modifyStopLimitOrder(int orderId, int newShares, int newLimitPrice, int newStopPrice) {
    executedOrdersCount = 0;
    Order* order = searchOrderMap(orderId);
    if (!order || !order->parentLimit) return;

    Limit* oldLevel = order->parentLimit;
    bool isBuy = order->getBuyOrSell();

    oldLevel->removeOrder(order);

    if (oldLevel->isEmpty()) {
        std::vector<Limit>& oldSide = isBuy ? stopBuyLimits : stopSellLimits;
        auto it = std::find_if(oldSide.begin(), oldSide.end(), [oldLevel](const Limit& l) { return &l == oldLevel; });
        if (it != oldSide.end()) {
            removeEmptyLimit(oldSide, std::distance(oldSide.begin(), it));
        }
    }

    order->modifyOrder(newShares, newLimitPrice);

    std::vector<Limit>& newSide = isBuy ? stopBuyLimits : stopSellLimits;
    Limit& newLevel = getOrCreateLimit(newSide, newStopPrice, isBuy);
    newLevel.appendOrder(order);
}

Order* Book::searchOrderMap(int orderId) const {
    auto it = orderMap.find(orderId);
    return it != orderMap.end() ? it->second : nullptr;
}

void Book::printBookEdges() const {
    int bestBid = buyLimits.empty() ? 0 : buyLimits.front().getLimitPrice();
    int bestAsk = sellLimits.empty() ? 0 : sellLimits.front().getLimitPrice();
    std::cout << "Best Bid: " << bestBid << " | Best Ask: " << bestAsk << std::endl;
}

void Book::printOrderBook() const {
    std::cout << "=== BUY SIDE (best to worst) ===\n";
    for (const auto& level : buyLimits) {
        level.print();
        level.printForward();
    }

    std::cout << "\n=== SELL SIDE (best to worst) ===\n";
    for (const auto& level : sellLimits) {
        level.print();
        level.printForward();
    }

    std::cout << "\n=== STOP BUY LEVELS ===\n";
    for (const auto& level : stopBuyLimits) level.print();

    std::cout << "\n=== STOP SELL LEVELS ===\n";
    for (const auto& level : stopSellLimits) level.print();
}

void Book::printOrder(int orderId) const {
    Order* o = searchOrderMap(orderId);
    if (o) {
        o->print();
    } else {
        std::cout << "Order " << orderId << " not found.\n";
    }
}

// Return a random active order
// 0:Limit, 1:Stop, 2:StopLimit
Order* Book::getRandomOrder(int key, std::mt19937 gen) const
{
    if (key == 0)
    {
        if (limitOrders.size() > 10000)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, limitOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = limitOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    } else if (key == 1)
    {
        if (stopOrders.size() > 500)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, stopOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = stopOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    } else if (key == 2)
    {
        if (stopLimitOrders.size() > 500)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, stopLimitOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = stopLimitOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    }
    return nullptr;
}
