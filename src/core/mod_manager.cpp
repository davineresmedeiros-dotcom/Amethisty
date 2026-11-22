#include "mod_manager.hpp"

namespace amethisty {

bool ModManager::scan(const std::string& modsDirectory) {
    // Package scanning, ZIP validation, manifest parsing and safe extraction
    // are intentionally not enabled until the ZIP/JSON dependencies and
    // Android filesystem paths are integrated.
    (void)modsDirectory;
    m_candidates.clear();
    return false;
}

const std::vector<ModCandidate>& ModManager::candidates() const noexcept {
    return m_candidates;
}

} // namespace amethisty
