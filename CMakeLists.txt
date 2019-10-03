cmake_minimum_required(VERSION 3.10)

project(matrix LANGUAGES CXX C)

set(MATRIX_ROOT ${CMAKE_SOURCE_DIR})
set(DEPS_ROOT ${CMAKE_SOURCE_DIR}/dependencies)

include(${DEPS_ROOT}/external.cmake)

add_subdirectory(dependencies)
add_subdirectory(matrix)
add_subdirectory(bin)
