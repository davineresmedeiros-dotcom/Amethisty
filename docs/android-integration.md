# Independent Android and Geometry Dash integration

## Current independent launcher prototype

The Android app in `android/` is an original, standalone Amethisty application with package ID `com.amethisty.launcher`. It queries the installed Geometry Dash package with Android PackageManager, displays the real `versionName` and `longVersionCode`, and starts the game through its launch intent.

It does **not** currently load the game's native libraries, inject Amethisty into the game's process, or load mods. The previous APK workflow that adapted Geode Android Launcher was removed because it did not satisfy the independent-project requirement.

## Target and version verification

Target game: Geometry Dash Android 2.2.145, arm64-v8a.

Do not assume version code 42. The launcher reads the installed package's actual `longVersionCode`; the exact target version code must be recorded from a verified 2.2.145 APK/device. Version-name matching alone is not proof of binary compatibility.

## Native core status

The C++ loader core can validate manifests and packages, extract native payloads, verify AArch64 ELF files, and load native modules when called in the appropriate process. It is not yet connected to the independent Android app or Geometry Dash runtime.

## Remaining work

1. Implement an independent runtime that loads the exact target game's required assets and native libraries, or another legitimate, verified in-process integration.
2. Confirm the exact 2.2.145 APK metadata and arm64 native binaries.
3. Connect the native loader after the game's required runtime initialization.
4. Implement mod import/management UI and end-to-end testing.

Do not add guessed offsets, binary patches, or hooks. Verify the target binary and ABI before implementing runtime integration. Native mods execute inside the game process and are not sandboxed.
