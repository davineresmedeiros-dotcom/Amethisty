# Independent Amethisty Android app

The manual GitHub Actions workflow **Build independent Amethisty APK** builds the app under `android/` and uploads `Amethisty-independent-debug-apk`.

This app is authored as an independent Android project; it does not check out, rename, or build the Geode Android Launcher.

## Implemented in this prototype

- Application ID: `com.amethisty.launcher`.
- Reads the installed Geometry Dash package using Android PackageManager.
- Displays the actual installed `versionName` and `longVersionCode`.
- Reports whether the version name is the target `2.2.145`; it does not assume that version code 42 is correct.
- Launches the installed game using its package launch intent.

## Not implemented yet

This APK is **not yet a functioning native mod loader**. The app does not load GD's native libraries in its own process, does not inject `libamethisty.so` into the game, and does not load `.amethyst` mods at runtime. The native loader core is currently separate.

The previous Geode-derived APK workflow was removed and must not be used. The independent runtime integration and mod UI remain future work. Do not describe this prototype as a finished loader until those parts are implemented and tested on the target GD 2.2.145 arm64 installation.

The debug APK is for testing only; it is not release-signed.
