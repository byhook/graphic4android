package com.onzhou.graphic.png;

import android.view.Surface;

/**
 * @anchor: andy
 * @date: 18-11-22
 */

public class NativePngLoader {

    static {
        System.loadLibrary("native-png");
    }

    public native void loadPNGImage(String imagePath, Surface surface);


}
