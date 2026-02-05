# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Clone GoogleTest (first time only)
git clone --depth 1 https://github.com/google/googletest.git

# Build
zig build

# Run tests
zig build test

# Run executable
./zig-out/bin/LimitOrderBook
```

The project uses Zig's build system with C++20 and -O2 optimization. GoogleTest is integrated as a subdirectory.

## Architecture

This is a high-frequency trading limit order book matching engine implementing FIFO/Price Priority algorithm.

### Core Data Structures (Limit_Order_Book/)

**Three-class hierarchy with hybrid data structure design:**

1. **Book** - Main order book engine
   - Dual AVL trees for buy/sell sides (regular orders)
   - Dual AVL trees for stop orders (stop buys/sells)
   - Hash maps for O(1) lookups: `orderMap`, `limitBuyMap`, `limitSellMap`, `stopMap`
   - Maintains `highestBuy`/`lowestSell` pointers for O(1) best bid/ask access

2. **Limit** - Price level node
   - AVL tree node (parent/leftChild/rightChild pointers)
   - Contains doubly-linked list of orders at this price (headOrder/tailOrder)
   - Tracks totalVolume and size (order count)

3. **Order** - Individual order
   - Doubly-linked list node within a Limit (nextOrder/prevOrder)
   - Back-reference to parentLimit for O(1) limit updates

### Time Complexities
- Add/Cancel/Modify/Execute: O(1) for existing limits, O(log M) for new price levels
- GetBestBid/Offer: O(1)
- AVL rebalancing: O(log M) when triggered

### Supporting Modules

- **OrderPipeline** (Process_Orders/) - Dispatcher pattern for order processing with nanosecond latency measurement
- **GenerateOrders** (Generate_Orders/) - Test data generation with normal distribution

### Order Types Supported
- Market orders
- Limit orders
- Stop orders
- Stop-limit orders

### Key Design Decisions

1. **AVL + Doubly-Linked List Hybrid** - AVL for O(log M) price navigation, doubly-linked list at each level for O(1) FIFO ordering

2. **Single-threaded matching** - Intentional design for FIFO/price priority consistency

3. **Manual memory management** - Uses new/delete with proper cleanup in destructors

4. **Separate stop order trees** - Parallel data structures isolate regular and stop order handling
