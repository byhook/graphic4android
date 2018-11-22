
cmake_minimum_required(VERSION 3.4.1)


add_library(gauss-bitmap
           SHARED
           src/main/cpp/gauss_bitmap.c)

find_library(log-lib
            log)

find_library(android-lib
            android)

find_library(jnigraphics-lib
            jnigraphics)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

target_link_libraries(gauss-bitmap ${log-lib} ${android-lib} ${jnigraphics-lib})
