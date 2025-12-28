#include "FIXEngine.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

FIXEngine::FIXEngine(Book* _book) 
    : book(_book), senderCompID("SERVER"), targetCompID("CLIENT"), msgSeqNum(1) {
}

std::string FIXEngine::processMessage(const std::string& rawMessage) {
    FIXMessage msg(rawMessage);
    
    if (!msg.hasField(FIXMessage::MsgType)) {
        return createReject("", "Missing MsgType");
    }
    
    char msgType = msg.getMsgType();
    
    switch (msgType) {
        case FIXMessage::NewOrderSingle:
            return handleNewOrder(msg);
        case FIXMessage::OrderCancelRequest:
            return handleCancelRequest(msg);
        case FIXMessage::OrderCancelReplaceRequest:
            return handleCancelReplaceRequest(msg);
        default:
            return createReject(msg.getField(FIXMessage::ClOrdID), 
                              "Unsupported message type");
    }
}

std::string FIXEngine::handleNewOrder(const FIXMessage& msg) {
    std::string clOrdID = msg.getField(FIXMessage::ClOrdID);
    char side = msg.getFieldAsChar(FIXMessage::Side);
    int orderQty = msg.getFieldAsInt(FIXMessage::OrderQty);
    char ordType = msg.getFieldAsChar(FIXMessage::OrdType);
    std::string symbol = msg.getField(FIXMessage::Symbol);
    
    if (clOrdID.empty() || orderQty <= 0) {
        return createReject(clOrdID, "Invalid order parameters");
    }
    
    bool buyOrSell = (side == FIXMessage::Buy);
    int orderID = std::stoi(clOrdID); // Use ClOrdID as OrderID for simplicity
    
    try {
        switch (ordType) {
            case FIXMessage::Market: {
                book->marketOrder(orderID, buyOrSell, orderQty);
                return createExecutionReport(orderID, FIXMessage::Fill, '2', 
                                            0, orderQty, 0.0, clOrdID, side, 
                                            orderQty, symbol).encode();
            }
            case FIXMessage::Limit: {
                double price = msg.getFieldAsDouble(FIXMessage::Price);
                if (price <= 0) {
                    return createReject(clOrdID, "Invalid limit price");
                }
                book->addLimitOrder(orderID, buyOrSell, orderQty, static_cast<int>(price));
                return createExecutionReport(orderID, FIXMessage::New, '0',
                                            orderQty, 0, 0.0, clOrdID, side,
                                            orderQty, symbol).encode();
            }
            case FIXMessage::Stop: {
                double stopPx = msg.getFieldAsDouble(FIXMessage::StopPx);
                if (stopPx <= 0) {
                    return createReject(clOrdID, "Invalid stop price");
                }
                book->addStopOrder(orderID, buyOrSell, orderQty, static_cast<int>(stopPx));
                return createExecutionReport(orderID, FIXMessage::New, '0',
                                            orderQty, 0, 0.0, clOrdID, side,
                                            orderQty, symbol).encode();
            }
            case FIXMessage::StopLimit: {
                double price = msg.getFieldAsDouble(FIXMessage::Price);
                double stopPx = msg.getFieldAsDouble(FIXMessage::StopPx);
                if (price <= 0 || stopPx <= 0) {
                    return createReject(clOrdID, "Invalid stop-limit prices");
                }
                book->addStopLimitOrder(orderID, buyOrSell, orderQty, 
                                       static_cast<int>(price), static_cast<int>(stopPx));
                return createExecutionReport(orderID, FIXMessage::New, '0',
                                            orderQty, 0, 0.0, clOrdID, side,
                                            orderQty, symbol).encode();
            }
            default:
                return createReject(clOrdID, "Unsupported order type");
        }
    } catch (const std::exception& e) {
        return createReject(clOrdID, std::string("Order failed: ") + e.what());
    }
}

