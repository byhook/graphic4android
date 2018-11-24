package com.onzhou.graphic.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;

import com.onzhou.common.base.AbsBaseActivity;
import com.onzhou.common.task.AssertReleaseTask;
import com.onzhou.graphic.yuv2image.NativeYUV2IMAGE;
import com.onzhou.graphic.yuv2image.R;

import java.io.File;

/**
 * @anchor: andy
 * @date: 18-11-23
 */

public class NativeGraphicActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_yuv2image);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask task = new AssertReleaseTask(this, "input.yuv", this);
        AsyncTaskCompat.executeParallel(task);
    }

    public void onTransferJPEG(View view) {
        NativeYUV2IMAGE yuv420p2jpeg = new NativeYUV2IMAGE();
        File yuvFile = new File(getExternalFilesDir(null), "input.yuv");
        File jpegFile = new File(getExternalFilesDir(null), "output.jpeg");
        yuv420p2jpeg.yuv2jpeg(yuvFile.getAbsolutePath(), jpegFile.getAbsolutePath(), 510, 510);
    }

    public void onTransferPNG(View view){
        NativeYUV2IMAGE yuv2image = new NativeYUV2IMAGE();
        File yuvFile = new File(getExternalFilesDir(null), "input.yuv");
        File jpegFile = new File(getExternalFilesDir(null), "output.png");
        yuv2image.yuv2png(yuvFile.getAbsolutePath(), jpegFile.getAbsolutePath(), 510, 510);
    }

    @Override
    public void onReleaseComplete() {

    }
}
