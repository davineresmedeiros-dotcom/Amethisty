#!/usr/bin/env python3
"""Prepare a pinned Geode Android Launcher checkout for an Amethisty arm64 APK build."""
import argparse
import shutil
from pathlib import Path

def replace_once(path: Path, old: str, new: str) -> None:
    text = path.read_text(encoding="utf-8")
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"Expected exactly one match in {path}, found {count}: {old!r}")
    path.write_text(text.replace(old, new, 1), encoding="utf-8")

def replace_all(path: Path, old: str, new: str) -> None:
    text = path.read_text(encoding="utf-8")
    count = text.count(old)
    if count == 0:
        raise RuntimeError(f"Expected at least one match in {path}: {old!r}")
    path.write_text(text.replace(old, new), encoding="utf-8")

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--launcher-root", type=Path, required=True)
    parser.add_argument("--native-lib", type=Path, required=True)
    args = parser.parse_args()

    root = args.launcher_root.resolve()
    repo = Path(__file__).resolve().parent.parent
    gradle = root / "app/build.gradle.kts"
    constants = root / "app/src/main/java/com/geode/launcher/utils/Constants.kt"
    strings = root / "app/src/main/res/values/strings.xml"
    activity = root / "app/src/main/java/com/geode/launcher/GeometryDashActivity.kt"
    settings = root / "settings.gradle.kts"
    manifest = root / "app/src/main/AndroidManifest.xml"

    # Android API 37 is not published in the runner SDK repository. Pin launcher
    # libraries to API-36-compatible releases and compile against API 36.
    replace_once(gradle, "compileSdk = 37", "compileSdk = 36")
    replace_once(gradle, "targetSdk = 37", "targetSdk = 36")

    # Keep dependencies compatible with compileSdk 36 (upstream's latest
    # versions currently advertise API 37 as their minimum compile SDK).
    replace_once(gradle, '2026.09.00', '2025.08.00')
    replace_once(gradle, '1.19.1', '1.17.0')
    replace_all(gradle, '2.11.0', '2.9.3')
    replace_all(gradle, '1.13.0', '1.10.1')
    replace_once(gradle, '1.8.0', '1.7.1')
    replace_once(gradle, '3.18.2', '3.15.0')
    replace_all(gradle, '5.5.0', '5.1.0')
    replace_all(gradle, '1.11.0', '1.9.0')
    replace_once(gradle, '1.10.0', '1.9.0')
    # Renderer 0.42+ requires compileSdk 37; use the compatible 0.38.1 release.
    replace_all(gradle, '0.46.0', '0.38.1')

    replace_once(gradle, 'applicationId = "com.geode.launcher"', 'applicationId = "com.amethisty.launcher"')
    replace_once(gradle, "versionCode = 31", "versionCode = 1")
    replace_once(gradle, 'versionName = "1.9.0"', 'versionName = "0.1.0"')
    replace_once(gradle, 'ndk.abiFilters += listOf("arm64-v8a", "armeabi-v7a")',
                 'ndk.abiFilters += listOf("arm64-v8a")')
    replace_once(gradle, 'include("arm64-v8a", "armeabi-v7a")', 'include("arm64-v8a")')
    replace_once(settings, 'rootProject.name = "launcher"', 'rootProject.name = "amethisty-launcher"')

    replace_once(constants, "const val SUPPORTED_VERSION_CODE_MIN_WARNING = 41L",
                 "const val SUPPORTED_VERSION_CODE_MIN_WARNING = 42L")
    replace_once(constants, "const val SUPPORTED_VERSION_CODE = 41L",
                 "const val SUPPORTED_VERSION_CODE = 42L")
    replace_once(constants, 'const val SUPPORTED_VERSION_STRING = "2.2.144"',
                 'const val SUPPORTED_VERSION_STRING = "2.2.145"')

    replace_once(strings, '<string name="app_name" translatable="false">Geode</string>',
                 '<string name="app_name" translatable="false">Amethisty</string>')
    replace_once(strings, '<string name="launcher_title">Geode</string>',
                 '<string name="launcher_title">Amethisty</string>')
    replace_once(manifest, 'android:icon="@mipmap/ic_launcher"',
                 'android:icon="@drawable/ic_amethisty_launcher"')

    insertion = (
        "        LauncherFix.performPatches()\n\n"
        "        // Initialize Amethisty after Cocos and launcher patches are ready.\n"
        "        AmethistyLoader.load(this)\n\n"
        "        loadInternalMods()"
    )
    replace_once(activity, "        LauncherFix.performPatches()\n\n        loadInternalMods()", insertion)

    icon_source = repo / "integrations/geode-android-launcher/ic_amethisty_launcher.xml"
    icon_target = root / "app/src/main/res/drawable/ic_amethisty_launcher.xml"
    icon_target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(icon_source, icon_target)

    adapter_source = repo / "integrations/geode-android-launcher/AmethistyLoader.kt"
    adapter_target = root / "app/src/main/java/com/geode/launcher/AmethistyLoader.kt"
    adapter_target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(adapter_source, adapter_target)

    native_target = root / "app/src/main/jniLibs/arm64-v8a/libamethisty.so"
    native_target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(args.native_lib.resolve(), native_target)

    licenses = root / "app/src/main/assets/licenses"
    licenses.mkdir(parents=True, exist_ok=True)
    shutil.copy2(root / "LICENSE.txt", licenses / "GEODE_ANDROID_LAUNCHER_BSL-1.0.txt")
    shutil.copy2(repo / "LICENSE", licenses / "AMETHISTY_MIT.txt")
    third_party = repo / "docs/third-party.md"
    if third_party.exists():
        shutil.copy2(third_party, licenses / "THIRD_PARTY_NOTICES.md")

    print("Prepared Amethisty arm64 launcher source.")
    print("Application ID: com.amethisty.launcher")
    print("Target Geometry Dash: 2.2.145 / expected version code 42")
    print(f"Native library: {native_target}")

if __name__ == "__main__":
    main()
