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
    std::cout<<"Amethisty native loader ELF checks passed.\n";
    return 0;
}