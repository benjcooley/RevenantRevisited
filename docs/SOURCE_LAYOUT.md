# Source layout

Since 2026-10-18 `src/` is two trees. `src/engine` is the engine: it knows
nothing about Revenant. `src/game` is this game. Every include is spelled
from `src/` (`#include "engine/scene/node.h"`, `#include "game/object/object.h"`),
which is on the include path for every target.

**The one rule:** nothing under `src/engine` includes anything under
`src/game`. `tools/test_layering.py` enforces it (CTest `test_layering.py`).
Engine files include `engine/core/revengine.h`, the engine umbrella; game
files include `game/app/revenant.h`, which includes the engine umbrella and
adds the game's globals.

## engine

| Folder | Holds |
|---|---|
| `core` | base types and defines (`revtypes.h`, `revdefs.h`, `revstring.h`), the umbrella `revengine.h`, portability shims, logging and the fatal-error service, streams, the DEF parser, JSON, hashing, time, math, soft refs, the integer views of a transform (`intworld.h`) |
| `refl` | reflection records and registry, the packed codec, DEF documents and the DEF reader/writer over reflection, node packing |
| `scene` | `TNode`, `TComponent`, `TTransform`, the animation system, prefabs |
| `assets` | resources and the asset cache, bitmaps and their decoders, chunk cache, fonts and the TrueType atlas, the I3D and sprite imagery bodies, wave data, the multi-resource archive |
| `render` | the renderer and its submission types, surfaces and blits, the display and ImGui host, frame capture, bitmap atlases, the shaders |
| `fx` | the particle engine, its definitions and component, authored particle systems |
| `audio` | the audio backend |
| `platform` | everything operating-system specific: the window and headless window, the sokol implementation, the editor platform shim, GPU readback, the `macosx` / `posix` / `windows` folders |
| `editor` | editor UI support that is not about this game: fonts, log viewer, preferences |

## game

| Folder | Holds |
|---|---|
| `app` | `revmain`, the game umbrella `revenant.h` and `revutils`, settings, run-time modes, test modes, session and flow, options, cheats, console |
| `object` | `TObjectInstance` and the class hierarchy (characters, player, containers, items, helpers, tiles), object stats, the retail stream codec (`legacystream`), lights, inventory |
| `imagery` | `TObjectImagery`, sprite imagery, the I3D imagery and animator, mesh extraction and glTF export |
| `world` | sectors and the sector store, the map pane and map renderer, game maps and the map manager, areas and modules, lighting (DLS, classic), terrain templates |
| `effects` | the effect classes and their forensics data (`*core.h`, `*profile.h`, `*matrix.h`), missiles and strips; `legacy/` holds the hand-ported shims the prefab system replaces |
| `spells` | spells and the spellbook |
| `ai` | boss AI, combat tracing |
| `script` | the script engine and values, commands, rules, dialog |
| `save` | save files and save games |
| `ui` | screens, panes, bars, the HUD, cursors, input queue, control mapping, UI tests, the imagery atlas feeder |
| `editor` | the map editor: commands, serialization, undo, icons |
| `ab` | the retail A/B harnesses, VFX review and the VFX test harness |
| `playtest` | playtest sessions, recording, observation, the player bot |
| `shadow` | the Shadow Smoke in-engine playthrough |
| `audio` | the game's music and sound manager |

## Moving a file

Put it where its dependencies allow: a file that includes game headers is
game code until those dependencies are cut. Update the includes to the new
spelled-out path (the tools' `../src/...` includes too), then run
`tools/test_layering.py` and a build. CMake globs `src/**`, so no list to
edit; test targets that compile individual files name them by path.

## The bar, and what still falls short of it

The engine should be able to run and work without the game, within reason.
It need not actually run standalone, but every engine object is declared
and defined by the engine module that owns it (`Display`, `ChunkCache`,
`ResourcePath`, `SystemFont`, the surface locking flags, the fatal-error and
status services, `tickcount`, the critical-section stubs), and the game only
sets them. Two engine services are still implemented by the game until the
VFS moves into the engine: `rev_fopen` and `rev_engine_asset`, declared in
`engine/core/file.h`, defined in `game/app/revutils.cpp`.

Engine by nature, parked in `game/` because they include game code:

| File | What it is | What ties it to the game |
|---|---|---|
| `game/imagery/3dimage` | the I3D mesh loader and skeletal animator | object.h, mappane.h, the effect profile headers; the right split is an `engine/assets/i3d` loader with the object imagery and retail profiles staying here |
| `game/imagery/imagery`, `animimage` | the imagery asset base and 2D sprite imagery | every draw entry takes a `TObjectInstance*` |
| `game/imagery/d3dport`, `meshextract`, `i3dgltf`, `i3danimpose` | D3D math shims, mesh extraction, glTF export, pose sampling | only through 3dimage |
| `game/world/3dscene` | the 3D scene: lights and camera | mappane and 3dimage |
| `game/world/dls`, `classiclighting`, `colortable` | the dynamic lighting system and its tables | dls reaches into the map pane and objects |
| `game/ui/inputqueue`, `cursor` | input events and the cursor | screen.h |
| `game/app/timer` | the timer | screen.h |
| `game/audio/sound` | the sound and music manager | game globals |

These move as the render and light components land (docs/OBJECT_MODEL_PLAN.md).
