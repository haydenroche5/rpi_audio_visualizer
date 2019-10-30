list(APPEND srcs
    ${CMAKE_CURRENT_LIST_DIR}/src/fftw++.cc
)

find_library(libfftw3_omp fftw3_omp)
if (NOT libfftw3_omp)
    message(FATAL_ERROR "Could not find libfftw3_omp.")
endif ()
find_library(libfftw3 fftw3)
if (NOT libfftw3)
    message(FATAL_ERROR "Could not find libfftw3.")
endif ()
find_library(libm m)
if (NOT libm)
    message(FATAL_ERROR "Could not find libm.")
endif ()
list(APPEND link_libs ${libfftw3_omp} ${libfftw3} ${libm})

add_library(fftwpp STATIC ${srcs})
target_include_directories(fftwpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
# TODO: Can probably remove -g
target_compile_options(fftwpp PRIVATE -O3 -fopenmp -g -Wall -ansi -DNDEBUG -fomit-frame-pointer -fstrict-aliasing -ffast-math -msse2 -mfpmath=sse -march=native)
target_link_libraries(fftwpp ${link_libs})
