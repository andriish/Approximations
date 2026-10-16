cmake_minimum_required(VERSION 3.16)

project(approximations LANGUAGES CXX Fortran)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_Fortran_STANDARD 2008)
set(CMAKE_Fortran_STANDARD_REQUIRED ON)
set(CMAKE_Fortran_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/O2 /DNDEBUG)
else()
    add_compile_options(-O3 -DNDEBUG -march=native -mtune=native -fno-tree-vectorize )
endif()

add_executable(log_speed_bench tests/log_speed_bench.cpp)
target_compile_features(log_speed_bench PRIVATE cxx_std_20)
target_include_directories(log_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(loglog_speed_bench tests/loglog_speed_bench.cpp)
target_compile_features(loglog_speed_bench PRIVATE cxx_std_20)
target_include_directories(loglog_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(loglog_cpp STATIC tests/loglog_c.cpp)
target_compile_features(loglog_cpp PRIVATE cxx_std_20)
target_include_directories(loglog_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(loglog_speed_bench_f90
    loglog.F90
    tests/loglog_speed_bench_f90.f90)

add_executable(loglog_speed_bench_f90_cpp
    loglog.F90
    tests/loglog_speed_bench_cpp_f90.f90)
target_link_libraries(loglog_speed_bench_f90_cpp PRIVATE loglog_cpp)

add_executable(expexp_test tests/expexp_test.cpp)
target_compile_features(expexp_test PRIVATE cxx_std_20)
target_include_directories(expexp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(expexp_speed_bench tests/expexp_speed_bench.cpp)
target_compile_features(expexp_speed_bench PRIVATE cxx_std_20)
target_include_directories(expexp_speed_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME log_speed_bench COMMAND log_speed_bench --samples 25000000)
add_test(NAME loglog_speed_bench COMMAND loglog_speed_bench --samples 5000000)
add_test(NAME loglog_speed_bench_f90 COMMAND loglog_speed_bench_f90 --samples 5000000)
add_test(NAME loglog_speed_bench_f90_cpp COMMAND loglog_speed_bench_f90_cpp --samples 5000000)
add_test(NAME expexp_test COMMAND expexp_test)
add_test(NAME expexp_speed_bench COMMAND expexp_speed_bench --samples 5000000)
