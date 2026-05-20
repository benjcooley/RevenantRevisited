# SPARKS TSparkAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | (none yet — needs an INVENTORY row; suggest **X22**, see §1) |
| **Class(es)** | `TParticle3DAnimator` (the actual animator). `TSparkAnimator` is **not a class** — it is only the *builder symbol name* the registration macro mints (see §2, §7). Registered name = `"sparks"`. No paired `TEffect` subclass; the spawned object is a generic `TEffect`. |
| **Status** | forensics-complete (see §13 for the few genuine unknowns) |
| **Retail fidelity** | **retail-confirmed (3 divergences) + 1 author-correction.** Retail `TCharacter::EffectBurst` decompiles cleanly and corroborates asset, registration, structure, most constants; but **gravity, trails, bounce were re-tuned for ship** (snapshot 0.2/1/false → retail 0.25/2/true — verified IEEE bit patterns at cls_0x5a7b98.cpp:4640-4664). **Blend = additive** per the original developer; the snapshot's Alpha is snapshot-only (retail `TParticle3DAnimator::Render` body not decompiled). Use the RETAIL param values + additive. See §2.1 + §7. |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 (retail reconciliation: vfx-forensics-agent / 2026-05-20) |
| **Family** | blood/combat (combat-feedback; mechanically a generic spark burst) |
| **Draws** | particle emitter — N screen-aligned billboards, each one sub-object of `Misc\Sparks.I3D` placed at a particle position |
| **Archetype(s)** | (E) particle emitter — **simple emitter with gravity** (spawn, ballistic arc, fixed lifetime, die). No texture animation, no associated light, no state machine. The single-trigger **burst** sub-flavor. |

---

## 1. Summary

`"sparks"` is the generic spark-burst particle effect: a one-shot puff of small
bright billboards that fly out in a cone, arc under gravity, and wink out after a
short fixed lifetime. In the shipped game it fires when a melee attack is
**blocked** by a target's shield/parry (the impact sparks), spawned by
`TCharacter::EffectBurst("sparks")` (`src/character.cpp:1808`). It is a *reusable
building block*: the animator class, `TParticle3DAnimator`, is a fully generic
particle system parameterized entirely by an `SParticleParams` struct the caller
fills in — the commented-out sibling registrations (`icedsparks`, `sand`,
`blood`, `snow`, `src/effect_old.cpp:4689-4692`) confirm it was designed to drive
many particle effects, not just sparks. There are **no spark-specific constants
inside the animator**; every numeric (count, spread, gravity, lifetime, …) comes
from the caller's params (§3, §5). The sparks read as bright white/yellow
metallic sparks — a single emitter, single color per burst (one 2×2-atlas cell,
chosen per character), drawn **Additive** (video-confirmed; the code's Alpha was
snapshot-only WIP — §7), self-lit. The simplest effect in the game.

> **INVENTORY action:** no row exists for this effect (INVENTORY has the generic
> infra row `I03 TParticle3DAnimator`, but no concrete "sparks" effect row). Add
> one to the *Misc / other* section. Suggested: `X22 | "sparks" (impact-block
> spark burst) | PE | effect_old.cpp:4688 | not-extracted | src/effect.h:1010
> (TParticle3DAnimator) | forensics-complete | - | forensics doc at
> docs/vfx/forensics/SPARKS_TSparkAnimator.md`.

---

## 2. Sources & evidence

- **Retail decomp:** **NONE.** No `recon/classes/` body and no `recon/mappings/`
  entry exists for `TParticle3DAnimator`, `TSparkAnimator`, or the `"sparks"`
  builder (confirmed by grep — see §13.1). The `"sparks"` *string* is XREF'd from
  the unrelated `TSymGlowAnimator` candidate
  (`recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml:9-11`), which is a
  different effect that also emits spark particles — not this animator.
- **Pre-release (authoritative):** `src/effect_old.cpp`
  - Registration: `REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator, TParticle3DAnimator)` `src/effect_old.cpp:4688`.
  - `TParticle3DAnimator` bodies: `Initialize` `:4694-4705`, `Close` `:4707-4723`,
    `ResetTargetInfo` `:4731-4749`, `InitParticles` `:4752-4788`, `Animate`
    `:4790-4942`, `Render` `:4944-4986`, `RefreshZBuffer` `:4989-5002`.
  - Class + params declarations: `SParticleParams` `src/effect.h:987-1008`;
    `class TParticle3DAnimator` `src/effect.h:1010-1043`.
  - Registration macro: `REGISTER_MULTI_3DANIMATOR_` `src/3dimage.h:352-360`.
- **Caller (authoritative for the spark params):** `TCharacter::EffectBurst`
  `src/character.cpp:2225-2318` (the `else` branch `:2262-2317` fills the
  `SParticleParams`); the trigger `EffectBurst("sparks")`
  `src/character.cpp:1808`.
- **Asset:** `Misc\Sparks.I3D` registered `legacy/Class.Def:2035`
  (`"Sparks" "Misc\Sparks.I3D" 0x14db0f2e`); file at
  `legacy/Imagery/Misc/Sparks.i3d`.
- **Existing port shell:** none. `src/effect.h:1010-1043` declares
  `TParticle3DAnimator`; `src/effect.cpp:26` mentions the registration in a
  comment only. Nothing is wired in the modern port / `--test=vfx`.
- **Sister effects consulted:**
  - `TRibbonAnimator` (`src/effect_old.cpp:4084-4243`) — to **rule out** the
    "ribbon sparks" code (§13.2 / disambiguation below). That code is NOT this
    effect.
  - `TPhotonAnimator` (`src/missileeffect.h:84-93`) — same photon sprite family;
    corroborates the bright-spark color reading (§10).
