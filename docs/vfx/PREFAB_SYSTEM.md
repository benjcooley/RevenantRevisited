# VFX Prefab System — design

Status: **design, agreed in discussion 2026-10-16**. Supersedes the per-effect
"shim port" direction for everything that is not gameplay coupled. The engine
architecture it builds on is [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md);
the operational burndown stays in [INVENTORY.md](INVENTORY.md).

## 0. Goal in one paragraph

Every visual effect becomes a **prefab**: a named tree of nodes with
components, stamped into the scene by name. The visual parts (meshes,
billboards, emitters, lights, strips) are data. Most effects need **no
behaviour code at all**; the rest need geometry animation; a few need
gameplay coupling. In order of how often we expect each:

1. **Emitters only.** The effect is one or more particle emitters (the
   existing bucket schema with spawn / tick / kill expressions). Nothing
   ticks it but the particle engine. The majority of the inventory.
2. **Geometry animation + emitters.** Meshes and billboards move by
   either the I3D's **authored animation states** (played by the existing
   pose pipeline) or by **tracks** on node and component channels: a scale
   envelope, a spin, a fade, a UV scroll, a flipbook. Data, not code.
3. **An effect component**: a small C++ class pulsed each frame. Gameplay
   coupling (steering, impact transitions, caller parameters) always lives
   here, and so does any behaviour that is simpler as twenty lines of code
   than as a clip.

The judgment rule: **if we would hand-code it for simplicity in a new game
in 2026, leave it hand-coded.** A three-state missile, a one-off shatter,
a glow that tracks a target: code. A scale envelope reused across effects,
anything an artist should be able to retune, anything the editor should
show as a curve: tracks. Particle emitters are always data.
The result is tweakable from data files, A/B-able in `--test=vfx`, and
stamped in-game through the retail `EffectBurst` / `EFFECT` object-type
path without touching callers.

## 1. Base split: `TNodeInst`

Prefab nodes must not be map objects: nothing in a prefab should be sector
resident, saved, pulsed by MapPane, picked, or listed by the editor. The
fix is a base class that holds exactly what a node needs, with
`TObjectInstance` as the map-shaped specialization.

```
TNodeInst  : TSafeObjectBase<TNodeInst>
    name            std::string
    transform_      TTransform                 (parent / children / local pos, rot, scl / world matrix)
    components      std::vector<std::unique_ptr<TObjectComponent>>
    Parent(), GetChildCount(), GetChildAt(i)   -> TNodeInst*   (via TTransform owner back-pointer)
    FindChild(name), FindPath("a/b/c")
    AddComponent / RemoveComponent / ActivateComponents / GetComponent<T>()
    virtual TObjectInstance* AsObjectInstance()   { return nullptr; }
    property channels (see §2): LocalPosX..Z, LocalRotX..Z, LocalSclX..Z, Active

TObjectInstance : TNodeInst
    everything else: mapindex, sector, class/type info, imagery + frame state,
    inventory, script, lightdef, stats, movement, oldpos / screen cache
    AsObjectInstance() override { return this; }
```

Measured sizes (2026-10-16, arm64): `TObjectInstance` 560 B of which
`TTransform` 224 B. Memory was never the problem; **membership** was.

Rules of the split:

- **Component owner is the node.** `TObjectComponent::Owner()` returns
  `TNodeInst*`. Components that need the map object (today: `TObjectAnimator`,
  `TFlipbookBillboardComponent`, `TParticleEffectComponent`) call
  `Owner()->AsObjectInstance()` and handle null. Three subclasses exist, about
  eight `Owner()` call sites.
- **Soft refs.** `TNodeInst` inherits `TSafeObjectBase<TNodeInst>` (own
  registry, own id). `TObjectInstance` keeps **mapindex** as its
  `TSafeRef<TObjectInstance>` id, hiding the base contract, because existing
  code compares `TSafeRef::MapIndex()` against map indices (particle runtime
  lookup, editor selection set). Two id spaces, 12 extra bytes per object,
  zero churn on existing refs. Unifying them later is registry-internal.
- **`TSafeComponentRef<T>`** holds `TSafeRef<TNodeInst>` + slot + generation
  and moves from object.h to nodeinst.h next to the node definition. The
  component update list (`g_component_updates`, object.cpp:86-420) moves with
  it.
