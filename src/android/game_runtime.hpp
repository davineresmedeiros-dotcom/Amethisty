#pragma once

namespace amethisty::android {

struct GameRuntimeStatus {
    bool cocosLoaded;
    const char* abi;
};

// Inspects the already-loaded Geometry Dash/Cocos library only.
// It does not load game binaries, patch code, install hooks, or initialize Cocos.
GameRuntimeStatus inspectLoadedGame() noexcept;

} // namespace amethisty::android
