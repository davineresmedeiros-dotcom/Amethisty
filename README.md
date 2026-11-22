# Amethisty

Amethisty is an Android native mod-loader project targeting Geometry Dash 2.2.145. Verify the exact installed APK version code before enforcing compatibility.

## Current status
Native core prototype and launcher source integration in progress. Not yet a standalone APK.

Implemented:
- JNI entry and Android ABI/runtime inspection.
- Format-1 manifest validation for GD 2.2.145.
- Bounded ZIP package reader and private extraction for .amethyst.
- ModManager package discovery, validation, duplicate-ID rejection, and extraction.
- Arm64 ELF validation and native module loading through the public include/amethisty/mod_api.hpp API v1.
- Per-mod load errors and native tests in GitHub Actions.
- Launcher adapter source that initializes package loading after game libraries are ready.

See Android integration, package format, and native mod API documentation.

## Next milestones
1. Verify integrated runtime on a backed-up GD 2.2.145 installation.
2. Finish launcher integration and compatibility checks.
3. Build the Amethisty APK with package import and mod management UI.

Native mods execute inside the game process and are not sandboxed. Load only mods from sources you trust.

Amethisty's own code is MIT licensed; third-party components retain their original licenses and notices.