- **`TTransform` gets an owner back-pointer** (`TNodeInst*`, set by the node
  constructor) so child traversal by name and path resolution work from the
  transform hierarchy. Hierarchy links stay non-owning, as they are today.
- **Ownership of nodes** stays with the creator. A stamped prefab owns its
  child node instances and destroys them with the root effect.

Files: `src/nodeinst.{h,cpp}` (new), `src/objectcomponent.{h,cpp}`,
`src/object.{h,cpp}`, `src/transform.{h,cpp}`, `src/saferef.h` comment.
`tools/test_transform.cpp` gains node-tree + path-resolution tests.

## 2. Reflection: C++26 annotations, polyfilled by a generator

**Landed 2026-10-16** as `src/refl.{h,cpp}` + `tools/reflect/gen.py`;
tests in `tools/test_props.cpp`.

Reflection is its own module. A class that wants tweakable, serializable
or animatable state annotates members and nothing else: no base class,
no declared overrides, no enum, no switch.

```cpp
class TMeshComponent : public TObjectComponent
{
    DEF_COMPONENT("mesh")                             // block keyword + factory
    PROP(1, "Draw this mesh")       bool        visible   = true;
    PROP(2, "Material colour")      SRgba       diffuse   = {1, 1, 1, 1};
    PROP_NOSAVE(3)                  float       uv_scroll = 0.0f;
    PROP_FN(4, hmm_vec3, pos, LocalPos, SetLocalPos, "Local position");
    void PropChanged(uint16_t id);                  // optional hook
};
```

