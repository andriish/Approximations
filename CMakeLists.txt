cmake_minimum_required(VERSION 3.16)

project(approximations LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(log_speed_bench tests/log_speed_bench.cpp)
target_include_directories(log_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME log_speed_bench COMMAND log_speed_bench --samples 250000)
