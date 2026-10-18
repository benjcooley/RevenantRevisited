# Object model surgery: TNode → TObjectInstance → TSector

Status: plan, 2026-10-18. Companion to docs/OBJECT_SYSTEM.md (current state) and
docs/vfx/PREFAB_SYSTEM.md (reflection, packed codec, prefabs). This document is
the source of truth for the surgery; OBJECT_SYSTEM.md is updated as each phase
lands.

## 0. Intent

Retroactively write the object model we should have had in 1998:

- The core hierarchy is `TNode` → `TObjectInstance`. A sector is a set of
  root `TObjectInstance`s; each may own a subtree of plain `TNode`s
  (prefab stamps, attachment points, effect nodes).
- `SObjectDef` stops being a base class. It is demoted to a spawn record
  that a caller fills and the instance *applies*. The DEF is a serialization
  mechanism, not the core structure.
- There is one object state, held by the instance. The retail sector/save
  stream, the DEF text format and the packed binary are three codecs over
  that one state, all driven by the same reflection tables.
- The retail stream stays byte-exact as an import/export codec because port
  saves must load in retail and vice versa (docs/SAVE_GAME.md). The packed
  binary becomes the native sector format for Revisited content.

## 1. Where we are

```
class TObjectInstance : public TNode, protected SObjectDef
```

`SObjectDef` (object.h:657) holds `objclass, objtype, flags, state, level,
pos, vel, accum, rotatex, rotatey, rotatez|facing, group`. The instance ctor
does `memcpy(&objclass, def, sizeof(SObjectDef))` (object.cpp:557) and then
re-syncs the transform from the copied ints.

### 1.1 The transform is stored three times

| Store | Type | Who writes it | Who reads it |
|---|---|---|---|
| `SObjectDef::pos` | `S3DPoint` (int wu) | 82 direct writes (`oi->pos.x = ...`), 38 of them in mappane.cpp; SetPos/ForcePos via `WriteTransformPos` | 94 bare reads in object/character/player/effect; sector membership, ordering, collision |
| `rotatex/y/z` (`facing`) | `uint8_t` 0..255 | SetRotateX/Y/Z, SetFacing, retail Load | 47 sites (facing for AI/anim, rotate for props); `SyncTransformRot` rebuilds the quat |
| `TNode::transform_` | `TTransform` (float pos, quat+euler rot, scale, cached local/world matrices, parent/children) | kept in lockstep by `WriteTransformPos` and `SyncTransformRot` | **the whole 3D path**: owner matrix `inst->Transform().Matrix()` (3dimage.cpp:4139), root bones parent to `inst->Transform()`, `animobj->transform` is a `TTransform` (3dimage.cpp:2579), `RefreshHierarchy` (3dimage.cpp:4999), prefab stamps, particle anchors |

So yes: the 3D engine already consumes `TTransform` only. The integer `pos`
and the rotate bytes are the duplicates, kept alive by sync shims and by the
scatter of direct field access. The sync points are the risk: every direct
`pos.x =` write that skips `WriteTransformPos` leaves the mesh somewhere else
than the object.

### 1.2 What is gameplay state, not transform

`vel` and `accum` are retail's integer sub-unit movement accumulator
(`MoveObject`, object.cpp:1684). They are not spatial truth, they are the
movement integrator's carry. They stay integer gameplay fields on the instance.

`facing` is a gameplay quantity (8-bit heading used by AI, animation state
selection, retail scripts) that happens to equal rotatez. It stays an accessor
but is derived from the transform.

## 2. Target

```
class TNode                      // name, soft-ref id, TTransform, children, components
class TObjectInstance : public TNode
{
    // identity / registry
    TObjectClass*   cl;   SObjectInfo* inf;   TObjectImagery* imagery;
    int32_t         mapindex;   TSector* sector;
    // gameplay state (retail fields, now members)
    uint32_t  flags;      uint16_t state;   uint16_t level;   uint8_t group;
    S3DPoint  vel, accum;                      // movement integrator
    // systems: inventory, script, stats, lightdef, components (unchanged)
};
```

- **One spatial store.** `transform_` is authoritative. `GetPos()` returns the
  rounded integer world position (floats hold every integer below 2^24
  exactly, world coords are far below that, so integer writes round-trip
  exactly). `SetPos`/`ForcePos` write the transform. The rotate bytes become
  views over the transform's euler (`TTransform` keeps the last-set euler
  verbatim, so 0..255 → radians → 0..255 is lossless). `SetFacing(n)` sets
  euler z only.
- **Sector = roots.** `TSector::objects` stays a set of `TObjectInstance*`.
  Child `TNode`s hang off their object's transform and are owned by the
  component that stamped them (prefab) or by the object; they are never
  sector members, never have a map index, never saved by the retail codec.
- **SObjectDef = spawn record.** Plain struct, same fields, no inheritance.
  `TObjectInstance(const SObjectDef&, TObjectImagery*)` applies it through
  the setters. Later it is replaced by spawning from a packed message or from
  named reflection sets (§4, phase 5), and the struct is deleted.
