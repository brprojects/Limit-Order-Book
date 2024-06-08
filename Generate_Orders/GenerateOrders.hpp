#ifndef GENERATEORDERS_HPP
#define GENERATEORDERS_HPP

#include <random>
#include <fstream>

class Book;

class GenerateOrders {
private:
    Book* book;
    int orderId = 11001;
    std::ofstream file;

    // Seed for random number generation
    std::random_device rd;
    std::mt19937 gen;

    void market();
    void addLimit();
    void cancelLimit();
    void modifyLimit();
    void addLimitMarket();
    void addStop();
    void cancelStop();
    void modifyStop();
    void addStopLimit();
    void cancelStopLimit();
    void modifyStopLimit();

public:
    GenerateOrders(Book* book);
    void createInitialOrders(int numberOfOrders, int centreOfBook);
    void createOrders(int numberOfOrders);
};

#endif