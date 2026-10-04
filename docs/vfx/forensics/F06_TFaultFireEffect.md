# F06 TFaultFireEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F06 (covers F06 `TFaultFireEffect` + the paired `TFaultFireAnimator`) |
| **Class(es)** | `TFaultFireEffect` (object, near-empty shell — only `Initialize()` empty + `Pulse()` = `TEffect::Pulse()`) + **`TFaultFireAnimator`** (the visual — owns `float th`, `float tvs[4]`, drives Animate/Render). The mechanism lives entirely in the animator. |
| **Status** | forensics-complete (see §13 for genuine unknowns) |
| **Retail fidelity** | **retail-partial** — asset byte-identical to shipped (MD5 match), registration name `"FaultFire"` confirmed (string at retail `005e11dc`, 2 XREFs into the builder/animator factory entries at table `005c53e8`/`005c53ec`), structure size differs from snapshot (retail `cls_0x5ac83c` for the effect candidate = 400 bytes, snapshot `TFaultFireEffect` is a near-empty `TEffect`-shell — see §2.1 for divergence); **all per-tick / per-frame constants (`FF_STEP=0.1`, the `tv -= 0.01` baseline, the `0.125 * (cos+7)` tv-scale, the `random(2,8)/100` tu-scroll, `scl=(0.25,0.25,1.0)`, `RotateZ(π/2)`, `pos.z=32`, RefreshZBuffer 32×128) are snapshot-only — none of their IEEE bit patterns or decimal forms grep into the retail Ghidra decomp**. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-29 |
| **Family** | fire (spell-cast — rising flame along a fault line) |
| **Draws** | UV-animated billboard — **one quad** (sub-object 0 / `launch`'s `box01`), drawn **twice per frame** with two phase-offset tv-scales (`cos(th)` and `cos(th+π/2)`) for a beat/flicker compound effect; tu continuously scrolls across frames (flow). |
| **Archetype(s)** | (A) Texture / UV animation — **continuous tu-scroll** (NOT a `framehtexs[]` flipbook; NOT an atlas — the I3D quad has full 0..1 UVs). (B-ish) I3D geometry with explicit transforms — `Scale(0.25, 0.25, 1.0)` then `RotateZ(π/2)` then `Translate(0,0,32)`. (F2) Custom procedural logic — periodic tv-scale via `0.125*(cos(th)+7)`, two layered passes at 90°-offset phase. NO particles. NO state machine. NO associated dynamic light. NO audio. |

---

## 1. Summary

`"FaultFire"` is a spell-cast fire effect that draws a **rising flame column along
a fault line** by continuously scrolling the U coordinate of a single billboard
quad over time (so the texture flows horizontally across the quad), while
re-scaling the V coordinate each frame with a cosine envelope (so the flame
appears to throb / lick vertically). The same quad is drawn **twice per frame**
with a 90°-offset phase between the two passes, giving a layered two-flame beat
that reads as a flickering rising fire rather than a single static texture. The
geometry is a single 64×64-wu quad (sub-object 0 of `Magic\faultfire.i3d`, called
`launch`, holding the `box01` mesh), rotated 90° in-plane (`RotateZ(π/2)`),
scaled to (0.25, 0.25, 1.0) (so X/Y = 16 wu, Z = 64 wu — a tall narrow column),
and placed at the effect's local +Z=32 wu (raised slightly off ground), drawn
**Alpha-blended** per the code with the authored RGBA4444 alpha-masked fire
sprite (white-hot at top → invisible at bottom). No particles, no sub-emitter,
no light, no sound. The whole animator is `float th, tvs[4]` + ~75 lines of
Animate+Render.

> **Authoritative mechanism = `TFaultFireAnimator`** (`src/effect_old.cpp:11147-11237`).
> `TFaultFireEffect` is an empty shell: `Initialize()` is `{}`, `Pulse()` just
> calls `TEffect::Pulse()` (`src/effect_old.cpp:11130-11137`). The animator's
> `Animate` advances `th` and `tu`, `Render` draws the quad twice with two
> phase-offset tv-scales, `RefreshZBuffer` restores Z over a 32×128 px patch.
> No spell.def or area.def variant in the shipped or snapshot data references
> `"FaultFire"` (§12) — the effect is **vestigial / pre-release-only at the
> caller side** even though the asset + registration ship in retail.

---

## 2. Sources & evidence

- **Retail decomp:** **SPARSE.** Mapping
  `recon/mappings/TFaultFireEffect_cls_0x5ac83c_candidate.yaml` ties the retail
  `cls_0x5ac83c` to `TFaultFireEffect` (medium confidence) on the basis of the
  `"FaultFire"` string at `005e11dc` having exactly 2 XREFs into functions
  inside `0x4f13c0` (`004f1530`, `004f1570`) which are members of `cls_0x5ac83c`.
  Decompiled bodies available in `recon/classes/cls_0x5ac83c.cpp`:
  - `virt_meth_0x4f13c0` (the function that references "FaultFire" string —
    builder/initializer pattern; sets a sub-object's RGB to `0x3e99999a` (=0.3)
    on all 3 axes and a position from `mbr_0xfc/0x100/0x104`).
  - `virt_meth_0x4f9010` / `virt_meth_0x4f9040` / `virt_meth_0x4f90f0` —
    standard ctor/dtor/factory pattern; `meth_0x4f9040` allocates 0x128 bytes
    and stores `0x40, 0x80, 0x30` at `this_00[4]+0x2c/0x30/0x34` (likely a
    default size_x=64, size_y=128, padding/z=48 — close to but **NOT** the
    snapshot's `RefreshZBuffer` 32×128).
  - The `Animate` / `Render` bodies — the parts that hold every constant in §3 —
    are **NOT decompiled** in retail. The candidate yaml lists no methods.
- **Pre-release (snapshot — authoritative for the mechanism):** `src/effect_old.cpp`
  - Registration: `DEFINE_BUILDER("FaultFire", TFaultFireEffect)` `src/effect_old.cpp:11127`;
    `REGISTER_BUILDER(TFaultFireEffect)` `:11128`.
  - `TFaultFireEffect` bodies: `Initialize` `:11130-11132` (empty), `Pulse`
    `:11134-11137` (just `TEffect::Pulse()`). Class `src/effect.h:2200-2212`.
  - `TFaultFireAnimator`: `REGISTER_3DANIMATOR("FaultFire", TFaultFireAnimator)`
    `src/effect_old.cpp:11145`. Bodies: `SetupObjects` `:11147-11157`,
    `Initialize` `:11159-11163`, `Animate` `:11165-11180`, `Render`
    `:11182-11224`, `RefreshZBuffer` `:11226-11237`. Class
    `src/effect.h:2220-2243`; `#define FF_STEP 0.1` `effect_old.cpp:11143`;
    `#define FF_GROUPS 10` `effect.h:2218` (unused — see §13.5).
- **Asset:** `Magic\faultfire.i3d` registered `legacy/Class.Def:2071`
  (`"Faultfire" "Magic\faultfire.i3d" 0x51753bce`); file
  `legacy/Imagery/Magic/faultfire.i3d` (33,688 B). Shipped at retail
  `data/imagery.rvi:Imagery/Magic/faultfire.i3d` (33,688 B), **byte-identical**
  (MD5 `21cb69a1e4053d652dada99407885af1` for both).
- **Blend helpers:** `SetBlendState`/`SetAddBlendState`/`Save`/`Restore`
  `src/effect_old.cpp:181-244` — used to classify the Render blend.
- **Sister effects consulted (fire family):**
  - `TFlameAnimator` (F01, `src/effect_old.cpp:4249+`) — `SetBlendState` (Alpha),
    UV-subrect atlas pick — same blend class.
  - `TFireAnimator` (F03, `src/effect_old.cpp:4645+`) — `SetBlendState` (Alpha),
    `framehtexs[]` flipbook — same blend class.
  - `TFireSwarmAnimator` (F05, `src/effect_old.cpp:11617+`) — `SetBlendState`
    (Alpha) — same blend class.
  - `TFireConeAnimator` (`src/effect_old.cpp:10138+`) — `SetAddBlendState`
    (AdditiveStraight) — DIVERGENT in family (FireCone uses additive).
  - `TDragonFireAnimator` (`src/effect_old.cpp:10396+`) — `SetAddBlendState`
    — also DIVERGENT (additive).
  - The fire family is **not uniform on blend**: ground-attached / single-sprite
    fires (Flame, Fire, FireSwarm, FaultFire) tend to be Alpha (rely on the
    sprite's authored alpha mask), while burst/cone/breath effects (FireCone,
    DragonFire) tend to be Additive.
- **Source-of-truth ranking:** the **snapshot is authoritative for the
  mechanism** (full Animate/Render/SetupObjects bodies present; retail
  equivalent not decompiled). The **retail decomp + asset + registration table**
  corroborate existence, name, and asset identity but not the rendering
  numbers; treat the snapshot constants as the design intent and visually vet
  against in-game ground-truth where available (see §12 — **vestigial** caller,
  so ground-truth may not exist in the shipped game).

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp` is a pre-release snapshot. Cross-checks:

**1. Asset identity — IDENTICAL (strong corroboration).** The snapshot asset
`legacy/Imagery/Magic/faultfire.i3d` (33,688 B) and the shipped asset packed in
`data/imagery.rvi` member `Imagery/Magic/faultfire.i3d` (33,688 B) are
**byte-for-byte identical**: MD5 `21cb69a1e4053d652dada99407885af1` for both
(`cmp` reports identical). The fire sprite the effect consumes is unchanged
between snapshot and ship — geometry, the single `box01` quad, and the
128×128 RGBA4444 fire texture are exactly the same bytes that shipped.

**2. Registration + naming — CONFIRMED.** The retail binary registers the same
`"FaultFire"` name:
- `"FaultFire"` string at `005e11dc` (`recon/ghidra/_data.txt:107559-107560`)
  with **2 XREFs** at `004f1530` and `004f1570`.
- Those XREFs land **inside** `cls_0x5ac83c`'s function `0x4f13c0` (deltas
  `+0x170` and `+0x1b0`; per
  `recon/mappings/TFaultFireEffect_cls_0x5ac83c_candidate.yaml:7-9`).
- The retail function table at `005c53e4`–`005c53f4` lists two adjacent
  entries `LAB_004f1530` (table `005c53e8`) and `LAB_004f1570` (table
  `005c53ec`) — almost certainly the builder + animator registration entries
  for `"FaultFire"` (the two `REGISTER_*` calls in the snapshot:
  `REGISTER_BUILDER(TFaultFireEffect)` and
  `REGISTER_3DANIMATOR("FaultFire", TFaultFireAnimator)`).

**3. Structure / vftable — PARTIAL.** The retail candidate `cls_0x5ac83c` is
**400 bytes / 26 vmethods / 6 own methods** — substantially larger than the
snapshot `TFaultFireEffect` would be (a near-empty `TEffect` subclass with no
extra fields beyond what `TEffect` itself defines; class declaration
`src/effect.h:2200-2212` adds no members and only 2 method overrides). This is
a **divergence signal** — three possible explanations:
- (a) The candidate `cls_0x5ac83c` is the snapshot's `TFaultFireAnimator`, not
  `TFaultFireEffect`. The snapshot animator has more state (`th`, `tvs[4]`)
  but the inherited `T3DAnimator` base is itself large, which would explain
  the 400-byte size more plausibly than an empty TEffect shell. The candidate
  yaml infers "TFaultFireEffect (or TFaultFireAnimator)" — leaving the
  ambiguity open.
- (b) The retail effect was reworked from a near-empty shell into a more
  complex effect with internal state (counts, sub-effects, etc.) before ship.
  The candidate function `virt_meth_0x4f13c0` *does* set up a light-color-like
  triple of `0x3e99999a` (=0.3) on a sub-object's RGB channels and stores a
  position from `mbr_0xfc/0x100/0x104` — patterns consistent with a more
  involved effect than the snapshot's empty `Initialize()`.
- (c) The 400-byte size includes a TEffect base that itself is large in retail
  (the `cls_0x5b0074` "TEffect candidate" parent is 4524 bytes in the recon
  decomp — but that's almost certainly a Ghidra class-merging artefact, not
  the actual TEffect size, since 4524 bytes is implausibly large for the
  TEffect base).

Treat the structure result as **inconclusive divergence** — the retail layout
is bigger than the snapshot's empty shell would predict, but the candidate
mapping is medium-confidence and the recon body for the animator/render path
(where the snapshot constants live) is absent. Do not assume the snapshot's
empty `TFaultFireEffect::Initialize()` matches retail.

**4. Constant grep — NOT corroborated (snapshot-only).** The snapshot's render
immediates (`FF_STEP=0.1`, the `tv -= 0.01` adjustment in `SetupObjects`, the
`0.125 * (cos(th) + 7)` tv-scale envelope, the `random(2,8)/100` tu-scroll,
`scl=(0.25, 0.25, 1.0)`, `RotateZ(π/2)`, `pos.z = 32`, the RefreshZBuffer
32×128 patch) do **not** appear in the recon decomp. IEEE bit patterns
checked:

| immediate | float bit pattern (LE) | hits in `recon/`? |
|-----------|------------------------|-------------------|
| `0.1f` | `cd cc cc 3d` | not found in cls_0x5ac83c (other classes have, all unrelated) |
| `0.01f` | `0a d7 23 3c` | not found in cls_0x5ac83c |
| `0.125f` | `00 00 00 3e` | not found in cls_0x5ac83c |
| `7.0f` | `00 00 e0 40` | not found in cls_0x5ac83c |
| `0.25f` | `00 00 80 3e` | not found in cls_0x5ac83c |
| `1.0f` (=4×0.25) | `00 00 80 3f` | not found in cls_0x5ac83c |
| `32.0f` (pos.z) | `00 00 00 42` | not found in cls_0x5ac83c |

Integer immediates: snapshot RefreshZBuffer uses `size_x=32, size_y=128`
(`0x20, 0x80`); retail `meth_0x4f9040` stores `0x40, 0x80, 0x30` at
`this_00[4]+0x2c/0x30/0x34` — **the `0x80` matches but `0x40 ≠ 0x20` and
`0x30` has no snapshot equivalent**. This is consistent with the
RefreshZBuffer-equivalent sizes being on a different sub-object and possibly
re-tuned for ship. Cannot confirm the snapshot's 32×128.

This is because the retail `TFaultFireAnimator` body (the function that holds
the FF_STEP, tv-scale, tu-scroll, transform numbers) is **not decompiled** in
the recon dump — only ctor/dtor + the
"`FaultFire` string-referencing factory" decompile cleanly. Standard
snapshot-only pattern for render bodies (also seen with B01 Blood, F07
FireBall, X22 Sparks animator).

**Verdict — retail-partial.** The *what / where-registered / which-asset* is
**retail-confirmed** (asset byte-identical, `"FaultFire"` registration name
present in retail, two adjacent factory-table entries matching the two
snapshot `REGISTER_*` macros). The *exact rendering math* (FF_STEP cadence,
tv-scale envelope, tu-scroll rate, transform constants, RefreshZBuffer size)
is **snapshot-only and unverified** against the retail render body, **and the
class-size signal suggests the retail effect may have additional internal
state the snapshot lacks (most likely an associated dynamic light or
sub-emitter)**. Risk to reconstruction: the §3 constants below may not match
shipped, and there may be a retail-only light/sub-effect we cannot see from
the snapshot. Resolve by visually matching against retail ground-truth — but
note **no live caller exists** for `"FaultFire"` in either the snapshot or
retail data (§12), so ground-truth may not exist in the shipped game.

---

## 3. Constants

The animator's whole state is `float th, tvs[4]` (`src/effect.h:2226`). All
runtime numerics:

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `FF_STEP` | `0.1` | rad/tick (added to `th` each tick) | effect_old.cpp:11143, used at :11170 | **snapshot-only** (no retail corroboration; bit pattern `cd cc cc 3d` not in cls_0x5ac83c) |
| `th` wrap | `> 2π → subtract 2π` | rad | effect_old.cpp:11171-11172 | snapshot-only |
| `th` initial | `0.0` | rad | effect_old.cpp:11162 | snapshot-only |
| tu scroll Δ per tick (`du`) | `random(2, 8) / 100` ⇒ `0.02 .. 0.08` (uniform int 2..8 divided by 100) | UV units/tick | effect_old.cpp:11174 | snapshot-only |
| tu accumulator behavior | `tu += du` on ALL `obj->numverts` verts each tick — drifts indefinitely (texture wrap address mode handles wrap) | UV units | effect_old.cpp:11176-11179 | snapshot-only |
| tv baseline adjustment | `tv -= 0.01` per vertex in `SetupObjects` (one-time, stored in `tvs[v]`) | UV units | effect_old.cpp:11154-11155 | snapshot-only |
| tv-scale envelope | `scale = 0.125 * (cos(th + i*π/2) + 7)` for `i ∈ {0,1}` ⇒ range `[0.125*6, 0.125*8] = [0.75, 1.0]` | scalar | effect_old.cpp:11193 | snapshot-only |
| number of Render passes per frame | `2` (the `for (i = 0; i < 2; i++)` loop in Render) | count | effect_old.cpp:11191 | snapshot-only |
| tv-scale phase offset between passes | `π/2 rad` (90°) | rad | effect_old.cpp:11193 (`i * M_PI / 2`) | snapshot-only |
| object scale | `scl = (0.25, 0.25, 4.0 * 0.25) = (0.25, 0.25, 1.0)` | scalar / wu-scale | effect_old.cpp:11202-11203 | snapshot-only |
| Z-rotation | `RotateZ(π/2)` per pass (90° in-plane) | rad | effect_old.cpp:11208 | snapshot-only |
| position offset (per-pass) | `(0, 0, 32)` then `Translate` | wu (local) | effect_old.cpp:11211-11214 | snapshot-only |
| RefreshZBuffer patch | `size_x = 32, size_y = 128`; restored at `(screen.x - 16, screen.y - 128, 32, 128)` | screen px | effect_old.cpp:11228-11236 | snapshot-only (retail factory has `0x40, 0x80, 0x30` = 64, 128, 48 — different) |
| `FF_GROUPS` | `10` | count (UNUSED in snapshot — see §13.5) | effect.h:2218 | snapshot-only (dead in snapshot) |
| commented-out per-group `per[]` / `phase[]` | (dead) | — | effect.h:2227 | snapshot-only (dev-disabled — DO NOT reconstruct) |
| commented-out `RotateX(-π/2)` / `RotateX(-π/6)` | (dead) | rad | effect_old.cpp:11206-11207 | snapshot-only (dev-disabled — see §13.3) |
| commented-out `RotateZ(-(face*360/256)*TORADIAN)` | (dead) | rad | effect_old.cpp:11209 | snapshot-only (dev-disabled — see §13.3) |
| commented-out `RotateY(π/8)` | (dead) | rad | effect_old.cpp:11216 | snapshot-only (dev-disabled — see §13.3) |
| sim-tick rate | 24 Hz (engine default — Animate runs once per tick) | Hz | NOMENCLATURE §6 | snapshot-only (convention) |

`random(min, max)` is **inclusive** on both ends (`rand()%(max-min+1) + min`,
`src/revutils.cpp:1597-1612`). All cadences are authored for **24 Hz sim
tick**; `th` advances by `0.1 rad/tick` ⇒ full `2π` cycle in
`2π/0.1 ≈ 62.83 ticks ≈ 2.6 s`. Average tu-scroll = `0.05 UV/tick * 24 Hz ≈
1.2 UV/sec` (the texture flows across the quad width slightly faster than
once per second on average).

> **Reconstruction note (per-tick → per-second):** the animator was authored
> at integer-tick rates. For framerate-independent reconstruction (per
> NOMENCLATURE §6 / `feedback-framerate-independent-anim`), convert:
> `th_rate = FF_STEP * 24 = 2.4 rad/s`; `tu_rate ≈ random(0.02..0.08) * 24
> wu_uv/s ⇒ uniform 0.48..1.92 UV/s mean ≈ 1.2 UV/s`. Integrate by real `dt`
> and re-sample `du` either per real-tick boundary or per frame (per-frame is
> the snapshot's pattern).

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Faultfire | `legacy/Imagery/Magic/faultfire.i3d` (= retail `data/imagery.rvi:Imagery/Magic/faultfire.i3d`, **identical**) | 33,688 B (MD5 `21cb69a1e4053d652dada99407885af1` for both) | the rising-fire billboard imagery — a CGSR `STILL` I3D with **4 sub-objects** of which only 1 has geometry | registered `Class.Def:2071` under name `"Faultfire"` (ID `0x51753bce`); loaded by the OBJCLASS_EFFECT registry on spawn; bound by `T3DAnimator::GetObject(0)` (the snapshot only ever calls `GetObject(0)`) |

**Sub-objects (4)** — names read directly from the I3D object table at file
offsets `0x1c`, `0x68`, `0xb4`, `0x100` (each record 0x4C = 76 bytes):

| # | name | role | used by FaultFire animator? |
|---|------|------|---|
| 0 | `launch` | the renderable mesh — holds the `box01` 4-vertex quad (vertex data starts at file `0x228`) | **YES** — `GetObject(0)` in SetupObjects + Animate + Render |
| 1 | `fly` | attach-point dummy (flag `0x012000`, no geometry) | no |
| 2 | `explode` | attach-point dummy (flag `0x012000`, no geometry) | no |
| 3 | (unnamed) | invisible dummy (flag `0x012000` low bit cleared, no geometry) | no |

The `launch`/`fly`/`explode` naming pattern suggests this asset was authored to
hold spawn/anchor points for a multi-stage spell (launch → fly → explode), but
the snapshot animator only draws sub-object 0 and never references the
attach-point dummies. Sub-objects 1–3 are **not bone-attached effects** in the
snapshot — they exist in the asset but the animator does not read them.

### Decoded mesh — sub-object 0 `launch` / `box01`

A **single quad** (4 vertices, 4 indices — `numverts=4`, `numinds=4` at file
`0x220`). D3DVT_VERTEX format (32 bytes per vertex = pos.xyz + normal.xyz +
tu + tv), reading verts at file offset `0x228`:

| # | pos (x, y, z) | normal | tu | tv | tv after SetupObjects (`tvs[]`) |
|---|---------------|--------|----|----|----|
| 0 | (-32.03, 0.12, -28.09) | (0, -1, 0) | **0.0** | **0.0** | -0.01 |
| 1 | ( 31.97, 0.12, -28.09) | (0, -1, 0) | **1.0** | **0.0** | -0.01 |
| 2 | (-32.03, 0.12,  35.91) | (0, -1, 0) | **0.0** | **1.0** | 0.99 |
| 3 | ( 31.97, 0.12,  35.91) | (0, -1, 0) | **1.0** | **1.0** | 0.99 |

- **Quad extents:** X = -32 .. +32 (64 wu wide), Z = -28 .. +36 (64 wu tall in
  authored Z; normal faces -Y).
- **UVs:** **full 0..1 on both axes** — `box01` is **a single sprite, NOT an
  atlas**. UV (0..1) maps to the whole 128×128 texture. There is no per-cell
  sub-rect, no atlas grid, no flipbook-frame UV pick. The texture-animation
  mechanism is **tu-scroll + tv-rescale at runtime**, not cell selection (§8).
- **SetupObjects baseline `tvs[]`:** the one-time `tv -= 0.01` shift in
  SetupObjects (`effect_old.cpp:11154-11155`) lowers all tvs by 0.01, so
  Render's `tv = tvs[n] * scale` re-emits a slightly-shifted V each frame
  rather than the raw authored V. Stored values are `tvs = [-0.01, -0.01,
  0.99, 0.99]`.

### Decoded texture — 128×128 RGBA4444 (alpha-masked fire sprite)

Texture header (file offset `0x318` onward, decoded `unsigned int`s):

| field | value | meaning |
|-------|-------|---------|
| chunk tag | `0x10` | image chunk |
| pixel format flags | `0x1007` | RGB16 + alpha |
| width | `128` | px |
| height | `128` | px |
| (... padding to 0x364 ...) | | |
| bits/pixel | `32` | (header-reported; actual storage is 16-bit) |
| `0x368` field | `0x41` (=65) | flag/level count |
| (pad) | | |
| bits per channel | `0x10` (=16) | |
| **R mask** | `0x0F00` | bits 8–11 = 4-bit red |
| **G mask** | `0x00F0` | bits 4–7 = 4-bit green |
| **B mask** | `0x000F` | bits 0–3 = 4-bit blue |
| **A mask** | `0xF000` | bits 12–15 = 4-bit alpha |
| pixel data starts | `~0x394` | |

So **ARGB4444** (alpha in high nibble, then R, G, B, each 4 bits). The
**128×128 pixel grid** occupies 32,768 bytes — accounting for the file size.

**Color analysis (sample of all 16,384 pixels):**

| channel | distribution |
|---------|--------------|
| R (red) | 16,382 of 16,384 pixels have **R=15** (max) — essentially **always red** |
| G (green) | ramps from 0 at the bottom to 14–15 at the top (yellow-white at top, pure red at bottom) |
| B (blue) | mostly 0 at the bottom, rising to 12–14 near the top edge (helps push the top toward white) |
| A (alpha) | varies — many pixels at A=0 (transparent), many at A=15 (opaque); **this is an alpha-masked sprite**, not chroma-keyed bright-on-black |

Sample reads (rows checked: 0/64/127, columns 0/32/64/96/127):

| row | typical pixel | reading |
|-----|---------------|---------|
| 0 (top) | R=15, G=14, B=12, A=15 (= near-white, opaque) | **white-hot top of flame** |
| 64 (mid) | R=15, G=3-6, B=0-2, A=3-4 (= orange-red, semi-transparent) | warm orange middle |
| 127 (bottom) | R=15, G=1, B=0, A=0 (= saturated red, **transparent**) | dark red tip, invisible |

So the texture is a **vertical fire gradient**: bottom = invisible-red →
mid = warm-orange semi-transparent → top = white-hot opaque. Of the 7,000
low-alpha pixels (alpha < 2), only **1 has RGB=0** — i.e. the sprite is **NOT
chroma-keyed on black**; alpha-out areas retain their underlying red color.
The blend must be Alpha (so transparent areas don't add red to the scene); an
additive blend on this sprite would dump uniform dim red over the whole quad
rectangle, which is wrong. **This corroborates the snapshot's
`SetBlendState()` Alpha choice** (§7 sanity-check).

**The effect loads a real asset — do NOT substitute a procedural fire sprite.**
The authored 128×128 ARGB4444 fire-gradient texture (white-hot top → dark-red
transparent bottom) is the visual identity (§10). The quad is one tall flame
column.

---

## 5. Spawn & emit

- **Trigger semantics:** **continuous / persistent.** The effect object has no
  self-kill in the animator. Its `TFaultFireEffect::Pulse` just calls
  `TEffect::Pulse()` and does no damage/lifetime gating. The object lives
  until something external kills it (no internal lifetime). `Animate` calls
  `inst->SetCommandDone(false)` every tick (`effect_old.cpp:11168`), which
  keeps the effect "active / has commands pending" so the engine does not
  reap it via the idle-cleanup path. This is a **looping / spell-cast-while-
  alive** effect, not a one-shot.
- **Count per trigger:** **one** effect instance; no particles. The visual is
  one quad drawn twice per frame (two render passes of the same mesh).
- **Initial direction / distribution:** N/A — no particle spawn. The single
  quad has fixed authored geometry (XZ rectangle, normal -Y). Per-pass
  transform places it in local space (§7).
- **Emit anchor convention:** the visual sits at the effect *object*'s world
  position (whatever spawn point the caller — which is currently **none** in
  ship; §12 — placed it). Within the effect's local space, the quad is offset
  to `(0, 0, 32)` via `Translate(obj->pos)` (`effect_old.cpp:11211-11214`)
  before render — so it draws **+32 wu above the effect origin** along local
  Z. After the `RotateZ(π/2)`, the +Z translation remains the world-up
  direction (Z is invariant under XY-plane rotation). Net: the flame column
  rises from +32 wu above the spawn point's local Z (likely intended to lift
  the visible quad off the ground to chest/torso height when the effect spawns
  at ground level).
- **Coordinate space:** Local (relative to the effect object's anchor); the
  per-pass matrix is `Scale * RotateZ(π/2) * Translate(0,0,32)`. The effect
  origin is set by the caller (`def.pos`) — no internal pos integration. The
  fault-line *direction* would come from the caller's facing or from a
  `SetPos`/`SetAngle` call before Pulse, but the snapshot animator never
  reads `inst->GetFace()` (the only reference is commented out at `:11209`).
- **Spread / jitter:** none on geometry. The only stochastic element is the
  per-tick tu-scroll Δ (`du = random(2,8)/100`), which is a UV-flow
  randomization, not a positional one.

### Spatial diagram

```
                                       (single billboard "column" along local +Z,
                                        normal authored -Y — the wide face points
                                        toward the camera in iso view after the
                                        RotateZ(π/2) in-plane spin)

   wz (up)
   │              ╔══════╗    ← +Z = 64 wu tall, scaled by scl.z=1.0
   │              ║      ║       (geometry Z runs -28 .. +36 = 64 wu;
   │              ║flame ║        local +Z translation = 32 wu offset)
   │              ║column║
   │              ║      ║    ← X width 16 wu after scl.x=0.25 (64 wu * 0.25)
   │              ║      ║    ← drawn TWICE per frame with 90° tv-scale
   │              ║      ║       phase offset (cos(th) vs cos(th+π/2))
   │ effect       ║      ║
   ▲ origin ─ ─ ─ ╚══════╝ ─    quad base at local z = 32 + (-28*scl.z) = +4 wu
   │
   └──────────────────────── wx
  ╱
 ╱
wy

Top-down (XY plane after RotateZ(π/2)):
                                  +Y (north)
                                   │
   the quad's authored                │
   wide axis (X = ±32 wu) →           ▌  ← after RotateZ(π/2), the X-axis
   becomes the local Y                │     rotates into +Y (the quad's wide
   direction after the                │     face is now "front facing" along ±Y)
   90° in-plane spin           ──────┼──────── +X
                                     │
                                     │

NOTE: the quad has authored normal -Y (faces -Y in its rest pose). After
RotateZ(π/2), -Y → +X, so the quad faces +X after the spin. There is NO
camera-facing logic (no rot.x tip, no ScreenAligned billboard), so the
effect's visual orientation depends entirely on the camera direction
relative to the world +X axis — this is suggestive of "FaultFire is
intended to be cast aligned to a fault line whose orientation the spell's
caller code rotates into the effect's pos/face" but no such caller exists
in the snapshot or shipped data (§12). In rig isolation the column draws
facing +X (in-plane) and 16 wu wide × 64 wu tall.
```

---

## 6. Behavior & per-frame logic

The behavior is **entirely** in `TFaultFireAnimator`. There are NO particles,
NO state machine, NO sub-emitter, NO lifetime curve, NO scale envelope on the
object as a whole. The per-frame logic is:

### 6.1 One-time setup (`SetupObjects`, `effect_old.cpp:11147-11157`)

```
SetupObjects():
    o = NewObject(0)              // build the per-instance copy of sub-object 0 (launch / box01)
    GetVerts(o, D3DVT_VERTEX)      // populate o->verts from the imagery
    AddObject(o)
    for v in 0 .. o->numverts - 1: // 4 vertices
        o->verts[v].tv -= 0.01     // one-time downward V shift
        tvs[v] = o->verts[v].tv    // CACHE the (slightly-shifted) baseline tv
```

After SetupObjects, `tvs[] = [-0.01, -0.01, 0.99, 0.99]` (one-time baseline
that Render will multiply by the time-varying scale).

### 6.2 Initialize (`Initialize`, `effect_old.cpp:11159-11163`)

```
Initialize():
    T3DAnimator::Initialize()      // base init (runs the imagery instantiation)
    th = 0.0                       // reset phase
```

### 6.3 Per-tick Animate (`Animate`, `effect_old.cpp:11165-11180`)

```
Animate(draw):                          // once per 24 Hz sim tick
    T3DAnimator::Animate(draw)          // base tick (matrices, transforms refresh)
    inst->SetCommandDone(false)         // keep the effect alive (no self-kill)

    // 1. Advance phase: th += FF_STEP, wrap at 2π
    th += FF_STEP                       // FF_STEP = 0.1 rad/tick
    if th > 2π:
        th -= 2π                        // wraps at ~62.8 ticks ≈ 2.6 s

    // 2. Pick this tick's tu scroll: 0.02 .. 0.08 (uniform int 2..8 / 100)
    du = random(2, 8) / 100.0           // 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08
    obj = GetObject(0)                  // the launch / box01 quad

    // 3. Scroll U on ALL 4 verts (per-vertex tu accumulates indefinitely)
    for n in 0 .. obj->numverts - 1:
        obj->verts[n].tu += du
```

**Per-frame logic — what's actually happening:**

- **`th` (texture-throb phase)** advances by exactly `0.1 rad/tick`. Wraps at
  `2π`. Drives the V-scale envelope in Render (§7).
- **`tu` scrolls indefinitely** (no modulo, no reset). Each tick, every
  vertex gains the same `du = random(2..8)/100`. Since `tu` is a float on
  the vertex and D3D wraps at modulo-1 (or clamps, depending on address mode
  — the default is wrap for fire/water), the texture appears to **flow
  horizontally across the quad** indefinitely. The randomized `du` per tick
  gives a slightly irregular flow rate (textbook "flickering fire flow"
  trick — not perfectly periodic).
- **No state machine.** Every tick does the same thing.
- **No lifetime / age01 / curves on the effect as a whole.** The effect
  loops indefinitely until killed externally.

### 6.4 Per-frame Render (covered in §7; the actual draw logic)

The Render is two passes of the same quad with two different time-varying
`tv` scales. It runs every visible frame.

### Temporal diagram

```
                                       th wraps at 62.83 ticks ≈ 2.6 s
                                                  │
  th  2π ┤                            ●           │           ●
         │                          ╱   ╲         │         ╱   ╲
         │                        ╱       ╲       │       ╱       ╲
     π   ┤                      ╱           ╲     │     ╱           ╲
         │                    ╱               ╲   │   ╱               ╲
     0   ┤────●─────────────                    ╲ │ ╱                    ─
         └──────────────────────────────────────── ticks
              0                                 62.8                  125.6


  tv-scale envelope per pass (range 0.75 .. 1.00):

  scale ┤        ┌── pass i=0:  scale = 0.125 * (cos(th) + 7)
  1.00 ─┤        │     range [0.75, 1.00], peaks at th = 0, 2π
        │  ─ ─ ─ │ ─ ─ ─ pass i=1: scale = 0.125 * (cos(th + π/2) + 7)
        │  ╱       ╲           range [0.75, 1.00], peaks at th = -π/2 + 2π·k
        │ ╱         ╲          (90° phase-offset from i=0 — when one peaks
  0.75 ─┤            ╲          the other is at trough)
        │
        └──────────────────────────────────────── th (0 .. 2π)

  Net visual: TWO flame layers throb out of phase — when layer 0 is "tall"
  layer 1 is "short", and vice-versa. The compound read is "flickering
  rising fire" rather than a single periodic pulse.


  tu drift (continuous, randomized per tick):

  tu(t) = tu(t-1) + random(0.02..0.08)
        ≈ +1.2 UV/sec average (50 ticks → ~5 full texture wraps/sec on a
                                fast tick? Actually 0.05 UV/tick * 24 Hz =
                                +1.2 UV/sec — one full wrap every ~0.83 s.)

  All 4 vertices' tu advance in lock-step, so the texture rigidly translates
  across the quad — no per-vertex skew.
```

---

## 7. Rendering (original render state + geometry)

`TFaultFireAnimator::Render` (`src/effect_old.cpp:11182-11224`):

```
Render():                                           // every visible frame
    SaveBlendState()
    SetBlendState()                                 // → Alpha (MODULATE + SRC_ALPHA/INV_SRC_ALPHA)

    obj = GetObject(0)                              // the launch / box01 quad
    ResetExtents()

    for i in 0, 1:                                  // TWO passes
        scale = 0.125 * (cos(th + i * π/2) + 7)     // range [0.75, 1.0]
        for n in 0 .. obj->numverts - 1:
            obj->verts[n].tv = tvs[n] * scale       // V-scale (U unchanged — that's accumulated in Animate)

        D3DMATRIXClear(&obj->matrix)
        obj->flags = OBJ3D_MATRIX | OBJ3D_VERTS     // explicit matrix + use effect's own verts

        obj->scl.x = 0.25
        obj->scl.y = 0.25
        obj->scl.z = 4.0 * 0.25  = 1.0
        D3DMATRIXScale(&obj->matrix, &obj->scl)     // Scale

        // (commented dev-disabled rotations — see §13.3)
        // D3DMATRIXRotateX(&obj->matrix, -π/2)       // would tip quad ground-flat
        // D3DMATRIXRotateX(&obj->matrix, -π/6)
        D3DMATRIXRotateZ(&obj->matrix, π/2)         // 90° in-plane spin (the live rotation)
        // D3DMATRIXRotateZ(&obj->matrix, -((inst->GetFace()*360/256) * TORADIAN))   // commented

        obj->pos = (0, 0, 32)
        D3DMATRIXTranslate(&obj->matrix, &obj->pos)  // Translate +32 wu local Z

        // D3DMATRIXRotateY(&obj->matrix, π/8)        // commented

        RenderObject(obj)                            // draw the quad

    UpdateExtents()
    RestoreBlendState()
    return true
```

- **What it draws:** a single billboard quad (the `box01` sub-mesh of `launch`),
  drawn **twice per frame** with different `tv` scales (90°-phase-offset
  cosine envelope). The quad's UV is the entire 0..1 texture range (the
  authored `box01` UVs), with `tu` continuously accumulating across frames
  (Animate) and `tv` re-scaled each pass (Render).
- **Blend mode (original):** `SetBlendState()` (`effect_old.cpp:221-233`) =
  `D3DTBLEND_MODULATE`, `SRCBLEND = SRC_ALPHA`, `DESTBLEND = INV_SRC_ALPHA`,
  `ZWRITEENABLE = false`, `ZENABLE = true` → **Alpha (modulated)**
  (NOMENCLATURE §3).
  - **BLEND SANITY-CHECK.** The faultfire sprite is a **128×128 ARGB4444
    alpha-masked fire** texture (§4): R=15 everywhere, G/B ramps for hot-to-
    warm gradient, and **alpha is the flame shape mask** (~7,000 of the 16,384
    pixels have alpha < 2, and those pixels carry color but the alpha is
    near-zero). The low-alpha pixels are NOT black-keyed (only 1 of ~7,000
    low-alpha pixels has RGB=0); they retain their underlying red color and
    would dump a uniform dim red layer over the whole quad rectangle under an
    additive blend. Alpha is the textbook choice for this sprite design. **No
    blend conflict — the Alpha-helper is consistent with the sprite design.**
  - **Sister-family cross-check.** Mixed — Flame/Fire/FireSwarm
    (single-billboard / ground-pinned fires) use `SetBlendState` (Alpha);
    FireCone/DragonFire (burst/cone breath effects) use `SetAddBlendState`
    (AdditiveStraight). FaultFire fits the **single-billboard Alpha**
    sub-family, not the cone/burst Additive sub-family. **Consistent with
    siblings.**
  - **Snapshot-drift risk:** the retail `TFaultFireAnimator::Render` body is
    NOT decompiled (§2.1), so the blend choice is **snapshot-only and
    unverified against shipped retail**. However the sprite design + sister
    sub-family pattern both point to Alpha. Treat Alpha as the design intent;
    if visually vetted against retail and the read is wrong (e.g. a dim
    rectangle of red shows), reconsider. Given the asset is byte-identical
    and the alpha mask is the visual identity, an additive override is very
    unlikely for the shipped effect.
- **Lit vs self-lit:** **Unlit / self-lit.** The animator never folds ambient
  light into vertex color and never calls `DoLighting`/`GetClosestLights`. The
  imagery's authored vertex color is used as-is (the box01 verts in the I3D
  have no explicit color set in the visible vertex data — they default to
  white-ish/passthrough via the MODULATE stage). The sprite's color is
  literal. Classify **Unlit** (NOMENCLATURE §4). Fire is a textbook self-lit
  effect.
- **Depth / Z:** **TestNoWrite** — `ZENABLE = true`, `ZWRITEENABLE = false`
  (set by `SetBlendState`, `effect_old.cpp:224-225`). `RefreshZBuffer`
  (`effect_old.cpp:11226-11237`) restores scene Z over a fixed **32×128 px
  patch** anchored at the projected effect origin:
  `RestoreZ(screen.x − 16, screen.y − 128, 32, 128)` — i.e. 32 px wide, 128
  px tall, centered horizontally on the effect's screen position and
  extending 128 px UPWARD from the origin. (Retail factory function has
  `0x40, 0x80, 0x30` = 64, 128, 48 — possible re-tuning, see §2.1(4) and
  §13.6.)
- **Orientation:** **NOT ScreenAligned, NOT WorldXY** — the snapshot draws
  the quad with `RotateZ(π/2)` only. The authored geometry's normal is `-Y`
  (the quad lies in the XZ plane, facing −Y). After `RotateZ(π/2)`, the
  quad's normal becomes +X (the quad now lies in the YZ plane, facing +X).
  **No `rot.x = -π/2` tip onto the ground**; **no per-frame camera-facing
  logic**. This is a **world-fixed-orientation column** that faces +X in the
  effect's local frame. The reconstruction needs to expose either a
  `WorldUpAligned`-style "column pinned to world +Z with manual yaw" or to
  treat the caller's spell-cast pos/face as a yaw rotation applied at the
  effect-object level (the snapshot's commented-out
  `RotateZ(-(GetFace()*360/256)*TORADIAN)` at `:11209` is the "spin the
  column to the caster's facing" code that was disabled — see §13.3).
  Default rig orientation = quad faces +X local, vertical column 16 wu wide
  by 64 wu tall.
- **Per-quad transform:** explicit `OBJ3D_MATRIX` = `Scale(0.25, 0.25, 1.0)
  · RotateZ(π/2) · Translate(0, 0, 32)`. **The same matrix is used for both
  passes** — only the per-vertex `tv` changes between i=0 and i=1, so the
  two passes draw the same screen-space rectangle with two different vertical
  "stretches" of the texture, overlapping perfectly (additive in alpha terms,
  not in blend mode — each pass alpha-blends onto the previous).
- **Per-vertex color packing:** **NONE written by the effect.** The four
  vertex colors come from the imagery (`box01`'s authored verts). The effect
  modifies only `tu` (Animate) and `tv` (Render); under MODULATE the sampled
  texel is multiplied by the authored vertex color (effectively white →
  texture passes through unchanged).
- **Two-pass note (the key visual trick).** Because both passes draw the same
  matrix and only `tv` differs, and because `tv-scale` varies in `[0.75, 1.0]`
  90° out of phase between i=0 and i=1, the rendered result is **two layered
  flame columns** that throb out of sync over a 2.6 s cycle. When pass-0 is at
  its "tallest" (tv-scale = 1.0, texture spans more vertical) pass-1 is at
  "shortest" (tv-scale ≈ 0.75, texture is vertically squished), and vice versa.
  Combined with the continuous tu-scroll (`+1.2 UV/sec` average), the effect
  reads as a flickering, flowing fire — the rising/flowing visual identity.

---

## 8. Texture animation

**YES — UV ANIMATION** (NOT a `framehtexs[]` flipbook, NOT a UV-subrect atlas).
Mechanism:

- **U axis (tu):** **continuous scroll**, accumulated across frames in
  `Animate` (`effect_old.cpp:11174-11179`). Each tick adds a randomized
  `du = random(2,8)/100 = 0.02 .. 0.08` to every vertex's `tu`. The four
  verts of `box01` advance in lock-step, so the texture rigidly translates
  horizontally across the quad. Wrap behavior depends on the texture address
  mode — for fire/flow textures the default is `D3DTADDRESS_WRAP` (texture
  tiles seamlessly), which matches the snapshot's intent (continuous flow,
  no visible seam). Average rate: `~0.05 UV/tick × 24 Hz ≈ 1.2 UV/sec` (one
  full wrap every ~0.83 s, ±~30% randomization frame-to-frame).
- **V axis (tv):** **per-frame multiplicative scaling** in `Render`
  (`effect_old.cpp:11193-11196`). Each pass re-writes every vertex's `tv` as
  `tvs[n] * scale`, where `scale = 0.125 * (cos(th + i*π/2) + 7) ∈ [0.75,
  1.0]`. `tv` is **not accumulated** — it's overwritten each pass. The
  `tvs[]` baseline was captured once in `SetupObjects` after a `-= 0.01`
  shift. Effect: each pass renders the texture's vertical extent at 75–100 %
  of the authored span, modulating "how compressed / how stretched" the
  flame looks vertically.
- **Per-instance phase offset:** the two render passes use `cos(th)` and
  `cos(th + π/2)` — fixed 90° offset between i=0 and i=1 within a single
  instance. There is no per-instance random phase offset for the time
  variable `th` itself (it starts at 0 in `Initialize`). Two FaultFire effects
  spawned at exactly the same tick would animate in perfect sync; one spawned
  later would be `frame_offset * 0.1 rad` ahead.
- **No `framehtexs[]` flipbook.** The texture handle is set once by the
  imagery at load; no `obj->textureframe[]` writes in the animator (contrast
  F03 `TFireAnimator` which uses `framehtexs[]`).
- **No atlas UV-subrect selection.** The `box01` UVs are full 0..1; no
  cell-grid math anywhere in the animator.

> **Atlas note (per §4 decode requirement):** the `faultfire.i3d` asset is
> a 128×128 single-image texture with one quad UV-mapped to its full 0..1
> range. There is **no atlas grid** and **no per-cell sub-rect**. A
> reconstruction that draws "4 sub-objects sharing an atlas" or "cells
> 0..3 of a 2×2 grid" would be wrong: there is one renderable quad
> (sub-object 0 / `launch` / `box01`), and the texture is one whole-frame
> fire gradient. The "animation" is the runtime tu-scroll + tv-rescale, not
> a cell pick.

---

## 9. Associated light

**N/A — none emitted by the snapshot animator.** `TFaultFireAnimator`
makes no `AddPointLight` / dynamic-light call; the effect does not light the
scene.

**BUT a retail divergence is plausible (§2.1).** The retail
`virt_meth_0x4f13c0` (the function that references the `"FaultFire"` string)
sets three `0x3e99999a` (=0.3) values into a sub-object's RGB slot and then
copies a position from `mbr_0xfc/0x100/0x104`. This pattern is consistent
with an **`AddPointLight` call with color = (0.3, 0.3, 0.3)** (or with
`AddPointLight(intensity_xyz, pos_xyz)` taking RGB intensity + position).
Both the RGB triple and the position triple are written together. If this
inference is correct, the retail effect emits a **dim warm gray light at
intensity 0.3 per channel** (visually a soft glow on nearby walls/floor).

This is **unverified** — the snapshot animator emits no light, and we cannot
confirm the retail call without decompiling more context. **Flag as a
possible retail-only addition** (§13.2). A reconstruction should either:
(a) emit no light (snapshot fidelity), or (b) optionally add a dim
gray-warm point light at the effect origin matching the (0.3, 0.3, 0.3)
hint, vet visually.

---

## 10. Color

- **Source:** the **authored `Magic\faultfire.i3d` texture** (§4) — a 128×128
  ARGB4444 fire-gradient sprite. The effect supplies no color of its own (no
  per-vertex tint written by the animator, no spell-color injection, no
  chardata field). Color is literal from the sprite, modulated by the
  imagery's authored vertex diffuse (white passthrough under MODULATE).
- **Exact texture color (from byte analysis, §4):**
  - **Top row (y=0):** RGB(15, 14, 12), A=15 in 4-bit → 8-bit ≈ RGB(255, 238,
    204), A=255 — **near-white, warm cream highlights, fully opaque**.
  - **Mid row (y=64):** RGB(15, 3–6, 0–2), A=2–4 in 4-bit → ≈ RGB(255, 51,
    0–34), A=34–68 — **saturated orange, semi-transparent**.
  - **Bottom row (y=127):** RGB(15, 1, 0), A=0 in 4-bit → ≈ RGB(255, 17, 0),
    A=0 — **dark red, fully transparent** (invisible).
  - **Red channel:** 16,382 of 16,384 pixels have R=15 (max). The sprite is
    effectively a **pure red base + green/blue gradient pushing the top
    toward white** = textbook fire gradient.
  - **Alpha channel:** varies — the alpha mask is the flame shape (more
    opaque at the visible flame body, fading to transparent at the edges
    and the bottom).
- **Expected visual:** a **bright orange-to-white-hot vertical flame column**
  — saturated warm fire colors, fading down toward transparent dark-red at
  the base. Rich color, intentional saturation (per
  `feedback-vfx-color-health-signal`). A washed-out / gray / pink result at
  reconstruction = broken port (likely culprits: (1) procedural fire stand-in
  instead of the real `faultfire.i3d` texture, (2) wrong blend (additive on
  this alpha-masked sprite would smear dim red over the whole quad, killing
  the gradient), (3) ARGB4444 decoded as RGB565 or similar — would lose
  alpha and miss the flame-shape mask). The fire should read warm orange/red
  at the body and bright yellow-white at the tip, with a flickering throb
  cycle of ~2.6 s.
- **Normalization / boosts:** none. The animator does no color math; the
  imagery's per-vertex color is whatever the I3D authoring set (effectively
  white passthrough), and the texture sample is multiplied straight through
  under MODULATE.

---

## 11. Audio coupling

**No audio coupling found.** `TFaultFireAnimator` makes no `PLAY()` /
`PlayWave()` call (contrast `TFireFlashAnimator::Initialize` which calls
`PLAY("FireFlash")` at `src/effect_old.cpp:11269+`, and `TFireWindAnimator`
which calls `PLAY("FireWind")`). `TFaultFireEffect::Initialize` is empty.
**The effect is silent in the snapshot.** No sister-family sound name like
`"FaultFire"` exists in the wave registry strings.

If the retail effect was reworked to include sound (consistent with the size
divergence in §2.1(3)), it is undetectable from the snapshot. Record as
"no audio in the snapshot" for the audio phase.

---

## 12. Triggers & in-game appearance

- **Spawned by:** **NO LIVE CALLER FOUND** in either the snapshot or the
  shipped data:
  - The registration is live: `DEFINE_BUILDER("FaultFire", TFaultFireEffect)`
    (`effect_old.cpp:11127`) + `REGISTER_3DANIMATOR("FaultFire",
    TFaultFireAnimator)` (`:11145`) — both confirmed by the retail XREFs
    (§2.1(2)).
  - No `spell.def` variant invokes `"FaultFire"` in `legacy/spell.def`,
    `data/Resources/spell.def`, or `data/resources_unzipped/spell.def`
    (grep returns nothing).
  - No `ATTACHEFFECT "FaultFire"` anywhere in `data/` or `legacy/` (grep
    returns nothing).
  - No monster attack or scripted scene references the name.
  - No `area.def` ambient placement.
  - The only sites that mention `"FaultFire"` in shipped data are the
    retail binary's two factory-table entries (`005c53e8`, `005c53ec`)
    and the string at `005e11dc` — these are just the registration.
- **Where to see it in the original game:** **unconfirmed — likely
  unobservable in the shipped game.** Without a spell.def variant, an
  ATTACHEFFECT entry, or a scripted call, FaultFire never spawns in normal
  play. Possibilities:
  - (a) A cut spell — the asset + class shipped, but no caller wires it. Cf.
    F03 TFireEffect (also vestigial, no live caller).
  - (b) A debug-only spawn path that the snapshot doesn't preserve (an
    in-engine debug menu that calls `CreateObject("FaultFire")` directly).
  - (c) A late-bound caller in the retail binary that the recon decomp
    didn't reach (the "FaultFire" string has only 2 XREFs and both are the
    registration entries; a *caller* would XREF "FaultFire" from a third
    site, which the data shows does not exist).
- **Vestigial?** **Yes — vestigial at the caller side.** The asset +
  registration ship in retail, but no shipped spell or script invokes
  `"FaultFire"`. The visual identity (rising fire column with flowing
  texture) is recoverable from the asset + the snapshot animator, but
  there is no in-game ground-truth video to validate against. The
  reconstruction's visual vet must be against the asset (the rendered
  flame gradient should match the texture's authored colors) and the
  sister fire-family pattern (single-billboard Alpha-blended fires), not
  against retail gameplay footage.

---

## 13. Gaps & uncertainties

- **13.1 ALL Animate/Render constants are snapshot-only (the headline risk).**
  Per §2.1(4), the retail `TFaultFireAnimator` body is not decompiled, and
  none of the snapshot's IEEE bit patterns (`0.1`, `0.01`, `0.125`, `7.0`,
  `0.25`, `1.0`, `32.0`) appear in the retail decomp. The per-tick FF_STEP,
  the per-frame tv-scale envelope, the tu-scroll rate, the scale/rotation/
  translation constants, and the RefreshZBuffer 32×128 patch are all
  **unverified against shipped retail**. The byte-identical asset + matching
  registration name make them *likely* unchanged, but reconstruction must be
  ready to retune these specific numbers if a retail capture (if obtainable)
  shows a different cadence / size / aspect.
- **13.2 Retail may emit a dynamic light the snapshot doesn't (§9).** The
  retail `virt_meth_0x4f13c0` writes three `0.3` floats into a sub-object's
  RGB slot and a position from `mbr_0xfc/0x100/0x104` — a pattern consistent
  with `AddPointLight(rgb=(0.3,0.3,0.3), pos)`. The snapshot animator emits
  no light. If the inference is correct, the shipped effect has a dim warm
  gray glow on the surrounding scene that the snapshot lacks. **Cannot
  confirm from source**; reconstruction has two options — match the
  snapshot (no light) or optionally add a dim light per the inference (vet
  visually). Default: match the snapshot, log as a potential extension.
- **13.3 Substantial commented-out / dev-disabled code in Render — DO NOT
  reconstruct.** The Render body has four commented rotation lines
  (`effect_old.cpp:11206-11216`):
  - `D3DMATRIXRotateX(&obj->matrix, -π/2)` — would tip the quad ground-flat
    (WorldXY). **Disabled** — FaultFire is NOT a ground-flat fire.
  - `D3DMATRIXRotateX(&obj->matrix, -π/6)` — a 30° tip (alternative).
  - `D3DMATRIXRotateZ(&obj->matrix, -((inst->GetFace() * 360 / 256) *
    TORADIAN))` — would spin the column to the caster's facing direction.
    **Disabled** — likely because no live caller sets `GetFace()`
    meaningfully (the rig spawn doesn't drive the facing). If reconstructed
    for a hypothetical caller, this is the "rotate to fault-line direction"
    code. Currently the live `RotateZ(π/2)` is a hardcoded 90° spin, not
    facing-driven.
  - `D3DMATRIXRotateY(&obj->matrix, π/8)` — small Y tilt (22.5°).
    **Disabled.**
  Do NOT reconstruct any of these. The live rotation set is **only**
  `RotateZ(π/2)`.
- **13.4 The fragment header `//static bool ff_init = false;` and
  `//static float per[FF_GROUPS], phase[FF_GROUPS];` (effect.h:2222,2227) is
  dev-disabled.** `FF_GROUPS = 10` (effect.h:2218) suggests a planned
  "10 groups of fault-fire instances with per-group phase offsets" mechanism
  that was abandoned. **DO NOT reconstruct grouping** — the live code uses
  a single instance with two render passes, no group management.
- **13.5 The four sub-objects pattern (`launch`/`fly`/`explode`/unnamed) is
  partial intent.** Three of the four are zero-geometry attach-point dummies
  (flag `0x012000`). The snapshot animator never reads them (`GetObject(0)`
  only). They are likely **pre-release authored anchors** for a planned
  multi-stage fault-fire spell (launch → fly → explode) that was never wired
  up. **Do NOT reconstruct stage-based sub-effect spawning** — the live
  animator draws sub-object 0 twice and nothing else.
- **13.6 RefreshZBuffer 32×128 vs retail 64×128 (×48 z-extent).** The retail
  `meth_0x4f9040` stores `0x40, 0x80, 0x30` (= 64, 128, 48) at
  `this_00[4]+0x2c/0x30/0x34` — heights match (128 == 0x80) but the width
  diverges (32 vs 64), and there's a third value (48) the snapshot doesn't
  have. Could be: (a) the retail patch is 64×128 (wider) for the same
  effect; (b) the retail object stores **default** sizes used by *some other
  effect* sharing this class (FaultFire vs an unrelated peer); or (c) the
  offsets `0x2c/0x30/0x34` belong to a different sub-system. Treat the
  snapshot's 32×128 as the design intent for reconstruction; consider 64
  width if the rendered column appears wider than its z-restored patch.
- **13.7 No in-game ground truth (§12).** Because no live caller exists,
  visual reconstruction can only be vetted against the asset's authored
  texture (the orange/white-hot flame gradient should be evident) and the
  sister fire-family blend/orientation pattern. A standalone harness rig
  spawning the effect at a fixed point and observing the throb + flow is
  the only validation. If a future retail capture is obtained, the
  snapshot constants (FF_STEP, tv-scale envelope, tu rate) should be the
  first values vetted.
- **13.8 The effect persists indefinitely (no self-kill).**
  `SetCommandDone(false)` in Animate keeps the engine from idling the
  effect out. There is no built-in lifetime / cleanup; the effect lives
  until external code (presumably a hypothetical spell duration timer)
  destroys the object. A reconstruction rig must provide an external
  kill switch.
- **13.9 Possible structure divergence (§2.1(3)).** The retail
  `cls_0x5ac83c` is 400 bytes, larger than a near-empty
  `TFaultFireEffect`-on-`TEffect`-base would predict. The candidate yaml
  notes the ambiguity ("TFaultFireEffect OR TFaultFireAnimator"). If the
  retail effect was reworked to carry additional state (e.g. a sub-emitter,
  multiple instances, owner-side data), the snapshot may understate the
  shipped complexity. This is hard to act on without more decomp, but
  reconstruction should be open to "the shipped effect has more than the
  snapshot shows."

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\faultfire.i3d (33,688 B, byte-identical to retail). Address
      its 4 sub-objects: 0=launch (the renderable, with box01 mesh), 1=fly,
      2=explode, 3=(unnamed). Use ONLY sub-object 0 — the other three are
      zero-geometry attach-point dummies (flag 0x012000). NO procedural fire
      stand-in. The texture is 128x128 ARGB4444 (alpha-masked vertical fire
      gradient: white-hot top → transparent dark-red bottom). (§4)
- [ ] Drive it as TFaultFireAnimator. Effect state is float th + float tvs[4]
      only. TFaultFireEffect is an empty shell (Initialize/Pulse pass-through).
      No particles, no sub-emitter, no state machine. (§2, §6)
- [ ] SetupObjects (one-time): build per-instance copy of sub-object 0's
      box01 quad (4 verts, D3DVT_VERTEX format), apply tv -= 0.01 to all 4
      verts, cache adjusted tvs into tvs[] (yields tvs=[-0.01,-0.01,0.99,
      0.99]). (§6.1, §4)
- [ ] Initialize: th = 0.0 after T3DAnimator::Initialize. (§6.2)
- [ ] Animate (per 24Hz sim tick — but reconstruction integrates by real dt
      for framerate-independent motion per NOMENCLATURE §6): advance
      th += 0.1 rad/tick (= 2.4 rad/s), wrap at 2π. Pick du =
      random(2,8)/100 = uniform {0.02, 0.03, ..., 0.08}. Add du to verts[n].tu
      on all 4 verts (continuous UV-scroll, no reset, drifts indefinitely).
      Call SetCommandDone(false) so the effect persists. (§6.3, §8)
- [ ] Render: TWO passes per frame, same quad. For i in {0, 1}:
        scale = 0.125 * (cos(th + i * π/2) + 7)   // range [0.75, 1.0]
        for n in 0..3: verts[n].tv = tvs[n] * scale
        matrix = Scale(0.25, 0.25, 1.0) · RotateZ(π/2) · Translate(0, 0, 32)
        RenderObject(box01 quad with current matrix + verts).
      The same matrix for both passes; only tv differs between i=0 and i=1.
      90° phase offset gives the layered-flame beat. (§7)
- [ ] Blend = **Alpha** (SetBlendState: D3DTBLEND_MODULATE +
      SRC_ALPHA/INV_SRC_ALPHA). Sprite is alpha-masked (not chroma-keyed),
      additive would smear red — Alpha is correct per sprite design.
      **Snapshot-only / unverified vs retail render body**, but consistent
      with sister single-billboard fires (Flame, Fire, FireSwarm). (§7)
- [ ] Lit-mode = Unlit (self-lit fire — no DoLighting, no GetClosestLights).
      Depth = TestNoWrite (ZENABLE on, ZWRITE off). (§7)
- [ ] Orientation: NOT ScreenAligned and NOT WorldXY (no rot.x tip). The
      quad's authored normal is -Y; after RotateZ(π/2) it faces +X in local
      space — a world-fixed-orientation column. NO camera-facing logic in
      the snapshot. The commented-out
      "RotateZ(-(GetFace()*360/256)*TORADIAN)" hints at a planned
      caller-facing yaw that was never wired; reconstruction should expose
      the effect's local yaw as a knob driven by the caller's spell.face if
      / when a caller is wired. Default rig: face +X local. (§7, §13.3)
- [ ] RefreshZBuffer: restore scene Z over a 32x128 px patch at (screen.x −
      16, screen.y − 128, 32, 128) — extends 128 px UPWARD from the effect
      origin (a tall thin column ZBuffer-fix). (§7)
- [ ] Texture animation = **UV-scroll on U (continuous, ~1.2 UV/sec avg,
      ±30% per-tick random) + per-pass tv-scale envelope** (0.125 * (cos(th
      + i*π/2) + 7), range [0.75, 1.0], 90° phase offset between the two
      passes). NOT a framehtexs[] flipbook, NOT an atlas cell pick. The U
      drift wraps via the texture address mode (default WRAP). (§8)
- [ ] NO particles, NO sub-effects spawned, NO state machine, NO lifetime
      curve. (§6)
- [ ] Associated dynamic light EMITTED: NONE in the snapshot animator.
      **Retail may add a dim warm gray AddPointLight at (0.3, 0.3, 0.3)
      RGB based on a candidate function pattern in cls_0x5ac83c — flag as
      a possible extension; reconstruction default = no light.** (§9, §13.2)
- [ ] Color: from the authored 128x128 ARGB4444 fire-gradient texture
      (R=15 ≈ 255 nearly everywhere; G/B ramp gives the orange→white-hot
      vertical gradient; A is the flame-shape mask). Verify the rendered
      column reads warm orange/red at the body and bright yellow-white at
      the tip, with the flickering throb + horizontal flow visible. (§10)
- [ ] Audio: NONE in the effect (snapshot animator has no PLAY/PlayWave
      call). (§11)
- [ ] Trigger wiring: NO LIVE CALLER in snapshot or shipped data. The
      asset + registration ship, but no spell.def variant, ATTACHEFFECT,
      or script invokes "FaultFire". Reconstruction is by standalone
      harness only. (§12, §13.7)
- [ ] Effect persistence: SetCommandDone(false) keeps the object alive
      indefinitely. Reconstruction rig must externally kill the effect
      (no internal lifetime). (§6, §13.8)
- [ ] Do NOT reconstruct:
        - the four commented Render rotations (RotateX(-π/2), RotateX(-π/6),
          face-driven RotateZ, RotateY(π/8)) — dev-disabled (§13.3);
        - the FF_GROUPS = 10 / per[]/phase[] grouping (dev-disabled,
          single-instance is the live shape) (§13.4);
        - stage-based sub-effects from the "launch"/"fly"/"explode" attach
          dummies (the snapshot animator only reads sub-object 0) (§13.5);
        - any associated light beyond optionally vetting the §13.2 hint.
```

**Definition of done:** a tall narrow flame column (16 wu wide × 64 wu
tall, rising from 32 wu above the effect origin) appears at the spawn point,
faces +X in local space (caller may yaw the parent object), renders with
the authored orange-to-white-hot fire texture flowing horizontally across
its surface (~1.2 UV/sec average, ±randomized per tick), and visibly throbs
vertically with a 2.6 s cycle — the throb being two overlapping layered
flames (same matrix, different per-pass tv-scales 90° out of phase) that
read as flickering rather than periodic. Alpha-blended (sprite's authored
alpha mask carries the flame shape), self-lit, depth-tested but no
depth-write, no particles, no light, no sound, no lifetime — persists until
externally destroyed.
