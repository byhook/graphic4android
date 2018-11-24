package com.onzhou.graphic.yuv2image;

/**
 * @anchor: andy
 * @date: 18-11-23
 */

public class NativeYUV2IMAGE {

    static {
        System.loadLibrary("native-yuv2image");
    }

    public native void yuv2jpeg(String yuvPath, String jpegPath, int width, int height);

    public native void yuv2png(String yuvPath, String pngPath, int width, int height);

}
