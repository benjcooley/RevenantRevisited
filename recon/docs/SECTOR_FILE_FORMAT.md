# Sector file format (`N_X_Y.DAT`)

Revenant sector files live in two places and are loaded via
`TSector::LoadSector` (tries current save dir `CurMap/` first, then the
static `Map/` or packed-RVR fallback). Per-sector filename format:
`<level>_<sectorx>_<sectory>.DAT`.

## Header

Parsed by `TSector::Load` in [src/sector.cpp:114](../../src/sector.cpp#L114)
(also reference: 1998 source [Revenant/sector.cpp:114](/Users/benjamincooley/projects/Revenant/sector.cpp#L114)).

### 1998 source layout (what the port ships today — INCOMPLETE for retail v15)

```
int32  first                // either numobjects (old) or FCC "MAP "
if (first == 'MAP ') {
    int32 version
    int32 numobjects
}
```

### Retail v15 actual layout (confirmed 2026-04-17, binary-traced)

```
char[4] fcc        // "MAP "  = 0x20504150 LE (bytes: 4d 41 50 20)
int32   version    // = 0x0f (=15) in shipped data
int32   statehash   // v14+: CRC32 over state-bearing objects (see below)
int32   numobjects
```

`statehash` is a hash regenerated at save time by `FUN_00499e90` over
sector coords (level/sectorx/sectory) + numobjects + the serialized
body of state-bearing objects (object classes `0xb` and `0xc` —
TCharacter/TPlayer). It differs between save copies even when the
object list looks identical, and is likely used as a fingerprint so
the engine can detect whether a sector file has changed since last
save (e.g., to decide whether to reload from disk). Traced in
`data/Revenant.exe` (Ghidra project `RevenantDev`):

- `TSector::Load` = `FUN_00498780` — reads field into `TSector+0xb4`
  when `version > 13`.
- `TSector::Save` = `FUN_00498c90` — calls `FUN_00499e90` to compute
  a fresh hash, writes after version. The hash appears to be used as
  a fingerprint to detect whether a sector file has changed since last
  save (possibly CRC-family, but the exact algorithm is unconfirmed).

Evidence (`data/Curmap/` shipped sectors):

| File | FCC | version | mystery (hex LE) | numobjects |
|---|---|---|---|---|
| `0_0_27.DAT` | `4d 41 50 20` "MAP " | `0f 00 00 00` = 15 | `aa f4 5c 30` = `0x305cf4aa` | `53 00 00 00` = **83** |
| `2_8_10.DAT` | `4d 41 50 20` "MAP " | `0f 00 00 00` = 15 | `15 00 e0 00` = `0x00e00015` | `00 00 00 00` = **0** (empty) |

Without skipping the 4-byte mystery field, `numobjects` reads as
`0x305cf4aa` (811 M) for `0_0_27`, which explains why our port's sector
test harness never finishes the object loop (it enters the body with a
garbage count and each `TObjectInstance::LoadObject` then misparses
because the stream is offset by −4). The `TObjectInstance::Load`
misparse — every object reporting `"Invalid inventory size"` — is a
second-order symptom of the wrong start offset, not an independent bug.

## Object array

After the header, `numobjects` consecutive object blocks each formatted
per `TObjectInstance::SaveObject` (see [src/object.cpp:1783](../../src/object.cpp#L1783)).

### v4..v13

```
int16   objversion        // v8+ only
int16   objclass          // -1 = empty slot (skip)
uint32  uniqueid
int16   blocksize         // covers BOTH body and inventory
... body ... inventory ...
```

### v14+ (confirmed 2026-04-20 via FUN_00471ce0 full decomp)

```
int16   objversion
int16   objclass          // -1 = empty slot (skip)
uint32  uniqueid
int16   blocksize         // TOTAL post-header bytes (body + inventory)
int16   invblocksize      // tail portion of blocksize used for inventory
... body proper ...       // blocksize - invblocksize bytes
... inventory body ...    // invblocksize bytes (only when invblocksize > 0)
```

IMPORTANT: `blocksize` in v14+ covers body **and** inventory together;
`invblocksize` just carves out how much of the block's tail is inventory.
The body-proper is `blocksize − invblocksize` bytes. Do not add
`invblocksize` on top of `blocksize` when skipping an object — that
double-counts the inventory bytes and drifts the stream forward.

Key retail behavior from FUN_00471ce0:
- `LAB_00471e57` (bad-class or EFFECT skip): advances to
  `end_of_header + blocksize`, i.e. one single skip of `blocksize` bytes.
- `LAB_00471fa3`: when `version > 13 && invblocksize < 1`, LoadInventory
  is **not called** at all (the vtable dispatch at `*vtbl+0x168` is skipped).
- When `invblocksize > 0`, LoadInventory runs from position
  `start_of_body + (blocksize − invblocksize)`.
- `LAB_00471fbc` (final resync for every valid-load path): sets the stream
  to `start_of_body + blocksize` = start of next object, irrespective of
  how many bytes Load() and LoadInventory() actually consumed.

The stream position is re-synced to `start_of_body + blocksize` after each
object, so a single unknown/corrupt object (or overread Load) is non-fatal.

Verified empirically 2026-04-20: re-parsing shipped `data/Curmap/*.DAT`
sectors with this model parses every tested sector (including
`0_2_26`, `0_3_27`, `0_7_22`) to exactly file size with zero drift; the
previous body-only interpretation cascaded into `bad_class` garbage after
the first object that carried inventory.

### SaveObject (retail FUN_00472110)

Mirror image of the layout above. Written by
[src/object.cpp TObjectInstance::SaveObject](../../src/object.cpp) which
now matches retail's structure:

1. `MakeFreeSpace(1024)`.
2. If `inst == nullptr` OR (saving a map AND `inst->Flags() & OF_NONMAP`):
   emit a single `int16 = -1` placeholder and return.
3. Otherwise emit `[objversion][objclass][uniqueid][blocksize=0][invblocksize=0]`
   (the two sizes are zero placeholders, patched at step 7). For `MAP_VERSION < 14`
   the `invblocksize` slot is omitted (single-size pre-retail layout).
4. Record `bodystart = os.GetPos()`.
5. Call `inst->Save(os)`.
6. Record `bodyend = os.GetPos()`.
7. Only call `inst->SaveInventory(os)` when `inst->RealNumInventoryItems() > 0`.
   Retail's `(**(*vtable+0x170))()` guard amounts to "don't emit the 4-byte
   count=0 stub for empty inventories" — skipping the call keeps
   `invblocksize == 0` on disk, which LoadObject uses as the fast-path gate.
8. `end = os.GetPos()`. Rewind to the size placeholders and patch:
   `blocksize = end - bodystart` (total body+inv bytes),
   `invblocksize = end - bodyend` (inventory tail only).
9. `SetPos(end)`.

This means an empty-inventory container saved with the retail-compatible
path writes exactly zero bytes of inventory (no count stub), and a fresh
sector round-trip through our Save → Load path reproduces the retail
byte layout.

## Version mapping (from 1998 source + retail trace)

- `v0` — no unique IDs, objtype serialized directly, no blocksize
- `v1..v3` — uniqueid instead of objtype, no blocksize
- `v4+` — adds blocksize (enables skip-unknown)
- `v8+` — adds per-object version field
- `v14+` — splits single blocksize into blocksize + invblocksize (object header)
- `v15` — shipped retail format; sector header gains `statehash` (see above)

## Retail mapping

All addresses below refer to `data/Revenant.exe` (Ghidra project
`RevenantDev`), which is the source for every `recon/ghidra/cls_*.cpp`
snapshot. The smaller 1.1 MB pre-retail builds elsewhere in the tree
have different addresses *and* lack the v14+ `statehash` field.

- `TObjectInstance::LoadObject` = `FUN_00471ce0` — confirmed.
- `TSector::Load` = `FUN_00498780`.
- `TSector::Save` = `FUN_00498c90`.
- `TSector::Save` hash helper = `FUN_00499e90` (calls hash-family
  `FUN_0056ff60` init / `FUN_0056ff80` update / `FUN_0049cdd0`
  finalize — signature resembles CRC32 but is unconfirmed; used as a
  change-detection fingerprint, not a cryptographic integrity check).
- OOAnalyzer did not classify TSector as a struct, so none of these
  appear in `recon/ghidra/cls_*.cpp` — they live only in
  `recon/ghidra/_data.txt`.
