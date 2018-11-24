
#include <stdio.h>
#include "native_yuv2jpeg.h"
#include "native_yuvutils.h"
#include "jpeglib.h"


int YUV2JPEG::RGB24_TO_JPEG(const char *filename, u_char *rgbData, int image_width,
                            int image_height,
                            int quality) {
    jpeg_compress_struct jpegCompress;
    jpeg_error_mgr jpegError;

    jpegCompress.err = jpeg_std_error(&jpegError);
    //初始化压缩对象
    jpeg_create_compress(&jpegCompress);

    FILE *pFile = fopen(filename, "wb");
    if (pFile == NULL) {
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

void YUV2JPEG::YUV420P_TO_JPEG(const char *yuvPath, const char *jpegPath, int width, int height) {
    FILE *file = fopen(yuvPath, "rb");

    unsigned char *yuv420p = new unsigned char[width * height * 3 / 2];

    fread(yuv420p, 1, width * height * 3 / 2, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV420P转RGB24
    yuvutils::YUV420P_TO_RGB24(yuv420p, rgb24, width, height);

    RGB24_TO_JPEG(jpegPath, rgb24, width, height, 100);

    //释放内存
    delete[] yuv420p;
    delete[] rgb24;

    //关闭文件句柄
    fclose(file);
}