

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jpeglib.h>

#include "native_image.h"

#ifdef ANDROID

#include <android/log.h>
#include <malloc.h>
#include <string.h>

#define LOG_TAG    "NativeImage"
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
        {"loadJPEGImage", "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) loadJPEGImage}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/libjpeg/turbo/loader/NativeImageLoader");
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

int drawJPEG(const char *input_filename, ANativeWindow_Buffer &nwBuffer) {

    jpeg_decompress_struct jpegInfo;
    jpeg_error_mgr jpegError;
    FILE *input_file;
    JSAMPARRAY buffer;
    int row_width;

    unsigned char *pixel;

    jpegInfo.err = jpeg_std_error(&jpegError);

    if ((input_file = fopen(input_filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", input_filename);
        LOGE("open file error");
        return -1;
    }

    //初始化对象信息
    jpeg_create_decompress(&jpegInfo);

    //指定图片
    jpeg_stdio_src(&jpegInfo, input_file);

    //读取文件头信息,设置默认的解压参数
    jpeg_read_header(&jpegInfo, TRUE);

    //开始解压
    jpeg_start_decompress(&jpegInfo);

    row_width = jpegInfo.output_width * jpegInfo.output_components;

    buffer = (*jpegInfo.mem->alloc_sarray)((j_common_ptr) &jpegInfo, JPOOL_IMAGE,
                                           row_width, 1);

    //一行
    pixel = (unsigned char *) malloc(row_width);
    memset(pixel, 0, row_width);

    uint32_t *line = (uint32_t *) nwBuffer.bits;
    for (int i = 0; i < jpegInfo.output_height; i++) {
        //读取一行数据
        jpeg_read_scanlines(&jpegInfo, buffer, 1);
        pixel = *buffer;
        //根据缩放选取行
        for (int j = 0; j < jpegInfo.output_width; j++) {
            //存储顺序为BGR,BGR,BGR......
            line[j] = ((uint32_t) pixel[3 * j + 2]) << 16
                      | ((uint32_t) pixel[3 * j + 1] << 8)
                      | ((uint32_t) (pixel[3 * j + 0]));
        }
        line = line + nwBuffer.stride;
    }

    free(pixel);
    //完成解压
    jpeg_finish_decompress(&jpegInfo);

    //销毁解压相关信息
    jpeg_destroy_decompress(&jpegInfo);

    //关闭文件句柄
    fclose(input_file);

    return 0;
}

void loadJPEGImage(JNIEnv *env, jobject obj, jstring jpegPath, jobject surface) {

    const char *path = env->GetStringUTFChars(jpegPath, 0);
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
    //绘制JPEG图片
    drawJPEG(path, buffer);

    //解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(jpegPath, path);
    //释放
    ANativeWindow_release(window);
}

