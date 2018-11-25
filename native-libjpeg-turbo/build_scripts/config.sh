

#架构
if [ "$#" -lt 1 ]; then
	THE_ARCH=armv7
else
	THE_ARCH=$(tr [A-Z] [a-z] <<< "$1")
fi

#根据不同架构配置环境变量
case "$THE_ARCH" in
  arm|armv5|armv6|armv7|armeabi)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	HOST="arm-linux-androideabi"
	AOSP_ABI="armeabi"
	AOSP_ARCH="arch-arm"
	AOSP_FLAGS="-march=armv7-a -mfloat-abi=softfp -fprefetch-loop-arrays"
	PROCESSOR="arm"
	;;
  armv7a|armeabi-v7a)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	HOST="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arch-arm"
	AOSP_FLAGS="-march=armv7-a -mfloat-abi=softfp -fprefetch-loop-arrays"
	PROCESSOR="arm"
	;;
  armv8|armv8a|aarch64|arm64|arm64-v8a)
	TOOLCHAIN_BASE="aarch64-linux-android"
	HOST="aarch64-linux-android"
	AOSP_ABI="arm64-v8a"
	AOSP_ARCH="arch-arm64"
	AOSP_FLAGS=""
	PROCESSOR="aarch64"
	;;
  x86)
	TOOLCHAIN_BASE="x86"
	HOST="i686-linux-android"
	AOSP_ABI="x86"
	AOSP_ARCH="arch-x86"
	AOSP_FLAGS=""
	PROCESSOR="i386"
	;;
  x86_64|x64)
	TOOLCHAIN_BASE="x86_64"
	HOST="x86_64-linux-android"
	AOSP_ABI="x86_64"
	AOSP_ARCH="arch-x86_64"
	AOSP_FLAGS=""
	PROCESSOR="x86_64"
	;;
  *)
	echo "ERROR: Unknown architecture $1"
	[ "$0" = "$BASH_SOURCE" ] && exit 1 || return 1
	;;
esac

echo "TOOLCHAIN_BASE="$TOOLCHAIN_BASE
echo "TOOLNAME_BASE="$TOOLNAME_BASE
echo "AOSP_ABI="$AOSP_ABI
echo "AOSP_ARCH="$AOSP_ARCH
echo "AOSP_FLAGS="$AOSP_FLAGS
echo "HOST="$HOST
