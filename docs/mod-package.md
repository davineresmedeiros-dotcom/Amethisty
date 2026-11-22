# Amethisty mod package format

Status: format-1 manifest validation, bounded ZIP reading, and private library extraction are implemented. Native module loading is not.

A .amethyst package is a ZIP archive containing a root manifest.json and the native library declared by the library field.

Expected layout:
- manifest.json
- mod/libExampleMod.so

Example manifest:
- format: 1
- id: author.examplemod
- name: Example Mod
- version: 1.0.0
- game: 2.2.145
- library: mod/libExampleMod.so

The reader supports stored and deflate entries. It limits archive size, entry count and decompressed data; rejects ZIP64, multi-disk, encrypted, duplicate, unsafe-path, symlink and special-file entries; checks local/central headers and CRC; and extracts only the declared library to app-private storage.

ModManager.scanAndExtract now:
1. Scans regular .amethyst files in deterministic order.
2. Parses each manifest and fills candidate ID/name/version.
3. Rejects duplicate mod IDs before extracting any package.
4. Extracts valid package libraries and records per-package errors.

The extracted .so is not loaded or executed. ABI validation and the native mod entry/API contract are still pending.

Native mods run inside the game process and are not sandboxed.
