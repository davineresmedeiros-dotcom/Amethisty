# Integrating Amethisty into an Android launcher source project

These are source integration files, not a complete launcher APK.

## Files
- AmethistyLoader.kt loads libamethisty.so, creates mod directories, and calls native initialization.
- GeometryDashActivity.patch inserts the call after game startup patches.

Copy the Kotlin adapter into app/src/main/java/com/geode/launcher/AmethistyLoader.kt.

Build Amethisty for the launcher's ABI and package it at:
app/src/main/jniLibs/arm64-v8a/libamethisty.so

Apply the patch at the equivalent point in GeometryDashActivity.tryLoadGame(), after Cocos and launcher patches are ready and before internal mods start. If upstream source changed, apply the call at the matching lifecycle point instead of forcing a stale patch.

The adapter stores user packages in the app-specific external files directory amethisty/mods and extracts libraries into private app storage. A future APK UI should let users import .amethyst packages into that folder.

Target: Geometry Dash Android 2.2.145. Verify the exact APK version code before enforcing a version gate.

Native mods run inside the game process and are not sandboxed. This integration does not produce, patch, or re-sign an APK. Verified game hooks, in-game UI, and device validation remain separate tasks.
