# Save / load — forensics

What retail Revenant writes to a save, how a game (new or saved) is
loaded, and how the sector working set (`curmap`) moves between the live
game and save slots. Input to [../ARCHITECTURE.md](../ARCHITECTURE.md)
§3. Companion to [GAME_FLOW.md](GAME_FLOW.md) (start modes, who calls
`LoadGame`).

Retail fidelity: **retail-confirmed** (Ghidra decomp + byte-level decode
of `Modules/Ahkuilon.rvm:newgame.sav`) unless marked *probable* or
*unidentified*.

This document supersedes the wrapper description in
[../T5_FORENSIC.md](../T5_FORENSIC.md) §1.1 and the "Load game" /
"Save game" lifecycle in
[../../../recon/discovered/save_system_notes.md](../../../recon/discovered/save_system_notes.md);
see §9 for what those got wrong. T5's findings about the **object
stream** (`TObjectInstance::LoadObject/SaveObject`, inventory slots,
post-load sector attach) remain correct and are built on, not replaced.

## 1. Requirements (manual)

- Save and load from named slots in the in-game menu; Load Game from the
  title screen; each slot shows a thumbnail.
- New Game starts the module's opening (Locke in the Keep's
  resurrection chamber).
- Death screen offers Load.

## 2. On-disk layout

```
<SavePath>/
├── curmap/<level>_<sx>_<sy>.DAT     working set: every sector modified this game
├── ss.bmp                           last thumbnail (copied into a slot on save) — probable
└── <saves root>/                    INI path at DAT_0065da00 — probable "Save"
    ├── Single/<slot name>/          single player
    │   ├── game.sav                 §3
    │   ├── ss.bmp                   thumbnail
    │   └── CurMap/*.DAT             frozen copy of curmap at save time
    └── Multi/<slot name>/           same shape (multiplayer)
<module>/newgame.sav                 the new-game start state, same format as game.sav
```

The base (pristine) sectors live in the module (`Ahkuilon.rvm:Map/*.DAT`).
A sector is read from `curmap` if present, otherwise from the module map;
sectors are written to `curmap` when they unload. `curmap` therefore
*is* the modified world, and new game = empty `curmap`.

## 3. `game.sav` / `newgame.sav` format

Little-endian. Writer `SaveGame` `0x0048d720` (`LAB_0048dbde`), reader
`LoadGame` `0x0048df70`.

### 3.1 Header (0x80 bytes, raw `fwrite`)

| Offset | Type | Field | Written as |
|---|---|---|---|
| +0x00 | int32 | game time | `PlayScreen.GameTime()` (`0x0047e940`) |
| +0x04..+0x13 | — | zero | |
| +0x14 | int32 | multiplayer flag | `DAT_0066829c` |
| +0x18 | int32 | player-list format | always 2 (0 = legacy single player, ≥2 = counted list + pair table) |
| +0x1c | int32 | stream version | always 15 (`MAP_VERSION`); passed to `LoadObject` |
| +0x20 | char[32] | module dirname | active module's dirname, 31 chars + NUL |
| +0x40..+0x7f | — | zero | |

If +0x14 ≠ 0 and +0x18 ≥ 1, a 0x200-byte multiplayer block follows
(copied from `DAT_00676764`, read into `DAT_00668300`). Single-player
saves never have it.

### 3.2 Body (one `TOutputStream`, written in one `fwrite`)

1. **Game states** (version > 9) — `TGameState::SaveStream` `0x004974d0`
   / `LoadStream` `0x00496110`:
   `int32 count`, then per state a stream string (`uint8 len` + bytes,
   no NUL) whose bytes are the name OR'd with 0x80 (load XORs 0x80),
   then `int32 value`. Load updates a state by name (case-insensitive)
   or appends it.
2. **Merchant unique-item table** (version ≥ 11, present when +0x18 > 1):
   `int32 count`, then `count × {int32 objclass, int32 objtype}`. See §6.
3. **Players**:
   - +0x18 < 1 (legacy): one object; version > 12 skips 0x50 bytes
     before it.
   - otherwise: `int32 count`, then `count` objects.
   Each object is the standard object stream (`SaveObject` `0x00472110`
   / `LoadObject` `0x00471ce0`, as ported by T5), player then inventory.

