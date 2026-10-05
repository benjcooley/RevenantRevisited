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

Before writing each player, `SaveGame` clears object flag `VIRGIN`
(`0x04000000`, §11.2) on the player and on each item directly in its
inventory (the iterator is non-recursive), so a loaded player keeps its
own stats.

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
6. Copy the current thumbnail `.\ss.bmp` (current directory = the
   install) into the slot (`0x004814d0`, file copy). §11.7 covers how
   that file is written.
7. `DAT_0065a250 = flags` for the duration (the object-save flags of
   §11.1; both single-player call sites pass 0), write header + MP block
   + body (§3), clear `DAT_0065a250`.

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

Saves root: INI `[Paths] SaveGamePath`, default `.\Save`
(`GetINISettings` `0x00484500`).

### 7.1 Requests during play

Loads and saves requested in game are queued on the PlayScreen
(`+0x5e4` load, `+0x5e8` save, `+0x5ec` slot index, `+0x5f0` slot name)
and carried out at the start of its next frame (`0x0047bfab..0x0047c0b8`):
the load first (announced on the text bar, then `LoadGame`, then
SetControl on), then the save (`0x00458f00` on the MapPane, announced,
then `SaveGame`).

| Address | Role |
|---|---|
| `0x0047e770` | RequestLoad(index) |
| `0x0047e7a0` | RequestLoad(name) |
| `0x0047e810` | RequestSave(name) |
| `0x0047e850` | QuickSave: refresh slots, take the first unused `"Quick Save %d"` (format from string `quicksavefmt` if present), request a save |
| `0x00428540` | console `loadgame.<name>`: refresh, find, request load |
| `0x004285b0` | console `savegame.<name>`: request save |
| `0x00423760` | console `newgame`: RequestLoad(index 0) |

Retail has a "Quick Save" control and no quick load.

### 7.2 Object stats on load

`TObjectInstance::Load` (`0x00472430`) matches each saved stat to the
class's stats by unique id, masking the saved id with `0x7f7f7f7f`; when
the order differs it searches every class stat. When object flag
`VIRGIN` (`0x04000000`) is set, it then resets every stat except indices
3–5 (health, fatigue, mana) to the object type's defaults; this is why
`SaveGame` clears that flag on the players before writing them. See §11.2.

## 8. Persistence outside `game.sav`

- **Automap**: the per-player record of explored map is saved *inside*
  the player object (§11.5). The per-sector automap bitmaps
  (`%sautomaps\%d_%d_%d.bmp`, `lev%dcomposite.bmp`, `0x0045f1e0`) are
  a render cache next to the working set.
- **HUD state**: retail writes four HUD words into the player record
  (§11.4: sidebar open flag, upper and lower sidebar mode, one
  unidentified word) but never reads them back; loading leaves the HUD
  as it was.

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
7. **Stats lost on load.** The port's stat loop was the 1998 one: no id
   mask, and a slow path that compared against the current index only,
   so every stat after the first ordering mismatch was dropped. Locke
   loaded from `newgame.sav` as level 0 with 0 HP. *Fixed in 2c* to
   retail (§7.2). This affects every object loaded from a sector too.
   *VIRGIN reset ported in 2f* (§11.2).
8. **Player saved in the 1998 layout.** `TPlayer`/`TCharacter::Save`
   write objversion 4 (809 bytes for the new-game Locke); retail writes
   objversion 14 (976 bytes). The port reads both, so its saves
   round-trip, but retail can't read them and v14-only fields are lost
   on save. Header, game states and merchant table match retail byte for
   byte. *Fixed in 2f:* every class writes retail's layout (§11), the
   player at objversion 15.
9. **Script ownership.** Objects delete their scripts and
   `TScriptManager::Close` deleted the same instances (double free at
   shutdown); a load's script reset would have touched freed scripts.
   *Fixed in 2c:* the manager's list is non-owning and maintained by
   `TScript` itself.
