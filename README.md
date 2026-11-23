# Amethisty

Amethisty is an independent Android mod-loader project for Geometry Dash 2.2.145 (arm64).

## Current status — prototype

An independent Android application project now exists under `android/`. Its debug APK can be built with the manual workflow **Build independent Amethisty APK**.

The app currently:
- has its own package ID: `com.amethisty.launcher`;
- detects the installed Geometry Dash package;
- reads and displays the installed game's real `versionName` and `longVersionCode` from Android PackageManager;
- opens the installed Geometry Dash through its launch intent.

**Important:** this is an independent launcher prototype, not a completed mod loader yet. It does not inject `libamethisty.so` into Geometry Dash or load mods at runtime. The native loader core remains a separate prototype. The previous Geode-derived APK workflow and adapter were removed.

## Native core

Implemented:
- JNI entry and Android runtime inspection.
- Format-1 mod manifest validation.
- Bounded ZIP reader and private extraction for `.amethyst` packages.
- Mod discovery, validation, duplicate-ID rejection, and extraction.
- arm64 ELF validation and native module loading through `include/amethisty/mod_api.hpp` API v1.
- Native tests in GitHub Actions.

## Next milestones

1. Build and install the independent launcher prototype.
2. Implement a verified, independent game runtime integration for GD 2.2.145.
3. Connect the native core to that runtime safely and only with verified version/ABI data.
4. Add mod import and management UI; test on-device before claiming the loader works.

Native mods execute inside the game process and are not sandboxed. Load only mods from sources you trust.

Amethisty's own code is MIT licensed; third-party components retain their original licenses and notices.
