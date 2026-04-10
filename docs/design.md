# Design Notes

## Design goals

- Keep the matching rules explicit and easy to audit
- Prefer deterministic replayability over framework complexity
- Show a clean CMake-based C++ workflow

## Matching rules

- Buy orders match the best available asks at or below their limit price
- Sell orders match the best available bids at or above their limit price
- Trade price is the resting order's price
- Orders at the same price follow time priority
- Modifies use cancel-and-replace semantics, so the replacement loses prior queue priority

## Data structures

- bids: descending price map
- asks: ascending price map
- per-level FIFO queues to preserve time priority
- order-id lookup map to support cancellation and modification lookup

## Scope

This is intentionally a compact engine:

- no hidden/iceberg orders
- no market orders in v0.1.0
- no persistence or networking