`PROP` is the C++26 member annotation (P2996 reflection + P3394
annotations, both in the C++26 draft) and expands to nothing until Apple
Clang and MSVC read them (GCC 16 and Bloomberg's Clang fork already do).
Until then `tools/reflect/gen.py`, run by CMake (`rev_reflect(<target>
<headers>)`), emits into the build tree exactly what reflection would
give us, **outside the class**: a `refl::SType` (property table + typed
get/set thunks) registered under the class's `type_info`, plus name
tables for any `enum class` an annotated member uses. When the compiler
catches up the generator is deleted and no header changes.

**The property record** is id, name, description, type, three booleans and
a default:

| Field | Source |
|---|---|
| `id` | the integer in the annotation; also the wire tag (append-only, never reused) |
| `name` | the member identifier (trailing `_` dropped) — the DEF key |
| `desc` | the quoted string in the annotation, else the declaration's trailing `//` comment (the house style for documenting a member) |
| `type` | closed map: bool, int32_t, float, std::string, hmm_vec3, SRgba, `enum class` declared in the same header; plus nested records: another annotated class (`Struct`, a `key { }` block) or `std::vector` of one (`List`, repeated `key [name] { }` blocks whose label fills the record's `name`) |
| `serialized` / `visible_in_editor` / `read_only` | true / true / false by default; the rare cases are separate macros `PROP_NOSAVE`, `PROP_HIDDEN`, `PROP_READONLY` (and `PROP_FN_NOSAVE` / `PROP_FN_HIDDEN`; a `PROP_FN` with no setter is read-only) |
| `def` | the member initializer, which must be a **simple literal** (number, `true`/`false`, string literal, brace list of numbers, `EType::Enumerator`); named constants, macros and expressions are rejected at generation. Accessor-backed properties state it with `def(literal)` |

Rules that keep it honest:

- **The parser stays dumb.** It recognises `class` / `struct` (brace on
  the same or next line), `enum class`, `PROP*` lines, `DEF_BLOCK` /
  `DEF_COMPONENT`, and the `PropChanged` hook. A line it cannot parse
  is a build error with file and line, never a guess.
- Fields stay private, as everywhere in the codebase: annotate through
  `PROP_FN` accessors, or annotate the private field directly and add one
  `REFL_FRIEND(ClassName)` line (the generated thunks are a
  `refl::Access<T>` specialization the class befriends; the macro folds
  away under real reflection).
- An accessor-backed property states its default with `def(literal)`;
  a test asserts every default matches a freshly constructed object.
- A derived annotated class sees its base's properties (`SType::base`,
  linked by name on first lookup).
- **Everything else talks to the registry**: `refl::Of<T>()` /
  `refl::Of(obj)` (dynamic type), `refl::FindByDefName("mesh")`,
  `FindProp`, `Set` / `Get`, `SetLane` / `GetLane` (lanes derived from
  the type: `Vec3` → x y z, `Color` → r g b a; bool is 0 / 1 under a
  track), `Pack` / `Unpack`, `Reset` (to defaults), `Equals`.
- **Track / script targets** are `node/path.component.property[.lane]`:
  `"core.mesh.diffuse.a"`, `"ring.scl.z"`. The component segment is
  omitted for node properties; two components of one type on a node are
  told apart by their DEF label (`mesh outer { }`). Resolved at stamp time
  to `SPropertyBinding { TSafeRef<TNodeInst> node; int16_t slot; uint32_t gen; uint16_t id; uint8_t lane; }`;
  a dead node makes the binding a no-op, unknown names are a load-time
  error.
- `TNodeInst` annotates `pos`, `rot` (euler degrees), `scl`, `active`
  (bool; inactive = subtree neither ticked nor drawn) and `name`
  (read-only), and is `DEF_BLOCK("node")`.
- A class may declare `void PostLoad()`: the post-deserialize
  constructor (Unity-style). Loading bypasses normal construction, so a
  loader calls it once the object's block is complete, and attaches a
  component to its node only after that, so `OnAttach` never sees
  default fields.
- The editor inspector, undo and clipboard walk the same records
  (editor.cpp, editorundo.cpp, editorserialize.cpp); `iobject.h` and the
  adapters are gone. `TObjectInstance` annotates class / type / state /
  frame / mapindex and routes the node `pos` / `rot` setters through its
  legacy fields; `TSector` annotates level / sector_x / sector_y /
  num_items.

**Done 2026-10-16:** `src/particledef.h` holds the annotated
`SParticleEffectsFile` / `SParticleEffectDef` / bucket / emitter / output
records (member names are the DEF keys, cooked fields derived in
`PostLoad`, structural checks in `Validate`); effects.def loads through
`defio::ReadObject` and the thirty-key hand parser is gone. A test
round-trips the shipped effects.def: read, validate, write, read,
identical packed bytes. Every other `Parse*Def` of that shape follows
the same recipe.

### 2.5 DEF reader / writer and the packed form

**Landed 2026-10-16** as `src/defio.{h,cpp}` (text), `src/nodepack.{h,cpp}`
(bytes), `src/packed.{h,cpp}` (wire codec); tests in `tools/test_defio.cpp`.
The DEF reader/writer is a separate module that *uses* reflection.

```
   DEF text  (data/Resources/prefabs/*.def; embedded in the shipped binary)
      │  defio::Read — the defdoc lexer/parser streams statements (Sink) straight
      │  into live classes: key = value → setter; `node x { }` → child TNodeInst;
      │  `<component> [label] { }` → DEF_COMPONENT factory + AddComponent.
      │  No document tree exists on this path. Unknown key / block = error with line.
      ▼
   live TNodeInst tree + components
      │  nodepack::Pack — the baked form (one per prefab, held by TPrefabLibrary)
      ▼
   packed bytes          ── nodepack::Unpack on every stamp (cursor + factories)
      ▲
      │  editor: inspector edits the live tree; "Apply to prefab" re-packs;
      │          "Save" writes DEF text with defio::Write (only non-default values)
```

**Document shape.** A document *is* the root's body: top-level statements
are the root node's properties, components and `node` children; the root's
name lives outside (prefab name, file name). `Write` emits exactly that
shape, so write → read is the identity. A flat annotated struct reads with
`defio::ReadObject` (no blocks allowed).

**Wire format** (`packed.h`): a message is a run of fields, each a varint
tag `(id << 2) | wire` and a payload: wire 0 varint (bool, zigzag int,
enum, ref id), 1 fixed32 (float), 2 length-delimited (string, vec3 = 12 B,
color = 16 B, nested message). Unknown ids skip by wire kind; missing ids
keep the C++ default; tags are the only version. A node message is its own
fields, then `kTagComponent` sub-messages (def name, label, fields) and
`kTagChild` sub-messages (name, node message). The reader is a cursor:
`TPackedReader::Next(f)` → id, wire, pointer, size.

## 3. Prefab file format

One `defdoc` block per prefab, in `data/Resources/prefabs/<Name>.def`
(revisited overlay applies per the overlay convention). Same parser and
style as effects.def.

```
prefab IceBolt
{
    imagery = "Magic\\IceBolt.I3D"      // default imagery for mesh / billboard / emitter

    node root                            // implicit; the owning effect instance IS root
    {
        node core
        {
            rot = [90, 0, 0]
            mesh { object = "cyl01"  blend = alpha  lit = false }
        }
        node ring  { mesh { object = "cylinder02"  blend = additive } }
        node frost { emitter { /* bucket fields, see §4.3 */ } }
        node glow  { light { radius = 120  color = [0.6, 0.8, 1.0]  intensity = 1 } }

        animation
        {
            entry = LAUNCH
            state LAUNCH  { clip = launch   next = FLY }
            state FLY     { clip = fly      loop = true }
            state EXPLODE { clip = explode  next = END }
        }
        effect { class = "IceBoltSteer" }   // gameplay coupling only: target, SetState(EXPLODE) on hit
    }

    clip launch
    {
        duration = 16   time_unit = ticks   tick_hz = 24
        track "core.mesh.Visible"  { keys = [[0, 1]] }
        track "core.LocalSclZ"     { interp = linear   keys = [[0, 0.1], [16, 1.0]] }
        track "ring.mesh.DiffuseA" { expr = "0.5 + 0.5 * sin(age * 6.28)" }
    }
}
```

An emitter-only prefab is just `node root { emitter { ... } }` and has no
animation or effect block at all.

Node keys: `pos`, `rot` (euler degrees), `scl`, `active`. `imagery` may be
overridden per component. Everything a track can animate has a static
default in the component block.

## 4. Component catalogue

All data driven except `script`. Properties listed are the animatable
channels; static fields are set once in the block.

### 4.1 `mesh`
One I3D sub-object. Static: `imagery`, `object` (name or index), `blend`
(`alpha` | `additive`), `lit` (bool), `depth` (`test_no_write` |
`test_write`). Draws through `SubmitHelperMesh` with the node world matrix.
Material defaults come from the I3D (`LoadMaterial`), **emissive zeroed** per
PORT_CHECKLIST B.2.

Properties: `Visible`, `DiffuseR/G/B/A`, `EmissiveR/G/B`, `UvOffsetU/V`,
`UvScaleU/V`, `Frame` (atlas cell → UV rect).

### 4.2 `billboard`
One quad. Static: `imagery`, `texture_slot` or `object` (UV sub-rect from
the sub-object, the "atlas-cell gotcha"), `orientation` (`screen` |
`world_xy` | `world_z`), `atlas_cols/rows/frames`, `blend`, `lit`, `size`.
Draws through `SubmitFxParticle` (rotation support) at the node world
position.

Properties: `Visible`, `Width`, `Height`, `Spin` (rad), `ColorR/G/B/A`,
`Frame`, `UvOffsetU/V`.

### 4.3 `emitter`
**This is `TParticleEffectComponent` evolved, not a new type.** One component
= one bucket def; its owner is the node, so the bucket anchor is the node's
world position and the chain/owner id is the node's soft-ref id. The
effects.def `effect { bucket ... }` wrapper goes away: the bucket block
*is* the component block. Exactly today's bucket schema (`SParticleBucketEffectDef`: `spawn_expr`,
`tick_expr`, `kill_expr`, `spawn_burst` | `spawn_count`, `tick_hz`,
`default_life`, `chain_bucket`, blend / light / depth / orientation, atlas,
`random_start_frame`). The bucket's anchor is the **node world position**
and `emit_dir` is the node's world +Y in spawn expressions; `space = local`
makes particles follow the node. Chains resolve to sibling emitters in the
same prefab.

