//
// Created by byhook on 18-11-24.
//

#ifndef GRAPHIC4ANDROID_NATIVE_YUV2JPEG_H
#define GRAPHIC4ANDROID_NATIVE_YUV2JPEG_H


#include <sys/types.h>

namespace YUV2JPEG {


    int
    RGB24_TO_JPEG(const char *filename, u_char *rgbData, int image_width, int image_height,
                  int quality);

    void YUV420P_TO_JPEG(const char *yuvPath, const char *jpegPath, int width, int height);

};


#endif //GRAPHIC4ANDROID_NATIVE_YUV2JPEG_H
