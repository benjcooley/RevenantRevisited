# F01 TFlameEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F01 (covers F01 `TFlameEffect` + F02 `TFlameAnimator`) |
| **Class(es)** | `TFlameEffect` (object, near-empty shell — no override bodies) + `TFlameAnimator` (the visual: one `int32_t frame` field + `Initialize`/`Animate`/`Render`/`RefreshZBuffer`). |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Retail fidelity** | **retail-partial** — registration string `"FLAME"` is **retail-confirmed** (`s_FLAME_005e10f4` XREF'd from the animator-builder factory table @ `005c5348→0x4e4ea0`, the same registration mechanism as B01/Sparks); asset `Magic\Flame.I3D` is in shipped `data/imagery.rvi` but **NOT byte-identical** to snapshot — **7 bytes differ in the surface descriptor at file offset 0x48-0x4E** (snapshot's surface dims 339×316 are nonsensical for the 41,348 B file; retail's 128×160 fit the 40,960 B payload exactly). The texture **pixel data is unchanged**; the asset's loader-visible metadata was fixed. The `TFlameAnimator::Animate/Render` bodies could **NOT** be cross-checked against a readable retail decomp — `cls_0x5a9d88` (the MEDIUM candidate via the `"FLAME"` XREF) has a 264 B layout + 25 vftable entries that do **NOT** match a tiny `T3DAnimator + int32_t frame` class (the snapshot TFlameAnimator class size is much smaller). The render body at `0x4e4ea0` lives as a free-function trampoline that the recon pass didn't isolate. Net: name + asset corroborated, kinematic constants (the frame-wrap 18, the `frame*11/24` UV-cell math, the 45°/30°/160° rotations, the 0.5 scale, the Alpha blend) are **snapshot-only** and the **render blend is the top drift risk** (torches glow → textbook additive sprite design; snapshot codes Alpha — flagged in §7). Retail also added two color-variant assets (`flameb.i3d`, `flameg.i3d`) not present in snapshot; effect class unchanged. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-29 |
| **Family** | fire (single-card flipbook torch flame — the iconic dungeon ambient torch/candle/brazier) |
| **Draws** | a **single ScreenAligned billboard quad** — one `box01` sub-object of `Magic\Flame.I3D` drawn with a UV sub-rect picked off a **4-column × 2-row atlas** (8 frames in one 128×160 RGB565 surface). |
| **Archetype(s)** | **(A) UV-coordinate atlas-cell pick flipbook** — `tu/tv` recomputed per-frame from a per-render-frame counter (`frame * 11 / 24`) to land on one of 8 atlas cells, NOT a `framehtexs[]` texture-handle swap. **(B) continuous loop** — `frame` cycles `0..17` forever, no kill condition in the animator (the effect object lives until its owning sector / spell kills it). No particle emitter, no sub-emit, no associated light, no audio. |

---

## 1. Summary

`"FLAME"` is the iconic Revenant torch/candle flame seen in every dungeon, tavern,
keep, and ambient lighting scene. It is the simplest possible mesh-effect: a
**single billboard quad** holding one I3D sub-object (`box01`), with a
**static-cell-pick UV flipbook** that rewrites the four corner UVs each frame to
land on one of **8 cells** in a **4×2 atlas** packed into a single 128×160 RGB565
texture (`Magic\Flame.I3D`). The frame counter loops `0..17` indefinitely (the
mapping `n = frame * 11 / 24` makes most cells hold for ~2 frames, giving a
non-uniform flicker), and the quad is drawn through an explicit object matrix
that scales by 0.5 and rotates by (45° X, 30° Y, 160° Z). There is **no particle
emitter, no sub-emitted sparks, no dynamic light, no audio, and no state
machine** — every torch in the world is one of these, lit forever, drawn each
render frame. The visual identity is the **bright hot orange/yellow flame with
near-white core on a green chroma-key background** (the `0x0f20` RGB565 = (8,
228, 0) green is 54% of the texture pixels — flame.i3d is **green-keyed**, NOT
black-keyed; this is the asset-side fact that decides the chroma-key handling
in any consumer).

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

**(1) Asset identity — NEAR-IDENTICAL, with a 7-byte surface-header fix.**
- Snapshot `legacy/Imagery/Magic/flame.i3d`: 41,348 B, MD5
  `736514e58a772c2cc7040543b919150a`.
- Retail (extracted from `data/imagery.rvi` member `Imagery/Magic/flame.i3d`):
  41,348 B, MD5 `e108cad4d45b6b23af8ad73bafed286c`.
- `cmp -l` reports **7 differing bytes, all at file offsets 0x48-0x4E**
  (inside the `SSurfaceDesc` block at 0x44):
  | offset | snapshot bytes (u16 LE) | retail bytes (u16 LE) |
  |--------|------------------------|------------------------|
  | 0x48-0x49 | 0x20, 0x01 (some flag) | 0x20, 0x01 (same) |
  | 0x4A-0x4B | 0x64, 0x00 (= 100) | 0x64, 0x00 (same) |
  | 0x4C-0x4D | **0x0153 = 339** | **0x0080 = 128** ← surface width |
  | 0x4E-0x4F | **0x013c = 316** | **0x00a0 = 160** ← surface height |
  | 0x50-0x51 | **0x0153 = 339** | **0x0040 = 64**  ← cell width |
  | 0x52-0x53 | **0x0087 = 135** | **0x0078 = 120** ← cell height |
- The snapshot's surface dimensions (339×316) are **nonsensical** for the
  41,348 B file (would require 214 KB of payload at 16-bit). The retail dims
  (128×160 → 40,960 B at 16-bit) fit the **40,976-byte payload from 0x174 to
  end** exactly. **The texel data is unchanged**; the snapshot asset would
  mis-decode at load time (or be corrected by a loader heuristic). This is a
  late asset-header fix, not a content change.
- The **8 of `data/imagery.rvi` total `flame*.i3d` members** also include
  `flameb.i3d`/`flameg.i3d` (33,404 B, 1999-03-17) and
  `Equip/flamearrow.i3d` (23,362 B, 1999-09-02) — none of which the snapshot
  has. These are color-variant + flame-arrow add-ons; the F01 effect class
  itself consumes the orange `Magic\flame.i3d` first-registered.

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
**the render blend** (§7 BLEND SANITY-CHECK: a bright-on-green-key glow
sprite is textbook additive; the snapshot codes Alpha — flag and
visually vet); secondary risk is the explicit rotation triple (does the
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
| asset surface dims | 128×160 RGB565 (retail) / 339×316 (snapshot, bogus) | px | retail bytes 0x4C-0x4F | **yes (retail)** for 128×160 (matches the 40,960-byte payload exactly); snapshot-bogus |
| asset cell-dim hint | 64×120 (retail) / 339×135 (snapshot, bogus) | px | retail bytes 0x50-0x53 | yes (retail) for 64×120 — but see §4 / §13.2 (doesn't tile the 128×160 surface) |
| asset payload offset | starts at `0x174`, runs to `0xa174` = exactly 128×160×2 B | bytes | computed | yes |
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

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Flame | snapshot `legacy/Imagery/Magic/flame.i3d` (41,348 B, MD5 `736514e58a772c2cc7040543b919150a`); retail `data/imagery.rvi:Imagery/Magic/flame.i3d` (41,348 B, MD5 `e108cad4d45b6b23af8ad73bafed286c`) — **same texel payload, differ in 7 surface-header bytes** (§2.1) | 41,348 B | the torch-flame sprite — STILL 2D billboard I3D with **1 sub-object** (`box01`) | registered `Class.Def:2030` under name `"Flame"` (id `0x50ba373b`); loaded by the OBJCLASS_EFFECT registry on instance spawn; the sole sub-object bound via `GetObject(0)` and its verts pulled via `GetVerts(obj, D3DVT_LVERTEX)` (`effect_old.cpp:4490-4491`) so the effect owns its own `obj->lverts[0..3]` it mutates each render |

**Sub-objects (1)** — confirmed from the I3D object table at file offset
`0x7c0`: count = 1, name = `box01` (`0x7c4: '62 6f 78 30 31 00'` = `"box01"`).
A single billboard quad. No multiple flame cards, no spark sub-object, no
additional geometry. (Both snapshot and retail asset have the same 1-count
+ same `box01` name.)

**Texture decode (the §4 atlas decode — read off the actual UVs):**

- **Dimensions (retail):** **128 × 160 pixels**, **RGB565 16-bit**
  (`data[0x4C-0x4F] = 0x0080 0x00a0`). Payload runs `0x174 → 0xa174` =
  `128 * 160 * 2 = 40,960` bytes exactly. **One physical surface — there is
  no `framehtexs[]` per-frame texture handle array.** `S3DTex.numframes = 1`
  for this asset (no per-frame texture swap; the flipbook is purely UV-based).
- **Atlas layout:** **4 columns × 2 rows = 8 cells** of **32 × 80 pixels**
  each. This is **decoded from the actual UV math in the snapshot's Render
  body** (`effect_old.cpp:4543-4556`):
  - Column stride `0.25` (UV) = 1/4 of the surface width = `0.25 * 128 = 32 px`.
  - Row stride `0.5` (UV) = 1/2 of the surface height = `0.5 * 160 = 80 px`.
  - So one cell = `(col*32 .. (col+1)*32, row*80 .. (row+1)*80)` in pixels.
- **Per-cell contents (visually inferred — must be vetted against the decoded
  texture):** 8 stages of a flickering flame — each cell is a `32 × 80`
  vertical flame still. The bottom of the cell is the flame base (texture
  origin in I3D STILL convention), the top is the flame tip. Cells (0,0)
  through (3,1) are 8 sequential flicker poses (likely an animator-authored
  loop: flame growing → peaking → diminishing → growing again, but the
  specific per-cell pose mapping isn't documented in source and the §6
  temporal math doesn't traverse them linearly — see the cell-by-frame table
  below).
- **The UV mapping (the actual mapping from the Render body, NOT 0..1)** —
  per `effect_old.cpp:4543-4556`, every render frame computes `xpos = col *
  0.25; ypos = row * 0.5` (the cell's *origin* UV), then writes the four
  quad-corner UVs as a 0.25 × 0.5 sub-rect anchored at `(xpos, ypos)`:
  ```
  lverts[0].tu/tv = (xpos,        ypos)        ; top-left
  lverts[1].tu/tv = (xpos + 0.25, ypos)        ; top-right
  lverts[2].tu/tv = (xpos,        ypos + 0.5)  ; bottom-left
  lverts[3].tu/tv = (xpos + 0.25, ypos + 0.5)  ; bottom-right
  ```
  So **the quad samples exactly one 0.25 × 0.5 sub-rect = one of the 8
  cells**, NOT the whole texture. Any reconstruction that maps the full
  texture (UV `(0,0)-(1,1)`) onto the quad will draw all 8 cells in a 4×2
  grid — the "atlas-grid bug" the protocol explicitly warns against.
- **Animation mechanism (cell-by-frame, the §8 table):** the cell is
  selected by `n = (int32_t)(frame * 11 / 24); col = n % 4; row = n / 4`,
  with `frame` cycling 0..17. This produces the following sequence over one
  18-frame loop (each row is one render frame):

  | frame | `n` | (col, row) | (xpos, ypos) UV | held for (frames) |
  |-------|-----|------------|-----------------|-------------------|
  | 0  | 0 | (0, 0) | (0.00, 0.0) | 3 |
  | 1  | 0 | (0, 0) | (0.00, 0.0) | – |
  | 2  | 0 | (0, 0) | (0.00, 0.0) | – |
  | 3  | 1 | (1, 0) | (0.25, 0.0) | 2 |
  | 4  | 1 | (1, 0) | (0.25, 0.0) | – |
  | 5  | 2 | (2, 0) | (0.50, 0.0) | 2 |
  | 6  | 2 | (2, 0) | (0.50, 0.0) | – |
  | 7  | 3 | (3, 0) | (0.75, 0.0) | 2 |
  | 8  | 3 | (3, 0) | (0.75, 0.0) | – |
  | 9  | 4 | (0, 1) | (0.00, 0.5) | 2 |
  | 10 | 4 | (0, 1) | (0.00, 0.5) | – |
  | 11 | 5 | (1, 1) | (0.25, 0.5) | 3 |
  | 12 | 5 | (1, 1) | (0.25, 0.5) | – |
  | 13 | 5 | (1, 1) | (0.25, 0.5) | – |
  | 14 | 6 | (2, 1) | (0.50, 0.5) | 2 |
  | 15 | 6 | (2, 1) | (0.50, 0.5) | – |
  | 16 | 7 | (3, 1) | (0.75, 0.5) | 2 |
  | 17 | 7 | (3, 1) | (0.75, 0.5) | – |

  So all 8 cells are visited each cycle, with cells **(0,0) and (1,1)
  held for 3 frames** and the others held for **2 frames** — the
  `frame * 11 / 24` integer division gives a deliberately non-uniform
  flicker (most cells get ~2.18 frames at the average rate, but the
  rounding clusters extras on cells 0 and 5). At a nominal 24 Hz the full
  cycle takes `18/24 = 0.75 s`. **This is the source of the flame's
  natural irregular flicker — it is NOT a uniform 8-cell flipbook.**

- **Chroma key — GREEN, NOT BLACK.** Histogramming the 20,480 texture
  pixels:
  | RGB565 | (R, G, B) 8-bit | count | % |
  |--------|-----------------|-------|---|
  | `0x0f20` | (8, 228, 0) | 11,057 | 54.0 % ← **green chroma key** |
  | `0x0000` | (0, 0, 0) | 3,620 | 17.7 % |
  | `0x1f30` | (24, 228, 128) | 491 | 2.4 % |
  | `0xfffd` | (248, 252, 232) | 253 | 1.2 % ← near-white flame core |
  | `0xfff8` | (248, 252, 192) | 201 | 1.0 % |
  | `0xfff9` | (248, 252, 200) | 191 | 0.9 % |

  The dominant background color is **bright green `(8, 228, 0)`**, not
  black. The 20%-black-pixel heuristic the port loader uses
  (`src/3dimage.cpp:1701-1721`, per knowledge 02 §4.1) would **not fire**
  on this asset — its background is green-keyed. The 17.7 % pure-black
  pixels are mostly the *inside* of the flame (dark core where flame texels
  are intentionally `0,0,0` to read as black under MODULATE + per-vertex
  diffuse, rather than as transparent). A reconstruction needs to chroma-
  key the GREEN, not the BLACK, on this asset. **This is a per-asset
  chroma convention divergence from the Blood/Sparks/black-keyed family.**

The effect loads a real asset — do NOT substitute a procedural flame
sprite. The hot near-white core, hot-yellow flame body, and bright-green
chroma key are the visual identity (§10).

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
   │      /│           (one box01 sub-object, 32×80 px source cell)
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

The `50.0f * 0.5f` and `125.0f * 0.5f` are clearly the
**asset's cell-dimension hint** (`64 × 120` from the retail surface
descriptor) downscaled by the **same 0.5 scale** the Render body applies —
roughly matching the flame's on-screen footprint. (Why 50/125 instead of
64/120? Likely a hand-picked tighter patch. The cast-to-int truncates 62.5
to 62.) `RestoreZ` then issues `Scene3D.RestoreZBuffer(rect)` per the
free-function helper at `effect_old.cpp:162-171`.

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

  ### BLEND SANITY-CHECK (mandatory)

  **Sprite design:** the flame texture is a bright-white-core + hot-yellow
  flame on a **bright-green (chroma-keyed) background** (§4 histogram: 54%
  green-key pixels, 17.7% black core pixels, the rest hot whites/yellows
  with greenish edge bleed). A bright-on-keyed-background sprite is the
  **textbook additive sprite** (the chroma background reads as transparent
  under additive; the flame contributes additively to the lit scene like
  a torch should). Torches in the game world **glow** — they light up the
  surrounding stone, brass, the player's clothes. Drawing them with
  straight Alpha mutes that glow effect (the flame becomes a translucent
  overlay rather than a self-luminous additive contribution).

  **The snapshot code says Alpha. This is SUSPECT for two reasons:**
  1. Bright-on-key sprite design + observed in-game glow strongly suggest
     **AdditiveStraight** in the shipped game.
  2. The retail render body at `0x4e4ea0` is NOT decompiled (§2.1), so the
     blend choice is **snapshot-only** and unverified — exactly the
     "snapshot Alpha on a glow is unverified" drift the SPARKS exemplar
     (§2.1) confirmed happens for sparks (snapshot Alpha → shipped
     Additive). The same risk applies here.

  **Sister-family cross-check:**
  - `TFlareAnimator::Render` (`src/effect_old.cpp:584-612`) — **Alpha**
    (`SetBlendState`). Glow-family sister, same blend.
  - `TSymGlowAnimator::Render` (`src/effect_old.cpp:4642-4665`) — **Alpha**
    (`SetBlendState`). Glow-family sister, same blend.
  - **Both `TFlareAnimator` and `TSymGlowAnimator` zero their material at
    `Initialize` to make the flat-color blend read as self-lit emissive**
    (`TFlareAnimator::Initialize` `:517-550` — ambient/diffuse/specular/
    emissive/power all 0). **`TFlameAnimator::Initialize` (`:4486-4494`)
    does NOT zero its material.** So `TFlameAnimator` is more naive
    than its sisters: it uses the imagery's authored material *and* the
    Alpha blend. The authored I3D material is unread by the snapshot's
    Render path (per-vertex diffuse is what MODULATE uses, and the
    snapshot Render never writes `lverts[i].color`), so the per-vertex
    diffuse defaults to whatever `GetVerts` initialized — most likely
    opaque white (1.0, 1.0, 1.0, 1.0), so the texture passes through
    unmodulated.
  - The contrast with the additive fire-family glow effects (FireFlash,
    FireWind, Burn, Aura — all `SetAddBlendState`, knowledge 03 §1.1) is
    notable: the spell-fire effects are additive, the *ambient* fire
    (Flame, Fault Fire) is Alpha per the snapshot. **Whether this is
    intentional differentiation or a snapshot bug for the ambient
    family is the open question.**

  **Verdict (with the SPARKS-protocol caveat):** classify the snapshot as
  **Alpha** per code, but **flag it as snapshot-only-and-suspect** for the
  reconstruction. A bright-green-keyed glow being rendered Alpha rather
  than Additive is exactly the F03/Sparks pattern that turned out
  retail-wrong (the SPARKS doc §2.1 documented snapshot Alpha → shipped
  Additive for spark sprites). The reconstruction agent **must visually
  vet against an in-game torch capture** (§12) and be ready to switch to
  AdditiveStraight if the snapshot blend reads dull/translucent on the
  scene.

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
- **Orientation:** **ScreenAligned-with-explicit-tilt** — see §6.3 note.
  Not a canonical WorldXY (no `rot.x = -π/2`), not a bare ScreenAligned
  (matrix builds explicit (45°, 30°, 160°) rotation). The flame quad
  presents at a fixed orientation, NOT camera-aligned per render frame.
  Whether this reads "facing camera enough" under the iso projection is a
  property of how the iso camera is also tilted by ~30°; the snapshot
  values appear tuned for the iso view. **Per NOMENCLATURE §2, this
  effect would want a `WorldUpAligned` enum (future-noted as not yet
  implemented) — pin the quad's up axis to world +Z and let it rotate
  about Z to face the camera. The snapshot's static 3-Euler rotation is
  the original's approximation of that, fixed for the iso view.**
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
no per-frame texture handles), confirmed by the 40,960-byte payload fitting
exactly one 128×160×2 surface and the asset header at offset 0x174 reading
`numframes=1`. So the flipbook is **purely UV-based** — no
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

- **Source:** **the authored `Magic\Flame.I3D` texture** (§4). Texture pixel
  data confirmed: hot near-white core `(248, 252, 232)` + bright yellow
  body `(248, 252, 192-216)` + transitions through yellow/orange to the
  green chroma-keyed background `(8, 228, 0)`. The effect supplies **no
  per-vertex tint** (`lverts[i].color` is never written by Render) and
  **no spell color** (there is no live spell caller, §12). The hue is
  entirely the texture's.
- **Exact values:** the dominant hot colors are
  - `0xfffd` (248, 252, 232) — flame core, near-white
  - `0xfff8` (248, 252, 192) — pale yellow
  - `0xfff9` (248, 252, 200) — pale yellow
  - `0xfffb` (248, 252, 216) — pale yellow-white
  - `0x0f20` (8, 228, 0) — **green chroma key** (54% of pixels — the
    background)
  - `0x0000` (0, 0, 0) — black (17.7% — internal flame core where the
    texture deliberately fades to black inside the flame body)
- **Expected visual:** **bright hot orange/yellow flame with a near-white
  core, on a transparent (green-keyed) background.** Saturated warm hot
  color, NOT pale. Per the `feedback-vfx-color-health-signal` memory, a
  pale/gray/missing-color flame at reconstruction = broken port (likely
  causes: wrong chroma-key handling — code looking for black but the
  asset is green-keyed; wrong blend — Alpha when it should be additive;
  the texture not loaded at all and the renderer falling back to a
  default; or vertex color writing something non-white).
- **Normalization / boosts:** none — there is no NormalizeColors or
  ambient-mix or hilt-brightness code in the flame path.

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

- **13.1 The render blend is the headline snapshot-drift risk
  (snapshot-only-and-suspect — vet vs ground truth).** Per §7's BLEND
  SANITY-CHECK: the snapshot says **Alpha** (`SetBlendState`), but the
  sprite design (bright hot-white-core flame on green chroma-key) +
  the in-game intent (a torch GLOWS) point to **AdditiveStraight**. The
  retail render body at `0x4e4ea0` is **NOT** decompiled, so the blend
  is unverified. The SPARKS exemplar (§2.1) documented exactly this
  pattern shipping Additive despite a snapshot-Alpha; F01 carries the
  same risk. **Reconstruction MUST visually vet against a torch capture
  and be ready to switch to AdditiveStraight.** If the reconstructed
  flame reads dull, translucent, or as a sticker rather than a glow,
  the blend is wrong.
- **13.2 The asset's cell-dimension hint (`64×120`) doesn't tile the
  surface evenly.** The retail surface descriptor at byte 0x50-0x53
  reports cell dims of 64×120, but the actual UV math (cells of 32×80
  px from `0.25 × 0.5` UV strides on the 128×160 surface) gives a
  different geometry. The 64×120 likely represents the *intended on-
  screen quad size* (i.e. authored render dims for the I3D STILL
  imagery), not the atlas cell size. Reconstruction: trust the UV math
  for atlas decoding (8 cells of 32×80), not the header hint.
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
- **13.7 Snapshot asset surface header is bogus.** The 7-byte
  difference at offsets 0x48-0x4E (snapshot's surface dims 339×316
  vs. retail's 128×160, snapshot cell 339×135 vs. retail 64×120)
  means the snapshot asset would mis-decode the texture in the
  load path. The texel data is correct; only the metadata is wrong.
  The retail asset is the one to consume. (This is a pure asset-side
  fix, not an effect-code change.)
- **13.8 The Render body never writes per-vertex color.** Under
  MODULATE the texture is multiplied by whatever default `lverts[i].
  color` `GetVerts` leaves in place (typically opaque white). This is
  fine if the default is `(1, 1, 1, 1)`; if the engine's `GetVerts`
  default for `D3DVT_LVERTEX` is some other value (e.g. 0 alpha →
  invisible) the flame would not draw at all. Reconstruction must
  ensure the per-vertex diffuse defaults to opaque white. This is
  noted in the doc rather than the burndown because it is an engine
  invariant rather than an effect-specific behavior.
- **13.9 Per the framerate-independent animation feedback,
  reconstruction must convert `Animate` frame-stepping to a time-
  based rate.** The snapshot increments `frame` per `Animate` call,
  not per sim tick — strictly framerate-dependent. Reconstruction
  should integrate the cell-advance by real `dt` at a rate of
  ~10.91 cells/sec (= 11/24 cells per frame × 24 frame/sec target).
  The non-uniform 2 vs 3 frame holds are baked into the `*11/24`
  integer math; the time-based equivalent is `cell = floor(t *
  (11/24) * 24)` (or equivalently, `cell = floor(t * 11)` where
  `t` is seconds since spawn), which preserves the non-uniform
  holds exactly.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\Flame.I3D — use the RETAIL copy from data/imagery.rvi
      (41,348 B, MD5 e108cad4d45b6b23af8ad73bafed286c). The snapshot's
      legacy/Imagery/Magic/flame.i3d differs in 7 surface-header bytes
      (bogus surface dims 339×316 vs retail's 128×160); the texel data is
      identical. Address its single STILL billboard sub-object: box01
      (GetObject 0). NO procedural flame sprite. (§2.1, §4, §13.7)
- [ ] Decode the texture as a 128×160 RGB565 surface = 4-column × 2-row
      atlas of 8 cells (each 32×80 px). The flipbook is a UV CELL PICK
      (§8 mechanism #2), NOT a framehtexs[] swap (S3DTex.numframes = 1).
      The §4 cell-by-frame table is the authoritative atlas-decode. (§4, §8)
- [ ] Handle CHROMA KEY as GREEN, not black: the dominant background color
      is RGB565 0x0f20 = (R=8, G=228, B=0) at 54% of pixels (NOT the
      knowledge-base's black-pixel heuristic — that would fail on this
      asset and leave the green visible). The 17.7% pure-black pixels are
      INTERNAL flame core texels (intentionally black; do NOT key those).
      Decision rule: green-key pixel ⇒ alpha=0 (premultiplied);
      keep black core pixels opaque. (§4 chroma-key section, §13)
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
      SRC_ALPHA/INV_SRC_ALPHA), BUT FLAG IT AS SNAPSHOT-ONLY-AND-SUSPECT:
      the sprite design (bright-on-green-key glow) + in-game intent
      (torches GLOW) point to AdditiveStraight; the retail render body is
      not decompiled (cls_0x5a9d88 is mis-mapped per §2.1/§13.5); the
      SPARKS exemplar §2.1 confirmed a similar snapshot-Alpha → shipped-
      Additive pattern. VISUALLY VET against a torch capture and be ready
      to switch to AdditiveStraight if the snapshot blend reads dull. (§7, §13.1)
- [ ] Lit-mode = Unlit / self-lit. The animator does NOT zero the material
      (unlike TFlareAnimator) and does NOT mix ambient (unlike water).
      The texture renders at full sprite brightness. (§7, §10)
- [ ] Depth = TestNoWrite (ZENABLE on, ZWRITE off — set by SetBlendState
      or whatever blend the visual vet lands on). (§7)
- [ ] RefreshZBuffer: restore scene Z over a 25 × 62 pixel patch centred
      on the projected effect origin (sizes are the snapshot's
      `50.0f * 0.5f, 125.0f * 0.5f` truncated to int — the 0.5f matches
      the render scale; the 50/125 are the authored sprite footprint). (§6.4)
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
world draws as a single ~32×80-pixel hot-orange/yellow flame with a near-
white core (real `Magic\Flame.I3D` texels, green-keyed), oriented in a
fixed (45°/30°/160°) Euler at uniform scale 0.5, animated as a UV-cell
flipbook walking the 8-cell 4×2 atlas in the §4 non-uniform sequence on
a ~0.75 s loop (per-instance, naturally desynced by spawn time), drawn
either as straight Alpha (snapshot-faithful) or AdditiveStraight (likely
shipped — see §7 / §13.1) on a depth-test-no-write pass with a 25×62 px
Z-restore patch, with NO emitted light, NO particles, NO audio, and NO
kill condition — the torch lives as long as its sector is loaded.
