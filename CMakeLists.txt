cmake_minimum_required(VERSION 3.20)
project(PlanG LANGUAGES CXX)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#WARNINGS
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

# Core library (headers - only for now)
add_library(ggi INTERFACE)


target_include_directories(ggi INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)

# Demo app
add_executable(ggi_demo src/main.cpp)
target_link_libraries(ggi_demo PRIVATE ggi)

# --- Tests
# --- Benchmarks