- **Source-of-truth ranking:** pre-release `effect_old.cpp` is authoritative for
  all behavior; the **caller** `character.cpp` is authoritative for the actual
  spark parameter values (the animator has none of its own). No retail to diverge
  from.

### Disambiguation — the `:4117-4260` "ribbon sparks" are NOT this effect

The task flagged "ribbon sparks" code around `effect_old.cpp:4117-4260` (comments
"Initialize the starting positions of the ribbon sparks" `:4117`, "Accelerate the
sparks movements upward" `:4225`). **That code belongs to `TRibbonAnimator`**
(`src/effect_old.cpp:4084` Initialize, `:4143` Animate, `:4244` Render), the
"Ribbon" magic effect (`Magic\Ribbon.I3D`, INVENTORY X11), whose fields are
`NUM_RIBBON_SPARKS = 30` et al. at `src/effect.h:865-883`. Its upward
acceleration (`:4225`) is the ribbon-spiral behavior and has **nothing to do**
with `TSparkAnimator`. The generic spark burst is `TParticle3DAnimator`
(`:4688`, `:4694-5002`) and applies **downward** gravity (`v.z -= gravity`,
`:4850`), not upward acceleration. Do not conflate them.

### 2.1 Retail-vs-snapshot reconciliation

`src/character.cpp` / `src/effect_old.cpp` are a **pre-release development
snapshot**, not the shipped game. Because the spark constants live in the
**caller** (`TCharacter::EffectBurst`, not the generic animator), the
reconciliation is primarily against the **retail `EffectBurst` decomp** — which,
unlike the animator, **does** decompile cleanly. (The §2 statement "Retail decomp:
NONE" was correct for the *animator* `TParticle3DAnimator`; it is **wrong for the
caller** — see below.) Verdict: **retail-confirmed, with three documented
divergences.** Use the retail values for those three.

**Retail source:** `TCharacter::EffectBurst` = `meth_0x4c85d0` @ `0x4c85d0`
(800 bytes). Full decompiled body: `recon/classes/cls_0x5a7b98.cpp:4535-4665`; the
`"sparks"` branch is `:4614-4660`, where the `SParticleParams` is built on the
stack at `&uStack_b0` and passed to the animator via `(**(code**)(*piVar5 +
0x60))(&uStack_b0)` (`:4659`) — vtable slot `+0x60` = `InitParticles`. The struct
field→stack-offset mapping is unambiguous (`SParticleParams` layout from
`effect.h:982-1002`; struct base `&uStack_b0`, fields 4-byte contiguous).

**(1) Constant grep — caller immediates, retail vs snapshot.** Every immediate in
the retail "sparks" branch was decoded and mapped to its `SParticleParams` field:

| field | snapshot (`character.cpp`) | retail immediate (`cls_0x5a7b98.cpp`) | match |
|-------|----------------------------|---------------------------------------|-------|
| particles | `random(15,25)` (:2290) | `RandomRange(0xf,0x19)` (:4635) | ✅ same |
| pos.z bias | `+45` (:2288) | `iStack_100 + 0x2d` = +45 (:4634) | ✅ same |
| facing jitter | `random(-80,80)` (:2274) | `RandomRange(0xffffffb0,0x50)` = (-80,80) (:4624) | ✅ same |
| pspread x/y/z | `3.0` (:2294-96) | `0x40400000` = 3.0 ×3 (:4637-40) | ✅ same |
| dir scale | `/100` (:2297-99) | `* _DAT_005a350c` = ×0.01 (:4652-55) | ✅ same |
| spread x/y/z | `0.5` (:2300-02) | `0x3f000000` = 0.5 ×3 (:4641-44) | ✅ same |
| **gravity** | **`0.2`** (:2303) | **`0x3e800000` = 0.25** (:4645, struct+0x34) | ❌ **DIVERGE** |
| **trails** | **`1`** (:2304) | **`2`** (:4646, struct+0x38) | ❌ **DIVERGE** |
| minstart / maxstart | `0` / `8` (:2305-06) | `0` / `8` (:4647-48) | ✅ same |
| minlife / maxlife | `20` / `40` (:2307-08) | `0x14` / `0x28` = 20/40 (:4649-50) | ✅ same |
| **bounce** | **`false`** (:2309) | **`1` = true** (:4651, struct+0x4c) | ❌ **DIVERGE** |
| killobj | `true` (:2310) | `1` (:4653, struct+0x50) | ✅ same |
| objflags | `1 << (ObjId()&3)` (:2311) | `1 << (… & 3)` (:4658, struct+0x54) | ✅ same |
| numtargets | `0` (:2313) | `0` (:4657, struct+0x60) | ✅ same |

So **11 of 14 spark params are byte-identical** between snapshot and retail;
**three were re-tuned before ship**: `gravity 0.2 → 0.25`, `trails 1 → 2`,
`bounce false → true`. These are exactly the kind of late tuning the protocol
warns about. (`_DAT_005a350c` = bytes `0a d7 23 3c` = `0x3c23d70a` = 0.01 =
`1/100`, `recon/classes/_data.txt:3898-3901` — the retail `×0.01` is the
snapshot's `/100`.)

**Effect of the divergences:** sparks fall slightly faster (gravity +25%); each
particle draws as a short **2-step motion streak** rather than a single billboard
(`trails=2` → the Render inner loop runs twice, stepping a ghost copy forward
along velocity, §7); and sparks **bounce** off geometry instead of passing
through (`bounce=true` enables the bounce block at `effect_old.cpp:4852-4858`,
which §6/§13.3 documents as "skipped for sparks" *per the snapshot* — in retail it
runs). The reconstruction must use the retail values.

**(2) Asset identity — IDENTICAL.** The shipped `Imagery/Misc/sparks.i3d` member
of `data/imagery.rvi` (a PK/ZIP archive) is **byte-for-byte identical** to the
snapshot `legacy/Imagery/Misc/Sparks.i3d`: both 16,300 B, both md5
`f285e02bb803bc53c45dbe9ce70d1956` (`cmp` reports identical). The 4-photon-sprite
visual identity (§4, §10) is unchanged between snapshot and retail — the asset side
of the effect did not move.

**(3) Structure / registration table — corroborated.** The retail binary holds a
contiguous animator-builder function-pointer table (`recon/classes/_data.txt`
around `005c5300+`, entries `addr LAB_004e____`); the entry at `005c5350 →
004e53a0` is the builder whose function references the `"sparks"` string at
`005e1104` (`_data.txt:107439-107440`). This confirms the retail
`REGISTER_MULTI_3DANIMATOR_("sparks", …)` registration mechanism is present and
slots into the same factory table. (The auto-fingerprinter heuristically maps
`SParticleParams ↔ cls_0x5a393c` `recon/analysis/full_output.txt:1240`, but that
cls also matches `TLightningAnimator` and carries "Armor" strings — treat the
struct-layout match as **unconfirmed**, not corroborating.)

**(4) Registration + naming + trigger — corroborated.** The `"sparks"` string is
XREF'd from three retail sites (`recon/classes/_data.txt:104150,104238,107439`):
the `EffectBurst` dispatch (`meth_0x4c85d0:0x4c86ce`), the attack-result handler
(`TPlayer virt_meth_0x4c6dd0:0x4c71aa` — the block-resolution caller that passes
`"sparks"` to `EffectBurst`, matching the snapshot's `character.cpp:1805-1810`
blocked-miss path), and the animator builder (`0x4e53a0`). The retail dispatch
also keeps the burning early-out (`if (mbr_0x1b8 && stricmp(type,"blood")==0)
return;`, `cls_0x5a7b98.cpp:4586`) matching the snapshot `EffectBurst` head. The
`CA_SPARKS`-gated, `ACTION_BLOCK`-conditioned trigger is therefore confirmed in
retail (the flag-test itself lives in the block-resolution caller, consistent with
the snapshot).

**Bottom line:** the effect is **retail-confirmed** — asset identical,
registration + trigger wiring identical, 11/14 caller constants identical — but the
snapshot is **not** authoritative for `gravity`, `trails`, and `bounce`, which the
retail decomp shows were changed for ship. The §3 table and §13 are updated
accordingly.

---

## 3. Constants

`TParticle3DAnimator` has **no hardcoded spark constants** — every numeric is a
field of the `SParticleParams` the caller passes to `InitParticles`. The table
below gives the values for the **shipped `"sparks"` use** (the block-spark burst),
all from `TCharacter::EffectBurst` (`src/character.cpp`), plus the engine-side
constants in the animator.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| particle count | `random(15, 25)` (inclusive) | count | character.cpp:2290 | **yes (retail)** — `RandomRange(0xf,0x19)` cls_0x5a7b98.cpp:4635 |
| spawn origin (pos) | `vect0` = vector(caster→target, dist/2) with `z += 45` | wu, caster-local | character.cpp:2283-2293 | **yes (retail)** — `+0x2d`/dist÷2 cls_0x5a7b98.cpp:4633-34 |
| pspread (position jitter) | `(3, 3, 3)` × `random(-100,100)/100` ⇒ ±3 wu each axis | wu | character.cpp:2294-2296 | **yes (retail)** — `0x40400000`=3.0 cls_0x5a7b98.cpp:4637-40 |
| dir (mean velocity) | `vect/100` where `vect = vector(face±random(-80,80), 100)` ⇒ unit-ish XY toward a randomized facing, z≈0 | wu/tick | character.cpp:2274-2277, 2297-2299 | **yes (retail)** — `×0.01` (_DAT_005a350c) cls_0x5a7b98.cpp:4652-55 |
| spread (velocity jitter) | `(0.5, 0.5, 0.5)` × `random(-100,100)/100` ⇒ ±0.5 wu/tick each axis | wu/tick | character.cpp:2300-2302 | **yes (retail)** — `0x3f000000`=0.5 cls_0x5a7b98.cpp:4641-44 |
| gravity ⚠️ | snapshot `0.2` → **retail `0.25`** | wu/tick² (added to `v.z` each tick) | character.cpp:2303 / cls_0x5a7b98.cpp:4645 | **DIVERGES — use 0.25 (retail)** `0x3e800000` |
| trails ⚠️ | snapshot `1` → **retail `2`** | count (render-only sub-steps per particle, §7) | character.cpp:2304 / cls_0x5a7b98.cpp:4646 | **DIVERGES — use 2 (retail)** (motion-streak, §7) |
| minstart / maxstart | `0` / `8` | tick start-delay range, `random()` per particle | character.cpp:2305-2306 | **yes (retail)** — `0`/`8` cls_0x5a7b98.cpp:4647-48 |
| minlife / maxlife | `20` / `40` | tick lifetime range, `random()` per particle | character.cpp:2307-2308 | **yes (retail)** — `0x14`/`0x28` cls_0x5a7b98.cpp:4649-50 |
| bounce ⚠️ | snapshot `false` → **retail `true`** | bool | character.cpp:2309 / cls_0x5a7b98.cpp:4651 | **DIVERGES — use true (retail)** `1` (enables §6.2 bounce block) |
| killobj | `true` | bool — object self-destructs when all particles done | character.cpp:2310 | **yes (retail)** — `1` cls_0x5a7b98.cpp:4653 |
| objflags | `1 << (ObjId() & 0x3)` | bitmask — selects exactly ONE of the 4 sub-objects (§4) | character.cpp:2311 | **yes (retail)** — `1 << (… & 3)` cls_0x5a7b98.cpp:4658 |
| seektargets / numtargets | `false` / `0` | no homing | character.cpp:2312-2313 | **yes (retail)** — `0`/`0` cls_0x5a7b98.cpp:4656-57 |
| EffectBurst `height` arg | `50` (default, unused for "sparks") | wu | character.h:311 | yes (unused — see §5; retail "sparks" branch likewise ignores the height arg) |
| spawn-facing jitter | `random(-80, 80)` byte-angle | added to caster face | character.cpp:2274 | **yes (retail)** — `RandomRange(0xffffffb0,0x50)` cls_0x5a7b98.cpp:4624 |
| **sample-default count** | `10` | count — fallback only if `params.particles==0` (§6.1) | effect_old.cpp:4797 | snapshot-only (dev fallback in the animator; not exercised — caller always sets count) |
| RefreshZBuffer patch | `size_x=75, size_y=50`, offset `+25 / −50` px | screen px | effect_old.cpp:4996-5000 | snapshot-only (animator-internal; no retail `TParticle3DAnimator` body to corroborate, §13.1) |

`random(min,max)` is **inclusive** on both ends (`rand() % (max-min+1) + min`,
`src/revutils.cpp:1609-1610`).

> **Reconstruction note:** because the animator is generic, the only way to get
> "sparks" right is to feed it *these* params. They are not in the animator — they
> are in `character.cpp:2290-2313`. Treat that block as the spark constant table.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Sparks | `legacy/Imagery/Misc/Sparks.i3d` | 16,300 B | the spark sprite imagery — STILL (2D billboard) I3D with **4 sub-objects** | registered `Class.Def:2035` under name `"Sparks"` (ID `0x14db0f2e`); loaded by the OBJCLASS_EFFECT registry when the effect spawns; bound by `T3DAnimator` |

**Sub-objects (4)** — read directly from the I3D object table at file offset
`0x7c4` (`04 00 00 00` count, then four 28-byte name records):

| # | name | role |
|---|------|------|
| 0 | `photon`   | spark billboard quad (variant 0) |
| 1 | `photon01` | spark billboard quad (variant 1) |
| 2 | `photon02` | spark billboard quad (variant 2) |
| 3 | `photon03` | spark billboard quad (variant 3) |

All four are billboard quads over **one texture that is a 2×2 atlas** — four
spark-sprite cells in a single image, NOT four separate sprites and NOT one
sprite shared. Each photon sub-object is **UV-mapped to a different cell** of the
2×2 grid (read the sub-object UVs off the I3D geometry — they are sub-rects, not
0..1). So selecting one sub-object (via `objflags`, §6) selects one cell = one
spark variant. **A reconstruction that maps the whole texture (0..1 UV) onto each
quad draws all four cells per particle — the "2×2 multicolor grid" failure.
Render the chosen sub-object's actual UV sub-rect (one cell).**

Imagery header (file offset `0x14` onward): type tag `STILL` (`0x1C`), texture
**64 × 80** px (`0x44`: `40 00` = 64 wide, `50 00` = 80 high) holding the 2×2
atlas, 16-bit (RGB565) — the cells are small bright spark/sparkles,
**white/yellow-white** (texture region `0x174`–`0x3FAC`). No multi-frame
`framehtexs` array and no UV-scroll — the only "atlas" is the static 2×2 cell
selection via per-sub-object UVs (§8). (Same photon-sprite family as
`TPhotonAnimator`, `src/missileeffect.h:84`.)

> **NOTE (forensics correction, 2026-05-20):** an earlier draft said the four
> sub-objects "share the same sprite." That missed that the texture is a 2×2
> atlas with each sub-object UV-mapped to its own cell — the detail that decides
> single-cell vs whole-grid rendering. Exact per-cell UV rects to be confirmed
> from the I3D geometry during reconstruction; the 2×2 layout is confirmed.

**The effect loads a real asset — do NOT substitute a procedural spark sprite.**
The authored 4-variant photon sprite is the visual identity (§10).

---

## 5. Spawn & emit

- **Trigger semantics:** **one-shot burst.** All particles are seeded at once in
  `InitParticles` (`effect_old.cpp:4774-4787`); no continuous emission. The object
  lives only until every particle's life elapses, then self-destructs
  (`killobj=true`, §6).
- **Count per trigger:** `random(15, 25)` particles (`character.cpp:2290`).
- **Initial direction / distribution:** a **cone**, biased along a randomized
  horizontal facing. The caller computes `vect = ConvertToVector(face +
  random(-80,80), 100)` (`character.cpp:2274-2277`) — i.e. a horizontal unit-ish
  vector pointing roughly back along the attacker's facing, jittered ±80
  byte-angle (≈ ±112°, a wide fan). Mean per-particle velocity is `dir = vect/100`
  (so XY magnitude ≈ 1 wu/tick, z ≈ 0). Each particle then adds velocity jitter
  `spread = ±0.5` wu/tick on each of x/y/z (`effect_old.cpp:4779-4781`,
  `character.cpp:2300-2302`). Net: a fan of sparks spraying horizontally outward
  with a little vertical scatter, then falling under gravity.
- **Emit anchor convention:** the effect *object* is placed at the **caster's
  world position** (`def.pos = pos`, `character.cpp:2264`). Particles are seeded
  **relative to that object origin** by `pos = params.pos + jitter`
  (`effect_old.cpp:4776-4778`), where `params.pos = vect0` is a caster-local
  offset = `ConvertToVector(caster→target facing, distance/2)` with `z += 45`
  (`character.cpp:2283-2293`). So the burst originates **roughly halfway between
  the caster and the blocked target, at ~45 wu up** — i.e. about chest/weapon
  height at the point of impact, not at the caster's feet. (`EffectBurst`'s
  `height` arg defaults to 50 but is only used by the `"blood"` branch; for
  `"sparks"` the z offset is the hardcoded `+45` on `vect0`,
  `character.cpp:2288`.)
- **Coordinate space:** particle positions/velocities are integrated in the
  effect's **local space** (relative to the object origin); the object origin is
  the caster's world pos. Render places each sub-object at the particle's local
  `p[c]` via `OBJ3D_POS1` (§7).
- **Spread / jitter:** position ±3 wu/axis (`pspread`), velocity ±0.5 wu/axis
  (`spread`), facing ±80 byte-angle, count 15–25, lifetime 20–40 ticks, start
  delay 0–8 ticks — all uniform `random()`.

### Spatial diagram

```
   wz (up)
   │                  ScreenAligned spark billboards (face camera),
   │        · ·  ·    one of 4 photon variants each
   │      ·  ·  · ·   ← 15..25 sparks, ±3wu pos jitter, fan cone
   │     · ·· ·       velocity ≈1 wu/tick along (face±80°), z≈0,
   │    ·  ·          then gravity 0.2 wu/tick² pulls them down
   └──────────────── wx
  ╱   ▲ emit origin = caster_local + vector(caster→target, dist/2),
 ╱      z = +45 wu  (≈ impact point, mid-way to the blocked target)
wy

cone fan (top-down):  attacker faces →  sparks spray back over ±~112° arc
```

---

## 6. Behavior & per-frame logic

The whole effect is the generic `TParticle3DAnimator::Animate`
(`effect_old.cpp:4790-4942`). For the spark use only the *non-seeking* path runs
(`seektargets=false`), so the large homing block (`:4859-4936`) is **dead code
for sparks** — documented in §13.3, not reconstructed.

### 6.1 Seeding (`InitParticles`, `effect_old.cpp:4752-4787`)

```
InitParticles(params):                       // effect_old.cpp:4752
    copy params; allocate p[],v[],s[],l[],o[] (size = particles)   // :4754-4760
    build objs[] = list of sub-object indices whose bit is set in objflags  // :4764-4770
        if none set, objs = {0}                                    // :4771-4772
    for c in 0..particles-1:                                       // :4774
        p[c] = params.pos    + params.pspread * random(-1.00..1.00)   // :4776-4778  (±jitter per axis)
        v[c] = params.dir    + params.spread  * random(-1.00..1.00)   // :4779-4781
        l[c] = random(minlife, maxlife)        // ticks of life     // :4782
        s[c] = random(minstart, maxstart)      // start delay ticks  // :4783
        o[c] = objs[random(0, numobjs-1)]      // which sub-object   // :4784
```

For sparks, `objflags = 1 << (ObjId() & 3)` selects exactly **one** variant, so
every particle in a given burst draws the **same** photon variant (the per-burst
variety, not per-particle). (If `objflags` had multiple bits, particles would mix
variants.)

### 6.2 Per-tick update (non-seeking path, `effect_old.cpp:4826-4942`)

```
Animate(draw):                                // effect_old.cpp:4790
    T3DAnimator::Animate(draw)                //   base: refresh hierarchy
    if params.particles == 0: seed sample defaults (dev only, §6.1 note)  // :4794-4824

    isdone = true
    for c in 0..particles-1:                  // :4829
        if l[c] <= 0: continue                //   already dead       // :4831
        isdone = false
        if s[c] > 0: s[c]--; continue         //   still in start delay  // :4836-4840
        p[c] += v[c]                          //   integrate position // :4842-4844
        l[c]--                                //   age                 // :4846
        v[c].z -= params.gravity              //   gravity (sparks: 0.2) // :4847-4851
        // bounce path (params.bounce=false for sparks → skipped)     // :4852-4858
        // seek path (params.seektargets=false for sparks → skipped)  // :4859-4936

    inst->SetCommandDone(isdone)              // :4939
    if params.killobj && isdone: inst->SetFlags(OF_KILL)   // self-destruct  // :4940-4941
```

- **Velocity integration:** plain Euler, `p += v` then `v.z -= gravity` each tick.
- **Gravity:** `0.2` wu/tick² downward (sparks fall in a ballistic arc).
- **Lifetime / death:** each particle dies when `l[c]` reaches 0 (after 20–40
  ticks). When *all* are dead, `isdone` stays true and the object kills itself
  (`killobj=true`).
- **No color/alpha/scale curve.** The animator never modifies per-vertex color,
  alpha, or scale over life — particles render at constant size/brightness then
  simply stop being drawn when dead (a hard pop-off, not a fade). The visual
  fade-feel comes only from sparks thinning out as individuals expire at staggered
  times (lifetime + start-delay jitter). (Confirmed: `Render` `:4944-4986` writes
  only `obj->pos`, no color/alpha/scale.)

> **6.1 sample-default note.** If a caller leaves `params.particles == 0`, Animate
> seeds a built-in 10-particle demo set (`effect_old.cpp:4794-4824`: pos
> (0,0,70), pspread 3, dir (1,−1,0.5), spread 0.5, gravity 0.2, life 10–30,
> objflags 0xF). This is a **developer fallback**, not the live spark path
> (`EffectBurst` always sets `particles = random(15,25)`). Do not reconstruct it
> as spark behavior; note it as the no-params default.

### Temporal diagram

```
per particle (no curves — constant draw, then off):
draw? ┤████████████████████          (drawn while s==0 and l>0)
      │        ↑start-delay s (0..8)  ↑death at l ticks (20..40)
      └────────────────────────────── ticks
            staggered start + staggered death ⇒ the BURST as a whole
            ramps in over ~0..8 ticks and thins out over ~20..40 ticks
position z: ↗ small up/flat, then ↘ parabola under gravity 0.2
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** for each live, started particle, one sub-object of
  `Sparks.I3D` (a billboard quad) placed at the particle's position.
  `TParticle3DAnimator::Render` (`effect_old.cpp:4944-4986`):

```
Render():                                     // effect_old.cpp:4944
    SaveBlendState(); SetBlendState();         //   snapshot=Alpha; SHIP=Additive (video-confirmed — §7) // :4946-4947
    ResetExtents()
    for c in 0..particles-1:                   // :4951
        if s[c] > 0 or l[c] <= 0: continue     //   skip delayed/dead       // :4953
        pp = p[c]; vv = v[c]
        obj = GetObject(o[c])                  //   pick the chosen sub-object // :4959
        for d in 0..trails-1:                  //   trails=1 for sparks ⇒ 1 draw // :4961
            obj->flags |= OBJ3D_POS1           //   override position only   // :4963
            obj->pos = pp                      //   place at particle pos    // :4964
            RenderObject(obj)                  // :4965
            pp += vv; vv.z -= gravity          //   (only matters if trails>1) // :4967-4970
    UpdateExtents(); RestoreBlendState()        // :4982-4983
```

- **Blend mode:** **Additive (ONE/ONE)** — **video-confirmed** against retail
  (developer, 2026-05-20). The snapshot `Render` calls `SetBlendState()`
  (`effect_old.cpp:4947`) = `D3DTBLEND_MODULATE` + `SRC_ALPHA/INV_SRC_ALPHA`
  (translucent Alpha), BUT the retail `TParticle3DAnimator::Render` body was never
  decompiled (snapshot-only — §2.1), so the snapshot's Alpha was always
  unverified. Visual comparison to the retail video resolved it: the sparks are
  additive (they glow against the dark scene). The snapshot's Alpha is WIP that
  the shipped game superseded. **Reconstruct Additive (ONE/ONE).** This is a
  resolved snapshot→shipped divergence on the render blend, settled by visual vet
  (the methodology working as intended — the value was flagged snapshot-only, not
  asserted, and the video settled it).
- **Lit vs self-lit:** **Unlit / self-lit.** The animator never folds ambient
  light into vertex color and never zeroes the material — it simply draws the
  imagery's authored verts/texture. The color is literal (from the sprite). No
  scene lighting applied. Classify **Unlit** (NOMENCLATURE §4).
- **Depth / Z:** **TestNoWrite** — `ZENABLE=true`, `ZWRITEENABLE=false` (set by
  `SetBlendState`, `effect_old.cpp:224-225`). `RefreshZBuffer`
  (`effect_old.cpp:4989-5002`) restores scene Z over a fixed 75×50 px patch around
  the projected effect origin so the no-depth-write sparks composite correctly.
- **Orientation:** **ScreenAligned** (camera-facing billboards). Render sets only
  `obj->pos` via `OBJ3D_POS1`; it never sets `rot.x` (no `-π/2` ground tip) — the
  photon sub-objects keep their authored STILL-billboard facing. Default
  particle/spark orientation (NOMENCLATURE §2; AGENT_GUIDE §4.2.1.6 — "fire
  sparks … should always face the camera").
- **Per-quad transform:** position only (`OBJ3D_POS1`, particle `p[c]`). No
  rotation, no scale override, no per-quad spin. Size = the sub-object's authored
  quad size.
- **Per-vertex color packing:** NONE written by the effect. Verts come from the
  imagery (`T3DAnimator::GetVerts` → `GetObjVerts`, `src/3dimage.cpp:2749-2764`)
  with the imagery's authored color; under MODULATE the sampled spark texel is
  multiplied by that authored diffuse. The effect adds no tint.
- **`trails` field:** for sparks `trails = 1`, so the inner draw loop
  (`effect_old.cpp:4961-4978`) runs once — a single billboard per particle. With
  `trails > 1` the loop would draw the particle plus N−1 ghost copies stepped
  forward along its velocity (a motion-streak); not used by sparks.

---

## 8. Texture animation

**N/A — none.** The 4 photon sub-objects are single-still sprites (no
`framehtexs` array in the I3D; §4). `Render` never mutates `tu/tv` and never calls
`SetTextureFrame`. The texture is static; all motion is positional (the particle
flying), not UV/flipbook. (Contrast: F03 fire = framehtexs flipbook; this is
neither.)

---

## 9. Associated light

**N/A — none.** `TParticle3DAnimator` (Initialize/Animate/Render,
`effect_old.cpp:4694-5002`) makes no `AddPointLight` / dynamic-light call, and the
caller `EffectBurst` (`character.cpp:2225-2317`) adds none. Block sparks do not
light the scene in the original — they are self-lit billboards only.

---

## 10. Color

- **Source:** the **authored `Misc\Sparks.I3D` photon sprite texture** (§4). The
  effect supplies no color of its own (no per-vertex tint, no spell color, no
  chardata field). Color is whatever the sprite is, modulated by the imagery's
  authored vertex diffuse (effectively white → texture passes through unchanged).
- **Exact values:** the sprite is a bright **white / yellow-white** spark/sparkle
  (RGB565 texture data, `legacy/Imagery/Misc/Sparks.i3d` `0x174`–`0x3FAC`); same
  photon-sparkle family as `TPhotonAnimator` (`src/missileeffect.h:84`). Not a
  saturated hue — sparks are deliberately near-white hot, with a warm/yellow core.
- **Expected visual:** bright, hot, near-white metallic sparks (think
  steel-on-steel block impact). **Additive** (video-confirmed, §7) → they GLOW
  against the dark scene, bright cores blowing toward white. A flat/translucent
  (alpha) look is wrong. **A pale/gray/washed result at reconstruction = broken port**
  (likely culprits per AGENT_GUIDE §4.2.1.5: a procedural stand-in instead of the
  real photon sprite #1, or wrong blend/chroma-key). The sparks are *meant* to be
  bright-white, so "near-white" is correct here — but verify it's the **sprite's**
  white, not a default-white fallback (i.e. confirm the texture loaded).
- **Normalization / boosts:** none.

---

## 11. Audio coupling

No audio is triggered by the effect itself (`TParticle3DAnimator` makes no
sound call). The **block sound** is played by the *caller*, right next to the
spark spawn: `PlayWave(listrnd(chardata->blocksounds))` (`character.cpp:1809`),
i.e. a per-character random "block/parry clang" plays on the same event that
spawns the sparks. Record it for the audio phase: **the block-spark burst is
audio-paired with `chardata->blocksounds` at the spawn site
(`src/character.cpp:1804-1810`)**, but the pairing is in combat code, not in the
effect.

---

## 12. Triggers & in-game appearance

- **Spawned by:** combat block resolution. `TCharacter` attack handling, on a
  **missed** swing that the target is **blocking**, calls
  `EffectBurst("sparks")` when the attack carries the `CA_SPARKS` flag
  (`0x0100`, "Show sparks on block", `src/rules.h:40`):
  `if (targ && targ->doing->action == ACTION_BLOCK) { if (attack->flags &&
  CA_SPARKS) EffectBurst("sparks"); ... }` (`src/character.cpp:1805-1810`).
  The registration is `REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator,
  TParticle3DAnimator)` (`effect_old.cpp:4688`); the object is built as a generic
  `TEffect` (no `DEFINE_BUILDER("sparks", …)` exists — the `EffectClass`
  catalogue registers the `"Sparks"` type from `Class.Def:2035`, and the
  per-instance animator resolves to `"sparks"` via `CreateAnimator`,
  `character.cpp:2271-2272`).
- **Where to see it in the original game:** attack any enemy who is actively
  **blocking** with a weapon/attack that has `CA_SPARKS`, and miss into the
  block — sparks fly off the parry. Easiest repro: get an enemy to raise a block
  and swing at it (shield/sword block). The burst appears mid-way between you and
  the target at ~chest height.
- **Vestigial?** No — it has a live caller (combat block). Note that
  `EffectBurst` itself begins with a commented `// return;` (`character.cpp:2227`)
  — the function is live (the `return` is disabled).
