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

### v14+ (confirmed 2026-04-17 via FUN_00471ce0 decomp)

```
int16   objversion
int16   objclass          // -1 = empty slot (skip)
uint32  uniqueid
int16   blocksize         // body size only
int16   invblocksize      // inventory body size, 0 = no inventory
... body ...              // blocksize bytes
... inventory body ...    // invblocksize bytes (only when invblocksize > 0)
```

Key retail behavior from FUN_00471ce0 LAB_00471fa3:
- When `version > 13 && invblocksize < 1`, LoadInventory is **not called**
  at all (the vtable dispatch at `*vtbl+0x168` is skipped).
- When invblocksize > 0, LoadInventory runs, and the stream is re-synced
  to `invstart + invblocksize` afterward.

The stream position is re-synced via `start + blocksize (+ invblocksize)`
after each object, so a single unknown/corrupt object is non-fatal.

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
