#include "OrderPipeline.hpp"
#include "GenerateOrders.hpp"
#include "Book.hpp"
#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    Book* book = new Book();

    OrderPipeline orderPipeline(book);

    // GenerateOrders generateOrders(book);

    // generateOrders.createInitialOrders(10000, 300);

    orderPipeline.processOrdersFromFile("../initialOrders.txt");

    // generateOrders.createOrders(100000);


    // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();

    orderPipeline.processOrdersFromFile("../Orders.txt");

    // Stop measuring time
    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken to process orders: " << duration.count() << " milliseconds" << std::endl;

    std::cout << book->getBuyTree()->getLimitPrice() << std::endl;
    std::vector<int> vec2 = book->inOrderTreeTraversal(book->getBuyTree());
    std::cout << "[";
    for (size_t i = 0; i < vec2.size(); ++i) {
        std::cout << vec2[i];
        if (i != 0 && i != vec2.size()-1 && vec2[i] < vec2[i-1]) {
            throw std::runtime_error("Error: vector is error");
        }
        if (i != vec2.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;

    std::cout << book->getSellTree()->getLimitPrice() << std::endl;
    std::vector<int> vec3 = book->inOrderTreeTraversal(book->getSellTree());
    std::cout << "[";
    for (size_t i = 0; i < vec3.size(); ++i) {
        std::cout << vec3[i];
        if (i != 0 && i != vec3.size()-1 && vec3[i] < vec3[i-1]) {
            throw std::runtime_error("Error: Vector is error");
        }
        if (i != vec3.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;

    delete book;
    return 0;
}