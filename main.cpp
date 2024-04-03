#include <iostream>
#include <unordered_map>
#include "Limit.hpp"
#include "Order.hpp"
#include "Book.hpp"

int main() {
    Book* book = new Book();

    // Create a Limit
    book->addLimit(100);

    // Create some Order objects
    book->addOrder(1, true, 100, 100);
    book->addOrder(2, false, 50, 100);
    book->addOrder(3, true, 75, 100);

    // Print the Limit
    book->printLimit(100);

    book->printOrder(1);

    // Clear the maps and release memory
    delete book;

    return 0;
}