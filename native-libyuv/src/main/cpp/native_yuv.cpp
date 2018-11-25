

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <malloc.h>
#include <string.h>
#include "native_yuv.h"

#ifdef ANDROID

#include <android/log.h>
#include <libyuv.h>

#define LOG_TAG    "NativeYUV"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"yuv2rgb", "(Ljava/lang/String;IIILandroid/view/Surface;)V", (void *) yuv2rgb}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/graphic/libyuv/NativeYUV");
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

void ThrowException(JNIEnv *env, const char *exception, const char *message) {
    jclass clazz = env->FindClass(exception);
    if (NULL != clazz) {
        env->ThrowNew(clazz, message);
    }
}

enum Type {
    TYPE_YUV420P_TO_RGB24 = 0,
    TYPE_NV12_TO_RGB24 = 1,
    TYPE_NV21_TO_RGB24 = 2
};

void I420_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *ubase = &yuvData[width * height];
    unsigned char *vbase = &yuvData[width * height * 5 / 4];
    //YUV420P转RGB24
    libyuv::I420ToRGB24(ybase, width, ubase, width / 2, vbase, width / 2,
                        rgb24,
                        width * 3, width, height);

}

void NV12_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *uvbase = &yuvData[width * height];
    //NV12转RGB24
    libyuv::NV12ToRGB24(ybase, width, uvbase, width,
                        rgb24,
                        width * 3, width, height);

}

void NV21_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *vubase = &yuvData[width * height];
    //NV21转RGB24
    libyuv::NV21ToRGB24(ybase, width, vubase, width,
                        rgb24,
                        width * 3, width, height);

}

void drawYUV(const char *path, int type, int width, int height, ANativeWindow_Buffer buffer) {
    FILE *file = fopen(path, "rb");

    int frameSize = width * height * 3 / 2;

    unsigned char *yuvData = new unsigned char[frameSize];

    fread(yuvData, 1, frameSize, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV转RGB24
    switch (type) {
        case TYPE_YUV420P_TO_RGB24:
            //YUV420P转RGB24
            I420_TO_RGB24(yuvData, rgb24, width, height);
            break;
        case TYPE_NV12_TO_RGB24:
            //YUV420SP转RGB24
            NV12_TO_RGB24(yuvData, rgb24, width, height);
            break;
        case TYPE_NV21_TO_RGB24:
            //YUV420SP转RGB24
            NV21_TO_RGB24(yuvData, rgb24, width, height);
            break;
    }

    uint32_t *line = (uint32_t *) buffer.bits;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            line[x] = rgb24[index * 3] << 16
                      | rgb24[index * 3 + 1] << 8
                      | rgb24[index * 3 + 2];
        }
        line = line + buffer.stride;
    }

    //释放内存
    delete[] yuvData;
    delete[] rgb24;

    //关闭文件句柄
    fclose(file);
}

void yuv2rgb(JNIEnv *env, jobject obj, jstring yuvPath, jint type, jint width, jint height,
             jobject surface) {

    const char *path = env->GetStringUTFChars(yuvPath, 0);

    //获取目标surface
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ThrowException(env, "java/lang/RuntimeException", "unable to get native window");
        return;
    }
    //默认的是RGB_565
    int32_t result = ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to set buffers geometry");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }
    ANativeWindow_acquire(window);

    ANativeWindow_Buffer buffer;
    //锁定窗口的绘图表面
    if (ANativeWindow_lock(window, &buffer, NULL) < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to lock native window");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }

    //绘制YUV420P
    drawYUV(path, type, width, height, buffer);

    //解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(yuvPath, path);
    //释放
    ANativeWindow_release(window);
}

