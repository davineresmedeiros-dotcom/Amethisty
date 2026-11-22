#include "game_runtime.hpp"

#include <dlfcn.h>

namespace amethisty::android {
namespace {

const char* currentAbi() noexcept {
#if defined(__aarch64__)
    return "arm64-v8a";
#elif defined(__arm__)
    return "armeabi-v7a";
#elif defined(__x86_64__)
    return "x86_64";
#elif defined(__i386__)
    return "x86";
#else
    return "unknown";
#endif
}

bool isAlreadyLoaded(const char* libraryName) noexcept {
#ifdef RTLD_NOLOAD
    void* handle = dlopen(libraryName, RTLD_NOW | RTLD_NOLOAD);
    if (!handle) return false;
    dlclose(handle);
    return true;
#else
    (void)libraryName;
    return false;
#endif
}

} // namespace

GameRuntimeStatus inspectLoadedGame() noexcept {
    GameRuntimeStatus status{};
    status.abi = currentAbi();
    // Geometry Dash's native game code is hosted in libcocos2dcpp.so.
    status.cocosLoaded = isAlreadyLoaded("libcocos2dcpp.so");
    return status;
}

} // namespace amethisty::android
