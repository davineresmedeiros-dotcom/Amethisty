#pragma once

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
};

class ModManager {
public:
    // Enumerates .amethyst files only. It does not parse archives or load code.
    // Returns false when the directory cannot be opened.
    bool scan(const std::string& modsDirectory);

    const std::vector<ModCandidate>& candidates() const noexcept;

private:
    std::vector<ModCandidate> m_candidates;
};

} // namespace amethisty
