#ifndef FIXMESSAGE_HPP
#define FIXMESSAGE_HPP

#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>

class FIXMessage {
public:
    // FIX field tags (common ones)
    static constexpr int BeginString = 8;
    static constexpr int BodyLength = 9;
    static constexpr int MsgType = 35;
    static constexpr int SenderCompID = 49;
    static constexpr int TargetCompID = 56;
    static constexpr int MsgSeqNum = 34;
    static constexpr int SendingTime = 52;
    static constexpr int CheckSum = 10;
    
    // Order fields
    static constexpr int ClOrdID = 11;
    static constexpr int OrderID = 37;
    static constexpr int Side = 54;
    static constexpr int OrderQty = 38;
    static constexpr int OrdType = 40;
    static constexpr int Price = 44;
    static constexpr int StopPx = 99;
    static constexpr int Symbol = 55;
    static constexpr int ExecType = 150;
    static constexpr int OrdStatus = 39;
    static constexpr int LeavesQty = 151;
    static constexpr int CumQty = 14;
    static constexpr int AvgPx = 6;
    static constexpr int Text = 58;
    static constexpr int OrigClOrdID = 41;
    
    // Message Types
    static constexpr char NewOrderSingle = 'D';
    static constexpr char OrderCancelRequest = 'F';
    static constexpr char OrderCancelReplaceRequest = 'G';
    static constexpr char ExecutionReport = '8';
    static constexpr char Reject = '3';
    
    // Side values
    static constexpr char Buy = '1';
    static constexpr char Sell = '2';
    
    // OrdType values
    static constexpr char Market = '1';
    static constexpr char Limit = '2';
    static constexpr char Stop = '3';
    static constexpr char StopLimit = '4';
    
    // Exec Type values
    static constexpr char New = '0';
    static constexpr char PartialFill = '1';
    static constexpr char Fill = '2';
    static constexpr char Canceled = '4';
    static constexpr char Replaced = '5';
    static constexpr char Rejected = '8';
    
    FIXMessage();
    FIXMessage(const std::string& rawMessage);
    
    void setField(int tag, const std::string& value);
    void setField(int tag, int value);
    void setField(int tag, double value);
    void setField(int tag, char value);
    
    std::string getField(int tag) const;
    int getFieldAsInt(int tag) const;
    double getFieldAsDouble(int tag) const;
    char getFieldAsChar(int tag) const;
    
    bool hasField(int tag) const;
    
    std::string encode() const;
    bool parse(const std::string& rawMessage);
    
    char getMsgType() const;
    void setMsgType(char type);
    
private:
    std::unordered_map<int, std::string> fields;
    
    std::string calculateChecksum(const std::string& message) const;
    std::string getCurrentTimestamp() const;
    static constexpr char SOH = '\x01'; // FIX field delimiter
};

#endif
