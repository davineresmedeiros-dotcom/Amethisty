# Native mod API v1

Amethisty's native module loader targets Android arm64-v8a and Geometry Dash 2.2.145.

A mod library must be an ELF64 little-endian AArch64 shared object and export:
- \`extern "C" uint32_t amethisty_mod_api_version()\` returning 1.
- \`extern "C" bool amethisty_mod_init(const amethisty::ModHostApi*)\` returning true when initialization succeeds.
- Optional \`extern "C" void amethisty_mod_shutdown()\`, called before unload.

The host structure contains its byte size, API version, and a logging callback. The host validates ELF architecture and API version before calling init. A failed initialization causes the library to be closed.

**Security note:** native mods execute in the host process and are not sandboxed. Only load mods from sources you trust. This API is a compatibility contract, not a security boundary.

The ModManager exposes `loadNativeMods(host)`: it attempts each valid extracted candidate independently, records an error per failed mod, and keeps successful modules owned by the candidate until a rescan/destruction unloads them. The host must call this only after the game runtime is ready and only for mods the user trusts.
