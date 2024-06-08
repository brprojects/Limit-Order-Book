#include "../Limit_Order_Book/Limit.hpp"
#include "../Limit_Order_Book/Order.hpp"
#include "../Limit_Order_Book/Book.hpp"

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

    book->addLimitOrder(357, true, 27, 100);

    EXPECT_EQ(book->searchOrderMap(357)->getShares(), 27);
    EXPECT_EQ(book->searchLimitMaps(100, true)->getTotalVolume(), 27);
    EXPECT_EQ(book->searchLimitMaps(20, false), nullptr);

    book->addLimitOrder(222, false, 35, 110);

    EXPECT_EQ(book->searchLimitMaps(110, false)->getTotalVolume(), 35);
}

TEST_F(LimitOrderBookTests, TestMultipleOrdersInALimit){
    book->addLimitOrder(5, true, 80, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 80);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 1);

    book->addLimitOrder(6, true, 32, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 112);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 2);


    book->addLimitOrder(7, true, 111, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 223);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 3);

}

// Cancelling orders tests
TEST_F(LimitOrderBookTests, TestCancelOrderLeavingNonEmptyLimit){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 32, 20);
    book->addLimitOrder(7, true, 111, 20);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 3);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 223);

    book->cancelLimitOrder(6);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 2);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 191);

    book->cancelLimitOrder(7);

    EXPECT_EQ(book->searchLimitMaps(20, true)->getSize(), 1);
    EXPECT_EQ(book->searchLimitMaps(20, true)->getTotalVolume(), 80);
}

TEST_F(LimitOrderBookTests, TestLimitHeadOrderChangeOnOrderCancel){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 32, 20);
    book->addLimitOrder(7, true, 111, 20);

    Limit* limit = book->searchLimitMaps(20, true);

    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 5);

    book->cancelLimitOrder(5);
    
    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 6);
}

TEST_F(LimitOrderBookTests, TestLimitHeadOrderChangeOnOrderCancelLeavingEmptyLimit){
    book->addLimitOrder(5, true, 80, 20);

    Limit* limit = book->searchLimitMaps(20, true);

    EXPECT_EQ(limit->getHeadOrder()->getOrderId(), 5);

    book->cancelLimitOrder(5);
    
    EXPECT_EQ(limit->getHeadOrder(), nullptr);
}

TEST_F(LimitOrderBookTests, TestCancelOrderLeavingEmptyLimit){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 6);
    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);

    book->cancelLimitOrder(6);
    
    EXPECT_EQ(book->searchLimitMaps(15, true), nullptr);
    EXPECT_EQ(limit1->getLeftChild(), nullptr);
}

// Adding to BST tests
TEST_F(LimitOrderBookTests, TestCorrectLimitParent){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);
    Limit* limit3 = book->searchLimitMaps(25, true);

    EXPECT_EQ(limit1->getParent(), nullptr);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestCorrectLimitChildren){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit2->getRightChild(), nullptr);
}

TEST_F(LimitOrderBookTests, TestTreeHeightsCorrect){
    book->addLimitOrder(5, true, 80, 20);
    Limit* limit1 = book->searchLimitMaps(20, true);

    EXPECT_EQ(book->getLimitHeight(limit1), 1);

    book->addLimitOrder(6, true, 80, 15);
    Limit* limit2 = book->searchLimitMaps(15, true);

    EXPECT_EQ(book->getLimitHeight(limit2), 1);
    EXPECT_EQ(book->getLimitHeight(limit1), 2);

    book->addLimitOrder(7, true, 80, 25);
    Limit* limit3 = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getLimitHeight(limit3), 1);
    EXPECT_EQ(book->getLimitHeight(limit1), 2);

    book->addLimitOrder(8, true, 80, 10);
    Limit* limit4 = book->searchLimitMaps(10, true);

    EXPECT_EQ(book->getLimitHeight(limit4), 1);
    EXPECT_EQ(book->getLimitHeight(limit2), 2);
    EXPECT_EQ(book->getLimitHeight(limit1), 3);

    book->addLimitOrder(9, true, 80, 5);
}

