
cmake_minimum_required(VERSION 3.4.1)

##官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

#####################  libjpeg-turbo start #################################

add_library(native-yuv2image
           SHARED
           src/main/cpp/native_yuv2image.cpp
           src/main/cpp/native_yuv2jpeg.cpp
           src/main/cpp/native_yuv2png.cpp
           src/main/cpp/native_yuvutils.cpp)

add_library(jpeg
           SHARED
           IMPORTED)

set_target_properties(jpeg
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libjpeg.so
                    )

add_library(jpeg-turbo
           SHARED
           IMPORTED)

set_target_properties(jpeg-turbo
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libturbojpeg.so)

#头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include)

#####################  libjpeg-turbo end #################################

#####################  libpng start #################################

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

#PNG头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include_png)
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include_zlib)

#####################  libpng end #################################

target_link_libraries(native-yuv2image
            jpeg
            jpeg-turbo
            png
            zip
            android
            jnigraphics
            log)
