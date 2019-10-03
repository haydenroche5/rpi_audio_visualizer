set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)

get_target_property(portaudiocpp_include_dirs portaudiocpp INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(audio_include_dirs audio INTERFACE_INCLUDE_DIRECTORIES)
add_executable(print_samples ${src_dir}/print_samples.cpp)
set_property(TARGET print_samples PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_dir})  
target_include_directories(print_samples PRIVATE ${portaudiocpp_include_dirs} ${audio_include_dirs})
target_link_libraries(print_samples audio ${Boost_LIBRARIES})