TEST_F(LimitOrderBookTests, TestBinarySearchTree){
    book->addLimitOrder(5, false, 80, 20);
    book->addLimitOrder(6, false, 80, 15);
    book->addLimitOrder(7, false, 80, 25);
    book->addLimitOrder(8, false, 80, 10);
    book->addLimitOrder(9, false, 80, 19);

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
    book->addLimitOrder(5, false, 80, 20);
    book->addLimitOrder(6, false, 80, 15);
    book->addLimitOrder(7, false, 80, 25);
    book->addLimitOrder(8, false, 80, 10);
    book->addLimitOrder(9, false, 80, 19);

    Limit* limit = book->searchLimitMaps(15, false);

    EXPECT_EQ(limit->getRightChild()->getLimitPrice(), 19);

    book->cancelLimitOrder(9);

    EXPECT_EQ(limit->getRightChild(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithLeftChildOnly){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(10, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);

    book->cancelLimitOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithRightChildOnly){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 19);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(19, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);

    book->cancelLimitOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 19);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 18);
    book->addLimitOrder(10, true, 80, 23);
    book->addLimitOrder(11, true, 80, 27);
    book->addLimitOrder(12, true, 80, 17);
    book->addLimitOrder(13, true, 80, 19);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(10, true);
    Limit* limit3 = book->searchLimitMaps(18, true);
    Limit* limit4 = book->searchLimitMaps(17, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 15);
    EXPECT_EQ(limit3->getLeftChild()->getLimitPrice(), 17);

    book->cancelLimitOrder(6);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 17);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 17);
    EXPECT_EQ(limit3->getLeftChild(), nullptr);
    EXPECT_EQ(limit4->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit4->getRightChild()->getLimitPrice(), 18);
    EXPECT_EQ(limit4->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren_RightChildHasNoLeftChild){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 18);
    book->addLimitOrder(10, true, 80, 23);
    book->addLimitOrder(11, true, 80, 27);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(23, true);
    Limit* limit3 = book->searchLimitMaps(27, true);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 25);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->cancelLimitOrder(7);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 27);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 27);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 20);
}

TEST_F(LimitOrderBookTests, TestRemoveLimitWithTwoChildren_RightChildHasLeftChildWithRightChild){
    book->addLimitOrder(3, true, 80, 224);
    book->addLimitOrder(4, true, 80, 220);
    book->addLimitOrder(5, true, 80, 228);
    book->addLimitOrder(6, true, 80, 218);
    book->addLimitOrder(7, true, 80, 221);
    book->addLimitOrder(8, true, 80, 226);
    book->addLimitOrder(9, true, 80, 231);
    book->addLimitOrder(10, true, 80, 217);
    book->addLimitOrder(11, true, 80, 225);
    book->addLimitOrder(12, true, 80, 229);
    book->addLimitOrder(13, true, 80, 233);
    book->addLimitOrder(14, true, 80, 230);

    std::vector<int> expectedInOrder = {217, 218, 220, 221, 224, 225, 226, 228, 229, 230, 231, 233};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {224, 220, 218, 217, 221, 228, 226, 225, 231, 229, 230, 233};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {217, 218, 221, 220, 225, 226, 230, 229, 233, 231, 228, 224};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelLimitOrder(5);

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
    book->addLimitOrder(3, true, 80, 250);
    book->addLimitOrder(4, true, 80, 255);
    book->addLimitOrder(5, true, 80, 228);
    book->addLimitOrder(6, true, 80, 251);
    book->addLimitOrder(7, true, 80, 260);
    book->addLimitOrder(8, true, 80, 226);
    book->addLimitOrder(9, true, 80, 231);
    book->addLimitOrder(10, true, 80, 265);
    book->addLimitOrder(11, true, 80, 225);
    book->addLimitOrder(12, true, 80, 229);
    book->addLimitOrder(13, true, 80, 233);
    book->addLimitOrder(14, true, 80, 230);

    std::vector<int> expectedInOrder = {225, 226, 228, 229, 230, 231, 233, 250, 251, 255, 260, 265};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {250, 228, 226, 225, 231, 229, 230, 233, 255, 251, 260, 265};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {225, 226, 230, 229, 233, 231, 228, 251, 265, 260, 255, 250};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelLimitOrder(5);

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
    book->addLimitOrder(5, true, 80, 20);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);

    book->cancelLimitOrder(5);

    EXPECT_EQ(book->getBuyTree(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithLeftChildOnly){
    book->addLimitOrder(5, false, 80, 20);
    book->addLimitOrder(6, false, 80, 15);

    Limit* limit = book->searchLimitMaps(15, false);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit->getParent()->getLimitPrice(), 20);

    book->cancelLimitOrder(5);

    EXPECT_EQ(book->getSellTree()->getLimitPrice(), 15);
    EXPECT_EQ(limit->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithRightChildOnly){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 25);

    Limit* limit = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit->getParent()->getLimitPrice(), 20);

    book->cancelLimitOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 25);
    EXPECT_EQ(limit->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 27);
    book->addLimitOrder(9, true, 80, 22);

    Limit* limit1 = book->searchLimitMaps(15, true);
    Limit* limit2 = book->searchLimitMaps(25, true);
    Limit* limit3 = book->searchLimitMaps(22, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 22);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->cancelLimitOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 22);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 22);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 22);
    EXPECT_EQ(limit2->getLeftChild(), nullptr);
    EXPECT_EQ(limit3->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren_RightChildHasNoLeftChild){
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 27);

    Limit* limit1 = book->searchLimitMaps(15, true);
    Limit* limit2 = book->searchLimitMaps(25, true);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 20);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);

    book->cancelLimitOrder(5);

    EXPECT_EQ(book->getBuyTree()->getLimitPrice(), 25);
    EXPECT_EQ(limit1->getParent()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent(), nullptr);
}

