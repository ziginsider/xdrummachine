#include <jni.h>
#include <string>

#include <android/asset_manager_jni.h>
#include "utils/logging.h"

#include "ZDrumMachine.h"

extern "C" {

std::unique_ptr<ZDrumMachine> machine;

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1onInit(JNIEnv *env, jobject thiz,
                                                                   jobject asset_manager,
                                                                   jint bpm) {
    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    if (assetManager == nullptr) {
        LOGE("Could not obtain the AAssetManager");
        return;
    }

    // Init
    if (machine == nullptr) {
        machine = std::make_unique<ZDrumMachine>(*assetManager, bpm);
    } else {
        LOGD("ZDrumMachine instance has been already created");
    }
}

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1onStart(JNIEnv *env, jobject thiz, jint bpm) {

    // Start
    if (machine == nullptr) {
        LOGE("Could not obtain the ZDrumMachine during native_onStart()");
        return;
    }
    machine->start(bpm);
}

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1onPause(JNIEnv *env, jobject thiz) {
    if (machine == nullptr) {
        LOGE("Could not obtain the ZDrumMachine during native_onPause()");
        return;
    }
    machine->pause();
}

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1onStop(JNIEnv *env, jobject thiz) {
    if (machine == nullptr) {
        LOGE("Could not obtain the ZDrumMachine during native_onStop()");
        return;
    }
    machine->stop();
}

JNIEXPORT void JNICALL
Java_io_github_ziginsider_zdrummachine_MainActivity_native_1setPattern(JNIEnv *env, jobject thiz,
                                                                       jint id, jintArray pattern) {
    if (machine == nullptr) {
        LOGE("Could not obtain the ZDrumMachine during native_setPattern(...)");
        return;
    }

    std::vector<int> _pattern;
    auto size = env->GetArrayLength(pattern);
    auto _pattern_c_array = env->GetIntArrayElements(pattern, nullptr);
    _pattern.reserve(size);
    for (int i = 0; i < size; ++i) {
        _pattern.push_back(_pattern_c_array[i]);
    }

    env->ReleaseIntArrayElements(pattern, _pattern_c_array, 0);

    machine->setPatternForSound(id, _pattern);
}

} // extern "C"