Properties: `Enabled` (bool, gates top-up), `Burst` (trigger: emit
`spawn_burst` again), `ScaleMul`, `ColorR/G/B/A` (tint multiplier applied at
submit).

Engine extensions this migration is expected to surface, added generically
(never per effect): `trail_count` (draw each particle N times stepped along
its velocity: sparks, fountain), per-burst sub-object pick (`objflags`),
`world_z` orientation (vertical flame cards), node-basis `emit_dir`.

### 4.4 `strip`
Ribbon through an ordered list of nodes: `points = ["hilt", "mid", "tip"]`
or `follow_children = true`. Static: `imagery`, `texture_slot`, `blend`,
`uv_swapped`, `v_cells`. Draws through `SubmitFxStrip`.

Properties: `Visible`, `WidthA`, `WidthB`, `ColorAR..A`, `ColorBR..A`,
`UvScroll`, `VCell`.

### 4.5 `light`
Point light into the deferred path (`AddPointLight`). Properties: `Enabled`,
`Radius`, `Intensity`, `ColorR/G/B`.

### 4.6 `i3danim` — authored skeleton playback
Plays an animation state of the prefab's `imagery` through the existing
pose pipeline (`BuildAnimPoseObjectMatrix`, the same path `--dumpgltf`
exports), writing the posed sub-object matrices onto the nodes that declare
`bind = "<sub-object>"`. Static: `state`, `loop`, `rate`. Properties:
`Playing` (bool), `Speed`, `Frame`. Effects whose motion is baked in the I3D
(vortices, swirls, sweeps) use this.

