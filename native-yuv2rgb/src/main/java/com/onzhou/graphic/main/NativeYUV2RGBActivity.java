package com.onzhou.graphic.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.onzhou.common.base.AbsBaseActivity;
import com.onzhou.common.task.AssertReleaseTask;
import com.onzhou.graphic.yuv2rgb.NativeYUV2RGB;
import com.onzhou.graphic.yuv2rgb.R;

import java.io.File;

/**
 * @anchor: andy
 * @date: 2018-11-23
 * @description:
 */
public class NativeYUV2RGBActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private ViewGroup mRootLayer;

    private Button mBtnYuv420p, mBtnNV12, mBtnNV21;

    private SurfaceView mSurfaceView;

    private NativeYUV2RGB mNativeYUV2RGB;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_yuv2rgb);

    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        setupView();
        setupAsserts();
    }

    private void setupAsserts() {
        String[] targetFiles = {"yuv420p.yuv", "nv12.yuv", "nv21.yuv"};
        AssertReleaseTask task = new AssertReleaseTask(this, targetFiles, this);
        AsyncTaskCompat.executeParallel(task);
    }

    private void setupView() {
        mRootLayer = (ViewGroup) findViewById(R.id.native_yub2rgb_root_layer);
        mBtnYuv420p = (Button) findViewById(R.id.btn_yuv420p);
        mBtnNV12 = (Button) findViewById(R.id.btn_nv12);
        mBtnNV21 = (Button) findViewById(R.id.btn_nv21);

        mSurfaceView = new SurfaceView(this);
        mRootLayer.addView(mSurfaceView);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                mNativeYUV2RGB = new NativeYUV2RGB();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    public void onYUV420PToRGB24(View view) {
        File file = new File(getExternalFilesDir(null), "yuv420p.yuv");
        mNativeYUV2RGB.yuv2rgb(file.getAbsolutePath(), NativeYUV2RGB.Type.YUV420P_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }

    public void onNV12ToRGB24(View view) {
        File file = new File(getExternalFilesDir(null), "nv12.yuv");
        mNativeYUV2RGB.yuv2rgb(file.getAbsolutePath(), NativeYUV2RGB.Type.NV12_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }

    public void onNV21ToRGB24(View view) {
        File file = new File(getExternalFilesDir(null), "nv21.yuv");
        mNativeYUV2RGB.yuv2rgb(file.getAbsolutePath(), NativeYUV2RGB.Type.NV21_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }

    @Override
    public void onReleaseComplete() {
        mBtnYuv420p.setEnabled(true);
        mBtnNV12.setEnabled(true);
        mBtnNV21.setEnabled(true);
    }

}
