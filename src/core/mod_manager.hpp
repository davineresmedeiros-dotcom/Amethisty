#pragma once

#include <string>
#include <vector>

namespace amethisty {

struct ModCandidate {
    std::string packagePath;
    std::string id;
    std::string name;
    std::string version;
    std::string libraryPath;
};

class ModManager {
public:
    // The implementation will scan installed .amethyst packages and validate
    // their manifests before any native module is loaded.
    bool scan(const std::string& modsDirectory);

    const std::vector<ModCandidate>& candidates() const noexcept;

private:
    std::vector<ModCandidate> m_candidates;
};

} // namespace amethisty
