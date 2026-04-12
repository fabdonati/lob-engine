# lob-engine

`lob-engine` is a compact C++20 project that implements a price-time-priority limit order book.

## Features

- Limit order book with buy and sell queues
- Price-time-priority matching against resting liquidity
- Order cancellation by id
- Order modification by id with cancel-and-replace semantics
- Best bid / best ask snapshots
- Replay executable for deterministic command files
- Benchmark executable for quick throughput smoke checks

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Replay format

The replay driver accepts a text file with one command per line:

```text
ADD 1 BUY 100 10
ADD 2 SELL 101 4
MODIFY 1 102 8
CANCEL 1
```

Run it with:

```bash
./build/order_book_replay commands.txt
```

`MODIFY id price quantity` removes the existing order and re-inserts the replacement with the
same side and id. In this stage, every modify resets time priority and can trade immediately if
the replacement price crosses resting liquidity.

A committed example scenario lives at `examples/basic_lifecycle.txt`:

```bash
./build/order_book_replay examples/basic_lifecycle.txt
```

## Benchmark

```bash
./build/order_book_benchmark
./build/order_book_benchmark --scenario resting
./build/order_book_benchmark --scenario matching
./build/order_book_benchmark --scenario lifecycle
```

The benchmark is intentionally simple and is meant as a quick regression signal, not a formal
latency study. It now reports:

- `scenario`
- `processed_orders`
- `add_count`
- `cancel_count`
- `cancel_hit_count`
- `modify_count`
- `modify_hit_count`
- `trade_count`
- `traded_quantity`
- `total_operations`
- `remaining_orders`
- `elapsed_us`
- `operations_per_second`

The scenarios are:

- `resting`: mostly non-crossing adds with occasional cancels
- `matching`: resting sell inventory followed by aggressive crossing buys
- `lifecycle`: add / modify / cancel churn with deterministic matches

These are intended as deterministic workload contrasts, not exchange-realistic simulations.