10. **Construction read the current screen.** `TCharacter::ClearChar`
    read `CurrentScreen->FrameCount()`, so building the world before the
    PlayScreen ran crashed. *Fixed in 2c* (frame 0, which is what
    retail's load inside `TPlayScreen::Initialize` saw).
11. Not ported in `LoadGame`'s reset: finishing a PlayScreen fade
    (`0x0047ece0`), ending a conversation (`0x005360f0`), emptying the
    buy/sell pane (`0x00532f40`). In `SaveGame`: the editor path that
    rewrites the module's `newgame.sav`. *Thumbnail ported in 2f*
    (§11.7): `TSaveGame::CaptureThumbnail` reads back the next presented
    frame into `<SavePath>/ss.bmp` (retail `.\ss.bmp`); quick save calls
    it, the in-game menu and save dialog must call it when they open
    (retail `0x0047cb52`, `0x0047dc05`); a save with no thumbnail captured
    since the last one captures its own.
12. **Stat layout.** The port's CHARACTER and PLAYER object stats were
    in the 1998 order (level at index 6; damage resistances, NextExp,
    the modifiers, skill next-exp and caps appended from class.def;
    SpellDamageInc and EdgeBonus missing). Saves list stats in class
    order and class.def's per-type default lists are positional, so
    Locke's type defaults landed on the wrong stats, and SINC/EBNS were
    dropped on load. *Fixed in 2f:* `charstats.h` and the code-defined
    stats follow retail's registrations (`recon/scripts/object_stats.py`):
    90 player stats in retail order.

## 11. Object stream, per class

Everything an object writes into a sector file or a save. Decompiles of
every function named here are in
[`recon/discovered/save/`](../../../recon/discovered/save/). The class
behind each vtable was found from the vtables themselves
(`recon/scripts/object_vtables.py`: 124 object vtables share
`TObjectInstance`'s layout up to slot `0x170`) and from the builder that
constructs each one (`recon/scripts/object_builders.py`: a
`REGISTER_BUILDER` static initializer pushes the class.def builder name;
its `Build` override inlines the constructor that stores the vtable).

Vtable slots used here: `0x15c` ObjVersion, `0x160` Load, `0x164` Save,
`0x168` LoadInventory, `0x16c` SaveInventory, `0x170` linked inventory
(null for every class except the container family, which returns
`+0xd8`).

### 11.1 `SaveObject` `0x00472110` / `LoadObject` `0x00471ce0`

Header (stream version ≥ 14): `int16 objversion` (ObjVersion()),
`int16 objclass`, `uint32 type unique id`, `int16 blocksize` (body +
inventory), `int16 invblocksize` (inventory tail). An empty slot is a
lone `int16 -1`. The body follows, then, when the object has inventory,
`int32 count` and `count` objects (recursively).

Save flags (`DAT_0065a250`, set by the caller): bit 1 = writing a map
(a NONMAP object becomes the `-1` placeholder), bit 2 = omit
inventories (the sector hash), bit 0x10 = blank the player's private
strings (a multiplayer export; never set by a single-player save). Load
flags (`DAT_0065a254`): bit 1 = loading a map (NONMAP objects are
discarded), bit 2 = skip inventories. `TSector::Load` sets bit 1,
`LoadGame` clears it.

`LoadObject` differences from the 1998 source:

- The type is found by unique id through one global table
  (`0x00477f20`); when that type belongs to another class, the class is
  replaced and the base `TObjectInstance::Load` is used.
- The object is created with `def.flags = LOADING` (`0x08000000`); the
  constructor skips the script for LOADING objects and `Load` clears the
  flag (§11.2).
- `MAPSCROLL` (class 26) objects are never loaded: skipped by block size,
  or deleted after loading when the block size is unknown.
- A block size that doesn't match what `Load`/`LoadInventory` consumed
  logs `"Block size error for object %s"` (`0x004820b0`); the stream is
  resynced to the end of the block either way.

`SaveInventory` (`0x00472380`) writes the inventory array's high-water
item count (`+0x68`), then every non-null item. Retail's array keeps
holes when an item is removed from the middle (`0x0041cb40` only trims
trailing nulls), so after such a removal the count exceeds the items
written and a later load reads the following data as inventory: a latent
retail bug. `LoadInventory` (`0x00472310`) adds each item with the
array's `Add` (`0x0041c840`, first free slot) and sets its `invindex`
to the slot it got, so a loaded inventory is always packed.

### 11.2 `TObjectInstance::Load` `0x00472430` / `Save` `0x00472980`

| Field | Type | Present |
|---|---|---|
| name length | uint8 | always; 0 = the type's name (Save writes 0 unless the instance was renamed) |
| name | bytes, each with bit 7 set | |
| flags | uint32 | |
| pos x, y, z | int32 ×3 | |
| vel x, y, z | int32 ×3 | !IMMOBILE |
| state | uint16 | |
| level | uint16 | NONMAP |
| inventnum, invindex | int16 ×2 | |
| shadow | int32 | |
| rotatex, rotatey, rotatez | uint8 ×3 | |
| mapindex | int32 | |
| frame, framerate | int16 ×2 | ANIMATE |
| group | uint8 | |
| stat count | uint8 | the class's object-stat count |
| stats | {int32 value, uint32 unique id \| `0x80808080`} × count | |
| light | uint8 flags; int32 x, y, z; uint8 red, green, blue, intensity; int16 multiplier | LIGHT |

Save first sets ANIMATE|PULSE on a LIGHT object. Load, after the fields:

1. Flags: bits in `0x3ff1ffd7` come from the file; MOVING, AI, COMPLEX,
   NOTIFY, NONMAP, INVENTORY and CALLEDPREDEL (`0xc00e0028`) keep the
   constructor's value.
2. `moveangle = rotatez`; the stat array is sized to the class and
   filled by unique id (§7.2); VIRGIN resets stats other than 3–5 to the
   type defaults.
3. LIGHT → flags |= LIGHT|ANIMATE.
4. Class and type pointers are re-resolved; an existing script is
   deleted.
5. Stream version < 12: clear FOREGROUND and bits 26–31. Always clear
   LOADING; a lit tile also loses PULSE; INVENTORY → ANIMATE|PULSE.
6. Script: attached unless INVENTORY, or a tile whose name is its
   type's name (a renamed tile gets its script).

Retail's object flag names (the `OBJFLAGNAMES` table at `0x005d4770`,
which scripts use by name):

| Bit | Name | Port before this work |
|---|---|---|
| 9 | FOREGROUND | `OF_DRAWFLIP` |
| 26 | VIRGIN: untouched since the designer placed it; stats reload from the type | — |
| 27 | LOADING: set while `LoadObject` builds the object | — |
| 28 | INVULNERABLE | — |
| 29 | BACKGROUND | — |
| 30 | INVENTORY: an item, one that can live in an inventory | — |
| 31 | CALLEDPREDEL | — |

Constructor `0x0046e1f0`: NOTIFY on every object; INVENTORY|ANIMATE|
PULSE for the item classes (0–4, 6–8, 16–18, 21–23, 26); ANIMATE|PULSE
for effects (25); VIRGIN for characters and players (11, 12);
ANIMATE|PULSE when the imagery needs an animator; PULSE for every
non-tile. The script is attached unless INVENTORY, LOADING, or a tile
with its type's name. VIRGIN is cleared by `TPlayer` setup (`0x00518750`,
`0x00518b2c`), by `SaveGame` (§5) and by `TMoney::Load`.

