# lob-engine

`lob-engine` is a compact C++20 project that implements a price-time-priority limit order book.

## Why this project

This repo is meant to show systems-oriented thinking in a small surface area: explicit data
structures, deterministic behavior, testable matching logic, and lightweight benchmarking.

## Features

- Limit order book with buy and sell queues
- Price-time-priority matching against resting liquidity
- Order cancellation by id
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
CANCEL 1
```

Run it with:

```bash
./build/order_book_replay commands.txt
```

## Benchmark

```bash
./build/order_book_benchmark
```

The benchmark is intentionally simple and is meant as a quick regression signal, not a formal
latency study.
