cmake_minimum_required(VERSION 3.15.5)

project(matrix LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_PREFIX_PATH /usr/local/)
# set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1) # TODO:     Shouldn't be required in this version
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/modules/)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(PortAudio REQUIRED)
find_package(PortAudioCpp REQUIRED)
find_package(Fftw3 REQUIRED)

set(DEPS_ROOT ${CMAKE_CURRENT_LIST_DIR}/dependencies)

include(${DEPS_ROOT}/external.cmake)

add_subdirectory(dependencies)
add_subdirectory(matrix)
add_subdirectory(bin)
