#ifndef ORDERPIPELINE_HPP
#define ORDERPIPELINE_HPP

#include <string>
#include <unordered_map>
#include <string_view>
#include <sstream>

class Book;

class OrderPipeline {
private:
    Book& book;

    using OrderFunction = void(OrderPipeline::*)(std::istringstream&);
    std::unordered_map<std::string_view, OrderFunction> orderFunctions;

    void processMarketOrder(std::istringstream& iss);
    void processAddOrder(std::istringstream& iss);
    void processCancelOrder(std::istringstream& iss);
    void processModifyOrder(std::istringstream& iss);

public:
    OrderPipeline(Book& book);
    void processOrdersFromFile(const std::string& filename);
};

#endif