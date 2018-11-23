package com.onzhou.graphic.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.onzhou.common.base.AbsBaseActivity;
import com.onzhou.common.task.AssertReleaseTask;
import com.onzhou.graphic.yuv2rgb.NativeYUV2RGB;

import java.io.File;

/**
 * @anchor: andy
 * @date: 2018-11-23
 * @description:
 */
public class NativeYUV2RGBActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask task = new AssertReleaseTask(this, "input.yuv", this);
        AsyncTaskCompat.executeParallel(task);
    }

    private void setupView() {
        mSurfaceView = new SurfaceView(this);
        setContentView(mSurfaceView);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                NativeYUV2RGB nativeImageLoader = new NativeYUV2RGB();
                File file = new File(getExternalFilesDir(null), "input.yuv");
                nativeImageLoader.yuv2rgb(file.getAbsolutePath(), 510, 510, holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    @Override
    public void onReleaseSuccess(String filePath) {

    }

}
