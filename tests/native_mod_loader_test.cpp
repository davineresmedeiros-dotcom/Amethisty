#include "native_mod_loader.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
int main() {
    char temp[]="/tmp/amethisty-elf-XXXXXX";
    int fd=mkstemp(temp); if(fd<0) return 1; close(fd);
    { std::ofstream f(temp,std::ios::binary); f<<"not an ELF library"; }
    std::string error;
    if(amethisty::isArm64ElfLibrary(temp,error)) {
        std::cerr<<"FAILED: invalid ELF accepted\n"; unlink(temp); return 1;
    }
    if(error.empty()) { std::cerr<<"FAILED: missing ELF error\n"; unlink(temp); return 1; }
    unlink(temp);
    amethisty::ModHostApi host{sizeof(amethisty::ModHostApi), amethisty::kModApiVersion, nullptr};
    amethisty::NativeMod mod;
    if(amethisty::loadNativeMod(temp,host,mod,error) || mod.loaded()) {
        std::cerr<<"FAILED: invalid native module loaded\\n"; unlink(temp); return 1;
    }
    amethisty::ModHostApi wrong{sizeof(amethisty::ModHostApi), 99, nullptr};
    if(amethisty::loadNativeMod(temp,wrong,mod,error) || error.empty()) {
        std::cerr<<"FAILED: incompatible host API accepted\\n"; unlink(temp); return 1;
    }
    std::cout<<"Amethisty native loader ELF checks passed.\n";
    return 0;
}