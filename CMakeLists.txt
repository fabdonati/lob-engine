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
add_test(
    NAME replay_basic_lifecycle
    COMMAND ${CMAKE_COMMAND}
        -DREPLAY_EXECUTABLE=$<TARGET_FILE:order_book_replay>
        -DCOMMANDS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/examples/basic_lifecycle.txt
        -DEXPECTED_FILE=${CMAKE_CURRENT_SOURCE_DIR}/tests/replay_basic_lifecycle.expected.txt
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_replay_fixture.cmake
)
add_test(
    NAME benchmark_smoke
    COMMAND ${CMAKE_COMMAND}
        -DBENCHMARK_EXECUTABLE=$<TARGET_FILE:order_book_benchmark>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_benchmark_smoke.cmake
)
