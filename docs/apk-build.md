# Independent Amethisty Android app

Run the manual GitHub Actions workflow **Build independent Amethisty APK and native library**. It builds the Android app under `android/` from Amethisty's own C++ source with the Android NDK and uploads two separate artifacts:

- `Amethisty-independent-debug-apk` — debug APK for arm64-v8a.
- `Amethisty-native-arm64-v8a-so` — standalone `libamethisty.so`, built from this repository's native source (not the old Geode-derived binary).

The workflow verifies that the APK contains the native library and that the standalone library is an AArch64 ELF. The artifact is generated when the workflow is run; it is not checked into the repository.

## Implemented in this prototype

- Application ID: `com.amethisty.launcher`.
- Reads the installed Geometry Dash package using Android PackageManager.
- Displays the actual installed `versionName` and `longVersionCode`.
- Reports whether the version name is the target `2.2.145`; it does not assume that version code 42 is correct.
- Launches the installed game using its package launch intent.
- Compiles Amethisty's native loader core as `libamethisty.so` for arm64-v8a.

## Not implemented yet

This APK is **not yet a functioning in-game native mod loader**. The app does not load GD's native libraries in its own process, does not inject `libamethisty.so` into the game, and does not load `.amethyst` mods inside Geometry Dash at runtime. The native loader core is currently compiled and packaged, but the verified in-game runtime integration remains unfinished.

The previous Geode-derived APK workflow was removed and must not be used. Do not describe this prototype as a finished loader until the in-game integration is implemented and tested on the target GD 2.2.145 arm64 installation.

The debug APK is for testing only; it is not release-signed.