- **Reusability:** the same `TParticle3DAnimator` was meant to drive `icedsparks`,
  `sand`, `blood`, `snow` (commented siblings, `effect_old.cpp:4689-4692`) and is
  invoked generically anywhere a caller fills `SParticleParams` and calls
  `InitParticles` (the only other live spark-style call in the pre-release tree
  is the snow/sand emitters at `effect_old.cpp:8116`, `:9543`, which build their
  own params). For "sparks" specifically, the block path is the one live trigger.

---

## 13. Gaps & uncertainties

- **13.0 RETAIL DIVERGENCE — snapshot is wrong for 3 spark params (§2.1).** The
  retail `TCharacter::EffectBurst` decompiles cleanly and shows the shipped spark
  params changed `gravity 0.2 → 0.25`, `trails 1 → 2`, `bounce false → true`
  (`recon/classes/cls_0x5a7b98.cpp:4645-46,4651`). **Use the retail values.** The
  most visible consequence is `trails=2` (each spark draws as a 2-step motion
  streak, not a single billboard, §7) and `bounce=true` (sparks bounce off
  geometry — the §6.2/§13.3 "bounce block skipped" note holds only for the
  *snapshot*; in retail the bounce path at `effect_old.cpp:4852-4858` runs). The
  remaining 11 caller constants + the asset + registration are byte-/value-identical
  in retail, so the rest of this doc is retail-confirmed.
