# Integrating Amethisty into an Android launcher source project

This folder contains source snippets for a launcher developer. It does not contain a complete launcher APK.

## Files

- AmethistyLoader.kt: Kotlin wrapper around System.loadLibrary("amethisty").
- GeometryDashActivity.patch: insertion point in the Geode Android launcher's GeometryDashActivity.

## Add the Kotlin source

Copy AmethistyLoader.kt into:

app/src/main/java/com/geode/launcher/AmethistyLoader.kt

The file uses package com.geode.launcher, matching the launcher activity package.

## Add the native library

Build Amethisty as a shared library named libamethisty.so for the same ABI that the launcher uses. Put it under the launcher's app/src/main/jniLibs/<ABI>/ directory, for example:

- app/src/main/jniLibs/arm64-v8a/libamethisty.so
- app/src/main/jniLibs/armeabi-v7a/libamethisty.so

Do not put a library built for one ABI in another ABI directory.

## Add the startup call

The patch inserts AmethistyLoader.load() directly after LauncherFix.performPatches() and before loadInternalMods() in GeometryDashActivity.tryLoadGame(). This is after the launcher has loaded Cocos and applied its startup patches.

The patch is based on the upstream source layout at the time this integration was prepared. If the launcher source has changed, apply the same insertion manually at the equivalent startup point rather than forcing a stale patch.

## Important target-version warning

The upstream launcher Constants.kt currently declares supported version string 2.2.144 and supported version code 41. Amethisty targets Geometry Dash 2.2091, so do not assume the current upstream launcher release supports that target. The launcher itself must first be updated and tested for the target game version.

## Verify on device

After the launcher is built and installed, inspect logcat for the Amethisty tag. Expected messages include the JNI entry and ABI/Cocos status. If the JNI entry does not appear, the launcher did not successfully load libamethisty.so.

This integration only loads Amethisty and reports runtime state. It does not yet install game hooks or load .amethyst mods.
