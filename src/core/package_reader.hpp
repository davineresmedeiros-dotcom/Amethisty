#pragma once
#include "manifest.hpp"
#include <string>
namespace amethisty {
struct ExtractedPackage { ModManifest manifest; std::string libraryPath; };
bool readPackageManifest(const std::string& packagePath, ModManifest& manifest, std::string& error);
bool validateAndExtractPackage(const std::string& packagePath, const std::string& privateRoot, ExtractedPackage& output, std::string& error);
}