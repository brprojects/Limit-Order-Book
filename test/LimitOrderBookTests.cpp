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

// Adding orders tests
TEST_F(LimitOrderBookTests, TestAddingAnOrder) {
    EXPECT_EQ(book->searchOrderMap(357), nullptr);
    EXPECT_EQ(book->searchLimitMaps(100, true), nullptr);

    book->addOrder(357, true, 27, 100);

    EXPECT_EQ(book->searchOrderMap(357)->getShares(), 27);
    EXPECT_EQ(book->searchLimitMaps(100, true)->getTotalVolume(), 27);
    EXPECT_EQ(book->searchLimitMaps(20, false), nullptr);

    book->addOrder(222, false, 35, 110);

    EXPECT_EQ(book->searchLimitMaps(110, false)->getTotalVolume(), 35);
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

// Cancelling orders tests
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

TEST_F(LimitOrderBookTests, TestLimitHeadOrderChangeOnOrderCancelLeavingEmptyLimit){
    book->addOrder(5, true, 80, 20);

    Limit* limit = book->searchLimitMaps(20, true);

    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 5);

    book->cancelOrder(5);
    
    EXPECT_EQ(limit->getHeadOrder(), nullptr);
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

// Adding to BST tests
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

    EXPECT_EQ(book->getLimitHeight(limit1), 1);

    book->addOrder(6, true, 80, 15);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(book->getLimitHeight(limit2), 1);
    EXPECT_EQ(book->getLimitHeight(limit1), 2);

    book->addOrder(7, true, 80, 25);
    Limit* limit3 = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getLimitHeight(limit3), 1);
    EXPECT_EQ(book->getLimitHeight(limit1), 2);

    book->addOrder(8, true, 80, 10);
    Limit* limit4 = book->searchLimitMaps(10, true);

    EXPECT_EQ(book->getLimitHeight(limit4), 1);
    EXPECT_EQ(book->getLimitHeight(limit2), 2);
    EXPECT_EQ(book->getLimitHeight(limit1), 3);

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

    std::vector<int> expectedPreOrder = {20, 15, 10, 19, 25};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {10, 19, 15, 25, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

// Remove from BST tests
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

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren_RightChildHasLeftChildWithRightChild){
    book->addOrder(3, true, 80, 224);
    book->addOrder(4, true, 80, 220);
    book->addOrder(5, true, 80, 228);
    book->addOrder(6, true, 80, 218);
    book->addOrder(7, true, 80, 221);
    book->addOrder(8, true, 80, 226);
    book->addOrder(9, true, 80, 231);
    book->addOrder(10, true, 80, 217);
    book->addOrder(11, true, 80, 225);
    book->addOrder(12, true, 80, 229);
    book->addOrder(13, true, 80, 233);
    book->addOrder(14, true, 80, 230);

    std::vector<int> expectedInOrder = {217, 218, 220, 221, 224, 225, 226, 228, 229, 230, 231, 233};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {224, 220, 218, 217, 221, 228, 226, 225, 231, 229, 230, 233};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {217, 218, 221, 220, 225, 226, 230, 229, 233, 231, 228, 224};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelOrder(5);

    expectedInOrder = {217, 218, 220, 221, 224, 225, 226, 229, 230, 231, 233};
    actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    expectedPreOrder = {224, 220, 218, 217, 221, 229, 226, 225, 231, 230, 233};
    actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    expectedPostOrder = {217, 218, 221, 220, 225, 226, 230, 233, 231, 229, 224};
    actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren_RightChildHasLeftChildWithRightChild2){
    book->addOrder(3, true, 80, 250);
    book->addOrder(4, true, 80, 255);
    book->addOrder(5, true, 80, 228);
    book->addOrder(6, true, 80, 251);
    book->addOrder(7, true, 80, 260);
    book->addOrder(8, true, 80, 226);
    book->addOrder(9, true, 80, 231);
    book->addOrder(10, true, 80, 265);
    book->addOrder(11, true, 80, 225);
    book->addOrder(12, true, 80, 229);
    book->addOrder(13, true, 80, 233);
    book->addOrder(14, true, 80, 230);

    std::vector<int> expectedInOrder = {225, 226, 228, 229, 230, 231, 233, 250, 251, 255, 260, 265};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {250, 228, 226, 225, 231, 229, 230, 233, 255, 251, 260, 265};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {225, 226, 230, 229, 233, 231, 228, 251, 265, 260, 255, 250};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelOrder(5);

    expectedInOrder = {225, 226, 229, 230, 231, 233, 250, 251, 255, 260, 265};
    actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    expectedPreOrder = {250, 229, 226, 225, 231, 230, 233, 255, 251, 260, 265};
    actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    expectedPostOrder = {225, 226, 230, 233, 231, 229, 251, 265, 260, 255, 250};
    actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
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

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren_RightChildHasLeftChildWithRightChild){
    book->addOrder(5, true, 80, 228);
    book->addOrder(6, true, 80, 226);
    book->addOrder(7, true, 80, 231);
    book->addOrder(8, true, 80, 225);
    book->addOrder(9, true, 80, 229);
    book->addOrder(10, true, 80, 233);
    book->addOrder(11, true, 80, 230);

    std::vector<int> expectedInOrder = {225, 226, 228, 229, 230, 231, 233};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {228, 226, 225, 231, 229, 230, 233};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {225, 226, 230, 229, 233, 231, 228};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelOrder(5);

    expectedInOrder = {225, 226, 229, 230, 231, 233};
    actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    expectedPreOrder = {229, 226, 225, 231, 230, 233};
    actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    expectedPostOrder = {225, 226, 230, 233, 231, 229};
    actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

// AVL tree balancing tests
TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateOnInsert){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 30);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(25, true);
    Limit* limit3 = book->searchLimitMaps(30, true);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getRightChild()->getLimitPrice(), 30);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->addOrder(11, true, 80, 35);

    Limit* limit4 = book->searchLimitMaps(35, true);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 30);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 30);
    EXPECT_EQ(limit2->getRightChild(), nullptr);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 35);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 30);

    std::vector<int> expectedInOrder = {10, 15, 17, 20, 25, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 15, 10, 17, 30, 25, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {10, 17, 15, 25, 35, 30, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateOnInsert){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 22);
    book->addOrder(10, true, 80, 30);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);
    Limit* limit3 = book->searchLimitMaps(10, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 15);

    book->addOrder(11, true, 80, 5);

    Limit* limit4 = book->searchLimitMaps(5, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 5);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 10);

    std::vector<int> expectedInOrder = {5, 10, 15, 20, 22, 25, 30};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 10, 5, 15, 25, 22, 30};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 15, 10, 22, 30, 25, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateOnInsert){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 24);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 5);
    book->addOrder(13, true, 80, 28);
    book->addOrder(14, true, 80, 35);

    book->addOrder(15, true, 80, 26);

    std::vector<int> expectedInOrder = {5, 10, 15, 17, 20, 24, 25, 26, 28, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 15, 10, 5, 17, 28, 25, 24, 26, 30, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 10, 17, 15, 24, 26, 25, 35, 30, 28, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateOnInsert){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 24);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 5);
    book->addOrder(13, true, 80, 13);
    book->addOrder(14, true, 80, 35);

    book->addOrder(15, true, 80, 12);

    std::vector<int> expectedInOrder = {5, 10, 12, 13, 15, 17, 20, 24, 25, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 13, 10, 5, 12, 15, 17, 25, 24, 30, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 12, 10, 17, 15, 13, 24, 35, 30, 25, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateRootOnInsert){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 543, 81);
    book->addOrder(113, false, 46, 82);

    std::vector<int> expectedInOrder = {80, 81, 82};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {81, 80, 82};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {80, 82, 81};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 81);
    EXPECT_EQ(book->getSellTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateRootOnInsert){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 543, 79);
    book->addOrder(113, true, 46, 78);

    std::vector<int> expectedInOrder = {78, 79, 80};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {79, 78, 80};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {78, 80, 79};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 79);
    EXPECT_EQ(book->getBuyTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateRootOnInsert){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 543, 81);
    book->addOrder(113, false, 46, 82);

    std::vector<int> expectedInOrder = {80, 81, 82};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {81, 80, 82};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {80, 82, 81};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 81);
    EXPECT_EQ(book->getSellTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateRootOnInsert){
    book->addOrder(111, true, 43, 80);
    book->addOrder(113, true, 46, 78);
    book->addOrder(112, true, 543, 79);

    std::vector<int> expectedInOrder = {78, 79, 80};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {79, 78, 80};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {78, 80, 79};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 79);
    EXPECT_EQ(book->getBuyTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateOnDelete){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 22);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 27);
    book->addOrder(13, true, 80, 35);


    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(25, true);
    Limit* limit3 = book->searchLimitMaps(30, true);
    Limit* limit4 = book->searchLimitMaps(35, true);
    Limit* limit5 = book->searchLimitMaps(27, true);
    

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getRightChild()->getLimitPrice(), 30);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 22);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 27);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 35);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 30);
    EXPECT_EQ(limit5->getParent()->getLimitPrice(), 30);


    book->cancelOrder(10);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 30);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 30);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit2->getRightChild()->getLimitPrice(), 27);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 35);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 30);
    EXPECT_EQ(limit5->getParent()->getLimitPrice(), 25);

    std::vector<int> expectedInOrder = {10, 15, 17, 20, 25, 27, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 15, 10, 17, 30, 25, 27, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {10, 17, 15, 27, 25, 35, 30, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateOnDelete){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 22);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 5);
    book->addOrder(13, true, 80, 13);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);
    Limit* limit3 = book->searchLimitMaps(10, true);
    Limit* limit4 = book->searchLimitMaps(5, true);
    Limit* limit5 = book->searchLimitMaps(13, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getRightChild()->getLimitPrice(), 17);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 15);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 13);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 5);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 10);
    EXPECT_EQ(limit5->getParent()->getLimitPrice(), 10);
    
    book->cancelOrder(9);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getRightChild(), nullptr);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 13);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getRightChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 5);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 10);
    EXPECT_EQ(limit5->getParent()->getLimitPrice(), 15);

    std::vector<int> expectedInOrder = {5, 10, 13, 15, 20, 22, 25, 30};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 10, 5, 15, 13, 25, 22, 30};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 13, 15, 10, 22, 30, 25, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateOnDelete){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 24);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 5);
    book->addOrder(13, true, 80, 23);
    book->addOrder(14, true, 80, 28);
    book->addOrder(15, true, 80, 35);
    book->addOrder(16, true, 80, 26);
    book->addOrder(17, true, 80, 29);

    book->cancelOrder(13);

    std::vector<int> expectedInOrder = {5, 10, 15, 17, 20, 24, 25, 26, 28, 29, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 15, 10, 5, 17, 28, 25, 24, 26, 30, 29, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 10, 17, 15, 24, 26, 25, 29, 35, 30, 28, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateOnDelete){
    book->addOrder(5, true, 80, 20);
    book->addOrder(6, true, 80, 15);
    book->addOrder(7, true, 80, 25);
    book->addOrder(8, true, 80, 10);
    book->addOrder(9, true, 80, 17);
    book->addOrder(10, true, 80, 24);
    book->addOrder(11, true, 80, 30);
    book->addOrder(12, true, 80, 5);
    book->addOrder(13, true, 80, 13);
    book->addOrder(14, true, 80, 19);
    book->addOrder(15, true, 80, 35);
    book->addOrder(16, true, 80, 12);
    book->addOrder(17, true, 80, 14);

    book->cancelOrder(14);

    std::vector<int> expectedInOrder = {5, 10, 12, 13, 14, 15, 17, 20, 24, 25, 30, 35};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {20, 13, 10, 5, 12, 15, 14, 17, 25, 24, 30, 35};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {5, 12, 10, 14, 17, 15, 13, 24, 35, 30, 25, 20};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRRRotateRootOnDelete){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 43, 79);
    book->addOrder(113, false, 543, 82);
    book->addOrder(114, false, 46, 81);
    book->addOrder(115, false, 46, 83);

    book->cancelOrder(112);

    std::vector<int> expectedInOrder = {80, 81, 82, 83};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {82, 80, 81, 83};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {81, 80, 83, 82};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 82);
    EXPECT_EQ(book->getSellTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLLRotateRootOnDelete){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 543, 78);
    book->addOrder(113, true, 543, 81);
    book->addOrder(114, true, 46, 77);
    book->addOrder(115, true, 46, 79);

    book->cancelOrder(113);

    std::vector<int> expectedInOrder = {77, 78, 79, 80};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {78, 77, 80, 79};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {77, 79, 80, 78};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 78);
    EXPECT_EQ(book->getBuyTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeRLRotateRootOnDelete){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 43, 75);
    book->addOrder(113, false, 543, 81);
    book->addOrder(114, false, 46, 82);

    book->cancelOrder(112);

    std::vector<int> expectedInOrder = {80, 81, 82};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {81, 80, 82};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {80, 82, 81};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getSellTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 81);
    EXPECT_EQ(book->getSellTree()->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAVLTreeLRRotateRootOnDelete){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 46, 78);
    book->addOrder(113, true, 46, 85);
    book->addOrder(114, true, 543, 79);

    book->cancelOrder(113);

    std::vector<int> expectedInOrder = {78, 79, 80};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {79, 78, 80};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {78, 80, 79};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 79);
    EXPECT_EQ(book->getBuyTree()->getParent(), nullptr);
}

