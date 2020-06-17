set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(test_fft ${src_dir}/test_fft.cpp)
set_property(TARGET test_fft PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})
target_link_libraries(test_fft
    audio
    Boost::thread
)

add_executable(visualizer ${src_dir}/visualizer.cpp)
set_property(TARGET visualizer PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})
target_link_libraries(visualizer
    Boost::program_options
    Boost::thread
    audio
    led_matrix
    rendering
)
