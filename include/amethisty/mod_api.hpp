#pragma once

#include <cstdint>

#if defined(__GNUC__)
#define AMETHISTY_EXPORT __attribute__((visibility("default")))
#else
#define AMETHISTY_EXPORT
#endif

namespace amethisty {

inline constexpr std::uint32_t MOD_API_VERSION = 1;
inline constexpr const char* MOD_ENTRY_SYMBOL = "amethisty_mod_init";

struct ModInfo {
    std::uint32_t apiVersion;
    const char* id;
    const char* name;
    const char* version;
};

struct Host {
    std::uint32_t apiVersion;
    void (*log)(int level, const char* message);
};

using ModInit = bool (*)(const Host* host);

} // namespace amethisty

// A mod should export this function from its native library:
// extern "C" AMETHISTY_EXPORT bool amethisty_mod_init(
//     const amethisty::Host* host);
