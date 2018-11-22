#!/bin/sh

＃初始化环境变量
source config.sh

# 获取当前路径
NOW_DIR=$(cd `dirname $0`; pwd)

# 待编译的库目录名称
MY_LIBS_NAME=libjpeg-turbo
# 源代码路径
MY_SOURCE_DIR=$NOW_DIR/libjpeg-turbo

#编译的过程中产生的中间件的存放目录
BINARY_DIR=binary

#NDK路径
ANDROID_NDK_ROOT=/media/byhook/backup/android/android-ndk-r16b
BUILD_PLATFORM=linux-x86_64
AOSP_TOOLCHAIN_SUFFIX=4.9
AOSP_API=21

LIBS_DIR=$NOW_DIR/libs
echo "LIBS_DIR="$LIBS_DIR


# 构建中间文件
BUILD_DIR=./${BINARY_DIR}/${AOSP_ABI}

# 最终编译的安装目录
PREFIX=${LIBS_DIR}/${AOSP_ABI}/
SYSROOT=${ANDROID_NDK_ROOT}/platforms/android-${AOSP_API}/${AOSP_ARCH}

export CFLAGS="$AOSP_FLAGS -D__ANDROID_API__=${AOSP_API} --sysroot=${SYSROOT} \
               -isystem ${ANDROID_NDK_ROOT}/sysroot/usr/include \
               -isystem ${ANDROID_NDK_ROOT}/sysroot/usr/include/${HOST} "
export LDFLAGS=-pie

TOOLCHAIN=${ANDROID_NDK_ROOT}/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/${BUILD_PLATFORM}

#创建当前编译目录
mkdir -p ${BUILD_DIR}
mkdir -p ${PREFIX}
cd ${BUILD_DIR}

cat <<EOF >toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ${PROCESSOR})
set(CMAKE_C_COMPILER ${TOOLCHAIN}/bin/${HOST}-gcc)
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN}/${HOST})
EOF

cmake -G"Unix Makefiles" \
      -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
      -DCMAKE_POSITION_INDEPENDENT_CODE=1 \
      -DCMAKE_INSTALL_PREFIX=${PREFIX} \
      -DWITH_JPEG8=1 \
      ${MY_SOURCE_DIR}

make clean
make
make install
