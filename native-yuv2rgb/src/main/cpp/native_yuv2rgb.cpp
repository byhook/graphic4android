

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <malloc.h>
#include <string.h>

#include "native_yuv2rgb.h"

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "NativeYUV2RGB"
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
    jclass cl = env->FindClass("com/onzhou/graphic/yuv2rgb/NativeYUV2RGB");
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

/**
 * YUV420P转RGB24
 * @param data
 * @param rgb
 * @param width
 * @param height
 */
void YUV420P_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height) {
    int index = 0;
    unsigned char *ybase = data;
    unsigned char *ubase = &data[width * height];
    unsigned char *vbase = &data[width * height * 5 / 4];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYUUVV
            u_char Y = ybase[x + y * width];
            u_char U = ubase[y / 2 * width / 2 + (x / 2)];
            u_char V = vbase[y / 2 * width / 2 + (x / 2)];


            rgb[index++] = Y + 1.402 * (V - 128); //R
            rgb[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb[index++] = Y + 1.772 * (U - 128); //B
        }
    }
}

/**
 * NV12属于YUV420SP格式
 * @param data
 * @param rgb
 * @param width
 * @param height
 */
void NV12_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height) {
    int index = 0;
    unsigned char *ybase = data;
    unsigned char *ubase = &data[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYUVUV
            u_char Y = ybase[x + y * width];
            u_char U = ubase[y / 2 * width + (x / 2) * 2];
            u_char V = ubase[y / 2 * width + (x / 2) * 2 + 1];

            rgb[index++] = Y + 1.402 * (V - 128); //R
            rgb[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb[index++] = Y + 1.772 * (U - 128); //B
        }
    }
}

/**
 * NV12属于YUV420SP格式,android相机默认格式
 * @param data
 * @param rgb
 * @param width
 * @param height
 */
void NV21_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height) {
    int index = 0;

    unsigned char *ybase = data;
    unsigned char *ubase = &data[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYVUVU
            u_char Y = ybase[x + y * width];
            u_char U = ubase[y / 2 * width + (x / 2) * 2 + 1];
            u_char V = ubase[y / 2 * width + (x / 2) * 2];

            rgb[index++] = Y + 1.402 * (V - 128); //R
            rgb[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb[index++] = Y + 1.772 * (U - 128); //B
        }
    }
}

void drawYUV(const char *path, int type, int width, int height, ANativeWindow_Buffer buffer) {
    FILE *file = fopen(path, "rb");

    unsigned char *yuvData = new unsigned char[width * height * 3 / 2];

    fread(yuvData, 1, width * height * 3 / 2, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV转RGB24
    switch (type) {
        case TYPE_YUV420P_TO_RGB24:
            //YUV420P转RGB24
            YUV420P_TO_RGB24(yuvData, rgb24, width, height);
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
            line[x] = rgb24[index * 3 + 2] << 16
                      | rgb24[index * 3 + 1] << 8
                      | rgb24[index * 3];
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

