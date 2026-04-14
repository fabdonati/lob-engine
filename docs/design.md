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

## Determinism and measurement

- Replay input is deterministic and comment-tolerant, so the same command file always produces the same trades and top-of-book states
- Benchmark scenarios are deterministic and selected explicitly, so workload comparisons are reproducible
- Modify uses cancel-and-replace semantics, which simplifies the implementation and makes queue-priority tradeoffs explicit

## Benchmark scenarios

- `resting`: emphasizes insertion, level maintenance, and queue growth with minimal matching
- `matching`: emphasizes crossing flow and trade generation against seeded resting liquidity
- `lifecycle`: emphasizes order lookup, modify/cancel churn, and cancel-replace behavior

The benchmark is a regression and workload-contrast tool, not a latency lab. The useful signal is
how the same engine behaves across distinct deterministic workloads.

## Scope

Current scope:

- no hidden/iceberg orders
- no market orders in v0.1.0
- no persistence or networking
