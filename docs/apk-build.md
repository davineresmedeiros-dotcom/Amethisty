# Building the Amethisty APK

The APK workflow is a manual GitHub Actions workflow: Actions → Build Amethisty APK (arm64) → Run workflow.

It checks out a pinned revision of the Geode Android Launcher under its BSL-1.0 license, builds Amethisty for arm64-v8a, applies a small scripted integration, and builds the installable standard-debug APK.

The preparation script:
- changes the application ID to com.amethisty.launcher;
- brands the launcher as Amethisty;
- limits the APK to arm64-v8a, matching the native loader;
- updates launcher compatibility metadata to Geometry Dash 2.2.145 (expected version code 42);
- inserts Amethisty initialization after the launcher startup patches;
- packages libamethisty.so and the upstream/license notices.

The resulting debug APK is signed with the standard Android debug key and can be installed for testing. It is not a release-signed distribution build. The workflow artifact expires after 14 days.

Before distributing, verify the exact Geometry Dash APK version code on-device, test launch and mod import, review all upstream/third-party notices, and create a release signing configuration. Native mods are not sandboxed.
