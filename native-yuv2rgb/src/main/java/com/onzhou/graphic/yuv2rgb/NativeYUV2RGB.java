package com.onzhou.graphic.yuv2rgb;

import android.view.Surface;

/**
 * @anchor: andy
 * @date: 2018-11-23
 * @description:
 */
public class NativeYUV2RGB {

    static {
        System.loadLibrary("native-yuv2rgb");
    }

    /**
     * 转换类型
     */
    public interface Type {
        int YUV420P_TO_RGB24 = 0;
        int NV12_TO_RGB24 = 1;
        int NV21_TO_RGB24 = 2;
    }

    public native void yuv2rgb(String imagePath, int type, int width, int height, Surface surface);

}
