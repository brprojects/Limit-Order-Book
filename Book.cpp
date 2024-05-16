#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>
#include <algorithm>
#include <random>

Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr), 
            stopBuyTree(nullptr), stopSellTree(nullptr), lowestStopSell(nullptr), highestStopBuy(nullptr){}

// When deleting the book need to ensure all used memory is freed
Book::~Book()
{
    for (auto& [id, order] : orderMap) {
        // order->print();
        delete order;
    }
    orderMap.clear();

    for (auto& [limitPrice, limit] : limitBuyMap) {
        // std::cout << limit->getLimitPrice() << std::endl;
        delete limit;
    }
    limitBuyMap.clear();

    for (auto& [limitPrice, limit] : limitSellMap) {
        delete limit;
    }
    limitSellMap.clear();
}

Limit* Book::getBuyTree() const
{
    return buyTree;
}

Limit* Book::getSellTree() const
{
    return sellTree;
}

Limit* Book::getLowestSell() const
{
    return lowestSell;
}

Limit* Book::getHighestBuy() const
{
    return highestBuy;
}

// Execute a market order. If the book is empty and can't complete market 
// order then market order just doesn't execute and is forgotten.
void Book::marketOrder(int orderId, bool buyOrSell, int shares)
{
    auto& bookEdge = buyOrSell ? lowestSell : highestBuy;

    while (bookEdge != nullptr && bookEdge->getHeadOrder()->getShares() <= shares)
    {
        shares -= bookEdge->getHeadOrder()->getShares();
        Order* headOrder = bookEdge->getHeadOrder();
        headOrder->cancel();
        if (bookEdge->getSize() == 0)
        {
            deleteLimit(bookEdge);
        }
        deleteFromOrderMap(headOrder->getOrderId());
        delete headOrder;
    }
    if (bookEdge != nullptr && shares != 0)
    {
        bookEdge->getHeadOrder()->partiallyFillOrder(shares);
    }
}

// Add a new order to the book
void Book::addOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    // Account for order being executed immediately
    shares = limitOrderAsMarketOrder(orderId, buyOrSell, shares, limitPrice);
    
    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
        orderMap.emplace(orderId, newOrder);

        auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

        if (limitMap.find(limitPrice) == limitMap.end())
        {
            addLimit(limitPrice, newOrder->getBuyOrSell());
        }
        limitMap.at(limitPrice)->append(newOrder);
    }
}

// Delete an order from the book
void Book::cancelOrder(int orderId)
{
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {   
                deleteLimit(order->getParentLimit());
            }
        deleteFromOrderMap(orderId);
        delete order;
    }
}

// Modify an existing order
void Book::modifyOrder(int orderId, int newShares, int newLimit)
{
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {
                deleteLimit(order->getParentLimit());
            }
        
        order->modifyOrder(newShares, newLimit);
        auto& limitMap = order->getBuyOrSell() ? limitBuyMap : limitSellMap;

        if (limitMap.find(newLimit) == limitMap.end())
        {
            addLimit(newLimit, order->getBuyOrSell());
        }
        limitMap.at(newLimit)->append(order);
    }
}

// Add a stop order
void Book::addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice)
{
    // Account for order being executed immediately
    // shares = limitOrderAsMarketOrder(orderId, buyOrSell, shares, limitPrice);
    
    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, 0);
        orderMap.emplace(orderId, newOrder);

        if (stopMap.find(stopPrice) == stopMap.end())
        {
            addStop(stopPrice, newOrder->getBuyOrSell());
        }
        stopMap.at(stopPrice)->append(newOrder);
    }
}

// Get the height of a limit in a binary tree
int Book::getLimitHeight(Limit* limit) const {
    if (limit == nullptr) {
        return 0; // Height of an empty tree is 0
    } else {
        int l_height = getLimitHeight(limit->getLeftChild());
        int r_height = getLimitHeight(limit->getRightChild());
        int max = std::max(l_height, r_height) + 1;
        return max;
    }
}

