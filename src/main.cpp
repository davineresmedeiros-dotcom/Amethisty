#if defined(__ANDROID__)
#include <android/log.h>
#include <jni.h>

namespace {
constexpr const char* kLogTag = "Amethisty";

void logInfo(const char* message) {
    __android_log_write(ANDROID_LOG_INFO, kLogTag, message);
}
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    logInfo("Native library loaded. Game hooks and mod loading are not wired yet.");
    return JNI_VERSION_1_6;
}
#else
// This translation unit can be compiled on a desktop for basic source checks.
extern "C" int amethisty_source_check() {
    return 1;
}
#endif
