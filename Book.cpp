#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>

Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr), 
            stopBuyTree(nullptr), stopSellTree(nullptr), highestStopSell(nullptr), lowestStopBuy(nullptr){}

// When deleting the book need to ensure all used memory is freed
Book::~Book()
{
    for (auto& [id, order] : orderMap) {
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

    for (auto& [stopPrice, stopLevel] : stopMap) {
        delete stopLevel;
    }
    stopMap.clear();
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

Limit* Book::getStopBuyTree() const
{
    return stopBuyTree;
}

Limit* Book::getStopSellTree() const
{
    return stopSellTree;
}

Limit* Book::getHighestStopSell() const
{
    return highestStopSell;
}

Limit* Book::getLowestStopBuy() const
{
    return lowestStopBuy;
}

// Execute a market order
void Book::marketOrder(int orderId, bool buyOrSell, int shares)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    marketOrderHelper(orderId, buyOrSell, shares);

    executeStopOrders(buyOrSell);
}

// Add a new limit order to the book
void Book::addLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    AVLTreeBalanceCount = 0;
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
        // limitOrders.insert(newOrder);
    } else {
        executeStopOrders(buyOrSell);
    }
}

// Delete a limit order from the book
void Book::cancelLimitOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {   
                deleteLimit(order->getParentLimit());
            }
        deleteFromOrderMap(orderId);
        // limitOrders.erase(order);
        delete order;
    }
}

// Modify an existing limit order
void Book::modifyLimitOrder(int orderId, int newShares, int newLimit)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
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
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    // Account for stop order being executed immediately
    shares = stopOrderAsMarketOrder(orderId, buyOrSell, shares, stopPrice);
    
    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, 0);
        orderMap.emplace(orderId, newOrder);

        if (stopMap.find(stopPrice) == stopMap.end())
        {
            addStop(stopPrice, newOrder->getBuyOrSell());
        }
        stopMap.at(stopPrice)->append(newOrder);
        // stopOrders.insert(newOrder);
    }
}

// Delete an stop order from the stop book
void Book::cancelStopOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {   
                deleteStopLevel(order->getParentLimit());
            }
        deleteFromOrderMap(orderId);
        // stopOrders.erase(order);
        delete order;
    }
}

// Modify an existing stop order
void Book::modifyStopOrder(int orderId, int newShares, int newStopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {
                deleteStopLevel(order->getParentLimit());
            }
        
        order->modifyOrder(newShares, 0);

        if (stopMap.find(newStopPrice) == stopMap.end())
        {
            addStop(newStopPrice, order->getBuyOrSell());
        }
        stopMap.at(newStopPrice)->append(order);
    }
}

// Add a stop limit order
void Book::addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    // Account for stop limit order being executed immediately
    shares = stopLimitOrderAsLimitOrder(orderId, buyOrSell, shares, limitPrice, stopPrice);
    
    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
        orderMap.emplace(orderId, newOrder);

        if (stopMap.find(stopPrice) == stopMap.end())
        {
            addStop(stopPrice, newOrder->getBuyOrSell());
        }
        stopMap.at(stopPrice)->append(newOrder);
        // stopLimitOrders.insert(newOrder);
    }
}

void Book::cancelStopLimitOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {   
                deleteStopLevel(order->getParentLimit());
            }
        deleteFromOrderMap(orderId);
        // stopLimitOrders.erase(order);
        delete order;
    }
}

