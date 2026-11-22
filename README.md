# Amethisty

Amethisty is an Android mod-loader project targeting Geometry Dash **2.2.145** (Android version code **42**, pending confirmation from the exact installed APK metadata).

## Status

**Prototype / early development — not ready to install or compile as a complete loader.**

The native code has a JNI entry point, ABI detection, non-invasive detection of the already-loaded Geometry Dash Cocos library, a launcher-load adapter, and a directory scanner for files ending in .amethyst. It does not yet install game hooks, read package archives, parse manifests, extract or load mods. See [Android integration status](docs/android-integration.md) and the [launcher source integration guide](integrations/geode-android-launcher/README.md).

## Planned features

- Android native loader for Geometry Dash 2.2.145
- In-game mod management
- Dedicated .amethyst mod package format
- Native mod API and developer SDK

## Development approach

We are studying and adapting these projects:

- [GD Mod Example Android](https://github.com/iAndyHD3/gd-mod-example-android)
- [Android-ML](https://github.com/BlackTeaML/Android-ML)
- [Geode Android Launcher](https://github.com/geode-sdk/android-launcher) for the Android startup integration reference

Legacy mod examples are not assumed to work with Geometry Dash 2.2.145 unchanged. See [the compatibility audit](docs/base-audit.md). Preserve original copyright and license notices for any files copied or adapted.

All builds and device testing are performed manually.

## Mod package

See [the Amethisty package specification](docs/mod-package.md).

## License

Amethisty's own code is under the MIT License in this repository. Third-party components retain their original licenses and notices.
