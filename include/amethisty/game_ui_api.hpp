#pragma once

#include <cstddef>
#include <cstdint>

#if defined(__GNUC__)
#define AMETHISTY_GAME_UI_EXPORT __attribute__((visibility("default")))
#else
#define AMETHISTY_GAME_UI_EXPORT
#endif

extern "C" {

inline constexpr std::uint32_t AMETHISTY_GAME_UI_API_VERSION = 1;

using AmethistyGameUiAction = void (*)(void* userData);
using AmethistyModInfoQuery = bool (*)(std::size_t index, const char** id,
                                      const char** name, const char** version,
                                      bool* loaded, void* userData);

// Implemented by the in-game Cocos integration, which must call these functions
// on the Geometry Dash main/UI thread.
struct AmethistyGameUiHost {
    std::uint32_t apiVersion;
    void* userData;
    bool (*addButton)(void* userData, const char* label,
                      AmethistyGameUiAction action, void* actionUserData);
    void (*openModsPanel)(void* userData, std::size_t count,
                          AmethistyModInfoQuery query, void* queryUserData);
};

AMETHISTY_GAME_UI_EXPORT bool amethisty_register_game_ui_host(
    const AmethistyGameUiHost* host);
AMETHISTY_GAME_UI_EXPORT bool amethisty_install_mods_button();
AMETHISTY_GAME_UI_EXPORT std::size_t amethisty_get_mod_count();
AMETHISTY_GAME_UI_EXPORT bool amethisty_get_mod_info(
    std::size_t index, const char** id, const char** name,
    const char** version, bool* loaded);

}
