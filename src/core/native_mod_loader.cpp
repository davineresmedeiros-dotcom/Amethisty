#include "native_mod_loader.hpp"
#include <dlfcn.h>
#include <elf.h>
#include <fstream>
#include <utility>
namespace amethisty {
namespace {
constexpr const char* kInitSymbol = MOD_ENTRY_SYMBOL;
constexpr const char* kShutdownSymbol = "amethisty_mod_shutdown";
}
NativeMod::~NativeMod() { unload(); }
NativeMod::NativeMod(NativeMod&& other) noexcept : handle_(other.handle_), shutdown_(other.shutdown_) {
    other.handle_ = nullptr; other.shutdown_ = nullptr;
}
NativeMod& NativeMod::operator=(NativeMod&& other) noexcept {
    if (this != &other) {
        unload(); handle_ = other.handle_; shutdown_ = other.shutdown_;
        other.handle_ = nullptr; other.shutdown_ = nullptr;
    }
    return *this;
}
bool NativeMod::loaded() const noexcept { return handle_ != nullptr; }
void NativeMod::unload() noexcept {
    if (!handle_) return;
    if (shutdown_) {
        try { shutdown_(); } catch (...) { /* Never let a mod exception escape unload. */ }
    }
    dlclose(handle_);
    handle_ = nullptr; shutdown_ = nullptr;
}
bool isArm64ElfLibrary(const std::string& path, std::string& error) {
    error.clear();
    unsigned char h[EI_NIDENT + 8]{};
    std::ifstream f(path, std::ios::binary);
    if (!f || !f.read(reinterpret_cast<char*>(h), sizeof(h))) {
        error = "Cannot read native library ELF header."; return false;
    }
    if (h[EI_MAG0] != ELFMAG0 || h[EI_MAG1] != ELFMAG1 ||
        h[EI_MAG2] != ELFMAG2 || h[EI_MAG3] != ELFMAG3 ||
        h[EI_CLASS] != ELFCLASS64 || h[EI_DATA] != ELFDATA2LSB ||
        h[EI_VERSION] != EV_CURRENT) {
        error = "Native library is not a little-endian ELF64 file."; return false;
    }
    const unsigned type = unsigned(h[16]) | (unsigned(h[17]) << 8);
    const unsigned machine = unsigned(h[18]) | (unsigned(h[19]) << 8);
    if (type != ET_DYN || machine != EM_AARCH64) {
        error = "Native library must be an arm64-v8a shared ELF object."; return false;
    }
    return true;
}
bool loadNativeMod(const std::string& path, const Host& host,
                   NativeMod& output, std::string& error) {
    output.unload(); error.clear();
    if (host.apiVersion != MOD_API_VERSION) {
        error = "Host API version mismatch."; return false;
    }
    if (!isArm64ElfLibrary(path, error)) return false;
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) { const char* message = dlerror(); error = message ? message : "dlopen failed."; return false; }
    dlerror();
    auto init = reinterpret_cast<ModInitFn>(dlsym(handle, kInitSymbol));
    const char* symbolError = dlerror();
    if (symbolError || !init) {
        error = "Required mod init symbol is missing."; dlclose(handle); return false;
    }
    dlerror();
    auto shutdown = reinterpret_cast<ModShutdownFn>(dlsym(handle, kShutdownSymbol));
    symbolError = dlerror();
    if (symbolError) shutdown = nullptr;
    if (!init(&host)) {
        error = "Mod initialization returned failure.";
        if (shutdown) {
            try { shutdown(); } catch (...) { /* Ignore exceptions during failed init cleanup. */ }
        }
        dlclose(handle); return false;
    }
    output.handle_ = handle; output.shutdown_ = shutdown;
    return true;
}
}