TEST_F(LimitOrderBookTests, TestRemoveRootLimitWithTwoChildren_RightChildHasLeftChildWithRightChild){
    book->addLimitOrder(5, true, 80, 228);
    book->addLimitOrder(6, true, 80, 226);
    book->addLimitOrder(7, true, 80, 231);
    book->addLimitOrder(8, true, 80, 225);
    book->addLimitOrder(9, true, 80, 229);
    book->addLimitOrder(10, true, 80, 233);
    book->addLimitOrder(11, true, 80, 230);

    std::vector<int> expectedInOrder = {225, 226, 228, 229, 230, 231, 233};
    std::vector<int> actualInOrder = book->inOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedInOrder, actualInOrder);

    std::vector<int> expectedPreOrder = {228, 226, 225, 231, 229, 230, 233};
    std::vector<int> actualPreOrder = book->preOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPreOrder, actualPreOrder);

    std::vector<int> expectedPostOrder = {225, 226, 230, 229, 233, 231, 228};
    std::vector<int> actualPostOrder = book->postOrderTreeTraversal(book->getBuyTree());

    EXPECT_EQ(expectedPostOrder, actualPostOrder);

    book->cancelLimitOrder(5);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 30);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(25, true);
    Limit* limit3 = book->searchLimitMaps(30, true);

    EXPECT_EQ(limit1->getRightChild()->getLimitPrice(), 25);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getRightChild()->getLimitPrice(), 30);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 25);

    book->addLimitOrder(11, true, 80, 35);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 22);
    book->addLimitOrder(10, true, 80, 30);

    Limit* limit1 = book->searchLimitMaps(20, true);
    Limit* limit2 = book->searchLimitMaps(15, true);
    Limit* limit3 = book->searchLimitMaps(10, true);

    EXPECT_EQ(limit1->getLeftChild()->getLimitPrice(), 15);
    EXPECT_EQ(limit2->getParent()->getLimitPrice(), 20);
    EXPECT_EQ(limit2->getLeftChild()->getLimitPrice(), 10);
    EXPECT_EQ(limit3->getParent()->getLimitPrice(), 15);

    book->addLimitOrder(11, true, 80, 5);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 24);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 5);
    book->addLimitOrder(13, true, 80, 28);
    book->addLimitOrder(14, true, 80, 35);

    book->addLimitOrder(15, true, 80, 26);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 24);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 5);
    book->addLimitOrder(13, true, 80, 13);
    book->addLimitOrder(14, true, 80, 35);

    book->addLimitOrder(15, true, 80, 12);

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
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 543, 81);
    book->addLimitOrder(113, false, 46, 82);

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
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 543, 79);
    book->addLimitOrder(113, true, 46, 78);

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
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 543, 81);
    book->addLimitOrder(113, false, 46, 82);

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
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(113, true, 46, 78);
    book->addLimitOrder(112, true, 543, 79);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 22);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 27);
    book->addLimitOrder(13, true, 80, 35);


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


    book->cancelLimitOrder(10);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 22);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 5);
    book->addLimitOrder(13, true, 80, 13);

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
    
    book->cancelLimitOrder(9);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 24);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 5);
    book->addLimitOrder(13, true, 80, 23);
    book->addLimitOrder(14, true, 80, 28);
    book->addLimitOrder(15, true, 80, 35);
    book->addLimitOrder(16, true, 80, 26);
    book->addLimitOrder(17, true, 80, 29);

    book->cancelLimitOrder(13);

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
    book->addLimitOrder(5, true, 80, 20);
    book->addLimitOrder(6, true, 80, 15);
    book->addLimitOrder(7, true, 80, 25);
    book->addLimitOrder(8, true, 80, 10);
    book->addLimitOrder(9, true, 80, 17);
    book->addLimitOrder(10, true, 80, 24);
    book->addLimitOrder(11, true, 80, 30);
    book->addLimitOrder(12, true, 80, 5);
    book->addLimitOrder(13, true, 80, 13);
    book->addLimitOrder(14, true, 80, 19);
    book->addLimitOrder(15, true, 80, 35);
    book->addLimitOrder(16, true, 80, 12);
    book->addLimitOrder(17, true, 80, 14);

    book->cancelLimitOrder(14);

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
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 43, 79);
    book->addLimitOrder(113, false, 543, 82);
    book->addLimitOrder(114, false, 46, 81);
    book->addLimitOrder(115, false, 46, 83);

    book->cancelLimitOrder(112);

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
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 543, 78);
    book->addLimitOrder(113, true, 543, 81);
    book->addLimitOrder(114, true, 46, 77);
    book->addLimitOrder(115, true, 46, 79);

    book->cancelLimitOrder(113);

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
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 43, 75);
    book->addLimitOrder(113, false, 543, 81);
    book->addLimitOrder(114, false, 46, 82);

    book->cancelLimitOrder(112);

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
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 46, 78);
    book->addLimitOrder(113, true, 46, 85);
    book->addLimitOrder(114, true, 543, 79);

    book->cancelLimitOrder(113);

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
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 46, 78);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 78);
    
    book->addLimitOrder(113, false, 46, 77);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);

    book->addLimitOrder(114, false, 46, 85);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnInsertHighestBuy){
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 46, 78);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);
    
    book->addLimitOrder(113, true, 46, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);

    book->addLimitOrder(114, true, 46, 70);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSell){
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 46, 78);
    book->addLimitOrder(113, false, 46, 77);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 77);
    
    book->cancelLimitOrder(113);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 78);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuy){
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 46, 78);
    book->addLimitOrder(113, true, 46, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
    
    book->cancelLimitOrder(113);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyEmptyTree){
    book->addLimitOrder(111, true, 43, 80);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);

    book->cancelLimitOrder(111);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellEmptyTree){
    book->addLimitOrder(111, false, 43, 80);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 80);

    book->cancelLimitOrder(111);

    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyRootLimit){
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 43, 75);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 80);

    book->cancelLimitOrder(111);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 75);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellRootLimit){
    book->addLimitOrder(111, false, 10, 80);
    book->addLimitOrder(112, false, 20, 85);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 80);

    book->cancelLimitOrder(111);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 85);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteHighestBuyNotParent){
    book->addLimitOrder(111, true, 43, 80);
    book->addLimitOrder(112, true, 43, 75);
    book->addLimitOrder(113, true, 43, 85);
    book->addLimitOrder(114, true, 43, 82);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 85);

    book->cancelLimitOrder(113);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 82);
}

