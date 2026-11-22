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
        "Android integration initialized; ABI=%s; cocos2dcpp=%s; libgame=%s",
        status.abi,
        status.cocosLoaded ? "loaded" : "not loaded",
        status.gameLoaded ? "loaded" : "not loaded"
    );

    if (!status.cocosLoaded || !status.gameLoaded) {
        logInfo("Game libraries are not both visible yet; no hooks were installed.");
    } else {
        logInfo("Game libraries detected. Hook installation remains disabled pending verified 2.2091 bindings.");
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
