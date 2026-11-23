# Independent Android and Geometry Dash integration

## Current independent launcher prototype

The Android app in `android/` is an original, standalone Amethisty application with package ID `com.amethisty.launcher`. It queries the installed Geometry Dash package with Android PackageManager, displays the real `versionName` and `longVersionCode`, and starts the game through its launch intent.

It does **not** currently load the game's native libraries, inject Amethisty into the game's process, or load mods. The previous APK workflow that adapted Geode Android Launcher was removed because it did not satisfy the independent-project requirement.

## Target and version verification

Target game: Geometry Dash Android 2.2.145, arm64-v8a.

Do not assume version code 42. The launcher reads the installed package's actual `longVersionCode`; the exact target version code must be recorded from a verified 2.2.145 APK/device. Version-name matching alone is not proof of binary compatibility.

## Native core status

The C++ loader core can validate manifests and packages, extract native payloads, verify AArch64 ELF files, and load native modules when called in the appropriate process. It is not yet connected to the independent Android app or Geometry Dash runtime.

## Existing Android example as the implementation reference

Use the MIT-licensed [gd-mod-example-android](https://github.com/iAndyHD3/gd-mod-example-android) already selected as the project reference. Its `jni/src/main.cpp` demonstrates the actual in-game pattern: hook `MenuLayer::init`, call the original init, create a sprite and `CCMenuItemSpriteExtra`, add it to a `CCMenu`, then attach that menu to the game layer. Its `custom-layer.cpp/.hpp` demonstrate creating a Cocos layer and transitioning to it for a custom screen.

The example targets an older 32-bit Geometry Dash build. Its old prebuilt libraries, old hook target, and hard-coded patch example must not be carried over to Amethisty's 2.2.145 arm64 target. Port the pattern—not its old binary assumptions. Keep the example's MIT copyright/license notice if any substantial source is reused.

The prior abstract `AmethistyGameUiHost` bridge was removed because it was unnecessary for this chosen approach. The next implementation is the real 2.2.145 arm64 in-game hook and Cocos menu, using verified target symbols/bindings and a compatible arm64 hook backend. The Mods button must be created from inside the game process on its UI thread.

## Remaining work

1. Port the example's in-game `MenuLayer::init` hook and Cocos button/layer pattern to a verified 2.2.145 arm64 runtime integration.
2. Confirm the exact 2.2.145 APK metadata and arm64 native binaries.
3. Connect the native loader after the game's required runtime initialization.
4. Implement the verified 2.2.145 Cocos UI host for the new bridge, then test the in-game Mods button and panel end to end.

Do not add guessed offsets, binary patches, or hooks. Verify the target binary and ABI before implementing runtime integration. Native mods execute inside the game process and are not sandboxed.