- **13.1 No retail decomp for the *animator* (caller IS decompiled).** There is no
  `recon/classes/` body for the generic `TParticle3DAnimator` itself
  (Animate/Render/InitParticles) — that translation unit wasn't extracted — so the
  animator-internal numbers (the §6.2 update math, the §7 render/blend, the
  `RefreshZBuffer` 75×50 patch, the §6.1 dev sample-default) are **snapshot-only**,
  unverified against shipped retail. Their behavioral risk is low (the snapshot
  bodies are complete and the asset/registration/caller all confirm), but a
  reconstruction should still **visually match against an in-game block capture
  (§12)** to confirm blend, billboard size, and the streak look. By contrast the
  **caller** `TCharacter::EffectBurst` *is* fully decompiled
  (`cls_0x5a7b98.cpp:4535-4665`) and is the authority for §3's spark constants
  (see §2.1). (The only `"sparks"` string XREF that lands in an *unrelated* place
  is the `TSymGlowAnimator` candidate
  `recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml:9-11`; the three
  *relevant* `"sparks"` XREFs are in EffectBurst, the attack-result handler, and
  the animator builder, §2.1(4).)
- **13.2 Ribbon-spark code is a different class** — resolved (see §2
  disambiguation): `effect_old.cpp:4117-4260` is `TRibbonAnimator`, not this
  effect. The "accelerate upward" comment at `:4225` is ribbon behavior; sparks
  use downward gravity.
