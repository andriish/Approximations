cmake_minimum_required(VERSION 3.16)

project(approximations LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/O2 /DNDEBUG)
else()
    add_compile_options(-O3 -DNDEBUG -march=native)
endif()

add_executable(log_speed_bench tests/log_speed_bench.cpp)
target_compile_features(log_speed_bench PRIVATE cxx_std_20)
target_include_directories(log_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(loglog_speed_bench tests/loglog_speed_bench.cpp)
target_compile_features(loglog_speed_bench PRIVATE cxx_std_20)
target_include_directories(loglog_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME log_speed_bench COMMAND log_speed_bench --samples 25000000)
add_test(NAME loglog_speed_bench COMMAND loglog_speed_bench --samples 5000000)
