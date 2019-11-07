list(APPEND audio_srcs
    ${CMAKE_CURRENT_LIST_DIR}/src/FftMagnitudePrinter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Recorder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/RecorderStream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SampleCollector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SamplePrinter.cpp
)

add_library(audio STATIC ${audio_srcs})
target_include_directories(audio INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE include/audio ${Boost_INCLUDE_DIRS})
target_link_libraries(audio PkgConfig::PortAudioCpp fftwpp)

# target_include_directories(  )

# ADD_EXECUTABLE( anyExecutable myMain.cpp )

# TARGET_LINK_LIBRARIES( anyExecutable LINK_PUBLIC ${Boost_LIBRARIES} )