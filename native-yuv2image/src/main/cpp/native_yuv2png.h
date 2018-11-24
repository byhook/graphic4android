//
// Created by byhook on 18-11-24.
//

#ifndef GRAPHIC4ANDROID_NATIVE_YUV2PNG_H
#define GRAPHIC4ANDROID_NATIVE_YUV2PNG_H


#include <sys/types.h>

namespace YUV2PNG {


    bool
    RGB24_TO_PNG(const char *filename, u_char *rgbData, int image_width, int image_height,
                  int quality);

    void YUV420P_TO_PNG(const char *yuvPath, const char *pngPath, int width, int height);

};


#endif //GRAPHIC4ANDROID_NATIVE_YUV2PNG_H
