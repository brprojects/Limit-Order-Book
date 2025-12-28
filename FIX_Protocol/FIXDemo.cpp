#include "FIXEngine.hpp"
#include "FIXMessage.hpp"
#include <iostream>
#include <string>

void printFIXMessage(const std::string& label, const std::string& fixMsg) {
    std::cout << "\n" << label << ":\n";
    std::cout << "Raw: ";
    for (char c : fixMsg) {
        if (c == '\x01') std::cout << "|";
        else std::cout << c;
    }
    std::cout << "\n";
    
    // Parse and display fields
    FIXMessage msg(fixMsg);
    std::cout << "  MsgType: " << msg.getFieldAsChar(FIXMessage::MsgType) << "\n";
    if (msg.hasField(FIXMessage::ClOrdID))
        std::cout << "  ClOrdID: " << msg.getField(FIXMessage::ClOrdID) << "\n";
    if (msg.hasField(FIXMessage::OrderID))
        std::cout << "  OrderID: " << msg.getField(FIXMessage::OrderID) << "\n";
    if (msg.hasField(FIXMessage::Side))
        std::cout << "  Side: " << (msg.getFieldAsChar(FIXMessage::Side) == '1' ? "Buy" : "Sell") << "\n";
    if (msg.hasField(FIXMessage::OrderQty))
        std::cout << "  OrderQty: " << msg.getField(FIXMessage::OrderQty) << "\n";
    if (msg.hasField(FIXMessage::Price))
        std::cout << "  Price: " << msg.getField(FIXMessage::Price) << "\n";
    if (msg.hasField(FIXMessage::ExecType)) {
        char execType = msg.getFieldAsChar(FIXMessage::ExecType);
        std::string execTypeStr;
        switch(execType) {
            case '0': execTypeStr = "New"; break;
            case '1': execTypeStr = "PartialFill"; break;
            case '2': execTypeStr = "Fill"; break;
            case '4': execTypeStr = "Canceled"; break;
            case '5': execTypeStr = "Replaced"; break;
            case '8': execTypeStr = "Rejected"; break;
            default: execTypeStr = "Unknown";
        }
        std::cout << "  ExecType: " << execTypeStr << "\n";
    }
    if (msg.hasField(FIXMessage::Text))
        std::cout << "  Text: " << msg.getField(FIXMessage::Text) << "\n";
}

