#include <iostream>
#include <unordered_map>
#include "Limit.hpp"
#include "Order.hpp"
#include "Book.hpp"

int main() {
    Book* book = new Book();

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

    book->cancelOrder(7);

    std::cout << limit1->getLeftChild()->getLimitPrice() << std::endl;
    std::cout << limit2->getParent()->getLimitPrice() << std::endl;
    std::cout << limit3->getLeftChild() << std::endl;
    std::cout << limit4->getLeftChild()->getLimitPrice() << std::endl;
    std::cout << limit4->getRightChild()->getLimitPrice() << std::endl;
    std::cout << limit4->getParent()->getLimitPrice() << std::endl;

    // Clear the maps and release memory
    delete book;

    return 0;
}