- **13.3 Dead code in the shared animator.** For the spark use, the homing/seek
  block (`effect_old.cpp:4859-4936`), the bounce block (`:4852-4858`), and the
  `ResetTargetInfo` machinery (`:4731-4749`) never run (`seektargets=false`,
  `bounce=false`, `numtargets=0`). Do NOT reconstruct them as spark behavior.
  They exist because the animator is generic.
- **13.4 No fade/scale curve.** Confirmed the animator applies no alpha/scale
  envelope (§6, §7) — particles pop off at end-of-life rather than fading. If the
  in-game sparks visibly fade, that fade would have to come from the sprite's own
  authored content or a render-side detail not in this code path; from the source,
  there is no per-particle fade. The "soft" feel comes from staggered
  start-delay + lifetime jitter thinning the burst. (Reasonable: bright sparks
  winking out individually reads fine without a per-spark fade.)
- **13.5 Object-variant selection.** `objflags = 1 << (ObjId() & 3)`
  (`character.cpp:2311`) keys the chosen photon variant off the *attacker's*
  object id, so a given character always throws the same spark variant. Whether
  this was intentional (cheap determinism) or meant to be per-particle random is
  unknown; reconstruction can keep it as-is or randomize per particle without a
  visible difference (the 4 variants are near-identical sprites). Low impact.