// Search the order map to find an order
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

// Search the limit maps to find a limit
Limit* Book::searchLimitMaps(int limitPrice, bool buyOrSell) const
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

    auto it = limitMap.find(limitPrice);
    if (it != limitMap.end())
    {
        return it->second;
    } else
    {
        std::cout << "No "<< (buyOrSell ? "buy " : "sell ") << "limit at " << limitPrice << std::endl;
        return nullptr;
    }
}

// Search the stop map to find a stop level
Limit* Book::searchStopMap(int stopPrice) const
{
    auto it = stopMap.find(stopPrice);
    if (it != stopMap.end())
    {
        return it->second;
    } else
    {
        std::cout << "No stop level at " << stopPrice << std::endl;
        return nullptr;
    }
}

Order* Book::getRandomOrder(std::mt19937 gen) const
{
    if (orderMap.size() > 1000)
    {
    // Generate a random index within the range of the hash map size
    std::uniform_int_distribution<> mapDist(0, orderMap.size() - 1);
    int randomIndex = mapDist(gen);

    // Access the element at the random index directly
    auto it = std::next(orderMap.begin(), randomIndex);
    return it->second;
    }
    return nullptr;
    
}

void Book::printLimit(int limitPrice, bool buyOrSell) const
{
    searchLimitMaps(limitPrice, buyOrSell)->print();
}

void Book::printOrder(int orderId) const
{
    searchOrderMap(orderId)->print();
}

void Book::printBookEdges() const
{
    std::cout << "Buy edge: " << highestBuy->getLimitPrice() 
    << "Sell edge: " << lowestSell->getLimitPrice() << std::endl;
}

// In order traversal of the binary search tree
std::vector<int> Book::inOrderTreeTraversal(Limit* root)
{
    std::vector<int> result;
    if (root == nullptr)
        return result;

    std::vector<int> leftSubtree = inOrderTreeTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());
    
    result.push_back(root->getLimitPrice());

    std::vector<int> rightSubtree = inOrderTreeTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

    return result;
}

// Pre order traversal of the binary search tree
std::vector<int> Book::preOrderTreeTraversal(Limit* root)
{
    std::vector<int> result;
    if (root == nullptr)
        return result;

    result.push_back(root->getLimitPrice());

    std::vector<int> leftSubtree = preOrderTreeTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());
    
    std::vector<int> rightSubtree = preOrderTreeTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

    return result;
}

// Post order traversal of the binary search tree
std::vector<int> Book::postOrderTreeTraversal(Limit* root)
{
    std::vector<int> result;
    if (root == nullptr)
        return result;

    std::vector<int> leftSubtree = postOrderTreeTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());
    
    std::vector<int> rightSubtree = postOrderTreeTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());
    
    result.push_back(root->getLimitPrice());

    return result;
}

// Add a new limit to the book
void Book::addLimit(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;
    auto& tree = buyOrSell ? buyTree : sellTree;
    auto& bookEdge = buyOrSell ? highestBuy : lowestSell;

    Limit* newLimit = new Limit(limitPrice, buyOrSell);
    limitMap.emplace(limitPrice, newLimit);

    if (tree == nullptr)
    {
        tree = newLimit;
        bookEdge = newLimit;
    } else
    {
        Limit* root = insert(tree, newLimit);
        updateBookEdgeInsert(newLimit);
    }
}

// Add a new stop level to the book
void Book::addStop(int stopPrice, bool buyOrSell)
{
    auto& tree = buyOrSell ? stopBuyTree : stopSellTree;
    auto& bookEdge = buyOrSell ? highestStopBuy : lowestStopSell;

    Limit* newStop = new Limit(stopPrice, buyOrSell);
    stopMap.emplace(stopPrice, newStop);

    if (tree == nullptr)
    {
        tree = newStop;
        bookEdge = newStop;
    } else
    {
        Limit* root = insert(tree, newStop);
        updateStopBookEdgeInsert(newStop);
    }
}

