# CMakeLists for dataport library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

set (bigend_sources
    bigend/bigend.c
)

set (smallend_sources
    smallend/smallend.c
)

set (swap_sources
    swap/swap.c
)

add_library (dataport ${bigend_sources} ${smallend_sources} ${swap_sources})

include_directories (./include $ENV{LROSE_INSTALL_DIR}/include)

INSTALL(FILES libdataport.a
        DESTINATION $ENV{LROSE_INSTALL_DIR}/lib
	)


