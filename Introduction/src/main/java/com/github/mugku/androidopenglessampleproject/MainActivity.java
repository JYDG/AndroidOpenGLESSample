package com.github.mugku.androidopenglessampleproject;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    private final int CONTEXT_CLIENT_VERSION = 3;
    private GLSurfaceView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);

        if (detectOpenGLES30()) {
            glSurfaceView.setEGLContextClientVersion (CONTEXT_CLIENT_VERSION);
            glSurfaceView.setRenderer (new CustomRenderer(this));
        }
        else {
            finish();
        }

        setContentView ( glSurfaceView );
    }

    private boolean detectOpenGLES30() {
        ActivityManager am = (ActivityManager) getSystemService (Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return ( info.reqGlEsVersion >= 0x30000 );
    }

    @Override
    protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        glSurfaceView.onPause();
    }


//    static {
//        System.loadLibrary("native-lib");
//    }
//    public native String stringFromJNI();
}
