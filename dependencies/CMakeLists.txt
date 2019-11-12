set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/modules/)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED CONFIG COMPONENTS thread chrono)
set_target_properties(Boost::headers PROPERTIES IMPORTED_GLOBAL TRUE)
set_target_properties(Boost::thread PROPERTIES IMPORTED_GLOBAL TRUE)
set_target_properties(Boost::chrono PROPERTIES IMPORTED_GLOBAL TRUE)

find_package(PkgConfig REQUIRED)
find_package(PortAudio REQUIRED)
find_package(PortAudioCpp REQUIRED)
find_package(Fftw3 REQUIRED)
# find_package(spdlog REQUIRED)

add_subdirectory(led_matrix)
add_subdirectory(fftwpp)
