# Independent Amethisty APK

The former APK workflow was removed. It checked out and modified Geode Android Launcher, so its output was not the independent Amethisty launcher requested.

**There is currently no APK build workflow or independent installable APK in this repository.** Do not install or distribute the previous Geode-derived artifact as an Amethisty build.

The independent Android application still needs to be implemented. Its version compatibility check must query the installed Geometry Dash package through Android PackageManager and compare the actual `versionName` and `longVersionCode` against values verified from a genuine GD 2.2.145 arm64 installation. Do not infer or hardcode version code 42 without verifying the installed APK metadata.

Once the independent launcher and runtime integration are implemented, add a manual GitHub Actions workflow to build the APK.
