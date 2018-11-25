package com.onzhou.graphic.libyuv;

import android.view.Surface;

/**
 * @anchor: andy
 * @date: 18-11-23
 */

public class NativeYUV {

    static {
        System.loadLibrary("native-yuv");
    }

    /**
     * 转换类型
     */
    public interface Type {
        int YUV420P_TO_RGB24 = 0;
        int NV12_TO_RGB24 = 1;
        int NV21_TO_RGB24 = 2;
    }

    public native void yuv2rgb(String yuvPath, int type, int width, int height, Surface surface);

}