// Book edge tests
TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnInsertLowestSell){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 46, 78);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 78);
    
    book->addOrder(113, false, 46, 77);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);

    book->addOrder(114, false, 46, 85);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnInsertHighestBuy){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 46, 78);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);
    
    book->addOrder(113, true, 46, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);

    book->addOrder(114, true, 46, 70);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSell){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 46, 78);
    book->addOrder(113, false, 46, 77);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);
    
    book->cancelOrder(113);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 78);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuy){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 46, 78);
    book->addOrder(113, true, 46, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
    
    book->cancelOrder(113);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyEmptyTree){
    book->addOrder(111, true, 43, 80);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);

    book->cancelOrder(111);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellEmptyTree){
    book->addOrder(111, false, 43, 80);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 80);

    book->cancelOrder(111);

    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyRootLimit){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 43, 75);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);

    book->cancelOrder(111);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 75);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellRootLimit){
    book->addOrder(111, false, 10, 80);
    book->addOrder(112, false, 20, 85);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 80);

    book->cancelOrder(111);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 85);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyNotParent){
    book->addOrder(111, true, 43, 80);
    book->addOrder(112, true, 43, 75);
    book->addOrder(113, true, 43, 85);
    book->addOrder(114, true, 43, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 85);

    book->cancelOrder(113);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellNotParent){
    book->addOrder(111, false, 43, 80);
    book->addOrder(112, false, 43, 75);
    book->addOrder(113, false, 43, 85);
    book->addOrder(114, false, 43, 76);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 75);

    book->cancelOrder(112);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 76);
}

