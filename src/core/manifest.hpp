#pragma once

#include <string>

namespace amethisty {

struct ModManifest {
    int format = 0;
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string game;
    std::string library;
};

// Parses and validates a format-1 manifest. The JSON input is bounded to 64 KiB.
// Returns false and fills error on malformed or incompatible metadata.
bool parseManifest(const std::string& json, ModManifest& manifest,
                   std::string& error);

} // namespace amethisty
