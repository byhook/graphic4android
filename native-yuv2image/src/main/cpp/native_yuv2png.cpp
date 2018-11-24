//
// Created by byhook on 18-11-24.
//

#include <stdio.h>
#include "native_yuv2png.h"
#include "native_yuvutils.h"
#include "include_png/png.h"


bool YUV2PNG::RGB24_TO_PNG(const char *filename, u_char *rgbData, int width, int height,
                           int quality) {
    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    fp = fopen(filename, "wb");
    if (fp) {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        info_ptr = png_create_info_struct(png_ptr);
        if (png_ptr == NULL || info_ptr == NULL) {
            fclose(fp);
            return false;
        }
        png_init_io(png_ptr, fp);
        png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(png_ptr, info_ptr);
        png_set_packing(png_ptr);
        uint8_t *data = new uint8_t[width * 3];
        memset(data, 255, width * 3);
        int nv_start = width * height;
        uint32_t rgb_index = 0, pos;
        int r, g, b, nv_index, y, ru, rv;
        for (uint32_t i = 0; i < height; ++i) {
            for (uint32_t j = 0; j < width; ++j) {
                data[j * 3 + 0] = rgbData[rgb_index++];
                data[j * 3 + 1] = rgbData[rgb_index++];
                data[j * 3 + 2] = rgbData[rgb_index++];
            }
            png_write_row(png_ptr, data);
        }

        delete data;
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return true;
    }
    return false;
}


void YUV2PNG::YUV420P_TO_PNG(const char *yuvPath, const char *pngPath, int width, int height) {
    FILE *file = fopen(yuvPath, "rb");

    unsigned char *yuv420p = new unsigned char[width * height * 3 / 2];

    fread(yuv420p, 1, width * height * 3 / 2, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV420P转RGB24
    yuvutils::YUV420P_TO_RGB24(yuv420p, rgb24, width, height);

    //RGB24转PNG文件
    RGB24_TO_PNG(pngPath, rgb24, width, height, 100);

    //释放内存
    delete[] yuv420p;
    delete[] rgb24;

    //关闭文件句柄
    fclose(file);
}