- **Reflection covers the persistent state.** `TNode` owns ids 1-5 (name,
  pos, rot, scl, active). `TObjectInstance` owns 10+: class/type (names),
  state, frame, mapindex already exist; flags, level, group, vel, accum,
  lightdef, inventory and stats are added as the codecs below need them.

## 3. Three codecs, one state

| Codec | Module | Purpose | Driven by |
|---|---|---|---|
| Retail stream | `objectstream.{h,cpp}` (split out of object.cpp Load/Save 2637/2823) | Load retail sectors; read/write retail-compatible saves | hand-written, byte-exact, reads/writes through accessors (no memcpy of the class) |
| DEF text | `defio` | Editor round trips, prefabs, diffs of sector content | reflection |
| Packed binary | `nodepack` | Native Revisited sector format; prefab bake; editor undo | reflection |

The native sector file is a packed message: a header (format id, level,
sector x/y, version) followed by one `nodepack` node message per root object.
A node message is what nodepack already writes: own props, components
(`kTagComponent`), children (`kTagChild`). Object class/type travel as names
(props 10/11) so the file does not depend on class.def ordering.

Retail interop rule: a module whose sectors are retail files keeps the retail
stream as its save codec (hash-delta saves need the retail bytes). The packed
format is for content authored or overlaid by Revisited (revisited/ overlay,
editor output). A sector loaded from retail and saved by the editor goes to
the packed format under the overlay; the retail file is never modified
(docs/DATA_LAYOUT.md distribution rule).

## 4. Phases (inches)

Each phase is one PR, builds green on all three CI targets, and passes the
byte-identity test introduced in phase 1 before anything else changes.

### Phase 1: remove the base class, zero behaviour change

- `SObjectDef` fields become `TObjectInstance` members (same names, same
  types, private; `REFL_FRIEND` where the generator needs it).
- Ctor applies the record field by field through the existing setters
  instead of `memcpy`; `SyncTransformRot` / `WriteTransformPos` stay.
- `TSpell::NewEffect(SObjectDef&)`, `NewStrikeEffect`, builders, `NewObject`
  keep their signatures. The 23 fill sites are untouched.
- Test: load every sector of the retail module through `TSector::Load`,
  `Save` to memory, byte-compare against the file (also a retail
  `game.sav` round trip through the existing save test). Register in CMake.
- Blast radius: object.h/.cpp only. No code outside the class can see the
  protected base today, so nothing else changes.

### Phase 2: one transform store

- Delete the legacy `pos` field. 82 direct writes become `SetPos`/`ForcePos`
  (mappane.cpp first, one site at a time); 94 bare reads become `GetPos()`.
- `rotatex/y/z` deleted; accessors derive from `transform_` euler;
  `SyncTransformRot` and `WriteTransformPos` go away.
- Verify: phase 1 byte-identity test still passes; Shadow Smoke playthrough
  and the retail A/B emulator (positions are integer-exact so these must be
  bit-identical).

### Phase 3: retail stream becomes a codec module

- `TObjectInstance::Load/Save` move to `objectstream.cpp` as free functions
  `ReadRetailObject(stream, TObjectInstance&)` / `WriteRetailObject`. They
  read and write through accessors. sector.cpp `LoadObject/SaveObject` call
  them. Same for the save-game path.
- Nothing about the bytes changes; the byte-identity test is the gate.

### Phase 4: native packed sector format

- `sectorpack.{h,cpp}`: `Pack(const TSector&)` / `Unpack` over nodepack.
  Reflection props added for the persistent gameplay fields (flags, level,
  group, vel, accum, lightdef; inventory and stats as nested lists).
- Loader: `TSector::LoadSector` tries the overlay packed file, then the
  retail file. Editor saves packed. Import tool: retail module → packed, for
  the test that proves retail → packed → instances is state-identical to
  retail → instances (compare via reflection `Get` over every prop).
- DEF text of a sector via defio for review diffs (not a load path).

### Phase 5: spawn API, retire SObjectDef

- `TObjectClass::NewObject(class, type, const SSpawn&)` or spawn from a
  packed message (prefab-style). Builders take the instance-to-be or the
  message; the 135 derived ctors change in one mechanical pass once the
  record's consumers are gone.
- The 23 `SObjectDef def; memset` fill sites migrate one at a time to the new
  call. Delete `SObjectDef`.

Phases 1-3 are mechanical and low risk; 4 is the new work; 5 is cleanup that
waits until 4 has proven the packed format on real sectors.

## 5. Out of scope here

- Prefab stamping in game (docs/vfx/PREFAB_SYSTEM.md §6).
- Inventory/stats/script reflection beyond what phase 4 needs.
- Changing retail semantics of flags/state/facing (combat divergence policy).

## 6. Decision log

- 2026-10-18 Sector stays a flat set of root objects; node subtrees are
  never sector members. Keeps retail sector semantics (ordering, objsets,
  flags-changed bookkeeping) untouched.
- 2026-10-18 Retail stream kept byte-exact as a codec, not deleted. Save
  interop with retail is a hard requirement.
- 2026-10-18 `vel`/`accum` remain integer gameplay fields; they are the
  movement integrator's carry, not a second transform.
- 2026-10-18 Rotation is derived from the transform euler, not stored as
  bytes. Lossless both ways; the 8-bit facing remains a gameplay accessor.
