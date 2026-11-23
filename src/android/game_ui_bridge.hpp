#pragma once

#include <amethisty/game_ui_api.hpp>

namespace amethisty::android {

bool registerGameUiHost(const AmethistyGameUiHost* host) noexcept;
bool installModsButton() noexcept;

} // namespace amethisty::android
