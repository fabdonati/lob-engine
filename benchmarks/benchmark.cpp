#include "lob_engine/order_book.hpp"

#include <iostream>

int main() {
    lob_engine::OrderBook book;
    std::cout << "book_size=" << book.size() << '\n';
    return 0;
}
