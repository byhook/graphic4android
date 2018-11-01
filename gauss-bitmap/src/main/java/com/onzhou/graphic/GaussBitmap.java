package com.onzhou.graphic;

/**
 * @anchor: andy
 * @date: 18-11-2
 */

public class GaussBitmap {

    static {
        System.loadLibrary("gauss-bitmap");
    }

    public static native int gaussBitmap(Object bitmap, int radius);

}