// Market orders tests
TEST_F(LimitOrderBookTests, TestBuyMarketOrderFilledBySingleOrder){
    book->addOrder(111, false, 100, 80);
    book->addOrder(112, false, 30, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 100);

    book->marketOrder(113, true, 20);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 80);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderFilledBySingleOrder){
    book->addOrder(111, true, 100, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 100);

    book->marketOrder(112, false, 98);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 2);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderFilledByMultipleOrders){
    book->addOrder(111, false, 10, 80);
    book->addOrder(112, false, 10, 80);
    book->addOrder(113, false, 10, 80);
    book->addOrder(114, false, 30, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 60);

    book->marketOrder(115, true, 40);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 20);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 20);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderFilledByMultipleOrders){
    book->addOrder(111, true, 5, 80);
    book->addOrder(112, true, 7, 80);
    book->addOrder(113, true, 31, 80);
    book->addOrder(114, true, 9, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 52);

    book->marketOrder(115, false, 12);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 31);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 40);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderPerfectlyFilledBySingleOrder){
    book->addOrder(111, false, 15, 80);
    book->addOrder(112, false, 21, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 15);

    book->marketOrder(115, true, 15);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 21);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderPerfectlyFilledBySingleOrder){
    book->addOrder(111, true, 1153, 80);
    book->addOrder(112, true, 832, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 1153);

    book->marketOrder(115, false, 1153);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 832);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderGoingIntoDifferentLimit){
    book->addOrder(111, false, 10, 80);
    book->addOrder(112, false, 5, 80);
    book->addOrder(113, false, 20, 85);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);

    book->marketOrder(115, true, 20);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 15);
    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 85);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderGoingIntoDifferentLimit){
    book->addOrder(111, true, 10, 80);
    book->addOrder(112, true, 20, 80);
    book->addOrder(113, true, 7, 85);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 7);

    book->marketOrder(115, false, 18);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 19);
    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderEmptySellTree){
    EXPECT_EQ(book->getLowestSell(), nullptr);

    book->marketOrder(115, true, 18);

    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderEmptyBuyTree){
    book->addOrder(111, true, 10, 80);
    book->addOrder(113, true, 7, 85);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 7);

    book->marketOrder(115, false, 18);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

