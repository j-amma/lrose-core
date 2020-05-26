# CMakeLists for toolsa library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

# Make sure the linker can find the libraries after they're built
link_directories ($ENV{LROSE_INSTALL_DIR}/lib)

# Make sure the compiler can find include files
include_directories ($ENV{LROSE_INSTALL_DIR}/include)

add_executable (TdrpTest
               Args.cc
               Main.cc
               Params.cc
               TdrpTest.cc
)

# Link the executable to appropriate libraries
target_link_libraries (
	tdrp
)
