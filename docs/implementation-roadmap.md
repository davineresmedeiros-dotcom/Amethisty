# Implementation roadmap

This roadmap separates package handling from game integration so each layer can be reviewed independently.

## 1. Package discovery (current prototype)

- Enumerate regular `.amethyst` files from the chosen mods directory.
- Reject symlinks and keep scan order deterministic.
- Do not treat filename-derived metadata as trusted manifest data.

## 2. Android and game integration (in progress)\n\n- Verify the exact Geometry Dash 2.2091 APK, package ID, ABI, and native library load order.\n- Integrate the native library through the appropriate launcher/APK startup path.\n- Confirm runtime library detection in logcat.\n- Add hooks only after the target bindings and offsets are verified.\n\n## 3. Archive and manifest handling (after integration)

- Add a ZIP reader with bounded entry count, file size, and decompressed size.
- Parse `manifest.json` with a small, tested JSON parser.
- Validate format version, ID, game version, library path, and required fields.
- Extract only required files to an app-private directory using safe path handling.
- Reject malformed archives, path traversal, duplicate IDs, and missing libraries.

## 3. Native module loading

- Confirm the process ABI and package ABI before loading.
- Load only the extracted library from the validated private directory.
- Resolve the documented entry symbol and verify API version.
- Keep load errors isolated per mod and report them through the host logger.

## 4. Geometry Dash integration

- Confirm the exact installed game version and ABI on-device.
- Integrate with the Android launcher/native library initialization sequence.
- Add game bindings/hooks only after validating their source and version.
- Do not copy old offsets or prebuilt game libraries without verification.

## 5. Device validation

- First validate package scanning and manifest parsing with native tests.
- Then test malformed and hostile ZIP cases.
- Finally test on a backed-up Geometry Dash installation, with logs and a way to disable all mods.

## Current limitation

Amethisty is still a scaffold. The package scanner is present, but archive parsing, extraction, mod loading, game hooks, and a complete Android launcher integration are not implemented. This repository has not been compiled or device-tested by the project assistant.
