
#include <jni.h>
#include <stdlib.h>
#include "esUtil.h"
#include "Hello_Triangle.h"

#define  LOG_TAG    "hello_triangle"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


char vShaderStr[] =
        "#version 300 es                          \n"
                "layout(location = 0) in vec4 vPosition;  \n"
                "void main()                              \n"
                "{                                        \n"
                "   gl_Position = vPosition;              \n"
                "}                                        \n";

char fShaderStr[] =
        "#version 300 es                              \n"
                "precision mediump float;                     \n"
                "out vec4 fragColor;                          \n"
                "void main()                                  \n"
                "{                                            \n"
                "   fragColor = vec4 (0.0, 1.0, 0.0, 1.0);  \n"
                "}                                        \n";

GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                         -0.5f, -0.5f, 0.0f,
                         0.5f, -0.5f, 0.0f
};

//화면의 중앙이 (0,0)이고 X 축은 오른쪽으로, Y 축은 위쪽으로 증가한다.
//오른쪽 위 모서리 좌표는 (1,1)이며 왼쪽 아래는 (-1, -1)이다.
// 임의의 한 점은 -1 ~ 1 사이의 실수값으로 지정한다.
// glBegin과 glEnd 사이의 glVertex2f 함수로 지정한 세 점이 삼각형의 꼭지점을 가리키고 있으며 이 세 점을 연결하여 삼각형을 그린다. x, y 두 개의 값만 지정했으므로 z 좌표는 자동으로 0이 된다.
//GLfloat vVertices[] = {  0.0f, 0.5f,
//                         -0.5f, -0.5f,
//                         0.5f, -0.5f
//};

static void printGLString(const char* name, GLenum s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

//------------------------------------
class TriangleDataStorage{
public:
    TriangleDataStorage(){}
    virtual ~TriangleDataStorage(){

    }

    GLuint* programObject;
    GLuint* gvPositionHandle;

//    GLuint getProgramObject(){
//        return programObject;
//    }
//
//    GLuint getGvPositionHandle() {
//        return gvPositionHandle;
//    }
//
//    GLuint setProgramObject(GLuint val){
//        programObject = val;
//    }
//
//    GLuint setGvPositionHandle(GLuint val) {
//        gvPositionHandle = val;
//    }

private:
//    GLuint programObject;
//    GLuint gvPositionHandle;
};

//TriangleDataStorage::TriangleDataStorage() {
//
//}
//
//TriangleDataStorage::~TriangleDataStorage() {
//
//}

TriangleDataStorage* createTriangleDataStorage() {
    TriangleDataStorage* triangleDataStorage = new TriangleDataStorage;
    return triangleDataStorage;
}

//GLuint* TriangleDataStorage::getProgramObject() {
//    return &programObject;
//}
//
//GLuint* TriangleDataStorage::getGvPositionHandle() {
//    return &gvPositionHandle;
//}

//GLuint gProgram;
//GLuint gvPositionHandle;

static TriangleDataStorage* triangleDataStorage = NULL;

GLuint loadShader(GLenum shaderType, const char* pShaderSrc) {
    // Create the shader object
    GLuint shader = glCreateShader(shaderType);
    if (shader == 0) {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &pShaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    GLint compiled = 0;
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = (char*) malloc(infoLen);
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGE("Could not compile shader %d:\n%s\n",
                     shaderType, buf);
                free(buf);
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragementShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!fragementShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");

        glAttachShader(program, fragementShader);
        checkGlError("glAttachShader");

        // Link the program
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        // Check the link status
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    triangleDataStorage = createTriangleDataStorage();

    // Store the program object
//    GLuint* program = triangleDataStorage->getProgramObject();
//    GLuint* positionHandle = triangleDataStorage->getGvPositionHandle();

    GLuint program = createProgram(vShaderStr, fShaderStr);
    triangleDataStorage->programObject = &program;
//    if (!program) {
//        LOGE("Could not create program.");
//        return false;
//    }

    GLuint positionHandle = glGetAttribLocation(program, "vPosition");
    triangleDataStorage->gvPositionHandle = &positionHandle;
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n", positionHandle);

    // Set the viewport
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}

// Draw a triangle using the shader pair created in Init()
void renderFrame() {
    // Clear the color buffer
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    // Use the program object
    glUseProgram(*(triangleDataStorage->programObject));
    checkGlError("glUseProgram");

    // Load the vertex data
//    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(*(triangleDataStorage->gvPositionHandle));
    checkGlError("glEnableVertexAttribArray");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

// ----------------------------------------------------------------------------

static Renderer* g_renderer = NULL;

extern "C" {
    JNIEXPORT void JNICALL Java_com_github_mugku_androidopenglessampleproject_ndk_GLES3JNILib_init(JNIEnv* env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_github_mugku_androidopenglessampleproject_ndk_GLES3JNILib_step(JNIEnv* env, jobject obj);
};

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() {
    return GL_TRUE;
}
#endif

JNIEXPORT void JNICALL
Java_com_github_mugku_androidopenglessampleproject_ndk_GLES3JNILib_init(JNIEnv* env, jobject obj,  jint width, jint height) {
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL
Java_com_github_mugku_androidopenglessampleproject_ndk_GLES3JNILib_step(JNIEnv* env, jobject obj) {
    renderFrame();
}