// Insert a limit into its binary search tree
Limit* Book::insert(Limit* root, Limit* limit, Limit* parent)
{
    if (root == nullptr)
    {
        limit->setParent(parent);
        return limit;
    }
    if (limit->getLimitPrice() < root->getLimitPrice())
    {
        root->setLeftChild(insert(root->getLeftChild(), limit, root));
        root = balance(root);
    } else if (limit->getLimitPrice() > root->getLimitPrice())
    {
        root->setRightChild(insert(root->getRightChild(), limit, root));
        root = balance(root);
    }

    return root;
}

// Update the edge of the book if new limit is on edge of the book
void Book::updateBookEdgeInsert(Limit* newLimit)
{
    if (newLimit->getBuyOrSell())
    {
        if (newLimit->getLimitPrice() > highestBuy->getLimitPrice())
        {
            highestBuy = newLimit;
        }
    } else
    {
        if (newLimit->getLimitPrice() < lowestSell->getLimitPrice())
        {
            lowestSell = newLimit;
        }
    }
}

// Update the edge of the stop book if new stop is on edge of the book
void Book::updateStopBookEdgeInsert(Limit* newStop)
{
    if (newStop->getBuyOrSell())
    {
        if (newStop->getLimitPrice() > highestStopBuy->getLimitPrice())
        {
            highestStopBuy = newStop;
        }
    } else
    {
        if (newStop->getLimitPrice() < lowestStopSell->getLimitPrice())
        {
            lowestStopSell = newStop;
        }
    }
}

// Update the edge of the book if current edge of the book is emptied
void Book::updateBookEdgeRemove(Limit* limit)
{
    auto& bookEdge = limit->getBuyOrSell() ? highestBuy : lowestSell;
    auto& tree = limit->getBuyOrSell() ? buyTree : sellTree;

    if (limit == bookEdge)
    {
        if (bookEdge != tree)
        {
            if (limit->getBuyOrSell() && bookEdge->getLeftChild() != nullptr)
            {
                bookEdge = bookEdge->getLeftChild();
            } else if (!limit->getBuyOrSell() && bookEdge->getRightChild() != nullptr)
            {
                bookEdge = bookEdge->getRightChild();
            } else {
            bookEdge = bookEdge->getParent();
            }
        } else {
            if (limit->getBuyOrSell() && bookEdge->getLeftChild() != nullptr)
            {
                bookEdge = bookEdge->getLeftChild();
            } else if (!limit->getBuyOrSell() && bookEdge->getRightChild() != nullptr)
            {
                bookEdge = bookEdge->getRightChild();
            } else {
            bookEdge = nullptr;
            }
        }
    }
}

// Change the root limit in the AVL tree if the root limit is deleted
void Book::changeBookRoots(Limit* limit){
    auto& tree = limit->getBuyOrSell() ? buyTree : sellTree;
    if (limit == tree)
    {
        if (limit->getRightChild() != nullptr)
        {
            tree = tree->getRightChild();
            while (tree->getLeftChild() != nullptr)
            {
                tree = tree->getLeftChild();
            }
        } else
        {
            tree = limit->getLeftChild();
        }
    }
}

// Delete a limit after it has been emptied
void Book::deleteLimit(Limit* limit)
{
    updateBookEdgeRemove(limit);
    deleteFromLimitMaps(limit->getLimitPrice(), limit->getBuyOrSell());
    changeBookRoots(limit);

    Limit* parent = limit->getParent();
    int limitPrice = limit->getLimitPrice();
    delete limit;
    while (parent != nullptr)
    {
        parent = balance(parent);
        if (parent->getParent() != nullptr)
        {
            if (parent->getParent()->getLimitPrice() > limitPrice)
            {
                parent->getParent()->setLeftChild(parent);
            } else {
                parent->getParent()->setRightChild(parent);
            }
        }
        parent = parent->getParent();
    }
}

