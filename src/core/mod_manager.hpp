#pragma once

#include "native_mod_loader.hpp"
#include <string>
#include <vector>

namespace amethisty {

struct ModCandidate {
    // Absolute or app-private path to the package file.
    std::string packagePath;
    // Temporary filename-derived ID until manifest.json parsing is added.
    std::string id;
    std::string name;
    std::string version;
    // Empty until the package archive is validated and extracted safely.
    std::string libraryPath;
    bool valid = false;
    bool loaded = false;
    NativeMod nativeModule;
    std::string error;
};

class ModManager {
public:
    // Enumerates .amethyst files only. It does not parse archives or load code.
    // Returns false when the directory cannot be opened.
    bool scan(const std::string& modsDirectory);

    // Validates package manifests, rejects duplicate IDs, then extracts libraries.
    // This still does not load or execute native modules.
    bool scanAndExtract(const std::string& modsDirectory, const std::string& privateDirectory);

    // Loads each extracted library after ELF and API checks. Mods are native code, not sandboxed.
    bool loadNativeMods(const ModHostApi& host);

    const std::vector<ModCandidate>& candidates() const noexcept;

private:
    std::vector<ModCandidate> m_candidates;
};

} // namespace amethisty
