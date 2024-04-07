#include <iostream>
#include <unordered_map>
#include "Limit.hpp"
#include "Order.hpp"
#include "Book.hpp"

int main() {
    Book* book = new Book();

    // Create some Order objects
    book->addOrder(1, true, 100, 100);
    book->addOrder(4, true, 100, 110);
    book->addOrder(2, true, 50, 120);
    book->addOrder(3, true, 75, 90);

    // Print the Limit
    book->printLimit(100, true);

    book->printOrder(1);

    // Clear the maps and release memory
    delete book;

    return 0;
}