### 4.7 `animation` + clips — keyed geometry animation
The track layer (§5): clips of keyed or expression tracks on node and
component channels, sequenced by a minimal controller. This is how a
cylinder grows over 16 ticks, a glow pulses, a ring expands and fades, a
flipbook steps. Static: `entry`, `state` blocks. Properties: `Speed`;
read-only `Time`, `StateIndex`. C++ API: `SetState(name)`, `State()`.

### 4.8 `effect` — hand-written behaviour
A C++ class derived from `TObjectComponent`, registered by name
(`REGISTER_EFFECT_COMPONENT("IceBolt", TIceBoltEffect)`). Hooks: `OnAttach`
(resolve siblings by node path once, hold `TSafeComponentRef`s),
`OnUpdate` (24 Hz accumulator per WAVE_1_LESSONS §1.4 when transcribing
retail tick logic), `OnDetach`. It reads caller parameters (`Height`,
`Facing`, `TargetX/Y/Z` exposed as its own channels), steers, switches
state, sets channels on sibling meshes / billboards / emitters / lights,
and ends the effect (retail `killobj`). It never submits draws and never
loads assets; the visual components do that. Per the §0 judgment rule,
keep logic here when code is the simpler honest answer.

## 5. Clips and the controller

**Clip** = `duration` + tracks. A track targets one channel and is either
`keys = [[t, v], ...]` with `interp = step | linear | hermite` (hermite =
Catmull-Rom via math3d `Spline`, explicit tangents optional), or
`expr = "..."` evaluated by the particle VM in stateless Expression mode
with `age` = clip seconds, `age01` = clip progress, `time_frame` = clip
ticks. Trigger channels fire when the clip time crosses a key.

**Time** is seconds internally and advances on `TTime::DeltaTime()` (scaled,
frame-rate independent). `time_unit = ticks` + `tick_hz` converts key times
at load so retail constants transcribe 1:1 from the forensics docs.

**Controller**: `entry` state; each state = `clip`, `loop`, `next`. `END`
is terminal and destroys the effect root (retail `killobj`). Gameplay or a
script calls `SetState("EXPLODE")` for event-driven transitions (missile
hit), so FLY is never a timer.

**Per-frame order** for a stamped prefab: `i3danim` pose → controller
advances and evaluates tracks → effect component `OnUpdate` (if any) →
emitter integration (existing `TParticleEffectManager::IntegrateEffect`) →
submissions. No blend trees, no layers, no retargeting.

## 6. Stamping

