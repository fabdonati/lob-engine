#include "lob_engine/order_book.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

lob_engine::Side parse_side(const std::string& token) {
    if (token == "BUY") {
        return lob_engine::Side::Buy;
    }
    if (token == "SELL") {
        return lob_engine::Side::Sell;
    }
    throw std::runtime_error("unknown side: " + token);
}

void print_top_of_book(const lob_engine::OrderBook& book) {
    const auto bid = book.best_bid();
    const auto ask = book.best_ask();

    std::cout << "BID=";
    if (bid) {
        std::cout << bid->price << "x" << bid->quantity;
    } else {
        std::cout << "NONE";
    }

    std::cout << " ASK=";
    if (ask) {
        std::cout << ask->price << "x" << ask->quantity;
    } else {
        std::cout << "NONE";
    }
    std::cout << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: order_book_replay <commands.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "failed to open replay file\n";
        return 1;
    }

    lob_engine::OrderBook book;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream stream(line);
        std::string command;
        stream >> command;

        if (command == "ADD") {
            int id = 0;
            std::string side_token;
            int price = 0;
            int quantity = 0;
            stream >> id >> side_token >> price >> quantity;
            const auto trades = book.add_order({id, parse_side(side_token), price, quantity});
            for (const auto& trade : trades) {
                std::cout << "TRADE buy=" << trade.buy_order_id << " sell=" << trade.sell_order_id
                          << " price=" << trade.price << " qty=" << trade.quantity << '\n';
            }
        } else if (command == "MODIFY") {
            int id = 0;
            int price = 0;
            int quantity = 0;
            stream >> id >> price >> quantity;
            const auto result = book.modify_order(id, price, quantity);
            std::cout << "MODIFY " << id << ' ' << (result.found ? "OK" : "MISS") << '\n';
            for (const auto& trade : result.trades) {
                std::cout << "TRADE buy=" << trade.buy_order_id << " sell=" << trade.sell_order_id
                          << " price=" << trade.price << " qty=" << trade.quantity << '\n';
            }
        } else if (command == "CANCEL") {
            int id = 0;
            stream >> id;
            std::cout << "CANCEL " << id << ' ' << (book.cancel_order(id) ? "OK" : "MISS") << '\n';
        } else {
            std::cerr << "unknown command: " << command << '\n';
            return 1;
        }

        print_top_of_book(book);
    }

    return 0;
}