- **13.6 Exact sprite color.** The texture is RGB565 and reads white/yellow-white
  by inspection of the byte pattern + the photon-family lineage; the precise hot
  core hue should be confirmed against the decoded sprite during reconstruction
  (decode `Sparks.i3d` cell 64×80, or capture an in-game block). Not a behavioral
  unknown — just "render the real texture and verify."

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Sparks.I3D (16,300 B). Address its 4 STILL billboard sub-objects
      by index: 0=photon, 1=photon01, 2=photon02, 3=photon03 (64x80 sprite cell).
      NO procedural spark stand-in — use the real photon sprite. (§4)
- [ ] Drive it as the GENERIC particle system TParticle3DAnimator, parameterized by
      SParticleParams — there are no spark-specific constants in the animator; the
      values live in the caller (§3). (§2, §3, §6)
- [ ] Spawn = one-shot BURST: random(15,25) particles seeded at once; object self-
      destructs (killobj) when all particles' life elapses. (§5, §6.2)
- [ ] Emit origin = caster-local offset vector(caster→target, dist/2) with z=+45;
      object placed at caster world pos; particles in object-local space. (§5)
- [ ] Initial velocity = cone fan: dir = vector(face ± random(-80,80) byte-angle,
      mag~1)/100 (z≈0); per-particle velocity jitter ±0.5 wu/tick each axis;
      position jitter ±3 wu each axis. (§5, §6.1)