**Landed 2026-10-17 (first slice):** `src/prefab.{h,cpp}` — `TPrefabLibrary`
bakes `assets/prefabs/<Name>.def` on first use (deserialize with PostLoad
off, serialize to one packed buffer, drop the scratch tree) and
`TPrefabComponent` stamps that buffer under its owner with
`nodepack::Unpack` (root properties untouched; PostLoad runs here).
`TParticleEffectComponent` is the `particles` DEF component with its
effect block inline (`src/particlecomponent.h`); the particle runtime is
keyed by node id and anchored at the node's world position, so it serves
map objects and prefab nodes alike. The map walk pulses stamped nodes
with their root; the harness registers `Prefab:<Name>` per file and
drives pulse + submit. `assets/prefabs/Mist.def` and `Blood.def` stamp and
render frame for frame like their effects.def entries. Found and fixed
on the way: the particle manager stored global buckets by value, so a
second bucket's creation moved the first and left runtimes with dangling
pointers. Still to do from this section: the in-game type-name hook,
the `SubmitFx` / `SubmitWorld` component virtuals.

- **`TPrefabLibrary`** parses every `prefabs/*.def` once (boot or first
  use), validates all track targets against the component tables, and
  exposes `Find(name)`.
- **`TPrefabComponent`** on the root `TObjectInstance` (a `TEffect`): on
  `OnAttach` it stamps the node tree (root node = the owner instance, so
  root transform = effect pos + facing), binds clips, builds the controller
  and script. `OnDetach` tears everything down. It owns the child
  `TNodeInst`s.
- **In game**: `T3DImagery::AttachAnimatorComponents` (3dimage.cpp:2046)
  already resolves by `oi->GetTypeName()` with class-name fallback. The
  prefab lookup goes first with the same name; on a hit it attaches
  `TPrefabComponent` and skips the legacy builder. `TCharacter::EffectBurst`
  and every other `OBJCLASS_EFFECT` `NewObject` caller stay untouched.
  Caller-supplied parameters (blood height / angle / spread) become effect
  component or emitter channels set right after spawn.
- **Harness**: every prefab in the library auto-registers as a
  `--test=vfx` entry (`Prefab:<Name>`), sharing the existing preview styles
  and rig anchors. During migration the shim keeps its entry so
  `tools/vfx/snap_ab.py` pairs them.
- **Rendering hooks**: `TObjectComponent` gains two optional virtuals,
  `SubmitFx(TRenderer&)` (fx queue: billboards, buckets, strips, lights)
  and `SubmitWorld(TRenderer&)` (inside the tile pass: helper meshes).
  maprenderer calls them for every component of every visible instance at
  the right pass, replacing today's `GetComponent<TParticleEffectComponent>`
  / `<TFlipbookBillboardComponent>` special cases; `TPrefabComponent`
  forwards into its node tree. Mesh submissions from the fx walk are a
  programming error and are asserted.

## 7. Migration

Rule: port the prefab, A/B it against the shim with the existing capture
tools until it matches, **then delete the shim** (and its `_Bespoke` class
and harness entry). `d3dport.{h,cpp}` goes when its last user does. Old
snapshot bodies stay in `effect_old.cpp` as reference.

| Effect (shim / bespoke) | Prefab archetype | Needs |
|---|---|---|
| Sparks | emitter only (burst params from gameplay) | **done 2026-10-17**: `assets/prefabs/Sparks.def`, matches the engine port in the harness; needed `spawn_delay_*`, `life_max`, `trail_count` / `trail_gravity`, `burst_frame` (all generic bucket fields) and a VM fix (swizzle reads are a dedicated `LoadLane` op). Shim + `TSparkEffect` go when the in-game hook stamps prefabs. |
| Fountain (+4 colour variants) | emitter only | `trail_count` |
| Fizzle | emitter ×3 (world_xy, per-particle spin) | — |
| Blood, Mist, TorchFlame (effects.def) | emitter / billboard | move blocks into prefab files |
| Flame | billboard flipbook | `world_z` orientation |
| FaultFire | billboard + clip (UV scroll expr) | — |
| Burn | emitter (+ clip for the rise envelope if not expressible per particle) | — |
| FireSwarm | mesh + i3danim (or clip) | — |
| Iced | mesh ×N + clip states (HOLD→SHATTER→TUMBLE) | — |
| IceBolt | mesh composite + emitter + clip | — |
| FireBall | mesh + emitter + light + clip states + effect (steer, impact) | effect |
| FireWind / FireFlash | mesh + billboard + clip | — |
| LightStrip | billboard + emitter + strip + clip (strand pick via rand expr) + effect (target) | effect |
| Teleporter, Vortex-type | mesh + i3danim | i3danim |
| Aura, Heal, Shield, SymGlow, Flare, Photon, Pixie, Fog, Sandswirl, Shock, Streamer, Ribbon, Water, WaterFall, MeteorStorm, Halo, Ripple, Drip, WeaponSwipe | per forensics; emitter / mesh / billboard + clip | TBD per row |
| FireColumn, FlameDisc, others on the don't-port list | — | skipped (asset missing / vestigial) |

