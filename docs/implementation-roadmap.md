# Implementation roadmap

This roadmap separates package handling from game integration so each layer can be reviewed independently.

## 1. Package discovery (current prototype)

- Enumerate regular .amethyst files from the chosen mods directory.
- Reject symlinks and keep scan order deterministic.
- Do not treat filename-derived metadata as trusted manifest data.

## 2. Android and game integration (in progress)

- Use the verified Android launcher load sequence as the integration reference.
- Package libamethisty.so for the launcher's matching ABI and load it after Cocos and launcher patches.
- Confirm the JNI entry and Cocos detection in logcat on the target device.
- Confirm Geometry Dash Android 2.2.145 (expected version code 42) and the installed ABI from the exact APK/device.
- Add hooks only after the target bindings and offsets are verified.

## 3. Archive and manifest handling (after integration)

- Add a ZIP reader with bounded entry count, file size, and decompressed size.
- Parse manifest.json with a small, tested JSON parser.
- Validate format version, ID, game version, library path, and required fields.
- Extract only required files to an app-private directory using safe path handling.
- Reject malformed archives, path traversal, duplicate IDs, and missing libraries.

## 4. Native module loading

- Confirm the process ABI and package ABI before loading.
- Load only the extracted library from the validated private directory.
- Resolve the documented entry symbol and verify API version.
- Keep load errors isolated per mod and report them through the host logger.

## 5. Device validation

- First validate package scanning and manifest parsing with native tests.
- Then test malformed and hostile ZIP cases.
- Finally test on a backed-up Geometry Dash installation, with logs and a way to disable all mods.

## Current limitation

Amethisty is still a scaffold. The launcher adapter is a source integration example, not a complete launcher APK. Archive parsing, extraction, mod loading, game hooks, and device validation are not implemented. The project assistant has not compiled or device-tested this repository.
