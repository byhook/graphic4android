package com.onzhou.graphic.yuv2jpeg;

/**
 * @anchor: andy
 * @date: 18-11-23
 */

public class NativeYUV2JPEG {

    static {
        System.loadLibrary("native-yuv2jpeg");
    }

    public native void yuv2jpeg(String yuvPath, String bmpPath, int width, int height);


}
