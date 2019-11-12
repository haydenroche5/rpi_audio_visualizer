add_library(audio
    ${CMAKE_CURRENT_LIST_DIR}/src/FftMagnitudePrinter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Recorder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/RecorderStream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SampleCollector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/SamplePrinter.cpp)
# TODO: generators?
target_include_directories(audio
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include/audio)
target_link_libraries(audio
    PUBLIC
        Boost::headers
        fftwpp
        PkgConfig::PortAudioCpp)
set_target_properties(audio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
