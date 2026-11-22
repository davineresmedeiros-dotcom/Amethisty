# Amethisty mod package format

**Status: proposal for the first package format.**

The file extension is `.amethyst`. A package is planned as a ZIP container with this layout:

```text
ExampleMod.amethyst
├── manifest.json
├── mod/
│   └── libExampleMod.so
└── assets/
    └── (optional mod assets)
```

The package's root must contain `manifest.json`. The native module filename must match the manifest's `library` field and target the device ABI supported by the loader.

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

## Current implementation

The current native ModManager only enumerates files ending in `.amethyst` in a directory. It derives a temporary display ID from the filename. It does **not** open ZIP archives, parse manifests, extract files, or load native mod libraries yet. These are the next implementation steps.

## Validation and security requirements

- `format` identifies the package format version, not the mod version.
- `id` must be unique and use lowercase reverse-domain-style identifiers.
- `version` is the mod's semantic version.
- `game` declares the supported Geometry Dash version.
- The loader must reject malformed manifests, duplicate IDs, unsupported package versions, and missing libraries.
- Package extraction must validate paths and must not allow files to escape the mod's own directory.
- Mods are native code and can crash the game or access the process. Install only mods you trust.