// Modify an existing stop limit order
void Book::modifyStopLimitOrder(int orderId, int newShares, int newLimitPrice, int newStopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
            if (order->getParentLimit()->getSize() == 0)
            {
                deleteStopLevel(order->getParentLimit());
            }
        
        order->modifyOrder(newShares, newLimitPrice);

        if (stopMap.find(newStopPrice) == stopMap.end())
        {
            addStop(newStopPrice, order->getBuyOrSell());
        }
        stopMap.at(newStopPrice)->append(order);
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

// Print out all the limit and stop levels and their liquidity
void Book::printOrderBook() const
{
    std::vector<int> vec = inOrderTreeTraversal(getStopBuyTree());
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << "-" << searchStopMap(vec[i])->getTotalVolume();
        if (i != 0 && i != vec.size()-1 && vec[i] < vec[i-1]) {
            throw std::runtime_error("Error: vector is error");
        }
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;

    vec = inOrderTreeTraversal(getStopSellTree());
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << "-" << searchStopMap(vec[i])->getTotalVolume();
        if (i != 0 && i != vec.size()-1 && vec[i] < vec[i-1]) {
            throw std::runtime_error("Error: Vector is error");
        }
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;

    vec = inOrderTreeTraversal(getBuyTree());
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << "-" << searchLimitMaps(vec[i], true)->getTotalVolume();
        if (i != 0 && i != vec.size()-1 && vec[i] < vec[i-1]) {
            throw std::runtime_error("Error: vector is error");
        }
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;

    vec = inOrderTreeTraversal(getSellTree());
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << "-" << searchLimitMaps(vec[i], false)->getTotalVolume();
        if (i != 0 && i != vec.size()-1 && vec[i] < vec[i-1]) {
            throw std::runtime_error("Error: Vector is error");
        }
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

// In order traversal of the binary search tree
std::vector<int> Book::inOrderTreeTraversal(Limit* root) const
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
std::vector<int> Book::preOrderTreeTraversal(Limit* root) const
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
std::vector<int> Book::postOrderTreeTraversal(Limit* root) const
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
    auto& bookEdge = buyOrSell ? lowestStopBuy : highestStopSell;

    Limit* newStop = new Limit(stopPrice, buyOrSell);
    stopMap.emplace(stopPrice, newStop);

    if (tree == nullptr)
    {
        tree = newStop;
        bookEdge = newStop;
    } else
    {
        Limit* root = insertStop(tree, newStop);
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

// Insert a limit into its stop binary search tree
Limit* Book::insertStop(Limit* root, Limit* limit, Limit* parent)
{
    if (root == nullptr)
    {
        limit->setParent(parent);
        return limit;
    }
    if (limit->getLimitPrice() < root->getLimitPrice())
    {
        root->setLeftChild(insertStop(root->getLeftChild(), limit, root));
        root = balanceStop(root);
    } else if (limit->getLimitPrice() > root->getLimitPrice())
    {
        root->setRightChild(insertStop(root->getRightChild(), limit, root));
        root = balanceStop(root);
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
        if (newStop->getLimitPrice() < lowestStopBuy->getLimitPrice())
        {
            lowestStopBuy = newStop;
        }
    } else
    {
        if (newStop->getLimitPrice() > highestStopSell->getLimitPrice())
        {
            highestStopSell = newStop;
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

// Update the edge of the stop book if current edge of the stop book is emptied
void Book::updateStopBookEdgeRemove(Limit* stopLevel)
{
    auto& bookEdge = stopLevel->getBuyOrSell() ? lowestStopBuy : highestStopSell;
    auto& tree = stopLevel->getBuyOrSell() ? stopBuyTree : stopSellTree;
    
    if (stopLevel == bookEdge)
    {
        if (bookEdge != tree)
        {
            if (stopLevel->getBuyOrSell() && bookEdge->getRightChild() != nullptr)
            {
                bookEdge = bookEdge->getRightChild();
            } else if (!stopLevel->getBuyOrSell() && bookEdge->getLeftChild() != nullptr)
            {
                bookEdge = bookEdge->getLeftChild();
            } else {
            bookEdge = bookEdge->getParent();
            }
        } else {
            if (stopLevel->getBuyOrSell() && bookEdge->getRightChild() != nullptr)
            {
                bookEdge = bookEdge->getRightChild();
            } else if (!stopLevel->getBuyOrSell() && bookEdge->getLeftChild() != nullptr)
            {
                bookEdge = bookEdge->getLeftChild();
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

// Change the root stop level in the AVL tree if the root stop level is deleted
void Book::changeStopBookRoots(Limit* stopLevel){
    auto& tree = stopLevel->getBuyOrSell() ? stopBuyTree : stopSellTree;
    if (stopLevel == tree)
    {
        if (stopLevel->getRightChild() != nullptr)
        {
            tree = tree->getRightChild();
            while (tree->getLeftChild() != nullptr)
            {
                tree = tree->getLeftChild();
            }
        } else
        {
            tree = stopLevel->getLeftChild();
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

// Delete a stop level after it has been emptied
void Book::deleteStopLevel(Limit* stopLevel)
{
    updateStopBookEdgeRemove(stopLevel);
    deleteFromStopMap(stopLevel->getLimitPrice());
    changeStopBookRoots(stopLevel);

    Limit* parent = stopLevel->getParent();
    int stopPrice = stopLevel->getLimitPrice();
    delete stopLevel;
    while (parent != nullptr)
    {
        parent = balanceStop(parent);
        if (parent->getParent() != nullptr)
        {
            if (parent->getParent()->getLimitPrice() > stopPrice)
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

// Delete a stop level from the stop map
void Book::deleteFromStopMap(int stopPrice)
{
    stopMap.erase(stopPrice);
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
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= lowestSell->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, lowestSell->getTotalVolume());
            }
        }
        return shares;
    } else {
        while (highestBuy != nullptr && shares != 0 && highestBuy->getLimitPrice() >= limitPrice)
        {
            if (shares <= highestBuy->getTotalVolume())
            {
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= highestBuy->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, highestBuy->getTotalVolume());
            }
        }
        return shares;
    }
}

// When a stop order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a market order
int Book::stopOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int stopPrice)
{
    if (buyOrSell && lowestSell != nullptr && stopPrice <= lowestSell->getLimitPrice())
    {
        marketOrder(orderId, true, shares);
        return 0;
    } else if (!buyOrSell && highestBuy != nullptr && stopPrice >= highestBuy->getLimitPrice())
    {
        marketOrder(orderId, false, shares);
        return 0;
    }
    return shares;
}

// When a limit order that used to be a stop limit order overlaps with the highest buy or lowest sell, 
// immediately execute it as if it were a market order
int Book::existingOrderAsMarketOrder(Order* headOrder, bool buyOrSell)
{
    int shares = headOrder->getShares();
    int orderId = headOrder->getOrderId();
    int limitPrice = headOrder->getLimit();
    
    if (buyOrSell)
    {
        while (lowestSell != nullptr && lowestSell->getLimitPrice() <= limitPrice)
        {
            if (shares <= lowestSell->getTotalVolume())
            {
                deleteFromOrderMap(orderId);
                delete headOrder;
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= lowestSell->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, lowestSell->getTotalVolume());
            }
        }
        return shares;
    } else {
        while (highestBuy != nullptr && highestBuy->getLimitPrice() >= limitPrice)
        {
            if (shares <= highestBuy->getTotalVolume())
            {
                deleteFromOrderMap(orderId);
                delete headOrder;
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            } else {
                shares -= highestBuy->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, highestBuy->getTotalVolume());
            }
        }
        return shares;
    }
}

// When a stop limit order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a limit order
int Book::stopLimitOrderAsLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice)
{
    if (buyOrSell && lowestSell != nullptr && stopPrice <= lowestSell->getLimitPrice())
    {
        addLimitOrder(orderId, true, shares, limitPrice);
        return 0;
    } else if (!buyOrSell && highestBuy != nullptr && stopPrice >= highestBuy->getLimitPrice())
    {
        addLimitOrder(orderId, false, shares, limitPrice);
        return 0;
    }
    return shares;
}

// Executes any stop orders which need to be executed
void Book::executeStopOrders(bool buyOrSell)
{
    if (buyOrSell)
    {
        // Execute any buy stop market orders
        // If the book is empty and can't complete stop market order then it just doesn't execute and is forgotten.
        while (lowestStopBuy != nullptr && (lowestSell == nullptr || lowestStopBuy->getLimitPrice() <= lowestSell->getLimitPrice()))
        {
            Order* headOrder = lowestStopBuy->getHeadOrder();
            if (headOrder->getLimit() == 0)
            {
                int shares = headOrder->getShares();
                headOrder->execute();
                if (lowestStopBuy->getSize() == 0)
                {
                    deleteStopLevel(lowestStopBuy);
                }
                deleteFromOrderMap(headOrder->getOrderId());
                // stopOrders.erase(headOrder);
                delete headOrder;
                marketOrderHelper(0, true, shares);
            } else {
                // stopLimitOrders.erase(headOrder);
                stopLimitOrderToLimitOrder(headOrder, buyOrSell);
            }
        }
    } else {
        // Execute any sell stop market orders
        // If the book is empty and can't complete stop market order then it just doesn't execute and is forgotten.
        while (highestStopSell != nullptr && (highestBuy == nullptr || highestStopSell->getLimitPrice() >= highestBuy->getLimitPrice()))
        {
            Order* headOrder = highestStopSell->getHeadOrder();
            if (headOrder->getLimit() == 0)
            {
                int shares = headOrder->getShares();
                headOrder->execute();
                if (highestStopSell->getSize() == 0)
                {
                    deleteStopLevel(highestStopSell);
                }
                deleteFromOrderMap(headOrder->getOrderId());
                // stopOrders.erase(headOrder);
                delete headOrder;
                marketOrderHelper(0, false, shares);
            } else {
                // stopLimitOrders.erase(headOrder);
                stopLimitOrderToLimitOrder(headOrder, buyOrSell);
            }
        }
    }
}

// Turn stop limit order into limit order
void Book::stopLimitOrderToLimitOrder(Order* headOrder, bool buyOrSell)
{
    auto& bookEdge = buyOrSell ? lowestStopBuy : highestStopSell;
    headOrder->execute();
    if (bookEdge->getSize() == 0)
    {
        deleteStopLevel(bookEdge);
    }

    // Account for order being executed immediately - majority of cases
    int shares = existingOrderAsMarketOrder(headOrder, buyOrSell);
    
    if (shares != 0)
    {
        headOrder->setShares(shares);
        auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

        if (limitMap.find(headOrder->getLimit()) == limitMap.end())
        {
            addLimit(headOrder->getLimit(), buyOrSell);
        }
        limitMap.at(headOrder->getLimit())->append(headOrder);
        // limitOrders.insert(headOrder);
    }
}

// Function which actually executes the market order.
// If the book is empty and can't complete market order then market order just doesn't execute and is forgotten
void Book::marketOrderHelper(int orderId, bool buyOrSell, int shares)
{
    auto& bookEdge = buyOrSell ? lowestSell : highestBuy;

    while (bookEdge != nullptr && bookEdge->getHeadOrder()->getShares() <= shares)
    {
        Order* headOrder = bookEdge->getHeadOrder();
        shares -= headOrder->getShares();
        headOrder->execute();
        if (bookEdge->getSize() == 0)
        {
            deleteLimit(bookEdge);
        }
        deleteFromOrderMap(headOrder->getOrderId());
        // limitOrders.erase(headOrder);
        delete headOrder;
        executedOrdersCount += 1;
    }
    if (bookEdge != nullptr && shares != 0)
    {
        bookEdge->getHeadOrder()->partiallyFillOrder(shares);
        executedOrdersCount += 1;
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
        AVLTreeBalanceCount += 1;
    } else if (bal_factor < -1) {
        if (limitHeightDifference(limit->getRightChild()) > 0)
            limit = rl_rotate(limit);
        else
            limit = rr_rotate(limit);
        AVLTreeBalanceCount += 1;
    }
    return limit;
}

// RR rotation for AVL stop tree restructure
Limit* Book::rr_rotateStop(Limit* parent) {
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
        auto& tree = parent->getBuyOrSell() ? stopBuyTree : stopSellTree;
        tree = newParent;
    }
    parent->setParent(newParent);
    return newParent;
}

// LL rotation for AVL stop tree restructure
Limit* Book::ll_rotateStop(Limit* parent) {
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
        auto& tree = parent->getBuyOrSell() ? stopBuyTree : stopSellTree;
        tree = newParent;
    }
    parent->setParent(newParent);
    return newParent;
}

// LR rotation for AVL stop tree restructure
Limit* Book::lr_rotateStop(Limit* parent) {
    Limit* newParent = parent->getLeftChild();
    parent->setLeftChild(rr_rotateStop(newParent));
    return ll_rotateStop(parent);
}

// RL rotation for AVL stop tree restructure
Limit* Book::rl_rotateStop(Limit* parent) {
    Limit* newParent = parent->getRightChild();
    parent->setRightChild(ll_rotateStop(newParent));
    return rr_rotateStop(parent);
}

// Check if the AVL stop tree needs to be restructured
Limit* Book::balanceStop(Limit* limit) {
    int bal_factor = limitHeightDifference(limit);
    if (bal_factor > 1) {
        if (limitHeightDifference(limit->getLeftChild()) >= 0)
            limit = ll_rotateStop(limit);
        else
            limit = lr_rotateStop(limit);
        AVLTreeBalanceCount += 1;
    } else if (bal_factor < -1) {
        if (limitHeightDifference(limit->getRightChild()) > 0)
            limit = rl_rotateStop(limit);
        else
            limit = rr_rotateStop(limit);
        AVLTreeBalanceCount += 1;
    }
    return limit;
}
