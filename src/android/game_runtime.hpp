#pragma once

namespace amethisty::android {

struct GameRuntimeStatus {
    bool cocosLoaded;
    bool gameLoaded;
    const char* abi;
};

// Inspects already-loaded game libraries only. It does not load game binaries,
// patch code, install hooks, or initialize Cocos.
GameRuntimeStatus inspectLoadedGame() noexcept;

} // namespace amethisty::android
