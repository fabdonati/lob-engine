#include "lob_engine/order_book.hpp"

#include <stdexcept>

namespace lob_engine {

std::vector<Trade> OrderBook::add_order(Order order) {
    if (order.quantity <= 0) {
        throw std::invalid_argument("order quantity must be positive");
    }
    if (order_sides_.contains(order.id)) {
        throw std::invalid_argument("order id already exists");
    }

    auto trades = order.side == Side::Buy ? match_buy(order) : match_sell(order);
    if (order.quantity > 0) {
        if (order.side == Side::Buy) {
            bids_[order.price].push_back(order);
        } else {
            asks_[order.price].push_back(order);
        }
        order_sides_.emplace(order.id, order.side);
    }

    return trades;
}

bool OrderBook::cancel_order(int order_id) {
    const auto side_it = order_sides_.find(order_id);
    if (side_it == order_sides_.end()) {
        return false;
    }

    if (side_it->second == Side::Buy) {
        for (auto level_it = bids_.begin(); level_it != bids_.end(); ++level_it) {
            auto& queue = level_it->second;
            for (auto order_it = queue.begin(); order_it != queue.end(); ++order_it) {
                if (order_it->id == order_id) {
                    queue.erase(order_it);
                    if (queue.empty()) {
                        bids_.erase(level_it);
                    }
                    order_sides_.erase(side_it);
                    return true;
                }
            }
        }
    } else {
        for (auto level_it = asks_.begin(); level_it != asks_.end(); ++level_it) {
            auto& queue = level_it->second;
            for (auto order_it = queue.begin(); order_it != queue.end(); ++order_it) {
                if (order_it->id == order_id) {
                    queue.erase(order_it);
                    if (queue.empty()) {
                        asks_.erase(level_it);
                    }
                    order_sides_.erase(side_it);
                    return true;
                }
            }
        }
    }

    return false;
}

std::optional<Level> OrderBook::best_bid() const {
    if (bids_.empty()) {
        return std::nullopt;
    }
    const auto& [price, _] = *bids_.begin();
    return Level{price, aggregate_quantity(bids_, price)};
}

std::optional<Level> OrderBook::best_ask() const {
    if (asks_.empty()) {
        return std::nullopt;
    }
    const auto& [price, _] = *asks_.begin();
    return Level{price, aggregate_quantity(asks_, price)};
}

int OrderBook::size() const noexcept { return static_cast<int>(order_sides_.size()); }

std::vector<Trade> OrderBook::match_buy(Order& incoming) {
    std::vector<Trade> trades;
    while (incoming.quantity > 0 && !asks_.empty()) {
        auto best_ask_it = asks_.begin();
        if (best_ask_it->first > incoming.price) {
            break;
        }

        auto& queue = best_ask_it->second;
        auto& resting = queue.front();
        const int traded_quantity = std::min(incoming.quantity, resting.quantity);
        trades.push_back(
            Trade{incoming.id, resting.id, resting.price, traded_quantity}
        );

        incoming.quantity -= traded_quantity;
        resting.quantity -= traded_quantity;
        if (resting.quantity == 0) {
            order_sides_.erase(resting.id);
            queue.pop_front();
            if (queue.empty()) {
                asks_.erase(best_ask_it);
            }
        }
    }
    return trades;
}

std::vector<Trade> OrderBook::match_sell(Order& incoming) {
    std::vector<Trade> trades;
    while (incoming.quantity > 0 && !bids_.empty()) {
        auto best_bid_it = bids_.begin();
        if (best_bid_it->first < incoming.price) {
            break;
        }

        auto& queue = best_bid_it->second;
        auto& resting = queue.front();
        const int traded_quantity = std::min(incoming.quantity, resting.quantity);
        trades.push_back(
            Trade{resting.id, incoming.id, resting.price, traded_quantity}
        );

        incoming.quantity -= traded_quantity;
        resting.quantity -= traded_quantity;
        if (resting.quantity == 0) {
            order_sides_.erase(resting.id);
            queue.pop_front();
            if (queue.empty()) {
                bids_.erase(best_bid_it);
            }
        }
    }
    return trades;
}

template <typename Levels>
int OrderBook::aggregate_quantity(const Levels& levels, int price) {
    const auto level_it = levels.find(price);
    if (level_it == levels.end()) {
        return 0;
    }

    int total = 0;
    for (const auto& order : level_it->second) {
        total += order.quantity;
    }
    return total;
}

}  // namespace lob_engine