Before writing each player, `SaveGame` clears object flag `0x04000000`
on the player and on every inventory item (*unidentified* retail-only
flag; the port's `OF_*` set stops at bit 25).

### 3.3 `newgame.sav` decoded (Ahkuilon, 2,396 bytes)

Header: time 24791, MP 0, format 2, version 15, module `ahkuilon`.
82 game states (e.g. `MISTSTATE=1`, `DEMOSTATE=4`) ending at 0x578;
pair count 0; player count 1; player object at 0x580 (objversion 14,
objclass 11 = player).

## 4. `LoadGame(name, flags)` — `0x0048df70`

`this` is the save manager (global; the slot list, a loading flag and
the pair table live on it).

| Flag | Meaning |
|---|---|
| 1 | New game: open `<modules path>\<active module dirname>\newgame.sav`; no slot `CurMap`. |
| 2 | Header only: read header (+ MP block), close, return. Used by `0x0048e5e0` (slot info for the load screen). |

Sequence (flags & 2 clear):

1. Resolve the file. Named load: look the name up in the slot list
   (case-insensitive; null name → "Default Save"), fail if absent;
   remember `<slot dir>\CurMap` in `DAT_00667eb8` (the *current save
   dir*). New game: `DAT_00667eb8 = ""`.
2. `loading = 1`.
3. Reset the world, in this order:

| Call | Object | Role | Port today |
|---|---|---|---|
| `0x0044e460` | MapPane | **ClearCurMap**: unload all sectors (they save to `curmap`), delete `curmap\*.*` | `TMapPane::ClearCurMap` (operates on the legacy empty MapPane arrays) |
| `0x0044e050(DAT_00667eb8)` | MapPane | **LoadCurMap** (named load only): clear, then copy `<slot>\CurMap\*` into `curmap` | `TMapPane::LoadCurMap` (same problem) |
| `0x0047ece0` | PlayScreen | if a fade is active (+0x6ac), mark it finished (+0x6b4) | — |
| `_DAT_00667ca8 = 1` | — | *unidentified* flag | — |
| `0x005360f0` | dialog pane `0x00667cc8` | end any conversation: free choice list, send `Finish` to the speaker's script, close the pane | — (dialog not ported) |
| `0x00532f40` | buy/sell pane `0x0065a3b8` | clear its item list | — |
| `0x00496e20` | ScriptManager | reset every script instance: End if running, rewind, clear trigger/wait state | — |
| `0x004975c0` | ScriptManager | **ReloadStates**: reload `state.def` defaults | `TScriptManager::ReloadStates` |
| `0x0041c600` | AreaManager | leave every active area (stop its sounds, ambient/music, exit script) | — |
| `0x0051eda0` | PlayerManager | **Clear** | `TPlayerManager::Clear` |
| `0x0047c580(1)` | PlayScreen | **SetControl(on)** | — |

4. Read header, optional MP block, body (§3). Game states load only if
   version > 9; the pair table only if version ≥ 11 and format > 1
   (otherwise emptied); legacy single-player saves take the module name
   from the active module.
5. Players: `LoadObject(stream, version, 1)` → `AddPlayer` →
   `SetPlayerState((state & ~2) | 1)` → `SetMainPlayer` if not
   multiplayer.
6. Fail if no players. **SetCurModule** by the header's module name
   (`0x00460d60` find, `0x004609f0` set; a no-op when already active).
7. `SetGameTime(header time)` (`0x0047e950`), `_DAT_0065cb40 = 1`
   (*unidentified*), `SetControl(on)`, `loading = 0`.

`LoadGame` does **not** load sectors or place the player in a sector.
`TPlayScreen::Initialize` does that afterwards: it loads the sectors
around the main player's position (`0x004997d0`, with a progress
callback driving the loading bar), then closes the bar and turns control
on. See [GAME_FLOW.md](GAME_FLOW.md) §2.3.

**Third argument of `LoadObject`.** Retail passes it through to
`NewObject` and `LoadInventory`. The "drop non-map objects" gate is a
different value: the global `DAT_0065a254 & 1`, which `LoadGame` sets to
0. The port's `LoadObject(…, bool ismap)` merges the two, so the player
must be loaded with `ismap = false`, or it is discarded as a non-map
object.

## 5. `SaveGame(name, flags)` — `0x0048d720`

1. Return 0 if there is no main player.
2. Editor and name = `newgame` and the module is unpacked: target is the
   module's own `newgame.sav` (skip to 7). This is how designers
   authored the start state.
3. Null name → "Default Save". Build `<saves root>\Single|Multi\<name>`,
   creating each directory, and `<slot>\CurMap`.
4. Delete `<slot>\game.sav`, `<slot>\ss.bmp`, `<slot>\CurMap\*`.
5. **SaveCurMap**(`<slot>\CurMap`) — `0x0044e250`: save every loaded
   sector into `curmap` (`0x00499e60`); if the game was loaded from a
   different slot, first copy that slot's `CurMap` into the new one;
   then copy `curmap\*` over it.
