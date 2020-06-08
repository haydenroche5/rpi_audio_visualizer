cmake_minimum_required(VERSION 3.15.5)

project(matrix LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")

set(DEPS_ROOT ${CMAKE_CURRENT_LIST_DIR}/dependencies)

add_subdirectory(dependencies)
add_subdirectory(matrix)
add_subdirectory(bin)
