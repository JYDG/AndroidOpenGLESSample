#include <jni.h>
#include <string>
extern "C"
jstring
Java_com_github_mugku_androidopenglessampleproject_ndk_GLES3JNILib_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
