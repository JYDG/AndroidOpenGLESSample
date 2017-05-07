package com.github.mugku.androidopenglessampleproject.ndk;

// Wrapper for native library

public class GLES3JNILib {

     static {
          System.loadLibrary("Hello_Triangle");
//          System.loadLibrary("native-lib");
     }

    //     public static native String stringFromJNI();
     public static native void init(int width, int height);
     public static native void step();
}
