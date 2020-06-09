set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(print_fft_mag_samples ${src_dir}/print_fft_mag_samples.cpp)
set_property(TARGET print_fft_mag_samples PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
target_link_libraries(print_fft_mag_samples
    audio
    Boost::thread
)

add_executable(visualizer ${src_dir}/visualizer.cpp)
set_property(TARGET visualizer PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})
target_link_libraries(visualizer
    audio
    Boost::thread
    led_matrix
    rendering
)

get_target_property(led_matrix_include_dirs led_matrix INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(rendering_include_dirs rendering INTERFACE_INCLUDE_DIRECTORIES)
add_executable(draw_gradients ${src_dir}/draw_gradients.cpp)
set_property(TARGET draw_gradients PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
target_include_directories(draw_gradients PRIVATE
    ${led_matrix_include_dirs}
    ${rendering_include_dirs}
)
target_link_libraries(draw_gradients
    led_matrix
    rendering
)
