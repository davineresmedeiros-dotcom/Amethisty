# Android and Geometry Dash integration

## Current integration stage

Amethisty currently provides a JNI shared-library entry point. On Android, `JNI_OnLoad` verifies that it can obtain a JNI 1.6 environment and logs the process ABI plus whether the already-loaded `libcocos2dcpp.so` and `libgame.so` libraries are visible.

The runtime inspection uses `dlopen(..., RTLD_NOLOAD)` and immediately releases the temporary handles. It does not load another copy of the game libraries.

## What this does not do yet

- It does not provide an Android launcher or patch an APK.
- It does not inject itself into Geometry Dash.
- It does not initialize Cocos2d-x.
- It does not install hooks or patch game code.
- It does not load `.amethyst` packages or mod libraries.

## Integration order to verify on-device

The native library must be loaded by the Android app/launcher as part of the game's native startup, after the game libraries are available if runtime inspection is expected to report them. The exact Java/smali loading location depends on the installed Geometry Dash APK and its version; do not paste legacy instructions for an older APK without checking the actual package.

For the target 2.2091 build, verify the package ID, ABI, library names, and native load order from the installed APK before wiring hooks. The legacy Android example describes an older setup and is not authoritative for this target.

## Next integration work

1. Confirm the target APK's Java load sequence and native library filenames.
2. Add a launcher/injection integration appropriate to the actual APK, preserving the original game libraries and signing requirements.
3. Validate runtime status in logcat.
4. Only then add version-verified Cocos/game bindings and hook initialization.

Until those steps are complete, the library is a JNI/runtime-inspection prototype, not an installable loader.
