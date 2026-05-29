# F05 TFireSwarmEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F05 (covers `TFireSwarmEffect` + paired `TFireSwarmAnimator`) |
| **Class(es)** | `TFireSwarmEffect : TEffect` (empty shell, only an inherited `Pulse`) + `TFireSwarmAnimator : T3DAnimator` (one int32_t frame counter + three floats — `cylhscl`, `cylvscl`, `cylth`). |
| **Status** | forensics-complete (the snapshot bodies are tiny and exhaustive; gaps are real-game callers, not code). |
| **Retail fidelity** | **retail-partial** — registration string `"FireSwarm"` is **retail-confirmed** with **2 XREFs** at `004f00d0` and `004f0110` (the paired `REGISTER_BUILDER` + `REGISTER_3DANIMATOR` factory entries at the consecutive table slots `005c53c8` and `005c53cc`, matching the FireFlash / FireWind / FireBall registration pattern; `recon/classes/_data.txt:107531-107532, 56018-56021`). Asset `Magic\FireSwarm.i3d` is **byte-identical** between snapshot (`legacy/Imagery/Magic/fireswarm.i3d`) and the shipped `data/imagery.rvi:Imagery/Magic/fireswarm.i3d` (24,300 B, MD5 `927caed9595abffae3ea7a939b3a240a` on both — verified by extracting via `zipfile`). Asset registered as `"FireSwarm" "Magic\FireSwarm.i3d" 0x582c1e78` in the retail `class.def` member of `data/imagery.rvi` (note the **content hash differs** from the snapshot's `legacy/Class.Def:2062` value `0x482dfe82` — same name+path, different `0x???????` hash, which is the `class.def`-line content hash, not an asset MD5; the asset itself is byte-identical). The Ghidra mapping `cls_0x5abe4c` (392 B / 26 vftable entries; `recon/mappings/TFireSwarmEffect_cls_0x5abe4c_candidate.yaml`) is **mis-sized** for the snapshot's tiny `TFireSwarmEffect` (which is empty over `TEffect` plus a 16 B animator) — same MEDIUM-confidence wrong-class problem as F01 / F11; treat the mapping as a registry hit, not a struct match. No spell.def variant in shipped or legacy invokes `"FireSwarm"`, no `ATTACHEFFECT`, no `char.def` reference → **the effect class+asset shipped but it has no live caller in retail data — vestigial in shipped**. The blend (`SetBlendState` = Alpha) and all tuning constants are **snapshot-only** and unverified against a retail Render decomp. See §2.1. |
| **Author / Date** | vfx-forensics-agent (worktree-redo-F05) / 2026-05-29 |
| **Family** | fire (decorative/spell — a swirling scaling cylinder of fire; the inventory's "swarm of small fire particles" hint is **wrong** — it is NOT a particle emitter, it is a **single I3D cylinder mesh** transform-animated over 75 ticks). |
| **Draws** | **one I3D sub-object** — `GetObject(1)` = `tube01` of `Magic\FireSwarm.i3d` (a 64-vertex cylinder mesh authored along the local +Z axis with an embedded 64×128 ARGB4444 flame texture), drawn as a single transformed mesh per render frame. The `box01` sub-object at index 0 (a small red glow disc on the same texture's top half) is **NOT drawn** — `GetObject(1)` skips it. |
| **Archetype(s)** | **(B) I3D-mesh transform-animated** — a single cylinder mesh whose object-space scale and Z-rotation are recomputed every frame from a 3-float per-instance state. **(D) one-shot ramp** — runs once for 75 render-frames then `KillThisEffect()`s itself. Specifically: XY radius grows linearly (`cylhscl += 0.4` per frame, from `0.4` → `30.4`), Z height shrinks linearly (`cylvscl -= 0.4` per frame, from `30.0` → `0.0` at exactly tick 75), and the whole mesh continuously yaws about world-Z (`cylth += 0.5` rad per frame, wrapped at `2π`). No emitter, no sub-particles, no flipbook, no associated light, no audio. |

---

## 1. Summary

`"FireSwarm"` is the spinning, expanding, flattening **cylinder of fire** spell
visual. At spawn the engine instantiates a `Magic\FireSwarm.i3d` mesh and
attaches a `TFireSwarmAnimator`; the animator owns three scalars
(`cylhscl, cylvscl, cylth`) and a `frameon` tick counter. Every Animate tick
(per render frame, since this animator does not gate on sim-tick):

- `cylhscl += 0.4` — the XY radius grows linearly (so the tube **bulges
  outward** like an expanding ring of fire).
- `cylvscl -= 0.4` — the Z height shrinks linearly from `30.0` down to `0.0`
  (so the tube **flattens** vertically as it grows outward).
- `cylth   += 0.5 rad` (~28.6°) — continuous yaw, wrapped at `2π`.
- `frameon++`; on tick 76 the effect calls `KillThisEffect()` and removes
  itself from the world (the mesh dies in place — there is no fade-out beyond
  the natural `cylvscl → 0` collapse to a flat disc).

In Render the cylinder is drawn with a transform built as
`Scale(cylhscl, cylhscl, cylvscl) · RotZ(cylth)` (no translation — the mesh
sits at the effect's world position via the engine's standard concatenation),
the blend state set to **Alpha** (`SetBlendState` → MODULATE + SRC_ALPHA/
INV_SRC_ALPHA, ZWRITE off, ZTEST on). The mesh is a 64-vertex cylinder tube
wrapped with a 64×128 ARGB4444 flame texture (red core, yellow flame
tongues drooping downward — see §10), so the whole thing reads as a
**short-lived ring of fire that spreads outward and collapses to a disc on
the floor**.

The snapshot codes the blend as **Alpha**, the same as F01 Flame and the
glow-family sisters (Flare, SymGlow). The texture is **ARGB4444 with real
per-pixel alpha** (NOT chroma-keyed), so Alpha blend reads correctly even
without a key-color step — the green-keyed-flame story of F01 does **not**
apply here. See §7 BLEND SANITY-CHECK for the additive-vs-Alpha question on
this glow sprite.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/classes/cls_0x5abe4c.cpp` + `cls_0x5abe4c__vftable_5abe4c.cpp` — the
    MEDIUM-confidence candidate per `recon/mappings/TFireSwarmEffect_cls_0x5abe4c_candidate.yaml`.
    Class size **392 B**, vftable **104 B (26 slots)**, parent
    `cls_0x5b0074` (TEffect candidate) per `recon/mappings/EXTRACTION_PASS_2026-05-16.md:60`.
    **WRONG SIZE.** The snapshot's `TFireSwarmEffect` adds zero fields over
    `TEffect`, and the snapshot's `TFireSwarmAnimator` adds 16 B (`int32_t
    frameon` + 3 floats) over `T3DAnimator`. A 392 B retail class is far too
    big for either — it looks like a *different* effect class (probably a
    larger particle-system-owning class) that just happens to consume the
    `"FireSwarm"` string in its imagery-name table (cf. F11's note that
    FireColumn references `"FLAMEDISC"` too — these registrar-style "I load
    these sub-imageries" XREFs are not the same as "I am this class").
    **Treat cls_0x5abe4c as MIS-MAPPED for F05.** The animator/render bodies
    are not isolated in the recon corpus.
  - `recon/classes/_data.txt:107531-107532` — the `"FireSwarm"` string at
    `.rdata 005e11b8` has **2 XREFs** at `004f00d0` and `004f0110`. These
    are the canonical `REGISTER_BUILDER + REGISTER_3DANIMATOR` paired
    factory entries (same pattern as `"FireFlash"` 2 XREFs at
    `_data.txt:107360`, `"FireWind"` 2 XREFs at `:107365`, `"FireBall"` 2
    XREFs at `:108738` — and **distinct** from `"FLAME"`'s **1**-XREF
    pattern at `:107431` which lacks the matching `REGISTER_BUILDER`
    because the snapshot omits the animator registration). So the retail
    binary **does** ship both the effect builder and the animator builder
    for FireSwarm — the snapshot's `DEFINE_BUILDER("FireSwarm", …)` +
    `REGISTER_3DANIMATOR("FireSwarm", …)` are both present in retail.
  - The two XREFs land in the function-pointer table at
    `005c53c8 → 004f00d0` and `005c53cc → 004f0110` (`_data.txt:56018-56021`)
    — consecutive 4 B slots, the canonical "effect ctor / animator ctor"
    pair the same way `0x4e4ea0` registers `"FLAME"` at slot `005c5348`
    (`_data.txt:55954` = F01) and `0x4e53a0` registers `"sparks"` at slot
    `005c5350` (`_data.txt:55958` = SPARKS).
- **Pre-release (snapshot — authoritative for the mechanism):**
  - `TFireSwarmEffect`: `DEFINE_BUILDER("FireSwarm", TFireSwarmEffect)`
    `src/effect_old.cpp:10453`; `REGISTER_BUILDER(TFireSwarmEffect)` `:10454`.
    Class declaration `src/effect.h:1850-1862`; `Initialize` `:10456-10458`
    is **empty**; `Pulse` `:10460-10463` only calls `TEffect::Pulse()` (no
    override behavior — the effect is a pure data shell).
  - `TFireSwarmAnimator`: class declaration `src/effect.h:1868-1890` —
    fields `int32_t frameon, float cylhscl, float cylvscl, float cylth`.
    `REGISTER_3DANIMATOR("FireSwarm", TFireSwarmAnimator)`
    `src/effect_old.cpp:10475`. `Initialize` `:10483-10490`, `Animate(bool)`
    `:10499-10515`, `Render()` `:10524-10549`, `RefreshZBuffer()`
    `:10551-10562` (note: `RestoreZ` call is **commented out** —
    RefreshZBuffer is a no-op, the patch dims are stale comments).
  - `#define FIRESWARM_DURATION 75` `:10469`,
    `FIRESWARM_CYLHSCLSTEP 0.4` `:10470`,
    `FIRESWARM_CYLTHSTEP 0.5` `:10471`,
    `FIRESWARM_CYLVSCLINIT 30` `:10472`,
    `FIRESWARM_CYLVSCLSTEP 0.4` `:10473`.
- **Asset:** `Magic\FireSwarm.i3d`. Registered at
  `legacy/Class.Def:2062` (`"FireSwarm" "Magic\FireSwarm.i3d" 0x482dfe82`)
  and at the retail extracted `class.def` member of `data/imagery.rvi`
  (`"FireSwarm" "Magic\FireSwarm.i3d" 0x582c1e78`). Snapshot file at
  `legacy/Imagery/Magic/fireswarm.i3d` (24,300 B, MD5
  `927caed9595abffae3ea7a939b3a240a`). Retail file extracted from
  `data/imagery.rvi:Imagery/Magic/fireswarm.i3d` (24,300 B, MD5
  `927caed9595abffae3ea7a939b3a240a` — **byte-identical** to snapshot,
  verified via `python -m zipfile`). Date in the retail RVI index:
  `1998-07-13 16:50:24`.
- **Sister effects consulted:**
  - `TQuicksandAnimator::Render` (`src/effect_old.cpp:9265-9333`) — same
    `GetObject(1) + OBJ3D_MATRIX + Scale + RotZ` cylinder rendering
    pattern, also under `SetBlendState` (Alpha). Quicksand draws 2
    cylinders per frame (rotation pair), FireSwarm draws 1; otherwise the
    same architecture confirms the "transform-animated I3D cylinder"
    family.
  - `TFlameAnimator::Render` (`src/effect_old.cpp:4519-4565`, the F01
    forensics doc §6.3) — same `SetBlendState` (Alpha) glow sister. Has
    `OBJ3D_MATRIX | OBJ3D_VERTS` for UV mutation; FireSwarm uses only
    `OBJ3D_MATRIX` (no per-frame UV writes — the texture is static, the
    visual variation comes from the geometry transform).
  - `TFireConeAnimator` (`src/effect_old.cpp:7211-7320`) — sister fire
    spell but uses `TParticleSystem` particles; **completely different
    archetype** from FireSwarm (which is a single mesh). Confirms the
    naming is misleading: "swarm" does not imply a particle system.
  - Blend helpers `SetBlendState` `effect_old.cpp:221-233` (Alpha:
    MODULATE + SRC_ALPHA/INV_SRC_ALPHA, ZWRITE off, ZTEST on) /
    `SetAddBlendState` `:235-244` (Additive: DECALALPHA + ONE/ONE, ZWRITE
    off, ZTEST on) / `SaveBlendState` `:181-191` / `RestoreBlendState`
    `:200-209` (the standard 6-state save/restore wrapper used by all
    `Render()` bodies in `effect_old.cpp`).
- **`spell.def` / placement search:** searched `data/Resources/spell.def`,
  `data/Resources/effects.def`, `data/Resources/rules.def`,
  `data/Resources/master.s`, `data/Imagery/char.def`, `legacy/spell.def`,
  `legacy/rules.def`, `legacy/char.def`, every `data/Modules/*/*.def`,
  and `data/Modules/Ahkuilon_unzipped/*.s` for `FireSwarm` / `fireswarm`
  → **zero hits**. No spell, no rule, no module script, no
  ATTACHEFFECT references the effect at runtime. The class is registered
  but never instantiated by content data — **vestigial in shipped**
  (same status as F11 FlameDisc).
- **Source-of-truth ranking:** the snapshot is **authoritative** for the
  bodies (they are tiny, complete, and the only available source for the
  Animate/Render math + constants). The retail decomp is authoritative for
  the **registration name + asset file existence + byte-identical asset
  payload + the 2-XREF "both registrations shipped" pattern**. The retail
  is **insufficient** for the per-frame math (the `0.4 / 0.5 / 30 / 75`
  constants are snapshot-only) and the **blend** (snapshot codes Alpha
  but the Render body is not in the recon corpus). No spell.def or world
  placement makes the effect runnable in retail, so there is **no
  in-game ground truth** to vet against.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial / vestigial)

`src/effect_old.cpp` is the pre-release snapshot. Four cross-checks:

**(1) Constant grep — NO COVERAGE.** None of the snapshot's FireSwarm
constants (`75` for duration, `0.4` for `CYLHSCLSTEP`/`CYLVSCLSTEP`, `0.5`
for `CYLTHSTEP`, `30` for `CYLVSCLINIT`) appear as isolated patterns in
the recon corpus. The `0x3F000000`-style bit patterns for these floats
hit hundreds of unrelated places. The animator body at the
factory-pointer destination (`0x4f0110` for the animator ctor, or the
Render entry chained off it) was not isolated by the recon pass. **All
tuning constants are snapshot-only.**

**(2) Asset identity — BYTE-IDENTICAL.**
- Snapshot `legacy/Imagery/Magic/fireswarm.i3d`: 24,300 B, MD5
  `927caed9595abffae3ea7a939b3a240a`.
- Retail (extracted via `python -m zipfile` from `data/imagery.rvi`
  member `Imagery/Magic/fireswarm.i3d`): 24,300 B, MD5
  `927caed9595abffae3ea7a939b3a240a`.
- **Identical bytes.** No surface-header fix (unlike F01 Flame's 7 B
  header patch), no payload change. The asset shipped as-is from
  1998-07-13.
- The retail `imagery.rvi` index lists the file as **uncompressed**
  ("stored" ZIP method, compressed_size == uncompressed_size == 24,300 B).

**(3) Registration + naming — CONFIRMED.** The retail binary contains the
`"FireSwarm"` string at `.rdata 005e11b8` with **2 XREFs**
(`recon/classes/_data.txt:107531`):
- `004f00d0` — the `REGISTER_BUILDER`-equivalent ctor/registrar trampoline
  for `TFireSwarmEffect`.
- `004f0110` — the `REGISTER_3DANIMATOR`-equivalent ctor/registrar
  trampoline for `TFireSwarmAnimator`.

These two addresses appear as **consecutive 4 B entries** in the
animator-builder factory table at `005c53c8` and `005c53cc`
(`_data.txt:56018-56021`). This is the **canonical paired-registration
pattern** (`REGISTER_BUILDER + REGISTER_3DANIMATOR` for the same name)
that the snapshot's `effect_old.cpp:10453-10454, 10475` codifies. Compare:

| name      | XREF count | factory-table slots                | snapshot has both? |
|-----------|------------|------------------------------------|--------------------|
| `FLAME`   | 1          | `005c5348 → 004e4ea0` only         | NO (snapshot lacks `REGISTER_3DANIMATOR("FLAME", …)`) — F01 §2.1 |
| `sparks`  | 1          | `005c5350 → 004e53a0` only         | NO (registered as `REGISTER_3DANIMATOR_("blood", TBloodAnimator, TBloodParticle3DAnimator)`-style) — see SPARKS doc |
| `FireFlash` | 2        | `005c5310 → 004e1750`, `005c5314 → 004e18a0` | YES — `effect_old.cpp:1989, 2111` |
| `FireWind`  | 2        | `005c5318 → 004e2250`, `005c531c → 004e23e0` | YES — `effect_old.cpp:2517, 2684` |
| **`FireSwarm`** | **2**| **`005c53c8 → 004f00d0`, `005c53cc → 004f0110`** | **YES — `effect_old.cpp:10453, 10475`** |
| `FireBall`  | 2        | `00510bb0`, `00510ce0`             | YES (missile family) |

So the registration **matches the snapshot exactly** — both the effect
builder and the animator builder shipped.

The asset `Magic\FireSwarm.i3d` is also registered in the shipped
`class.def` (extracted from `data/imagery.rvi` at offset `0x8988c`):
`"FireSwarm" "Magic\FireSwarm.i3d" 0x582c1e78` (vs. snapshot's
`legacy/Class.Def:2062` value `0x482dfe82`). The `0x???????` is the
class.def-line content hash (an engine-internal ID stamp), not an asset
MD5 — the **path and registry name are identical**, so the engine
resolves the same asset regardless of the differing line-hash.

**(4) Structure/layout — NOT corroborated.** As noted above, `cls_0x5abe4c`
is **392 B / 26 vftable slots**, which is far too big for the snapshot's
`TFireSwarmEffect` (`sizeof(TEffect) + 0 = ~60-80 B`) or
`TFireSwarmAnimator` (`sizeof(T3DAnimator) + 16 B`). The candidate yaml
(`TFireSwarmEffect_cls_0x5abe4c_candidate.yaml:13`) itself notes
"Effect vs Animator — same as TStreamerEffect candidate yaml" — i.e. the
mapping pass couldn't decide which of the two snapshot classes this
394-B retail class is. The **likeliest explanation** (per F01's
analogous note) is that `cls_0x5abe4c` is a *third* class entirely (some
larger fire-particle-system owner) that consumes the `"FireSwarm"`
string for its own sub-imagery registry, and the actual
`TFireSwarmEffect` / `TFireSwarmAnimator` bodies live at the factory
trampolines `0x4f00d0` and `0x4f0110` that the recon pass did not
isolate. The 26-vtable-slot count is more consistent with a sibling of
`TFireBallEffect` (which has many vtable slots) than with a `TEffect`
leaf that adds zero override methods.

**(5) Caller search — NONE.** No spell.def, no rules.def, no module
script (`.s`/`.def`), no `char.def` ATTACHEFFECT references
`"FireSwarm"` in either shipped or legacy data. The class registers, the
asset ships, but **no live content selects it**. Same status as F11
FlameDisc. The effect is **vestigial in shipped retail**.

**Verdict — retail-partial / vestigial.** The *registration name +
asset (byte-identical) + paired effect/animator factory entries* are
**retail-confirmed**. The *per-frame mechanism, all 5 tuning constants,
the blend choice, the cylinder-tube transform, and the GetObject(1)
sub-object pick* are **snapshot-only** and unverified against a retail
Render decomp. The effect has **no live caller** in shipped data, so
there is no in-game ground truth — reconstruction must work from the
snapshot + the byte-identical asset alone, and visually self-test.

Risk to reconstruction: highest on **the blend** (§7 BLEND
SANITY-CHECK: a bright-yellow flame-on-transparent ARGB4444 cylinder is
arguably additive — the snapshot codes Alpha, which is the same pattern
F01 Flame and Sparks have where the snapshot blend turned out
retail-wrong for Sparks). Secondary risk is the **constant** `FIRESWARM_CYLVSCLINIT
= 30`: that initial Z-scale of 30 means the cylinder starts as a
**tall thin column** (radius 0.4 wu × height 30 wu = 75× aspect ratio).
Whether that matches the artists' intent or is a vestigial pre-release
value (e.g. should have been 5 or 10) cannot be verified without a
retail capture, and the effect has no retail capture. See §13.

---

## 3. Constants

Every numeric the effect actually uses, with citation. The `confirmed?`
column reflects §2.1 — only the registration + asset are
retail-corroborated.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| builder name | `"FireSwarm"` (case-sensitive — the registry lookup matches the registered name verbatim) | string | `effect_old.cpp:10453, 10475`, `Class.Def:2062` | **yes (retail)** — `s_FireSwarm_005e11b8`, `_data.txt:107531` |
| effect-builder factory entry | `0x4f00d0` (the `REGISTER_BUILDER` trampoline) | function pointer | `effect_old.cpp:10453-10454` | **yes (retail)** — `005c53c8 → LAB_004f00d0`, `_data.txt:56018` |
| animator-builder factory entry | `0x4f0110` (the `REGISTER_3DANIMATOR` trampoline) | function pointer | `effect_old.cpp:10475` | **yes (retail)** — `005c53cc → LAB_004f0110`, `_data.txt:56020` |
| asset path | `Magic\FireSwarm.i3d` | path | `Class.Def:2062`, retail `class.def` in `data/imagery.rvi` | **yes (retail)** — same path + same registry name in both |
| asset content hash (snapshot) | `0x482dfe82` | u32 | `Class.Def:2062` | snapshot-only |
| asset content hash (retail) | `0x582c1e78` | u32 | retail `class.def` member of `data/imagery.rvi` (search offset `0x8988c`) | **yes (retail)** — differs from snapshot (engine-internal stamp re-hashed; asset bytes are identical) |
| **FIRESWARM_DURATION** | **75** (ticks; the effect calls `KillThisEffect()` when `frameon > 75`, so it lives for frames 0..75 inclusive = **76 ticks**) | render-frames | `effect_old.cpp:10469` | **snapshot-only** |
| **FIRESWARM_CYLHSCLSTEP** | **0.4** (added to `cylhscl` per Animate tick) | scale-units/tick | `effect_old.cpp:10470` | **snapshot-only** |
| **FIRESWARM_CYLTHSTEP** | **0.5** (added to `cylth` per Animate tick) | rad/tick (≈28.6°/tick) | `effect_old.cpp:10471` | **snapshot-only** |
| **FIRESWARM_CYLVSCLINIT** | **30** (initial `cylvscl`) | scale-units (= wu after mesh scale) | `effect_old.cpp:10472, 10489` | **snapshot-only** |
| **FIRESWARM_CYLVSCLSTEP** | **0.4** (subtracted from `cylvscl` per Animate tick) | scale-units/tick | `effect_old.cpp:10473` | **snapshot-only** |
| `cylhscl` init | `FIRESWARM_CYLHSCLSTEP = 0.4` (the first Animate brings it to 0.8) | scale-units | `effect_old.cpp:10488` | **snapshot-only** |
| `cylvscl` init | `FIRESWARM_CYLVSCLINIT = 30.0` | scale-units | `effect_old.cpp:10489` | **snapshot-only** |
| `cylth` init | `0.0` | rad | `effect_old.cpp:10487` | **snapshot-only** |
| `frameon` init | `0` | tick | `effect_old.cpp:10486` | **snapshot-only** |
| `cylth` wrap | at `M_2PI` (`= 2π ≈ 6.2832`), subtracted; **NOT** modulo — single subtraction per overflow | rad | `effect_old.cpp:10507-10508` | **snapshot-only** |
| sub-object index | `1` (= `tube01`, the cylinder mesh; `box01` at index 0 is **not drawn**) | index | `effect_old.cpp:10531`; asset object table at file offsets `0xac` (`box01`) and `0xdc` (`tube01`) | **snapshot-only** for the pick; the asset's 2-sub-object count is verified from the I3D file (§4) |
| obj flags | `OBJ3D_MATRIX (0x100)` only — note: **no `OBJ3D_VERTS`** (the mesh's authored verts are used as-is — no per-frame UV mutation) | bitmask | `effect_old.cpp:10532` + `src/3dimage.h:113, 118` | **snapshot-only** |
| `obj->scl` write | `(cylhscl, cylhscl, cylvscl)` — uniform XY, independent Z | × | `effect_old.cpp:10535-10538` | **snapshot-only** |
| matrix build order | `D3DMATRIXClear → D3DMATRIXScale(scl) → D3DMATRIXRotateZ(cylth)` — chained right-mul, so effective `m = Scale · RotZ` (note: NO translate — mesh sits at parent effect's world pos via engine concatenation) | helper chain | `effect_old.cpp:10533, 10538, 10540` | **snapshot-only** |
| blend helper | `SetBlendState()` → MODULATE + SRC_ALPHA/INV_SRC_ALPHA, ZWRITE off, ZTEST on (= **Alpha**) | render state | `effect_old.cpp:10527`; helper body `:221-233` | **snapshot-only** (render body not in retail decomp) — **SUSPECT** (see §7 BLEND SANITY-CHECK) |
| kill condition | `frameon > FIRESWARM_DURATION (= 75)` → `KillThisEffect()` (sets `OF_KILL | OF_PULSE`, signals parent spell) | gate | `effect_old.cpp:10511-10514`; helper `:426-432` | **snapshot-only** |
| RefreshZBuffer patch w/h | `50, 50` — **but the `RestoreZ` call is commented out** (`effect_old.cpp:10561`), so RefreshZBuffer is a **no-op** | screen px | `effect_old.cpp:10553-10554, 10561` | **snapshot-only** (vestigial constants) |
| M_2PI | `2 × M_PI = 6.2831853...` | rad | `src/revdefs.h:24` | helper |
| asset surface format | **ARGB4444** (NOT RGB565 — masks `A=0xF000, R=0x0F00, G=0x00F0, B=0x000F` at file offsets `0x1ec0-0x1ed0`) | pixel format | I3D bytes `0x1ec0`+ | yes (asset-verified) |
| asset surface dims | **64 × 128** pixels (payload at file offset `0x1ee8`, runs 16,384 B to `0x5ee8`) | px | I3D byte-level decode | yes (asset-verified — fits exactly, see §4) |

`Animate(bool draw)` runs **per Animate call** in this animator (the
snapshot does not gate by sim-tick — every call increments `frameon` and
the three accumulators). Following the project's
`feedback-framerate-independent-anim` note, reconstruction should treat
these as `per-second` rates derived from the original 24 Hz baseline:

- `cylhscl_rate = 0.4 × 24 = 9.6` scale-units/sec
- `cylvscl_rate = 0.4 × 24 = 9.6` scale-units/sec
- `cylth_rate = 0.5 × 24 = 12.0` rad/sec ≈ 687.5°/sec ≈ **1.91 rotations/sec**
- `duration = 75 / 24 = 3.125` seconds (effect dies at ~3.13 s)

At the duration limit (`frameon = 75`, just before kill):
- `cylhscl = 0.4 + 0.4 × 75 = 30.4` (XY radius peaks at ~30 scale-units)
- `cylvscl = 30.0 − 0.4 × 75 = 0.0` (Z height collapses to 0 exactly at kill)
- `cylth = 0.5 × 75 = 37.5 rad ≈ 5.97 full rotations`

So the cylinder makes **~6 full spins** over its ~3.1 s lifetime while
expanding to ~30 wu radius and flattening to 0 wu height — a
**ground-collapsing ring of fire** that ends as a flat horizontal disc
in world space.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| FireSwarm | snapshot `legacy/Imagery/Magic/fireswarm.i3d` (24,300 B, MD5 `927caed9595abffae3ea7a939b3a240a`); retail `data/imagery.rvi:Imagery/Magic/fireswarm.i3d` (24,300 B, MD5 `927caed9595abffae3ea7a939b3a240a`) — **byte-identical** (§2.1.2) | 24,300 B | the swirling fire-cylinder mesh — 2 sub-objects (`box01`, `tube01`); only `tube01` is drawn by F05; embedded ARGB4444 64×128 flame texture | registered `Class.Def:2062` and the retail `class.def` member as `"FireSwarm" "Magic\FireSwarm.i3d"`; loaded by the OBJCLASS_EFFECT registry on instance spawn; the `tube01` sub-object bound via `GetObject(1)` `effect_old.cpp:10531` |

**Sub-objects (2)** — confirmed from the I3D object table:
- `GetObject(0) = "box01"` — name at file offset `0xac`. **NOT drawn** by F05 (the animator skips it).
- `GetObject(1) = "tube01"` — name at file offset `0xdc`. **The only sub-object drawn.**

The `box01` index-0 object is the small red glow disc visible at the **top
half** of the texture preview (see §10) — it appears to be an authored
"core glow" mesh the developers built into the same I3D but the
`TFireSwarmAnimator` opted not to use. Its presence in the asset is
likely vestigial (or staged for a future variant that never landed) —
reconstruction should preserve its existence in the asset but only draw
`tube01`.

**`tube01` geometry** (decoded from float vertex data starting around
file offset `0x100`-ish):
- Cylinder mesh authored along the local **+Z axis** (the height direction
  that the per-frame `cylvscl` scale modulates).
- 64 vertices arranged in vertical strips around the cylinder (face index
  buffer visible at `0x1bd0`+ with small u16 indices like `0x93, 0x4a,
  0x96, 0x93`...).
- The texture wraps around the cylinder circumference (U axis) and
  vertically along the cylinder height (V axis). Per-vertex UVs are
  authored at I3D build time — **never mutated at runtime** (the Render
  body does not write `lverts[*].tu/tv`; in fact `OBJ3D_VERTS` is **not
  set** so the effect doesn't own its own vert buffer).
- Material: a default `S3DMat` with `power = 0.9` (the `0x3f666666` =
  `0.9f` float seen at `0x1d78` is the standard "shiny" power), color
  values all `1.0` (`0x3f800000` from `0x1d58`-`0x1d74`).

**Texture decode (the cylinder skin):**

- **Dimensions:** **64 × 128 pixels** (16,384 B payload at file offsets
  `0x1ee8..0x5ee8`). Header bytes at `0x1ec0..0x1ee0` confirm:
  | offset | u16 LE | meaning |
  |--------|--------|---------|
  | `0x1ec0` | `0x0010` (= 16) | **bits per pixel** |
  | `0x1ec4` | `0x0f00` | **R mask** (4-bit, bits 8-11) |
  | `0x1ec8` | `0x00f0` | **G mask** (4-bit, bits 4-7) |
  | `0x1ecc` | `0x000f` | **B mask** (4-bit, bits 0-3) |
  | `0x1ed0` | `0xf000` | **A mask** (4-bit, bits 12-15) |

  These masks unambiguously identify the format as **ARGB4444**, not
  RGB565 (the typical Revenant texture format). This is **the relevant
  asset-level fact for the blend question** — ARGB4444 has a real alpha
  channel, so the texture does **not** need chroma-keying to render
  transparent edges, and the snapshot's `SetBlendState` Alpha blend can
  read the alpha channel literally. Compare F01 Flame (`Magic\flame.i3d`)
  which is RGB565 with a green chroma key — completely different
  transparency mechanism.

- **No flipbook, no atlas.** The texture surface count is `numtex = 1`
  (`numframes = 1`) — the cylinder shows the **same** texture every
  frame. The visual motion comes from the geometry transform (RotZ
  yaw + radial expand + vertical collapse), not from texture animation.
  This is consistent with the snapshot Render body never calling
  `SetTextureFrame()` and never writing `lverts[*].tu/tv`.

- **Pixel content (decoded into a preview PNG by extracting the 16,384 B
  payload and re-mapping ARGB4444 → 8-bit RGBA, composited on black):**
  | region | description |
  |--------|-------------|
  | top ¼ (64×32 ish) | a small **bright red ball of light** centered horizontally — appears to be the texture for the **unused `box01` sub-object** (a glow disc). Alpha 0 around it, red core. |
  | bottom ¾ (64×96 ish) | **drooping flame tongues** — bright **yellow-white** at the top (`(255,255,238)`, `(255,255,221)`), fading down through orange (`(255,170,68)`, `(255,136,51)`) to darker red, with **alpha increasing from 0 at the edges to 255 at the flame cores**. The bottom of the texture has 4-5 visible flame-tongue spikes drooping downward. |

  Decoded color samples (ARGB4444 → 8-bit RGB):
  - `0xffff` (255, 255, 255) — white flame core (peak)
  - `0xfffe` (255, 255, 238) — pale yellow
  - `0xffed` (255, 238, 221) — light yellow/cream
  - `0xefec` (255, 238, 204) — pale yellow
  - `0xcfdb` (255, 204, 187) — pale orange
  - `0xafb8` (255, 187, 136) — orange
  - `0x8fa6` (255, 170, 102) — burnt orange
  - `0x7fa5` (255, 170, 85) — burnt orange
  - `0x6f83` (255, 136, 51) — red-orange (flame body)
  - `0x4f60` (255, 102, 0) — red core
  - `0x3f60` (255, 102, 0) with α=51 — flame edge
  - `0x2f00` (255, 0, 0) with α=34 — dark red ember edge
  - `0x0f00` (255, 0, 0) with α=0 — **transparent background** (this dominates the texture, with alpha=0 so it never contributes to the framebuffer)

  **Chroma-key — NONE.** Unlike F01 Flame (`Magic\flame.i3d`, RGB565
  green-keyed), this texture uses **real per-pixel ARGB alpha**.
  Reconstruction must respect the alpha channel literally; do NOT apply
  a chroma-key conversion. (Any chroma-key code path the engine has for
  RGB565 imagery should be skipped for this asset based on the
  16-bpp-but-ARGB4444 surface descriptor at file offset `0x1ec0`.)

- **Wrap convention.** Cylinder UVs typically wrap U around the
  circumference (so the texture U=0 and U=1 edges meet at the cylinder
  seam) and V along the height (V=0 at one end, V=1 at the other). The
  cylinder authored verts encode this directly; the engine clamps or
  tiles per the I3D material flags (default WRAP — standard Revenant
  cylinder skinning).

The effect loads a real asset — do NOT substitute a procedural cylinder
or a generic ring-of-fire (per `feedback-no-standins`). The yellow-tipped
ARGB4444 flame skin is the visual identity; a procedural ring would
miss the drooping flame-tongue silhouettes that define the look.

---

## 5. Spawn & emit

- **Trigger semantics:** **one-shot, 76-tick lifetime.** The animator
  has a hard kill at `frameon > 75` (`effect_old.cpp:10511-10514`):
  `KillThisEffect()` sets `OF_KILL | OF_PULSE` and signals the parent
  spell to die. The effect runs once from spawn to death. **No looping,
  no rebirth, no fade-out beyond the natural `cylvscl → 0` collapse.**
- **Count per trigger:** **1 mesh** — a single `tube01` cylinder, rendered
  once per `Render()` call (`effect_old.cpp:10524-10549`). No per-instance
  multiplication, no particle array, no sub-emit.
- **Initial direction / distribution:** N/A — this effect is **NOT** a
  particle emitter. The cylinder sits at the effect object's world
  position (the engine concatenates parent transform at draw; the
  `Render()` body sets no `obj->pos`, so `pos` stays at its default —
  the parent's world origin from the engine concat).
- **Emit anchor convention:** **at the spawn call's exact `(x, y, z)`** —
  the world position of the `TFireSwarmEffect` instance, set by the
  spawning spell at construction (theoretical — no live spell caller
  exists in shipped data, §2.1.5). The animator does not ground-project,
  doesn't follow a character, doesn't read a bone matrix.
- **Coordinate space:** the mesh lives in **local space** (authored along
  local +Z axis), then the engine concatenates the parent effect's world
  transform at render. With no explicit translate in the matrix build
  (`D3DMATRIXClear → Scale → RotZ`, `effect_old.cpp:10533-10540`), the
  mesh is centered at the parent's world position. Both XY-scale and the
  Z-axis-yaw are applied in this local frame.
- **Spread / jitter:** **none** — no randomness in spawn. The 3 floats
  (`cylhscl, cylvscl, cylth`) are initialized deterministically
  (`effect_old.cpp:10487-10489`), so every spawn produces the same
  visual sequence. (The only natural variation between two spawned
  FireSwarms is whatever the parent spell's world placement does.)

### Spatial diagram (single cylinder mesh — life trajectory)

```
   wz (up)
   │
   │  ┌─────┐                                    Authored tube01 cylinder
   │  │     │   ← FIRESWARM_CYLVSCLINIT = 30      mesh, oriented along +Z.
   │  │     │     local Z-height                  Texture wraps U around the
   │  │     │     scl.z starts at 30              tube; V is the height axis.
   │  │     │
   │  │tube │   frameon=0: scl=(0.4, 0.4, 30.0)
   │  │     │       tall thin column, rad ~0.4, h ~30
   │  │ 01  │
   │  │     │
   │  └─────┘                            cylth=0
   │
   │  vs.
   │   ╭───╮     frameon=37 (mid-life):
   │   │   │       cylhscl ≈ 15.2, cylvscl ≈ 15.2
   │   │   │       ~ equal radius and height (a unit cylinder)
   │   ╰───╯       cylth ≈ 18.75 rad (≈3 rotations)
   │
   │  ▆▆▆▆▆▆▆▆▆   frameon=75 (just before kill):
   │              cylhscl ≈ 30.4, cylvscl ≈ 0.0
   │              flat disc on the ground plane (radius ~30 wu)
   │              cylth ≈ 37.5 rad (≈6 rotations total)
   │
   └─────────────── wx, wy
       ●           ← emit origin = world (x,y,z) of the TFireSwarmEffect
                      instance (parent spell-target / cast-position)
```

**Temporal envelope** (per-frame state — selected sample frames):

| frameon | cylhscl (X/Y radius scale) | cylvscl (Z height scale) | cylth (yaw rad) | shape                                  |
|---------|----------------------------|--------------------------|-----------------|----------------------------------------|
| 0 (init)| **0.4**                    | **30.0**                 | 0.0             | tall thin column, ~0.4 wide × 30 tall  |
| 1       | 0.8                        | 29.6                     | 0.5             | still narrow + tall                    |
| 10      | 4.4                        | 26.0                     | 5.0  (~287°)    | wider, still tall                      |
| 25      | 10.4                       | 20.0                     | 12.5 (~2.0 rotations) | radius = height = 10–20             |
| 37      | 15.2                       | 15.2                     | 18.75 (~3.0 rotations) | unit-aspect cylinder            |
| 50      | 20.4                       | 10.0                     | 25.0 (~4.0 rotations) | wider, lower                     |
| 75      | **30.4**                   | **0.0**                  | 37.5 (~6.0 rotations) | flat horizontal disc on ground |
| 76      | (dead)                     | (dead)                   | (dead)          | `KillThisEffect()` fires               |

The crossover (when radius = height) is at `frameon ≈ 37.0` (when both
`cylhscl` and `cylvscl` equal `15.2`). Before that the effect is a
**tall column**, after it's a **flat disc**. The full transition
takes ~3.1 s at the original 24 Hz baseline.

---

## 6. Behavior & per-frame logic

The animator's full per-frame logic is short. The effect (object) side has
**no override bodies** — only the inherited `TEffect`/`TObjectInstance`
plumbing (the `Pulse` body at `:10460-10463` only calls `TEffect::Pulse()`).

### 6.1 `TFireSwarmEffect::Initialize()` (`effect_old.cpp:10456-10458`)

```
Initialize():
    // empty body — no override state
```

### 6.2 `TFireSwarmEffect::Pulse()` (`effect_old.cpp:10460-10463`)

```
Pulse():
    TEffect::Pulse()                       // inherited; resets SetFrame(0) +
                                           // SetCommandDone(false); :418-424
```

The Pulse body is purely passthrough — the effect-instance side has no
state machine, no transitions, no kill logic. **All behavior lives in
the animator.**

### 6.3 `TFireSwarmAnimator::Initialize()` (`effect_old.cpp:10483-10490`)

```
Initialize():
    T3DAnimator::Initialize()              // base: standard animator setup    // :10485
    frameon = 0                            // tick counter                     // :10486
    cylth   = 0.0                          // yaw angle (rad)                  // :10487
    cylhscl = FIRESWARM_CYLHSCLSTEP = 0.4  // XY radius scale (matches one     // :10488
                                           //   tick's increment, so the
                                           //   first Render frame already
                                           //   has cylhscl > 0)
    cylvscl = FIRESWARM_CYLVSCLINIT = 30.0 // Z height scale                   // :10489
```

Note: there is **no `GetObject(…)`** or `GetVerts(…)` call here (unlike
F01 Flame which binds its sub-object and allocates a per-instance lvert
buffer at Initialize). FireSwarm does its sub-object pick in Render
(`GetObject(1)` per call) — the mesh's authored verts are used as-is,
no per-instance buffer is allocated.

### 6.4 `TFireSwarmAnimator::Animate(bool draw)` (`effect_old.cpp:10499-10515`)

```
Animate(draw):
    T3DAnimator::Animate(draw)             // base: refresh hierarchy, mirror  // :10501
                                           //   pos from inst, etc.
    inst->SetCommandDone(false)            // keep the animator alive          // :10502

    frameon++                                                                  // :10504

    cylth += FIRESWARM_CYLTHSTEP = 0.5     // yaw advance (rad)                // :10506
    if (cylth > M_2PI):                                                        // :10507
        cylth -= M_2PI                     // wrap (single subtraction,       // :10508
                                           //   NOT modulo — overflow is
                                           //   one step max since the
                                           //   accumulator only grows by
                                           //   0.5 per tick)
    cylhscl += FIRESWARM_CYLHSCLSTEP = 0.4 // XY radius scale grows           // :10509
    cylvscl -= FIRESWARM_CYLVSCLSTEP = 0.4 // Z height scale shrinks          // :10510

    if (frameon > FIRESWARM_DURATION = 75):                                    // :10511
        ((TEffect*)inst)->KillThisEffect() // OF_KILL | OF_PULSE,             // :10513
                                           //   parent spell signaled         // (:426-432)
```

That's the entire per-frame update. **No per-instance phase, no
velocity, no gravity, no spawn rate, no sub-emit, no envelope curve
beyond the three linear ramps**. The `cylth` wrap is `if > 2π then
−2π` (single subtraction) — fine because the per-tick step is only
0.5 rad and so the accumulator can never overflow by more than one
`2π` period per tick.

**Note on the `inst->SetCommandDone(false)` call**: this keeps the
animator "running" from the engine's perspective every tick (otherwise
the engine would consider the animator finished and stop calling it).
This pattern is used by every long-lived animator in the snapshot —
it's the opposite of the missile-family pattern where
`SetCommandDone(true)` signals LAUNCH→FLY hand-off.

### 6.5 `TFireSwarmAnimator::Render()` (`effect_old.cpp:10524-10549`)

```
Render():
    SaveBlendState()                       // snapshot D3D states              // :10526
                                           //   (TEXTUREMAPBLEND, ZWRITE,
                                           //    ZENABLE, CULLMODE,
                                           //    SRCBLEND, DESTBLEND;
                                           //    helper :181-191)
    SetBlendState()                        // → Alpha (MODULATE + SRC_ALPHA/  // :10527
                                           //          INV_SRC_ALPHA, ZWRITE
                                           //          off, ZENABLE on)
                                           //   [helper :221-233]
                                           //   [SUSPECT — see §7 BLEND
                                           //    SANITY-CHECK; render body
                                           //    not in retail decomp]

    obj = GetObject(1)                     // bind tube01 sub-object           // :10531
                                           //   (note: NOT GetObject(0);
                                           //    box01 is unused)
    obj->flags = OBJ3D_MATRIX              // 0x100 — explicit matrix only.   // :10532
                                           //   NO OBJ3D_VERTS (mesh verts
                                           //   are read-only; engine uses
                                           //   asset's authored UVs)

    D3DMATRIXClear(&obj->matrix)           // identity                        // :10533

    obj->scl = (cylhscl, cylhscl, cylvscl) // non-uniform: XY = same scale,   // :10535-10537
                                           //   Z = independent scale
    D3DMATRIXScale(&obj->matrix, &obj->scl)// chained scale                   // :10538

    D3DMATRIXRotateZ(&obj->matrix, cylth)  // chained yaw about local Z       // :10540
                                           //   (the cylinder's axis,
                                           //   which is the mesh's
                                           //   "height" axis)

    // NO translate — obj->pos not written; engine concatenates parent
    // effect's world transform at draw time.

    ResetExtents()                         // clear bounding rect             // :10542
    RenderObject(obj)                      // submit the cylinder mesh        // :10543
    UpdateExtents()                        // refresh on-screen rect          // :10544

    RestoreBlendState()                    // pop saved states                // :10546
    return true                                                               // :10548
```

The matrix is built **right-multiplicatively** (`m = ID · Scale ·
RotZ`), per the `D3DMATRIX*` helpers' chained-right-multiply convention.
The effective transform is `m = Scale(cylhscl, cylhscl, cylvscl) ·
RotZ(cylth)` — the mesh is **first scaled** (non-uniformly) then
**yawed** about its local Z axis. Because the scale is non-uniform
(`(s, s, h)` with `s ≠ h`), the rotation interacts with the scale in a
visible way: the yaw of the texture-coordinate cylinder around its
axis is the dominant motion, but as `cylvscl → 0` the mesh degenerates
into a flat XY disc and the RotZ becomes a rotation of the disc-shape
about its vertical axis (visually like a spinning saucer).

> **Orientation classification.** The matrix has NO explicit ground-tip
> (`rot.x = -π/2`), so the cylinder is **not WorldXY**. It is also NOT
> ScreenAligned (no per-frame camera-facing math). The cylinder's
> authored local +Z axis becomes the **world-up direction** under the
> engine's standard parent-transform concatenation (assuming the parent
> effect has no explicit rotation). So this is, in NOMENCLATURE §2
> terms, a **WorldUpAligned mesh** — pinned to world +Z, free to yaw
> about Z (and the animator does yaw it). The "spinning cylinder of
> fire that flattens to a ground disc" reading depends on this
> orientation being correct in the parent transform.

### 6.6 `TFireSwarmAnimator::RefreshZBuffer()` (`effect_old.cpp:10551-10562`)

```
RefreshZBuffer():
    size_x = 50                                                                // :10553
    size_y = 50                                                                // :10554
    inst->GetPos(effect)                   // world pos of parent              // :10557
    WorldToScreen(effect, screen)          // project to screen px             // :10559

    // RestoreZ(screen.x - size_x/2, screen.y + 20 - size_y/2, size_x, size_y);
    //                                                                         // :10561
    //  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //  COMMENTED OUT in source — the body is a no-op.
```

The `RestoreZ` call is **commented out** at `:10561` (the `//RestoreZ(...)`
prefix in the snapshot source — verified by reading
`effect_old.cpp:10561`). So `RefreshZBuffer` does **no Z-buffer
restoration** — the 50×50 patch size is a vestigial leftover. The body
just computes screen coords and returns. This means the effect either:
(a) doesn't need a Z patch because its Alpha+ZWRITE-off draw doesn't
disturb depth (likely), or (b) the dev intentionally disabled it
because the cylinder's footprint changes too dramatically across the
75-tick life for a single fixed patch to cover.

### Temporal diagram (one-shot ramp, three linear envelopes)

```
              0 ────── 25 ────── 50 ────── 75   frameon (ticks)
              │       │         │         │
   cylhscl  0.4 ─────────────────────── 30.4   (X/Y radius scale; linear up)
              │       │         │         │
   cylvscl 30.0 ─────────────────────── 0.0    (Z height scale; linear down)
              │       │         │         │
   cylth    0.0 ─────────────────────── 37.5   (yaw rad; wraps at 2π = 6.28)
              │       │         │         │
                                          └── KillThisEffect() at frameon > 75
                                              (effect lives ticks 0..75 = 76 ticks)
```

At a nominal 24 Hz the full cycle takes `75 / 24 ≈ 3.13 s`. The
cylinder makes `37.5 / 2π ≈ 5.97` full rotations and ends as a flat
ground-disc of radius ~30 wu.

---

## 7. Rendering (original render state + geometry)

- **What it draws:** **one mesh** — the `tube01` sub-object (index 1) of
  `Magic\FireSwarm.i3d`. A 64-vertex cylinder authored along the mesh's
  local +Z axis, skinned with a 64×128 ARGB4444 flame texture (red core,
  yellow flame-tongue silhouettes). The `box01` sub-object at index 0
  (the small red glow disc on the same texture's top half) is **NOT
  drawn**.
- **Blend mode (original — what the code actually does):** **Alpha
  (modulated)** via `SetBlendState()` (`effect_old.cpp:10527`; helper
  body at `:221-233`). The render states set:
  - `D3DRENDERSTATE_TEXTUREMAPBLEND = D3DTBLEND_MODULATE` (`:223`) —
    texel multiplied by interpolated per-vertex diffuse
  - `D3DRENDERSTATE_ZWRITEENABLE = false` (`:224`) — no depth write
  - `D3DRENDERSTATE_ZENABLE = true` (`:225`) — depth test on
  - `D3DRENDERSTATE_SRCBLEND = D3DBLEND_SRCALPHA` (`:228`)
  - `D3DRENDERSTATE_DESTBLEND = D3DBLEND_INVSRCALPHA` (`:229`)

  Classify as **Alpha** (NOMENCLATURE §3) — the same `SetBlendState()`
  mode F01 Flame, Flare, SymGlow, and the Quicksand cylinder pair use.

  ### BLEND SANITY-CHECK (mandatory)

  **Sprite design:** the texture is a **bright yellow-white flame-tongue
  silhouette on a fully-transparent ARGB4444 background** (§4: alpha=0
  for the background pixels, alpha=255 at the bright flame cores, with
  smooth alpha gradients at the flame edges). The texture is **ARGB4444
  with real per-pixel alpha** (NOT chroma-keyed) — unlike F01 Flame
  which is RGB565 green-keyed.

  A bright-yellow flame on a real-alpha background is the **textbook
  alpha-blend sprite** — the ARGB4444 format itself signals "the
  artist authored real alpha; respect it literally." The snapshot's
  Alpha (`SetBlendState`) reads the alpha channel correctly and
  composites the flame over the scene. **This is the strongest
  retail-fidelity signal we have**: the asset format itself implies
  the blend choice.

  **However**, fire is canonically additive in this engine for spell
  effects:
  - `TFireFlashAnimator` (`src/effect_old.cpp:1989`+) uses
    `SetAddBlendState` (Additive).
  - `TFireWindAnimator` (`:2517`+) uses `SetAddBlendState`.
  - `TBurnAnimator` (`:3215`+) uses `SetAddBlendState`.
  - `TAuraAnimator` (`:3480`+) uses `SetAddBlendState`.

  The **only** snapshot fire-family Alpha effects are F01 Flame (ambient
  torch) and F05 FireSwarm (ambient/spell-cylinder). So **the question
  for FireSwarm is: did the artists author the ARGB4444 alpha because
  they explicitly wanted standard alpha compositing (e.g. to show the
  flame as a translucent overlay), or did they author the alpha for an
  additive blend that uses RGB but treats alpha as a fall-off mask?**

  **Sister-family cross-check:**
  - `TFlameAnimator::Render` (`effect_old.cpp:4521-4522`) — **Alpha**.
    Glow-family sister, but the asset is RGB565+green-key — different
    asset format, but same Alpha blend (and the F01 doc §7
    BLEND-SANITY-CHECK flags Alpha as SUSPECT for the same reason).
  - `TQuicksandAnimator::Render` (`effect_old.cpp:9335`) — **Alpha**
    (`SetBlendState`). Cylinder-pair sister, same blend. The Quicksand
    visual is dust/sand swirl, not fire, so Alpha is more natural there.
  - `TFireConeAnimator` (`effect_old.cpp:7211`+) — particle-system
    sister fire spell. Uses `SetAddBlendState` (Additive). **Different
    blend from FireSwarm** despite both being fire spells.

  **Verdict (with the SPARKS-protocol caveat):** classify the snapshot
  as **Alpha** per code, but **flag it as snapshot-only-and-suspect**
  for the reconstruction. Two facts pull in opposite directions:
  1. The asset is ARGB4444 with real per-pixel alpha — the format
     **strongly implies** Alpha is intentional (the artist wouldn't
     bother authoring alpha if the blend was going to be additive).
  2. Every other fire-family **spell** effect uses Additive
     (FireFlash, FireWind, Burn, Aura, FireCone), so a fire-spell
     ground-disc visual that uses Alpha is an outlier.

  The ARGB4444 asset format is the **stronger** signal — it's a hard
  artist-encoded fact, vs. blend choice which is a code-side
  pre-release WIP knob. Reconstruction should default to **Alpha** per
  the snapshot + asset, and the visual-vet against an in-game capture
  is impossible (no live caller — §2.1.5), so the only fallback is to
  visually self-vet the cylinder + flame texture against either the
  asset's standalone rendering or a sister Alpha-glow capture (e.g. an
  F01 torch). If the rendered result looks wrong (translucent ground
  disc rather than a luminous ring), switch to AdditiveStraight and
  re-check.

- **Lit vs self-lit:** classify as **Unlit** (NOMENCLATURE §4 — texture
  color is literal). The per-vertex diffuse is whatever the engine's
  mesh-load default produced (typically opaque white for an authored
  cylinder mesh), and the Render body does **not** zero the material
  (unlike `TFlareAnimator`/`TSymGlowAnimator` which explicitly zero
  material slots in Initialize). The MODULATE pass therefore multiplies
  texel × white-vertex-diffuse, passing the texture through unchanged.
- **Depth / Z:** **TestNoWrite** (NOMENCLATURE §5) — `ZENABLE = true`,
  `ZWRITEENABLE = false` (set by `SetBlendState`,
  `effect_old.cpp:224-225`). `RefreshZBuffer` is a **no-op** (the
  `RestoreZ` is commented out — §6.6), so no scene Z patch under the
  cylinder footprint.
- **Cull:** **Engine default** (whatever `D3DRENDERSTATE_CULLMODE` was
  set to before this draw). `SetBlendState` does NOT touch `CULLMODE`
  (`SaveBlendState` saves it, `RestoreBlendState` restores it, but the
  setter doesn't override) — so the cylinder uses whatever cull mode
  the engine was last set to. For a one-sided cylinder shell this is
  usually `D3DCULL_CCW` (engine default in most Revenant render
  contexts) which culls inside-facing tris and shows only the outer
  cylinder skin. If the cylinder is double-sided (inner + outer
  authored), then the cull happens correctly. The snapshot author did
  not override, so reconstruction should leave cull at engine default.
- **Orientation:** **WorldUpAligned** (NOMENCLATURE §2 — the proposed
  enum value mentioned in F01 §7). The cylinder's authored local +Z
  axis maps to world +Z via the engine's standard parent-transform
  concatenation; the per-frame matrix only does `Scale + RotZ`, no
  ground-tip, no camera-facing math. The cylinder stands upright in
  world space and yaws about world-Z. As `cylvscl → 0` it flattens to
  a disc on the ground plane.
- **Per-quad / per-object transform:** explicit `OBJ3D_MATRIX = 0x100`
  (and **no** `OBJ3D_VERTS` — important: the engine reads authored
  verts from the asset, the effect does not own a per-instance vert
  buffer). The matrix is `Scale(cylhscl, cylhscl, cylvscl) ·
  RotZ(cylth)` with no translation (parent's world pos used by engine
  for placement).
- **Per-vertex color packing:** **NONE written by the effect.** The
  Render body never touches vertex color. Mesh verts come from the
  asset; their per-vertex diffuse is whatever the I3D authored
  (typically `0xFFFFFFFF` = opaque white).

---

## 8. Texture animation

**Mechanism: STATIC TEXTURE.** The asset has `numtex = 1` (one texture
slot) with `numframes = 1` (one frame per slot — no `framehtexs[]`
array). The Render body never calls `SetTextureFrame()`, never writes
`obj->textureframe`, never writes `lverts[*].tu/tv`. **The texture
does not animate.**

The visual motion comes entirely from the **geometry transform** (the
RotZ yaw spinning the cylinder, the XY scale expanding it, the Z
scale flattening it). Any apparent "fire flicker" in-game would come
from the parallax of the spinning cylinder + the flame-tongue
silhouettes in the texture passing through the viewer's perspective —
not from temporal texture change.

**No per-instance phase, no rate, no wrap, no flipbook, no atlas.**

---

## 9. Associated light

**N/A — none emitted.** The animator makes no `AddPointLight` /
`AddLight` / `SetLight` calls. Searched `effect_old.cpp:10449-10562`
(the entire FireSwarm class span) — zero references to any light
system code.

The effect produces a **visible glow on screen** (the flame texture is
bright), but it does **not** contribute a light source to the scene —
nearby objects do not receive any dynamic illumination from the
FireSwarm. For comparison: missile-family effects like FireBall do add
point lights in their Render bodies; ambient/decorative fire effects
like F01 Flame and F05 FireSwarm do not.

---

## 10. Color

- **Source:** **the authored `Magic\FireSwarm.i3d` ARGB4444 texture**
  (§4). Texture pixel data confirmed by hex decode of payload bytes at
  file offsets `0x1ee8..0x5ee8`. The texture's bottom ¾ (the cylinder's
  V axis) holds the flame-tongue strip; the top ¼ holds the unused
  `box01` red glow disc. The effect supplies **no per-vertex tint**
  (Render never writes vertex color), **no spell color** (there is no
  live spell caller, §12), **no material override**.
- **Exact values** (ARGB4444 → 8-bit RGB; top 10 most-common decoded
  by histogram):
  | ARGB4444 | (A, R, G, B) 8-bit | role |
  |----------|---------------------|------|
  | `0xffff` | (255, 255, 255, 255) | white flame core (peak brightness) |
  | `0xfffe` | (255, 255, 255, 238) | pale yellow tip |
  | `0xffed` | (255, 255, 238, 221) | pale yellow |
  | `0xefec` | (238, 255, 238, 204) | pale yellow (slight alpha drop) |
  | `0x9fc9` | (153, 255, 204, 153) | mid orange |
  | `0xafb8` | (170, 255, 187, 136) | orange |
  | `0x8fa6` | (136, 255, 170, 102) | burnt orange |
  | `0x7fa5` | (119, 255, 170, 85)  | burnt orange |
  | `0x6f83` | (102, 255, 136, 51)  | red-orange (flame body) |
  | `0x4f60` | (68, 255, 102, 0)    | red core |
  | `0x3f60` | (51, 255, 102, 0)    | red core (low alpha) |
  | `0x0f00` | (0, 255, 0, 0)       | **fully-transparent red** (= background) |
- **Expected visual:** **a swirling cylinder of bright yellow-white-tipped
  flame tongues drooping downward, with a deep red core glow** — full
  alpha at the flame centers, smooth alpha falloff at the edges. As the
  cylinder expands and flattens it becomes a **ring of flame collapsing
  to a flat horizontal disc on the ground**. Saturated warm hot color,
  NOT pale.
- **Color identity per `feedback-vfx-color-health-signal`:** if the
  reconstruction renders the FireSwarm as a dull/pale/translucent
  cylinder rather than as bright vibrant yellow-orange-red flame, the
  port is broken. Likely failure modes for this asset specifically:
  1. **Texture format detected as RGB565 instead of ARGB4444** — the
     surface descriptor at `0x1ec0` has masks `0xF000/0x0F00/0x00F0/0x000F`
     which the loader must recognize as ARGB4444. If it falls through
     to RGB565 (with masks `0xF800/0x07E0/0x001F`), the color channels
     will read wrong (R/G/B all shifted, alpha lost).
  2. **Chroma-key code path applied** — F01-style green-key code
     should NOT fire here (the asset has alpha, not green key).
     `legacy/Imagery/Magic/fireswarm.i3d`'s 0-alpha pixels are
     `(α=0, r=255, g=0, b=0)`; if the loader chroma-keys pure red
     instead of trusting the alpha channel, the entire texture goes
     transparent.
  3. **Wrong blend** — if the asset's per-pixel alpha is ignored and
     the blend is set to opaque, the cylinder will look like a solid
     red-yellow texture with hard edges instead of soft-edged flame
     tongues.
  4. **Texture not loaded at all** — the renderer falls back to
     default magenta/white, instead of fire colors.

---

## 11. Audio coupling

**No audio coupling found.** `TFireSwarmAnimator` and `TFireSwarmEffect`
make no sound calls. There is no `PLAY(...)` / `PlayWave(...)` /
`SoundPlayer.Play(...)` / `SoundPlayer.Mount(...)` in the snapshot's
FireSwarm bodies (`effect_old.cpp:10449-10562`).

The asset `fireswarm.i3d` is a STILL (single-state, single-frame)
imagery — it has no per-frame `S3DTag` sound triggers either.

Compare F11 FlameDisc which mounts and plays `LIGHTNING_SOUND` on
FLY-entry; FireSwarm has **no equivalent** — the effect is silent.

---

## 12. In-game callers (spell.def / rules.def / ATTACHEFFECT — vestigial)

**No live callers in shipped retail data.** Confirmed by grep across:
- `data/Resources/spell.def` (0 hits)
- `data/Resources/rules.def` (0 hits)
- `data/Resources/effects.def` (0 hits)
- `data/Resources/master.s` (0 hits)
- `data/Imagery/char.def` (0 hits — no `ATTACHEFFECT "FireSwarm"`)
- `data/Modules/Ahkuilon_unzipped/*.def` and `*.s` (0 hits — no
  module places it)
- `data/Modules/*` recursive grep (0 hits, ignoring binary `.dat`
  files which would only contain world placements — see below)

**Status: vestigial in shipped.** The class registers in the retail
binary (the 2 XREFs and paired factory entries confirm this), the
asset ships byte-identical, but no spell selects it and no module
script invokes it. The class is **unreachable at runtime** in the
shipped game.

Caveat — **binary world-placement check incomplete:** the per-module
`Map/*.dat` files (Ahkuilon, Demo, Deathmatch1..6) are binary sector
files that could in principle contain a world-placed
`OBJCLASS_EFFECT` instance with name `"FireSwarm"`. The grep above
hits only text files; a definitive check would require parsing the
sector binaries for an embedded `"FireSwarm"` string. **The
likelihood of any module placing a one-shot 76-tick spell effect as
ambient world geometry is essentially zero** (one-shot spells don't
get placed as decoration — they fire then die). So the vestigial
verdict stands.

Legacy clues to original intent (pre-release WIP):
- The name `"FireSwarm"` is in the spell-style namespace alongside
  `"FireFlash"`, `"FireWind"`, `"FireBall"`, `"FlameDisc"` — clearly
  authored as a fire spell visual.
- The 76-tick one-shot lifetime + KillThisEffect-on-finish pattern
  matches every other fire spell animator (FireFlash etc.).
- The cylinder-flattening-to-disc envelope suggests an
  **area-of-effect "burst around the caster"** spell — the cylinder
  starts thin and tall above the caster, then expands radially and
  flattens to a flat fire-disc on the ground, like a Photon-style
  AoE explosion.
- The absence of a spell.def variant + the absence of damage code
  (`BurnCharactersInRange` etc.) in the Animate body suggest the
  effect was **pre-release WIP** — the visual was authored before
  the gameplay (the spell.def link) was wired. Same status as F11
  FlameDisc.

---

## 13. Open questions / carve-outs

1. **Render body at retail `0x4f0110` not extracted.** The recon pass
   identified the factory-pointer entry but did not isolate the
   actual `TFireSwarmAnimator::Render` body. A future Ghidra pass
   should walk back from `0x4f0110` (the animator-builder
   trampoline) to find the Render method on the resulting
   class-instance vftable and confirm the snapshot's blend +
   matrix-build sequence. **Mapping the recon coverage to the
   snapshot Render is the single biggest remaining gap.**
2. **`cls_0x5abe4c` is the wrong class.** 392 B / 26 vftable
   entries is structurally inconsistent with the snapshot's tiny
   `TFireSwarmEffect`. Likeliest explanation: it's a larger fire
   class (perhaps a parametrized fire-particle-system owner) that
   consumes the `"FireSwarm"` string in its imagery registrar (cf.
   F11's note that FireColumn references `"FLAMEDISC"`). A future
   pass should isolate the real TFireSwarmEffect class via the
   `0x4f00d0` ctor trampoline. The candidate yaml
   (`TFireSwarmEffect_cls_0x5abe4c_candidate.yaml`) should be
   **demoted** based on this mis-sizing.
3. **`FIRESWARM_CYLVSCLINIT = 30` aspect ratio is extreme.** The
   initial cylinder is `0.4 wu radius × 30 wu height` = 75× aspect
   ratio. Whether this matches artist intent (a tall thin column
   that flattens to a wide disc — the "burst") or is a vestigial
   pre-release placeholder (e.g. should have been 5 or 10) cannot
   be verified without retail capture. The fact that the constant
   is a clean `30` (not `15` or `7.5` or similar) suggests it was
   intentional, but it's worth visually self-vetting in
   reconstruction — if the resulting column looks too thin/tall to
   read as fire, lower it to ~5-10 and re-check.
4. **`OBJ3D_VERTS` not set — engine may need to lock the mesh.** The
   Render body sets `OBJ3D_MATRIX` only (`:10532`). This means the
   effect does not own per-instance vert data — but it does want a
   per-instance matrix. In the engine's submission pipeline, this
   should still draw correctly (the matrix is the per-instance
   transform; the mesh verts are shared/read-only). If the
   reconstruction renderer requires `OBJ3D_VERTS` for any per-instance
   transform to take effect, that's a port-side concern, not a
   forensics issue.
5. **The `box01` sub-object is in the asset but never drawn.** The
   small red glow disc at the top half of the texture is authored as
   `GetObject(0) = "box01"`, but `TFireSwarmAnimator::Render` calls
   `GetObject(1)` (tube01). Two possibilities: (a) the dev abandoned
   the `box01` sub-object during pre-release iteration and left it in
   the asset, or (b) a never-shipped variant of FireSwarm would draw
   both objects (e.g. a "core glow + cylinder tube" composite). The
   reconstruction should preserve the asset as-is (do NOT delete
   `box01` from the I3D) but only render `tube01`.
6. **No retail caller means no in-game ground truth.** Unlike F01
   Flame (every dungeon torch) or B01 Blood (every hit reaction),
   FireSwarm has no live placement. The reconstruction has to
   visually self-vet against the asset's standalone rendering, not
   against an in-game capture. This makes the blend question (§7)
   particularly hard to settle — there is no "what does it look
   like in retail" answer.
7. **Cylinder cull-mode ambiguity.** `SetBlendState` does not set
   `CULLMODE`. If the cylinder is single-sided (only outer faces
   authored), the engine's default `D3DCULL_CCW` is correct. If the
   cylinder is double-sided (inner + outer authored), the cull
   should be `NONE`. Quicksand's Render
   (`effect_old.cpp:9337-9339`) explicitly sets `CULLMODE = NONE`
   for its sand-overlay quads — FireSwarm does NOT. This implies
   the FireSwarm tube is meant to render single-sided. Should be
   visually self-checked on reconstruction; if the cylinder shows a
   "hole in the back" when viewed from one side, set CULLMODE to
   NONE.
8. **`SetCommandDone(false)` semantics during the kill tick.** On
   `frameon = 76`, the animator calls `KillThisEffect()` (which sets
   `OF_KILL | OF_PULSE`) and the next Pulse will remove the effect.
   The `SetCommandDone(false)` at `:10502` runs every tick including
   the kill tick — this should be fine since the kill flag takes
   precedence, but worth a glance during reconstruction to confirm
   the engine reaps the effect on the next tick.
9. **24 Hz baseline assumption.** All time conversions in §3 assume
   the original game ran the animator's Animate at 24 Hz nominal.
   The snapshot doesn't gate on sim-tick (no `Pulse()` check inside
   Animate), so the effect was framerate-dependent in the original.
   Per `feedback-framerate-independent-anim`, the reconstruction
   should convert per-tick→per-second (rates given in §3) and
   integrate by `dt`.

---

## 14. Reconstruction burndown

For the reconstruction agent — what they need to do, in order:

1. **Load the asset.** Use the byte-identical `Magic\FireSwarm.i3d`
   from either `legacy/Imagery/Magic/fireswarm.i3d` or
   `data/imagery.rvi:Imagery/Magic/fireswarm.i3d` (same MD5,
   byte-identical). Confirm the loader recognizes the **ARGB4444**
   surface format (masks `0xF000/0x0F00/0x00F0/0x000F` at I3D offset
   `0x1ec0`); if the loader only handles RGB565, add ARGB4444
   support. Do NOT apply chroma-key conversion — this asset uses
   real per-pixel alpha.
2. **Bind sub-object 1 (`tube01`).** Skip `GetObject(0) = "box01"`
   (the unused glow disc). Render only `tube01`.
3. **Implement the 3-float per-instance state.** `cylhscl`,
   `cylvscl`, `cylth`, plus the `frameon` lifetime tick. Initialize
   per §6.3: `frameon=0, cylth=0, cylhscl=0.4, cylvscl=30.0`.
4. **Per-tick update (time-based — rates from §3):**
   - `cylhscl += 9.6 * dt` (== `+0.4` per 24 Hz tick)
   - `cylvscl -= 9.6 * dt` (== `-0.4` per 24 Hz tick)
   - `cylth += 12.0 * dt; if (cylth > 2π) cylth -= 2π;`
   - `frameon += 24.0 * dt` (or just `lifetime_s += dt; if (lifetime_s
     > 75/24 = 3.125) kill effect`).
5. **Build the per-frame transform.** `m = Scale(cylhscl, cylhscl,
   cylvscl) · RotZ(cylth)`, no translate. The mesh sits at the
   effect's world position via the engine's parent-transform
   concatenation.
6. **Submit with Alpha blend.** `MODULATE + SRC_ALPHA/INV_SRC_ALPHA,
   ZWRITE off, ZTEST on`. Use the engine's `EFxBlend::Alpha` or
   equivalent. **Watch for the BLEND SANITY-CHECK risk in §7** —
   if the cylinder visually reads dull/translucent rather than
   luminous, switch to `AdditiveStraight` and re-check.
7. **Cull-mode = engine default** (likely CCW). If the cylinder shows
   a hole in the back, switch to `NONE` per gap §13.7.
8. **No Z patch.** The original `RefreshZBuffer` is commented out;
   no scene Z disturbance.
9. **No texture animation.** Static texture. No flipbook, no UV
   mutation, no atlas pick.
10. **No light, no audio.** Skip those branches entirely.
11. **Kill at lifetime > 3.125 s** (or `frameon > 75` if using tick
    counter). Use the engine's kill-effect equivalent
    (`KillThisEffect` / mark-for-deletion). Parent spell signaling
    is a no-op since no live spell calls this.
12. **Visual self-vet.** Spawn one FireSwarm in the VFX test harness
    (`--test=vfx` F05 row, if added). Watch the full 3.1 s cycle:
    starts as tall thin column at the spawn point, expands and
    flattens to a wide ring, ends as a flat horizontal flame disc
    on the ground plane. The flame texture should be vibrantly
    yellow-orange with smooth alpha edges (NOT chroma-keyed, NOT
    dull). The cylinder should yaw continuously (~6 rotations over
    its life). If any of those visual cues fail, debug per the §10
    failure modes.

The reconstruction is small (a single transform-animated mesh + a
3-float state machine + a 76-tick lifetime). Total port complexity is
**low** — comparable to F01 Flame (without the per-frame UV math).
The main risk is the asset format (ARGB4444 must load correctly) and
the blend choice (snapshot Alpha vs. fire-family Additive — see §7).
