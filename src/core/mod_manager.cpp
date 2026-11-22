#include "mod_manager.hpp"
#include "package_reader.hpp"
#include "native_mod_loader.hpp"

#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <unordered_set>

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


bool ModManager::scanAndExtract(const std::string& modsDirectory,
                                const std::string& privateDirectory) {
    if (!scan(modsDirectory)) return false;

    // Parse every manifest first, so duplicate IDs are rejected before any
    // package can overwrite another package's extracted library.
    std::unordered_set<std::string> ids;
    for (ModCandidate& candidate : m_candidates) {
        ModManifest manifest;
        std::string error;
        if (!readPackageManifest(candidate.packagePath, manifest, error)) {
            candidate.error = error;
            continue;
        }
        if (!ids.insert(manifest.id).second) {
            candidate.error = "Duplicate mod ID: " + manifest.id;
            continue;
        }
        candidate.id = manifest.id;
        candidate.name = manifest.name;
        candidate.version = manifest.version;
        candidate.valid = true;
    }

    for (ModCandidate& candidate : m_candidates) {
        if (!candidate.valid) continue;
        ExtractedPackage package;
        std::string error;
        if (!validateAndExtractPackage(candidate.packagePath, privateDirectory,
                                      package, error)) {
            candidate.valid = false;
            candidate.error = error;
            continue;
        }
        candidate.libraryPath = package.libraryPath;
    }
    return true;
}


bool ModManager::loadNativeMods(const ModHostApi& host) {
    bool allLoaded = true;
    for (ModCandidate& candidate : m_candidates) {
        if (!candidate.valid || candidate.libraryPath.empty()) continue;
        std::string error;
        if (!loadNativeMod(candidate.libraryPath, host, candidate.nativeModule, error)) {
            candidate.loaded = false;
            candidate.error = error;
            allLoaded = false;
            continue;
        }
        candidate.loaded = true;
        candidate.error.clear();
    }
    return allLoaded;
}

const std::vector<ModCandidate>& ModManager::candidates() const noexcept {
    return m_candidates;
}

} // namespace amethisty
