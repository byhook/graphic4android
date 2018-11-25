package com.onzhou.libjpeg.turbo.loader;

import android.view.Surface;

/**
 * @anchor: andy
 * @date: 2018-11-21
 * @description:
 */
public class NativeImageLoader {

    static {
        System.loadLibrary("native-image");
    }


    public native void loadJPEGImage(String imagePath, Surface surface);

}