TEST_F(LimitOrderBookTests, TestUpdateBookEdgeOnDeleteLowestSellNotParent){
    book->addLimitOrder(111, false, 43, 80);
    book->addLimitOrder(112, false, 43, 75);
    book->addLimitOrder(113, false, 43, 85);
    book->addLimitOrder(114, false, 43, 76);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 75);

    book->cancelLimitOrder(112);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 76);
}

// Market orders tests
TEST_F(LimitOrderBookTests, TestBuyMarketOrderFilledBySingleOrder){
    book->addLimitOrder(111, false, 100, 80);
    book->addLimitOrder(112, false, 30, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 100);

    book->marketOrder(113, true, 20);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 80);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderFilledBySingleOrder){
    book->addLimitOrder(111, true, 100, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 100);

    book->marketOrder(112, false, 98);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 2);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderFilledByMultipleOrders){
    book->addLimitOrder(111, false, 10, 80);
    book->addLimitOrder(112, false, 10, 80);
    book->addLimitOrder(113, false, 10, 80);
    book->addLimitOrder(114, false, 30, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 60);

    book->marketOrder(115, true, 40);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 20);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 20);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderFilledByMultipleOrders){
    book->addLimitOrder(111, true, 5, 80);
    book->addLimitOrder(112, true, 7, 80);
    book->addLimitOrder(113, true, 31, 80);
    book->addLimitOrder(114, true, 9, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 52);

    book->marketOrder(115, false, 12);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 31);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 40);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderPerfectlyFilledBySingleOrder){
    book->addLimitOrder(111, false, 15, 80);
    book->addLimitOrder(112, false, 21, 80);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 15);

    book->marketOrder(115, true, 15);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 21);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderPerfectlyFilledBySingleOrder){
    book->addLimitOrder(111, true, 1153, 80);
    book->addLimitOrder(112, true, 832, 80);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 1153);

    book->marketOrder(115, false, 1153);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 832);
}

