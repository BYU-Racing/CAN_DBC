cmake_minimum_required(VERSION 3.16)
project(can_parser_tests LANGUAGES CXX)

include(CTest)
include(FetchContent)

# Download Catch2 into the build directory.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Catch2)

# Disable dbcppp's optional components.
set(build_kcd OFF CACHE BOOL "" FORCE)
set(build_tools OFF CACHE BOOL "" FORCE)
set(build_tests OFF CACHE BOOL "" FORCE)
set(build_examples OFF CACHE BOOL "" FORCE)

# Source and build directories must both be given for an external folder.
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../dbcppp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbcppp"
)
target_compile_definitions(dbcppp PRIVATE DBCPPP_EXPORT)

add_executable(can_tests
    test_signal_map.cpp
    test_can_parser.cpp
    ../signal_map.cpp
    ../can_parser.cpp
)

target_compile_features(can_tests PRIVATE cxx_std_17)
target_include_directories(can_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
target_link_libraries(can_tests PRIVATE
    dbcppp
    Catch2::Catch2WithMain
)
target_compile_definitions(can_tests PRIVATE
    TEST_DBC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../BYU_Racing_CAN.dbc"
)

# Make dbcppp's DLL available when running tests on Windows.
add_custom_command(TARGET can_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:dbcppp>
        $<TARGET_FILE_DIR:can_tests>
    VERBATIM
)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(can_tests)