// Modifying orders tests
TEST_F(LimitOrderBookTests, TestModifyOrderToExistingLimit){
    book->addOrder(111, true, 10, 80);
    book->addOrder(112, true, 20, 80);
    book->addOrder(113, true, 7, 85);
    book->addOrder(114, true, 14, 85);

    book->modifyOrder(113, 40, 80);

    Limit* limit1 = book->searchLimitMaps(80, true);
    Limit* limit2 = book->searchLimitMaps(85, true);

    EXPECT_EQ(limit1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(limit1->getTotalVolume(), 70);
    EXPECT_EQ(limit2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyOrderToNewLimit){
    book->addOrder(111, true, 10, 80);
    book->addOrder(112, true, 20, 80);
    book->addOrder(113, true, 7, 85);
    book->addOrder(114, true, 14, 85);

    book->modifyOrder(113, 40, 82);

    Limit* limit1 = book->searchLimitMaps(82, true);
    Limit* limit2 = book->searchLimitMaps(85, true);

    EXPECT_EQ(limit1->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(limit1->getTotalVolume(), 40);
    EXPECT_EQ(limit2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyOrderInvalidOrderId){
    book->addOrder(111, true, 10, 80);
    book->addOrder(112, true, 20, 80);
    book->addOrder(113, true, 7, 85);
    book->addOrder(114, true, 14, 85);

    book->modifyOrder(110, 40, 82);

    Limit* limit1 = book->searchLimitMaps(80, true);
    Limit* limit2 = book->searchLimitMaps(85, true);

    EXPECT_EQ(limit1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(limit1->getTotalVolume(), 30);
    EXPECT_EQ(limit2->getTotalVolume(), 21);
    EXPECT_EQ(book->searchLimitMaps(82, true), nullptr);
    EXPECT_EQ(book->searchOrderMap(110), nullptr);
}

// Limit order that is a market order tests
TEST_F(LimitOrderBookTests, TestAddingSellLimitOrderWhichIsAMarketOrder) {
    book->addOrder(357, true, 40, 100);
    book->addOrder(222, false, 35, 100);

    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 5);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingSellLimitOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addOrder(357, true, 15, 110);
    book->addOrder(358, true, 35, 100);
    book->addOrder(359, true, 35, 100);
    book->addOrder(222, false, 40, 100);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 45);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingSellLimitOrderWhichIsOnlyPartiallyAMarketOrder) {
    book->addOrder(357, true, 40, 100);
    book->addOrder(357, true, 40, 99);
    book->addOrder(222, false, 45, 100);

    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 40);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 40);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 5);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsAMarketOrder) {
    book->addOrder(357, false, 40, 100);
    book->addOrder(222, true, 35, 100);

    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addOrder(357, false, 15, 90);
    book->addOrder(358, false, 35, 100);
    book->addOrder(359, false, 35, 100);
    book->addOrder(222, true, 40, 100);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 45);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsOnlyPartiallyAMarketOrder) {
    book->addOrder(357, false, 40, 100);
    book->addOrder(357, false, 40, 101);
    book->addOrder(222, true, 45, 100);

    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 40);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 40);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 5);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
}