std::string FIXEngine::handleCancelRequest(const FIXMessage& msg) {
    std::string clOrdID = msg.getField(FIXMessage::ClOrdID);
    std::string origClOrdID = msg.getField(FIXMessage::OrigClOrdID);
    char side = msg.getFieldAsChar(FIXMessage::Side);
    std::string symbol = msg.getField(FIXMessage::Symbol);
    
    if (origClOrdID.empty()) {
        return createReject(clOrdID, "Missing OrigClOrdID");
    }
    
    try {
        int orderID = std::stoi(origClOrdID);
        // Try canceling as limit order first, if fails try stop orders
        try {
            book->cancelLimitOrder(orderID);
        } catch (...) {
            try {
                book->cancelStopOrder(orderID);
            } catch (...) {
                book->cancelStopLimitOrder(orderID);
            }
        }
        
        return createExecutionReport(orderID, FIXMessage::Canceled, '4',
                                    0, 0, 0.0, clOrdID, side, 0, symbol).encode();
    } catch (const std::exception& e) {
        return createReject(clOrdID, std::string("Cancel failed: ") + e.what());
    }
}

std::string FIXEngine::handleCancelReplaceRequest(const FIXMessage& msg) {
    std::string clOrdID = msg.getField(FIXMessage::ClOrdID);
    std::string origClOrdID = msg.getField(FIXMessage::OrigClOrdID);
    char side = msg.getFieldAsChar(FIXMessage::Side);
    int orderQty = msg.getFieldAsInt(FIXMessage::OrderQty);
    double price = msg.getFieldAsDouble(FIXMessage::Price);
    std::string symbol = msg.getField(FIXMessage::Symbol);
    
    if (origClOrdID.empty() || orderQty <= 0 || price <= 0) {
        return createReject(clOrdID, "Invalid modify parameters");
    }
    
    try {
        int orderID = std::stoi(origClOrdID);
        bool buyOrSell = (side == FIXMessage::Buy);
        
        book->modifyLimitOrder(orderID, orderQty, static_cast<int>(price));
        
        return createExecutionReport(orderID, FIXMessage::Replaced, '5',
                                    orderQty, 0, 0.0, clOrdID, side,
                                    orderQty, symbol).encode();
    } catch (const std::exception& e) {
        return createReject(clOrdID, std::string("Modify failed: ") + e.what());
    }
}

FIXMessage FIXEngine::createExecutionReport(int orderID, char execType, char ordStatus,
                                           int leavesQty, int cumQty, double avgPx,
                                           const std::string& clOrdID, char side,
                                           int orderQty, const std::string& symbol) {
    FIXMessage msg;
    msg.setMsgType(FIXMessage::ExecutionReport);
    msg.setField(FIXMessage::SenderCompID, senderCompID);
    msg.setField(FIXMessage::TargetCompID, targetCompID);
    msg.setField(FIXMessage::MsgSeqNum, msgSeqNum++);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time);
    #else
        localtime_r(&time, &tm);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d-%H:%M:%S");
    msg.setField(FIXMessage::SendingTime, oss.str());
    
    msg.setField(FIXMessage::OrderID, orderID);
    msg.setField(FIXMessage::ClOrdID, clOrdID);
    msg.setField(FIXMessage::ExecType, execType);
    msg.setField(FIXMessage::OrdStatus, ordStatus);
    msg.setField(FIXMessage::Side, side);
    msg.setField(FIXMessage::OrderQty, orderQty);
    msg.setField(FIXMessage::LeavesQty, leavesQty);
    msg.setField(FIXMessage::CumQty, cumQty);
    msg.setField(FIXMessage::AvgPx, avgPx);
    msg.setField(FIXMessage::Symbol, symbol);
    
    return msg;
}

std::string FIXEngine::createReject(const std::string& clOrdID, const std::string& reason) {
    FIXMessage msg;
    msg.setMsgType(FIXMessage::Reject);
    msg.setField(FIXMessage::SenderCompID, senderCompID);
    msg.setField(FIXMessage::TargetCompID, targetCompID);
    msg.setField(FIXMessage::MsgSeqNum, msgSeqNum++);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time);
    #else
        localtime_r(&time, &tm);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d-%H:%M:%S");
    msg.setField(FIXMessage::SendingTime, oss.str());
    
    if (!clOrdID.empty()) {
        msg.setField(FIXMessage::ClOrdID, clOrdID);
    }
    msg.setField(FIXMessage::Text, reason);
    
    return msg.encode();
}
