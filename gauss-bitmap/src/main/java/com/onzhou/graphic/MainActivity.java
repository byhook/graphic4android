package com.onzhou.graphic;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity {

    private ImageView gaussImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupViews();
    }

    private void setupViews() {
        gaussImageView = (ImageView) findViewById(R.id.iv_image);
    }

    public void onGaussClick(View view) {
        //获取Bitmap对象
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.input);
        gaussImageView.setImageBitmap(bitmap);
        //开始高斯模糊
        GaussBitmap.gaussBitmap(bitmap, 50);
    }
}