- [ ] Per-particle: random start-delay 0..8 ticks, random lifetime 20..40 ticks. (§3, §6.1)
- [ ] Motion (framerate-independent — convert the 24Hz per-tick rates to per-second,
      integrate by real dt; do NOT step at 24Hz): p += v·dt; v.z -= gravity; die at
      end of life. Plain ballistic arc. **Use RETAIL values: gravity 0.25** (snapshot
      0.2), **trails = 2** (snapshot 1 — each spark draws as a 2-step motion streak),
      **bounce = true** (snapshot false — sparks bounce off geometry). All three
      retail-confirmed in cls_0x5a7b98.cpp:4640-4664 (§2.1, §3 DIVERGES rows). (§6.2)
- [ ] NO color/alpha/scale curve — constant draw then pop off at end-of-life; the
      burst "fades" only via staggered start+death. (§6.2, §13.4)
- [ ] Core geometry: one ScreenAligned (camera-facing) billboard per live particle,
      placed at p[c] via position-override only (no rotation/scale). trails=1 ⇒ one
      draw per particle. (§7)
- [ ] Blend = **Alpha** (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA) — what
      the code does; snapshot-only/unverified (retail render body not decompiled, §7),
      so visually vet (if dull, try additive). lit-mode = Unlit, depth = TestNoWrite. (§7)
