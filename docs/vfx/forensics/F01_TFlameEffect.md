# F01 TFlameEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F01 (covers F01 `TFlameEffect` + F02 `TFlameAnimator`) |
| **Class(es)** | `TFlameEffect` (object, near-empty shell — no override bodies) + `TFlameAnimator` (the visual: one `int32_t frame` field + `Initialize`/`Animate`/`Render`/`RefreshZBuffer`). |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Retail fidelity** | **bounded retail frontend verified (2026-10-07)** — actual retail Animate `0x4e4ef0` and Render `0x4e4f20` execute in the thin emulator. Authored geometry, UVs, rotations, scale, local frame wrap and blend requests are checked against the compiled port component. Eighteen RGB565 image pairs match using the same original software rasterizer. Full map/hierarchy integration and Revisited GPU-backend parity remain open. |
| **Author / Date** | vfx-forensics-agent / 2026-05-29 |
| **Family** | fire (single-card flipbook torch flame — the iconic dungeon ambient torch/candle/brazier) |
| **Draws** | One authored `box01` quad of `Magic\Flame.I3D`, transformed by explicit rotations and scale, selecting one cell from a **4×2 atlas in a 128×128 ARGB4444 surface**. |
| **Archetype(s)** | **(A) UV-coordinate atlas-cell pick flipbook** — `tu/tv` recomputed per-frame from a per-render-frame counter (`frame * 11 / 24`) to land on one of 8 atlas cells, NOT a `framehtexs[]` texture-handle swap. **(B) continuous loop** — `frame` cycles `0..17` forever, no kill condition in the animator (the effect object lives until its owning sector / spell kills it). No particle emitter, no sub-emit, no associated light, no audio. |

---

## 1. Summary

**2026-10-07 retail-body verification:** the earlier statements that Render and
Animate are unextracted or snapshot-only are superseded for base Flame
`0x50ba373b`. Registration `0x4e4ea0` leads through builder `0x5a9de8` and factory
`0x4f5d70` to animator vtable `0x5a9dec`. The unchanged retail instructions verify
X45°/Y30°/Z160° rotations, uniform 0.5 scale, atlas-cell integer selection
`frame*11/24`, and the 18-frame Animate wrap. The retained
[thin-emulator report](../../../recon/retail_asm/runtime/effects/flame-frontend-ab/manifest.json)
records the real shipped mesh/atlas, compiled production component, 18 matching
image pairs, eight distinct images and exact warm resets. Both frontends use
the original software renderer; owner hierarchy/extents and imagery submission
are explicit fixture boundaries. This is new source/frontend evidence, not
full map-context or Metal-renderer acceptance.

**2026-10-04 asset correction:** §2.1(1) and §4 supersede the earlier
asset interpretation throughout this document. The actual atlas is
**128×128 ARGB4444 with authored alpha**, not a 128×160 RGB565 green-keyed
surface. The original mesh is centered, 38.11058×69.56522 model units;
50×125 is a restore rectangle. Older green-background blend heuristics
in §7/§13 are not evidence of the retail blend state.

