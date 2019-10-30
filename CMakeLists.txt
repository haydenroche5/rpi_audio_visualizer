cmake_minimum_required(VERSION 3.10)

project(matrix LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

set(DEPS_ROOT ${CMAKE_SOURCE_DIR}/dependencies)

include(${DEPS_ROOT}/external.cmake)

add_subdirectory(dependencies)
add_subdirectory(matrix)
add_subdirectory(bin)
