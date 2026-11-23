#if defined(__ANDROID__)
#include "android/game_runtime.hpp"
#include "android/game_ui_bridge.hpp"
#include <amethisty/game_ui_api.hpp>
#include "core/mod_manager.hpp"
#include <amethisty/mod_api.hpp>
#include <android/log.h>
#include <jni.h>
namespace {
constexpr const char* kLogTag = "Amethisty";
amethisty::ModManager g_modManager;
void logInfo(const char* message) { __android_log_write(ANDROID_LOG_INFO, kLogTag, message); }
void hostLog(int level, const char* message) {
    const int priority = level >= 3 ? ANDROID_LOG_ERROR : level == 2 ? ANDROID_LOG_WARN : ANDROID_LOG_INFO;
    __android_log_write(priority, kLogTag, message ? message : "(null)");
}
void logRuntimeStatus() {
    const auto status = amethisty::android::inspectLoadedGame();
    __android_log_print(ANDROID_LOG_INFO, kLogTag,
        "Android integration reached; ABI=%s; libcocos2dcpp=%s",
        status.abi, status.cocosLoaded ? "loaded" : "not loaded");
    if (!status.cocosLoaded) logInfo("Cocos was not detected; no mods initialized.");
    else logInfo("Cocos detected; game hooks remain disabled until 2.2.145 bindings are verified.");
}
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK || !env) {
        logInfo("JNI_OnLoad could not obtain JNIEnv."); return JNI_ERR;
    }
    logInfo("Amethisty JNI entry reached."); logRuntimeStatus();
    return JNI_VERSION_1_6;
}
extern "C" JNIEXPORT jboolean JNICALL
Java_com_amethisty_launcher_NativeBridge_initializeNative(
    JNIEnv* env, jclass, jstring modsDirectory, jstring privateDirectory) {
    if (!modsDirectory || !privateDirectory) { logInfo("Mod directory arguments are null."); return JNI_FALSE; }
    const auto status = amethisty::android::inspectLoadedGame();
    if (!status.cocosLoaded) { logInfo("Cocos is not loaded; refusing mod initialization."); return JNI_FALSE; }
    const char* mods = env->GetStringUTFChars(modsDirectory, nullptr);
    const char* priv = env->GetStringUTFChars(privateDirectory, nullptr);
    if (!mods || !priv) {
        if (mods) env->ReleaseStringUTFChars(modsDirectory, mods);
        if (priv) env->ReleaseStringUTFChars(privateDirectory, priv);
        logInfo("Could not read mod directory strings."); return JNI_FALSE;
    }
    const bool scanned = g_modManager.scanAndExtract(mods, priv);
    env->ReleaseStringUTFChars(modsDirectory, mods);
    env->ReleaseStringUTFChars(privateDirectory, priv);
    if (!scanned) { logInfo("Could not scan mod directory; game continues."); return JNI_FALSE; }
    const amethisty::Host host{amethisty::MOD_API_VERSION, &hostLog};
    const bool allLoaded = g_modManager.loadNativeMods(host);
    for (const auto& candidate : g_modManager.candidates()) {
        if (!candidate.error.empty()) __android_log_print(ANDROID_LOG_ERROR, kLogTag,
            "Mod %s failed: %s", candidate.id.c_str(), candidate.error.c_str());
        else if (candidate.loaded) __android_log_print(ANDROID_LOG_INFO, kLogTag,
            "Loaded mod %s (%s)", candidate.id.c_str(), candidate.version.c_str());
    }
    if (!allLoaded) logInfo("Some mods failed; continuing with successfully loaded mods.");
    return JNI_TRUE;
}
extern "C" AMETHISTY_GAME_UI_EXPORT std::size_t amethisty_get_mod_count() {
    return g_modManager.candidates().size();
}
extern "C" AMETHISTY_GAME_UI_EXPORT bool amethisty_get_mod_info(
    std::size_t index, const char** id, const char** name,
    const char** version, bool* loaded) {
    const auto& candidates = g_modManager.candidates();
    if (index >= candidates.size() || !id || !name || !version || !loaded) return false;
    const auto& mod = candidates[index];
    *id = mod.id.c_str();
    *name = mod.name.c_str();
    *version = mod.version.c_str();
    *loaded = mod.loaded;
    return true;
}
extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM*, void*) { logInfo("Amethisty JNI library unloading."); }
#else
extern "C" int amethisty_source_check() { return 1; }
#endif
