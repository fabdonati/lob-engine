#pragma once

#include <deque>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace lob_engine {

enum class Side { Buy, Sell };

struct Order {
    int id;
    Side side;
    int price;
    int quantity;
};

struct Trade {
    int buy_order_id;
    int sell_order_id;
    int price;
    int quantity;
};

struct Level {
    int price;
    int quantity;
};

class OrderBook {
  public:
    [[nodiscard]] std::vector<Trade> add_order(Order order);
    [[nodiscard]] bool cancel_order(int order_id);
    [[nodiscard]] std::optional<Level> best_bid() const;
    [[nodiscard]] std::optional<Level> best_ask() const;
    [[nodiscard]] int size() const noexcept;

  private:
    using BuyLevels = std::map<int, std::deque<Order>, std::greater<int>>;
    using SellLevels = std::map<int, std::deque<Order>>;

    std::vector<Trade> match_buy(Order& incoming);
    std::vector<Trade> match_sell(Order& incoming);

    template <typename Levels>
    static int aggregate_quantity(const Levels& levels, int price);

    BuyLevels bids_;
    SellLevels asks_;
    std::unordered_map<int, Side> order_sides_;
};

}  // namespace lob_engine
