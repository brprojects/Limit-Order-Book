#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>
using namespace std;

Order::Order(int _idNumber, bool _buyOrSell, int _shares, int _limit)
    : idNumber(_idNumber), buyOrSell(_buyOrSell), shares(_shares), limit(_limit), 
    nextOrder(nullptr), prevOrder(nullptr), parentLimit(nullptr) {}

void Order::print(){
    if (parentLimit != nullptr) {
        parentLimit->print();
    } else {
        cout << "No parent limit assigned" << endl;
    }
}
