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

TEST_F(LimitOrderBookTests, TestRemoveLimitWithNoChildren){
    book->addOrder(5, false, 80, 20);
    book->addOrder(6, false, 80, 15);
    book->addOrder(7, false, 80, 25);
    book->addOrder(8, false, 80, 10);
    book->addOrder(9, false, 80, 19);

    Limit* limit = book->searchLimitMaps(15, false);

    EXPECT_EQ(limit->getRightChild()->getLimitPrice(), 19);

    book->cancelOrder(9);

    EXPECT_EQ(limit->getRightChild(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithLeftChildOnly){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(10, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);

    book->cancelOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithRightChildOnly){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 19);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(19, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);

    book->cancelOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 19);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 18);
    book->addOrder(10, true, 80, 23);
    book->addOrder(11, true, 80, 27);
    book->addOrder(12, true, 80, 17);
    book->addOrder(13, true, 80, 19);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(10, true);
    Limit* limit3 = book->searchLimitMaps(18, true);
    Limit* limit4 = book->searchLimitMaps(17, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 17);

    book->cancelOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 17);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 17);
    EXPECT_EQ(limit3->getLeftChild(), nullptr);
    EXPECT_EQ(limit4->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit4->getRightChild()->getLimitPrice(), 18);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren_RightChildHasNoLeftChild){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 18);
    book->addOrder(10, true, 80, 23);
    book->addOrder(11, true, 80, 27);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(23, true);
    Limit* limit3 = book->searchLimitMaps(27, true);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 25);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->cancelOrder(7);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 27);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 27);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestEmptyingATree){
    book->addOrder(5, true, 80, 20);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);

    book->cancelOrder(5);

    EXPECT_EQ(book->getBuyTree(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithLeftChildOnly){
    book->addOrder(5, false, 80, 20);
    book->addOrder(6, false, 80, 15);

    Limit* limit = book->searchLimitMaps(15, false);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit->getParent()->getLimitPrice(), 20);

    book->cancelOrder(5);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 15);
    EXPECT_EQ(limit->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithRightChildOnly){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 25);

    Limit* limit = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit->getParent()->getLimitPrice(), 20);

    book->cancelOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 25);
    EXPECT_EQ(limit->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 27);
    book->addOrder(9, true, 80, 22);

    Limit* limit1 = book->searchLimitMaps(15, true);
    Limit* limit2 = book->searchLimitMaps(25, true);
    Limit* limit3 = book->searchLimitMaps(22, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 22);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->cancelOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 22);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 22);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 22);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit3->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren_RightChildHasNoLeftChild){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 27);

    Limit* limit1 = book->searchLimitMaps(15, true);
    Limit* limit2 = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);

    book->cancelOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 25);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent(), nullptr);
}





TEST_F(LimitOrderBookTests, TestBookEdgeLowestSell){

}

TEST_F(LimitOrderBookTests, TestBookEdgeHighestBuy){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateOnInsert){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateOnInsert){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateOnInsert){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateOnInsert){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateOnDelete){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateOnDelete){

}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateOnDelete){

}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateOnDelete){

}