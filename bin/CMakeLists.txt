set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)

# get_target_property(portaudiocpp_include_dirs portaudiocpp INTERFACE_INCLUDE_DIRECTORIES)
# get_target_property(audio_include_dirs audio INTERFACE_INCLUDE_DIRECTORIES)
# add_executable(print_samples ${src_dir}/print_samples.cpp)
# set_property(TARGET print_samples PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
# target_include_directories(print_samples PRIVATE
#     ${portaudiocpp_include_dirs}
#     ${audio_include_dirs}
# )
# target_link_libraries(print_samples
#     audio
#     ${Boost_LIBRARIES}
# )

get_target_property(fftwpp_include_dirs fftwpp INTERFACE_INCLUDE_DIRECTORIES)
add_executable(print_fft_mag_samples ${src_dir}/print_fft_mag_samples.cpp)
set_property(TARGET print_fft_mag_samples PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
target_include_directories(print_fft_mag_samples PRIVATE
    ${portaudiocpp_include_dirs}
    ${audio_include_dirs}
    ${fftwpp_include_dirs}
)
target_link_libraries(print_fft_mag_samples
    audio
    ${Boost_LIBRARIES}
    fftwpp
)

# add_executable(render_fft_mag_bars ${src_dir}/render_fft_mag_bars.cpp)
# set_property(TARGET render_fft_mag_bars PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
# get_target_property(rendering_include_dirs rendering INTERFACE_INCLUDE_DIRECTORIES)
# target_include_directories(render_fft_mag_bars PRIVATE
#     ${portaudiocpp_include_dirs}
#     ${audio_include_dirs}
#     ${rendering_include_dirs}
#     ${fftwpp_include_dirs}
# )
# target_link_libraries(print_fft_mag_samples
#     audio
#     ${Boost_LIBRARIES}
#     fftwpp
# )

# add_executable(rgb_to_hsv ${src_dir}/rgb_to_hsv.cpp)
# set_property(TARGET rgb_to_hsv PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  

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

add_executable(random_freq_bars ${src_dir}/random_freq_bars.cpp)
set_property(TARGET random_freq_bars PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
target_include_directories(random_freq_bars PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${led_matrix_include_dirs}
    ${rendering_include_dirs}
)
target_link_libraries(random_freq_bars
    ${Boost_LIBRARIES}
    led_matrix
    rendering
)

# add_executable(bar_idx_test ${src_dir}/bar_idx_test.cpp)
