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

    public native void yuv2rgb(String imagePath, int width, int height, Surface surface);


}
