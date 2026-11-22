# Implementation roadmap

The project separates package handling from game integration so each layer can be reviewed independently.

## 1. Package discovery
- [x] Enumerate regular .amethyst files; reject symlinks and sort deterministically.
- [x] Validate format-1 manifest metadata for Geometry Dash 2.2.145.
- [x] Add a bounded ZIP reader for stored/deflate entries, safe paths, duplicate rejection, CRC verification, and private extraction.
- [ ] Connect package validation/extraction to ModManager and report per-package errors.
- [ ] Reject duplicate mod IDs before extraction and installation.

## 2. Android and game integration
- Confirm the installed game version code and ABI on the target device.
- Confirm JNI entry and Cocos detection in logcat.
- Add game hooks only after target bindings and offsets are verified.

## 3. Native module loading
- Confirm process ABI and package ABI before loading.
- Load only a validated extracted library from app-private storage.
- Resolve a documented entry symbol and verify API version.
- Isolate failures per mod and report them through the host logger.

## 4. Device validation
- Run native tests for valid and malformed package files.
- Test extraction against hostile ZIP cases.
- Validate on a backed-up Geometry Dash 2.2.145 installation with a way to disable all mods.

## Current limitation
Amethisty remains a scaffold. The package reader/extractor exists but is not connected to ModManager. ABI checks, native mod loading, game hooks, and device validation remain unfinished. The launcher adapter is a source integration example, not a complete launcher APK.
