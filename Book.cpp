#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>

Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr){}

Book::~Book()
{
    for (auto& [id, order] : orderMap) {
        // order->print();
        delete order;
    }
    orderMap.clear();

    for (auto& [limitPrice, limit] : limitBuyMap) {
        delete limit;
    }
    limitBuyMap.clear();

    for (auto& [limitPrice, limit] : limitSellMap) {
        delete limit;
    }
    limitSellMap.clear();
}

void Book::addOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
    orderMap.emplace(orderId, newOrder);

    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

    if (limitMap.find(limitPrice) == limitMap.end())
    {
        addLimit(limitPrice, newOrder->buyOrSell);
    }
    limitMap.at(limitPrice)->append(newOrder);
}

void Book::addLimit(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;
    auto& tree = buyOrSell ? buyTree : sellTree;
    auto& bookEdge = buyOrSell ? highestBuy : lowestSell;

    Limit* newLimit = new Limit(limitPrice);
    limitMap.emplace(limitPrice, newLimit);

    if (tree == nullptr)
    {
        tree = newLimit;
        bookEdge = newLimit;
    } else
    {
        Limit* root = insert(tree, newLimit);
        updateBookEdge(newLimit, buyOrSell);
    }
}

Limit* Book::insert(Limit* root, Limit* limit, Limit* parent)
{
    if (root == nullptr)
    {
        limit->parent = parent;
        return limit;
    }
    if (limit->limitPrice < root->limitPrice)
    {
        root->leftChild = insert(root->leftChild, limit, root);
    } else if (limit->limitPrice > root->limitPrice) 
    {
        root->rightChild = insert(root->rightChild, limit, root);
    }

    return root;
}

void Book::updateBookEdge(Limit* newLimit, bool buyOrSell)
{
    if (buyOrSell)
    {
        if (newLimit->limitPrice > highestBuy->limitPrice)
        {
            highestBuy = newLimit;
        }
    } else
    {
        if (newLimit->limitPrice < lowestSell->limitPrice)
        {
            lowestSell = newLimit;
        }
    }
}

void Book::cancelOrder(int orderId)
{
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
        delete order;
        orderMap.erase(orderId);
    }
}

void Book::printLimit(int limitPrice, bool buyOrSell) const
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

    if (limitMap.find(limitPrice) != limitMap.end())
    {
        std::cout << (buyOrSell ? "Buy " : "Sell ") << "limit forwards:" << std::endl;
        limitMap.at(limitPrice)->printForward();
        std::cout << (buyOrSell ? "Buy " : "Sell ") << "limit backwards:" << std::endl;
        limitMap.at(limitPrice)->printBackward();
        limitMap.at(limitPrice)->print();
    } else
    {
        std::cout << "No " << (buyOrSell ? "buy " : "sell ") << "limit at " << limitPrice <<std::endl;
    }
}

void Book::printOrder(int orderId) const
{
    if (orderMap.find(orderId) != orderMap.end())
    {
        orderMap.at(orderId)->print();
    }
}

Order* Book::searchOrderMap(int orderId) const
{
    auto it = orderMap.find(orderId);
    if (it != orderMap.end())
    {
        return it->second;
    } else
    {
        std::cout << "No order number " << orderId << std::endl;
        return nullptr;
    }
}

Limit* Book::searchLimitBuyMap(int limitPrice) const
{
    auto it = limitBuyMap.find(limitPrice);
    if (it != limitBuyMap.end())
    {
        return it->second;
    } else
    {
        std::cout << "No buy limit at " << limitPrice << std::endl;
        return nullptr;
    }
}

Limit* Book::searchLimitSellMap(int limitPrice) const
{
    auto it = limitSellMap.find(limitPrice);
    if (it != limitSellMap.end())
    {
        return it->second;
    } else
    {
        std::cout << "No sell limit at " << limitPrice << std::endl;
        return nullptr;
    }
}