// Adding stop orders tests
TEST_F(LimitOrderBookTests, TestAddingAStopOrder) {
    EXPECT_EQ(book->searchOrderMap(357), nullptr);
    EXPECT_EQ(book->searchStopMap(100), nullptr);

    book->addStopOrder(357, true, 27, 100);

    EXPECT_EQ(book->searchOrderMap(357)->getShares(), 27);
    EXPECT_EQ(book->searchStopMap(100)->getTotalVolume(), 27);
    EXPECT_EQ(book->searchStopMap(20), nullptr);

    book->addStopOrder(222, false, 35, 110);

    EXPECT_EQ(book->searchStopMap(110)->getTotalVolume(), 35);
}

// Cancelling stop orders tests
TEST_F(LimitOrderBookTests, TestCancelStopOrderLeavingNonEmptyLimit){
    book->addStopOrder(5, true, 80, 20);
    book->addStopOrder(6, true, 32, 20);
    book->addStopOrder(7, true, 111, 20);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 3);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 223);

    book->cancelStopOrder(6);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 2);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 191);

    book->cancelStopOrder(7);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 1);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 80);
}

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnOrderCancel){
    book->addStopOrder(5, true, 80, 20);
    book->addStopOrder(6, true, 32, 20);
    book->addStopOrder(7, true, 111, 20);

    Limit* stop = book->searchStopMap(20);

    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 5);

    book->cancelStopOrder(5);
    
    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 6);
}

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnOrderCancelLeavingEmptyLimit){
    book->addStopOrder(5, true, 80, 20);

    Limit* stop = book->searchStopMap(20);

    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 5);

    book->cancelStopOrder(5);
    
    EXPECT_EQ(stop->getHeadOrder(), nullptr);
}

