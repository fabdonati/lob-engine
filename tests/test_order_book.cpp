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

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Buy, 100, 10}));
            const auto result = book.modify_order(1, 101, 8);
            if (!result.found || !result.trades.empty()) {
                throw std::runtime_error("expected modify to rest updated order");
            }
            const auto best_bid = book.best_bid();
            if (!best_bid || best_bid->price != 101 || best_bid->quantity != 8) {
                throw std::runtime_error("modify should replace resting order details");
            }
        }

        {
            OrderBook book;
            const auto result = book.modify_order(99, 101, 8);
            if (result.found || !result.trades.empty()) {
                throw std::runtime_error("modify miss should return empty result");
            }
        }

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Sell, 101, 4}));
            static_cast<void>(book.add_order({2, Side::Buy, 99, 6}));
            const auto result = book.modify_order(2, 102, 6);
            if (!result.found || result.trades.size() != 1) {
                throw std::runtime_error("modify should match against resting liquidity");
            }
            const Trade& trade = result.trades.front();
            if (trade.buy_order_id != 2 || trade.sell_order_id != 1 || trade.price != 101 ||
                trade.quantity != 4) {
                throw std::runtime_error("modify trade details mismatch");
            }
            const auto best_bid = book.best_bid();
            if (!best_bid || best_bid->price != 102 || best_bid->quantity != 2) {
                throw std::runtime_error("modify should rest remaining quantity at new price");
            }
        }

        {
            OrderBook book;
            static_cast<void>(book.add_order({1, Side::Buy, 100, 5}));
            static_cast<void>(book.add_order({2, Side::Buy, 100, 7}));
            const auto result = book.modify_order(1, 100, 4);
            if (!result.found) {
                throw std::runtime_error("modify should succeed for existing order");
            }
            const auto trades = book.add_order({3, Side::Sell, 100, 6});
            if (trades.size() != 1 || trades.front().buy_order_id != 2 || trades.front().quantity != 6) {
                throw std::runtime_error("modify should reset time priority via cancel-replace");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
