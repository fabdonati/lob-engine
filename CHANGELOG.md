# Changelog

## 0.1.0

- Added a C++20 price-time-priority limit order book with matching and cancellation support
- Added CMake build targets for the library, tests, replay driver, and benchmark executable
- Added scenario tests for best levels, partial fills, time priority, and cancellation behavior
- Documented replay commands and matching rules
- Fixed validation to reject invalid nonpositive order prices
