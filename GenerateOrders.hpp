#ifndef GENERATEORDERS_HPP
#define GENERATEORDERS_HPP

#include <random>
#include <fstream>

class Book;

class GenerateOrders {
private:
    Book* book;
    int orderId = 10001;
    std::ofstream file;

    // Seed for random number generation
    std::random_device rd;
    std::mt19937 gen;

    void market();
    void modify();
    void add();
    void cancel();

public:
    GenerateOrders(Book* book);
    void createInitialOrders(int numberOfOrders, int centreOfBook);
    void createOrders(int numberOfOrders);
};

#endif