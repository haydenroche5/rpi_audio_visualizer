pkg_check_modules(Fftw3 REQUIRED IMPORTED_TARGET GLOBAL fftw3)
get_target_property(Fftw3_include_dirs PkgConfig::Fftw3 INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Fftw3_link_libs PkgConfig::Fftw3 INTERFACE_LINK_LIBRARIES)
message("Fftw3_include_dirs: ${Fftw3_include_dirs}")
message("Fftw3_link_libs: ${Fftw3_link_libs}")
message("Fftw3_LINK_LIBRARIES: ${Fftw3_LINK_LIBRARIES}")