void runFIXDemo() {
    std::cout << "=== FIX Protocol Demo ===" << std::endl;
    
    Book* book = new Book();
    FIXEngine engine(book);
    
    // Demo 1: New Limit Order (Buy)
    std::cout << "\n--- Demo 1: New Buy Limit Order ---";
    FIXMessage newOrder1;
    newOrder1.setMsgType(FIXMessage::NewOrderSingle);
    newOrder1.setField(FIXMessage::ClOrdID, "1001");
    newOrder1.setField(FIXMessage::Side, FIXMessage::Buy);
    newOrder1.setField(FIXMessage::OrderQty, 100);
    newOrder1.setField(FIXMessage::OrdType, FIXMessage::Limit);
    newOrder1.setField(FIXMessage::Price, 150.50);
    newOrder1.setField(FIXMessage::Symbol, "AAPL");
    newOrder1.setField(FIXMessage::SenderCompID, "CLIENT");
    newOrder1.setField(FIXMessage::TargetCompID, "SERVER");
    newOrder1.setField(FIXMessage::MsgSeqNum, 1);
    
    std::string encodedOrder1 = newOrder1.encode();
    printFIXMessage("Sent Order", encodedOrder1);
    
    std::string response1 = engine.processMessage(encodedOrder1);
    printFIXMessage("Execution Report", response1);
    
    // Demo 2: New Limit Order (Sell)
    std::cout << "\n--- Demo 2: New Sell Limit Order ---";
    FIXMessage newOrder2;
    newOrder2.setMsgType(FIXMessage::NewOrderSingle);
    newOrder2.setField(FIXMessage::ClOrdID, "1002");
    newOrder2.setField(FIXMessage::Side, FIXMessage::Sell);
    newOrder2.setField(FIXMessage::OrderQty, 50);
    newOrder2.setField(FIXMessage::OrdType, FIXMessage::Limit);
    newOrder2.setField(FIXMessage::Price, 151.00);
    newOrder2.setField(FIXMessage::Symbol, "AAPL");
    newOrder2.setField(FIXMessage::SenderCompID, "CLIENT");
    newOrder2.setField(FIXMessage::TargetCompID, "SERVER");
    newOrder2.setField(FIXMessage::MsgSeqNum, 2);
    
    std::string encodedOrder2 = newOrder2.encode();
    printFIXMessage("Sent Order", encodedOrder2);
    
    std::string response2 = engine.processMessage(encodedOrder2);
    printFIXMessage("Execution Report", response2);
    
    // Demo 3: Market Order (will match with sell limit)
    std::cout << "\n--- Demo 3: Market Buy Order (Immediate Fill) ---";
    FIXMessage marketOrder;
    marketOrder.setMsgType(FIXMessage::NewOrderSingle);
    marketOrder.setField(FIXMessage::ClOrdID, "1003");
    marketOrder.setField(FIXMessage::Side, FIXMessage::Buy);
    marketOrder.setField(FIXMessage::OrderQty, 30);
    marketOrder.setField(FIXMessage::OrdType, FIXMessage::Market);
    marketOrder.setField(FIXMessage::Symbol, "AAPL");
    marketOrder.setField(FIXMessage::SenderCompID, "CLIENT");
    marketOrder.setField(FIXMessage::TargetCompID, "SERVER");
    marketOrder.setField(FIXMessage::MsgSeqNum, 3);
    
    std::string encodedMarket = marketOrder.encode();
    printFIXMessage("Sent Order", encodedMarket);
    
    std::string response3 = engine.processMessage(encodedMarket);
    printFIXMessage("Execution Report", response3);
    
    // Demo 4: Modify Order
    std::cout << "\n--- Demo 4: Modify Order ---";
    FIXMessage modifyOrder;
    modifyOrder.setMsgType(FIXMessage::OrderCancelReplaceRequest);
    modifyOrder.setField(FIXMessage::ClOrdID, "1004");
    modifyOrder.setField(FIXMessage::OrigClOrdID, "1001");
    modifyOrder.setField(FIXMessage::Side, FIXMessage::Buy);
    modifyOrder.setField(FIXMessage::OrderQty, 200);
    modifyOrder.setField(FIXMessage::Price, 150.75);
    modifyOrder.setField(FIXMessage::Symbol, "AAPL");
    modifyOrder.setField(FIXMessage::SenderCompID, "CLIENT");
    modifyOrder.setField(FIXMessage::TargetCompID, "SERVER");
    modifyOrder.setField(FIXMessage::MsgSeqNum, 4);
    
    std::string encodedModify = modifyOrder.encode();
    printFIXMessage("Sent Modify", encodedModify);
    
    std::string response4 = engine.processMessage(encodedModify);
    printFIXMessage("Execution Report", response4);
    
    // Demo 5: Cancel Order
    std::cout << "\n--- Demo 5: Cancel Order ---";
    FIXMessage cancelOrder;
    cancelOrder.setMsgType(FIXMessage::OrderCancelRequest);
    cancelOrder.setField(FIXMessage::ClOrdID, "1005");
    cancelOrder.setField(FIXMessage::OrigClOrdID, "1002");
    cancelOrder.setField(FIXMessage::Side, FIXMessage::Sell);
    cancelOrder.setField(FIXMessage::Symbol, "AAPL");
    cancelOrder.setField(FIXMessage::SenderCompID, "CLIENT");
    cancelOrder.setField(FIXMessage::TargetCompID, "SERVER");
    cancelOrder.setField(FIXMessage::MsgSeqNum, 5);
    
    std::string encodedCancel = cancelOrder.encode();
    printFIXMessage("Sent Cancel", encodedCancel);
    
    std::string response5 = engine.processMessage(encodedCancel);
    printFIXMessage("Execution Report", response5);
    
    // Demo 6: Stop Order
    std::cout << "\n--- Demo 6: Stop Order ---";
    FIXMessage stopOrder;
    stopOrder.setMsgType(FIXMessage::NewOrderSingle);
    stopOrder.setField(FIXMessage::ClOrdID, "1006");
    stopOrder.setField(FIXMessage::Side, FIXMessage::Sell);
    stopOrder.setField(FIXMessage::OrderQty, 75);
    stopOrder.setField(FIXMessage::OrdType, FIXMessage::Stop);
    stopOrder.setField(FIXMessage::StopPx, 145.00);
    stopOrder.setField(FIXMessage::Symbol, "AAPL");
    stopOrder.setField(FIXMessage::SenderCompID, "CLIENT");
    stopOrder.setField(FIXMessage::TargetCompID, "SERVER");
    stopOrder.setField(FIXMessage::MsgSeqNum, 6);
    
    std::string encodedStop = stopOrder.encode();
    printFIXMessage("Sent Order", encodedStop);
    
    std::string response6 = engine.processMessage(encodedStop);
    printFIXMessage("Execution Report", response6);
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    
    delete book;
}

int main() {
    runFIXDemo();
    return 0;
}
