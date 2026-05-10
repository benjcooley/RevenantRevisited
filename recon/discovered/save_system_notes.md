# Save / Sector Storage System (retail)

Source of truth for retail behavior is the Ghidra decompile under `recon/`,
cross-checked against the on-disk layout shipped with GoG retail
(`data/Save/Single/New Game1/`) and the original Cinematix install tree
(`data/`). The current port lives under `src/`. This doc captures what
retail did so we can restore it; we are *not* redesigning anything here.

## On-disk layout (retail)

The retail install is rooted at the directory the `Revenant.exe` lives in
(call it `<run>`). On the user's GoG copy this is `data/`:

```
<run>/                                # = RunPath, also default SavePath
├── Revenant.exe
├── revenant.ini                      # global config (paths + key bindings + lighting + ...)
├── ss.bmp                            # last screenshot for in-progress game (top-level)
├── game.sav                          # in-progress game.sav (top-level, NOT in Save/)
├── Curmap/                           # ACTIVE working sector dir (CURMAPDIR = "curmap")
│   └── <level>_<sx>_<sy>.DAT         # mutated sector snapshots, written hot
├── Modules/                          # base maps live inside per-module .rvm zips
│   ├── Ahkuilon.rvm
│   ├── Demo/...
│   └── ...
├── Imagery/, Resources/, ...         # asset trees (read-only on CD-ROM installs)
├── resources.rvr, imagery.rvi        # base packs mounted via VFS
└── Save/                             # named save slots only
    ├── Single/<savename>/            # ← single-player slots
    │   ├── game.sav                  # snapshot of player + automap + slot data
    │   ├── ss.bmp                    # title-screen thumbnail
    │   └── CurMap/<level>_<sx>_<sy>.DAT   # frozen copy of run/Curmap at save time
    ├── Multi/<savename>/             # multiplayer slots, same shape
    └── Chars/<charname>/             # character-only saves (multiplayer character carry)
```

Key invariant: `Curmap/` (top-level, lowercase per `CURMAPDIR`) is the
**active working set**. Every `TSector::Save()` writes there. `Save/Single/<n>/CurMap/`
(capital C) is a frozen copy made by `SaveGame` for that named slot.

`map/` (BASEMAPDIR) is the *pristine* base map used to repopulate `Curmap/` on
new game; in the GoG tree the base sectors actually live inside the per-module
`.rvm` zips, not in a loose `map/` directory, but the engine still tries that
loose path first (see `TSector::Load` fallback chain).

The user's GoG copy has the expected layout: `data/Curmap/*.DAT` and
`data/Save/Single/New Game1/{game.sav, ss.bmp, CurMap/*.DAT}` — confirmed by
`ls`, 283 sector files in that slot's CurMap.

## Path globals

| Global | Set by | Retail value | Purpose |
|---|---|---|---|
| `RunPath` | `GetProgramPaths()` from `GetModuleFileName(hInstance)` | `<install dir>\` (lowercased, trailing `\`) | read-only install root (CD-ROM safe) |
| `SavePath` | `GetProgramPaths()` after a `test.fil` write probe; falls back to `c:\Revenant\` (then `d:\…`, `e:\…`) | `<install dir>\` if writable, else `c:\Revenant\` | writable root for INI, saves, curmap |
| `ClassDefPath` | `GetINISettings()` reads `[Paths]ClassDefPath` from `Revenant.ini`, normalizes trailing `\` | `.\` (resolved through `makepath`) | `class.def`, `area.def`, `exit.def`, etc. |
| `ExileRCPath` | INI `[Paths]ExileRCPath` | `.\` | editor resource compiler workdir |
| `ResourcePath` | INI `[Paths]ResourcePath` | `.\` | Imagery/ root for editor |
| `BaseMapPath` | INI `[Paths]BaseMapPath` | `.\` | prefix for `map\<file>.DAT` reads |
| `CurMapPath` | INI `[Paths]CurMapPath` | `.\` | prefix for `curmap\<file>.DAT` reads/writes |

`makepath()` interprets `name` as relative-to-SavePath unless it starts with
`\`, `<drive>:`, or `..\` — the leading `.` (or `.\`) is stripped and
`SavePath` is prepended. So `CurMapPath = "."` means "rooted at SavePath",
not literal "./". The Win32 INI text comes back as `"."`, the `if … != '\\'`
appendage at `revmain.cpp:1571-1583` makes it `".\"`, and `makepath` strips
the leading `.` again.

