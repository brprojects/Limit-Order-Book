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

    Limit* newLimit = new Limit(limitPrice);
    limitMap.emplace(limitPrice, newLimit);
    Limit* root = insert(tree, newLimit);
    // newLimit->print();
    if (tree == nullptr)
    {
        tree = newLimit;
    } 
    // else
    // {
    //     std::cout << newLimit->parent->limitPrice << std::endl;
    // }
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

void Book::printLimit(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

    if (limitMap.find(limitPrice) != limitMap.end())
    {
        std::cout << "Limit forwards:" << std::endl;
        limitMap.at(limitPrice)->printForward();
        std::cout << "Limit backwards:" << std::endl;
        limitMap.at(limitPrice)->printBackward();
        limitMap.at(limitPrice)->print();
    }
}

void Book::printOrder(int orderId)
{
    if (orderMap.find(orderId) != orderMap.end())
    {
        orderMap.at(orderId)->print();
    }
}