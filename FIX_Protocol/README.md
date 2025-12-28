# FIX Protocol Implementation


To understand FIX simply, think of it as an accepted standard to communicate between different exchanges. Like a mode for a monkey to communicate to a lion.
A simple implementation of the FIX (Financial Information eXchange) protocol version 4.2 for the Limit Order Book.

## Overview

This implementation provides a lightweight FIX messaging layer that allows trading applications to communicate with the order book using industry-standard FIX protocol messages.

## Features

### Supported Message Types

1. **NewOrderSingle (D)** - Submit new orders
   - Market Orders
   - Limit Orders
   - Stop Orders
   - Stop-Limit Orders

2. **OrderCancelRequest (F)** - Cancel existing orders

3. **OrderCancelReplaceRequest (G)** - Modify existing orders

4. **ExecutionReport (8)** - Order status updates from the engine
   - New order acknowledgments
   - Fill notifications
   - Cancel confirmations
   - Reject notifications

### Key FIX Fields Supported

| Tag | Field Name | Description |
|-----|------------|-------------|
| 8   | BeginString | FIX version (FIX.4.2) |
| 9   | BodyLength | Message body length |
| 10  | CheckSum | Message checksum |
| 11  | ClOrdID | Client order ID |
| 35  | MsgType | Message type |
| 37  | OrderID | Unique order ID |
| 38  | OrderQty | Order quantity |
| 39  | OrdStatus | Order status |
| 40  | OrdType | Order type (Market/Limit/Stop/StopLimit) |
| 44  | Price | Limit price |
| 49  | SenderCompID | Sender ID |
| 54  | Side | Buy (1) or Sell (2) |
| 55  | Symbol | Trading symbol |
| 56  | TargetCompID | Target ID |
| 99  | StopPx | Stop price |
| 150 | ExecType | Execution type |

## Architecture

```
FIX_Protocol/
├── FIXMessage.hpp/cpp    - FIX message parser and encoder
├── FIXEngine.hpp/cpp     - FIX protocol engine
└── FIXDemo.cpp           - Demo application
```

### Components

**FIXMessage**: Handles FIX message encoding/decoding
- Parse raw FIX messages
- Build FIX messages
- Field management
- Checksum calculation

**FIXEngine**: Business logic layer
- Process incoming FIX messages
- Execute orders on the order book
- Generate execution reports
- Handle errors and rejections

## Usage Examples

### 1. Create a Buy Limit Order

```cpp
FIXMessage order;
order.setMsgType(FIXMessage::NewOrderSingle);
order.setField(FIXMessage::ClOrdID, "1001");
order.setField(FIXMessage::Side, FIXMessage::Buy);
order.setField(FIXMessage::OrderQty, 100);
order.setField(FIXMessage::OrdType, FIXMessage::Limit);
order.setField(FIXMessage::Price, 150.50);
order.setField(FIXMessage::Symbol, "AAPL");

std::string fixMessage = order.encode();
```

### 2. Process FIX Message

```cpp
Book* book = new Book();
FIXEngine engine(book);

std::string response = engine.processMessage(fixMessage);
// response contains execution report
```

### 3. Cancel an Order

```cpp
FIXMessage cancel;
cancel.setMsgType(FIXMessage::OrderCancelRequest);
cancel.setField(FIXMessage::ClOrdID, "1002");
cancel.setField(FIXMessage::OrigClOrdID, "1001"); // Original order ID
cancel.setField(FIXMessage::Side, FIXMessage::Buy);
cancel.setField(FIXMessage::Symbol, "AAPL");

std::string response = engine.processMessage(cancel.encode());
```

### 4. Modify an Order

```cpp
FIXMessage modify;
modify.setMsgType(FIXMessage::OrderCancelReplaceRequest);
modify.setField(FIXMessage::ClOrdID, "1003");
modify.setField(FIXMessage::OrigClOrdID, "1001");
modify.setField(FIXMessage::Side, FIXMessage::Buy);
modify.setField(FIXMessage::OrderQty, 200);
modify.setField(FIXMessage::Price, 151.00);
modify.setField(FIXMessage::Symbol, "AAPL");

std::string response = engine.processMessage(modify.encode());
```

## Running the Demo

### Build
```bash
cd build
cmake --build .
```

### Run
```bash
.\FIXDemo.exe
```

The demo will show:
1. Buy limit order creation
2. Sell limit order creation
3. Market order execution (immediate fill)
4. Order modification
5. Order cancellation
6. Stop order creation

## FIX Message Format

FIX messages use SOH (Start of Header, ASCII 0x01) as field delimiters:

```
8=FIX.4.2|9=73|35=D|49=CLIENT|56=SERVER|34=1|11=1001|55=AAPL|54=1|38=100|40=2|44=150.50|10=135|
```

Where `|` represents SOH character.

## Extension Points

To extend this implementation:

1. **Add more message types**: Implement handlers in `FIXEngine`
2. **Add more fields**: Define constants in `FIXMessage`
3. **Custom validation**: Add validation logic in message handlers
4. **Session management**: Add FIX session layer with sequence numbers and heartbeats
5. **Network layer**: Add TCP/IP socket handling for real FIX sessions

## Limitations

This is a simplified implementation for demonstration purposes:

- No session-level FIX protocol (logon, logout, heartbeat)
- No message recovery or resend requests
- Limited field validation
- In-memory only (no persistence)
- Single-threaded

## References

- [FIX Protocol Official Documentation](https://www.fixtrading.org/)
- FIX 4.2 Specification
