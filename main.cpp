#include <iostream>
#include <unordered_map>
#include "Limit.hpp"
#include "Order.hpp"

int main() {

    std::unordered_map<int, Order*> ordersMap;

    // Create a Limit
    Limit* limit = new Limit(100);

    // Create some Order objects
    Order* order1 = new Order(1, true, 100, 100);
    Order* order2 = new Order(2, false, 50, 100);
    Order* order3 = new Order(3, true, 75, 100);

    ordersMap.emplace(1, order1);
    ordersMap.emplace(2, order2);
    ordersMap.emplace(3, order3);

    // Append the Order objects to the Limit
    limit->append(order1);
    limit->append(order2);
    limit->append(order3);

    // Print the Limit forwards
    std::cout << "Limit forwards:" << std::endl;
    limit->printForward();

    // Print the Limit backwards
    std::cout << "Limit backwards:" << std::endl;
    limit->printBackward();
    limit->print();

    order1->print();

    ordersMap[1]->print();

    // Don't forget to release memory
    delete limit;
    delete order1;
    delete order2;
    delete order3;

    // Don't forget to clear the map
    // for (auto& pair : ordersMap) {
    //     delete pair.second;
    // }
    ordersMap.clear();

    return 0;
}