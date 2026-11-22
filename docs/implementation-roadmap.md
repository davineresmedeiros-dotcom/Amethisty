# Implementation roadmap

The project separates package handling from game integration so each layer can be reviewed independently.

## 1. Package discovery
- [x] Enumerate regular .amethyst files; reject symlinks and sort deterministically.
- [x] Validate format-1 manifest metadata for Geometry Dash 2.2.145.
- [x] Read bounded ZIP packages, reject unsafe/duplicate entries and symlinks, verify CRC, and extract the declared library privately.
- [x] Connect package manifest validation and extraction to ModManager.
- [x] Reject duplicate mod IDs before extraction and report per-package errors.

## 2. Android and game integration
- Confirm installed game version code and ABI on the target device.
- [x] Add JNI entry and Cocos runtime detection.
- [x] Add launcher JNI initialization to scan, extract, and attempt loading mods.
- [ ] Verify JNI startup and mod loading on the target device.
- Add game hooks only after target bindings and offsets are verified.

## 3. Native module loading
- [x] Confirm library ELF class, endianness, and AArch64 machine type before loading.
- [x] Define and document native mod entry symbols and API version 1.
- [x] Add an opt-in ModManager load stage for validated extracted libraries.
- [x] Isolate load failures per mod and retain per-candidate error messages.

## 4. Device validation
- [x] Native tests for valid package, traversal rejection, missing library, and ModManager integration.
- [ ] Test additional malformed ZIP cases and duplicate package IDs.
- [ ] Validate on a backed-up Geometry Dash 2.2.145 installation with a way to disable all mods.

## Current limitation
Amethisty can discover, validate, and extract package libraries, but it does not load or execute them. ABI checks, native mod loading, game hooks, and device validation remain unfinished. The launcher adapter is a source integration example, not a complete launcher APK.
