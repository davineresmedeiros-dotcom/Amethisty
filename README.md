# Amethisty

Amethisty is an independent Android native mod-loader project for Geometry Dash, targeting the user's installed GD 2.2.145 (arm64).

## Current status

The native loader core is a prototype. **There is not yet a working independent Amethisty APK.**

Implemented in the native core:
- JNI entry and Android runtime inspection.
- Format-1 mod manifest validation.
- Bounded ZIP reader and private extraction for `.amethyst` packages.
- Mod discovery, validation, duplicate-ID rejection, and extraction.
- arm64 ELF validation and native module loading through `include/amethisty/mod_api.hpp` API v1.
- Native tests in GitHub Actions.

The previous APK workflow and Geode Android Launcher adaptation were removed because they produced a Geode-derived launcher, not the independent app requested. Do not use the APK artifact from that workflow as an Amethisty release.

## Remaining work

1. Create Amethisty's own Android launcher/application and package identity.
2. Detect the installed Geometry Dash package and read its actual `versionName` and `longVersionCode` from Android PackageManager; verify the target against a real 2.2.145 install rather than hardcoding an unverified number.
3. Implement the game startup/runtime integration independently.
4. Add mod import and management UI, then build and test an APK.

Native mods execute inside the game process and are not sandboxed. Load only mods from sources you trust.

Amethisty's own code is MIT licensed; third-party components retain their original licenses and notices.
