# Android and Geometry Dash integration

## Launcher startup point
The Android launcher must load Geometry Dash's native libraries and apply startup patches before Amethisty initializes. The Geode Android launcher is the integration reference, particularly GeometryDashActivity.tryLoadGame().

The intended insertion is after LauncherFix.performPatches() and before loadInternalMods(). At that point Amethisty checks that libcocos2dcpp.so is already loaded. It does not load game libraries itself or install guessed hooks.

## Amethisty runtime
The Kotlin adapter loads libamethisty.so and prepares:
- app-specific external amethisty/mods for imported .amethyst packages;
- private app storage files/amethisty for extracted native libraries.

It calls JNI initialization, which scans and validates packages, extracts declared libraries, verifies AArch64 ELF format, checks public mod API version, and attempts each native mod independently. A bad mod is logged and does not intentionally stop game startup.

## Target version
- Geometry Dash Android 2.2.145.
- Expected version code 42 is not authoritative until checked against exact APK metadata.
- The upstream launcher reference had compatibility checks for 2.2.144/version code 41; update and verify them for 2.2.145 in the APK project.

## Current limitations
- The native core and launcher source adapter are not a complete standalone Amethisty APK.
- No in-game mod management UI or package import UI exists yet.
- No verified Geometry Dash 2.2.145 bindings/hooks are installed.
- Native mods execute inside the game process and are not sandboxed.
- Device validation on the target game installation remains required.

Do not add guessed offsets or hooks. Confirm the exact game binary, version, ABI, and binding data first.