TEST_F(LimitOrderBookTests, TestBuyMarketOrderGoingIntoDifferentLimit){
    book->addLimitOrder(111, false, 10, 80);
    book->addLimitOrder(112, false, 5, 80);
    book->addLimitOrder(113, false, 20, 85);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);

    book->marketOrder(115, true, 20);

    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 15);
    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 85);
}

TEST_F(LimitOrderBookTests, TestSellMarketOrderGoingIntoDifferentLimit){
    book->addLimitOrder(111, true, 10, 80);
    book->addLimitOrder(112, true, 20, 80);
    book->addLimitOrder(113, true, 7, 85);

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
    book->addLimitOrder(111, true, 10, 80);
    book->addLimitOrder(113, true, 7, 85);

    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 7);

    book->marketOrder(115, false, 18);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

// Modifying orders tests
TEST_F(LimitOrderBookTests, TestModifyOrderToExistingLimit){
    book->addLimitOrder(111, true, 10, 80);
    book->addLimitOrder(112, true, 20, 80);
    book->addLimitOrder(113, true, 7, 85);
    book->addLimitOrder(114, true, 14, 85);

    book->modifyLimitOrder(113, 40, 80);

    Limit* limit1 = book->searchLimitMaps(80, true);
    Limit* limit2 = book->searchLimitMaps(85, true);

    EXPECT_EQ(limit1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(limit1->getTotalVolume(), 70);
    EXPECT_EQ(limit2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyOrderToNewLimit){
    book->addLimitOrder(111, true, 10, 80);
    book->addLimitOrder(112, true, 20, 80);
    book->addLimitOrder(113, true, 7, 85);
    book->addLimitOrder(114, true, 14, 85);

    book->modifyLimitOrder(113, 40, 82);

    Limit* limit1 = book->searchLimitMaps(82, true);
    Limit* limit2 = book->searchLimitMaps(85, true);

    EXPECT_EQ(limit1->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(limit2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(limit1->getTotalVolume(), 40);
    EXPECT_EQ(limit2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyOrderInvalidOrderId){
    book->addLimitOrder(111, true, 10, 80);
    book->addLimitOrder(112, true, 20, 80);
    book->addLimitOrder(113, true, 7, 85);
    book->addLimitOrder(114, true, 14, 85);

    book->modifyLimitOrder(110, 40, 82);

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
    book->addLimitOrder(357, true, 40, 100);
    book->addLimitOrder(222, false, 35, 100);

    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 5);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingSellLimitOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addLimitOrder(357, true, 15, 110);
    book->addLimitOrder(358, true, 35, 100);
    book->addLimitOrder(359, true, 35, 100);
    book->addLimitOrder(222, false, 40, 100);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 45);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getLowestSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingSellLimitOrderWhichIsOnlyPartiallyAMarketOrder) {
    book->addLimitOrder(357, true, 40, 100);
    book->addLimitOrder(357, true, 40, 99);
    book->addLimitOrder(222, false, 45, 100);

    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 40);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 40);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 5);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsAMarketOrder) {
    book->addLimitOrder(357, false, 40, 100);
    book->addLimitOrder(222, true, 35, 100);

    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addLimitOrder(357, false, 15, 90);
    book->addLimitOrder(358, false, 35, 100);
    book->addLimitOrder(359, false, 35, 100);
    book->addLimitOrder(222, true, 40, 100);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 45);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getHighestBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyLimitOrderWhichIsOnlyPartiallyAMarketOrder) {
    book->addLimitOrder(357, false, 40, 100);
    book->addLimitOrder(357, false, 40, 101);
    book->addLimitOrder(222, true, 45, 100);

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

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnStopOrderCancel){
    book->addStopOrder(5, true, 80, 20);
    book->addStopOrder(6, true, 32, 20);
    book->addStopOrder(7, true, 111, 20);

    Limit* stop = book->searchStopMap(20);

    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 5);

    book->cancelStopOrder(5);
    
    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 6);
}

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnStopOrderCancelLeavingEmptyLimit){
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
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 10, 98);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 4);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredByMarketBuyOrder){
    book->addLimitOrder(111, false, 10, 100);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 4);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestMultipleStopOrderLevelsTriggeredByMarketSellOrder){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 98);
    book->addLimitOrder(113, true, 30, 97);

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
    book->addLimitOrder(111, false, 30, 101);
    book->addLimitOrder(112, false, 10, 100);
    book->addLimitOrder(113, false, 10, 98);

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
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 30, 97);

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
    book->addLimitOrder(111, false, 30, 101);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

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
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 20, 97);
    book->addLimitOrder(118, true, 30, 95);

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
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 10, 98);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->addLimitOrder(115, false, 11, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 4);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopOrdersTriggeredByBuyLimitOrderWhichIsAMarketOrder){
    book->addLimitOrder(111, false, 10, 100);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->addLimitOrder(115, true, 11, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 4);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestBuyStopOrderWhenEmptySellTree){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);

    book->addStopOrder(114, false, 15, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestSellStopOrderWhenEmptyBuyTree){
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopOrder(114, true, 15, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell(), nullptr);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

// Stop order that is a market order tests
TEST_F(LimitOrderBookTests, TestAddingSellStopOrderWhichIsAMarketOrder) {
    book->addLimitOrder(357, true, 40, 100);
    book->addStopOrder(222, false, 35, 100);

    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 5);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingSellStopOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addLimitOrder(357, true, 15, 100);
    book->addLimitOrder(358, true, 35, 90);
    book->addLimitOrder(359, true, 35, 90);
    book->addStopOrder(222, false, 40, 100);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 90);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 45);
    EXPECT_EQ(book->getHighestBuy()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyStopOrderWhichIsAMarketOrder) {
    book->addLimitOrder(357, false, 40, 100);
    book->addStopOrder(222, true, 35, 100);

    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 5);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestAddingBuyStopOrderWhichIsAMarketOrderAcrossMultipleLimits) {
    book->addLimitOrder(357, false, 15, 100);
    book->addLimitOrder(358, false, 35, 110);
    book->addLimitOrder(359, false, 35, 110);
    book->addStopOrder(222, true, 40, 100);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 110);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 45);
    EXPECT_EQ(book->getLowestSell()->getHeadOrder()->getShares(), 10);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

