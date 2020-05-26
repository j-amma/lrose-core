cmake_minimum_required (VERSION 3.12)
project (LROSE-CORE)
    
if (POLICY CMP0079)
 cmake_policy (SET CMP0079 NEW)
endif ()

add_subdirectory (libs)
add_subdirectory (apps)

