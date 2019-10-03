list(APPEND audio_srcs
    src/Recorder.cpp
    src/RecorderStream.cpp
    src/SamplePrinter.cpp
)

add_library(audio STATIC ${audio_srcs})
target_include_directories(audio PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${Boost_INCLUDE_DIRS})
target_link_libraries(audio portaudiocpp)

# target_include_directories(  )

# ADD_EXECUTABLE( anyExecutable myMain.cpp )

# TARGET_LINK_LIBRARIES( anyExecutable LINK_PUBLIC ${Boost_LIBRARIES} )