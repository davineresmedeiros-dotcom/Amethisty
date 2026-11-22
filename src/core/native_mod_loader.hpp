#pragma once
#include <amethisty/mod_api.hpp>
#include <string>
namespace amethisty {
using ModShutdownFn = void (*)();
class NativeMod {
public:
    NativeMod() noexcept = default;
    ~NativeMod();
    NativeMod(const NativeMod&) = delete;
    NativeMod& operator=(const NativeMod&) = delete;
    NativeMod(NativeMod&& other) noexcept;
    NativeMod& operator=(NativeMod&& other) noexcept;
    bool loaded() const noexcept;
    void unload() noexcept;
private:
    friend bool loadNativeMod(const std::string&, const Host&, NativeMod&, std::string&);
    void* handle_ = nullptr;
    ModShutdownFn shutdown_ = nullptr;
};
bool isArm64ElfLibrary(const std::string& path, std::string& error);
bool loadNativeMod(const std::string& path, const Host& host, NativeMod& output, std::string& error);
}
