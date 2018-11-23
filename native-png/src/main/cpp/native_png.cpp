

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "native_png.h"
#include "png.h"
#include "zlib.h"
#include <malloc.h>
#include <string.h>

#ifdef ANDROID

#include <android/log.h>


#define LOG_TAG    "NativePNG"
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
        {"loadPNGImage", "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) loadPNGImage}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/graphic/png/NativePngLoader");
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

void drawPNG(const char *name, ANativeWindow_Buffer &nwBuffer) {
    FILE *file = fopen(name, "rb");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == NULL) {
        fclose(file);
        return;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(file);
        png_destroy_write_struct(&png_ptr, NULL);
        return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(file);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }
    //开始读文件
    png_init_io(png_ptr, file);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    //获取文件的宽高色深
    int m_width = png_get_image_width(png_ptr, info_ptr);
    int m_height = png_get_image_height(png_ptr, info_ptr);

    //获取图像的色彩类型
    int color_type = png_get_color_type(png_ptr, info_ptr);

    LOGI("width=%d,height=%d,color_type=%d", m_width, m_height, color_type);

    // row_pointers内部存放的就是RGBA数据了
    png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);

    switch (color_type) {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            //RGBA32
            break;
        case PNG_COLOR_TYPE_RGB: {
            //RGB24,没有A通道,就要3位3位的读
            uint32_t *line = (uint32_t *) nwBuffer.bits;
            for (int row = 0; row < m_height; row++) {
                for (int column = 0; column < m_width; column++) {
                    //存储顺序为BGR,BGR,BGR......
                    line[column] = ((uint32_t) row_pointers[row][3 * column + 2]) << 16
                                   | ((uint32_t) row_pointers[row][3 * column + 1]) << 8
                                   | (uint32_t) row_pointers[row][3 * column];
                }
                line = line + nwBuffer.stride;
            }
        }
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    fclose(file);
}


void loadPNGImage(JNIEnv *env, jobject obj, jstring jpegPath, jobject surface) {

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

    //绘制PNG图片
    drawPNG(path, buffer);

    //解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(jpegPath, path);
    //释放
    ANativeWindow_release(window);
}