// Adding stop limit orders tests
TEST_F(LimitOrderBookTests, TestAddingAStopLimitOrder) {
    EXPECT_EQ(book->searchOrderMap(357), nullptr);
    EXPECT_EQ(book->searchStopMap(100), nullptr);

    book->addStopLimitOrder(357, true, 27, 110, 100);

    EXPECT_EQ(book->searchOrderMap(357)->getShares(), 27);
    EXPECT_EQ(book->searchStopMap(100)->getTotalVolume(), 27);
    EXPECT_EQ(book->searchStopMap(20), nullptr);

    book->addStopLimitOrder(222, false, 35, 105, 110);

    EXPECT_EQ(book->searchStopMap(110)->getTotalVolume(), 35);
}

// Cancelling stop limit orders tests
TEST_F(LimitOrderBookTests, TestCancelStopLimitOrderLeavingNonEmptyLimit){
    book->addStopLimitOrder(5, true, 80, 25, 20);
    book->addStopLimitOrder(6, true, 32, 22, 20);
    book->addStopLimitOrder(7, true, 111, 23, 20);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 3);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 223);

    book->cancelStopLimitOrder(6);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 2);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 191);

    book->cancelStopLimitOrder(7);

    EXPECT_EQ(book->searchStopMap(20)->getSize(), 1);
    EXPECT_EQ(book->searchStopMap(20)->getTotalVolume(), 80);
}

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnStopLimitOrderCancel){
    book->addStopLimitOrder(5, true, 80, 25, 20);
    book->addStopLimitOrder(6, true, 32, 23, 20);
    book->addStopLimitOrder(7, true, 111, 25, 20);

    Limit* stop = book->searchStopMap(20);

    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 5);

    book->cancelStopLimitOrder(5);
    
    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 6);
}

