# Android and Geometry Dash integration

## Confirmed Android entry point

The official Geode Android launcher source provides a concrete integration point. Its GeometryDashActivity.tryLoadGame() flow:

1. Sets up game data redirection.
2. Initializes Cocos2d-x and the launcher context.
3. Loads libfmod.so and libcocos2dcpp.so.
4. Applies LauncherFix.performPatches().
5. Loads internal mods and creates the game view.
6. Runs post-library setup and loads Geode.

Geometry Dash's game code is hosted in libcocos2dcpp.so; the launcher source does not load a separate libgame.so. Amethisty's runtime inspection checks the actual Cocos library only.

Reference implementation: https://github.com/geode-sdk/android-launcher, especially app/src/main/java/com/geode/launcher/GeometryDashActivity.kt and utils/Constants.kt.

## Amethisty launcher adapter

The Kotlin adapter at integrations/geode-android-launcher/AmethistyLoader.kt calls System.loadLibrary("amethisty") and logs a load failure without preventing the game from starting.

The source patch at integrations/geode-android-launcher/GeometryDashActivity.patch inserts the load call after LauncherFix.performPatches() and before loadInternalMods(). See integrations/geode-android-launcher/README.md for exact source placement and ABI packaging instructions.

At this point the launcher has already loaded Cocos and applied its startup patches, so JNI_OnLoad can report the expected runtime state. These files are source integration artifacts; they do not modify or re-sign an installed APK by themselves.

## Target version notes

- Target Geometry Dash Android version: 2.2.145.
- Expected Android version code: 42; confirm against the exact APK metadata before using it as a hard compatibility gate.
- The upstream launcher source consulted for this integration declares supported version string 2.2.144 and version code 41. Its compatibility checks must be updated and tested for 2.2.145 before building the integrated launcher.

## Current implementation status

Implemented in this repository:

- JNI 1.6 entry point.
- ABI detection.
- Non-invasive detection of an already-loaded libcocos2dcpp.so.
- Kotlin launcher adapter and source patch that request loading libamethisty.so.
- Step-by-step instructions for integrating the adapter into a launcher source tree.

Not implemented yet:

- A complete Amethisty launcher application or APK patcher.
- Automatic packaging of the native library into a launcher.
- Verified Cocos/game bindings or hooks for Geometry Dash 2.2.145.
- ZIP archive reading, manifest.json parsing, extraction, or mod loading.

Do not add hooks or guessed offsets until the exact 2.2.145 native binary and bindings are verified. This repository has not been compiled or device-tested by the project assistant.
