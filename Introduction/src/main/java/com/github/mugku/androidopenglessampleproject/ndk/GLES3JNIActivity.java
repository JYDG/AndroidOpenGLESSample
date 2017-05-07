package com.github.mugku.androidopenglessampleproject.ndk;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.File;

public class GLES3JNIActivity extends Activity {

    GLES3JNIView gles3JNIView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        gles3JNIView = new GLES3JNIView(getApplication());
        setContentView(gles3JNIView);
    }

    @Override protected void onPause() {
        super.onPause();
        gles3JNIView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        gles3JNIView.onResume();
    }
}