Measured over the 558 sector files and the save of §12: every object has
NOTIFY; every item has INVENTORY; VIRGIN is set on 57 of 110 characters
and 119 of 11,008 tiles.

### 11.3 Classes with their own stream data

ObjVersion is 0 for every class except `TComplexObject` 1, `TCharacter`
4 and `TPlayer` 15. Everything not listed uses the base Load/Save.

| Class (builders) | Vtables | Load / Save | Body after the base fields |
|---|---|---|---|
| TComplexObject | `0x5a7b98` | `0x004db930` / `0x004dbb80` | uint8 TObjectInstance version; base; uint8 root action; stream string root name. Load keeps the loaded `state` when it is the root's state, else `state = FindState(root name)` |
| TCharacter (`Character`) | `0x5a7848` | `0x004d4eb0` / `0x004d50d0` | uint8 TComplexObject version; complex object; int32 lasthealthrecov, lastfatiguerecov, lastmanarecov, lastpoisondamage; int32 teleport x, y, z, level. Load (objversion ≥ 1) clamps a CHARACTER's health/fatigue/mana and maxima to its chardata, sets KILL when health < 1, and resets the fade (fade 100, step 0, limit 100, fade direction 0) |
| TPlayer (`Player`) | `0x5b4f30` | `0x0051b960` / `0x0051bdc0`, LoadInventory `0x0051bd20` | §11.4 |
| TAmmo (`AMMO`, `Arrow`, `Fire/Poison/Ice/Magic Arrow`) | `0x5a6944`…`0x5a73f8` | `0x004bf940` / `0x004bf980` | none (stream version < 5: int32 amount − 1) |
| TMoney (`MONEY`) | `0x5b4a7c` | `0x00515f40` / `0x00515fa0` | none (v < 5: int32 amount − 1). Load clears VIRGIN first |
| TContainer (`CONTAINER`), TInvContainer (`INVCONTAINER`) | `0x5a8118`, `0x5a8360` | `0x004dd3e0` / `0x004dd470` | none (2 ≤ v < 5: int32 locked, int32 pick difficulty). Load: unless INVENTORY, SetCommandDone(true) and SetState(state) |
| TVialRack (`VIAL RACK`) | `0x5a7ed0` | `0x004dd3e0` / `0x004ddbe0` | none; Save deletes the rack's contents first |
| TExit family (`EXIT`, `PressPlate`, `UpBlock`, `LEVER`) | `0x5b2258`, `0x5b24e0`, `0x5b27cc`, `0x5b2a4c` | `0x0050d990` / `0x0050d9c0` | container fields; int32 exitflags |
| TFood (`FOOD`), TPotion (`POTION`) | `0x5b2cd4`, `0x5b2ee4` | `0x0050eae0` / base | none; Load sets Amount to 1 when it is 0 |
| TWeapon (`WEAPON`) | `0x5b9088` | `0x00528e70` / `0x00528ea0` | int32 poison |
| TScroll (`SCROLL`) | `0x5b5560` | `0x00520d80` / `0x00520e00` | int16 length; text bytes |
| Drip (effect) | `0x5ac630` | `0x004f0fb0` / `0x004f1000` | int32 ripplesize, height, period |
| Speaker (ambient sound effect) | `0x5ada3c` | `0x004f47a0` / `0x004f4820` | stream string sound name; int32 ×3 (`+0x1a8..+0x1b0`; the 1998 source had one, the sample length) |
| Cube, BarrierCube | `0x5b341c`, `0x5b360c` | `0x0050f250` / `0x0050f1b0` | int32 x, y, z (a target; Load derives its offset from `pos`) |
| MonsterGen (class HELPER) | `0x5b4cd4` | `0x00516e00` / `0x00516eb0` | uint8 count of used slots (of 5); per slot: NUL-terminated monster name, int32 `+8`, int32 `+4` (capped at 19999), int32 `+0xc` start time (stream version ≥ 15); then int32 `+0xec` |

