list(APPEND srcs
    ${CMAKE_CURRENT_LIST_DIR}/src/fftw++.cc
)

find_library(path_to_libfftw3_omp fftw3_omp)
if (NOT libfftw3_omp)
    message(FATAL_ERROR "Could not find libfftw3_omp.")
endif ()

add_library(fftwpp STATIC ${srcs})
target_include_directories(fftwpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_options(fftwpp PRIVATE -O3 -fopenmp -Wall -ansi -DNDEBUG -fomit-frame-pointer -fstrict-aliasing -ffast-math -march=native)
target_link_libraries(fftwpp PRIVATE ${path_to_libfftw3_omp} PkgConfig::Fftw3)
set_target_properties(fftwpp PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
