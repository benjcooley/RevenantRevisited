# Object model surgery: TNode → TObjectInstance → TSector

Status: phases 1-3 and 3b landed 2026-10-18 (branch feature/objectmodel, PR #19); 4 and 5 open. Companion to docs/OBJECT_SYSTEM.md (current state) and
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

Before the surgery:

```
class TObjectInstance : public TNodeInst, protected SObjectDef
```

`SObjectDef` held `objclass, objtype, flags, state, level, pos, vel, accum,
rotatex, rotatey, rotatez|facing, group`. The instance ctor did
`memcpy(&objclass, def, sizeof(SObjectDef))` and then re-synced the
transform from the copied ints.

### 1.1 The transform was stored three times

| Store | Type | Who writes it | Who reads it |
|---|---|---|---|
| `SObjectDef::pos` | `S3DPoint` (int wu) | 82 direct writes (`oi->pos.x = ...`), 38 of them in mappane.cpp; SetPos/ForcePos via `WriteTransformPos` | 94 bare reads in object/character/player/effect; sector membership, ordering, collision |
| `rotatex/y/z` (`facing`) | `uint8_t` 0..255 | SetRotateX/Y/Z, SetFacing, retail Load | 47 sites (facing for AI/anim, rotate for props); `SyncTransformRot` rebuilds the quat |
| `TNode::transform_` | `TTransform` (float pos, quat+euler rot, scale, cached local/world matrices, parent/children) | kept in lockstep by `WriteTransformPos` and `SyncTransformRot` | **the whole 3D path**: owner matrix `inst->Transform().Matrix()` (3dimage.cpp:4139), root bones parent to `inst->Transform()`, `animobj->transform` is a `TTransform` (3dimage.cpp:2579), `RefreshHierarchy` (3dimage.cpp:4999), prefab stamps, particle anchors |

So the 3D engine already consumed `TTransform` only. The integer `pos` and
the rotate bytes were the duplicates, kept alive by sync shims and by the
scatter of direct field access. The sync points were the risk: every direct
`pos.x =` write that skipped `WriteTransformPos` left the mesh somewhere else
than the object.

### 1.2 What is gameplay state, not transform

`vel` and `accum` are retail's integer sub-unit movement accumulator
(`MoveObject`, object.cpp:1684). They are not spatial truth, they are the
movement integrator's carry. They stay integer gameplay fields on the instance.

`facing` is a gameplay quantity (8-bit heading used by AI, animation state
selection, retail scripts) that happens to equal rotatez. It stays an accessor
but is derived from the transform.

## 2. Target (landed)

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

- **One spatial store.** `transform_` is authoritative. The integer views
  are free functions in `intworld.h`, named for why they exist: `IntPos`,
  `SetIntPos`, `IntFacing`, `SetIntFacing`, `IntRotateX/Y`,
  `SetIntRotateX/Y`. They are expressions over the transform, not class
  methods, and take a node by reference, pointer, `unique_ptr` or
  `TSafeRef`. Floats hold every integer below 2^24 exactly, so integer
  writes round-trip; the transform keeps the last-set euler verbatim, so
  0..255 angles round-trip too. `TObjectInstance::SetPos` stays as the map
  move (sector, walkmap, shadow, light side effects) and `Face(n)` as the
  facing-plus-move-angle verb; both write the transform.
- **Sector = roots.** `TSector::objects` stays a set of `TObjectInstance*`.
  Child `TNode`s hang off their object's transform and are owned by the
  component that stamped them (prefab) or by the object; they are never
  sector members, never have a map index, never saved by the retail codec.
- **SObjectDef = spawn record.** Plain struct with initializers, same
  fields, no inheritance. The ctor applies it through the setters and lets
  it go. Phase 5 replaces it with spawning from a packed message and deletes
  the struct.
- **Reflection covers the persistent state.** `TNode` owns ids 1-5 (name,
  pos, rot, scl, active). `TObjectInstance` owns 10+: class, type, state,
  frame, mapindex, flags, level, group, vel, accum. lightdef, inventory and
  stats come with the packed sector format.

## 3. Three codecs, one state

| Codec | Module | Purpose | Driven by |
|---|---|---|---|
| Retail stream | `legacystream.{h,cpp}`: `TLegacyDeserialize` / `TLegacySerialize` | Load retail sectors; read/write retail-compatible saves | hand-written, byte-exact; a friend of each streamed class with one method per class (`ReadObjectInstance`, `ReadCharacter`, ...), dispatched by C++ type most-derived first; the object classes have no stream methods |
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

## 4. Phases

### Phases 1-3 (landed 2026-10-18, one PR)

Done as one bullet rather than three inches, at the author's call:

- `TNodeInst` renamed `TNode` (node.{h,cpp}).
- `SObjectDef` demoted to a spawn record; its fields are instance members;
  the ctor applies the record.
- Legacy `pos` and rotate bytes deleted, with `WriteTransformPos` and
  `SyncTransformRot`. 935 call sites moved to the `intworld.h` views
  (mechanical regex pass, then compiler-driven fixes for the bare member
  reads inside the hierarchy).
- The retail stream moved to `legacystream.{h,cpp}`. Every per-class
  `Load`/`Save` override (object, complex object, character, player,
  container, vial rack, exit, ammo, money, food, scroll, weapon, drip
  effect, monster generator, automap record) is a method of the two codec
  classes; the `ObjVersion` virtuals became four version constants.
- Reflection props 15-19 (flags, level, group, vel, accum). The generator
  learned `uint32_t` and `S3DPoint` (Vec3 lanes, rounded on set).

Gate: `--test=sector-dump --level=L --dump-out=DIR` re-serializes a level
through the codec (`TSector::Serialize`); `tools/sector_gate.sh` runs every
level. Before/after on the retail module: 4896 sectors, 23 MB, byte
identical except:

- 1_4_19.DAT, one object (class 9, uid 0x926400cc) whose stored z is
  0xAAAAAAAA, retail garbage beyond float's exact integer range; it comes
  back rounded by 42 units. Harmless: both values are garbage.
- Two NPCs (karl1 in 1_4_19, CAMERON1 in 1_7_11) whose `state` byte differs
  between any two runs of the same binary: a random idle pick at load, not
  the surgery.

Also green: unit tests, tools/test_transient_map.py (stubs updated),
headless quickstart.

### Phase 3b: the object is a component of its node (landed 2026-10-18)

The question "can TObjectInstance be sealed" had a better answer than
dicing the class tree: seal the node instead.

```
class TNode final                                // name, id, transform, children, components,
                                                 // map membership (map index, sector, level)
class TObjectInstance : public TComponent  // root of the class hierarchy, unchanged below it
```

- A map object is a root node in a sector whose first component is a
  `TObjectInstance` (or a subclass). The node owns the instance like any
  component; `node->Object()` and `instance->Node()` are cached, never
  searched. Exactly one object component per node, attached first,
  destroyed last.
- The instance keeps its whole API. The node-facing part (transform, map
  index, sector, level, components) is one-line forwarders to its node, so
  the ~1500 `TObjectInstance*` sites across the codebase do not change:
  measured, two thirds of them are other systems asking class, id, state,
  name, map index, flags or sector, and the instance still answers.
- What changes is ownership: `delete inst` becomes `DeleteObject(inst)`
  (deletes the node); holders use `TObjectPtr<T>` (unique_ptr with the
  node-deleting deleter); the two stack-allocated fixtures go on the heap.
- Containers (sector lists, the map index registry, inventories, safe
  refs) keep instance pointers for now; the node is the owner. Switching
  the sector list to root nodes is a later inch, when the packed format
  needs it, and the safe-ref id unification with it.
- Inventory later: a contained item is a child node owned through the
  tree, the inventory component is the policy over those children.
  Transform parenting of map objects is a capability, not a goal (retail
  draws equipment its own way; an elevator is a maybe).
- Sector rules: a node with a sector is in the map, one without is not; a
  whole tree shares one sector (`TNode::SetSector` recurses, a child adopts
  its parent's on attach); only a root may be on a sector's object list,
  which is what the map pulses and draws. `TObjectInstance::IsSectorMember`
  is that test; `GetSector` alone means "in the map", directly or through
  the root. An object can sit at any depth: a contained item is in the map
  through its chest and is never pulsed by the map.
- Landed as: `TNode final` with `Object()` cached at attach through one
  virtual on the component base (`AsObjectInstance`); the node destroys
  its object first and the rest after, `DetachComponents` re-entrant;
  `AdoptNode` in every object constructor; `DeleteObject`, `SObjectDeleter`,
  `TObjectPtr<T>`; the inspector draws the node then each component, undo
  snapshots node plus components, the clipboard writes node then object
  props. Gates as for phases 1-3: unit tests, sector dump (same two
  known diffs), quickstart, editor boot, VFX harness, AddressSanitizer on
  the quickstart and a sector dump.

### Phase 3c: the light is a component (landed 2026-10-18)

`TLightComponent` (game/world/lightcomponent.{h,cpp}, `DEF_COMPONENT("light")`)
replaces the `SLightDef` every object carried. A map object has one exactly
when `OF_LIGHT` is set: `ResetFlags` adds or removes it, the flag is what the
sector stream records, and the stream reads and writes the component's
fields (retail's types, so the bytes are unchanged). The object keeps a soft
ref to it (`Light()`, `EnsureLight()`) and nothing else: the map pane, the
renderer, the debug UI, the script commands and the effects all talk to the
component. The component owns the two runtime handles, the 3D scene light
(entered and left with the owner's on-screen state) and the DLS light-table
id (acquired when drawn, released off screen), and the move / colour /
radius / multiplier setters keep both in step as retail's object setters did.
A prefab node can carry one too. Props: offset, color, intensity,
multiplier, flags.

### Phase 3d: render components (next)

A `TRenderComponent` base (engine) that the renderer submits through, with
the sprite (2D imagery) and mesh (I3D) components deriving from it; the map
renderer walks sector roots and asks each node's render components to
submit instead of switching on imagery type. This is what unparks the
imagery and lighting files listed in docs/SOURCE_LAYOUT.md.

### Phase 4: native packed sector format

- `sectorpack.{h,cpp}`: `Pack(const TSector&)` / `Unpack` over nodepack.
  Reflection props added for the rest of the persistent state (lightdef;
  inventory and stats as nested lists).
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
- 2026-10-18 The integer views are free functions with an `Int` prefix
  (`IntPos`, `IntFacing`), not class methods: the legacy unit is visible at
  the call site and the class API stays clean (author's direction).
- 2026-10-18 The retail stream is `TLegacyDeserialize` / `TLegacySerialize`,
  friends of the streamed classes with a method per class, dispatched on the
  C++ type (author's direction). Builders register per type name, so the
  dispatch cannot key on the object class.
- 2026-10-18 Euler order: `TTransform` composes X, then Y, then Z; retail's
  root matrix composed Z, X, Y. The renderer's root matrix still composes
  from the integer views in retail order (maprenderer `BuildRootMatrixSource`),
  so draws are unchanged. Only the bone-root path that reads
  `Transform().Matrix()` would see a difference, and only for an object with
  two or more non-zero rotate bytes. To be checked on the sector scan before
  phase 4; if any exist, the fix is a TTransform rotation order, not a
  second rotation store.
- 2026-10-18 Sectors are not nodes. The world is a streaming grid of
  sectors, each holding root nodes; there is no scene graph above a root.
  An arbitrary 3D scene system is one step too far for this game.
- 2026-10-18 One PR for phases 1-3 ("bite the bullet"), gated by the
  before/after sector dump rather than file-vs-save identity, because the
  port's writer already diverges from retail's bytes on purpose (inventory
  counts, SAVE_GAME.md §11.1).