// Delete an order from the order map
void Book::deleteFromOrderMap(int orderId)
{
    orderMap.erase(orderId);
}

// Delete a limit from the limit maps
void Book::deleteFromLimitMaps(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;
    limitMap.erase(limitPrice);
}

// When a limit order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a market order
int Book::limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    if (buyOrSell)
    {
        while (lowestSell != nullptr && shares != 0 && lowestSell->getLimitPrice() <= limitPrice)
        {
            if (shares <= lowestSell->getTotalVolume())
            {
                marketOrder(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= lowestSell->getTotalVolume();
                marketOrder(orderId, buyOrSell, lowestSell->getTotalVolume());
            }
        }
        return shares;
    } else {
        while (highestBuy != nullptr && shares != 0 && highestBuy->getLimitPrice() >= limitPrice)
        {
            if (shares <= highestBuy->getTotalVolume())
            {
                marketOrder(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= highestBuy->getTotalVolume();
                marketOrder(orderId, buyOrSell, highestBuy->getTotalVolume());
            }
        }
        return shares;
    }
}

// Get height difference between a limits children
int Book::limitHeightDifference(Limit* limit) {
    int l_height = getLimitHeight(limit->getLeftChild());
    int r_height = getLimitHeight(limit->getRightChild());
    int b_factor = l_height - r_height;
    return b_factor;
}

// RR rotation for AVL restructure
Limit* Book::rr_rotate(Limit* parent) {
    Limit* newParent = parent->getRightChild();
    parent->setRightChild(newParent->getLeftChild());
    if (newParent->getLeftChild() != nullptr)
    {
        newParent->getLeftChild()->setParent(parent);
    }
    newParent->setLeftChild(parent);
    if (parent->getParent() != nullptr)
    {
        newParent->setParent(parent->getParent());
    } else {
        newParent->setParent(nullptr);
        auto& tree = parent->getBuyOrSell() ? buyTree : sellTree;
        tree = newParent;
    }
    parent->setParent(newParent);
    return newParent;
}

// LL rotation for AVL restructure
Limit* Book::ll_rotate(Limit* parent) {
    Limit* newParent = parent->getLeftChild();
    parent->setLeftChild(newParent->getRightChild());
    if (newParent->getRightChild() != nullptr)
    {
        newParent->getRightChild()->setParent(parent);
    }
    newParent->setRightChild(parent);
    if (parent->getParent() != nullptr)
    {
        newParent->setParent(parent->getParent());
    } else {
        newParent->setParent(nullptr);
        auto& tree = parent->getBuyOrSell() ? buyTree : sellTree;
        tree = newParent;
    }
    parent->setParent(newParent);
    return newParent;
}

// LR rotation for AVL restructure
Limit* Book::lr_rotate(Limit* parent) {
    Limit* newParent = parent->getLeftChild();
    parent->setLeftChild(rr_rotate(newParent));
    return ll_rotate(parent);
}

// RL rotation for AVL restructure
Limit* Book::rl_rotate(Limit* parent) {
    Limit* newParent = parent->getRightChild();
    parent->setRightChild(ll_rotate(newParent));
    return rr_rotate(parent);
}

// Check if the AVL tree needs to be restructured
Limit* Book::balance(Limit* limit) {
    int bal_factor = limitHeightDifference(limit);
    if (bal_factor > 1) {
        if (limitHeightDifference(limit->getLeftChild()) >= 0)
            limit = ll_rotate(limit);
        else
            limit = lr_rotate(limit);
    } else if (bal_factor < -1) {
        if (limitHeightDifference(limit->getRightChild()) > 0)
            limit = rl_rotate(limit);
        else
            limit = rr_rotate(limit);
    }
    return limit;
}