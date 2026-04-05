#include "lob_engine/order_book.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

using lob_engine::OrderBook;
using lob_engine::Side;
using lob_engine::Trade;

int main() {
    try {
        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Buy, 100, 10}));
            static_cast<void>(book.add_order({2, Side::Buy, 101, 5}));
            static_cast<void>(book.add_order({3, Side::Sell, 105, 7}));

            const auto best_bid = book.best_bid();
            const auto best_ask = book.best_ask();
            if (!best_bid || best_bid->price != 101 || best_bid->quantity != 5) {
                throw std::runtime_error("best bid mismatch");
            }
            if (!best_ask || best_ask->price != 105 || best_ask->quantity != 7) {
                throw std::runtime_error("best ask mismatch");
            }
        }

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Sell, 101, 4}));
            const auto trades = book.add_order({2, Side::Buy, 102, 6});
            if (trades.size() != 1) {
                throw std::runtime_error("expected one trade");
            }
            const Trade& trade = trades.front();
            if (trade.buy_order_id != 2 || trade.sell_order_id != 1 || trade.price != 101 ||
                trade.quantity != 4) {
                throw std::runtime_error("trade details mismatch");
            }
            const auto best_bid = book.best_bid();
            if (!best_bid || best_bid->price != 102 || best_bid->quantity != 2) {
                throw std::runtime_error("resting remainder mismatch");
            }
        }

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Sell, 101, 3}));
            static_cast<void>(book.add_order({2, Side::Sell, 101, 4}));
            const auto trades = book.add_order({3, Side::Buy, 102, 5});
            if (trades.size() != 2) {
                throw std::runtime_error("expected two trades");
            }
            if (trades[0].sell_order_id != 1 || trades[0].quantity != 3) {
                throw std::runtime_error("price-time priority mismatch");
            }
            if (trades[1].sell_order_id != 2 || trades[1].quantity != 2) {
                throw std::runtime_error("partial fill mismatch");
            }
        }

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Buy, 100, 10}));
            if (!book.cancel_order(1)) {
                throw std::runtime_error("expected cancel to succeed");
            }
            if (book.cancel_order(1)) {
                throw std::runtime_error("duplicate cancel should fail");
            }
            if (book.size() != 0 || book.best_bid().has_value()) {
                throw std::runtime_error("book should be empty after cancel");
            }
        }

        {
            OrderBook book;
            bool threw = false;
            try {
                static_cast<void>(book.add_order({1, Side::Buy, 0, 10}));
            } catch (const std::invalid_argument&) {
                threw = true;
            }
            if (!threw) {
                throw std::runtime_error("expected invalid price rejection");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
