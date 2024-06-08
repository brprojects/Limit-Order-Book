#include "./Generate_Orders/GenerateOrders.hpp"
#include "./Process_Orders/OrderPipeline.hpp"
#include "./Limit_Order_Book/Book.hpp"
#include "./Limit_Order_Book/Limit.hpp"
#include "./Limit_Order_Book/Order.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    Book* book = new Book();

    OrderPipeline orderPipeline(book);

    // GenerateOrders generateOrders(book);

    // generateOrders.createInitialOrders(10000, 300);

    orderPipeline.processOrdersFromFile("./initialOrders.txt");

    // generateOrders.createOrders(5000000);


    // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();

    orderPipeline.processOrdersFromFile("./Orders.txt");

    // Stop measuring time
    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken to process orders: " << duration.count() << " milliseconds" << std::endl;

    delete book;
    return 0;
}