6. Copy the current thumbnail `ss.bmp` into the slot (*probable*: source
   path `DAT_005d9cfc` not decoded).
7. `DAT_0065a250 = flags` for the duration (*unidentified* use), write
   header + MP block + body (§3), clear `DAT_0065a250`.

## 6. Merchant unique-item table

`AddPair` `0x0048e670` / `HasPair` `0x0048e630` on the save manager,
called only from the buy/sell pane (`0x0052ff40`, `0x00530670`,
`0x00530af0`). On a purchase, an item whose class stat `SaleType` is 1
is recorded as `(objclass, objtype)`; stock listings skip recorded
pairs. So: unique merchant items, once bought, never restock. The table
persists only through the save file.

## 7. Slot list and helpers

| Address | Role |
|---|---|
| `0x0048d260` | Rebuild the slot list: scan `<saves root>\Single` (or `Multi`) for subdirectories with `game.sav`; per slot store `{name, path, module dirname from header +0x20}` |
| `0x0048d6d0` | Find a slot by name → index, or -1 |
| `0x0048e5b0` | `LoadGame(slot[index].name, flags)` |
| `0x0048e5e0` | Header-only load of slot `index`; returns the MP block or null |
| `0x0048e610` | **LoadNewGame** = `LoadGame("newgame", 1)` |
| `0x0048df40` | `SaveGame(slot[index].name)` |

## 8. Persistence outside `game.sav`

- **Automap**: retail stores per-sector automap bitmaps as files
  (`%sautomaps\%d_%d_%d.bmp`, `lev%dcomposite.bmp`, `0x0045f1e0`)
  alongside the sector working set, not in `game.sav`. Not yet traced
  in full.
- **HUD state**: not saved by retail.

## 9. Corrections to earlier notes

- The 0x80-byte block is the save header above, not "automap data".
  Retail `game.sav` has no automap blob and no 32-int "data slots"; that
  is the pre-release (1998 source) wrapper the port still writes.
- The optional 0x200 block is multiplayer data, not an automap walkmap.
- Game states, the merchant table and the players are all inside
  `game.sav`; there are no separate state/script files.
- `LoadGame` flag 1 = new game, flag 2 = header only (not "no clear").
- New game: `ClearCurMap` then `LoadNewGame`; there is no copy of the
  base map into `curmap` (sectors fall back to the module map on read).
- `ReloadStates` reloads `state.def`; `TScriptManager::Initialize` is
  not part of a load.

## 10. Port gaps found during this work

1. **Writes escape into the install.** `rev_fopen("…", "wb")` falls back
   through RunPath, the module dir and the data root when the SavePath
   target directory doesn't exist. `SavePath/curmap` is never created, so
   every sector save lands in the data root (the repo's copy of the
   retail install) as a file literally named `curmap\L_X_Y.DAT`: 197 in
   the main checkout's `data/` (Jun 7), and the same again in this
   worktree's `data/` from today's runs. A `.gitignore` rule
   (`/data/curmap\\*.DAT`) hid them.
2. **Reads pick those files back up.** The data-root fallback joins the
   unnormalized path, so `curmap\0_0_14.DAT` resolves to the stray
   files. Each run re-saved them with duplicated objects: level 0 loaded
   47,205 → 48,542 → 49,879 objects across consecutive runs, against
   35,549 in the pristine base map. A retail `data/Curmap/` (someone's
   retail playthrough) would be read the same way.
   *Fixed in 2a:* `SectorStore` (working set under SavePath, base map
   through the resource layer), `rev_fopen` writes confined to
   SavePath, the `.gitignore` rule removed.
3. **The pre-release wrapper overwrites retail fields.** `TSaveGame`
   writes AutoMap data + 32 slots + one player. UI commit `ab4d8f5`
   stores HUD state in slots 2–12 with a `0xABCD` sentinel. None of this
   matches the retail file; retail saves and `newgame.sav` can't be read.
4. **Stand-ins for `newgame.sav`.** `TPlayScreen::SpawnDefaultPlayer`
   (Misthaven fountain + hand-picked loadout) and the Level-10 floor in
   `TPlayer` exist only because `newgame.sav` wasn't loaded.
5. `TGameState::LoadStream/SaveStream` are stubs typed on `TParseStream`;
   they belong on the binary streams.
6. Player state flags (+0x36c, `SetPlayerState` `0x0051d680`) are not
   ported; in single player the load-time value only marks the player
   active.
