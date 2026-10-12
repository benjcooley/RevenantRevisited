# Forensics: the map queries combat leans on

**Topic:** the two TMapPane queries every fight runs through: the line of
sight (`0x4533d0`) that sight and the waypoint searches use, and the
objects-in-range search (`0x452060`) that area spells, missiles, pickups
and the AI's waypoints use.
**Status:** 2026-10-11. Retail read from the disassembly; port changed to
match; both measured by the dojo's katas `map-los` and `map-find-objects`
([../COMBAT_DOJO.md](../COMBAT_DOJO.md), M11 / M12).
**Evidence:** the retail disassembly (`recon/retail_asm/baseline`, the
unchanged executable); the Ghidra decompile of `0x452060` in
`recon/classes/cls_0x5a5320_TMapPane.cpp` (labelled TPlayScreen there; its
reading of the radius term is wrong, §2.2); the A/B dumps
(`build/retail_ab/map-los/`, `map-find-objects/`).

Walk cells are 16 units, centred on multiples of 16 (`(x + 8) >> 4`); a
sector is 64 × 64 cells (1024 units). Map pane `this` is `0x6668d8`.

## 1. LineOfSight `0x4533d0`

`TMapPane::LineOfSight(S3DPoint* from, S3DPoint* to, int level,
S3DPoint* blockedat, S3DPoint* laststep)`, thiscall, ret `0x14`. `this` is
never read. Returns 1 for a clear line, 0 for a blocked one.

Callers (all of them, by direct call):

