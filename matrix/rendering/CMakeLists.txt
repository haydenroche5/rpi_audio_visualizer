list(APPEND rendering_srcs
    ${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/FrameBufferManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Renderer.cpp
)

add_library(rendering STATIC ${rendering_srcs})
target_include_directories(rendering INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/rendering)
target_link_libraries(rendering led_matrix)
