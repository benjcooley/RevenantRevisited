# `revisited/` — overlay assets, source materials, and builders

Three sibling directories that together produce the **enhancement layer**
for Revenant Revisited. None of these files are part of the original
Cinematix Revenant — those belong to the user (their GoG / CD install).

## Hard invariant

**The engine must boot and play the retail game without this directory.**

If `revisited/` is empty or absent, Revenant Revisited runs as vanilla
retail Revenant — same balance, same assets, same scripts. Adding files
here only ever *enhances*; it never becomes a runtime requirement.

That keeps us legal (we don't ship original assets), keeps installs tiny
(only patches), and gives players a clean "play vanilla" mode by simply
moving the overlay aside.

## Layout

```
revisited/
├── resources/   ← runtime overlay. Mirrors the user's data/ tree exactly.
│                  At boot the engine mounts this AHEAD of the user's
│                  resources.rvr/imagery.rvi, so any file present here
│                  shadows its retail counterpart.
│                  In dev mode this is mounted as a loose folder.
│                  In production, packed into RevenantRevisited.rvr and
│                  shipped beside the per-platform binary.
│
├── srcassets/   ← source materials for everything in resources/.
│                  Higher-resolution textures, .blend / .max files,
│                  source scripts, raw WAVs, etc. Never read by the
│                  engine. Inputs to the build pipeline.
│
└── builders/    ← scripts that turn srcassets/ into resources/.
                   Texture conversions, I3D rebuilds, .rvr packers,
                   manifest generators. Run by hand or in CI.
```

An override sits at the path the engine asks the install for, which is
retail's path (docs/DATA_LAYOUT.md §2.4): a module's file under
`Modules/<module>/` (`resources/Modules/Ahkuilon/area.def`), a shared
game file under `Resources/` (`resources/Resources/rules.def`), an
imagery def under `Imagery/`.

## Discovery (engine boot)

Resolution order in `vfs_resolve_revisited_overlay`:

1. `$REVENANT_REVISITED_PATH` — explicit override (env var). Useful when
   developers keep the overlay outside the repo (e.g. on a different
   drive) or when QA wants to test against an alternate overlay.
2. `<exe-dir>/RevenantRevisited.rvr` — production. Mounted as a ZIP via
   the existing VFS layer.
3. `<RunPath>/RevenantRevisited.rvr` — alternate production location.
4. `<repo-root>/revisited/resources/` — dev mode loose folder. Detected
   by walking up from the executable's directory until we find a
   sibling `revisited/` next to `src/`.

Empty / missing → no overlay → vanilla retail. The engine logs at
boot whether an overlay was found and which mode (loose / archive).

## High-res tile art (local override, not an overlay path)

The map tile pass can take a tile's **colour** from an upscaled art tree
while depth, height min/max, coverage and the walkmap stay retail's
(`src/tiletexoverride.cpp`). This is a player-side extra: it is *not* the
overlay's path shadowing, and it does not need `--revisited`.

- `[Revisited] HighResTiles = 1` (default on) and
  `HighResTileRoot = <dir>` in the overlay's `Revenant.ini`. An empty
  root means `<overlay>/Imagery`, i.e. **`revisited/resources/Imagery/`**
  in the dev layout.
- Entries are named `<NNNN>_<Name>.png`, as `--dumptiles` writes them,
  and may sit in subdirectories. Hand-staged copies win over the bulk
  upscale, in tiers: `Final/` (finished picks) > `Remade/` + `Rework/`
  (hand-redone art) > the base set -- no matter which one the scanner
  meets first; `original/`, `alpha/`, `depth/` and other names are
  skipped.
- Only the still state's albedo is swapped. The override's alpha is
  clipped to the retail silhouette (nearest 1x sample) so art cannot
  leak outside the tile.
- `[Revisited] HighResTileMode = 0|1` (default 1 = modern): what a tile
  loads at boot -- the tree's art (1) or the retail pixels (0). The live
  switch/reload is `reloadtiles [retail|modern]` (console) or the panel
  button (see below): it re-indexes the tree and re-colours the tiles the
  game already holds, so editing a PNG shows up without leaving the game.
  Depth, height chain, coverage and the walkmap always stay retail's.
- The two keys above are read even without `--revisited`; every other
  Revisited tunable still needs the flag.
- Dropping PNGs straight into `revisited/resources/` does **nothing**:
  that folder shadows retail files by path (`Imagery/`, `Resources/`,
  `Modules/<module>/`, ...). Put tile art under `Imagery/` (or point
  `HighResTileRoot` elsewhere) and keep the set out of git;
  `builders/import_tile_textures.py` mirrors an external art tree into
  `Imagery/`.

## Sprite art (local override, retail vs modern)

The 3D models (characters, monsters, props, effect billboards) can take
their textures from a local tree of PNGs instead of the palettised bitmaps
inside the retail `.I3D` files (`src/spritetexoverride.cpp`). Same idea as
the tile art: a player-side extra that does not need `--revisited`.

- `[Revisited] SpriteOverride = 1` (default on), `SpriteRoot = <dir>` and
  `SpriteMode = 0|1` (0 = retail art, 1 = modern art) in the overlay's
  `Revenant.ini`. An empty root means `<overlay>/Sprites`, else
  `<HighResTileRoot>/sprites`.
- Entries are laid out the way `--dumpi3d` writes them:
  `<root>/<subdir>/<name>/texture_<NN>_frame_<NN>.png`, where
  `<subdir>/<name>` is the `.I3D`'s path without its extension
  (`Chars\locke.i3d` -> `chars/locke`). `texture_<NN>.png` covers every
  frame of that texture.
- The art has to have the retail texture's size: the model's own
  dimensions come from it (flipbook cells, face aspect). A differently
  sized file is skipped with a warning.
- The debug panel's top-level **Sprites** tab (F10, after *Map*) has two
  sections. **Location art (tiles)**: retail/modern radio for the tile
  art, an *art root* box -- type a folder, press "Use this folder" and the
  tree is re-indexed from it in the running game (empty keeps the
  configured default) -- plus "Reload tiles" (re-index the tree, re-read
  every tile PNG and put it into the tiles the renderer already draws --
  an edited PNG shows up in the running game). **Model textures (3D)**:
  the same switch, *art root* box and reload for the 3D models' textures.
  The boxes edit the session only; nothing is written to `Revenant.ini`
  until the panel's Save button. The console commands do the same
  from a script: `reloadsprites [retail|modern]`, `reloadtiles
  [retail|modern]` (each switches the mode, re-indexes and reloads) and
  `debugui` (open the panel).
- `--test=i3ddump --dumpi3d=<file.I3D>` writes the **retail** pixels of a
  model, so the dumped PNGs can be used as the base for modern art (the
  dump is taken before the art override is applied).

## Phase plan

- **Phase 1 (current — Demo 1)**: directory exists, empty. Engine
  discovers, mounts, finds nothing, boots vanilla. Validates the
  discovery code path without any actual overrides shipping yet.

- **Phase 2 (first published patch)**: ship one or two cosmetic /
  balance overrides (e.g. modified `rules.def` lines, a re-tuned
  texture). Validates the shadowing semantics end-to-end.

- **Phase 3 (auto-regen + manifest)**: `builders/manifest.toml`
  declares which files ship verbatim in `RevenantRevisited.rvr` and
  which are *derived* on first boot from the user's GoG sources.
  Derived files live in the engine's `<SavePath>/cache/` (see the
  root README's "Cache" section) — separate from this overlay.
  Reduces shipped patch size, makes the regen recipe transparent and
  auditable.

## Categories of enhancements

The overlay can carry any of:

1. **Bug fixes** — split into two kinds:
   - *Pure-broken* bugs (engine crashes, parser fails, attack always
     rejects) — always-on, no toggle. Without these the game can't be
     played at all on modern hardware. Live in engine code, not here.
   - *Behavioral* "fixes" (e.g. tightening retail's monster wander
     timer because it feels broken to a 2026 player) — toggleable, see
     below. Live in the overlay if data-driven, in engine code with
     a runtime gate if logic-driven.
2. **Game behavior / area changes** — modified sectors, alternate
   scripts, retuned monster stats, new triggers, optional alternate
   storylines.
3. **Enhanced graphics / sound / effects** — higher-resolution textures,
   re-mastered audio, additional particle effects, replacement I3Ds.
4. **Enhanced UI** — alternate fonts, redrawn HUD elements, expanded
   automap art.

Every one of these is optional. The engine **must not** assume any of
them are present; if the overlay is absent the game plays exactly as
the original Cinematix retail did.

### Toggle model for behavioral changes

Anything that's not a pure-broken-engine fix is a **runtime toggle**
the player can turn off, even when the overlay is installed. Toggles
default to "off" (= vanilla) so an installed overlay still produces
retail behavior unless the user opts in.

Toggles live in `<SavePath>/Revenant.ini` under `[Revisited]`:

```ini
[Revisited]
BugFix.MonsterImpactAniFallback = true   ; was rejected silently in retail
Tweak.MonsterAggressionRange    = false  ; vanilla wander/sight
Visual.HighResTextures          = true   ; if overlay ships them
UI.ExpandedHUD                  = false
```

Engine code reads via a single accessor:

```cpp
if (Revisited::IsEnabled("BugFix.MonsterImpactAniFallback")) {
    // revisited path
} else {
    // verbatim retail path
}
```

This keeps both paths live and reviewable, and lets us fall back to
vanilla on any toggle if a bug is reported. (Implementation: TBD —
add when the first behavioral toggle ships.)

## What does NOT belong here

- **User saves** → `<SavePath>/Save/Single/<n>/` (App Support /
  XDG_DATA_HOME / LOCALAPPDATA — never the install dir).
- **User INI / preferences** → `<SavePath>/Revenant.ini`.
- **Original Revenant assets** → user's GoG install, never copied or
  redistributed by us.
- **Engine-derived asset conversions** → `<SavePath>/cache/`. That's
  *runtime working storage* for converting GoG assets into engine-
  friendlier forms (different renderer pipeline, different sector
  format, etc.). It is regeneratable on demand, distinct in role and
  location from this opt-in overlay. See the root README's "Cache"
  section.
