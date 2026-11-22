# Base project compatibility audit

Checked against the public repository files during initial development.

## GD Mod Example Android

Repository: https://github.com/iAndyHD3/gd-mod-example-android

- The README describes an older Android injection/template workflow and asks for Android NDK r16b.
- Its checked-in native build uses legacy Android.mk / Application.mk.
- The sample's README discusses a much older Geometry Dash APK and old library loading instructions.
- It is useful as a reference for Android native mod initialization, but its bundled binaries, offsets, and game-specific hooks must not be presumed compatible with 2.2091.

## Android-ML

Repository: https://github.com/BlackTeaML/Android-ML

- Its CMake configuration uses C++20 and a fixed `arm64-v8a` ABI.
- It links against a bundled `libcocos2dcpp.so` and Dobby archive.
- It is a native modding/library base, not a complete ready-made 2.2091 loader.
- Its headers and binary dependencies must be checked against the exact target game version and ABI before integration.

## Integration rule

Do not copy old offsets or prebuilt game libraries into Amethisty and label them 2.2091 compatible. First establish the target APK version, package ABI, exact native library version, and correct symbol/binding set. Then port the needed source and record the upstream file and revision in [third-party notices](third-party.md).

The current Amethisty CMake target is only a small source scaffold; it does not yet link either upstream project's full native dependencies.
