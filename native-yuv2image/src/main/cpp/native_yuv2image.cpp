

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

#include "native_yuv2image.h"
#include "jpeglib.h"

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "NativeYUV2IMAGE"
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
        {"yuv2jpeg", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) yuv2jpeg}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/graphic/yuv2image/NativeYUV2JPEG");
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

/**
 *
 * @param yuv420p
 * @param rgb24
 * @param width
 * @param height
 */
void YUV420P2RGB(unsigned char *yuv420p, unsigned char *rgb24, int width, int height) {
    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int indexY = y * width + x;
            int indexU = width * height + y / 2 * width / 2 + x / 2;
            int indexV = width * height + width * height / 4 + y / 2 * width / 2 + x / 2;

            u_char Y = yuv420p[indexY];
            u_char U = yuv420p[indexU];
            u_char V = yuv420p[indexV];

            rgb24[index++] = Y + 1.402 * (V - 128); //R
            rgb24[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb24[index++] = Y + 1.772 * (U - 128); //B
        }
    }
}

int rgb2jpeg(const char *filename, unsigned char *rgbData, int image_width, int image_height,
             int quality) {

    jpeg_compress_struct jpegCompress;
    jpeg_error_mgr jpegError;

    jpegCompress.err = jpeg_std_error(&jpegError);
    //初始化压缩对象
    jpeg_create_compress(&jpegCompress);

    FILE *pFile = fopen(filename, "wb");
    if (pFile == NULL) {
        LOGE("open file failed");
        return -1;
    }

    jpeg_stdio_dest(&jpegCompress, pFile);

    //设置压缩参数
    jpegCompress.client_data = &pFile;
    jpegCompress.image_width = image_width;
    jpegCompress.image_height = image_height;
    jpegCompress.input_components = 3;
    jpegCompress.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jpegCompress);
    //指定亮度及色度质量
    jpegCompress.q_scale_factor[0] = jpeg_quality_scaling(100);
    jpegCompress.q_scale_factor[1] = jpeg_quality_scaling(100);
    //图像采样率,默认为2 * 2
    jpegCompress.comp_info[0].v_samp_factor = 2;
    jpegCompress.comp_info[0].h_samp_factor = 2;
    //设置压缩质量
    jpeg_set_quality(&jpegCompress, quality, TRUE);  //100 is the highest

    //开始压缩图片
    jpeg_start_compress(&jpegCompress, TRUE);

    JSAMPROW row_pointer[1];

    //从上到下,设置每一个像素
    for (unsigned int i = 0; i < jpegCompress.image_height; i++) {
        row_pointer[0] = rgbData + i * jpegCompress.image_width * 3;
        jpeg_write_scanlines(&jpegCompress, row_pointer, 1);
    }
    //完成压缩
    jpeg_finish_compress(&jpegCompress);

    fclose(pFile);
    pFile = NULL;
    jpeg_destroy_compress(&jpegCompress);
    return 0;
}

void
yuv2jpeg(JNIEnv *env, jobject obj, jstring jyuvPath, jstring jjpegPath, jint width, jint height) {

    const char *yuvPath = env->GetStringUTFChars(jyuvPath, 0);
    const char *jpegPath = env->GetStringUTFChars(jjpegPath, 0);

    FILE *file = fopen(yuvPath, "rb");

    unsigned char *yuv420p = new unsigned char[width * height * 3 / 2];

    fread(yuv420p, 1, width * height * 3 / 2, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV420P转RGB24
    YUV420P2RGB(yuv420p, rgb24, width, height);

    rgb2jpeg(jpegPath, rgb24, width, height, 100);

    //释放内存
    delete[] yuv420p;
    delete[] rgb24;

    //关闭文件句柄
    fclose(file);

    env->ReleaseStringUTFChars(jyuvPath, yuvPath);
    env->ReleaseStringUTFChars(jjpegPath, jpegPath);
}

