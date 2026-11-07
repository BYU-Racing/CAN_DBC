cmake_minimum_required(VERSION 3.16)
project(can_parser LANGUAGES CXX)

set(build_kcd OFF CACHE BOOL "" FORCE)
set(build_tools OFF CACHE BOOL "" FORCE)
set(build_tests OFF CACHE BOOL "" FORCE)
set(build_examples OFF CACHE BOOL "" FORCE)

add_subdirectory(dbcppp)

# Export symbols while building the DLL; consumers retain dllimport.
target_compile_definitions(dbcppp PRIVATE DBCPPP_EXPORT)

add_executable(can_parser can_parser.cpp signal_map.cpp)
target_compile_features(can_parser PRIVATE cxx_std_17)
target_link_libraries(can_parser PRIVATE dbcppp)

# dbcppp builds as a shared library; put its DLL beside the executable.
add_custom_command(TARGET can_parser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:dbcppp>
        $<TARGET_FILE_DIR:can_parser>
)
