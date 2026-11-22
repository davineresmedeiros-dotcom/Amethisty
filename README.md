# Amethisty

Amethisty is an Android mod-loader project targeting Geometry Dash 2.2091.

## Status

**Prototype / early development — not ready to install or compile as a complete loader.**

The current native code has a basic Android library entry point and a directory scanner for files ending in `.amethyst`. It does not yet read package archives, parse mod manifests, load mods, or install game hooks.

## Planned features

- Android native loader for Geometry Dash 2.2091
- In-game mod management
- Dedicated `.amethyst` mod package format
- Native mod API and developer SDK

## Development approach

We are studying and adapting these projects:

- [GD Mod Example Android](https://github.com/iAndyHD3/gd-mod-example-android)
- [Android-ML](https://github.com/BlackTeaML/Android-ML)

They are legacy projects and are not assumed to work with Geometry Dash 2.2091 unchanged. See [the compatibility audit](docs/base-audit.md). Preserve the original copyright and MIT license notices for any files copied or adapted.

All builds and device testing are performed manually.

## Mod package

See [the Amethisty package specification](docs/mod-package.md).

## License

Amethisty's own code is under the MIT License in this repository. Third-party components retain their original licenses and notices.
