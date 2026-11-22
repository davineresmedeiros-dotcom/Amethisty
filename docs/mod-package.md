# Amethisty mod package format

**Status: package-format proposal; archive parsing is not implemented yet.**

The file extension is `.amethyst`. The intended container is a ZIP archive with this layout:

```text
ExampleMod.amethyst
├── manifest.json
├── mod/
│   └── libExampleMod.so
└── assets/
    └── (optional mod assets)
```

The package root must contain `manifest.json`. The native module filename must match the manifest's `library` field and target the ABI of the running game process.

## Manifest

```json
{
  "format": 1,
  "id": "author.examplemod",
  "name": "Example Mod",
  "version": "1.0.0",
  "author": "Author",
  "description": "A short description.",
  "game": "2.2091",
  "library": "mod/libExampleMod.so"
}
```

## Validation rules planned for format 1

- `format` must be the integer `1`.
- `id` must be unique and use lowercase reverse-domain-style identifiers.
- `name`, `version`, and `library` must be non-empty strings.
- `game` must match the supported Geometry Dash version.
- `library` must be a relative path inside the archive, use forward slashes, and point to a regular file.
- Reject absolute paths, parent-directory components (`..`), duplicate manifest keys, malformed JSON, duplicate mod IDs, and missing native libraries.
- Extraction must stay inside the loader's private mod directory. Never trust archive entry paths.
- Do not load a library until the archive and manifest have passed validation and extraction succeeds.

## Current implementation

The native `ModManager` currently scans a selected directory for regular files with the `.amethyst` extension. It rejects symlinks, skips hidden entries, and sorts the results deterministically. Candidate metadata is still filename-derived and provisional.

It does **not** open ZIP archives, parse manifests, extract files, check ABI compatibility, or load native mod libraries. The scan result is not safe or sufficient to pass to `dlopen()`.

## Native code warning

Amethisty mods are native code running in the game's process. A mod can crash the game or access process data. The loader cannot sandbox native mods; install only mods you trust.