INVENTORY.md gains a `Prefab` status column; a row is done when the prefab
file exists, the A/B passes, and the shim is gone.

## 8. Phases

1. **PR: reflection + DEF reader/writer + node split** (landed
   2026-10-16, feature/vfx). `tools/reflect/gen.py` + CMake hook, `refl`,
   `packed`, `defio`, `nodepack`, defdoc parser turned into a Sink stream,
   `TNodeInst` split with annotated node properties; gtests for each.
   Follow-up in the same PR: annotate `TObjectInstance` / `TSector`,
   move the editor inspector, undo and clipboard onto `refl`, delete
   `iobject.h` and the adapters.
2. **PR: core + two archetypes.** Generic DEF → packed bake, packed
   reader/writer, prefab library (bake at startup, decode on stamp),
   `TPrefabComponent`, node / `mesh` / `billboard` / `light`
   components, `emitter` (the evolved `TParticleEffectComponent`, node
   anchored, one bucket each), clips + minimal controller, harness
   auto-registration, render hooks. Prefabs: **Sparks** (emitter only) and
   **Iced** (meshes + clip states). effects.def blocks (Blood, Mist,
   TorchFlame) move into prefab files. Shims deleted when A/B passes.
3. **PR: i3danim + strip + effect registry.** Skeleton playback; `strip`;
   the effect-component registry for gameplay coupling. Prefabs:
   **FireBall**, **LightStrip**, **Teleporter**.
4. **Fan-out.** Remaining rows in parallel, one prefab per agent, same A/B
   gate. Delete `d3dport`, the `_Bespoke` classes and the shim harness
   entries as they empty.

## 9. Decision log

- 2026-10-16 Nodes are not `TObjectInstance`s; base split to `TNodeInst`
  (name, soft-ref id, transform + children, components). Components are
  behaviours on a node, never nodes.
- 2026-10-16 `TObjectInstance` keeps mapindex as its soft-ref id; node
  registry is separate for now.
- 2026-10-16 Properties: C++26 annotations on members (`PROP(id)`),
  polyfilled by a dumb build-time generator that emits the table +
  Set/Get/Pack/Unpack switches until Apple Clang / MSVC read annotations;
  then the generator is dropped and no header changes. Id = wire tag,
  append-only.
- 2026-10-16 Packed buffers: DEF text baked once at startup into a
  protobuf-style byte buffer per prefab; spawns decode with a cursor
  (`Next() → id + pointer`); editor edits live and writes DEF back.
  Lanes derived from the property type, bool = 0/1, triggers rising-edge,
  name → (id, lane) resolved at stamp time through soft refs.
- 2026-10-16 Reflection is its own module (`refl`); the DEF reader/writer
  (`defio`) uses it, loading straight into classes with no document
  tree; defdoc's parser streams to sinks. Property record = id, name,
  description, type, serialized / visibleInEditor / readOnly, default
  (literal from the initializer). Rare cases are separate macros.
  `PostLoad()` is the post-deserialize constructor. The old editor
  reflection (`iobject.h`, adapters) is deleted.
- 2026-10-16 Emitters are never hand coded. The emitter is a component
  (`TParticleEffectComponent` evolved: one bucket, node anchored).
- 2026-10-16 Behaviour, in expected frequency: emitters only (no code) →
  geometry animation (I3D authored states or keyed tracks) + emitters →
  effect component. Judgment rule: if we would hand-code it for simplicity
  in a new game in 2026, leave it hand-coded.
- 2026-10-16 Controller is sequential states + `END`; no blend trees.
