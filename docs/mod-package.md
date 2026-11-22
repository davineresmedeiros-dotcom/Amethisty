# Amethisty mod package format

Status: format-1 manifest validation and a bounded ZIP reader/extractor exist. The ModManager does not yet call the reader, and native mods are not loaded.

A .amethyst package is a ZIP archive containing a root manifest.json and the native library declared by the library field.

Expected layout:
- manifest.json
- mod/libExampleMod.so

Example manifest fields:
- format: 1
- id: author.examplemod
- name: Example Mod
- version: 1.0.0
- game: 2.2.145
- library: mod/libExampleMod.so

The reader supports stored and deflate entries. It limits archive size, entry count, and uncompressed data; rejects ZIP64, multi-disk, encrypted, duplicate, unsafe-path, symlink and special-file entries; checks local/central headers and CRC; and extracts only the declared library to app-private storage.

The package reader is currently a standalone core API. ModManager::scan still only enumerates .amethyst files and uses provisional filename metadata. The extracted .so is not loaded or executed.

Native mods run inside the game process and are not sandboxed. A later loading stage must verify ABI and the mod entry/API contract before loading any library.
