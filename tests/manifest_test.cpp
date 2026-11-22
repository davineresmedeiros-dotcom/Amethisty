#include "manifest.hpp"

#include <iostream>
#include <string>

namespace {
const std::string valid = R"({
  "format": 1,
  "id": "author.examplemod",
  "name": "Example Mod",
  "version": "1.0.0",
  "author": "Author",
  "description": "A short description.",
  "game": "2.2.145",
  "library": "mod/libExampleMod.so"
})";

bool expect(bool condition, const char* label) {
    if (!condition) std::cerr << "FAILED: " << label << '\n';
    return condition;
}
}

int main() {
    int failures = 0;
    amethisty::ModManifest manifest;
    std::string error;

    failures += !expect(amethisty::parseManifest(valid, manifest, error),
                        "valid format-1 manifest");
    failures += !expect(manifest.id == "author.examplemod" &&
                        manifest.library == "mod/libExampleMod.so",
                        "manifest fields decoded");

    std::string badGame = valid;
    badGame.replace(badGame.find("2.2.145"), 7, "2.1.999");
    failures += !expect(!amethisty::parseManifest(badGame, manifest, error),
                        "reject unsupported game version");

    std::string traversal = valid;
    traversal.replace(traversal.find("mod/libExampleMod.so"),
                     std::string("mod/libExampleMod.so").size(),
                     "mod/../evil.so");
    failures += !expect(!amethisty::parseManifest(traversal, manifest, error),
                        "reject parent-directory library path");

    std::string duplicate = valid;
    duplicate.insert(duplicate.find("\n  \"name\""), "\n  \"name\": \"Second Name\",");
    failures += !expect(!amethisty::parseManifest(duplicate, manifest, error),
                        "reject duplicate keys");

    failures += !expect(!amethisty::parseManifest("{\"format\": 2}", manifest, error),
                        "reject unsupported manifest format");

    if (failures != 0) return 1;
    std::cout << "All Amethisty manifest tests passed.\n";
    return 0;
}
