if(NOT DEFINED BENCHMARK_EXECUTABLE)
    message(FATAL_ERROR "BENCHMARK_EXECUTABLE is required")
endif()

execute_process(
    COMMAND "${BENCHMARK_EXECUTABLE}"
    RESULT_VARIABLE benchmark_status
    OUTPUT_VARIABLE benchmark_output
    ERROR_VARIABLE benchmark_error
)

if(NOT benchmark_status EQUAL 0)
    message(FATAL_ERROR "Benchmark command failed:\n${benchmark_error}")
endif()

foreach(required_key IN ITEMS
    "processed_orders="
    "add_count="
    "cancel_count="
    "cancel_hit_count="
    "trade_count="
    "traded_quantity="
    "remaining_orders="
    "elapsed_us="
    "operations_per_second=")
    string(FIND "${benchmark_output}" "${required_key}" match_position)
    if(match_position EQUAL -1)
        message(FATAL_ERROR "Benchmark output missing key: ${required_key}\n${benchmark_output}")
    endif()
endforeach()