`"FLAME"` is the placed torch/candle flame used in shipped dungeon and
ambient scenes. It draws one authored `box01` quad from `Magic\Flame.I3D`,
rewriting four UVs to select one of eight 32×64 cells in a 128×128
ARGB4444 atlas. Its local counter cycles 0..17; `floor(frame*11/24)`
gives nonuniform two/three-tick cel holds. Snapshot source rotates the
real mesh by X45°, Y30°, Z160° and scales it uniformly by 0.5. It adds
no particles, light, audio, or lifetime state. Color and alpha come
from the actual hot orange/yellow texture, with a near-white core and
transparent red background.

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/cls_0x5a9d88.cpp` — **SPARSE / WRONG-SIZE**.
  The mapping pass tagged `cls_0x5a9d88` as a MEDIUM candidate for
  `TFlameEffect`/`TFlameAnimator` purely off a single XREF of the `"FLAME"`
  string (`recon/mappings/TFlameAnimator_cls_0x5a9d88_candidate.yaml:1-23`,
  `recon/mappings/EXTRACTION_PASS_2026-05-16.md:96`). But cls_0x5a9d88 is 264 B
  with 25 vftable entries (`cls_0x5a9d88__vftable_5a9d88.cpp:7-32`) — far too
  big for the snapshot's `TFlameAnimator` (one `int32_t frame` field on top of
  `T3DAnimator`). The 4 decompiled methods in `cls_0x5a9d88.cpp` look like a
  larger effect class (collision/registration logic, `mbr_0x9c`/`mbr_0xac`/
  `mbr_0xec`/`mbr_0xfc`/`mbr_0x100`/`mbr_0x104` state). **The TFlameAnimator
  render/animate bodies are NOT in cls_0x5a9d88.** They live as the
  free-function trampoline at `0x4e4ea0` (the entry in the animator-builder
  function-pointer table, `recon/classes/_data.txt:55954` = `005c5348 a0 4e 4e
  → LAB_004e4ea0`, the same table-of-builders structure SPARKS §2.1 confirmed
  for the `"sparks"` builder). The recon did not extract that trampoline body.
- **Pre-release (snapshot — authoritative for the mechanism):**
  - `TFlameEffect`: `DEFINE_BUILDER("FLAME", TFlameEffect)`
    `src/effect_old.cpp:4398`; `REGISTER_BUILDER(TFlameEffect)` `:4399`. No
    override bodies — the class has only inherited `TEffect` ctors
    (`src/effect.h:542-547`, treating the `InitializeVisualComponent` /
    `AttachVisualComponent` / `TFlameAnimatorBuilder` code at `:4401-4477` /
    `effect.h:545-560` as port additions, per hard-rule 8).
  - `TFlameAnimator`: class declaration `src/effect.h:1039-1059` — `int32_t
    frame` is the only field; `Initialize` `src/effect_old.cpp:4486-4494`,
    `Animate(bool draw)` `:4503-4510`, `Render()` `:4519-4565`, `RefreshZBuffer()`
    `:4567-4578`.
  - **No `REGISTER_3DANIMATOR("FLAME", TFlameAnimator)` exists in the snapshot
    tree** (grep across `src/effect_old.cpp`/`effect2.cpp`/etc. returns zero
    hits; the only `REGISTER_3DANIMATOR(... [Ff]lame ...)` in the snapshot is
    `"FLAMEDISC"` for `TFlameDiscAnimator` at `src/missileeffect.cpp:1488`).
    Retail **does** have it (the `005c5348→0x4e4ea0` entry above), so this is a
    **snapshot omission** — the registration was added later in development.
- **Asset:** `Magic\Flame.I3D` registered at `legacy/Class.Def:2030`
  (`"Flame" "Magic\Flame.I3D" 0x50ba373b`); snapshot file at
  `legacy/Imagery/Magic/flame.i3d` (41,348 B, MD5
  `736514e58a772c2cc7040543b919150a`). Retail equivalent inside
  `data/imagery.rvi` member `Imagery/Magic/flame.i3d` (41,348 B, MD5
  `e108cad4d45b6b23af8ad73bafed286c`) — **same file size, 7 bytes differ in
  the surface descriptor header** (see §2.1 / §4).
- **Sister effects consulted:**
  - `TFlareAnimator` (`src/effect_old.cpp:515-612`) — same "glow billboard +
    Alpha (`SetBlendState`) + material-zeroed-for-self-lit" family. Confirms
    Alpha is the snapshot's default for the glow-family on green/black-keyed
    sprites — important sister-blend cross-check (§7 — but the same blend is
    the snapshot-only drift risk the protocol warns about).
  - `TSymGlowAnimator` (`src/effect_old.cpp:4584-4682`) — also `SetBlendState`
    (Alpha), also glow-on-keyed-bg, also a per-frame UV mutation
    (`tu += u, u = random(2,8)/100`). Same blend family.
  - `TFireConeAnimator` (`src/effect_old.cpp:7211-7320`) — sister fire spell
    using a `TParticleSystem`-based particle emitter with FLAME_* constants
    (FLAME_COUNT=80, FLAME_VEL=10, FLAME_MIN_LIFE/MAX_LIFE=10/45,
    FLAME_FRAME_COUNT=17, …). Provides corroboration for "FLAME_FRAME_COUNT
    = 17" being the engine's understanding of a flame frame budget (matches
    the F01 animator's `if (frame >= 18) frame = 0` wrap = 18 unique
    counter values).
- **`spell.def` / `data/Modules/*` placements:** searched
  `data/Resources/spell.def` and the shipped `class.def` — no spell VARIANT
  invokes `"FLAME"` (this is **NOT** a cast spell); F01 is placed in modules
  as a world object (the `OBJCLASS_EFFECT` registry consumed by sector
  binaries — placements baked into `Map/*.dat` files). The shipped class.def
  (`data/imagery.rvi:class.def:3362-3364`) actually registers **three**
  `"Flame"` rows:
  ```
  "Flame" "Magic\Flame.I3D"  0x50ba373b {0,0} {}
  "Flame" "Magic\FlameB.I3D" 0x50ba373c {0,0} {}
  "Flame" "Magic\FlameG.I3D" 0x50ba373d {0,0} {}
  ```
  The snapshot `legacy/Class.Def:2030` registers only the first; retail added
  blue + green color-variant assets (`Imagery/Magic/flameb.i3d`,
  `Imagery/Magic/flameg.i3d`, both 33,404 B, dated `1999-03-17` in the .rvi
  index versus `Flame.I3D`'s `1998-07-14`). The orange `Flame.I3D` is the one
  the snapshot+class registration uses.
- **Source-of-truth ranking:** the snapshot is authoritative for the
  **mechanism** (the bodies are simple, complete, and the only available source
  for the Animate/Render math). The retail decomp is authoritative for the
  **registration name + asset file existence**. The retail decomp is
  **insufficient** for the per-frame UV math, the explicit rotation, the
  blend, and the scale — those are snapshot-only and a visual-vet-vs-retail
  ground truth is the only fallback (§12).

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp` is a pre-release snapshot. Cross-checks:

**(1) Asset identity — identical mesh/texture, seven graphics-bounds bytes differ.**
Both files are 41,348 bytes. Snapshot MD5 is
`736514e58a772c2cc7040543b919150a`; retail MD5 is
`e108cad4d45b6b23af8ad73bafed286c`. The seven differences are at absolute
file offsets `0x48..0x4e`, inside `SImageryStateHeader`, not a texture
surface descriptor. Retail changes graphics width/height from 339×316 to
128×160 and registration x/y from 339×135 to 64×120. The actual texture
is **128×128 ARGB4444 in both files**, and the mesh is unchanged.

The CGSR file header is 20 bytes, with an 84-byte imagery header; the
`SOld3DImageryBody` begins at file offset `0x68`. Its texture descriptor
is at `0x400`: height/width are both 128, bit count 16, channel masks
R=`0x0f00`, G=`0x00f0`, B=`0x000f`, A=`0xf000`. The frame pixels are
`0x2184..0xa184` (32,768 bytes), located by following the texture/frame
relative offsets. Earlier readings of `0x174` as pixel payload and
RGB565 green-key decoding were incorrect. The dominant `0x0f20` texel is
**transparent red** RGBA(255,34,0,0), not opaque green.

**(2) Registration + naming — CONFIRMED.** The retail binary contains the
`"FLAME"` string at `.rdata 0x5e10f4` with one XREF: `0x4e4ea0`
(`recon/classes/_data.txt:107431-107432`,
`recon/mappings/EXTRACTION_PASS_2026-05-16.md:96`). The address `0x4e4ea0` is
the **Build() trampoline** that the animator-builder factory table at
`0x5c5300+` indexes into; the entry `005c5348 → LAB_004e4ea0` is the
`REGISTER_3DANIMATOR("FLAME", …)` registration's function pointer
(`recon/classes/_data.txt:55954`). This matches the same pattern SPARKS §2.1
confirmed for `"sparks"` at `005c5350 → 004e53a0`. So **retail ships
`REGISTER_3DANIMATOR("FLAME", TFlameAnimator)` even though the snapshot
omits it.** The uppercase `"FLAME"` is the registered string in both the
snapshot's `DEFINE_BUILDER` and the retail binary.

**(3) Structure/layout — NOT corroborated.** `cls_0x5a9d88` (the
single-XREF candidate the mapping pass tagged) is **the wrong size**: 264 B,
25 vftable entries (`cls_0x5a9d88__vftable_5a9d88.cpp`). The snapshot
`TFlameAnimator` is `T3DAnimator + int32_t frame` — roughly the size of
`T3DAnimator` + 4 bytes, and with the 5 standard `T3DAnimator` vftable slots
(plus our 4 overrides for ctor/dtor/Initialize/Animate/Render/RefreshZBuffer).
The cls_0x5a9d88 fields (`mbr_0x100, mbr_0x104` etc.) and methods
(virt_meth_0x4e4d50, virt_meth_0x4e4d90, virt_meth_0x4f5d70 etc., the latter
allocates 0x100 bytes for a sub-object) look like a **different, larger
effect class** that just happens to consume the `"FLAME"` string. Possibly
the parent `TFireConeEffect` or a related composite — not investigated
further here. **Treat cls_0x5a9d88 as MIS-MAPPED for F01/F02.** The recon
mapping yaml itself flags the MEDIUM confidence
(`TFlameAnimator_cls_0x5a9d88_candidate.yaml:6,20-23`). A future Ghidra pass
should isolate the body at `0x4e4ea0` and walk back from there.

**(4) `recon/discovered/` etc. — NO COVERAGE.** No `recon/discovered/`
function bodies cover the flame animator's `Animate`/`Render` math. No
constant grep matches: searched the retail decomp for the snapshot's
flame-specific immediates (`frame * 11 / 24`, the angle constants 45/30/160,
the 0.5 scale, the RefreshZBuffer `50.0f * .5f` / `125.0f * .5f` patch
sizes) and got zero hits in the recon corpus (the `0.5f` bit pattern
0x3F000000 hits hundreds of unrelated places; the more specific
`frame*11/24` arithmetic doesn't surface as an isolated pattern).

**Verdict — retail-partial.** The *what/where/how-triggered* is
**retail-confirmed** (registration name `"FLAME"` matches; the asset
`Magic\Flame.I3D` is in shipped retail data with only the surface-header
metadata fixed; the binary contains the animator-builder factory entry for
`"FLAME"`). The *exact per-frame mechanism* inside `TFlameAnimator::Render` —
the **frame-wrap-18**, the **`frame*11/24` UV-cell math**, the
**(45°, 30°, 160°) rotation**, the **0.5 uniform scale**, the **Alpha blend**,
and the **(25, 62) RefreshZBuffer patch** — is **snapshot-only** and
unverified against shipped retail. Risk to reconstruction is highest on
**the render blend** (snapshot Alpha and current capture evidence favor
it, but retail Render is unextracted); secondary risk is the explicit rotation triple (does the
flame face the iso camera correctly with the snapshot's angles, or were
they re-tuned for ship?).

---

## 3. Constants

Every numeric the effect actually uses, with citation. The `confirmed?`
column reflects §2.1: only the registration name + asset file path are
retail-corroborated; the per-frame math values are snapshot-only.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| builder name | `"FLAME"` (uppercase; case-insensitive registry lookup matches `"Flame"` in `Class.Def`) | string | `effect_old.cpp:4398`, `Class.Def:2030` | **yes (retail)** — `s_FLAME_005e10f4`, `_data.txt:107431` |
| animator-builder factory entry | `0x4e4ea0` (the `REGISTER_3DANIMATOR("FLAME", TFlameAnimator)` build trampoline) | function pointer | (missing in snapshot — see §2 / §13.1) | **yes (retail)** — `005c5348 → LAB_004e4ea0`, `_data.txt:55954` |
| asset path | `Magic\Flame.I3D` (id `0x50ba373b`) | path + content hash | `Class.Def:2030` | **yes (retail)** — same path + same id in `data/imagery.rvi:class.def:3362` |
| **frame wrap** | **18** (`frame` cycles `0..17`) | render-frames per atlas cycle | `effect_old.cpp:4508-4509` | **snapshot-only** |
| frame seed | `frame = 0` at Initialize | render-frame counter | `effect_old.cpp:4493` | **snapshot-only** |
| frame advance | `++frame` per `Animate(bool draw)` call (once per render frame) | — | `effect_old.cpp:4507` | **snapshot-only** |
| atlas col stride | `0.25` (= 1/4) | UV units | `effect_old.cpp:4543,4549,4555` | **snapshot-only** (matches asset's 4-column layout, §4) |
| atlas row stride | `0.5` (= 1/2) | UV units | `effect_old.cpp:4544,4553,4556` | **snapshot-only** (matches asset's 2-row layout, §4) |
| atlas indexing math | `n = (int32_t)(frame * 11 / 24)`; `col = n % 4`; `row = n / 4` | indexes the 8-cell atlas | `effect_old.cpp:4543-4544` | **snapshot-only** (see §4 / §8 cell-by-frame table) |
| rotate X | `45 * π/180 ≈ 0.7854 rad` (= 45°) | rad | `effect_old.cpp:4531` | **snapshot-only** |
| rotate Y | `30 * π/180 ≈ 0.5236 rad` (= 30°) | rad | `effect_old.cpp:4532` | **snapshot-only** |
| rotate Z | `160 * π/180 ≈ 2.7925 rad` (= 160°) | rad | `effect_old.cpp:4533` | **snapshot-only** |
| scale | `(0.5, 0.5, 0.5)` (uniform) | × | `effect_old.cpp:4535-4537` | **snapshot-only** |
| translation | `(0, 0, 0)` (no-op — placement is the parent object's world pos) | wu | `effect_old.cpp:4540` | **snapshot-only** |
| obj flags | `OBJ3D_MATRIX (0x100) \| OBJ3D_VERTS (0x2000) = 0x2100` | bitmask | `effect_old.cpp:4528` + `src/3dimage.h:113,118` | **snapshot-only** |
| blend helper | `SetBlendState()` ⇒ MODULATE + SRC_ALPHA/INV_SRC_ALPHA (= **Alpha**) | render state | `effect_old.cpp:4521-4522`; helper bodies `:221-233` | **snapshot-only** (render body not in retail decomp) — **SUSPECT** (see §7 BLEND SANITY-CHECK) |
| sub-object index | `0` (sole sub-object `box01` in the asset) | index | `effect_old.cpp:4490,4524`; asset object-table at file offset `0x7c0`: count `1`, name `"box01"` | **snapshot-only** (asset corroborates the single-sub-object count) |
| vertex format | `D3DVT_LVERTEX` (per-vertex color + tu/tv) | type | `effect_old.cpp:4491` | **snapshot-only** |
| RefreshZBuffer patch w | `(int32_t)(50.0f * 0.5f) = 25` px | screen px | `effect_old.cpp:4571` | **snapshot-only** |
| RefreshZBuffer patch h | `(int32_t)(125.0f * 0.5f) = 62` px (truncated from 62.5) | screen px | `effect_old.cpp:4572` | **snapshot-only** |
| RefreshZBuffer patch centering | offset by `-size.x/2, -size.y/2` (centred on projected origin) | px | `effect_old.cpp:4577` | **snapshot-only** |
| asset surface dims | 128×128 ARGB4444 in retail and snapshot | px | descriptor at file `0x400`, h/w `0x408/0x40c`, masks `0x458..0x464` | **yes (retail)** |
| imagery graphics bounds / registration | retail 128×160, reg (64,120); snapshot 339×316, reg (339,135) | px | `SImageryStateHeader`, file `0x48..0x4e` | **yes (retail)** — not texture or cell dimensions |
| asset payload offset | `0x2184..0xa184`, 128×128×2 bytes | bytes | mesh texture/frame relative-offset chain | **yes (retail)** |
| TORADIANf | `π/180` | rad/deg | `revdefs.h` | snapshot-only |

`Animate(bool draw)` runs per **render frame** in this animator (the snapshot
does not gate by sim-tick — it simply increments `frame` every time
`Animate` is called). The cadence is whatever the renderer dispatches; per
NOMENCLATURE §6 the *intended* base is the engine's 24 Hz sim tick, but
because the body does no `Pulse()` gating the animator is effectively
framerate-dependent in the original. Reconstruction should convert
`frame_advance_rate = 1 cell per ~2.18 render frames` into a time-based
rate per the framerate-independent-animation feedback note.

> **The frame counter is per-instance**: every spawned `TFlameAnimator` has
> its own `frame`. Each torch independently runs through 0..17 starting from
> whenever it was spawned, so torches placed at different times will be out
> of phase naturally (no explicit per-instance phase offset is set).

---

## 4. Assets

Use the retail `Imagery/Magic/flame.i3d` member of `data/imagery.rvi`
(41,348 bytes, MD5 `e108cad4d45b6b23af8ad73bafed286c`). Its single
sub-object is `box01`; object count is at file `0x7c0`, name at `0x7c4`.
The snapshot has the same mesh and texture bytes (§2.1).

### 4.1 Actual texture format and atlas

The actual `SSurfaceDesc` is at file `0x400`, with a **128×128 ARGB4444**
surface, one frame, channel masks R=`0x0f00`, G=`0x00f0`, B=`0x000f`,
A=`0xf000`. Frame pixels begin at `0x2184`; 16,384 texels occupy exactly
32,768 bytes through EOF `0xa184`. The animator's 4-column×2-row UV
strides therefore select **32×64 texel cells**, not 32×80.

Texture alpha is authored. The dominant `0x0f20` texel (11,065 of 16,384
pixels, 67.5%) decodes to RGBA(255,34,0,0), a transparent red background.
The hot core values such as `0xfffd` decode to RGBA(255,255,221,255).
There is no source RGB565 green chroma convention to reconstruct. Any
remaining color-key handling is a loader/renderer implementation choice,
not evidence that the original flame was green-keyed. `0x48..0x4e`
contains imagery graphics bounds and registration, not surface dimensions.

The original renderer rewrites UVs by vertex index:

```
vertex 0: (u,      v)
vertex 1: (u+.25,  v)
vertex 2: (u,      v+.5)
vertex 3: (u+.25,  v+.5)
u = (floor(frame*11/24) % 4) * .25
v = (floor(frame*11/24) / 4) * .5
```

The 18 authored ticks visit cells
`0,0,0,1,1,2,2,3,3,4,4,5,5,5,6,6,7,7`. This is a 0.75-second loop
at 24 authored ticks/sec. Preserve that discrete cell timing with elapsed
time; continuous `floor(time*11)` alone does not retain the same holds.

### 4.2 Authored geometry and registration

The global vertex pool starts at file `0x1dd4`, with four 32-byte
`S3DVertex` records. Runtime `GetObjVerts(0)` returns these centered
object-local positions:

```
0: (-19.0552902222, -34.7826118469, 0)
1: ( 19.0552902222, -34.7826118469, 0)
2: (-19.0552902222,  34.7826118469, 0)
3: ( 19.0552902222,  34.7826118469, 0)
faces: (2,0,3), (1,3,0)
```

The quad is **38.11058×69.56522 model units before scale**. The
`RefreshZBuffer` 50×125 constants are damage/restore rectangle dimensions;
they are not the mesh size. At render, original source applies row-vector
`Rx(45°)*Ry(30°)*Rz(160°)*S(0.5)`, then the instance world transform.
The current port's instance transform already includes `WORLD3D_Z_SCALE`
(1.5) plus object rotation/translation, so it must be applied once to
these transformed local corners. The resulting quad has tilt and shear;
an axis-aligned billboard or a fitted projected bounding box loses that
geometry. This 1.5 stretch is the current port convention, not a verified
retail mesh-to-world conversion; the remaining projection gap is recorded
in the lower-ambient comparison below. Retail Flame renderer-body
confirmation remains pending.

Verified independently by extracting the retail archive member and by
`Revenant --headless --dumpi3d=Magic\flame.i3d`, which reports a 128×128
texture, four vertices, six indices, and the above model bounds. Dump
artifacts for this pass are `/tmp/revenant-flame-authored/`.

---

## 5. Spawn & emit

- **Trigger semantics:** **continuous (looping, infinite).** The animator
  has **no kill condition** — `Animate` only advances `frame` and wraps it;
  there is no `KillThisEffect()` call anywhere in the snapshot's
  TFlameEffect/TFlameAnimator bodies. The effect lives for as long as its
  owning sector / world placement keeps it alive. (Sector-bound torches are
  reaped when the sector unloads; spell-bound flame use would need an
  explicit kill from outside, but there is no live spell caller for
  `"FLAME"` in `data/Resources/spell.def`.)
- **Count per trigger:** **1 quad** — the single `box01` sub-object,
  rendered once per `Render()` call (`effect_old.cpp:4519-4565`). No
  particle array, no per-instance multiplication.
- **Initial direction / distribution:** N/A — this effect is **NOT** a
  particle emitter. The single quad is positioned at the effect object's
  world origin (the iso projection places it; the animator sets
  `obj->pos = (0, 0, 0)` for the matrix translate, `effect_old.cpp:4540`,
  so the effect inherits the parent `TEffect` object's world position).
- **Emit anchor convention:** **at the spawn call's exact `(x, y, z)`** —
  the world position of the `TEffect` object. The animator does not
  ground-project, doesn't follow a character, doesn't read a bone matrix.
  Each torch instance is placed by the world-data sector spawn at its
  literal (x, y, z). (Sector binaries in `data/Modules/*/Map/*.dat` bake the
  torch positions; the snapshot has no readable text-form placement for the
  flame effect since the .dat files are binary.)
- **Coordinate space:** the matrix-built quad lives in **local space**
  (the matrix transforms the authored `box01` verts from object-local
  coordinates to a final per-quad orientation), then the engine concatenates
  the effect object's world transform at render. With `obj->pos = (0,0,0)`
  there is no local-to-anchor offset.
- **Spread / jitter:** N/A — no randomness in spawn. (Per-instance phase
  variation comes only from the fact that different torches are spawned at
  different sim ticks, so their `frame` counters are naturally desynced —
  no explicit jitter.)

### Spatial diagram (single quad, anchored at world origin)

```
   wz (up)
   │
   │       ▲           Single ScreenAligned/oriented billboard
   │      /│           (one box01 sub-object, 32×64 px source cell)
   │     ╱ │           Authored "up" along quad's local Y; matrix
   │    ╱  │           applies RotX(45°)·RotY(30°)·RotZ(160°)·Scale(0.5)
   │   ╱   │           ────────────────────────────────────────────────
   │  ●────┘            ← emit origin = world (x,y,z) of the TEffect
   │  │                   object (literal sector spawn point; e.g.
   │  │                   torch sconce world pos)
   └──┼─────────── wx
     ╱   No emit cone, no spread, no particles. Every torch in the
    ╱    world is one of these quads.
   wy
```

---

## 6. Behavior & per-frame logic

The animator's full per-frame logic is short. The effect (object) side has
no override bodies — only the inherited `TEffect`/`TObjectInstance`
plumbing.

### 6.1 `TFlameAnimator::Initialize()` (`effect_old.cpp:4486-4494`)

```
Initialize():
    T3DAnimator::Initialize()              // base: standard animator setup     // :4488
    obj = GetObject(0)                     // bind the sole box01 sub-object    // :4490
    GetVerts(obj, D3DVT_LVERTEX)           // allocate effect-owned lvert buf   // :4491
                                           //   (4 verts, per-vertex color + UV)
    frame = 0                              // seed flipbook counter             // :4493
```

`GetVerts(obj, D3DVT_LVERTEX)` allocates the effect's own `lverts[0..3]`
buffer (instead of sharing the imagery's read-only verts), which is what
lets `Render` mutate `obj->lverts[i].tu/tv` per frame.

### 6.2 `TFlameAnimator::Animate(bool draw)` (`effect_old.cpp:4503-4510`)

```
Animate(draw):
    T3DAnimator::Animate(draw)             // base: refresh hierarchy, mirror   // :4505
                                           //   pos from inst, etc.
    ++frame                                                                     // :4507
    if (frame >= 18)                                                            // :4508
        frame = 0                                                               // :4509
```

That is the entire per-frame update. Each `Animate` call (per render frame)
advances `frame` by 1; on the 18th invocation `frame` wraps back to 0. **No
sim-tick gate, no per-instance phase, no velocity/gravity/drag, no scale or
alpha envelope, no spawn rate, no sub-emit.** Per the framerate-independent
animation feedback (`feedback-framerate-independent-anim` memory),
reconstruction must convert this `1 cell per ~2.18 render frames` into a
time-based rate (e.g. ~24 frames/sec → ~10.91 cells/sec → ~0.75 s per cycle)
and integrate by real `dt`.

### 6.3 `TFlameAnimator::Render()` (`effect_old.cpp:4519-4565`)

```
Render():
    SaveBlendState()                       // snapshot current D3D states       // :4521
    SetBlendState()                        // → Alpha (MODULATE + SRC_ALPHA/    // :4522
                                           //          INV_SRC_ALPHA, ZWRITE
                                           //          off, ZENABLE on)
                                           //   [SUSPECT — see §7 BLEND SANITY-
                                           //    CHECK; render body not in
                                           //    retail decomp]

    obj = GetObject(0)                                                          // :4524
    ResetExtents()                         // clear bounding rect               // :4526

    obj->flags = OBJ3D_MATRIX | OBJ3D_VERTS  // 0x2100: explicit matrix +       // :4528
                                             //   effect-owned vert buffer
    D3DMATRIXClear(&obj->matrix)             // identity                        // :4529
    D3DMATRIXRotateX(&obj->matrix,  45 * π/180)    // chain rot X 45°           // :4531
    D3DMATRIXRotateY(&obj->matrix,  30 * π/180)    //       rot Y 30°           // :4532
    D3DMATRIXRotateZ(&obj->matrix, 160 * π/180)    //       rot Z 160°          // :4533

    obj->scl = (0.5, 0.5, 0.5)                                                  // :4535-4537
    D3DMATRIXScale(&obj->matrix, &obj->scl)        // uniform scale 0.5         // :4538

    obj->pos = (0, 0, 0)                                                        // :4540
    D3DMATRIXTranslate(&obj->matrix, &obj->pos)    // translate by 0 (no-op)    // :4541

    // FRAME-INDEXED UV CELL PICK ----------------------------------------------//
    n = (int32_t)(frame * 11 / 24)         // 0..7 over the 18-frame cycle      // :4543-4544
    xpos = (n % 4) * 0.25                  // column origin UV
    ypos = (n / 4) * 0.5                   // row    origin UV

    // Write the 4 quad-corner UVs to sample the 0.25 × 0.5 cell at (xpos,ypos)
    lverts[0].tu/tv = (xpos,        ypos       )                                // :4546-4547
    lverts[1].tu/tv = (xpos + 0.25, ypos       )                                // :4549-4550
    lverts[2].tu/tv = (xpos,        ypos + 0.5 )                                // :4552-4553
    lverts[3].tu/tv = (xpos + 0.25, ypos + 0.5 )                                // :4555-4556

    RenderObject(obj)                      // submit the quad                   // :4558
    UpdateExtents()                        // updates the on-screen rect        // :4560
    RestoreBlendState()                    // pop saved states                  // :4562
    return true
```

The matrix is built **right-multiplicatively** (`m = ID · RotX · RotY · RotZ ·
Scale · Translate`), per the `D3DMATRIX*` helpers' chained-right-multiply
convention (knowledge 03 §7). Because the translate is `(0,0,0)`, the
matrix is effectively `RotX(45°) · RotY(30°) · RotZ(160°) · Scale(0.5)` —
a pure rotation-and-scale of the authored `box01` quad. The flame's world
placement comes entirely from the parent `TEffect` object's position, which
the engine concatenates at draw time.

> **Orientation classification.** The matrix is built with explicit
> Euler-angle rotations, not the canonical WorldXY `rot.x = -π/2` tip or a
> ScreenAligned bare position. With NOMENCLATURE §2's enum-style taxonomy
> none of the standard orientation tells matches cleanly — the closest is
> **ScreenAligned-with-explicit-tilt**: the quad does not auto-billboard
> (no per-frame camera-facing math), but the static 45° X / 30° Y rotation
> roughly aligns the quad with the iso camera's 30°-tilted view. The 160°
> Z rotation is unusual (close to a 180° flip) and may be flipping the
> authored sprite (the I3D STILL convention puts texture origin at the
> bottom-left; flipping Z by ~180° puts the flame base at world-down).
> **Forensics flag:** the rotation triple is hard-coded and snapshot-only;
> it may have been re-tuned for ship. Visually vetting against an in-game
> torch capture is the only way to confirm.

### 6.4 `TFlameAnimator::RefreshZBuffer()` (`effect_old.cpp:4567-4578`)

```
RefreshZBuffer():
    size.x = (int32_t)(50.0f * 0.5f) = 25                                       // :4571
    size.y = (int32_t)(125.0f * 0.5f) = 62 (truncated from 62.5)                // :4572
    effect = inst->GetPos()                // world position of TEffect         // :4574
    screen = WorldToScreen(effect)         // project to screen pixels          // :4576
    RestoreZ(screen.x - size.x/2,          // restore Z patch centred on       // :4577
             screen.y - size.y/2,          //   the projected origin
             size.x, size.y)               //   25 × 62 px box
```

The 50×125 constants describe a centered Z-buffer restore/damage patch,
scaled by 0.5 and truncated to 25×62 pixels. They do not describe the
mesh size or graphics registration header (§4.2). `RestoreZ` issues
`Scene3D.RestoreZBuffer(rect)` via `effect_old.cpp:162-171`.

### Temporal diagram (continuous loop, never dies)

```
cell index n = (frame*11/24):
        0   1   2   3   4   5   6   7
        ┌───┐               ┌───┐
        │ # │               │ # │     ← cells held 3 render frames
3 ─────●┘   ●───●───●───●───●   ●───●  (others held 2)
        │   │   │   │   │   │   │   │
2 ─────│───│───●───●───●───│───●───●  cell n's column/row in 4×2 atlas:
        │   │               │           n=0 (0,0)    n=4 (0,1)
1 ─────│───│               │           n=1 (1,0)    n=5 (1,1)
0 ─────●                   ●           n=2 (2,0)    n=6 (2,1)
        └───┴───┴───┴───┴───┴───┴───┴─→ frame  n=3 (3,0)    n=7 (3,1)
        0   3   5   7   9  11  14  16  18 (wraps to 0, no kill)

cycle length: 18 render frames ≈ 0.75 s @ 24 Hz nominal rate
no scale/alpha envelope — only the cell-pick changes per frame.
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** **one billboard quad** — the sole `box01` sub-object
  of `Magic\Flame.I3D`, with the effect's own per-vertex `lverts[0..3]`
  buffer overriding UVs each render (the quad's vertex positions come from
  the asset's authored geometry, fed through the explicit object matrix).
- **Blend mode (original — what the code actually does):** **Alpha
  (modulated)** via `SetBlendState()` (`effect_old.cpp:4521-4522`; helper
  body at `:221-233`). The render states set:
  - `D3DRENDERSTATE_TEXTUREMAPBLEND = D3DTBLEND_MODULATE` (`:223`) — texel
    multiplied by interpolated per-vertex diffuse
  - `D3DRENDERSTATE_ZWRITEENABLE = false` (`:224`) — no depth write
  - `D3DRENDERSTATE_ZENABLE = true` (`:225`) — depth test on
  - `D3DRENDERSTATE_SRCBLEND = D3DBLEND_SRCALPHA` (`:228`)
  - `D3DRENDERSTATE_DESTBLEND = D3DBLEND_INVSRCALPHA` (`:229`)

  Classify as **Alpha** (NOMENCLATURE §3) — the same `SetBlendState()` mode
  TFlareAnimator and TSymGlowAnimator use on their (also-keyed) glow
  sprites.

  ### BLEND SANITY-CHECK

  The texture is ARGB4444 with authored alpha. Its transparent red texels
  do not establish additive blending: alpha and additive both discard
  or weight them through coverage. Snapshot source explicitly requests
  SRC_ALPHA/INV_SRC_ALPHA. Retail Render remains unextracted, so visual
  evidence is required to confirm its blend state.

  The 2026-10-04 isolated comparison favors Alpha over Additive on the
  same captured floor. That remains preliminary because the reference
  ambient was unusually high; lower-light and authored-map captures
  remain the acceptance checks. Scenery lights are separate map objects,
  not emitted by Flame. The earlier green-key/additive inference was
  based on a wrong asset decode and is withdrawn.

- **Lit vs self-lit:** classify as **Unlit** (the color is literal — the
  per-vertex diffuse is opaque white by default and the texture's authored
  hot-orange/yellow comes through unchanged under MODULATE). The
  animator does NOT zero the material (unlike TFlareAnimator) and does NOT
  fold ambient light in (unlike TWaterFallAnimator) — so the flame reads
  at full sprite brightness regardless of scene ambient. (This is consistent
  with the flame being a *self-lit* glow source rather than a *scene-lit*
  surface.)
- **Depth / Z:** **TestNoWrite** (NOMENCLATURE §5) — `ZENABLE=true,
  ZWRITEENABLE=false` (set by `SetBlendState`,
  `effect_old.cpp:224-225`). `RefreshZBuffer` repairs scene Z under the
  flame's projected footprint (§6.4).
- **Orientation:** an authored mesh at a fixed explicit tilt, not a
  camera-facing billboard. Apply the source matrix to the real corner
  positions. Do not replace it with a guessed WorldUpAligned behavior
  or a projected bounding box. Retail retuning of the angles is still
  an evidence gap, requiring native capture or extracted Render code.
- **Per-quad / per-object transform:** explicit
  `OBJ3D_MATRIX | OBJ3D_VERTS = 0x2100`. The matrix is
  `RotX(45°) · RotY(30°) · RotZ(160°) · Scale(0.5)` with translation 0
  (parent object's world pos used by the engine for placement).
- **Per-vertex color packing:** **NONE written by the effect.** The
  `GetVerts(obj, D3DVT_LVERTEX)` (`effect_old.cpp:4491`) allocates the
  4 lverts at Initialize, but the Render body never touches
  `lverts[i].color`. They keep their default (whatever `GetVerts`
  initialized — typically opaque white in the engine's lvert default), so
  under MODULATE the texture passes through with no per-vertex tint.

---

## 8. Texture animation

**Mechanism: UV-coordinate atlas-cell pick (NOT `framehtexs[]`).** Per
knowledge 03 §8.6 mechanism #2 ("UV atlas-cell pick"): the four corner
UVs are recomputed each render frame to land on one of 8 cells in a 4×2
atlas (§4). The asset's `S3DTex.numframes = 1` (single physical surface,
no per-frame texture handles), confirmed by the actual 32,768-byte
128×128×2 payload and the mesh texture record reporting one frame (§4). So the flipbook is **purely UV-based** — no
`SetTextureFrame()` call, no `framehtexs[]` swap, no `textureframe[]` write.

**Rate / wrap / per-instance phase offset:**
- **Rate (cells/frame):** non-uniform — `n = (frame * 11 / 24)` makes cell
  n advance roughly every 2.18 render frames; specifically cells 0 and 5
  hold for 3 frames, the others for 2 (cell-by-frame table in §4).
- **Wrap:** `frame >= 18 → frame = 0` (`effect_old.cpp:4508-4509`); cells
  traverse `0 → 7 → 0 → 7 → …` every 18 frames.
- **Per-instance phase offset:** **none explicit** — every animator
  starts with `frame = 0` at Initialize (`effect_old.cpp:4493`). Phase
  variation between torches comes only from staggered spawn times (each
  torch starts its own counter when it spawns; different sectors load at
  different times, so torches naturally desync).

**If a static texture (no animation):** N/A — this effect IS UV-animated;
the per-frame UV rewrite is the mechanism.

---

## 9. Associated light

**N/A — none emitted.** The animator makes no `AddPointLight` / dynamic-
light call, and there is no associated light system code in
`TFlameEffect`/`TFlameAnimator` (`effect_old.cpp:4395-4578` contains zero
references to light/AddLight/SetLight). The flame is **a light SOURCE in
the fiction** (it's a torch), but it is **NOT** a light source in the
engine — the dungeon lighting is baked / authored via static light fixtures
in `area.def` (e.g. POINTLIGHTINT / POINTLIGHTRANGE per the Revisited area-
def memory). The flame effect itself emits no dynamic lighting contribution;
it is a billboard glow only. (Contrast: explosions/fireball emit dynamic
light. The ambient torch flame does not.)

---

## 10. Color

The authored `Magic\Flame.I3D` texture supplies all color and coverage;
Render supplies no per-vertex tint or spell color. Decode by the actual
ARGB4444 masks (§4): `0xfffd` is RGBA(255,255,221,255), `0xfff8` is
RGBA(255,255,136,255), `0xfff9` is RGBA(255,255,153,255), and `0x0f20`
is RGBA(255,34,0,0), fully transparent red. Expected: hot orange/yellow
with a near-white core on a transparent background. Do not derive
RGB565 colors or green-key rules from these values. There is no
effect-specific color normalization or boost.

---

## 11. Audio coupling

**No audio coupling found.** `TFlameAnimator` and `TFlameEffect` make no
sound calls. There is no `PLAY(...)` / `PlayWave(...)` / `Sound...` call in
the snapshot's flame bodies (`effect_old.cpp:4395-4578`). The asset
`flame.i3d` is a STILL (single-state, single-frame) imagery — it has
no per-frame `S3DTag` sound triggers either.

Sector-placed torches in the world *may* be paired with positional
`TAmbSoundEffect` (`"Speaker"`) instances in the same world location for a
"crackle" loop, but that pairing is in module/sector data (the binary
`.dat` files) and is a separate `Speaker` effect, not part of `TFlameEffect`.
Record: **the flame effect itself is silent**; any associated crackle is
a sibling Speaker effect placed alongside it in world data.

---

## 12. Triggers & in-game appearance

- **Spawned by:** **world placement** (sector binary `.dat` data). The
  effect is registered via `DEFINE_BUILDER("FLAME", TFlameEffect)`
  (`effect_old.cpp:4398`) + `REGISTER_BUILDER(TFlameEffect)` (`:4399`), and
  the asset binding lives in `legacy/Class.Def:2030` (`"Flame"
  "Magic\Flame.I3D"`). Spawn happens through the standard
  `MapPane.NewObject(SObjectDef{objclass = OBJCLASS_EFFECT, objtype =
  EffectClass.FindObjType("flame"), pos, level, facing})` path
  (knowledge 01 §5), invoked from sector loading code. **NO `spell.def`
  variant invokes `"FLAME"`** — searched `data/Resources/spell.def`:
  no row references the flame builder. So this effect is **not** a
  cast spell.
- **Where to see it in the original game:** **every dungeon, keep, tavern,
  and ambient lit interior in the shipped Ahkuilon module.** Examples:
  - **Misthaven Cave torches** — `Ahkuilon_unzipped/cave.s` has multiple
    torch placements (the `"flamer1"/"flamer2"/"flamer3"/"flamer4"` /
    `"flameon"` / `"Flameon2"` blocks at `:671-770` are *damage* zones
    for player.burn — the VISUAL flames are in the sector `.dat` files
    placed near these zones).
  - **Tower / Keep interior braziers and wall torches** — wherever the
    map authoring placed a torch sconce, a `"FLAME"` effect is the
    visual.
  - **Anywhere a candle, brazier, or open flame appears in the
    background.**

  Easiest in-game repro for ground-truth capture: load the **Misthaven
  Caves** module (or the starting Keep), find any torch sconce, and
  observe — it should be a flickering single quad of hot orange/yellow
  flame with a hot-white core, looping ~0.75s/cycle, lighting the wall
  faintly via the area's static point-light fixtures (the flame itself
  emits no light).
- **Vestigial?** **No.** Live world caller via sector data placement —
  effectively every world map uses the effect. The retail binary keeps
  the registration (`s_FLAME_005e10f4` + the animator-builder factory
  entry at `005c5348 → 0x4e4ea0`), confirming the effect is in the
  shipped game.

---

## 13. Gaps & uncertainties

- **13.1 Retail blend remains unconfirmed.** Snapshot Alpha is explicit;
  current isolated capture evidence favors it over Additive. The old
  green-key/additive inference came from a wrong asset decode and is
  withdrawn. Confirm against lower-light and authored-map footage.
- **13.2 Resolved asset-header confusion (2026-10-04).** Width/height
  128×160 and registration 64×120 at file `0x48..0x4e` describe the
  imagery state's graphics rectangle. The actual surface is 128×128
  ARGB4444 at `0x400`, with 32×64 atlas cells; see corrected §4.
- **13.3 The (45°, 30°, 160°) rotation triple is unusual and
  snapshot-only.** It does not match any canonical orientation in
  NOMENCLATURE §2 (not WorldXY's `-π/2` tip, not bare ScreenAligned).
  Best read: it's the original's hand-tuned approximation of a
  "WorldUpAligned" billboard (pinning Z up, facing the iso camera)
  baked as a static 3-Euler. Whether ship retuned these is unknowable
  from the recon — visually vet the on-screen orientation against an
  in-game torch (flame upright, facing camera-ish, not laying flat,
  not 180° upside-down).
- **13.4 No `REGISTER_3DANIMATOR("FLAME", TFlameAnimator)` in the
  snapshot.** Snapshot has the `DEFINE_BUILDER` for the *object* but
  not the macro registration for the *animator*. Retail *does* have it
  (the `005c5348 → 0x4e4ea0` factory entry, §2.1(2)). Reconstruction
  needs to call `REGISTER_3DANIMATOR("FLAME", TFlameAnimator)` (or the
  port's equivalent) regardless — retail proves it must exist for the
  effect to spawn its animator.
- **13.5 cls_0x5a9d88 is mis-mapped to TFlameAnimator.** The candidate
  yaml flags MEDIUM confidence; the actual class layout (264 B, 25
  vftable entries) doesn't match a tiny `T3DAnimator + int32_t frame`.
  cls_0x5a9d88 is most likely a different, larger effect class that
  consumes the `"FLAME"` string for some other reason (possibly a
  parent registration site or a composite consumer). **Do NOT trust
  cls_0x5a9d88 as TFlameAnimator's body.** The real animator body is
  at the un-extracted free-function trampoline `0x4e4ea0`; a future
  Ghidra pass should walk back from there.
- **13.6 Retail asset adds two color variants (`FlameB.I3D`,
  `FlameG.I3D`) the snapshot doesn't have.** These are blue and green
  tint variants registered under the same `"Flame"` builder name in
  shipped class.def (`data/imagery.rvi:class.def:3362-3364`). They
  likely serve magic/alternate-flame placements; the F01 effect *class*
  works on whichever asset is bound when the object is built. The
  snapshot only has the orange original; reconstruction can defer the
  color variants to a later phase if focusing on the canonical torch.
- **13.7 Resolved texture-format confusion (2026-10-04).** Snapshot
  and retail texture descriptors/payload are identical; their seven
  differences concern graphics bounds/registration. Earlier RGB565
  histograms sampled mesh data and misread ARGB4444 pixels. Authored
  alpha makes the dominant red texel transparent. Consume the retail
  asset, but do not treat its 128×160 graphics bounds as texture size.
- **13.8 The Render body never writes per-vertex color.** Under
  MODULATE the texture is multiplied by whatever default `lverts[i].
  color` `GetVerts` leaves in place (typically opaque white). This is
  fine if the default is `(1, 1, 1, 1)`; if the engine's `GetVerts`
  default for `D3DVT_LVERTEX` is some other value (e.g. 0 alpha →
  invisible) the flame would not draw at all. Reconstruction must
  ensure the per-vertex diffuse defaults to opaque white. This is
  noted in the doc rather than the burndown because it is an engine
  invariant rather than an effect-specific behavior.
- **13.9 Preserve authored cel holds using elapsed time.** Original
  Animate increments once per call, with an 18-tick counter. At the
  nominal 24-Hz cadence, use `tick=floor(seconds*24) % 18`, then
  `cell=floor(tick*11/24)`. Continuous `floor(seconds*11)` alone does
  not preserve the exact two/three-tick holds. The current flipbook
  clock uses elapsed time and retains this discrete source sequence;
  retail cadence and phase remain unsynchronized.

---

## 14. Reconstruction burndown

### 2026-10-04 shipped-map acceptance scene

Primary context check: `Ahkuilon.rvm`, `Map/41_10_10.dat`, area
**The City of The Children**. Retail `area.def` sets ambient 24,
RGB(150,150,250). Do not replace these authored conditions with the
neutral isolated-fixture lighting for the main acceptance capture.

The v15 sector has 418 slots and 8,162 bytes, parsed exactly using the
retail block-size layout (`recon/docs/SECTOR_FILE_FORMAT.md`). Flame
(type `0x50ba373b`, class 25) occupies slots 17 and 18 at
(10449,10475,98) and (10757,10479,98). Their actual `DunTorch4` sconces
(type `0x8447068b`) occupy slots 255 and 256 at (10471,10497,16) and
(10779,10501,16). `DunWallS`, `DunColS`, tall wall pieces and
`Dunffff`/`Dunffff2` floors provide the authored surroundings.

Proposed camera: level 41, world center (10603,10477,16), logical
viewport 640×340. This frames both wall torches and their mounting
scenery. Their different world X coordinates also change isometric Y:
the flames appear near screen (166,20) and (470,180), not at a shared
screen Y. The first native context screenshot is
`~/RevenantRetailLab/captures/runs/vfx-map-ab-01-torches_city-retail/before.png`.
It shows the actual sconces, illuminated walls, floor and editor light
markers. The subsequent full-map comparison is
`captures/ab/vfx-map-ab-02-torches_city/`, binary SHA-256
`64d06458c76815a20092f65d053e0d9a8edab48226baf802bf06ff3650120b29`.
Both port flames are attached to the correct authored sconces. Retail's
orange illumination on the walls and floor is absent in the port capture;
this is a map light-object issue, not evidence for adding a light to
Flame. Yellow editor markers still obscure the retail flame pixels, so
exact map shape/cel acceptance needs a marker-free reference.

The two torch ROIs (140,0,205,75) and (440,145,510,225) each contain
exactly one distinct image across all 150 numbered port frames in that
capture: these map flames were not animating. The definition-based
Flame constructor attached its component before streamed `Load()`
replaced the temporary map index. Activation had already registered a
`TSafeComponentRef` using that old index; changing the index does not
repeat `OnAttach()` on an already active component. The constructor now
defers attachment to the existing imagery-builder `AttachComponents`
path after loading. The imagery-only preview constructor retains its
one-time activation after receiving a final runtime index. A new build
and full-map sequence must confirm at least eight changing cels before
this capture can support animation acceptance.

Retail console commands `hideobjects lightsource` and
`hideobjects helper` hide those classes, but do **not** remove these
yellow markers: the markers are class 9 Tile, type `Light`, imagery
`Town\\TwnLight.I2D`, with both `OF_LIGHT` and `OF_EDITOR` set. Do not
hide the entire Tile class. These class commands are present in the
retail binary but absent from the old editor snapshot.
The retail command table at `0x5c7924` dispatches `hideobjects` to
`0x427010`, which calls map method `0x45a7c0`. It resolves the class,
iterates matching objects and sets only `OF_INVISIBLE` (0x80), leaving
light flags and parameters intact. The reverse method `0x45a700` clears
that bit, including any previously invisible objects in the class, so
blind class-wide hide/show pairs are not an exact state restoration.
Port `show_gizmos=false` suppresses light and helper geometry through
`maprenderer.cpp`, while illumination remains enabled.

The shipped scoped command `Light.toggle invisible on` is confirmed by
the command table (`0x5c81cc` → handler `0x423a90`). The handler recognizes
`on`, `1`, and `true` at `0x423b58`–`0x423bd4`, sets value 1, and calls
`SetFlag` (`0x472db0`), which sets only bit 7 (`OF_INVISIBLE`, flag-name
table entry `0x5d478c`). The interpreter at `0x41e8e0` resolves the
`Light.` context through `0x41e690` and the nearest-object finder
`0x451fe0`/`0x451de0`. Unlike the old snapshot's scoped parser, the retail
resolver passes the current command context: distance is measured from
the selected object's position if there is one, otherwise the camera
center. Clear selection before positioning the camera exactly at a
known light marker and issuing this command. Empty serialized object
names use their type name, initialized by base constructor `0x46e1f0`.

The four on-screen marker world positions are (10450,10606,102),
(10477,10509,102), (10736,10448,64), and (10704,10736,224). All 13 Light
Tiles in this sector originally have flags `0x4c105`, with invisibility
clear. A targeted temporary hide can preserve their illumination and
avoid changing unrelated Tiles; restore the original camera, capture,
then unload without saving. No genuine global widget toggle has been
confirmed. Retail `widgets` is a UI definition token, not a console
command; Ctrl+Shift+W toggles script execution and is not a widget toggle.

The retail editor can view this shipped scene read-only;
loading a module must not save unrelated editor modifications. The
port should load the original sectors and submit both effects through
normal map rendering. An isolated floor capture remains diagnostic,
not sufficient to accept a persistent placed effect.

### 2026-10-04 compiled authored-quad lower-ambient check

Evidence: `~/RevenantRetailLab/captures/ab/vfx-dark-ab-01-flame/`,
native reference `captures/runs/vfx-dark-ref-01-flame/`. The comparison
uses ambient 32, white RGB, the same nine retail floor plates and
camera/placement as the earlier isolated fixture. Port binary SHA-256:
`7266123ce6bec3ef2a7d9f8917d093322c827bd80e9e7a9026ea0664f1715949`.
Retail AVI SHA-256:
`5e494cc071b4975da97f2f54f2749e3e383e64f0816b7908017f6846f9346fc4`.
No position or scale fitting was performed.

Both lossless native frames decoded at 30 Hz and port PNG frames contain
exactly eight distinct, repeating Flame cels. Matching the eight unique
cels in cyclic order, without moving or scaling pixels, gives mean
absolute RGB error 0.7317 in the 90×135 ROI. The pipeline's single global
17-frame phase estimate gives 0.7706. Background pixels dominate these
metrics; neither is a fidelity percentage. Raw elapsed and estimated
phase videos are both retained.

For pixels exceeding the matching ground baseline by more than 20 in
any channel, the port's intensity centroid is 0.26–0.47 screen pixels
right and 0.72–0.98 pixels down across matching cels. Its bottom is one
pixel lower in six cels, two pixels lower in one, unchanged in one.
Across the captured frames, the selected pixels' mean RGB is
(192.55,149.23,81.81) native and (191.58,147.23,81.85) port. The remaining
visible difference is a small consistent registration/shear difference;
there is no evidence here for replacing the atlas, adding a color tint,
or changing the authored rotations to fit the capture.

Native cycle starts span 21–24 decoded 30 Hz frames; the port alternates
22/23 frames after the partial initial cycle, consistent with the source
18 ticks at 24 Hz (0.75 seconds). Native display/simulation phase remains
unsynchronized. This evidence supports the present cel clock, not a
retail timing identity claim.

**Generic camera/model-Z evidence gap:** original `3dscene.cpp`
`SetSize` uses viewport scale `65536*512/sqrt(256²+256²)`, and its
30° camera projects model-local Z with coefficient `sqrt(3/2)`
(1.22474487 pixels/unit). Retail `T3DScene::meth_0x412150` retains that
viewport formula: constants at `0x5a37f0`, `0x5a37f8`, `0x5a3800` decode
to 65536 (double), 512 (float), 131072 (double). They were checked in
the retail executable SHA-256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Current port quad projection instead multiplies model-local Z by
`WORLD3D_Z_SCALE*0.867 = 1.3005`.

The snapshot animator translates map Z using `FIX_Z_VALUE(z)=z/1.46`,
without local Z stretch in `MakeMatrix`. Retail animator body
`0x40e460` (`recon/classes/cls_0x5a7e38.cpp`) instead uses
`(z/(1.46-z*(1/300)*0.01))*1.038`; its constants at
`0x5a351c`–`0x5a3528` were verified in the same executable. Original
`mappane.cpp::Update3DScenePos` also converts a positive-Z camera into
a zero-Z camera using integer `WorldToScreen` then `ScreenToWorld`;
camera (10000,10000,16) becomes (9987,9987,0).

Applying those source/retail-derived rules to the unchanged authored
corners predicts port-minus-retail screen Y differences
(1.861,2.583,0.247,0.969) pixels for vertices 0–3. This is a diagnostic
prediction, not a fitted adjustment or a fully verified retail camera
reconstruction. It explains why generic model/anchor projection should
be investigated before applying any Flame-specific offset or scale.
Pixel-center/rasterizer conventions and the complete retail camera
body still need reconciliation. Full-map ground, occlusion, lighting
and mounting registration remain unvalidated.

### 2026-10-04 authored geometry and map-path repair

The port now loads the four actual `box01` positions, applies the
snapshot's row-vector rotations and 0.5 scale, and then transforms each
corner through the instance world matrix. `SubmitFxQuad` uses the existing
FX-strip backend with zero extrusion to preserve all corners and UVs.
No fitted scale, position, screen bounding box, or procedural asset is used.

The old Flame setup attached both a flipbook and a particle component.
Map submission suppresses the flipbook whenever the particle component
exists, so full maps used the emitter's extra 32-unit Z offset and a
separate animation clock while the VFX preview used the flipbook. Flame
now attaches only the authored-quad flipbook component; map and preview
invoke the same virtual submission path. The old emitter block remains
in the parsed definition for format compatibility and is not attached.

The compiled lower-ambient check above now covers this geometry. A
complete capture on a shipped map and resolution of the generic
projection gap remain required. It is not marked validated.

### 2026-10-04 earlier billboard retail-capture check

The native 3dfx reference `retail-3dfx-white-20261004-a` now supplies a repeatable
visual check: MCP_AB module, nine Dunffff ground plates, camera at
(10000,10000,16), Flame at (10000,10000,96), ambient 128 with white RGB.
The port uses the captured floor as a backdrop, with no size or position fitting.
This checks the flame in isolation; it does not validate port ground rendering,
depth occlusion, or placement in full game maps.

The component path had two concrete implementation defects: it ignored the
definition's 0.5 scale, and supplied an integer frame counter to expressions
whose `frame(time, rate)` input is seconds. It now applies the declared scale
and samples a local elapsed clock at 24 authored ticks per second, wrapping
after 18 ticks. The expression uses rate 11, preserving the snapshot's exact
cel holds. Tests cover three loops, display rates of 30/60/120 Hz, and updates
crossing several authored ticks. Retail timing is still not phase-synchronized.

An isolated alpha-versus-additive comparison favors the snapshot's alpha blend
on this reference, so TorchFlame now uses alpha. Estimated-phase ROI mean
absolute RGB error fell from 6.009 before these fixes to 1.383 with scale/clock,
then 1.168 with alpha (0..255 channel units). These scores include background
pixels and use one global offset; they are not a fidelity percentage or proof
of the retail render state. Raw elapsed comparisons are retained separately.

Evidence is under `~/RevenantRetailLab/captures/ab/`: `flame-scale-01`,
`flame-clock-01`, and `flame-alpha-01`. Shape and placement still visibly differ;
the billboard approximation has not reconstructed the I3D mesh rotations or
confirmed retail render code. **The effect remains retail-partial.** The
burndown below is not marked complete by these isolated fixes.

```
- [ ] Load Magic\Flame.I3D — use the RETAIL copy from data/imagery.rvi
      (41,348 B, MD5 e108cad4d45b6b23af8ad73bafed286c). The snapshot's
      legacy/Imagery/Magic/flame.i3d differs in 7 graphics-header bytes
      (bounds/registration only, not texture dimensions); the texel data is
      identical. Address its single STILL billboard sub-object: box01
      (GetObject 0). NO procedural flame sprite. (§2.1, §4, §13.7)
- [ ] Decode the texture as a 128×128 ARGB4444 surface = 4-column × 2-row
      atlas of 8 cells (each 32×64 px). The flipbook is a UV CELL PICK
      (§8 mechanism #2), NOT a framehtexs[] swap (S3DTex.numframes = 1).
      The §4 cell-by-frame table is the authoritative atlas-decode. (§4, §8)
- [ ] Preserve authored ARGB4444 alpha and RGB. Dominant 0x0f20 is
      transparent red, not RGB565 green. No inferred chroma-key rule
      should replace the source alpha channel. (§4.1, §10)
- [ ] Spawn: one TEffect object per torch via the standard MapPane.NewObject
      path on OBJCLASS_EFFECT + FindObjType("flame"). Continuous LOOPING
      effect (no kill condition in the animator); lives as long as the
      sector / world placement keeps it alive. NOT a spell-cast effect
      (no spell.def variant invokes "FLAME"). Trigger is sector binary
      world placements in modules. (§5, §12)
- [ ] Emit anchor = the effect object's exact world (x,y,z). No
      ground-projection, no character-attach. Single quad at the literal
      spawn point. (§5)
- [ ] At animator Initialize: bind sub-object 0 (box01), allocate the
      effect-owned lvert buffer via GetVerts(obj, D3DVT_LVERTEX), seed
      frame = 0. Default the 4 verts' diffuse to OPAQUE WHITE so the
      texture passes through unmodulated under MODULATE. (§3, §6.1, §13.8)
- [ ] Per render frame (time-based — NOT framerate-stepped per the
      framerate-independent-animation feedback): advance a per-instance
      atlas-cell index using `n = floor(t_seconds * 11)`, with n wrapping
      to 0 every 18 ticks at 24 Hz nominal (i.e. n % 8 to land in the 8
      cells, but the snapshot wraps at frame=18 BEFORE the n=floor(n/24)
      cycle finishes — see §4 cell table for the exact per-frame cell
      sequence, which is the deliverable). Convert the snapshot's
      `frame * 11 / 24` math to a per-second equivalent. (§3, §6.2, §8, §13.9)
- [ ] Per render frame: write the 4 quad UVs to a 0.25 × 0.5 sub-rect of
      the texture anchored at (col*0.25, row*0.5) where (col, row) is the
      current cell (n%4, n/4). EXACTLY ONE CELL per quad — NEVER 0..1
      whole-texture UVs (the "2×2-grid bug" the protocol warns against).
      lverts[0..3].tu/tv layout: (xpos, ypos), (xpos+0.25, ypos),
      (xpos, ypos+0.5), (xpos+0.25, ypos+0.5). (§4, §6.3, §8)
- [ ] Quad orientation: build an explicit object matrix as
      RotateX(45°) · RotateY(30°) · RotateZ(160°) · Scale(0.5) with
      translation (0,0,0). OBJ3D_MATRIX + OBJ3D_VERTS (0x2100). The
      effect's parent TEffect provides world placement. NOTE: the rotation
      triple is snapshot-only and unusual; flag for visual vet against an
      in-game torch (could have been retuned for ship). The intent is a
      "WorldUpAligned" billboard (pinned Z up, ~camera-facing under iso),
      hand-tuned as 3 fixed Eulers. (§6.3, §7, §13.3)
- [ ] Per-quad scale = uniform 0.5. NO per-frame scale/alpha envelope —
      the only thing that changes per frame is the UV cell pick. (§6.3)
- [ ] Blend = ALPHA per snapshot code (SetBlendState: MODULATE,
      SRC_ALPHA/INV_SRC_ALPHA). Existing isolated comparisons favor this
      blend. Confirm the retail render body and a placed torch capture;
      perceived glow alone does not establish additive blending. The
      cls_0x5a9d88 mapping is incorrect per §2.1/§13.5. (§7, §13.1)
- [ ] Lit-mode = Unlit / self-lit. The animator does NOT zero the material
      (unlike TFlareAnimator) and does NOT mix ambient (unlike water).
      The texture renders at full sprite brightness. (§7, §10)
- [ ] Depth = TestNoWrite (ZENABLE on, ZWRITE off — set by SetBlendState
      or whatever blend the visual vet lands on). (§7)
- [ ] RefreshZBuffer: restore scene Z over a 25 × 62 pixel patch centred
      on the projected effect origin (sizes are the snapshot's
      `50.0f * 0.5f, 125.0f * 0.5f` truncated to int — the 0.5f matches
      the render scale; 50/125 describe the damage rectangle, not the
      authored quad dimensions). (§6.4)
- [ ] Texture animation mechanism: UV CELL PICK, NOT framehtexs[] swap.
      Cell index advances non-uniformly per the §4 cell-by-frame table
      (cells 0 and 5 held for 3 frames, others for 2). Per-instance frame
      counter; no explicit phase offset (per-instance variation comes from
      staggered spawn times). Wrap at frame=18. (§8, §13.9)
- [ ] Associated dynamic light: NONE. The flame does NOT emit a point
      light; world lighting is baked via static area.def fixtures. (§9)
- [ ] Color: comes entirely from the texture (no per-vertex tint, no spell
      color, no chardata field). Expected visual = bright hot orange/yellow
      flame with near-white core. Pale/gray = broken port (likely wrong
      chroma-key path keying the wrong color, or texture not loading). (§10)
- [ ] Sub-effects spawned: NONE. The animator spawns no children. No
      sparks, no smoke, no glow ring, no light. (§6)
- [ ] Audio: NONE in the effect. Any torch crackle would be a sibling
      Speaker (TAmbSoundEffect) placed alongside the flame in sector data
      — record this for the audio phase but do NOT bundle it into F01. (§11)
- [ ] Registration name = "FLAME" (uppercase). REGISTER_3DANIMATOR("FLAME",
      TFlameAnimator) MUST be present — retail has it; the snapshot
      doesn't. Without the registration the animator builder won't pair
      to the effect at spawn time. (§2.1, §13.4)
- [ ] Do NOT reconstruct: the cls_0x5a9d88 fields/methods (mis-mapped, §13.5);
      a framehtexs[] flipbook (this is UV-based, §8); a particle emitter or
      spark sub-emitter (none exist in the snapshot's TFlameAnimator); a
      dynamic light (§9); per-frame alpha/scale curve (none). The task
      hint mentioning a "spark sub-emitter" is incorrect for this effect —
      the snapshot's TFlameAnimator body at effect_old.cpp:4486-4578 is
      single-quad-only. (§6, §9, §13)
```

**Definition of done:** every torch / candle / brazier sconce in the
world draws as a single authored-quad hot-orange/yellow flame with a near-
white core (real `Magic\Flame.I3D` texels and alpha), oriented in a
fixed (45°/30°/160°) Euler at uniform scale 0.5, animated as a UV-cell
flipbook walking the 8-cell 4×2 atlas in the §4 non-uniform sequence on
a ~0.75 s loop (per-instance, naturally desynced by spawn time), drawn
either as straight Alpha (snapshot-faithful) or AdditiveStraight (likely
shipped — see §7 / §13.1) on a depth-test-no-write pass with a 25×62 px
Z-restore patch, with NO emitted light, NO particles, NO audio, and NO
kill condition — the torch lives as long as its sector is loaded.
