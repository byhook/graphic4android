package com.onzhou.graphic.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.onzhou.common.base.AbsBaseActivity;
import com.onzhou.common.task.AssertReleaseTask;
import com.onzhou.graphic.png.NativePngLoader;

import java.io.File;

/**
 * @anchor: andy
 * @date: 18-11-22
 */

public class NativePngActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask task = new AssertReleaseTask(this, "input.png", this);
        AsyncTaskCompat.executeParallel(task);
    }

    private void setupView() {
        mSurfaceView = new SurfaceView(this);
        setContentView(mSurfaceView);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                NativePngLoader nativeImageLoader = new NativePngLoader();
                File file = new File(getExternalFilesDir(null), "input.png");
                nativeImageLoader.loadPNGImage(file.getAbsolutePath(), holder.getSurface());
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
    public void onReleaseComplete() {

    }

}
