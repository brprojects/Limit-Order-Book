#include "FIXMessage.hpp"
#include <chrono>
#include <iomanip>
#include <ctime>

FIXMessage::FIXMessage() {
    // Set default FIX version
    setField(BeginString, "FIX.4.2");
}

FIXMessage::FIXMessage(const std::string& rawMessage) {
    parse(rawMessage);
}

void FIXMessage::setField(int tag, const std::string& value) {
    fields[tag] = value;
}

void FIXMessage::setField(int tag, int value) {
    fields[tag] = std::to_string(value);
}

void FIXMessage::setField(int tag, double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    fields[tag] = oss.str();
}

void FIXMessage::setField(int tag, char value) {
    fields[tag] = std::string(1, value);
}

std::string FIXMessage::getField(int tag) const {
    auto it = fields.find(tag);
    return (it != fields.end()) ? it->second : "";
}

int FIXMessage::getFieldAsInt(int tag) const {
    std::string value = getField(tag);
    return value.empty() ? 0 : std::stoi(value);
}

double FIXMessage::getFieldAsDouble(int tag) const {
    std::string value = getField(tag);
    return value.empty() ? 0.0 : std::stod(value);
}

char FIXMessage::getFieldAsChar(int tag) const {
    std::string value = getField(tag);
    return value.empty() ? '\0' : value[0];
}

bool FIXMessage::hasField(int tag) const {
    return fields.find(tag) != fields.end();
}

char FIXMessage::getMsgType() const {
    return getFieldAsChar(MsgType);
}

void FIXMessage::setMsgType(char type) {
    setField(MsgType, type);
}

std::string FIXMessage::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time);
    #else
        localtime_r(&time, &tm);
    #endif
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d-%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string FIXMessage::encode() const {
    std::ostringstream body;
    
    // Build body (all fields except BeginString, BodyLength, and CheckSum)
    for (const auto& [tag, value] : fields) {
        if (tag != BeginString && tag != BodyLength && tag != CheckSum) {
            body << tag << '=' << value << SOH;
        }
    }
    
    std::string bodyStr = body.str();
    
    // Build header with BodyLength
    std::ostringstream header;
    header << BeginString << '=' << getField(BeginString) << SOH;
    header << BodyLength << '=' << bodyStr.length() << SOH;
    
    // Calculate checksum
    std::string message = header.str() + bodyStr;
    std::string checksum = calculateChecksum(message);
    
    // Complete message
    std::ostringstream fullMessage;
    fullMessage << message << CheckSum << '=' << checksum << SOH;
    
    return fullMessage.str();
}

bool FIXMessage::parse(const std::string& rawMessage) {
    fields.clear();
    
    size_t pos = 0;
    while (pos < rawMessage.length()) {
        // Find the '=' separator
        size_t equalPos = rawMessage.find('=', pos);
        if (equalPos == std::string::npos) break;
        
        // Extract tag
        std::string tagStr = rawMessage.substr(pos, equalPos - pos);
        int tag = std::stoi(tagStr);
        
        // Find the SOH delimiter
        size_t sohPos = rawMessage.find(SOH, equalPos);
        if (sohPos == std::string::npos) sohPos = rawMessage.length();
        
        // Extract value
        std::string value = rawMessage.substr(equalPos + 1, sohPos - equalPos - 1);
        fields[tag] = value;
        
        pos = sohPos + 1;
    }
    
    return !fields.empty();
}

std::string FIXMessage::calculateChecksum(const std::string& message) const {
    int sum = 0;
    for (char c : message) {
        sum += static_cast<unsigned char>(c);
    }
    sum %= 256;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << sum;
    return oss.str();
}
