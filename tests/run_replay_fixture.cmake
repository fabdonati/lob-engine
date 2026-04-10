if(NOT DEFINED REPLAY_EXECUTABLE)
    message(FATAL_ERROR "REPLAY_EXECUTABLE is required")
endif()

if(NOT DEFINED COMMANDS_FILE)
    message(FATAL_ERROR "COMMANDS_FILE is required")
endif()

if(NOT DEFINED EXPECTED_FILE)
    message(FATAL_ERROR "EXPECTED_FILE is required")
endif()

execute_process(
    COMMAND "${REPLAY_EXECUTABLE}" "${COMMANDS_FILE}"
    RESULT_VARIABLE replay_status
    OUTPUT_VARIABLE replay_output
    ERROR_VARIABLE replay_error
)

if(NOT replay_status EQUAL 0)
    message(FATAL_ERROR "Replay command failed:\n${replay_error}")
endif()

file(READ "${EXPECTED_FILE}" expected_output)

string(REPLACE "\r\n" "\n" replay_output "${replay_output}")
string(REPLACE "\r\n" "\n" expected_output "${expected_output}")

if(NOT replay_output STREQUAL expected_output)
    message(
        FATAL_ERROR
        "Replay output mismatch.\nExpected:\n${expected_output}\nActual:\n${replay_output}"
    )
endif()
