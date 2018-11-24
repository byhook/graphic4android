

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

#include "native_yuv2image.h"
#include "native_yuv2jpeg.h"
#include "native_yuv2png.h"


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"yuv2jpeg", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) yuv2jpeg},
        {"yuv2png", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) yuv2png}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/graphic/yuv2image/NativeYUV2IMAGE");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

void
yuv2jpeg(JNIEnv *env, jobject obj, jstring jyuvPath, jstring jjpegPath, jint width, jint height) {

    const char *yuvPath = env->GetStringUTFChars(jyuvPath, 0);
    const char *jpegPath = env->GetStringUTFChars(jjpegPath, 0);

    YUV2JPEG::YUV420P_TO_JPEG(yuvPath, jpegPath, width, height);

    env->ReleaseStringUTFChars(jyuvPath, yuvPath);
    env->ReleaseStringUTFChars(jjpegPath, jpegPath);
}

void
yuv2png(JNIEnv *env, jobject obj, jstring jyuvPath, jstring jjpegPath, jint width, jint height) {

    const char *yuvPath = env->GetStringUTFChars(jyuvPath, 0);
    const char *jpegPath = env->GetStringUTFChars(jjpegPath, 0);

    YUV2PNG::YUV420P_TO_PNG(yuvPath, jpegPath, width, height);

    env->ReleaseStringUTFChars(jyuvPath, yuvPath);
    env->ReleaseStringUTFChars(jjpegPath, jpegPath);
}