TEST_F(LimitOrderBookTests, TestCancelStopOrderLeavingEmptyLimit){
    book->addStopOrder(5, true, 80, 20);
    book->addStopOrder(6, true, 80, 15);
    Limit* stop1 = book->searchStopMap(20);
    Limit* stop2 = book->searchStopMap(15);

    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 6);
    EXPECT_EQ(stop1->getLeftChild()->getLimitPrice(), 15);

    book->cancelStopOrder(6);
    
    EXPECT_EQ(book->searchLimitMaps(15, true), nullptr);
    EXPECT_EQ(stop1->getLeftChild(), nullptr);
}

// Modifying stop orders tests
TEST_F(LimitOrderBookTests, TestModifyStopOrderToExistingStopLevel){
    book->addStopOrder(111, true, 10, 80);
    book->addStopOrder(112, true, 20, 80);
    book->addStopOrder(113, true, 7, 85);
    book->addStopOrder(114, true, 14, 85);

    book->modifyStopOrder(113, 40, 80);

    Limit* stop1 = book->searchStopMap(80);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(stop1->getTotalVolume(), 70);
    EXPECT_EQ(stop2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyStopOrderToNewStopLevel){
    book->addStopOrder(111, true, 10, 80);
    book->addStopOrder(112, true, 20, 80);
    book->addStopOrder(113, true, 7, 85);
    book->addStopOrder(114, true, 14, 85);

    book->modifyStopOrder(113, 40, 82);

    Limit* stop1 = book->searchStopMap(82);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(stop1->getTotalVolume(), 40);
    EXPECT_EQ(stop2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyStopOrderInvalidOrderId){
    book->addStopOrder(111, true, 10, 80);
    book->addStopOrder(112, true, 20, 80);
    book->addStopOrder(113, true, 7, 85);
    book->addStopOrder(114, true, 14, 85);

    book->modifyStopOrder(110, 40, 82);

    Limit* stop1 = book->searchStopMap(80);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(stop1->getTotalVolume(), 30);
    EXPECT_EQ(stop2->getTotalVolume(), 21);
    EXPECT_EQ(book->searchStopMap(82), nullptr);
    EXPECT_EQ(book->searchOrderMap(110), nullptr);
}

// Stop book edge tests
TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnInsertHighestStopSell){
    book->addStopOrder(111, false, 43, 80);
    book->addStopOrder(112, false, 46, 78);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 80);
    
    book->addStopOrder(113, false, 46, 81);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 81);

    book->addStopOrder(114, false, 46, 79);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 81);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnInsertLowestStopBuy){
    book->addStopOrder(111, true, 43, 80);
    book->addStopOrder(112, true, 46, 78);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 78);
    
    book->addStopOrder(113, true, 46, 82);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 78);

    book->addStopOrder(114, true, 46, 70);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 70);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteHighestStopSell){
    book->addStopOrder(111, false, 43, 78);
    book->addStopOrder(112, false, 46, 80);
    book->addStopOrder(113, false, 46, 77);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 80);
    
    book->cancelStopOrder(112);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 78);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteLowestStopBuy){
    book->addStopOrder(111, true, 43, 80);
    book->addStopOrder(112, true, 46, 78);
    book->addStopOrder(113, true, 46, 82);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 78);
    
    book->cancelStopOrder(112);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteLowestStopBuyEmptyTree){
    book->addStopOrder(111, true, 43, 80);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 80);

    book->cancelStopOrder(111);

    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteHighestStopSellEmptyTree){
    book->addStopOrder(111, false, 43, 80);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 80);

    book->cancelStopOrder(111);

    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteLowestStopBuyRootLimit){
    book->addStopOrder(111, true, 43, 75);
    book->addStopOrder(112, true, 43, 80);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 75);

    book->cancelStopOrder(111);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestStopSellRootLimit){
    book->addStopOrder(111, false, 10, 85);
    book->addStopOrder(112, false, 20, 80);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 85);

    book->cancelStopOrder(111);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteLowestStopBuyNotParent){
    book->addStopOrder(111, true, 43, 80);
    book->addStopOrder(112, true, 43, 75);
    book->addStopOrder(113, true, 43, 85);
    book->addStopOrder(114, true, 43, 76);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 75);

    book->cancelStopOrder(112);

    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 76);
}

