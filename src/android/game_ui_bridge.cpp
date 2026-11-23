#include "game_ui_bridge.hpp"

#include <amethisty/game_ui_api.hpp>

namespace {
AmethistyGameUiHost g_host{};
bool g_hostRegistered = false;

void onModsButton(void*) {
    if (!g_hostRegistered || !g_host.openModsPanel) return;
    const auto count = amethisty_get_mod_count();
    g_host.openModsPanel(g_host.userData, count,
        [](std::size_t index, const char** id, const char** name,
           const char** version, bool* loaded, void*) -> bool {
            return amethisty_get_mod_info(index, id, name, version, loaded);
        }, nullptr);
}
}

namespace amethisty::android {

bool registerGameUiHost(const AmethistyGameUiHost* host) noexcept {
    if (!host || host->apiVersion != AMETHISTY_GAME_UI_API_VERSION ||
        !host->addButton || !host->openModsPanel) return false;
    g_host = *host;
    g_hostRegistered = true;
    return true;
}

bool installModsButton() noexcept {
    if (!g_hostRegistered) return false;
    return g_host.addButton(g_host.userData, "Mods", &onModsButton, nullptr);
}

} // namespace amethisty::android

extern "C" AMETHISTY_GAME_UI_EXPORT bool amethisty_register_game_ui_host(
    const AmethistyGameUiHost* host) {
    return amethisty::android::registerGameUiHost(host);
}

extern "C" AMETHISTY_GAME_UI_EXPORT bool amethisty_install_mods_button() {
    return amethisty::android::installModsButton();
}
