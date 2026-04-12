# Changelog

## 0.1.0

- Added a C++20 price-time-priority limit order book with matching and cancellation support
- Added CMake build targets for the library, tests, replay driver, and benchmark executable
- Added scenario tests for best levels, partial fills, time priority, and cancellation behavior
- Documented replay commands and matching rules
- Fixed validation to reject invalid nonpositive order prices

## Unreleased

- Added modify-order support with cancel-and-replace semantics
- Extended the replay driver with `MODIFY id price quantity`
- Added tests for modify misses, cross-book modifies, and priority reset after replace
- Added a committed replay fixture and a CTest scenario that validates replay output
- Expanded benchmark output with workload counters, trade totals, and throughput
- Added a benchmark smoke test to verify output shape in CTest
- Split the benchmark into deterministic `resting`, `matching`, and `lifecycle` scenarios
- Added explicit scenario reporting, operation totals, and modify counters to the benchmark output