TEST_F(LimitOrderBookTests, TestUpdateStopBookEdgeOnDeleteHighestStopSellNotParent){
    book->addStopOrder(111, false, 43, 80);
    book->addStopOrder(112, false, 43, 75);
    book->addStopOrder(113, false, 43, 85);
    book->addStopOrder(114, false, 43, 82);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 85);

    book->cancelStopOrder(113);

    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 82);
}

// Stop orders being triggered tests
TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredByMarketSellOrder){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 99);
    book->addOrder(113, true, 10, 98);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 4);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredByMarketBuyOrder){
    book->addOrder(111, false, 10, 100);
    book->addOrder(112, false, 10, 99);
    book->addOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 4);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestMultipleStopOrderLevelsTriggeredByMarketSellOrder){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 98);
    book->addOrder(113, true, 30, 97);

    book->addStopOrder(114, false, 15, 99);
    book->addStopOrder(115, false, 15, 98);
    book->addStopOrder(116, false, 15, 96);

    std::vector<int> expectedPreOrder = {98, 96, 99};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getStopSellTree());
    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(117, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 97);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 9);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 96);
}

TEST_F(LimitOrderBookTests, TestMultipleStopOrderLevelsTriggeredByMarketBuyOrder){
    book->addOrder(111, false, 30, 101);
    book->addOrder(112, false, 10, 100);
    book->addOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);
    book->addStopOrder(115, true, 15, 100);
    book->addStopOrder(116, true, 15, 102);

    std::vector<int> expectedPreOrder = {100, 99, 102};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getStopBuyTree());
    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(117, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 101);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 9);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 102);
}

TEST_F(LimitOrderBookTests, TestStopSellOrderTriggeringFurtherStopSellOrder){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 99);
    book->addOrder(113, true, 30, 97);

    book->addStopOrder(114, false, 15, 99);
    book->addStopOrder(115, false, 15, 98);
    book->addStopOrder(116, false, 15, 96);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(117, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 97);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 9);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 96);
}

TEST_F(LimitOrderBookTests, TestStopBuyOrderTriggeringFurtherStopBuyOrder){
    book->addOrder(111, false, 30, 101);
    book->addOrder(112, false, 10, 99);
    book->addOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);
    book->addStopOrder(115, true, 15, 100);
    book->addStopOrder(116, true, 15, 102);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(117, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 101);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 9);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 102);
}

TEST_F(LimitOrderBookTests, TestStopOrderTriggeringTwoFurtherIterationsOfStopOrders){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 99);
    book->addOrder(113, true, 20, 97);
    book->addOrder(118, true, 30, 95);

    book->addStopOrder(114, false, 15, 99);
    book->addStopOrder(115, false, 15, 98);
    book->addStopOrder(116, false, 15, 96);
    book->addStopOrder(119, false, 15, 94);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(117, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 95);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 14);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 94);
}

TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredBySellLimitOrderWhichIsAMarketOrder){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 99);
    book->addOrder(113, true, 10, 98);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->addOrder(115, false, 11, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 4);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredByBuyLimitOrderWhichIsAMarketOrder){
    book->addOrder(111, false, 10, 100);
    book->addOrder(112, false, 10, 99);
    book->addOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->addOrder(115, true, 11, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 4);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestBuyStopOrderWhenEmptySellTree){
    book->addOrder(111, true, 10, 100);
    book->addOrder(112, true, 10, 99);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestSellStopOrderWhenEmptyBuyTree){
    book->addOrder(112, false, 10, 99);
    book->addOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell(), nullptr);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}
