#include "lob_engine/order_book.hpp"

int main() {
    lob_engine::OrderBook book;
    return book.size() == 0 ? 0 : 1;
}