- [ ] Texture animation = NONE (single-still sprite, no UV scroll, no flipbook). (§8)
- [ ] Variant pick: objflags selects one of the 4 photon sub-objects per burst
      (1 << (ObjId() & 3)); per-particle index o[c] from the selected set. (§6.1, §13.5)
- [ ] Associated dynamic light: NONE. (§9)
- [ ] Color from Sparks.I3D photon texture = bright white/yellow-white; verify the
      real sprite loads (pale/gray ⇒ stand-in / wrong blend / chroma-key). (§10)
- [ ] Sub-effects spawned: NONE. (§6)
- [ ] RefreshZBuffer: restore scene Z over a 75x50 px patch (offset +25/-50) around
      the projected effect origin. (§7)
- [ ] Audio (record for audio phase, NOT in effect): block sound is played by the
      CALLER alongside the spawn — PlayWave(listrnd(chardata->blocksounds)),
      character.cpp:1809. The effect itself triggers no sound. (§11)
- [ ] Trigger wiring: spawned on a blocked melee miss when attack has CA_SPARKS
      (rules.h:40); see character.cpp:1805-1810 + EffectBurst character.cpp:2262-2317. (§12)
- [ ] Do NOT reconstruct: the seek/homing block, ResetTargetInfo, or the params==0
      sample-default — dead/unused for the spark path. (NOTE: bounce IS live in retail
      — bounce=true; reconstruct it. The earlier "skip bounce" note reflected the
      snapshot's bounce=false, which §2.1 superseded.) (§13.3, §6.1)
```

**Definition of done:** a blocked attack produces a one-shot fan of ~15–25 bright
white photon-sprite billboards at ~chest height between attacker and target, each
flying out along a wide cone, arcing down under gravity (0.25), bouncing once off
the floor, winking out over ~20–40 ticks, Alpha-blended (snapshot-only — visually
vet) and camera-facing, single color per burst, with no light and no per-particle
fade — and the burst dies on its own when the last spark expires. (Smooth/time-
based, not 24Hz-stepped. Draw ONE burst at a time when verifying — don't overlap.)
