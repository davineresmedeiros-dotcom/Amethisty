# Android and Geometry Dash integration

## Confirmed Android entry point

The official Geode Android launcher source provides a concrete integration point. Its GeometryDashActivity.tryLoadGame() flow:

1. Sets up game data redirection.
2. Initializes Cocos2d-x and the launcher context.
3. Loads libfmod.so and libcocos2dcpp.so.
4. Applies LauncherFix.performPatches().
5. Loads internal mods and creates the game view.
6. Loads Geode after the post-library setup.

Geometry Dash's game code is hosted in libcocos2dcpp.so; the launcher source does not load a separate libgame.so. Amethisty's runtime inspection has been corrected to check the actual Cocos library only.

Reference implementation: https://github.com/geode-sdk/android-launcher, especially app/src/main/java/com/geode/launcher/GeometryDashActivity.kt and utils/Constants.kt.

## Amethisty launcher adapter

A small Kotlin adapter is provided at integrations/geode-android-launcher/AmethistyLoader.kt. It calls System.loadLibrary("amethisty") and logs a load failure without preventing the game from starting.

To integrate it into a launcher source project:

1. Add AmethistyLoader.kt to the launcher's app/src/main/java/com/geode/launcher/ source directory.
2. Package the native output as libamethisty.so in the matching ABI folder under app/src/main/jniLibs/ (for example, arm64-v8a and armeabi-v7a). The launcher and native library must use the same ABI.
3. In GeometryDashActivity.tryLoadGame(), call AmethistyLoader.load() immediately after LauncherFix.performPatches() and before loadInternalMods().

At this point the launcher has already loaded Cocos and applied its startup patches, so JNI_OnLoad can report the expected runtime state. This adapter is a source integration example; it does not modify or re-sign an installed APK by itself.

## Target version notes

The launcher source identifies Geometry Dash's package as com.robtopx.geometryjump. Its version map identifies Geometry Dash 2.2091 as version code 43. Confirm the installed game's version and ABI before building or testing; launcher releases may not yet support every game version.

## Current implementation status

Implemented in this repository:

- JNI 1.6 entry point.
- ABI detection.
- Non-invasive detection of an already-loaded libcocos2dcpp.so.
- Kotlin launcher adapter that requests loading libamethisty.so.

Not implemented yet:

- A complete Amethisty launcher application or APK patcher.
- Automatic packaging of the native library into a launcher.
- Verified Cocos/game bindings or hooks for Geometry Dash 2.2091.
- ZIP archive reading, manifest.json parsing, extraction, or mod loading.

Do not add hooks or guessed offsets until the exact 2.2091 native binary and bindings are verified. This repository has not been compiled or device-tested by the project assistant.
