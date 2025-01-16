cmake_minimum_required(VERSION 3.15)
project(zadanie_4)

set(CMAKE_CXX_STANDARD 20)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add all source files
add_executable(zadanie_4
        src/main.cpp
        src/restriction_map.cpp
        include/restriction_map.h
        include/instance_generator.h
        src/instance_generator.cpp
        include/test_framework.h
        src/test_framework.cpp
        include/map_solver.h
        src/map_solver.cpp
        include/benchmark.h
        src/benchmark.cpp
        src/algorithms/bbd_algorithm.cpp
        src/algorithms/bbb_algorithm.cpp
        src/algorithms/bbb2_algorithm.cpp
        include/algorithms/bbb2_algorithm.h
        include/algorithms/bbb_algorithm.h
        include/algorithms/bbd_algorithm.h
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)