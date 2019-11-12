list(APPEND rendering_srcs
    ${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/GradientCycler.cpp
)

add_library(rendering STATIC ${rendering_srcs})
target_include_directories(rendering
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include/rendering
        ${Boost_INCLUDE_DIRS})
target_link_libraries(rendering led_matrix)
set_target_properties(rendering PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
