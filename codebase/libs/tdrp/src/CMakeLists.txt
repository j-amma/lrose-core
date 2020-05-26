# CMakeLists for toolsa library

cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)

add_library (tdrp
            tdrp/check.c
            tdrp/convert.c
            tdrp/copy.c
            tdrp/find_item.c
            tdrp/load.c
            tdrp/mem.c
            tdrp/override.c
            tdrp/print.c
            tdrp/read_item.c
            tdrp/tdrp.c
            tdrp/tdrp_obsolete.c
            tdrp/tdrpbuf.c
            tdrp/token_list.c
            tdrp/tokenize.c
            tdrp/warn_on_extra_params.c
)

include_directories (./include $ENV{LROSE_INSTALL_DIR}/include)

INSTALL(FILES libtdrp.a
        DESTINATION $ENV{LROSE_INSTALL_DIR}/lib
	)

#add_library (toolsa ${bigend_sources} ${smallend_sources} ${swap_sources})

