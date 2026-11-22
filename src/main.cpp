#if defined(__ANDROID__)
#include "android/game_runtime.hpp"

#include <android/log.h>
#include <jni.h>

namespace {
constexpr const char* kLogTag = "Amethisty";

void logInfo(const char* message) {
    __android_log_write(ANDROID_LOG_INFO, kLogTag, message);
}

void logRuntimeStatus() {
    const auto status = amethisty::android::inspectLoadedGame();
    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Android integration reached; ABI=%s; libcocos2dcpp=%s",
        status.abi,
        status.cocosLoaded ? "loaded" : "not loaded"
    );

    if (!status.cocosLoaded) {
        logInfo("Cocos library was not detected. No hooks were installed.");
    } else {
        logInfo("Cocos library detected. Game hooks remain disabled until 2.2.145 bindings are verified.");
    }
}
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK || !env) {
        logInfo("JNI_OnLoad could not obtain JNIEnv.");
        return JNI_ERR;
    }

    logInfo("Amethisty JNI entry reached.");
    logRuntimeStatus();
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM*, void*) {
    logInfo("Amethisty JNI library unloading.");
}
#else
// This translation unit can be compiled on a desktop for basic source checks.
extern "C" int amethisty_source_check() {
    return 1;
}
#endif
