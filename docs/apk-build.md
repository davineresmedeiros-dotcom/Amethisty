# Independent Amethisty Android app

Run the manual GitHub Actions workflow **Build independent Amethisty APK and native library**. It builds the Android app under `android/` from Amethisty's own C++ source with the Android NDK and uploads two separate artifacts:

- `Amethisty-independent-debug-apk` — debug APK for arm64-v8a.
- `Amethisty-native-arm64-v8a-so` — standalone `libamethisty.so`, built from this repository's native source (not the old Geode-derived binary).

The workflow verifies that the APK contains the native library and that the standalone library is an AArch64 ELF. The artifact is generated when the workflow is run; it is not checked into the repository.

## Launcher interface

The launcher uses a minimal layout inspired by the visual style of Geode's launcher, implemented independently in Amethisty:

- Amethisty title centered above the controls.
- One centered **Launch** button.
- A **Settings** gear beside Launch.
- No mod import/list button in the launcher; the mod interface belongs inside Geometry Dash.

## Implemented in this prototype

- Application ID: `com.amethisty.launcher`.
- Reads the installed Geometry Dash package using Android PackageManager.
- Enables Launch only when the game has a launch intent.
- Settings displays the installed game's actual `versionName` and `longVersionCode`, the target version `2.2.145`, and native-library status.
- Launches the installed game using its package launch intent.
- Compiles Amethisty's native loader core as `libamethisty.so` for arm64-v8a.

## Not implemented yet

This APK is **not yet a functioning in-game native mod loader**. The app does not load GD's native libraries in its own process, does not inject `libamethisty.so` into the game, and does not create a mods button inside Geometry Dash yet. The native loader core is currently compiled and packaged, but the verified in-game runtime integration remains unfinished.

The previous Geode-derived APK workflow was removed and must not be used. Do not describe this prototype as a finished loader until the in-game integration is implemented and tested on the target GD 2.2.145 arm64 installation.

The debug APK is for testing only; it is not release-signed.
