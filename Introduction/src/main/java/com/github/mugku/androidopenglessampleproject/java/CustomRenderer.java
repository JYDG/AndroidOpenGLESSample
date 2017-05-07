package com.github.mugku.androidopenglessampleproject.java;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CustomRenderer implements GLSurfaceView.Renderer {

    private static String TAG = "CustomRenderer";
    private int programObject;
    private int width;
    private int height;
    private FloatBuffer vertices;
    private final float[] verticesData =
            { 0.0f, 0.5f, 0.0f,
             -0.5f, -0.5f, 0.0f,
              0.5f, -0.5f, 0.0f };

    String vShaderStr =
            "#version 300 es 			  \n"
                    +   "in vec4 vPosition;           \n"
                    + "void main()                  \n"
                    + "{                            \n"
                    + "   gl_Position = vPosition;  \n"
                    + "}                            \n";

    String fShaderStr =
            "#version 300 es		 			          	\n"
                    + "precision mediump float;					  	\n"
                    + "out vec4 fragColor;	 			 		  	\n"
                    + "void main()                                  \n"
                    + "{                                            \n"
                    + "  fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );	\n"
                    + "}                                            \n";


    public CustomRenderer (Context context) {
        vertices = ByteBuffer.allocateDirect(verticesData.length * 4)
                                .order(ByteOrder.nativeOrder())
                                .asFloatBuffer();

        vertices.put(verticesData).position(0);
    }

    private int LoadShader (int type, String shaderSrc) {
        int shader;
        int[] compiled = new int[1];

        // Create the shader object
        shader = GLES30.glCreateShader (type);

        if (0 == shader) {
            return 0;
        }

        // Load the shader source
        GLES30.glShaderSource(shader, shaderSrc);

        // Compile the shader
        GLES30.glCompileShader(shader);

        // Check the compile status
        GLES30.glGetShaderiv(shader, GLES30.GL_COMPILE_STATUS, compiled, 0);

        if (0 == compiled[0]) {
            Log.e (TAG, GLES30.glGetShaderInfoLog(shader));
            GLES30.glDeleteShader (shader);
            return 0;
        }

        return shader;
    }

    @Override
    public void onSurfaceCreated (GL10 glUnused, EGLConfig config) {
        int vertexShader;
        int fragmentShader;
        int programObject;
        int[] linked = new int[1];

        // Load the vertex/fragment shaders
        vertexShader = LoadShader (GLES30.GL_VERTEX_SHADER, vShaderStr);
        fragmentShader = LoadShader (GLES30.GL_FRAGMENT_SHADER, fShaderStr);

        // Create the program object
        programObject = GLES30.glCreateProgram();

        if (0 == programObject) {
            return;
        }

        GLES30.glAttachShader (programObject, vertexShader);
        GLES30.glAttachShader (programObject, fragmentShader);

        // Bind vPosition to attribute 0
        GLES30.glBindAttribLocation (programObject, 0, "vPosition");

        // Link the program
        GLES30.glLinkProgram (programObject);

        // Check the link status
        GLES30.glGetProgramiv (programObject, GLES30.GL_LINK_STATUS, linked, 0);

        if (0 == linked[0]) {
            Log.e ( TAG, "Error linking program:" );
            Log.e ( TAG, GLES30.glGetProgramInfoLog ( programObject ) );
            GLES30.glDeleteProgram ( programObject );
            return;
        }

        // Store the program object
        this.programObject = programObject;

        GLES30.glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
    }

    @Override
    public void onDrawFrame (GL10 glUnused) {
        // Set the viewport
        GLES30.glViewport (0, 0, width, height);

        // Clear the color buffer
        GLES30.glClear (GLES30.GL_COLOR_BUFFER_BIT);

        // Use the program object
        GLES30.glUseProgram (programObject);

        // Load the vertex data
        GLES30.glVertexAttribPointer (0, 3, GLES30.GL_FLOAT, false, 0, vertices);
        GLES30.glEnableVertexAttribArray (0);

        GLES30.glDrawArrays (GLES30.GL_TRIANGLES, 0, 3);
    }

    @Override
    public void onSurfaceChanged (GL10 glUnused, int width, int height) {
        this.width = width;
        this.height = height;
    }
}
