#include <jni.h>
#include <string>

#include <android/asset_manager_jni.h>
#include "utils/logging.h"

#include "ZDrumMachine.h"

extern "C" {

std::unique_ptr<ZDrumMachine> machine;

JNIEXPORT jstring JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ !!!";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1onStart(JNIEnv *env, jobject thiz,
                                                                    jobject asset_manager) {

    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    if (assetManager == nullptr) {
        LOGE("Could not obtain the AAssetManager");
        return;
    }

    machine = std::make_unique<ZDrumMachine>(*assetManager);
    //machine->start();
}
} // extern "C"