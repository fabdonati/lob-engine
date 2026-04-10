#include "lob_engine/order_book.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>

int main() {
    lob_engine::OrderBook book;
    constexpr int iterations = 50'000;
    std::int64_t add_count = 0;
    std::int64_t cancel_count = 0;
    std::int64_t cancel_hit_count = 0;
    std::int64_t trade_count = 0;
    std::int64_t traded_quantity = 0;
    const auto start = std::chrono::steady_clock::now();

    for (int index = 0; index < iterations; ++index) {
        const int id = index + 1;
        const auto side = index % 2 == 0 ? lob_engine::Side::Buy : lob_engine::Side::Sell;
        const int price = side == lob_engine::Side::Buy ? 100 - (index % 5) : 101 + (index % 5);
        const auto trades = book.add_order({id, side, price, 10});
        ++add_count;
        trade_count += static_cast<std::int64_t>(trades.size());
        for (const auto& trade : trades) {
            traded_quantity += trade.quantity;
        }
        if (index % 10 == 0) {
            ++cancel_count;
            if (book.cancel_order(id)) {
                ++cancel_hit_count;
            }
        }
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const auto elapsed_seconds = static_cast<double>(elapsed_us) / 1'000'000.0;
    const auto operations = add_count + cancel_count;
    const auto operations_per_second =
        elapsed_seconds > 0.0 ? static_cast<double>(operations) / elapsed_seconds : 0.0;

    std::cout << "processed_orders=" << iterations << '\n';
    std::cout << "add_count=" << add_count << '\n';
    std::cout << "cancel_count=" << cancel_count << '\n';
    std::cout << "cancel_hit_count=" << cancel_hit_count << '\n';
    std::cout << "trade_count=" << trade_count << '\n';
    std::cout << "traded_quantity=" << traded_quantity << '\n';
    std::cout << "remaining_orders=" << book.size() << '\n';
    std::cout << "elapsed_us=" << elapsed_us << '\n';
    std::cout << "operations_per_second=" << static_cast<std::int64_t>(operations_per_second)
              << '\n';
    return 0;
}