`CURMAPDIR` = `"curmap"`, `BASEMAPDIR` = `"map"` (`src/revdefs.h:14-15`).

## Lifecycle

- **Boot (WinMain → engine init)**
  1. `GetProgramPaths(cmdline, RunPath, SavePath)` — derive both paths from
     `GetModuleFileName`. If RunPath is non-writable (CD), pick a writable
     `SavePath` under `c:\Revenant\` and `mkdir` it. If RunPath *is* writable,
     `SavePath = RunPath`. Both end with a trailing `\` and are lowercased.
  2. `INISetPath(RunPath)` (the retired duplicate at `revmain.cpp:662-681`
     shows the contract): if `SavePath ≠ RunPath` and there is no
     `Revenant.ini` in `SavePath`, *copy* the one from `RunPath` first.
     Subsequent `GetPrivateProfile*`/`WritePrivateProfile*` calls go against
     the SavePath copy.
  3. `GetINISettings()` populates `ClassDefPath`/`CurMapPath`/`BaseMapPath`,
     each terminated with a single `\`.
  4. VFS mounts: `resources.rvr`, `imagery.rvi`, then per-module `.rvm`
     when the active module changes (see `revutils.cpp` ZIP-VFS comment).

- **New game** (entry point `TPlayScreen::NewGame`, retail flow visible in
  `cls_0x45f7c0.cpp` around line 1700+, `s_newgame_005d9ca0` /
  `s__newgame_sav_005d9d1c`)
  1. `MapPane.LoadCurMap(nullptr)` → `ClearCurMap()` deletes
     `<SavePath>curmap\*.DAT`.
  2. `MapPane.LoadCurMap(<module's base map dir>)` → copies
     `<basemap>\*.DAT` into `<SavePath>curmap\`. (For module-packed maps
     the source is the mounted `.rvm`.)
  3. `LoadGame(this, "newgame", flag=2)` — flag bit-1 means "no clear /
     no reload", just hydrate the player from `<module>\newgame.sav`.
     The retail decompile at `cls_misc_LoadGame_48df70.cpp:78-107` builds
     the path as `<module-dir>\newgame.sav`.
  4. `PlayScreen` activates, scripts re-init via
     `TScriptManager::ReloadStates`.

- **Mid-session** (active sectors in `<SavePath>curmap\`)
  - When a sector is unloaded / paged out, `TSector::Save()` writes
    `<CurMapPath><CURMAPDIR>\<level>_<sx>_<sy>.DAT`. With
    `CurMapPath = ".\"`, this resolves through `makepath` (or a literal
    concat — see bug #1) to `<SavePath>curmap\<file>.DAT`.
  - `TSector::Load()` looks first under `<CurMapPath>curmap\…`, then falls
    back to `<BaseMapPath>map\…`.
  - `Player`, automap state, scripts, game time — kept in RAM; only sectors
    hit disk between SaveGame calls.

- **Save game** — `WriteGame(name)` (recon: `cls_0x45f7c0.cpp::FUN at 1370+`)
  1. Compute `<SavePath>Save\Single\<name>\` (or `Multi\<name>\`,
     `Chars\<name>\` depending on context — slot list selects which).
     `CreateDirectoryA` it (idempotent).
  2. `CreateDirectoryA` `<…\<name>>\CurMap\`.
  3. `SetCurrentDirectoryA(<slot dir>)`, `DeleteFileA("game.sav")`,
     `DeleteFileA("ss.bmp")` (clear stale copies).
  4. `SetCurrentDirectoryA(<slot dir>\CurMap)`, `FindFirstFile *.DAT` +
     `DeleteFileA` for every match (clear stale slot CurMap).
  5. `MapPane.SaveCurMap(<slot dir>)` — copies `<SavePath>curmap\*.DAT`
     into `<slot dir>\CurMap\`. (`SaveCurMap(nullptr)` writes to
     `<BaseMapPath>map\` instead — that's the editor "publish to base"
     path, not a normal user save.)
  6. `TSaveGame::WriteGame("<slot dir>\game.sav")` writes:
     - automap data (`AutoMap::WriteAutoMapData`)
     - 32-int spare-data block (DATA_SLOTS): `[0]=gametime`, `[1]=pane`,
       `[7]=version` (= MAP_VERSION = 15)
     - one streamed `TPlayer` object (which recursively writes inventory)
  7. Also writes `ss.bmp` (the last in-game framebuffer thumbnail —
     produced elsewhere; we haven't traced the writer yet).

- **Load game** — `LoadGame(this, name, flags)`
  (recon: `cls_misc_LoadGame_48df70.cpp` and the duplicated body in
  `cls_0x45f7c0.cpp` around 2227+)
  1. Resolve `<slot dir>\game.sav` from name, copy `<slot dir>` to a
     working buffer at `DAT_00667eb8` (the engine's "current save dir"
     global), then strip the basename and append `\CurMap` so subsequent
     code knows the slot's CurMap origin.
  2. `MapPane.ClearCurMap()` (delete `<SavePath>curmap\*.DAT`).
  3. `MapPane.LoadCurMap(<slot dir>\CurMap)` — copy slot CurMap back into
     active `<SavePath>curmap\`.
  4. `PlayScreen.Close()`, `ScriptManager.ReloadStates()`,
     `PlayerManager.Clear()`.
  5. Open `game.sav`, read the file size, then:
     - read the 128-byte automap header (`flen` 0x80 → 4 ints + walkmap)
     - if `iStack_170` (automap-walkmap-present flag) set, read 0x200 more
     - read the rest into a heap buffer, sized `total - 0x80 [- 0x200]`.
     - parse the 32-int slot data: `gametime, pane, …, version` at slot 7.
     - decode `TPlayer` (or array of players for multiplayer) via
       `CreateObjectFromStream` and insert via `PlayerManager.AddPlayer`.
  6. Reactivate pane, resume time, redraw.

## Functions by responsibility

### Setup / paths
| Retail | Address | Recon file | Our equivalent |
|---|---|---|---|
| `WinMain` | `004865a0` | (not extracted under that address; see `recon/docs/FREE_FUNCTION_IDENTIFICATIONS.md`) | `src/revmain.cpp::main` (sokol entry) |
| `GetProgramPaths` | inside WinMain, calls `GetModuleFileName`/`mkdir`/`unlink` | preserved verbatim under `#if 0` at `src/revmain.cpp:1304-1347` | `src/revmain.cpp::GetProgramPaths` (stub: both = "./") |
| `INISetPath` | early in WinMain | retired duplicate at `src/revmain.cpp:652-681` | `src/revutils.cpp::INISetPath` (live) |
| `GetINISettings` | called from WinMain | live in `src/revmain.cpp:1558-1593` | same |

### Sector save / load
| Retail | Address | Our equivalent | Status |
|---|---|---|---|
| `TSector::Load` | sector path resolution + stream parse | `src/sector.cpp:120-217` | live, uses `rev_fopen` (good) |
| `TSector::Save` | output stream + `fopen` of `<CurMapPath>curmap\…` | `src/sector.cpp:219-264` | **buggy** (see #1) |

### CurMap copy / clear
| Retail | Address | Recon | Our equivalent |
|---|---|---|---|
| `TMapPane::LoadCurMap(from)` | `~0x44e4xx` (search `s_curmap_*` in `cls_0x5a5320_TMapPane.cpp` line 583, 611) | `recon/classes/cls_0x5a5320_TMapPane.cpp:540-625` | `src/mappane.cpp:520-534` |
| `TMapPane::SaveCurMap(to)` | nearby | same recon file | `src/mappane.cpp:538-561` |
| `TMapPane::ClearCurMap` | `~0x44e460` | `cls_0x5a5320_TMapPane.cpp:632-698` (decompile of `meth_0x44e460`) | `src/mappane.cpp:564-582` |

### High-level Save/Load
| Retail | Address | Recon | Our equivalent |
|---|---|---|---|
| `TSaveGame::WriteGame` | `~0x48d?` | (not split into a discovered file yet — body lives inside the larger 0x45f7c0 / 0x48d region, see `cls_0x45f7c0.cpp:1370-2220`) | `src/savegame.cpp::WriteGame:27-93` |
| `TSaveGame::ReadGame` | `~0x48dxxx` | same | `src/savegame.cpp::ReadGame:95-214` |
| `LoadGame(savename, flags)` | `0x48df70` | `recon/discovered/cls_misc_LoadGame_48df70.cpp` | **MISSING** — orchestration is partially baked into `ReadGame`, but the curmap-copy + module/newgame.sav branch is absent |
| `SaveGame(savename)` | `~0x48d` (clear-then-copy block at `cls_0x45f7c0.cpp:1850-1942`) | same | **MISSING** — directly above `WriteGame` retail also did MkDir + clear + curmap copy via `MapPane.SaveCurMap`; we only call `MapPane.SaveCurMap` from… nowhere |

### Game-state side files
| Retail | Address | Recon |
|---|---|---|
| `TGameState::Save` | `0x4974d0` | `recon/discovered/cls_TGameState_Save_4974d0.cpp` |
| `TGameState::Load` / `LoadStream` | `0x495cf0` / `0x496110` | discovered files |
| `TScriptManager::Save` | `0x496690` | `recon/discovered/cls_TScriptManager_Save_496690.cpp` |
| `TScriptManager::Load` | `0x496490` | `recon/discovered/cls_TScriptManager_Load_496490.cpp` |
| `TScriptManager::ReloadStates` | `0x4975c0` | `recon/discovered/cls_TScriptManager_ReloadStates_4975c0.cpp` |

These three (`TGameState`, `TScriptManager`, automap walkmap blob) are the
"game state" written by SaveGame *in addition to* the player object.
Currently only `AutoMap::WriteAutoMapData` is invoked from our
`savegame.cpp` — `TGameState` and `TScriptManager` save are **not wired**.
TODO retail: confirm where in `WriteGame` retail emits TGameState /
TScriptManager streams (the 0x45f7c0 body shows multiple
`FUN_004975c0`/`FUN_00496690` calls; need to trace whether they're inside
`game.sav` or are separate `state.sav`/`script.sav` files in the slot dir).

### INI helpers
| Retail | Our equivalent | Status |
|---|---|---|
| `INIGetInt` / `INIGetText` / `INISetInt` / `INISetText` | `src/revutils.cpp:515-…` (live) and `src/revmain.cpp:683-…` (`#if 0`) | OK |
| `GetPrivateProfile*` | replaced by `mINI` (or whichever lib `INIGetText` wraps now) | OK |

## Current port status

| Retail behavior | Our state | Notes |
|---|---|---|
| `RunPath`/`SavePath` resolution | **STUB**: both = `"./"` | `revmain.cpp:1349-1353`. CD/HD-split logic preserved under `#if 0`. |
| Trailing-`\` path normalization (Win32) | **PORTED but Win32-flavored**: still appends `\`, `makepath` still uses `\` literals, callers still build `curmap\` strings | leaks backslashes into `fopen` on macOS — that's why `curmap\1_6_6.DAT` lands as a literal filename in the project root. |
| `makepath` | **DUPLICATED**: `src/revutils.cpp:838` and `src/revmain.cpp:1047` (both live, identical bodies) | ODR; linker picks one silently. |
| `rev_normalize_sep` | applied in `rev_fopen` (read), **NOT** applied in `makepath` / `popen` / `TSector::Save fopen` | write path keeps backslashes. |
| `TSector::Load` | **PORTED**, uses `rev_fopen` (works) | OK — backslash gets normalized for read. |
| `TSector::Save` | **BUGGY**: `strcpy` typo at `src/sector.cpp:253-254` clobbers the prefix | first `strcpy(mappath, CurMapPath)` is dead; second `strcpy(mappath, CURMAPDIR "\\")` overwrites it. So output always lands at `curmap\<file>.DAT` relative to CWD. |
| `TMapPane::{Load,Save,Clear}CurMap` | **PORTED** (`src/mappane.cpp:520-582`), but uses `makepath` which doesn't separator-normalize | `*.DAT` glob + `\` survive in path arg to `copyfiles`/`deletefiles`. |
| `TSaveGame::WriteGame` | **PORTED minimally** (`src/savegame.cpp:27-81`) — writes only automap + spare-data + Player; **does NOT** create slot dir, does NOT clear stale slot CurMap, does NOT call `MapPane.SaveCurMap`. | "save" today writes only `game01.sav` to CWD. |
| `TSaveGame::ReadGame` | **PORTED minimally** (`src/savegame.cpp:95-214`) — does call `ClearCurMap`/`ReloadStates`/`PlayerManager.Clear`, but does **not** `LoadCurMap(<slot>\CurMap)` from the slot. | "load" today resets curmap to empty, never restores slot's frozen sectors. |
| `LoadGame(savename, flags)` orchestration | **MISSING** — no top-level entry that mirrors retail's curmap-copy + newgame branch. | UI-driven flows go straight to `TSaveGame::ReadGame`. |
| `SaveGame(savename)` orchestration | **MISSING** — no caller of `MapPane::SaveCurMap` outside the editor "save base map" code path. | |
| `GameState`/`ScriptManager` save-load wiring | **MISSING** | not called from `WriteGame`/`ReadGame`. |

## Known bugs we need to fix to restore retail behavior

(Just the bugs that block retail behavior. Not improvements.)

1. **`TSector::Save` typo** — `src/sector.cpp:253-254`. The first `strcpy`
   is dead; the second writes only `"curmap\\"` over `mappath`. Should be
   `strcpy(mappath, CurMapPath); strcat(mappath, CURMAPDIR "\\"); strcat(mappath, filename);`
   (matches `TSector::Load` at line 123-125). With this fixed and a real
   SavePath, output goes to `<SavePath>curmap\<file>.DAT`.

2. **`GetProgramPaths` is stubbed** — `src/revmain.cpp:1349-1353` sets
   both `RunPath` and `SavePath` to `"./"`. Result: every `makepath`
   prepends `"./"` and then concatenates `curmap\…` verbatim, so writes
   land at `./curmap\<file>.DAT` (literal-backslash filename in project
   root). Need a portable `GetProgramPaths` that:
   - sets `RunPath` to the executable's directory with trailing `/`,
   - sets `SavePath` to a writable user-data dir (`$XDG_DATA_HOME/Revenant/`
     on Linux, `~/Library/Application Support/Revenant/` on macOS),
     or to `RunPath` when that's writable (matches retail HD-install
     behavior of "same as run dir").
   This *also* puts `Revenant.ini` and `imgui.ini` in the right place.

3. **Backslashes leak into `fopen`/`makepath` write path** — `makepath`
   at both `src/revutils.cpp:838` and `src/revmain.cpp:1047` doesn't call
   `rev_normalize_sep` on its output, and neither does `TSector::Save`.
   Reads work because `rev_fopen` normalizes; writes don't, because
   `TSector::Save` calls `fopen` directly. Either route every write
   through `rev_fopen`-equivalent (`rev_fopen("…", "wb")`) or normalize
   inside `makepath`. Also: kill the duplicate `makepath` (ODR); keep
   `revutils.cpp`'s, delete the one in `revmain.cpp`.

4. **`SaveGame`/`LoadGame` orchestration not ported** — retail's
   high-level `SaveGame(name)` did MkDir + clear-stale + `MapPane.SaveCurMap`
   + `TSaveGame::WriteGame`. Today our `WriteGame` only writes
   `game.sav` and skips all of the directory and curmap-copy work, so
   "saved game" is missing the sector snapshot entirely. `LoadGame`
   symmetric: doesn't repopulate `curmap/` from the slot's `CurMap/`.

5. **Game-state and script-state not in `game.sav`** — retail also calls
   `TGameState::Save` and `TScriptManager::Save` during `SaveGame`. We
   don't. Side files are missing on save and not consumed on load. (TODO
   retail: confirm whether these are appended to `game.sav` or written
   as separate `state.sav`/`script.sav` in the slot dir.)

## Open questions

- TODO retail: where exactly retail emits `TGameState` and
  `TScriptManager` streams during SaveGame — appended to `game.sav` after
  the player, or sibling files? Multiple `FUN_004975c0` / `FUN_00496690`
  calls visible in `cls_0x45f7c0.cpp` but the surrounding control flow
  needs more careful tracing.
- TODO retail: confirm where `ss.bmp` is written. It's deleted at
  SaveGame entry but the writer hasn't been located in recon.
- TODO retail: confirm Multi/Chars slot directory selection logic — the
  `flags & 1` branch at `cls_misc_LoadGame_48df70.cpp:35-77` differs from
  the named-slot branch but the call sites haven't been mapped.
- TODO retail: trace `BASEMAPDIR` writer — `SaveCurMap(nullptr)` writes
  base maps; presumably editor-only ("publish module"). Need to confirm
  this never fires from in-game SaveGame.
- The `_DAT_00667eb8` global ("current save slot dir") is referenced in
  multiple places. Worth tracking down what queries it after load (so the
  "next save defaults to the loaded slot" UX behaves like retail).
