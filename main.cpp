#include <iostream>
#include "Limit.hpp"
#include "Order.hpp"

int main() {
    // Create a Limit
    Limit limit(100);

    // Create some Order objects
    Order order1(1, true, 100, 100);
    Order order2(2, false, 50, 100);
    Order order3(3, true, 75, 100);

    // Append the Order objects to the Limit
    limit.append(&order1);
    limit.append(&order2);
    limit.append(&order3);

    // Print the Limit forwards
    std::cout << "Limit forwards:" << std::endl;
    limit.printForward();

    // Print the Limit backwards
    std::cout << "Limit backwards:" << std::endl;
    limit.printBackward();
    limit.print();

    order1.print();

    return 0;
}