# CMakeLists for toolsa library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

# Make sure the linker can find the libraries after they're built
link_directories ($ENV{LROSE_INSTALL_DIR}/lib)

# Make sure the compiler can find include files
include_directories ($ENV{LROSE_INSTALL_DIR}/include)

add_executable (tdrp_gen
               ctype_list.c
               enum_list.c
               parse_defs.c
               set_item.c
               struct_list.c
               tdrp_gen.c
               write_c++_files.c
               write_c_files.c
)

# Link the executable to appropriate libraries
target_link_libraries (
	tdrp
)
