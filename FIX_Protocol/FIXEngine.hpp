#ifndef FIXENGINE_HPP
#define FIXENGINE_HPP

#include "FIXMessage.hpp"
#include "../Limit_Order_Book/Book.hpp"
#include <string>
#include <functional>

class FIXEngine {
public:
    FIXEngine(Book* book);
    
    // Process incoming FIX message and return execution report
    std::string processMessage(const std::string& rawMessage);
    
    // Create execution report for order events
    FIXMessage createExecutionReport(int orderID, char execType, char ordStatus,
                                      int leavesQty, int cumQty, double avgPx,
                                      const std::string& clOrdID, char side,
                                      int orderQty, const std::string& symbol);
    
    // Handle different message types
    std::string handleNewOrder(const FIXMessage& msg);
    std::string handleCancelRequest(const FIXMessage& msg);
    std::string handleCancelReplaceRequest(const FIXMessage& msg);
    
    void setSenderCompID(const std::string& id) { senderCompID = id; }
    void setTargetCompID(const std::string& id) { targetCompID = id; }
    
private:
    Book* book;
    std::string senderCompID;
    std::string targetCompID;
    int msgSeqNum;
    
    std::string createReject(const std::string& clOrdID, const std::string& reason);
};

#endif
