cmake_minimum_required(VERSION 3.20)
project(lob_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lob_engine
    src/order_book.cpp
)

target_include_directories(lob_engine PUBLIC include)

add_executable(order_book_tests tests/test_order_book.cpp)
target_link_libraries(order_book_tests PRIVATE lob_engine)

add_executable(order_book_benchmark benchmarks/benchmark.cpp)
target_link_libraries(order_book_benchmark PRIVATE lob_engine)

add_executable(order_book_replay src/replay.cpp)
target_link_libraries(order_book_replay PRIVATE lob_engine)

enable_testing()
add_test(NAME order_book_tests COMMAND order_book_tests)
