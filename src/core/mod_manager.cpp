#include "mod_manager.hpp"

#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <utility>

namespace amethisty {
namespace {

bool hasPackageExtension(const char* filename) {
    if (!filename) return false;
    const std::string name(filename);
    constexpr char extension[] = ".amethyst";
    constexpr std::size_t extensionLength = sizeof(extension) - 1;
    if (name.size() <= extensionLength) return false;

    const std::size_t offset = name.size() - extensionLength;
    for (std::size_t i = 0; i < extensionLength; ++i) {
        const unsigned char actual = static_cast<unsigned char>(name[offset + i]);
        if (std::tolower(actual) != extension[i]) return false;
    }
    return true;
}

std::string packageIdFromFilename(const char* filename) {
    std::string id(filename ? filename : "");
    const auto dot = id.rfind('.');
    if (dot != std::string::npos) id.resize(dot);
    return id;
}

std::string joinPath(const std::string& directory, const char* filename) {
    std::string path(directory);
    if (!path.empty() && path.back() != '/') path.push_back('/');
    path += filename ? filename : "";
    return path;
}

} // namespace

bool ModManager::scan(const std::string& modsDirectory) {
    m_candidates.clear();

    DIR* directory = opendir(modsDirectory.c_str());
    if (!directory) return false;

    while (dirent* entry = readdir(directory)) {
        if (entry->d_name[0] == '.') continue;
        if (!hasPackageExtension(entry->d_name)) continue;

        const std::string packagePath = joinPath(modsDirectory, entry->d_name);
        struct stat status {};
        // stat follows symlinks; lstat lets us reject them and keep scanning
        // limited to regular package files in the selected mods directory.
        if (lstat(packagePath.c_str(), &status) != 0 || !S_ISREG(status.st_mode)) {
            continue;
        }

        ModCandidate candidate;
        candidate.packagePath = packagePath;

        // Until archive and manifest support lands, these fields are only
        // placeholders. This scan result must never be passed to dlopen().
        candidate.id = packageIdFromFilename(entry->d_name);
        candidate.name = candidate.id;
        candidate.version = "unknown";
        candidate.libraryPath.clear();

        m_candidates.push_back(std::move(candidate));
    }

    closedir(directory);
    std::sort(m_candidates.begin(), m_candidates.end(),
              [](const ModCandidate& left, const ModCandidate& right) {
                  return left.packagePath < right.packagePath;
              });
    return true;
}

const std::vector<ModCandidate>& ModManager::candidates() const noexcept {
    return m_candidates;
}

} // namespace amethisty