TEST_F(LimitOrderBookTests, TestStopLevelHeadOrderChangeOnStopLimitOrderCancelLeavingEmptyLimit){
    book->addStopLimitOrder(5, true, 80, 25, 20);

    Limit* stop = book->searchStopMap(20);

    EXPECT_EQ(stop->getHeadOrder()->getOrderId(), 5);

    book->cancelStopLimitOrder(5);
    
    EXPECT_EQ(stop->getHeadOrder(), nullptr);
}

TEST_F(LimitOrderBookTests, TestCancelStopLimitOrderLeavingEmptyLimit){
    book->addStopLimitOrder(5, true, 80, 21, 20);
    book->addStopLimitOrder(6, true, 80, 20, 15);
    Limit* stop1 = book->searchStopMap(20);
    Limit* stop2 = book->searchStopMap(15);

    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 6);
    EXPECT_EQ(stop1->getLeftChild()->getLimitPrice(), 15);

    book->cancelStopLimitOrder(6);
    
    EXPECT_EQ(book->searchLimitMaps(15, true), nullptr);
    EXPECT_EQ(stop1->getLeftChild(), nullptr);
}

// Modifying stop imit orders tests
TEST_F(LimitOrderBookTests, TestModifyStopLimitOrderToExistingStopLevel){
    book->addStopLimitOrder(111, true, 10, 82, 80);
    book->addStopLimitOrder(112, true, 20, 83, 80);
    book->addStopLimitOrder(113, true, 7, 86, 85);
    book->addStopLimitOrder(114, true, 14, 86, 85);

    book->modifyStopLimitOrder(113, 40, 82, 80);

    Limit* stop1 = book->searchStopMap(80);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(stop1->getTotalVolume(), 70);
    EXPECT_EQ(stop2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyStopLimitOrderToNewStopLevel){
    book->addStopLimitOrder(111, true, 10, 81, 80);
    book->addStopLimitOrder(112, true, 20, 82, 80);
    book->addStopLimitOrder(113, true, 7, 81, 85);
    book->addStopLimitOrder(114, true, 14, 86, 85);

    book->modifyStopLimitOrder(113, 40, 83, 82);

    Limit* stop1 = book->searchStopMap(82);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 114);
    EXPECT_EQ(stop1->getTotalVolume(), 40);
    EXPECT_EQ(stop2->getTotalVolume(), 14);
}

TEST_F(LimitOrderBookTests, TestModifyStopLimitOrderInvalidOrderId){
    book->addStopLimitOrder(111, true, 10, 81, 80);
    book->addStopLimitOrder(112, true, 20, 90, 80);
    book->addStopLimitOrder(113, true, 7, 87, 85);
    book->addStopLimitOrder(114, true, 14, 87, 85);

    book->modifyStopLimitOrder(110, 40, 85, 82);

    Limit* stop1 = book->searchStopMap(80);
    Limit* stop2 = book->searchStopMap(85);

    EXPECT_EQ(stop1->getHeadOrder()->getOrderId(), 111);
    EXPECT_EQ(stop2->getHeadOrder()->getOrderId(), 113);
    EXPECT_EQ(stop1->getTotalVolume(), 30);
    EXPECT_EQ(stop2->getTotalVolume(), 21);
    EXPECT_EQ(book->searchStopMap(82), nullptr);
    EXPECT_EQ(book->searchOrderMap(110), nullptr);
}

