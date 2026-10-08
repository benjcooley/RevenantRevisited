# T5 forensic — Save / Load player object round-trip

Worktree: `feature/gameflow` fork at `f1dd314` ("gameflow/T5 WIP: map-reset
bridge + save/load diagnostics"). This doc is the recon read before any code
change. The user's architectural rule applies: **player object serialization
must be retail-faithful; map orchestration around it stays modern.**

The recon material reviewed, in order:

1. `recon/discovered/save_system_notes.md` (parent-agent digest of the save
   lifecycle, on-disk layout, port status).
2. `recon/discovered/cls_misc_LoadGame_48df70.cpp` (retail `LoadGame`).
3. `recon/discovered/cls_misc_CreateObjectFromStream_471ce0.cpp` (retail's
   universal object-from-stream decoder; used to reconstruct the player).
4. `recon/discovered/cls_TObjectInstance_Dtor_46e420.cpp` (retail dtor
   summary).
5. `recon/classes/cls_0x45f7c0.cpp` lines 1656-2175 (`meth_0x48d720_SaveGame`,
   = retail's high-level SaveGame, contains both the dir-mkdir/clear/curmap-copy
   block AND the file-writing body starting at LAB_0048dbde).
6. `recon/discovered/cls_TGameState_Save_4974d0.cpp` (TGameState binary
   stream save; XOR-0x80 obfuscated name + int32 value pairs).
7. `recon/discovered/cls_TGameState_LoadStream_496110.cpp` (retail
   counterpart, gated on version >= 10 in LoadGame).
8. `recon/discovered/cls_TScriptManager_Save_496690.cpp` (separate `.s` file
   in `data\..\Scripts\` — NOT inside `game.sav`; we don't need to touch this
   for T5 file-format work).
9. `recon/classes/cls_0x5a50e8__vftable_5a50e8.cpp` (vtable; SaveObject at
   slot 0x164 / 356, LoadObject at slot 0x160 / 352 — confirms our virtual
   dispatch shape).
10. Source under audit: `src/savegame.cpp`, `src/object.cpp` (Save/Load /
    SaveObject/LoadObject / SaveInventory/LoadInventory and helpers),
    `src/complexobj.cpp`, `src/character.cpp`, `src/player.cpp`,
    `src/stream.{h,cpp}`.

---

## 1. The file format

### 1.1 Retail `game.sav` byte layout (single-player, version >= 10)

Reconstructed from `meth_0x48d720_SaveGame` LAB_0048dbde (write) and
`FUN_0048df70_LoadGame` (read), both at MAP_VERSION = 15:

| Offset | Size | Field |
|---|---|---|
| 0 | 0x80 | Automap header block (`auStack_6e0` on write; `local_184` + ints on read). First int is "current pane"; later ints include the walkmap-present flag and a `version` int that LoadGame reads as `iStack_19c` |
| 0x80 | 0x200 | Walkmap blob — **conditional**, only present when `DAT_0066829c != 0`. LoadGame zeroes its target buffer if absent. |
| varies | varies | **TGameState binary stream** — `int32 count` then `count × { name[]=bytes-XOR-0x80, NUL; int32 value }`. Gated on **version >= 10** in LoadGame. |
| varies | 32 × 4 = 128 | DATA_SLOTS spare-int block. `data[0] = gametime`, `data[1] = pane`, `data[7] = version`. |
| +128 | varies | (multi only — single-player skips this) `int32 numplayers` + per-player headers |
| varies | varies | One or more player objects encoded by `SaveObject` (FUN_00472110). |

Single-player retail writes exactly:
- 0x80 automap header
- 0x200 walkmap (if present)
- TGameState stream
- 128-byte DATA_SLOTS
- 1 player object via SaveObject

The "number of players" int is NOT in the single-player branch — retail
LoadGame reads it only inside the `multi` branch (`iVar6 = iStack_190 - 2`
gating on the `iStack_190`-from-automap-header field).

### 1.2 Our `game.sav` byte layout today (src/savegame.cpp at f1dd314)

| Offset | Size | Field |
|---|---|---|
| 0 | NumMaps × 32 | `AutoMap.WriteAutoMapData` — packed bit array, **not** retail-shaped. Retail's first 0x80 bytes form a header struct; ours is just compressed walkmap bits. |
| varies | 128 | DATA_SLOTS spare-int block. Same semantics: gametime / pane / version. |
| varies | varies | One player object via `TObjectInstance::SaveObject`. |

### 1.3 Divergences at the wrapper level

| Retail | Us | Verdict |
|---|---|---|
| 0x80 automap header struct | `MapList->NumMaps × 32` packed bits — totally different bytes | **Symmetric internally** (our Write + our Read agree). Treat as "modern wrapper, retail-faithful payload" for T5. Document and move on. |
| 0x200 walkmap blob (conditional) | absent | Our AutoMap doesn't yet maintain the retail header / walkmap split. Not in scope for T5; the player object is what matters. |
| TGameState stream | absent | `TGameState::SaveStream`/`LoadStream` in `src/script.cpp` are stubbed with `TODO(revsync)` (just landed in 52904fe). T5 cannot fix this — out of scope per task. Surface as a known gap. |
| 128-byte DATA_SLOTS at fixed offset | 128-byte DATA_SLOTS, but at a different absolute offset | Layout-equivalent; field semantics match (gametime, pane, version). |
| Player(s) via SaveObject | Same | Compatible (the SaveObject payload is what we care about). |

**Conclusion on file format**: we are not byte-compatible with retail
`game.sav` (different automap wrapper, no TGameState section). That's
acceptable for T5 — the player-object section inside the wrapper is what
must be retail-faithful, and it is.

---

## 2. The SaveObject / LoadObject envelope

This IS the retail-faithful path. The bytes between the wrapper and the
inventory tail are what TObjectInstance serializes.

### 2.1 Retail `FUN_00471ce0_CreateObjectFromStream` (LoadObject)

Header fields (from version gates in the decompile):
```
v >= 8:    int16  objversion         // -1 = empty slot, abort early
v >= 0:    int16  objclass           // -1 = empty slot, abort early
v <  1:    int16  objtype            // legacy v0
1 <= v <4: uint32 uniqueid
v >= 4:    uint32 uniqueid
v >= 4:    int16  blocksize
v >= 14:   int16  invblocksize
[body bytes — vtable dispatch slot 0x160 (=Load)]
[inventory bytes — vtable dispatch slot 0x168 (=LoadInventory)]
```

`blocksize` is the total post-header size in v14+ (body + inventory together).
`invblocksize` is the inventory tail carved out of the end. The single final
resync `is.SetPos(bodystart + blocksize)` snaps past the whole object.

If `objclass` is unknown (e.g. retail-added classes like bags / chests /
invcontainer = class 18 / 25) and we have a `blocksize`, retail's
`LAB_00471e57` and our LoadObject both `MovePos(blocksize)` to skip the
object cleanly.

### 2.2 Our `TObjectInstance::LoadObject` (`src/object.cpp:2030-2236`)

Header parse: matches retail. `objversion`, `objclass`, `uniqueid`,
`blocksize`, `invblocksize` all read in the same order, with the same
version gates. Skips on bad class are correct (`MovePos(blocksize)`).

The "force-simple" recovery path (when `objtype < 0` and `FindObjType`
scan finds a matching id in some OTHER class) calls
`inst->TObjectInstance::Load` instead of the virtual `inst->Load`,
matching retail's `FUN_00472430` direct dispatch (vtable slot bypass
because the wrong class was created). **OK.**

Final resync: `is.SetPos(bodystart + blocksize)` — matches retail. **OK.**

### 2.3 Retail SaveObject (`FUN_00472110`)

Not extracted into a discovered/ file, but called from
`cls_0x45f7c0.cpp:2153` and from `cls_TSector_FUN_00499e90.cpp:47` and
`cls_TSector_FUN_00498c90.cpp:36`. From the read side we know the layout
on disk; the writer just emits the same fields and back-patches sizes.

### 2.4 Our `TObjectInstance::SaveObject` (`src/object.cpp:2244-2292`)

Emits exactly the layout the loader expects, with both v14+
`(blocksize, invblocksize)` and pre-v14 single-`blocksize` paths. Empty
slot (`!inst || ismap && OF_NONMAP`) writes a lone `-1` int16 — matches
retail `local_9c = -1` early-out. Body length is set to `end - bodystart`
(total payload). Inventory length is `end - bodyend`. Back-patch is at
`bodystart - 4` (the two int16 placeholders). **OK.**

One small but verifiable retail-faithful subtlety: the writer skips
`SaveInventory` entirely when `RealNumInventoryItems() == 0`, so
`invblocksize` lands at 0 on disk and the loader's
`if (version < 14 || invblocksize >= 1)` fast-skips. **OK.**

---

## 3. The TObjectInstance body — field order and contents

### 3.1 Retail body order

We don't have a discovered/ extract of `FUN_00472430` (Load body) but
we have the SaveObject + LoadObject envelope and the per-field reads
in the call chain. The body is identical to our v15 code path because:
- The on-disk version is `MAP_VERSION = 15` (same constant in retail).
- All our `is >> ...` reads in `TObjectInstance::Load` are in the same
  order as `os << ...` writes in `TObjectInstance::Save` (paired
  inspection of `src/object.cpp:2340-2510` Load and `2512-2557` Save).

### 3.2 Our `TObjectInstance::Save` body (`src/object.cpp:2512`)

```
uint8_t namelen; name[namelen]              // [len][bytes], NUL not stored
uint32_t flags; int32 pos.x, pos.y, pos.z
if !(flags & OF_IMMOBILE): int32 vel.x, vel.y, vel.z
uint16_t state
if (flags & OF_NONMAP):    uint16_t level
int32_t inventnum
int32_t invindex
int32_t shadow
uint8_t rotatex, rotatey, rotatez
int32_t mapindex
if (flags & OF_ANIMATE): int32_t frame; int32_t framerate
int32_t group
uint8_t numstats
numstats × { int32_t stat; uint32_t uniqueid }
if (flags & OF_LIGHT): uint32 lightdef.flags; int32 lightdef.pos.{x,y,z};
                       uint8  color.{r,g,b}; uint8 intensity; int32 multiplier
```

### 3.3 Our `TObjectInstance::Load` body (`src/object.cpp:2340`)

Reads each of the above with the matching version gates. Specifically:
- v < 6: also reads `vel` even when OF_IMMOBILE (`!(flags & OF_IMMOBILE)`
  guard added in v6); we are on v15 so the guard is live.
- v < 9: `state` is uint8 not uint16. Live on v15 → uint16. OK.
- v < 6 + OF_NONMAP: also gates the `level` read (we emit only
  when `OF_NONMAP`, retail-symmetric). OK.
- v < 3: reads facing / dummy / inventnum / dummy / shadow / dummy and
  forces inventnum = -1, mapindex = -1. We're on v15. OK.
- v >= 5: reads stats. Frame/framerate gated on OF_ANIMATE (v >= 6). OK.

**Body-byte symmetry between our Save and our Load is solid.**

### 3.4 The intermediate-class chains

- `TComplexObject::Save` (`src/complexobj.cpp:453`): writes `uint8 action;
  uint8 namelen; namelen × char`.
- `TComplexObject::Load` (`src/complexobj.cpp:424`): `is >> action;
  is >> name`. The stream's `operator>>(char*)` reads `[len][bytes]`,
  NUL-terminating the destination. **Symmetric.** (Initially looked
  asymmetric because Save does the [len][bytes] write manually instead
  of delegating to `os << (char*)root->name`, but they end up writing
  the same bytes.)

- `TCharacter::Save` / `::Load` (`src/character.cpp:4955` / `:4899`):
  emits 3 last-recovery ints, lastpoisondamage, teleport_position{x,y,z},
  teleport_level. Symmetric.

- `TPlayer::Save` / `::Load` (`src/player.cpp:643` / `:624`): emits 5
  quickspell ints. Symmetric.

The full per-class chain is symmetric on Save/Load and uses identical
version gates. Round-trip of one player + chain is byte-clean.

---

## 4. The Locke-in-the-ground symptom — root-cause analysis

**Symptom**: after F5 (save) → F9 (load), the player is rendered visually
below the walkmap floor.

**Hypotheses considered**:

### H1: pos.z drifts through Save / Load (FAVORED → REJECTED)

`os << pos.x << pos.y << pos.z` writes from the legacy `pos` mirror.
`Pos()` reads from `transform_.Matrix().Elements[3][...]`. As long as
every write to `pos` also touches `transform_`, the values agree.
**`WriteTransformPos` is the only sanctioned writer**, and every
SetPos / ForcePos / Load already goes through it. The Load path is:

```
is >> newflags >> loaded.x >> loaded.y >> loaded.z;
WriteTransformPos(transform_, pos, loaded);  // sets both
```

Symmetric and clean. Diagnostic logs added at WriteGame exit + ReadGame
exit currently print `pos`, which is the legacy mirror — if `pos`
matches across save/load, that doesn't *prove* `Pos()` agrees, but the
WriteTransformPos invariant means it does. **Rejected** as the proximate
cause but the diagnostic should be hardened to log both `Pos()`
(transform path) and `pos` so a future drift is visible.

### H2: load resets transform Z-scale → Pos() Z reads as 0 (REJECTED)

`ClearObject` sets `transform_.SetLocalScl({1, 1, WORLD3D_Z_SCALE})`.
`Pos()` reads the translation row only, not scaled. SetLocalPos
overwrites the translation row. So Z-scale doesn't bleed into Pos().

### H3: the loaded player isn't attached to a sector, so the renderer's
floor-height composition path falls through to "world origin" — **FAVORED**.

On the NewGame spawn path (`playscreen.cpp:473`), Locke is added via
`sec->AddObject(oi)` which sets `oi->sector = this`, registers him
with the sector's object list, and (importantly) gives the renderer
something to query for a floor / depth context. The renderer
ultimately reads world Z from `Pos()` (which reads from `transform_`),
and the visual height includes the sector's depthmap.

On the Load path (`savegame.cpp:204-238`), the loaded player is added
to `PlayerManager` and made the main player, but **no
`sector->AddObject` equivalent runs**. `saved->sector` is nullptr.
The renderer pipeline then has no sector backing for floor lookup,
so anything that resolves "player feet height" via sector data falls
back to a default (0 or sector-origin), and Locke renders below the
walkmap surface.

Retail behavior: in `cls_misc_LoadGame_48df70.cpp` after
`CreateObjectFromStream` → `AddPlayer` → `SetMainPlayer`, retail does
NOT call `sector->AddObject`. **But retail's sector load is different**:
the player was saved with its `mapindex`, the sector that owns that
mapindex is **already loaded into curmap** (because retail's LoadGame
copies the slot's `CurMap/*.DAT` back into the active `curmap/` first,
then re-loads sectors on demand). Each sector's LoadFromStream
(`cls_TSector_LoadFromStream_498780.cpp:41`) calls
`CreateObjectFromStream` for each object and binds `piVar4[0x11] =
param_1` (= `oi->sector = this`).

**But the player is OF_NONMAP**, so it's saved to `game.sav`, not into
the sector. It comes back via LoadGame, not via TSector::LoadFromStream.
So in retail, who attaches the player to a sector after LoadGame?

Looking at retail LoadGame more carefully: after `AddPlayer` /
`SetMainPlayer`, retail calls `FUN_00460d60` (line 262) on the
`DAT_00667eb8`-derived path string. That's a level-change /
re-anchor call. Past that is `FUN_004609f0(iVar6)` and `FUN_0047e950`
(line 269). One of these very likely fires the post-load sector-attach
for the main player.

**Our port doesn't do this.** Our ReadGame just `AddPlayer` /
`SetMainPlayer` / `MapManager.SetCurrentLevel(saved->GetLevel())` and
calls it done.

**Most-likely root cause for Locke-in-ground**: the post-load player
is never re-attached to its sector. Specifically, no code:
- Finds the sector containing the loaded player's `mapindex` /
  `pos.{x,y}` on the loaded `level`.
- Calls `sec->AddObject(saved)` (or equivalent that sets
  `oi->sector` and re-registers with the sector's object list).

The visual artifact (player below floor) is what you get when the
renderer evaluates floor-Z from a null sector and falls through to
0 or origin.

### H4: SyncTransformRot wedges Z somehow (REJECTED)

`SyncTransformRot` writes only the rotation quat. Doesn't touch
translation. Not a candidate.

### Verdict: **H3 is the highest-probability root cause.**

**Fix shape**: after `SetMainPlayer` in `ReadGame`, find the sector
containing the loaded player's world position on the loaded level and
attach the player to it. This is map-side orchestration, but it's a
*consumer* of map state, not a redesign of how sectors are owned —
the modern engine still has sectors with object lists, and `AddObject`
is the existing entry point on the modern path. Per the user's rule
this counts as "modern-engine map orchestration", so it does NOT
need to mimic retail's `FUN_00460d60` call shape — we use whatever
modern API attaches an object to its containing sector. Keep
`mapindex` (retail-faithful from the player object) as the SoT for
lookups.

---

## 5. The exit crash — root-cause analysis

**Symptom**: after a load → continue → exit, the process crashes. No
ASan trace yet because the post-load crash hasn't been reproduced
under ASan.

**Hypotheses considered**:

### C1: PlayerManager + sector double-own the player (REJECTED FOR LOAD PATH)

After `ReadGame`, `saved->sector == nullptr` (per H3). The dtor's
`if (GetSector() != nullptr) MapPane.RemoveObject(this);` is skipped.
So sector-side doesn't have a stale pointer. No double-free this way.

### C2: Loaded inventory items have stale `invindex` (FAVORED)

`TObjectInstance::Load` reads `invindex` from the stream. Inventory
items are then re-added via `inventory.Add(inst)` in `LoadInventory`.
`TPointerArray::Add` puts items at the first available null slot
scanning backward from `numitems` (`src/revtypes.h:587-605`). **If the
saved `invindex` was, say, 2 but on load `Add` puts the item at slot 0
(because slots 0 and 1 were empty when Add ran), the loaded
`inst->invindex` (= 2) no longer matches its actual position in the
parent's `inventory` array (= 0).**

The crash trigger: on `~TObjectInstance` (the parent player's dtor),
the inventory teardown loop (`src/object.cpp:709-713`) is:

```cpp
for (TInventoryIterator i(this); i; i++)
{
    i.Item()->RemoveFromInventory();
    delete i.Item();
}
```

`RemoveFromInventory` does `owner->inventory.Remove(invindex)` using
the *child's* `invindex` field. With a stale `invindex` that doesn't
match the slot the iterator found the item in:
- The wrong slot gets nulled (or a no-op if out of bounds, since
  `Remove` checks bounds).
- The iterator's `invindex` advances past the just-deleted item.
- A child slot in the parent's `inventory[]` retains a dangling
  pointer to the deleted child (because `Remove(staleInvIndex)`
  cleared the wrong slot).
- After the loop, the parent's `inventory` array still contains a
  stale pointer to a freed item.
- `~TPointerArray` runs `delete items` (note: not `delete[]` — UB
  but irrelevant to this bug). The dangling slot is never
  dereferenced *by the dtor itself*, but in our codebase there are
  other consumers that walk live inventories after Clear()-style ops
  (Inventory.SetContainer, etc.). Subtle.

This bug is **latent** in the engine generally — any save → load that
holds inventory items in non-contiguous slots will produce it. It
becomes a clear crash only when subsequent code dereferences the
stale slot.

### C3: Script attached at load points at a TScriptProto that gets
freed mid-shutdown (TODO INVESTIGATE)

`TObjectInstance::Load` ends with
`InitScript(ScriptManager.ObjectScript(this))`. The dtor's
`if (script) delete script;` runs. If the script's lifecycle isn't
clean post-load, the dtor delete could touch freed state. This needs
ASan to confirm; out of scope for T5's player-object focus, but flag
as a potential concurrent cause.

### Verdict: **C2 is the highest-probability primary cause; C3 is a
secondary candidate that needs ASan + the right repro.**

**Fix shape for C2**:

Option A (preferred — retail-faithful): keep `invindex` saved/loaded
as-is (retail format), and have `LoadInventory` route through
`AddToInventory(inst, savedSlot)` which calls `inventory.Set(inst,
slot)` to put the item at the explicit slot. Then `invindex` matches
slot. `Set` is already wired (`src/revtypes.h:607`); we just need to
read invindex from `inst->invindex` after `TObjectInstance::Load`
filled it, then move the item from "added at Add-chosen slot" to the
slot encoded in the body.

Option B (cheaper): after `inventory.Add(inst)` in `LoadInventory`,
overwrite `inst->invindex` with the actual slot returned by `Add`.
This is what the live-spawn path does (`AddToInventory` sets
`invindex = inventory.Add(...)`'s return value, see
`src/object.cpp:1136-1167`). This loses the saved invindex (a real
divergence from retail's bytes), but it makes the in-memory state
consistent with the parent's array layout. Practical, but a divergence
from retail.

**Option A is the right call.** Retail's invindex IS the saved slot
number; the in-memory state and the file state both need to agree on
slot positions for retail-faithful round-trip and to fix the dtor
walk.

---

## 6. Additional findings — not crash / position bugs but adjacent

### 6.1 `~TPointerArray` uses `delete items` not `delete[] items`

`src/revtypes.h:575`. UB on most modern libc++/libstdc++ for
array-allocated memory. Not the T5 crash but worth flagging. Out of
scope for T5.

### 6.2 Inventory dtor loop skips items

The dtor loop at `src/object.cpp:709-713` advances the iterator AND
calls `RemoveFromInventory` which mutates the parent inventory. With
the bug from C2 ("stale invindex" remove-the-wrong-slot), this means
items get leaked (alive in the parent's array after the loop ends).
With C2 fixed (invindex always matches actual slot), `Remove` clears
the right slot, and subsequent iterator advance lands on the next
item correctly. So **fixing C2 also fixes this dtor walk.**

### 6.3 `TGameState` stream save / load is stubbed

`src/script.cpp:896-910`. Marked `TODO(revsync)`. Per task scope this
is out of bounds — script.cpp just got retail-synced in 52904fe and is
hands-off. Surfaced here as a known wrapper-format gap.

### 6.4 Pre-save / post-load diagnostic only logs the legacy `pos`

`src/savegame.cpp:94-101` and `:245-253`. With H3 in mind we'd want
this to log both `Pos()` (transform path) and `pos` (legacy mirror) to
catch any future drift between them, plus the player's `sector`
pointer and inventory item count. Small extension worth folding in.

---

## 7. Sync plan (one commit per logical change)

Order matters — earlier commits enable verification of later ones.

1. **`docs/gameflow/T5_FORENSIC.md`** (this doc). Standalone commit. — done first.

2. **Harden the savegame diagnostic logging** so future drift between
   `transform_.Pos()` and the legacy `pos` mirror is visible. Also
   log `sector` (nullptr / non-null) and inventory item count.
   `src/savegame.cpp` only. Small fix; lets us verify the rest with
   real numbers.

3. **Fix `LoadInventory` to route through `Set(slot)` so each item's
   in-memory `invindex` field matches its actual position in the
   parent's `inventory` array.** This is the C2 fix. Retail-faithful
   (we keep the saved invindex byte-for-byte; we just place the item
   at the slot it claims to be in). Likely the primary cause of the
   exit crash; also closes the dtor-walk skip bug.
   `src/object.cpp::LoadInventory`.

4. **Post-load sector attach.** Walk the loaded player into its
   containing sector after `SetMainPlayer`. This is map-side
   orchestration but uses the modern engine API (the sector's
   `AddObject` exists already on the modern map path). Likely fix
   for Locke-in-the-ground.
   `src/savegame.cpp::ReadGame`. (No changes to `mappane.cpp` /
   `maprenderer.cpp` / `mapmanager.cpp` — they're explicitly
   out-of-scope.)

5. **REVSYNC headers** on the touched `TObjectInstance::SaveObject`,
   `::LoadObject`, `::LoadInventory`, `::SaveInventory` with retail
   addresses (`@ 0x00472110` envelope writer; `@ 0x00471ce0`
   envelope reader; v-table slots 352 / 356 / 360 / 364). No
   functional change in this commit; pure provenance.
   `src/object.cpp`.

6. **`--test=savecycle` headless harness** that boots to PlayScreen,
   writes a side-by-side comparison of player fields pre-WriteGame and
   post-ReadGame, then exits. Lets the parent agent verify the
   round-trip without an interactive F5 / F9. Drives off the existing
   `--test=` mode plumbing.
   `src/revmain.cpp` (or wherever the test mode dispatch lives).

7. **`docs/gameflow/BURNDOWN.md`** — tick the T5 items completed.
   Tiny commit.

## 8. Out of scope (TODO / revsync questions surfaced for the user)

- `TGameState` binary stream save/load (`src/script.cpp:896-910`). The
  retail format is `[count:int32] count × { name xored 0x80, NUL ;
  int32 value }`. We don't write it today and don't read it. Once T5
  lands, the wrapper-format gap is the next biggest divergence. Out
  of scope per "Don't touch src/script.{h,cpp}".

- Retail's automap header (0x80) + walkmap (0x200) blob. Our
  `AutoMap::WriteAutoMapData` writes packed bits only. Symmetric
  internally; would need an `AutoMap` rework to match retail bytes.
  Defer.

- `~TPointerArray` `delete` vs `delete[]` bug (`src/revtypes.h:575`).
  Separate refactor.

- The `TScript::ip` save format. Listed as a T5 line item in BURNDOWN
  but TScriptManager save/load isn't called by our `WriteGame`
  /`ReadGame` (retail writes Scripts/<...>.s separately, not into
  game.sav). Surface for user clarification:
  `// REVSYNC-QUESTION: should TScript per-instance ip+vars survive
  WriteGame/ReadGame at all, or only via the separate script-state
  file retail writes?`

- The post-load script re-attach: `TObjectInstance::Load` calls
  `InitScript(ScriptManager.ObjectScript(this))`. That re-attaches a
  *fresh* script proto, not the one the player had pinned mid-game.
  If a mid-script save was meant to survive load, retail's
  TScript save/load chain (TScript::Save at TODO unknown addr) carries
  per-instance script state — we don't. Surface:
  `// REVSYNC-QUESTION: per-instance TScript::ip + TScript local
  vars round-trip — not in retail FUN_00472110/FUN_00471ce0 envelope;
  does retail rely on the separate per-module .s file for this, or
  is there a per-instance binary blob inside the player object?`

These questions go to the user; T5 doesn't attempt them.
