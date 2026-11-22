# Native mod API v1

Amethisty's native module loader targets Android arm64-v8a and Geometry Dash 2.2.145. It uses the public header \`include/amethisty/mod_api.hpp\`.

A mod library must be an ELF64 little-endian AArch64 shared object and export:
- \`extern "C" AMETHISTY_EXPORT bool amethisty_mod_init(const amethisty::Host*)\`, returning true when initialization succeeds.
- Optional \`extern "C" void amethisty_mod_shutdown()\`, called before unload.

The host passes \`amethisty::Host\`, which contains API version 1 and a logging callback. The mod checks \`host->apiVersion\` before using the interface. A missing init symbol, wrong ELF architecture, incompatible host version, or failed init causes the library to be closed.

The ModManager exposes \`loadNativeMods(host)\`: it attempts each valid extracted candidate independently, records an error per failed mod, and keeps successful modules owned by the candidate until a rescan/destruction unloads them. The host must call this only after the game runtime is ready and only for mods the user trusts.

**Security note:** native mods execute inside the host process and are not sandboxed. Only load mods from sources you trust. This API is a compatibility contract, not a security boundary.
