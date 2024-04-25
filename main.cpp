#include <iostream>
#include <unordered_map>
#include "Limit.hpp"
#include "Order.hpp"
#include "Book.hpp"

int main() {
    Book* book = new Book();

    // Create some Order objects
    book->addOrder(1, false, 100, 100);
    book->addOrder(4, false, 100, 110);
    book->addOrder(2, false, 50, 120);
    book->addOrder(3, false, 75, 90);
    book->addOrder(5, false, 30, 90);
    book->addOrder(6, false, 50, 90);
    book->addOrder(7, false, 10, 90);


    // Print the Limit
    book->printLimit(100, false);
    book->searchLimitMaps(100, false)->getHeadOrder()->print();

    book->cancelOrder(1);

    book->printLimit(100, false);
    book->searchLimitMaps(100, false)->getHeadOrder()->print();

    book->cancelOrder(7);

    // Clear the maps and release memory
    delete book;

    return 0;
}