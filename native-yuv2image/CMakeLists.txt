
cmake_minimum_required(VERSION 3.4.1)

##官方标准配置
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

add_library(native-yuv2image
           SHARED
           src/main/cpp/native_yuv2image.cpp
           src/main/cpp/native_yuv2jpeg.cpp
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

target_link_libraries(native-yuv2image
            jpeg
            jpeg-turbo
            android
            jnigraphics
            log)
