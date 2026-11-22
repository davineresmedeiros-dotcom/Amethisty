#include "mod_manager.hpp"

#include <dirent.h>
#include <string>

namespace amethisty {
namespace {

bool hasPackageExtension(const char* filename) {
    if (!filename) return false;
    const std::string name(filename);
    constexpr char extension[] = ".amethyst";
    constexpr std::size_t extensionLength = sizeof(extension) - 1;
    return name.size() > extensionLength &&
           name.compare(name.size() - extensionLength, extensionLength, extension) == 0;
}

std::string packageIdFromFilename(const char* filename) {
    std::string id(filename ? filename : "");
    const auto dot = id.rfind('.');
    if (dot != std::string::npos) id.resize(dot);
    return id;
}

} // namespace

bool ModManager::scan(const std::string& modsDirectory) {
    m_candidates.clear();

    DIR* directory = opendir(modsDirectory.c_str());
    if (!directory) return false;

    while (dirent* entry = readdir(directory)) {
        if (entry->d_type != DT_REG && entry->d_type != DT_UNKNOWN) continue;
        if (!hasPackageExtension(entry->d_name)) continue;

        ModCandidate candidate;
        candidate.packagePath = modsDirectory;
        if (!candidate.packagePath.empty() && candidate.packagePath.back() != '/') {
            candidate.packagePath.push_back('/');
        }
        candidate.packagePath += entry->d_name;

        // Metadata is intentionally provisional until ZIP and manifest
        // parsing are integrated. Never load a library from this scan result.
        candidate.id = packageIdFromFilename(entry->d_name);
        candidate.name = candidate.id;
        candidate.version = "unknown";
        candidate.libraryPath.clear();

        m_candidates.push_back(std::move(candidate));
    }

    closedir(directory);
    return true;
}

const std::vector<ModCandidate>& ModManager::candidates() const noexcept {
    return m_candidates;
}

} // namespace amethisty
