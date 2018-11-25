
cmake_minimum_required(VERSION 3.4.1)

##官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

add_library(native-png
           SHARED
           src/main/cpp/native_png.cpp)

add_library(png
           SHARED
           IMPORTED)

set_target_properties(png
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libpng.so
                    )

add_library(zip
           SHARED
           IMPORTED)

set_target_properties(zip
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libz.so
                    )

#头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include_png)
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include_zlib)

target_link_libraries(native-png
            png
            zip
            android
            jnigraphics
            log)
