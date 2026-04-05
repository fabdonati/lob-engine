#include "lob_engine/order_book.hpp"

#include <chrono>
#include <iostream>

int main() {
    lob_engine::OrderBook book;
    constexpr int iterations = 50'000;
    const auto start = std::chrono::steady_clock::now();

    for (int index = 0; index < iterations; ++index) {
        const int id = index + 1;
        const auto side = index % 2 == 0 ? lob_engine::Side::Buy : lob_engine::Side::Sell;
        const int price = side == lob_engine::Side::Buy ? 100 - (index % 5) : 101 + (index % 5);
        static_cast<void>(book.add_order({id, side, price, 10}));
        if (index % 10 == 0) {
            static_cast<void>(book.cancel_order(id));
        }
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "processed_orders=" << iterations << '\n';
    std::cout << "remaining_orders=" << book.size() << '\n';
    std::cout << "elapsed_us=" << elapsed_us << '\n';
    return 0;
}