Retail never saves a MAPSCROLL (§11.1).

### 11.4 TPlayer, objversion 15

After `uint8 4` (TCharacter's version) and the character body:

| Field | Type | Retail member | Notes |
|---|---|---|---|
| quickspells | stream string ×5 | `+0x2cc` char[6] ×5 | construct slot, then buttons 1–4 |
| known spells | int32 count; count × char[6] | `+0x2ec` list | talisman codes the player has learned |
| HUD words | int32 ×4 | — | Save writes `DAT_0065d190` (sidebar open), `DAT_0065d1b8` (upper sidebar mode), `DAT_0065d1bc` (lower sidebar mode) and `DAT_0065d19c` (unidentified); Load stores them at `+0x304..+0x310`, which nothing reads |
| level-up stats | int32 ×3 | `+0x360..+0x368` | player-stat indices (from stat `0x22`) used by the level-up messages (`LUPBASE`, `STATCFG%s`) |
| multiplayer identity | stream string `+0x494`, stream string `+0x4c6`, int32 `+0x4d8`, `+0x490`, `+0x4dc` | 0x60-byte record at `+0x490` | set and compared by `0x0051e4d0`; `+0x4dc` groups frag totals (`0x0051f6b0`); empty in single player |
| module | stream string | `+0x4f0` | the active module's dirname |
| player state | int32 | `+0x36c` | `SetPlayerState` `0x0051d680`; LoadGame then sets `(state & ~2) \| 1` |
| state time | int32 | `+0x370` | game time; Load derives `+0x374` = time × 24 / 100 |
| profile strings | stream string ×4 | `+0x378`, `+0x570`, `+0x590`, `+0x5d0` | multiplayer lobby text (`0x0044b2d0` lists `+0x570`); empty in single player |
| frag counters | int32 ×4 | `+0x650..+0x65c` | multiplayer kill counts (`0x00519050`); the last two from objversion 15 |
| automap record | §11.5 | `+0x314` | objversion ≥ 14 |

Older objversions (`0x0051b960`): ≥ 4 base version byte and quickspells;
≥ 5 known spells (6–8 add 4 skipped bytes per spell); ≥ 7 the HUD words;
≥ 8 the level-up stats; 10 a fixed 50-byte `+0x494`; 11–12 `+0x494`,
`+0x4c6`, `+0x4d8`, `+0x4f0` only; ≥ 13 the identity, state, time,
strings and two frag counters; ≥ 15 the other two frag counters. Below
13 the state is 0 and the time is the current game time.

`TPlayer::LoadInventory` (`0x0051bd20`) remembers health, fatigue and
mana, loads the inventory, re-equips (`0x00519230`) and recomputes stats,
then restores the three values (equipment changes the maxima).

### 11.5 Automap record (player `+0x314`)

Load `0x00529770` / Save `0x00529830`: stream string module dirname (the
record is discarded when the module changes, `0x0052c3d0`); int32 level
count; per level: int32 level, int32 word count, that many int16 words:
the level's explored mask (0x2080 bytes) compressed by `0x005295e0`
(decompressor `0x00529220`). Before saving, the automap pane compresses
the level it is showing back into the record (`0x0052c5c0`).

### 11.6 Sector files

`TSector::Load` `0x00498780`: header (see
[SECTOR_FILE_FORMAT.md](../../../recon/docs/SECTOR_FILE_FORMAT.md)),
then the objects with load flag 1. Each loaded object gets the sector's
level and sector, and a position outside the sector's 1024×1024 square
is moved into it by whole multiples of 1024 on x and y (z unchanged). In
multiplayer with option bit 8, monsters and `monstergen` helpers are
dropped.

`TSector::Save` `0x00498c90`: writes `"MAP "`, 15, a zero hash and the
object count, then the objects (save flag 1), then rewrites the header
with the hash from `0x00499e90`:

- Adler-32 (`0x0056ff60` init 1, `0x0056ff80` update; bytes are
  sign-extended, the sums reduced mod 65521 after each 5552-byte chunk),
- over level, sector x, sector y and object count (int32 each),
- then, for each character or player object in the sector, its
  `SaveObject` bytes with save flags 1|2 (a player reduces to `ff ff`; no
  inventories);
- a result of 0 becomes `0xf0f0f0f0`.

### 11.7 Thumbnail `ss.bmp`

The play screen writes `.\ss.bmp` when the player opens the in-game menu
(`0x0047cb52`, then `0x0047e500`), opens the save dialog (`0x0047dc05`,
then `0x005399f0`), quick-saves (`0x0047dd08`, then `0x0047e850`) and on
one more command (`0x0047c800`): it creates a 640×480 16-bit bitmap
(`0x004a1ec0`), blits the display into it and calls `TBitmap::SaveBMP`
(`0x004a2960`) with scale 3. `SaveGame` copies the file into the slot.

`SaveBMP(file, scale)`: output width `(640 / 3 + 3) & ~3` = 216, height
480 / 3 = 160, 24-bit uncompressed, bottom-up. Headers: `BM`, file size
`(width × height + 18) × 3` = 103,734, data offset 54; info header size
40, planes 1, 24 bits, all else 0. Each output pixel is the mean of a
3×3 block of the 16-bit image (RGB565 for a 16-bit bitmap, RGB555 for a
15-bit one; channels widened with `<< 3` / `& 0xf8`, no low-bit fill),
stored B, G, R. Output row *r* (counted from the bottom) averages source
rows `475 − 3r … 477 − 3r` (the pointer steps back `(scale − 1) × 2`
rows instead of `scale − 1`; clamped to rows 0–2 for the top row); the
last three output columns read past x = 639 into the next row.

## 12. Test data

| Data | Content |
|---|---|
| module `newgame.sav` | player objversion 14 |
| `New Game1` (a retail slot: `game.sav`, `ss.bmp`, 283 `CurMap` sectors on levels 0, 2, 6) | game time 294,225; player objversion 15; 9 inventory items, one of them a pouch with 4 talismans |
| 275 retail working-set sectors (levels 0, 1, 2, 6) | written by retail during play |

`tools/savefmt/revsave.py dump` decodes all of them field by field with
no bytes left over.
