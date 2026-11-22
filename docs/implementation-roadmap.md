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
- Confirm JNI entry and Cocos detection in logcat.
- Add game hooks only after target bindings and offsets are verified.

## 3. Native module loading
- [ ] Confirm process ABI and package ABI before loading.
- [ ] Define and document the native mod entry symbol and API version.
- [ ] Load only validated extracted libraries from app-private storage.
- [ ] Isolate failures per mod and report them through the host logger.

## 4. Device validation
- [x] Native tests for valid package, traversal rejection, missing library, and ModManager integration.
- [ ] Test additional malformed ZIP cases and duplicate package IDs.
- [ ] Validate on a backed-up Geometry Dash 2.2.145 installation with a way to disable all mods.

## Current limitation
Amethisty can discover, validate, and extract package libraries, but it does not load or execute them. ABI checks, native mod loading, game hooks, and device validation remain unfinished. The launcher adapter is a source integration example, not a complete launcher APK.
