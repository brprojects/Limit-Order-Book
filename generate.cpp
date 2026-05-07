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

    GenerateOrders generateOrders(book);
    generateOrders.createInitialOrders(10'000, 300);
    orderPipeline.processOrdersFromFile("./Generate_Orders/initialOrders.txt");
    generateOrders.createOrders(5'000'000);
    
    delete book;
    return 0;
}
