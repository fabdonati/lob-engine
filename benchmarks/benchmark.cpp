#include "lob_engine/order_book.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

struct BenchmarkStats {
    std::int64_t processed_orders = 0;
    std::int64_t add_count = 0;
    std::int64_t cancel_count = 0;
    std::int64_t cancel_hit_count = 0;
    std::int64_t modify_count = 0;
    std::int64_t modify_hit_count = 0;
    std::int64_t trade_count = 0;
    std::int64_t traded_quantity = 0;
    std::int64_t total_operations = 0;
    std::int64_t remaining_orders = 0;
    std::int64_t elapsed_us = 0;
    std::int64_t operations_per_second = 0;
};

enum class Scenario { Resting, Matching, Lifecycle };

void record_add(
    lob_engine::OrderBook& book,
    BenchmarkStats& stats,
    int id,
    lob_engine::Side side,
    int price,
    int quantity
) {
    const auto trades = book.add_order({id, side, price, quantity});
    ++stats.processed_orders;
    ++stats.add_count;
    for (const auto& trade : trades) {
        ++stats.trade_count;
        stats.traded_quantity += trade.quantity;
    }
}

void record_cancel(lob_engine::OrderBook& book, BenchmarkStats& stats, int id) {
    ++stats.cancel_count;
    if (book.cancel_order(id)) {
        ++stats.cancel_hit_count;
    }
}

void record_modify(
    lob_engine::OrderBook& book,
    BenchmarkStats& stats,
    int id,
    int price,
    int quantity
) {
    ++stats.modify_count;
    const auto result = book.modify_order(id, price, quantity);
    if (result.found) {
        ++stats.modify_hit_count;
    }
    for (const auto& trade : result.trades) {
        ++stats.trade_count;
        stats.traded_quantity += trade.quantity;
    }
}

BenchmarkStats run_resting_scenario() {
    lob_engine::OrderBook book;
    BenchmarkStats stats;
    constexpr int iterations = 50'000;

    const auto start = std::chrono::steady_clock::now();
    for (int index = 0; index < iterations; ++index) {
        const int id = index + 1;
        const auto side = index % 2 == 0 ? lob_engine::Side::Buy : lob_engine::Side::Sell;
        const int price = side == lob_engine::Side::Buy ? 95 - (index % 5) : 105 + (index % 5);
        record_add(book, stats, id, side, price, 10);
        if (index % 10 == 0) {
            record_cancel(book, stats, id);
        }
    }
    const auto end = std::chrono::steady_clock::now();

    stats.total_operations = stats.add_count + stats.cancel_count + stats.modify_count;
    stats.remaining_orders = book.size();
    stats.elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return stats;
}

BenchmarkStats run_matching_scenario() {
    lob_engine::OrderBook book;
    BenchmarkStats stats;
    constexpr int resting_orders = 25'000;
    int next_id = 1;

    const auto start = std::chrono::steady_clock::now();
    for (int index = 0; index < resting_orders; ++index) {
        record_add(book, stats, next_id++, lob_engine::Side::Sell, 101 + (index % 3), 10);
    }
    for (int index = 0; index < resting_orders; ++index) {
        record_add(book, stats, next_id++, lob_engine::Side::Buy, 110, 10);
    }
    const auto end = std::chrono::steady_clock::now();

    stats.total_operations = stats.add_count + stats.cancel_count + stats.modify_count;
    stats.remaining_orders = book.size();
    stats.elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return stats;
}

BenchmarkStats run_lifecycle_scenario() {
    lob_engine::OrderBook book;
    BenchmarkStats stats;
    constexpr int iterations = 12'500;
    int next_id = 1;

    const auto start = std::chrono::steady_clock::now();
    for (int index = 0; index < iterations; ++index) {
        const int buy_id = next_id++;
        const int sell_id = next_id++;
        record_add(book, stats, buy_id, lob_engine::Side::Buy, 99, 10);
        record_add(book, stats, sell_id, lob_engine::Side::Sell, 105, 10);
        record_modify(book, stats, buy_id, 106, 8);
        record_cancel(book, stats, sell_id);
    }
    const auto end = std::chrono::steady_clock::now();

    stats.total_operations = stats.add_count + stats.cancel_count + stats.modify_count;
    stats.remaining_orders = book.size();
    stats.elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return stats;
}

Scenario parse_scenario(int argc, char** argv) {
    if (argc == 1) {
        return Scenario::Resting;
    }
    if (argc == 3 && std::string_view(argv[1]) == "--scenario") {
        const std::string_view value(argv[2]);
        if (value == "resting") {
            return Scenario::Resting;
        }
        if (value == "matching") {
            return Scenario::Matching;
        }
        if (value == "lifecycle") {
            return Scenario::Lifecycle;
        }
    }

    throw std::invalid_argument(
        "usage: order_book_benchmark [--scenario resting|matching|lifecycle]"
    );
}

std::string scenario_name(Scenario scenario) {
    switch (scenario) {
        case Scenario::Resting:
            return "resting";
        case Scenario::Matching:
            return "matching";
        case Scenario::Lifecycle:
            return "lifecycle";
    }
    return "unknown";
}

void finalize_rates(BenchmarkStats& stats) {
    const auto elapsed_seconds = static_cast<double>(stats.elapsed_us) / 1'000'000.0;
    stats.operations_per_second = elapsed_seconds > 0.0
        ? static_cast<std::int64_t>(static_cast<double>(stats.total_operations) / elapsed_seconds)
        : 0;
}

void print_stats(const BenchmarkStats& stats, const std::string& scenario) {
    std::cout << "scenario=" << scenario << '\n';
    std::cout << "processed_orders=" << stats.processed_orders << '\n';
    std::cout << "add_count=" << stats.add_count << '\n';
    std::cout << "cancel_count=" << stats.cancel_count << '\n';
    std::cout << "cancel_hit_count=" << stats.cancel_hit_count << '\n';
    std::cout << "modify_count=" << stats.modify_count << '\n';
    std::cout << "modify_hit_count=" << stats.modify_hit_count << '\n';
    std::cout << "trade_count=" << stats.trade_count << '\n';
    std::cout << "traded_quantity=" << stats.traded_quantity << '\n';
    std::cout << "total_operations=" << stats.total_operations << '\n';
    std::cout << "remaining_orders=" << stats.remaining_orders << '\n';
    std::cout << "elapsed_us=" << stats.elapsed_us << '\n';
    std::cout << "operations_per_second=" << stats.operations_per_second << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const auto scenario = parse_scenario(argc, argv);
        BenchmarkStats stats;
        switch (scenario) {
            case Scenario::Resting:
                stats = run_resting_scenario();
                break;
            case Scenario::Matching:
                stats = run_matching_scenario();
                break;
            case Scenario::Lifecycle:
                stats = run_lifecycle_scenario();
                break;
        }
        finalize_rates(stats);
        print_stats(stats, scenario_name(scenario));
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
