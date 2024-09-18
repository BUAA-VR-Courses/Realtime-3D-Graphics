cmake_minimum_required(VERSION 3.16)
# project(rasterization LANGUAGES C CXX)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(rasterization)
add_subdirectory(testsuite)