// Stop limit orders being triggered tests
TEST_F(LimitOrderBookTests, TestStopLimitOrdersTriggeredToMarketOrderByMarketSellOrder){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 10, 98);

    book->addStopLimitOrder(114, false, 15, 97, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 4);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopLimitOrdersTriggeredToMarketOrderByMarketBuyOrder){
    book->addLimitOrder(111, false, 10, 100);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopLimitOrder(114, true, 15, 100, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 4);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopLimitOrdersPartiallyTriggeredToMarketOrderByMarketSellOrder){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 10, 98);

    book->addStopLimitOrder(114, false, 15, 99, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 10);
    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 99);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 6);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopLimitOrdersPartiallyTriggeredToMarketOrderByMarketBuyOrder){
    book->addLimitOrder(111, false, 10, 100);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopLimitOrder(114, true, 15, 99, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 10);
    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 99);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 6);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopLimitOrdersTriggeredToLimitOrderByMarketSellOrder){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 10, 98);

    book->addStopLimitOrder(114, false, 15, 100, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 99);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 9);
    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 100);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 15);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopLimitOrdersTriggeredToLimitOrderByMarketBuyOrder){
    book->addLimitOrder(111, false, 10, 100);
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopLimitOrder(114, true, 15, 98, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 99);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 9);
    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 15);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

TEST_F(LimitOrderBookTests, TestBuyStopLimitOrderWithEmptySellTree){
    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);

    book->addStopLimitOrder(114, false, 15, 98, 99);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 100);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 99);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy(), nullptr);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestSellStopLimitOrderWithEmptyBuyTree){
    book->addLimitOrder(112, false, 10, 99);
    book->addLimitOrder(113, false, 10, 98);

    book->addStopLimitOrder(114, true, 15, 100, 99);

    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 98);
    EXPECT_EQ(book->getLowestStopBuy()->getLimitPrice(), 99);

    book->marketOrder(115, true, 11);

    EXPECT_EQ(book->getLowestSell(), nullptr);
    EXPECT_EQ(book->getLowestStopBuy(), nullptr);
}

// Stop levels containing stop orders and stop limit orders tests
TEST_F(LimitOrderBookTests, TestStopOrdersAndStopLimitOrdersHeldInSameStopLevel){
    book->addStopLimitOrder(114, false, 15, 97, 99);
    book->addStopOrder(115, false, 26, 99);
    book->addStopLimitOrder(116, false, 5, 99, 99);
    book->addStopOrder(117, false, 19, 99);

    EXPECT_EQ(book->getHighestStopSell()->getTotalVolume(), 65);

    book->addLimitOrder(111, true, 10, 100);
    book->addLimitOrder(112, true, 10, 99);
    book->addLimitOrder(113, true, 60, 98);

    book->marketOrder(115, false, 11);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 98);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 9);
    EXPECT_EQ(book->getLowestSell()->getLimitPrice(), 99);
    EXPECT_EQ(book->getLowestSell()->getTotalVolume(), 5);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}

TEST_F(LimitOrderBookTests, TestStopOrdersAndStopLimitOrdersMoreComplexCase){
    book->addLimitOrder(111, true, 480, 292);
    book->addLimitOrder(112, true, 353, 291);
    book->addLimitOrder(113, true, 108, 289);
    book->addLimitOrder(114, true, 49033, 288);

    book->addStopLimitOrder(115, false, 441, 288, 289);
    book->addStopLimitOrder(116, false, 1000, 287, 288);
    book->addStopOrder(117, false, 3000, 288);
    book->addStopLimitOrder(118, false, 417, 287, 288);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 292);
    EXPECT_EQ(book->getHighestStopSell()->getLimitPrice(), 289);

    book->marketOrder(119, false, 163);
    book->marketOrder(120, false, 337);
    book->marketOrder(121, false, 977);

    EXPECT_EQ(book->getHighestBuy()->getLimitPrice(), 288);
    EXPECT_EQ(book->getHighestBuy()->getTotalVolume(), 43639);
    EXPECT_EQ(book->getHighestStopSell(), nullptr);
}