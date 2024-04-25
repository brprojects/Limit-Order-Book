#include "../Limit.hpp"
#include "../Order.hpp"
#include "../Book.hpp"

#include <gtest/gtest.h>
#include <vector>

struct LimitOrderBookTests: public ::testing::Test
{
    Book* book;

    virtual void SetUp() override{
        book = new Book();
    }

    virtual void TearDown() override{
        delete book;
    }
};

TEST_F(LimitOrderBookTests, TestBookCreated) {
    EXPECT_NE(book, nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingAnOrder) {
    EXPECT_EQ(book->searchOrderMap(357), nullptr);
    EXPECT_EQ(book->searchLimitMaps(100, true), nullptr);

    book->addOrder(357, true, 27, 100);

    EXPECT_EQ(book->searchOrderMap(357)->getOrderSize(), 27);
    EXPECT_EQ(book->searchLimitMaps(100, true)->getTotalVolume(), 27);
    EXPECT_EQ(book->searchLimitMaps(20, false), nullptr);

    book->addOrder(222, false, 35, 100);

    EXPECT_EQ(book->searchLimitMaps(100, false)->getTotalVolume(), 35);
}

TEST_F(LimitOrderBookTests, TestMultipleOrdersInALimit){
    book->addOrder(5, true, 80, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 80);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 1);

    book->addOrder(6, true, 32, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 112);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 2);


    book->addOrder(7, true, 111, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 223);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 3);

}

TEST_F(LimitOrderBookTests, TestCancelOrderLeavingNonEmptyLimit){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 32, 20);
    book->addOrder(7, true, 111, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 3);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 223);

    book->cancelOrder(6);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 2);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 191);

    book->cancelOrder(7);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 1);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 80);
}

TEST_F(LimitOrderBookTests, TestLimitHeadOrderChangeOnOrderCancel){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 32, 20);
    book->addOrder(7, true, 111, 20);

    Limit* limit = book->searchLimitMaps(20, true);

    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 5);

    book->cancelOrder(5);
    
    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 6);
}

TEST_F(LimitOrderBookTests, TestCancelOrderLeavingEmptyLimit){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 6);
    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);

    book->cancelOrder(6);
    
    EXPECT_EQ(book->searchLimitMaps(15, true), nullptr);
    EXPECT_EQ(limit1->getLeftChild(), nullptr);
}

TEST_F(LimitOrderBookTests, TestCorrectLimitParent){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);
    Limit* limit3 = book->searchLimitMaps(25, true);

    EXPECT_EQ(limit1->getParent(), nullptr);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestCorrectLimitChildren){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit2->getRightChild(), nullptr);
}

TEST_F(LimitOrderBookTests, TestTreeHeightsCorrect){
    book->addOrder(5, true, 80, 20);
    Limit* limit1 = book->searchLimitMaps(20, true);

    EXPECT_EQ(limit1->getHeight(), 1);

    book->addOrder(6, true, 80, 15);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(limit2->getHeight(), 1);
    EXPECT_EQ(limit1->getHeight(), 2);

    book->addOrder(7, true, 80, 25);
    Limit* limit3 = book->searchLimitMaps(25, true);

    EXPECT_EQ(limit3->getHeight(), 1);
    EXPECT_EQ(limit1->getHeight(), 2);

    book->addOrder(8, true, 80, 10);
    Limit* limit4 = book->searchLimitMaps(10, true);

    EXPECT_EQ(limit4->getHeight(), 1);
    EXPECT_EQ(limit2->getHeight(), 2);
    EXPECT_EQ(limit1->getHeight(), 3);

    book->addOrder(9, true, 80, 5);
}

TEST_F(LimitOrderBookTests, TestBinarySearchTree){
    book->addOrder(5, false, 80, 20);
    book->addOrder(6, false, 80, 15);
    book->addOrder(7, false, 80, 25);
    book->addOrder(8, false, 80, 10);
    book->addOrder(9, false, 80, 19);

    std::vector<int> expectedInOrder = {10, 15, 19, 20, 25};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    expectedInOrder = {20, 15, 10, 19, 25};
    actualInOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    expectedInOrder = {10, 19, 15, 25, 20};
    actualInOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);
}

TEST_F(LimitOrderBookTests, TestEmptyingATree){

}

TEST_F(LimitOrderBookTests, TestBookEdgeLowestSell){

}

TEST_F(LimitOrderBookTests, TestBookEdgeHighestBuy){

}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithNoChildren){

}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithLeftChildOnly){

}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithRightChildOnly){

}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotate){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotate){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotate){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotate){

}