| Site | Caller | Line | Outputs |
|---|---|---|---|
| `0x4cd651` | CanSeeCharacter `0x4cd540` | eye to eye: both positions + 50 (LIGHTINGCHARHEIGHT), the looker's level | 0, 0 |
| `0x528898` | WaypointReachable `0x528850` (thiscall on the waypoint, arg the character) | from (waypoint x, y, the character's z) to the character's position, **the waypoint's** level | 0, 0 |

WaypointReachable serves the AI's waypoint search (`0x4c9158`) and
Wander's (`0x4c95e0`, COMBAT_ATTACK_CHOICE.md §3.15). Spells and missiles
never call it; hearing has no line of sight (COMBAT_DOJO.md finding 13).

### 1.1 The walk

A DDA over walk cells in 16.16 fixed point, the height along the line
interpolated the same way:

```
sx = (from.x + 8) >> 4    sy = (from.y + 8) >> 4       // arithmetic shifts
ex = (to.x + 8) >> 4      ey = (to.y + 8) >> 4
dx = ex - sx   dy = ey - sy   dz = to.z - from.z
if |dx| >= |dy|:                                        // ties go to x
    loop = |dx|
    if loop > 0:
        dx = (dx << 16) / loop                          // idiv: toward zero
        dy = (dy << 16) / loop + (dy >= 0 ? 1 : -1)
        dz = (dz << 16) / loop + (dz >= 0 ? 1 : -1)
else:
    loop = |dy|
    dx = (dx << 16) / loop + (dx >= 0 ? 1 : -1)
    dy = (dy << 16) / loop
    dz = (dz << 16) / loop + (dz >= 0 ? 1 : -1)
x = sx << 16   y = sy << 16   z = from.z << 16
for c = 0 .. loop (inclusive: both end cells, loop + 1 of them):
    sector = FindSector(level, (x >> 16) >> 6, (y >> 16) >> 6)     // 0x499e10
    h = sector ? sector->ReturnWalkmap((x >> 16) & 63, (y >> 16) & 63) : 0
    if !sector or h == 0 or h > (z >> 16): blocked
    x += dx   y += dy   z += dz
return 1
```

- **Blocked** by a cell with no sector loaded for that level, no walkmap
  (height 0: a hole, off the map), or a walk height **above** the line's
  height there. A cell exactly at the line's height passes.
- The minor axis and the height get a nudge of one 1/65536 toward their
  sign, so a line of slope 1/2 lands on the cell boundaries' far side.
- `dz` is not divided by anything but `loop`: a steep line (a big height
  change over few cells) is still checked only once a cell.
- Same cell (`loop` 0): the start cell alone, against `from.z`.
- FindSector `0x499e10` (cdecl `(level, sx, sy)`) walks the loaded sector
  list (count `[0x668578]`, array `[0x668588]`; a sector's level `+4`, x
  `+8`, y `+0xc`). ReturnWalkmap `0x499720` reads the sector's 64 × 64
  `uint16` walkmap (`+0`) **masked to 10 bits** (`& 0x3ff`; a missing
  walkmap reads 0).

### 1.2 Outputs (no caller asks for them)

On a blocked return, with `x, y, z` the fixed-point position at the
blocking cell:

- `blockedat` (when non-null): `((x >> 16) << 4) - 8`,
  `((y >> 16) << 4) - 8`, `z >> 16` (the cell's corner, the line's
  height there);
- `laststep` (when non-null): `(x >> 16) - ((x - dx) >> 16)` and the same
  for y and z: the last step's change in cells (and in height units), -1,
  0 or 1 on the major axis; at the start cell it's the step that would
  have led there.

A clear line writes neither.

### 1.3 The port before

`TMapPane::LineOfSight(pos, to, obst)` carried the same walk (the 1998
source's), so the DDA itself matched. It differed in what it read and
returned:

- no level: it read the current map's walk cells (the pane's window, then
  MapManager's current map), where retail reads the caller's level's
  loaded sectors;
- no `laststep` output;
- its fixed point shifted negative values left (undefined in C++17).

The fix: retail's signature (`level`, both outputs), the cells read through
the level's loaded sectors (`GetWalkGridHeight(level, x, y)`: the pane's
window for the current level as before, else that level's cached map),
the shifts done unsigned. The callers pass retail's levels (the looker's;
the waypoint's).

**Kept divergence (open, not this kata's):** the port's walkmap keeps 16
bits a cell where ReturnWalkmap masks 10 (COMBAT_DOJO.md finding 8). The
kata's heights stay below `0x400`.

## 2. FindObjectsInRange `0x452060`

`TMapPane::FindObjectsInRange(S3DPoint* pos, int level, int* array,
int width, int height, int objclass, int maxnum, int objset)`, thiscall,
ret `0x20`. Fills `array` with map indices (the object's `+0x40`) nearest
first; returns how many.

Callers (args after `pos, level, array`):

| Site (function) | Width, height, class, max, objset | Use, where known |
|---|---|---|
| `0x422e50` (`0x422d70`) | `0x3200`, `0x3200`, 9, 64, (caller's) | a 12800-unit box |
| `0x4c02c2` (`0x4c01f0`) | 200, 0, any, 10, characters | |
| `0x4c5e0e` (`0x4c5d90`) | 60, 0, any, 10, all | |
| `0x4c9183` (AI `0x4c8b60`) | 250, 0, helpers (`0xf`), 10, all | the AI's waypoint search (COMBAT_ATTACK_CHOICE.md §3.10.5) |
| `0x4c9629` (`0x4c95e0`) | 2000, 0, helpers, 10, all | Wander's waypoint pick (§3.15 there) |
| `0x4d027a` (`0x4d01e0`), `0x4d04c7` (`0x4d0430`) | 60 / 80, 0, any, 10, all | two play-screen commands (`0x47d82b`, `0x47d83d`) |
| `0x4e6f87`, `0x4eb2fd`, `0x4ee164` | 500 / 200 / 400, 0, characters / any, 10, characters | effects (SPELLS_MISSILES.md) |
| `0x4f3b76`, `0x4f3bb9` (`0x4f39a0`) | 90, 0, characters (`0xc`), then players (`0xb`), 10, all | an effect's victims |
| `0x516db6` (`0x516c30`) | 300, 0, characters, 10, characters | |

### 2.1 The search

```
if maxnum < 1: return 0
dist = maxnum > 64 ? engine malloc(4 * maxnum) : a 64-entry local
if level < 0: level = this->level (+0x98, the map pane's)
if height < 1:  rect = [pos.x - width, pos.y - width] .. [pos.x + width - 1, pos.y + width - 1]
else:           rect = [pos.x - (width >> 1), pos.y - (height >> 1)]
                    .. [left + width - 1, top + height - 1]
found = 0
for o in MapIterator(rect, 0x6e0, objset, 0, level):    // init 0x44cf80, next 0x44d080
    if o->IsInInventory() (slot 0xb4: +0x7c >= 0): continue
    if objclass != -1 and o->class (+4) != objclass: continue
    if height < 1:                                       // a sphere
        r2 = objclass is 0xc or 0xb ? trunc((Radius() * 0.4f)^2) : 0     // slot 0x258
        d = dx*dx + dy*dy + dz*dz - r2                   // |dx| etc., 32-bit
        if d >= width*width: continue
    else:                                                // a box, flat
        if |dx| > width >> 1 or |dy| > height >> 1: continue
        d = dx*dx + dy*dy
    i = 0; while i < found and d > dist[i]: i++          // ties: the newcomer first
    if i >= maxnum: continue
    if i != found:                                       // memmove 0x58b790, both lists
        n = min(maxnum - 1, found) - i
        move dist[i .. i+n) and array[i .. i+n) up one
    array[i] = o->id (+0x40); dist[i] = d
    if found < maxnum: found++
free dist if it was allocated
return found
```

- **The iterator, flags `0x6e0`:** the loaded sectors (`0x80`) of one
  level (`0x400`, set because the level is >= 0), active or not (`0x200`),
  that meet the map rect, and their objects whose position lies in the
  rect, faces included (`0x40`; SRect::In `0x41c720`), no inventories
  (`0x20`). The rect is a half-open square: `+width - 1` on the right and
  bottom. So an object exactly `width` east or south is never handed out,
  and in a box of even width the `+width/2` column is cut although the box
  test would pass it.
- **The sphere is 3D** (height difference included) and measured centre to
  centre, less `(0.4 · Radius)²` (float constant `0x3ecccccd` at
  `0x5a4980`, x87 `fild`, `fmul`, squared, `_ftol` truncation) only when
  the search is for characters or players by class; a search for any class
  takes no radius off anyone. The box is flat.
- **Order:** by distance, nearest first; between equal distances the one
  handed out later comes first (the insertion stops at the first entry not
  nearer). A full list drops its farthest.

### 2.2 The port before

- It walked the pane's 3 × 3 window of the current level with no rect
  (`TMapIterator(nullptr, CHECK_NOINVENT, objset)`): objects of other
  loaded sectors were missed, objects outside retail's rect found, and the
  order (which is the tie order) was the window's.
- The insertion moved `n` **bytes** of the distances, not `n` ints
  (`memcpy`, overlapping), and never moved the indices: every object that
  landed before another left an array slot unwritten (the caller's garbage)
  and the distances torn. Any search that found a nearer object after a
  farther one returned junk indices.
- `(Radius / 2.5f)²` in float for the radius term (the same values for any
  radius in practice; now retail's arithmetic), `delete` for `new[]`.
- No level parameter.

The decompile reads the radius term as `__ftol` of something unnamed; the
asm (`0x4521d2`) is the square of `Radius · 0.4f`.

The fix: retail's search whole, with the level (an overload; the old
signature asks for the map pane's level, retail's `-1`), the iterator's
rect and loaded sectors (`TMapIterator(level, &rect, CHECK_NOINVENT,
objset)`), the shifting insertion.

## 3. Verification rig

Both katas run the original function on the retail side and the port's on
the other, over a stand-in map built from the case.

- **`map-los`** (`slots/combat/map_query.py` `call: line-of-sight`;
  `src/retailab_map.cpp`): the case's `ground` (guest.py: a base `z`,
  `cells` boxes, `nosector`, and here a `level` its sectors belong to)
  answers FindSector / ReturnWalkmap (retail) and GetWalkGridHeight
  (port); each cell read is recorded (`Cell`: level, cell or sector,
  height), so the walk itself is compared, step by step, with the result
  and both outputs.
- **`map-find-objects`** (`call: find-objects`): the map iterator is the
  seam on both sides (`MapIterator`: the rect, objset and level it was
  given); it hands out the case's `objects` in case order, those in the
  rect (inclusive) on that level. Characters are the fixture's own (their
  Radius through the GetStat seam), the rest stand-ins (class, position,
  inventory slot, id). Compared: the count, the array (every slot up to
  `maxnum`, pre-filled), the seams.

## 4. Gaps

- The walkmap's top six bits (ReturnWalkmap's mask; GetWalkHeight's third
  argument returns them) are unported, finding 8.
- The iterator itself (`0x44cf80` / `0x44d080`) has no kata of its own; the
  port's loaded-sector walk is its REVSYNC'd counterpart.
