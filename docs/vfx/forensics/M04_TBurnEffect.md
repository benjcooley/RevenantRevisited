# M04 TBurnEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | M04 |
| **Class(es)** | `TBurnEffect` (object — holds the victim ptr, ticks the periodic damage) + `TBurnAnimator` (visual — two `TParticleSystem` instances `fire` + `smoke` with a rising/decaying particle field). Registered builder `"BURN"` (`effect_old.cpp:3214`); registered 3D animator `"Burn"` (`effect_old.cpp:3290`). |
| **Status** | forensics-complete (see §13 for the genuine unknowns + the snapshot-only physics) |
| **Retail fidelity** | **retail-partial** — the `Magic\burnbabyburn.I3D` asset is **byte-identical** to the shipped `data/imagery.rvi` copy (md5 `7a4fc36ecb49d13b8cf7612256ca34ca`, 25 908 B); both registration strings `"BURN"` (.rdata `005e1098`) and `"Burn"` (.rdata `005e10a0`) live in the retail binary; the `BURN_FRAME = 50` (= `0x32`) constant is corroborated at `cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn` (`recon/classes/cls_0x5a7b98.cpp:10797`, the retail `TCharacter::Burn()`); a retail *fire-effect* poster (`cls_0x5a9194::virt_meth_0x4e1770`, `recon/classes/cls_0x5a9194.cpp:60-69`) is confirmed calling `TCharacter::Burn()` on a hit target — the live poster path of the FireFlash/FireCone family. The animator bodies (`TBurnAnimator::Initialize/Animate/Render/RefreshZBuffer`) are NOT recovered in the retail decomp (the registrar at `0x4e3160` has no isolated body), so every `BURN_*` particle-physics constant in §3 (counts, scales, gravity-substitute, z-range, life, decay) is **snapshot-only**. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-29 |
| **Family** | fire (status overlay sub-family — sister to I22 `TIcedEffect` for ice) |
| **Draws** | composite — two **`TParticleSystem`** instances (`fire` + `smoke`), each driving up to `BURN_COUNT=70` quads built from a *single* I3D sub-object: `fire` uses `smoke01` (`GetObject(1)`), `smoke` uses `smoke` (`GetObject(0)`); see §4. Character-attached, follows the victim each tick. NO dynamic light, NO texture animation, NO audio. |
| **Archetype(s)** | (E) particle emitter — **rising particles** with per-particle decay and a fire→smoke promotion at end-of-life; (F2) custom logic — periodic damage tick on the victim + the `to_add` ramp-up/ramp-down envelope; character-attached status overlay (sister role to I22). Per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md). |

---

## 1. Summary

**"BURN"** is the *burning-character status overlay*: a character that is set on
fire (`TCharacter::Burn()`, `character.cpp:4517`) gets a `TBurnEffect` attached
to them, which paints a column of bright **flame particles** rising off the
victim's body parts and a **darker smoke** trail of expired flames trailing
behind, runs them at full intensity for 50 sim-ticks (≈ 2.1 s @ 24 Hz), then
gradually fades — **stops spawning new particles** and lets the survivors decay
to nothing — and self-deletes. While alive the effect also **periodically
damages** the victim (`random(1,5)` HP, `DT_BURN`, ≈ 5-in-17 chance per
Pulse, gated to the first 50 ticks) and forces the victim into the **"onfire"
chicken-on-fire-dance-of-death** action (`ACTION_BURN`, `character.cpp:4542`).

The visual identity is **two `TParticleSystem` instances stacked**: a `fire`
system using the bright sub-object `smoke01` and a `smoke` system using the dim
sub-object `smoke`. Particles spawn at the victim's body part bone positions
(`ca->GetObjectMatrix(j)` for a random `j` in the char's mesh) with an upward
random velocity; each tick the particle scale decays by `BURN_DEC = 0.97`. When
a fire particle reaches its `life_span`, it is **promoted** to the smoke system
with a 1.25× scale boost and a shortened life — that's the fire-becomes-smoke
trick that gives the effect its rising-flame-then-trailing-smoke character. The
render bracket is `SaveBlendState() / SetAddBlendState() / RestoreBlendState()`
— **AdditiveStraight (ONE/ONE)** — which is the textbook fire-on-black-background
blend and matches the rest of the fire family (FireCone, FireFlash, FireWind,
DragonFire).

---

## 2. Sources & evidence

- **Retail decomp:**
  - **Asset** (`data/imagery.rvi/Imagery/Magic/burnbabyburn.i3d`, 25 908 B) =
    byte-identical to the snapshot (§2.1.2).
  - **Registration strings:** `"BURN"` at `.rdata 005e1098` (1 XREF →
    `004e2f80`); `"Burn"` at `.rdata 005e10a0` (1 XREF → `004e3160`).
    `recon/ghidra/_data.txt:107489-107504`. These two XREF sites are the
    `DEFINE_BUILDER("BURN", TBurnEffect)` + `REGISTER_3DANIMATOR("Burn",
    TBurnAnimator)` registrar thunks; their function *bodies* are not
    isolated in the decomp.
  - **Retail TBurnEffect class candidate:** `cls_0x5a9194` (Ghidra merged
    `TFireFlashEffect + TFireWindEffect + TBurnEffect` into one 13504-byte
    bag; `recon/mappings/TFireFlashEffect_cls_0x5a9194_candidate.yaml:15`
    explicitly notes "`"BURN"` 005e1098 -> 0x4e2f80 (+0x1790) also in
    `cls_0x5a9194`"). The merged file has 7 recovered methods, **none of
    which is the TBurnEffect/TBurnAnimator hot path** — they belong to the
    other merged classes (see §2.1.3).
  - **Retail TCharacter::Burn (the poster):**
    `cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn`
    (`recon/classes/cls_0x5a7b98.cpp:10758-10839`). Recovered cleanly.
    Corroborates **BURN_FRAME = 50** (= `0x32`) — see §2.1.1 — the
    `HasActionAni("onfire")` gate (via the action-name string `s_onfire_005e03b8`),
    the `EffectClass.FindObjType` call shape, the `ForceCommand` /
    `TActionBlock("onfire")` wiring with priority bit `| 0x10`, and the
    one-burning-effect-per-character latch on `mbr_0x1b8` (= `burning`).
  - **Retail fire-effect → Burn() poster:** `cls_0x5a9194::virt_meth_0x4e1770`
    (`recon/classes/cls_0x5a9194.cpp:53-70`) explicitly calls
    `cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn(this_00)` on a hit character
    (gated by `*piVar2 == 3 || *piVar2 == 0x19`, i.e. OBJCLASS_PLAYER /
    OBJCLASS_MONSTER) before calling `Damage(...)`. This is the retail-live
    poster path of the fire family — it confirms the snapshot's
    `chr->Burn()` callsites at `effect_old.cpp:7301` (TFireConeEffect-on-hit)
    were preserved or re-implemented in shipped, *inline* in the fire
    effects rather than via the snapshot's commented-out
    `BurnCharactersInRange` helper (§13.4).
- **Pre-release (authoritative for behaviour):** `src/effect_old.cpp`
  - Constants `:3201-3212` (`BURN_PART_MIN/MAX`, `BURN_ADD`, `BURN_SPREAD`,
    `BURN_MIN_SCL`/`MAX_SCL`, `BURN_MIN_Z`/`MAX_Z`, `BURN_MIN_LIFE`/`MAX_LIFE`,
    `BURN_DEC`, `BURN_FRAME`).
  - `TBurnEffect`: `DEFINE_BUILDER("BURN", TBurnEffect)` `:3214`;
    `Set(ch)` `:3217-3228`; `ResetFrameCount()` `:3231-3238`; `Initialize`
    `:3240-3245`; `Pulse` (the periodic damage tick) `:3247-3266`; `Notify`
    (auto-cleanup on victim deletion) `:3268-3284`.
  - `TBurnAnimator`: `REGISTER_3DANIMATOR("Burn", TBurnAnimator)` `:3290`;
    `Initialize` `:3292-3313`; `Animate` (state machine + per-tick spawn +
    decay + fire→smoke promotion) `:3315-3447`; `Render` `:3449-3460`;
    `RefreshZBuffer` `:3462-3473`. Class decls + `BURN_COUNT = 70`:
    `src/effect.h:1472-1518`.
- **TParticleSystem (the visual workhorse, shared with FireCone/FireWind etc.):**
  declaration `src/effectcomp.h:308-354`; bodies
  - `Init` `effectcomp.cpp:1031-1039`
  - `Animate` (life ticking + velocity integration with `acc` factor)
    `:1041-1068`
  - `Render` (per-particle matrix Scale·Rot·`-π/2`-tip·Translate, additive
    or whatever the caller bracketed) `:1070-1109`
  - `RefreshZBuffer` `:1111-1131`
  - `Add` (find an unused slot, copy params, `life=0`, `used=true`)
    `:1133-1158`.
  The `SParticleSystemInfo` struct (`pos / scl / rot / acc / vel / temp /
  flicker / life / life_span / used`) is the per-particle record.
- **Sister effects consulted (fire family blend + sub-object naming):**
  `TFireConeAnimator` (`effect_old.cpp:7316-7649`) — also uses two
  `TParticleSystem` (`fire` + `smoke` + `burst`) on a shared single I3D
  sub-object; also calls `SetAddBlendState()` (`:7460` Render path).
  `TFireFlashEffect`/`TFireWindEffect` — same family, also additive. This
  triangulates Burn's blend choice (§7).
- **Poster (the coupling source — character → BURN spawn):**
  `TCharacter::Burn()` `src/character.cpp:4517-4552` (the live posting site;
  the commented-out copy at `:5249-5279` inside `Flail()` is DEAD).
- **Other live callers of `chr->Burn()`:**
  - `BurnCharactersInRange(...)` `effect_old.cpp:314-342` — the AoE wrapper.
    Its only callsite in the snapshot is **commented out**
    (`:7648,7678,7689` all under `/* */`) — see §13.4. The function survives
    in the binary as compiled code, but its live coupling is gone in the
    snapshot.
  - `TFireConeEffect::Pulse` `effect_old.cpp:7265-7310` — on the first
    pulse, walks the live `fire.Get(0)` particle of the spawned
    `TFireConeAnimator`, finds the closest in-cone character, and calls
    `chr->Burn()` (`:7301`). **This is the snapshot's one live fire-spell
    Burn-poster.** Retail's `cls_0x5a9194::virt_meth_0x4e1770` is the
    descendent of this path.
  - `CmdBurn` `src/command.cpp:1537-1542` (script `<character>.burn`
    console command) — also live in retail (the command-table addr
    `LAB_00420d50` at `_data.txt:58994` is the retail `CmdBurn`).
- **Existing port shell:** the class declarations exist at
  `src/effect.h:1472-1518` (used here only to confirm field names + the
  `BURN_COUNT = 70` define); **per hard-rule 8 I do not consult
  `src/effect.{cpp}` body** for the M04 port — this forensics is
  archaeology of the snapshot, not against the port.
- **Source-of-truth ranking:** Asset + class-identity + registration +
  TCharacter::Burn() = retail-confirmed (§2.1). Animator bodies + the
  per-particle physics tuning = pre-release snapshot is the only source.
  `effect_old.cpp` is authoritative for *all* visual constants/behaviours;
  retail wins for the lifecycle skeleton wherever it speaks (BURN_FRAME=50
  is the one numeric corroboration available).

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp` is a pre-release development snapshot, not the shipped
game. Active cross-checks against the retail Ghidra decomp:

**1. Constant grep — BURN_FRAME = 50 corroborated.** The retail
`cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn` (= `TCharacter::Burn()`, the
poster) at `recon/classes/cls_0x5a7b98.cpp:10797` writes:

```
this->mbr_0x1bc = 0x32;     // 0x32 = 50  (== BURN_FRAME)
```

The snapshot's TCharacter has no field at offset 0x1bc — only the
`burning` TObjectInstance pointer (at retail's `mbr_0x1b8`, which the same
function checks and writes). So retail **added a per-character burn-timer
counter** at +0x1bc (a tuning re-work — see §13.6), and the initial value
written is exactly `BURN_FRAME = 50` from the snapshot. This is a
**partial corroboration**: the magnitude 50 is the same number that bounds
TBurnEffect's `frame` in `Pulse` (the periodic-damage gate `frame <
BURN_FRAME`, `effect_old.cpp:3252`) and in TBurnAnimator's `Animate` (the
spawn-cap / ramp pivot `frame < BURN_FRAME`, `:3329,3331,3442`). Whether
retail still has a `BURN_FRAME = 50` immediate inside the (un-recovered)
animator body is unknown, but the same magnitude appearing in the bound
character method is consistent. **BURN_FRAME = 50 → yes (retail, in
TCharacter::Burn).**

Grepping for the other immediates (`70 = 0x46` for `BURN_COUNT`, `8` for
`BURN_ADD`, `50 = 0x32` again for `BURN_PART_MIN/MAX`, `15`/`40` for the
scale range, `30`/`75` for the z range, `5`/`15` for life, `0.97f` =
`0x3f7851ec` for `BURN_DEC`) against the merged `cls_0x5a9194.cpp`
(`grep -nE "0x46|0x32|0x4b|0x1e|0xf|5|8|15|40|75" cls_0x5a9194.cpp`)
returns no isolatable animator-body match because the TBurnAnimator
`Initialize` / `Animate` / `Render` / `RefreshZBuffer` bodies are not
present in the recovered methods (the 7 methods on `cls_0x5a9194` are the
FireFlash/FireWind virtuals; the BURN entry is just a registrar pointer
into an un-decompiled body). **All particle-physics constants are
snapshot-only.**

**2. Asset identity — confirmed unchanged.** The shipped
`Imagery/Magic/burnbabyburn.i3d` inside `data/imagery.rvi` (a PK/ZIP
archive) is byte-for-byte identical to the snapshot
`legacy/Imagery/Magic/burnbabyburn.I3D`:

```
md5  7a4fc36ecb49d13b8cf7612256ca34ca   data/imagery.rvi:Imagery/Magic/burnbabyburn.i3d  (25 908 B, 1998-07-07)
md5  7a4fc36ecb49d13b8cf7612256ca34ca   legacy/Imagery/Magic/burnbabyburn.I3D            (25 908 B)
cmp → IDENTICAL
```

So the two embedded sprite cells (`smoke` + `smoke01`) and their authored
colors/textures (warm fire/smoke) are exactly what the shipped TBurnAnimator
draws. The §4 atlas decode and §10 colour reading apply identically to the
shipped effect.

**3. Structure / vftable.** Ghidra merged `TFireFlashEffect +
TFireWindEffect + TBurnEffect` into one massive 13 504 B `cls_0x5a9194`
(`recon/mappings/TFireFlashEffect_cls_0x5a9194_candidate.yaml:15-26`);
that allocator size is far larger than any single effect class can be —
`FUN_00482fb0(0x34c0)` confirms the merge is a Ghidra artefact, not the
real layout. So the **TBurnEffect struct footprint cannot be cleanly
isolated** from the retail decomp. The snapshot `TBurnEffect` carries the
TEffect base + `character` (4 B) + `frame` (4 B) = +8 B atop TEffect, and
the snapshot `TBurnAnimator` carries the T3DAnimator base + `fire`/`smoke`
(two TParticleSystem instances, each = `count`-particle array + 5×4B
fields ≈ 28 B + 70 × `sizeof(SParticleSystemInfo)` ≈ 70×80 = 5 600 B per
system, two systems ≈ 11 200 B for the particle arrays alone) + `size /
frame / to_add` (12 B) ≈ 11.2 KB of working set. This magnitude is
consistent with the merged 13 504 B but does NOT pin the class identity.
Structure verdict: **consistent with retail but unverified at field
granularity.**

**4. Registration + naming — confirmed.**
- `"BURN"` at `.rdata 005e1098` (4 raw bytes `42 55 52 4E 00 00 00 00`)
  — 1 XREF at `004e2f80` (`recon/ghidra/_data.txt:107489-107492` +
  `:107370`). That is the `DEFINE_BUILDER("BURN", TBurnEffect)`
  registrar thunk site.
- `"Burn"` at `.rdata 005e10a0` (4 raw bytes `42 75 72 6E 00 00 00 00`)
  — 1 XREF at `004e3160` (`_data.txt:107494-107498` + `:107379`). That
  is `REGISTER_3DANIMATOR("Burn", TBurnAnimator)`.
- The console-command tag `"usage: <character>.burn\n"` lives at `.rdata
  005c8874` with a command-table entry handler at `LAB_00420d50`
  (`_data.txt:58994-58998`) — confirms `CmdBurn` is live in retail too.
- The action animation `"onfire"` (`s_onfire_005e03b8/03c8/03d0`) is
  XREF'd from `meth_0x4d3590_Burn` — that's the retail-confirmed
  `HasActionAni("onfire")` + `TActionBlock("onfire", ACTION_BURN)`
  posting.

Both **builder strings live in the retail binary**, both **string XREFs
land inside the cls_0x5a9194 / cls_0x5a7b98 family**, and both **caller
paths** (`TCharacter::Burn()` and the retail fire-effect's
`virt_meth_0x4e1770`) are recovered intact. The naming and wiring are
**retail-confirmed.**

**Verdict: retail-partial.** Asset, registration, naming, the
`TCharacter::Burn()` posting handshake (with `BURN_FRAME = 50`
corroborated), and the live fire-effect-on-hit poster path are all
retail-confirmed. The **animator particle physics + the periodic damage
RNG + the `to_add` ramp envelope are snapshot-only** (the TBurnAnimator
bodies were not recovered in the Ghidra decomp). This is the standard
fire-family fidelity pattern — see the analogous reads on
`F07_TFireBallEffect.md` §2.1 and `F05_TFireSwarmEffect`.

---

## 3. Constants

All from `src/effect_old.cpp` / `src/effect.h` unless noted. "wu" = world
units. `random(a,b)` is **inclusive on both ends**
(`src/revutils.cpp:1597-1613`: `rand() % (max-min+1) + min`). The animator
runs once per `Animate` frame in the snapshot (ungated); per NOMENCLATURE
§6, reconstruction converts per-tick → per-second and time-integrates.
24 Hz is the design sim-rate, so the per-frame numbers below ARE the
per-tick numbers.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `BURN_COUNT` | 70 | per-system particle cap | effect.h:1494 | **snapshot-only** — animator body not recovered |
| `BURN_PART_MIN` | 50 | (commented "min particles" — DEAD, see §13.1) | count | effect_old.cpp:3201 | **snapshot-only** + **DEAD**: declared but never referenced in `TBurnAnimator::Animate` or `Initialize` (grep `BURN_PART_MIN` returns only the `#define`) |
| `BURN_PART_MAX` | 50 | (commented "max particles" — DEAD, see §13.1) | count | effect_old.cpp:3202 | **snapshot-only** + **DEAD**: same — `#define` only, no body reference |
| `BURN_ADD` | 8 | max particles spawned per tick (the `to_add` ceiling during ramp-up) | effect_old.cpp:3203 | **snapshot-only** |
| `BURN_SPREAD` | 2 | wu | per-axis position jitter around bone (`±2` x/y/z) | effect_old.cpp:3204 | **snapshot-only** |
| `BURN_MIN_SCL` | 15 | RNG min for scale × 0.01 → 0.15 base scale | effect_old.cpp:3205 | **snapshot-only** |
| `BURN_MAX_SCL` | 40 | RNG max for scale × 0.01 → 0.40 base scale (so `s ∈ [0.15, 0.40]`) | effect_old.cpp:3206 | **snapshot-only** |
| `BURN_MIN_Z` | 30 | RNG min for `vel.z` × 0.1 → +3.0 wu/tick upward | effect_old.cpp:3207 | **snapshot-only** |
| `BURN_MAX_Z` | 75 | RNG max for `vel.z` × 0.1 → +7.5 wu/tick upward (so `vz ∈ [3.0, 7.5]`) | effect_old.cpp:3208 | **snapshot-only** |
| `BURN_MIN_LIFE` | 5 | ticks (lower bound of particle lifespan, fire) | effect_old.cpp:3209 | **snapshot-only** |
| `BURN_MAX_LIFE` | 15 | ticks (upper bound of particle lifespan, fire) | effect_old.cpp:3210 | **snapshot-only** |
| `BURN_DEC` | 0.97f | per-tick scale multiplier (×0.97 each tick → -3% / frame) | effect_old.cpp:3211 | **snapshot-only** |
| `BURN_FRAME` | 50 | frames (master phase boundary @ 24 Hz ≈ 2.08 s) | effect_old.cpp:3212 | **yes (retail, in TCharacter::Burn)** — `cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn` at `recon/classes/cls_0x5a7b98.cpp:10797` writes `this->mbr_0x1bc = 0x32` (= 50) immediately after the `HasActionAni("onfire")` gate; this is the same magnitude that bounds `TBurnEffect::Pulse`'s damage gate (`frame < BURN_FRAME`, `effect_old.cpp:3252`) and the animator's ramp pivot (`frame < BURN_FRAME`, `:3329,3331,3442`) |
| damage chance (per Pulse) | `random(0,17) < 5` ⇒ 5/18 (= 27.78 %) | probability/tick | effect_old.cpp:3252 | **snapshot-only** |
| damage roll (per Pulse) | `random(1,5)` HP | per damage event | effect_old.cpp:3257 | **snapshot-only** |
| damage type | `DT_BURN` = 7 | enum | effect_old.cpp:3257; rules.h:74 | yes — enum is engine-wide |
| fire→smoke promotion scale | ×1.25 | scale multiplier when a fire particle expires | effect_old.cpp:3351-3353 | **snapshot-only** |
| fire→smoke promotion life | `life_span = (life_span * 2) / 3` | new (shortened) smoke lifespan | effect_old.cpp:3349 | **snapshot-only** |
| particle rot.x (Render) | `-90°` | deg (fixed, every particle) | effect_old.cpp:3398 | **snapshot-only** + **applied twice** — see §13.2 (`TParticleSystem::Render` also bakes a `-π/2` X-rot, `effectcomp.cpp:1088`) |
| particle rot.z (Render) | `-45°` | deg (fixed, every particle) | effect_old.cpp:3399 | **snapshot-only** + **applied twice** — see §13.2 (`TParticleSystem::Render` also bakes a `-π/4` Z-rot, `effectcomp.cpp:1089`) |
| particle rot.y (Render) | `0°` | deg | effect_old.cpp:3400 | **snapshot-only** |
| particle acc.{x,y,z} | 1.0 | velocity multiplier per tick — i.e. **no damping** (`vel *= 1.0`) | effect_old.cpp:3403 | **snapshot-only** — `acc` is multiplicative in `TParticleSystem::Animate` (`effectcomp.cpp:1063-1065`), so `acc=1` ⇒ velocity is preserved; gravity is **not** applied (rises forever) |
| particle vel.{x,y} | 0.0 | wu/tick — pure vertical rise, no lateral motion past the initial `pos` jitter | effect_old.cpp:3406-3407 | **snapshot-only** |
| RefreshZBuffer patch | 300 × 300 px, centred on `WorldToScreen(victim_pos)` | px | effect_old.cpp:3469-3472 | **snapshot-only** — RefreshZBuffer body not recovered; very large patch (covers the whole rising column) |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| burn mesh / sprite atlas | `legacy/Imagery/Magic/burnbabyburn.I3D` | **25 908 B** | source mesh for both `fire` and `smoke` particle quads | Registry name `"Burn"` `Class.Def:2050` (hash `0x55471bff`); resolved via the EFFECT object class → `T3DImagery`; the animator addresses sub-objects with `GetObject(0)` and `GetObject(1)` (`effect_old.cpp:3308-3309`) |

**Same file, ONE registry alias:** `"Burn"` only (`Class.Def:2050`); no
`"BURN"` mesh alias (the `"BURN"` registration is the *effect builder* name,
not an imagery alias — the imagery is loaded by class-name `"Burn"` which
the engine matches case-insensitively).

**`burnbabyburn.I3D` sub-object enumeration (verified by string-dump of the
file; file order = `GetObject` index — see `S3DAnimObj* GetObject(int
objnum)`, `src/3dimage.h:464`):**

- **States:** `STILL` (single state at file offset 28); the animator never
  switches imagery state.
- **Sub-object count:** **2** (header `02 00 00 00` at file offset 0x7c0).
- **Sub-objects (file order = index):**

  | index | name | file-offset of name | role |
  |-------|------|---------------------|------|
  | **0** | `smoke` | 1988 | **smoke** sub-object — used by `TBurnAnimator::smoke` (the dark trail particles, `smoke.Init(this, GetObject(0), ...)` `effect_old.cpp:3309`) |
  | **1** | `smoke01` | 2020 | **fire** sub-object — used by `TBurnAnimator::fire` (the bright flame particles, `fire.Init(this, GetObject(1), ...)` `effect_old.cpp:3308`) |

- **Textures:** stored **inline** in the `.I3D` (no external `.bmp`/`.tga`
  filenames in the string dump — same pattern as `iced.I3D`, see I22 §4).
  Both sub-objects are **single quads** (the file is 25.9 KB ≈ 2 ×
  small-sprite-cell + headers — small enough that each sub-object is one
  quad with one inline texture). The transparency follows the engine's
  black-key convention (knowledge 02 §4): black background → keyed
  transparent.

### 4.1 Texture-atlas / UV decode

**There is NO atlas grid here — the I3D contains TWO DISTINCT SUB-OBJECTS,
each a single-quad single-cell sprite.** This is the *sister-pair*
pattern (not the 4×4 fireball flipbook): two complete sprites packaged in
one I3D file, indexed by sub-object number, not by UV cell.

- **Cell layout:** `smoke` = full-quad sprite #0 (the dim/dark/cooler
  smoke quad); `smoke01` = full-quad sprite #1 (the bright/yellow/orange
  fire quad). **No grid.** UV mapping per sub-object is the standard
  whole-texture `(0..1, 0..1)` quad (each sub-object owns its own
  texture; there is no shared cell-index packing).
- **No runtime cell selection.** The animator never reads `objflags`,
  `textureframe`, or a frame counter — there is no per-particle cell
  index. The choice of *which* sprite a particle uses is made at spawn
  time by the **system the particle belongs to** (fire-system particles
  always draw `smoke01`; smoke-system particles always draw `smoke`).
- **UV's read off the actual I3D geometry:** each sub-object's quad
  spans the full texture (`u, v ∈ [0, 1]`). I3D file dump confirms no UV
  sub-rect mapping in the geometry block (single-cell single-quad).
- **Animation mechanism:** **NONE.** No UV-scroll, no `framehtexs`
  flipbook, no per-tick UV math anywhere in `TBurnAnimator::Animate` or
  `TParticleSystem::Render` (§8).
- **Why two textures, not one atlas:** the **fire→smoke promotion**
  (§6.3) re-classifies a fire particle as a smoke particle when it
  expires — that switches *which sub-object texture is drawn* for that
  particle. With distinct sub-objects this is "move the
  SParticleSystemInfo record from fire's array to smoke's array"
  (`smoke.Add(particle)`, `effect_old.cpp:3355`), which the
  `TParticleSystem` layer handles trivially. An atlas would require a
  per-particle cell-id swap, which the `TParticleSystem` doesn't
  support.

**Stand-in guard:** This effect is **asset-driven** for its two sprite
visuals. A reconstruction that draws procedural flame/smoke gradients,
default-additive-blob fallbacks, or one merged "fire+smoke" sprite (instead
of loading and switching between the two distinct sub-objects) is a
stand-in and is wrong (AGENT_GUIDE §4.2.1; memory
`feedback-no-standins`). If the fire reads gray/washed instead of warm
yellow-orange OR the smoke reads bright instead of dim/cool, suspect:
(1) `burnbabyburn.I3D` wasn't loaded (procedural fallback), (2) wrong
sub-object index (fire and smoke swapped — easy bug: §13.5 documents the
deliberate *snapshot* convention `fire=GetObject(1), smoke=GetObject(0)`),
or (3) black-key chroma miss eating the bright fire pixels.

---

## 5. Spawn & emit

- **Trigger semantics:** **attached status overlay** with a fixed-duration
  ramp envelope, not a free-standing world emitter. Spawned by
  `TCharacter::Burn()` (§12) at the victim's position. The animator
  runs forever until either:
  1. `frame >= BURN_FRAME` (= 50 ticks, ≈ 2.08 s) AND `to_add == 0`
     (ramp-down complete) AND `done == true` (all live particles have
     expired) → `KillThisEffect()` + `character->ClearBurn()`
     (`effect_old.cpp:3442-3446`).
  2. The victim is deleted while burning →
     `TBurnEffect::Notify(N_DELETING)` sets `OF_KILL` + calls
     `character->ClearBurn()` + nulls `character`
     (`effect_old.cpp:3268-3284`).
  3. The animator's `Animate` finds `character == nullptr` → immediate
     `KillThisEffect()` (`effect_old.cpp:3322-3326`).
- **Count per trigger:** one BURN effect per victim (the character's
  `burning` ptr is the single-instance latch in
  `character.cpp:4522-4548`; if the character is already burning, the
  poster only calls `ResetFrameCount()` to extend the duration). Per
  tick, up to `to_add` new fire particles are spawned, where `to_add`
  ramps `0 → BURN_ADD (=8)` over the first 50 ticks and `BURN_ADD → 0`
  over the next 50 ticks — see §6.2 and the temporal diagram.
- **Initial direction / distribution:** per particle:
  1. Pick a random bone `j = random(0, size-1)` where `size =
     ca->NumObjects()` — i.e. any of the character's mesh sub-objects
     (`effect_old.cpp:3367`). NOTE: the snapshot's `random` is inclusive
     on both ends, so `j ∈ [0, size-1]` covers every bone uniformly.
  2. Build the *world-space position of bone j* by transforming the
     origin `(0,0,0)` through `obj->matrix × ca->MakeMatrix(mtx)` —
     i.e. local-bone-frame → character-root frame → world frame
     (`effect_old.cpp:3371-3381`). `GetObjectMatrix(j, &obj->matrix)`
     fills the bone matrix for sub-object `j`; `MakeMatrix(&mtx)` is the
     character's root world matrix.
  3. Add `±BURN_SPREAD (=2)` jitter on each axis to that bone position
     (`:3387-3389`).
  4. Set velocity: `vel.x = vel.y = 0`; `vel.z = random(30,75)/10` ⇒
     pure-upward `[3.0, 7.5]` wu/tick (`:3406-3408`).
  5. Set rotation: `rot = (-90°, 0°, -45°)` deg (Render bakes `-π/2`
     X-tip + `-π/4` Z-tip ON TOP of these — see §13.2).
  6. Set scale: `scale = random(15,40) * 0.01` ⇒ `[0.15, 0.40]`, applied
     to all three axes (`:3392-3395`).
  7. Set acceleration: `acc = (1, 1, 1)` — multiplicative, so velocity is
     preserved each tick (no damping, **no gravity** in the snapshot).
  8. Set lifespan: `life_span = random(5, 15)` ticks (`:3410`).
  9. `fire.Add(&p)` — pushes the particle into the `fire` system
     (`:3412`).
- **Emit anchor convention:** **character-attached, per-bone** — the
  effect ORIGINATES at the victim character's `Pos()` (set in
  `TBurnEffect::Set` `:3217-3228` and re-set every Pulse in `Pulse`
  `:3260-3265` so it tracks the victim if they move), and **each
  particle** spawns at a *randomly-selected mesh-sub-object's world-space
  position* on the character's animated rig. So particles burst from
  hands, head, weapon, body — wherever bones are — not from a single
  origin point.
- **Coordinate space:** the per-bone matrix multiplication
  (`obj->matrix × root_matrix`) puts `p.pos` in **world space**; the
  effect's own `Pos()` is also world-space. Particles are integrated and
  rendered in absolute (world) coordinates via the `OBJ3D_ABSPOS` flag
  passed to `TParticleSystem::Render(... abs_pos=true)`
  (`effect_old.cpp:3454-3455` and `effectcomp.cpp:1081-1082`).
  **Coordinate-space verdict: World** (with the bone position as an emit
  anchor; once spawned, particles do not follow the character — they
  rise vertically in world space from wherever they were emitted).
- **Spread / jitter:** per-particle `±BURN_SPREAD = ±2` wu on each axis
  around the bone position; velocity has no lateral jitter (`vx=vy=0`),
  so all rise is purely vertical. The visual width of the column =
  character-mesh bounds + ±2 wu jitter.

### Spatial diagram (per-tick emit shape, side view ZX)

```
 SIDE (ZX), character standing at origin, particles rising:
   wz                                                     ScreenAligned
    │                                                     billboard (smoke/fire
    │                                                     sub-object quad, drawn
 80 │              · · · · · ·         older fire         additive)
    │            · · · · · · ·       (scaling ×.97/tick,
    │          · · · · · · · · ·     life ticking up)
 50 │        · · · · · · · · · · ·   newer fire ──────►   each one rises with
    │      · · · · · · · · · · ·                          vz=random(3.0,7.5)
    │    · · · · · · · · · · ·                            wu/tick; rot.x =
 20 │  ●--●--●--●--●--●--●--●         per-bone spawn      -π/2 tip + the -90°
    │   \\  \\  \\  \\  \\               (±2 wu jitter        baked in (effect)
   0 │   \\  bone hands  weapon         around `obj` pos    rotates the quad
    └────●────●────●────●─────► wx     of CharAnimator      flat-on-XY then onto
        head body lhand rhand          sub-objects)        billboard

 character mesh: TCharacter's animated rig — sub-objects exposed by
 ca->GetObject(j), with their world matrices via ca->GetObjectMatrix(j,...);
 particles spawn at each chosen bone's world position (not at the effect's
 own GetPos()).

 SPAWN RATE: `to_add` per tick (ramps 0→8 over t=0..50, then 8→0 over
 t=50..100). Each new particle gets a fresh random bone, random scale,
 random vz, random life — no atlas frame, no UV scroll, no lateral motion.
```

---

## 6. Behavior & per-frame logic

`TBurnEffect` (object) does the *damage tick* and the *position chase*;
`TBurnAnimator` does the *particle simulation, ramp, fire→smoke promotion,
and end-of-life check*. Both classes cooperate via the `frame` field
(the effect has its own `frame`, the animator has its own `frame`;
both are zeroed at Initialize/InitAnimator and **incremented
independently** in Pulse and Animate respectively — see §13.3).

### 6.0 Setup — `TBurnEffect::Initialize` / `Set` / `Notify`

```
Initialize():                          // effect_old.cpp:3240-3245
    character = nullptr
    SetNotify(N_DELETING)              // subscribe to victim's deletion
    frame = 0

Set(ch):                               // :3217-3228 — called by poster (TCharacter::Burn)
    character = ch
    if (character):
        S3DPoint pos
        character->GetPos(pos)
        SetPos(pos)                    // place the effect at the victim

ResetFrameCount():                     // :3231-3238 — called when re-igniting an already-burning char
    PT3DAnimator anim = (PT3DAnimator)GetAnimator()
    if (anim):
        ((PTBurnAnimator)anim)->ResetFrameCount()   // resets the ANIMATOR's frame (not the effect's)

Notify(notify, ptr):                   // :3268-3284 — broker-bus deletion notice
    if (sector == (TSector*)ptr): return
    TObjectInstance::Notify(notify, ptr)
    if (character and NOTIFY_DELETED(ptr, character)):
        SetFlags(OF_KILL)              // self-kill on next sweep
        character->ClearBurn()         // tell victim "you're no longer burning"
        character = nullptr            // and forget the pointer
```

### 6.1 Per-Pulse damage tick — `TBurnEffect::Pulse` (cite `:3247-3266`)

```
Pulse():
    TEffect::Pulse()                   // base pulse (lifetime/state checks)
    frame++                            // EFFECT's own frame counter, separate from animator's

    if (random(0, 17) < 5 and frame < BURN_FRAME):   // 5/18 chance per tick, ONLY during first 50 ticks
        if (spell):
            spell->Damage(character)            // use spell's damage config if posted by a spell
        else if (character):
            character->Damage(random(1, 5), DT_BURN)  // otherwise raw 1..5 HP burn damage

    if (character):                    // chase the victim every tick
        S3DPoint char_pos
        character->GetPos(char_pos)
        SetPos(char_pos)               // the effect's "world position" tracks the victim
```

**Damage envelope:** roughly `(5/18) × 3 (= mean of random(1,5)) ≈ 0.833`
HP per tick × 50 ticks ≈ **~42 HP expected over a single Burn**, but the
roll variance is high (some hits do 0 damage that tick, some do 5). The
gate `frame < BURN_FRAME` means damage ONLY ticks during the ramp-up
phase + the early flat phase — once the visual is winding down, the
damage stops too.

**Position chase:** the effect tracks the victim every Pulse so the
particle bone-positions stay current (the `MakeMatrix` in the animator
reads `ca`'s live root-matrix, but the effect's own world position
re-anchoring is also necessary for the `RefreshZBuffer` projection in
§7). If the victim moves, the rising flame column moves with them.

### 6.2 The `to_add` ramp envelope — `TBurnAnimator::Animate` (cite `:3328-3332`)

```
++frame                                 // ANIMATOR's frame, ≠ effect's frame
if (frame < BURN_FRAME and to_add < BURN_ADD):
    ++to_add                            // ramp UP: 0→8 over first 50 ticks (1 per tick)
else if (frame >= BURN_FRAME and to_add > 0):
    --to_add                            // ramp DOWN: 8→0 over next 50 ticks (1 per tick)
```

`to_add` is the **per-tick spawn count cap**. It starts at 0 (set in
`Initialize` `:3312`), ramps to `BURN_ADD = 8` over the first 8 ticks
(then stays pinned at 8 until tick 50), then ramps back down to 0 over
ticks 50..58, then stays at 0 until all live particles have decayed.

**The two phases:**
- **Ramp-up + steady-burn** (`frame ∈ [0, 50)`): `to_add` rises 0→8 by
  tick 8, then stays at 8. So tick 0 spawns 0, tick 1 spawns 1, …, tick
  8+ spawns 8 fire particles per tick. The fire column grows in density.
- **Ramp-down** (`frame ≥ 50`): `to_add` drops 8→0 over ticks 50..58. No
  new fire particles after tick ~58; the column dies down to nothing.

### 6.3 Per-tick simulation + fire→smoke promotion — `TBurnAnimator::Animate` (cite `:3339-3361`)

**Promotion step** (runs BEFORE the per-system `Animate` and before new
spawns):

```
for i in 0..BURN_COUNT-1:
    particle = fire.Get(i)
    if not particle->used: continue

    if (particle->life == particle->life_span):    // fire particle just expired
        particle->used = false                      // free slot in fire
        particle->life_span = (particle->life_span * 2) / 3   // shorter life as smoke
        particle->scl.{x,y,z} *= 1.25f                         // 25% bigger when promoted
        smoke.Add(particle)                                     // copy into smoke system
```

This is the **fire-becomes-smoke** moment: any fire particle that has
just reached its `life_span` ticks gets re-classified as a smoke
particle, with a **larger scale** (puff-out as the flame dies) and a
**shorter remaining life** (smoke fades faster than fire grew). The
visual reads as "bright yellow flame licks rise off the body, then puff
into bigger, dimmer, darker smoke that quickly fades."

**Per-system integration** (the workhorse, in `TParticleSystem::Animate`,
`effectcomp.cpp:1041-1068`):

```
fire.Animate()
smoke.Animate()

for each particle p in the system (.used == true):
    if (p.life >= p.life_span):
        p.used = false                  // particle dies
        continue
    p.life++
    if (check_move):                    // both systems init with check_move=true
        p.pos += p.vel                  // integrate position
        p.vel *= p.acc                  // multiplicative damping (acc=1.0 ⇒ no change)
```

So per tick: every live fire particle rises by `vz` wu (no gravity
because `acc.z = 1.0`, so vz never decays), and every live smoke
particle rises by *its* vz (which, since smoke is a promoted-fire copy,
is the same `random(3.0, 7.5)` it was born with).

**Per-tick spawn loop** (runs after the promotion + system Animate; cite
`:3362-3413`):

```
hmm_mat4 mtx
ca->MakeMatrix(&mtx)                    // character's root world matrix

for i in 0..to_add-1:
    j = random(0, size-1)               // pick a random bone (size = ca->NumObjects())
    obj = ca->GetObject(j)
    if (!obj or !ca->GetObjectMatrix(j, &obj->matrix)): continue

    // World-space position of bone j: transform origin through bone × root
    vp = (0, 0, 0)
    MultiplyD3DMATRIX(&dest, &obj->matrix, &mtx)
    D3DMATRIXTransform(&dest, &vp, &p.pos)

    // Per-bone jitter
    p.pos.{x,y,z} += random(-BURN_SPREAD, +BURN_SPREAD)

    // Random scale (uniform per axis)
    scale = random(BURN_MIN_SCL, BURN_MAX_SCL) * 0.01f   // [0.15, 0.40]
    p.scl.{x,y,z} = scale

    // Fixed rotation (Render applies more — see §13.2)
    p.rot = (-90, 0, -45)               // deg

    // Acceleration: 1.0 = no damping (vel preserved)
    p.acc.{x,y,z} = 1.0f

    // Pure upward velocity
    p.vel.x = p.vel.y = 0
    p.vel.z = random(BURN_MIN_Z, BURN_MAX_Z) * 0.1f      // [3.0, 7.5] wu/tick

    // Random life
    p.life_span = random(BURN_MIN_LIFE, BURN_MAX_LIFE)   // [5, 15] ticks

    fire.Add(&p)
```

**The unused `char_pos` line** (`:3383-3384` reads `character->GetPos` into
`char_pos` but never uses it — `p.pos` is already in world coords from the
bone-transform). Inert; preserve as-is for fidelity.

**Per-tick decay** (cite `:3415-3440`): every live particle (in BOTH
systems) gets its scale multiplied by `BURN_DEC = 0.97`:

```
done = true
for i in 0..BURN_COUNT-1:                       // fire
    particle = fire.Get(i)
    if not particle->used: continue
    done = false
    particle->scl.{x,y,z} *= BURN_DEC           // -3% scale per tick

for i in 0..BURN_COUNT-1:                       // smoke
    particle = smoke.Get(i)
    if not particle->used: continue
    done = false
    particle->scl.{x,y,z} *= BURN_DEC           // -3% scale per tick
```

A particle born with `scale=0.40` at life=0 will be at `0.40 × 0.97^15 =
0.252` by end-of-life (15 ticks). The fire→smoke promotion gives it a
`×1.25` boost back up (= 0.315), then the smoke's `(15·2/3) = 10` more
ticks of `×0.97` take it to `0.315 × 0.97^10 = 0.232`. So a long-lived
particle shrinks by roughly **half** over its total fire+smoke life.

### 6.4 End-of-life check (cite `:3442-3446`)

```
if (frame >= BURN_FRAME and to_add == 0 and done):
    ((PTEffect)inst)->KillThisEffect()
    character->ClearBurn()              // release the victim's `burning` latch
```

All three conditions must hold for self-kill:
- `frame >= BURN_FRAME` (we're past the steady-burn window),
- `to_add == 0` (ramp-down has finished — no more new fire spawning),
- `done == true` (no live particles in either system — set true at the
  top of the decay loop and turned false by ANY live particle found).

When all three are true, the effect tears itself down and tells the
character "you're not on fire anymore." Until then, even if `to_add == 0`
and `frame >> 50`, the smoke trail can keep the effect alive (the slowest
smoke particles can live up to ~10 ticks past the last promotion → up to
~tick 60-70 in practice).

### 6.5 Total lifetime estimate

- Ramp-up: ticks 0-8 (to_add 0→8).
- Steady burn: ticks 8-50 (full 8 spawns/tick, fire lifespan 5-15 → some
  particles spawned at tick ~35 will live until tick ~50 then promote
  to smoke for another `~10` → live till tick ~60).
- Ramp-down: ticks 50-58 (to_add 8→0; last fire spawn at tick ~57 might
  live till ~72 + smoke promotion ~82).
- Drain: ticks ~58-82 (no new fire; live fire + promoted smoke drain).

**Practical lifetime: ~85-100 ticks ≈ 3.5-4.2 s @ 24 Hz** — the 2 s
"active burn" phase + a ~1.5-2 s smoke tail.

### Temporal diagram (per-tick envelope + phases)

```
to_add (spawn cap per tick)
  8 ┤      ●━━━━━━━━━━━━━━━━━━━━━━━━━━━●          ramp-up t∈[0,8]:   to_add++ each tick
    │     ╱                              ╲         steady t∈[8,50]:   pinned at 8
    │    ╱                                ╲        ramp-down t∈[50,58]: to_add--
  0 ┤●━━╱                                  ╲●━━━━━━━━━━━━━━━━━━━━━━━━━━ no spawn
    └──┼─────────────────────────────────┼──────┼───────────────────────► frame
       0  8                              50    58                       ~85

damage tick (TBurnEffect::Pulse)
prob ┤●━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━●─────────────────────────  5/18 prob each tick
5/18 │  (random(0,17)<5 AND frame<BURN_FRAME)                          for t∈[0,50],
   0 ┤                                       ●━━━━━━━━━━━━━━━━━━━━━━━━ then 0
     └─────────────────────────────────────┼────────────────────────► frame
       0                                   50

phases:  ramp-up │  steady burn  │ ramp-down │ smoke drain │ self-kill
ticks:   0..8    │  8..50        │ 50..58    │ 58..~80     │ ~85
frame=:  EFFECT.frame and ANIMATOR.frame run in parallel from 0 (init/InitAnimator)
         and increment independently every Pulse / Animate respectively (§13.3).
```

---

## 7. Rendering (original render state + geometry)

Full body `Render()` `effect_old.cpp:3449-3460`:

```
SaveBlendState()
SetAddBlendState()        // <-- AdditiveStraight (ONE/ONE) — see below
fire.Render(false, true)
smoke.Render(false, true)
RestoreBlendState()
return true
```

- **What it draws:** an **I3D composite of two particle clouds**, each
  drawing up to `BURN_COUNT = 70` instances of a *single* shared
  sub-object quad: `fire.Render` draws `smoke01` (the bright cell) once
  per live fire particle; `smoke.Render` draws `smoke` (the dim cell)
  once per live smoke particle.
  - `TParticleSystem::Render(flicker=false, abs_pos=true)`
    (`effectcomp.cpp:1070-1109`) iterates every used particle, builds
    its matrix, and submits it via `animator->RenderObject(object)`.
    `abs_pos = true` sets `OBJ3D_ABSPOS` on the object's render flags,
    telling the renderer the particle position is **absolute world
    coordinates**, not relative to the effect's own pos.
  - `flicker=false` skips the per-particle ×1.5 scale boost
    (`effectcomp.cpp:1093-1098`) — burn particles do not flicker via
    that mechanism; the apparent flicker comes from the random spawn
    timing + the per-tick scale decay.
- **Blend mode (original):** **AdditiveStraight (ONE / ONE).** `Render`
  calls `SetAddBlendState()` (`effect_old.cpp:3452`), which sets
  `D3DTBLEND_DECALALPHA`, `SRCBLEND=ONE`, `DESTBLEND=ONE` per
  `effect_old.cpp:235-244`. Depth-write off (`ZWRITEENABLE=false`),
  depth-test on (`ZENABLE=true`).

  **BLEND SANITY-CHECK** (the §7 mandatory cross-check):
  - **Sprite design = bright fire/smoke on black background.** Both
    `smoke` and `smoke01` are warm-colored cells on the black-key
    background convention (knowledge 02 §4) — i.e. they are textbook
    **additive** sprites (black = transparent under ONE/ONE; bright
    fire pixels add into the framebuffer). The
    snapshot-Additive matches the asset design — **no conflict.**
  - **Sister-family consistency:** the full fire family is additive
    in the snapshot:
    - `TFireConeAnimator::Render` calls `SetAddBlendState()`
      (`effect_old.cpp:7460`)
    - `TFireFlashEffect`/`TFireWindEffect`/`TDragonFireEffect` follow
      the same pattern (per knowledge 03 §1.2)
    - The forensics record for `F07_TFireBallEffect.md` documents
      Additive for the fireball glow path.
    → **Burn is consistent with the fire family.** No suspect-flag.
  - **Top snapshot-drift risk:** the protocol notes that
    `*::Render` bodies are frequently NOT decompiled in retail, so the
    blend choice is often snapshot-only. **TBurnAnimator::Render is
    not recovered in the retail decomp** (the registrar at `0x4e3160`
    has no body extracted), so the render blend is technically
    snapshot-only. HOWEVER, the asset design + sister-family
    consistency make Additive the high-confidence read; the
    reconstruction agent should treat it as snapshot-only-but-strongly-
    indicated, and visually vet against in-game retail (§12).

- **Lit vs self-lit:** **Unlit / self-lit.** `Initialize` (`:3292-3313`)
  does NOT touch the imagery material (no `GetMaterial`/`SetMaterial`),
  and `Render` does not write `lverts[].color`. `TParticleSystem::Render`
  also does not modulate diffuse — the per-particle quad is drawn at the
  asset's authored colour, additively summed into the framebuffer
  (knowledge 03 §3). Classify **Unlit** — the fire colour is literal
  from the asset texture, brightened by the additive blend's
  framebuffer accumulation.
- **Depth / Z:** depth-test on, **depth-write off** (`SetAddBlendState`,
  `effect_old.cpp:238`) → **TestNoWrite**. `RefreshZBuffer`
  (`:3462-3473`) restores scene Z under a **300 × 300 px** patch
  centred on the victim's screen position — large enough to cover the
  whole rising column. (The patch is the same size for fire + smoke
  combined; `TParticleSystem::RefreshZBuffer` is NOT called individually
  — the animator implements its own 300×300 RefreshZBuffer in §7's body
  `:3462-3473`, overriding the per-system one.)
- **Orientation:** **ScreenAligned per particle, with applied per-quad
  rotations.** `TParticleSystem::Render` builds each particle's matrix
  as (`effectcomp.cpp:1083-1104`):

  ```
  D3DMATRIXClear(&object->matrix)
  D3DMATRIXRotateX(&object->matrix, p.rot.x * TORADIAN)       // -90° from burn = -π/2
  D3DMATRIXRotateY(&object->matrix, p.rot.y * TORADIAN)       // 0
  D3DMATRIXRotateZ(&object->matrix, p.rot.z * TORADIAN)       // -45° = -π/4
  D3DMATRIXRotateX(&object->matrix, -(M_PI / 2.0))            // ANOTHER -π/2 X-tip (§13.2)
  D3DMATRIXRotateZ(&object->matrix, -(M_PI / 4.0))            // ANOTHER -π/4 Z-tip (§13.2)
  D3DMATRIXRotateZ(&object->matrix, facing)                   // 0 here (TBurnAnimator passes facing=0 — default arg)
  Scale(p.scl)
  Translate(p.pos)
  ```

  So each particle's net authored-XY-quad orientation is **`RotX(-180°) ·
  RotZ(-90°) · RotY(0°)`** in the order of application. The double-tip
  cumulative rotation is **the snapshot's** convention; per §13.2 this
  is the engine's general pattern for the TParticleSystem path
  (FireCone, FireFlash, FireWind, MIST all hit the same double-tip
  code), so it is consistent across the fire family. The visual result
  with these constant per-particle rotations + an additive blend on a
  small soft sprite is effectively **screen-aligned** (the quad is
  small enough that the static rotation doesn't read as a perspective
  tilt). NO WorldXY tip (the quad does not lie on the floor — it rises
  vertically as a quasi-billboard).

  Classify orientation as **ScreenAligned** (the practical visual
  reading), with the understanding that the matrix is built with a
  literal per-quad rotation pose rather than a per-frame camera-align.
  The reconstruction can render these as ScreenAligned billboards and
  match the retail look.
- **Per-object transform:** **`RotX(-90°) · RotY(0°) · RotZ(-45°) ·
  RotX(-90°) · RotZ(-45°) · RotZ(facing=0) · Scale(s) · Translate(p)`**
  per particle. The two ScreenAligned-like additive blobs (fire + smoke)
  draw N times each per frame, at world positions, with these constant
  rotations + per-particle scale + per-particle translate.
- **Per-vertex color packing:** **NONE.** The animator does not write
  `lverts[].color`/`.diffuse`. Diffuse stays at the mesh default; under
  the ONE/ONE additive blend, the texture color is added directly into
  the framebuffer. The warm fire / dim smoke colors come **entirely
  from the `burnbabyburn.I3D` embedded textures** (§10).

---

## 8. Texture animation

- **NONE.** The animator never mutates `.tu`/`.tv`, never calls
  `ScrollTexture`/`SetTextureFrame`, and never advances a `framenum`
  (searched the full `Initialize`+`Animate`+`Render` bodies,
  `effect_old.cpp:3292-3473`). `TParticleSystem::Render` does not
  animate UVs either (`effectcomp.cpp:1070-1109`). The texture is
  **static per particle** for its entire life. All apparent motion is
  **per-particle position+scale animation** (the rising arc + the
  per-tick `×0.97` shrink + the fire→smoke promotion's `×1.25` puff),
  not texture-coordinate animation.

The `framehtexs[]` flipbook mechanism (used by F03 TFlameEffect) is NOT
in play here — `TParticleSystem` doesn't even expose that path.

---

## 9. Associated light

- **NO dynamic light** in the pre-release source. `TBurnEffect` and
  `TBurnAnimator` emit no light: no `AddLight`/`CreateLight`/`SLightInfo`
  call in `Initialize`/`Set`/`Pulse`/`Notify`/`Animate`/`Render`/
  `RefreshZBuffer` (`effect_old.cpp:3217-3473`). `TParticleSystem`
  doesn't expose a light path.
- The fire-family **spell.def `LIGHT` lines** (e.g. Fire Cone's spell
  light, Fireball's `LIGHT COLOR 255,130,0 INT 255 MULT 20`) belong to
  the *poster's* spell — they're spawned by the spell when the
  TFireConeEffect or similar fires, not by the TBurnEffect overlay.
  Burn itself adds NO light.
- So the BURN overlay produces **no light** of its own. (A reconstruction
  may *optionally* add a warm point-light at the victim for polish
  consistent with the fire-family glow — `LIGHT COLOR 255,130,0
  INT ~150 MULT 10` would be a plausible value-match against the
  fireball/firecone spell-light family. But it is not a pre-release
  fact — flag if added, same caveat as the analogous I22 §9 note.)

---

## 10. Color

- **Source:** the **authored textures + materials of `burnbabyburn.I3D`**
  (warm yellow-orange fire baked into `smoke01`, dim/cool gray-brown
  smoke baked into `smoke` — §4). Drawn under additive blend, so the
  apparent brightness is `framebuffer + texture_color`; texture must
  carry the bright fire pixels for the additive sum to read as warm
  flame. The animator writes no per-vertex colour and zeroes no
  material, so the colour is literally the asset under
  AdditiveStraight (§7). There is **no** chardata/spell-colour/tint
  path feeding this effect's mesh colour in the pre-release source.
- **Exact values:** not a code constant — it lives in the `.I3D`
  texture data. (The fire family's intended hue, warm orange ≈ RGB
  `(255,130,0)` 0-255 from the Fireball spell light, `spell.def:50`-ish
  family, is a useful *target* if a reconstruction ever needs a
  reference colour — but it is not this effect's source.)
- **Expected visual:** **warm yellow-orange fire** rising off the
  victim, transitioning to **darker translucent smoke** as particles
  promote. The fire family signature (AGENT_GUIDE §4.2.1.5). If the
  reconstruction renders pale, gray, or cool, suspect: (1) the
  `burnbabyburn.I3D` textures weren't loaded (procedural stand-in —
  the #1 failure mode here), (2) wrong blend (using Alpha instead of
  Additive — would dim the fire instead of brightening it; would also
  cause the black background to show through as visible gray rather
  than vanish), (3) black-key chroma miss eating the bright pixels,
  (4) `fire` and `smoke` sub-objects swapped (§13.5 — the snapshot
  convention is fire=GetObject(1)/`smoke01`, smoke=GetObject(0)/`smoke`).
- **Normalization / boosts:** none — no `NormalizeColors`/brightness
  math. The additive blend is the only brightness mechanism.

---

## 11. Audio coupling

- **No audio coupling found in `TBurnEffect` or `TBurnAnimator`** — no
  `PLAY(...)`, `SoundPlayer`, or imagery-tag sound call in
  `Initialize`/`Set`/`Pulse`/`Notify`/`Animate`/`Render`/`RefreshZBuffer`
  (`effect_old.cpp:3217-3473`). The dtor is the default (`effect.h:1511`
  `~TBurnAnimator() { Close(); }`).
- The `burnbabyburn.I3D` may carry per-(state,frame) `S3DTag` sounds
  (the data-driven mechanism, knowledge 02 §8), but the animator never
  switches imagery state or steps frames, so any such tags would not
  fire from the animator path. Record as **no code-driven audio**; the
  audio phase should check the `.I3D` tags directly for any
  ambient-loop or onset sound. The retail FireCone/FireBall family
  does have `FIRESHOOT`/`FIREHIT` sound names (per
  `F07_TFireBallEffect.md` §11) — those belong to the posting spells,
  not this overlay.
- The character-side action `TActionBlock("onfire", ACTION_BURN)`
  (`character.cpp:4542`) may itself carry an animation-event sound
  tag for the "onfire" action — that's the actor/animation-system's
  business, not the VFX overlay's. Note for the audio pass: check the
  retail `onfire` action animation for any per-frame sound trigger.

---

## 12. Triggers & in-game appearance

- **Spawned by:** `TCharacter::Burn()` (`src/character.cpp:4517-4552`) —
  the single live posting site in the snapshot. Sequence:
  ```
  bool TCharacter::Burn():
      if (!HasActionAni("onfire")): return false              // gate: rig must support the dance
      if (!burning):                                          // one-burning-per-character latch
          SObjectDef def; memset(&def, 0, sizeof def)
          def.objclass = OBJCLASS_EFFECT
          def.level    = MapPane.GetMapLevel()
          def.pos      = Pos()
          def.facing   = GetFace()
          def.objtype  = EffectClass.FindObjType("BURN")
          TBurnEffect* burn = (TBurnEffect*)MapPane.GetInstance(MapPane.NewObject(&def))
          if (!burn): return false
          burning = burn                                      // latch the ptr
          burn->Set(this)                                     // wire the victim
          TActionBlock* ab = new TActionBlock("onfire", ACTION_BURN)
          ab->priority = true
          ForceCommand(ab)                                    // make the character dance
      else:
          ((TBurnEffect*)burning)->ResetFrameCount()          // already burning → refresh the timer
      return true
  ```
  The retail equivalent (`cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn`,
  §2.1) matches this shape line-for-line + adds the `mbr_0x1bc = 0x32`
  (= 50) write to a new per-character burn timer (§13.6).

- **Live callers of `TCharacter::Burn()` in the snapshot:**
  1. **`TFireConeEffect::Pulse`** (`effect_old.cpp:7265-7310`, posting
     site `:7301`) — the live fire-spell-on-hit poster. On the first
     pulse the FireCone walks the live `fire.Get(0)` particle, finds
     in-range characters (distance ≤ 16 wu), and calls
     `target->Burn()`. **THIS IS THE LIVE SPELL-CAST PATH IN THE
     SNAPSHOT** (Fire Cone spell, `spell.def:133-142`, talisman `"fp"`,
     damage type `DT_BURN`).
  2. **`CmdBurn`** (`src/command.cpp:1537-1542`) — script/console
     command `<character>.burn` calls `Burn()` directly. Also live in
     retail (retail command-table entry at `LAB_00420d50`).
  3. **`BurnCharactersInRange`** (`effect_old.cpp:314-342`) — AoE
     wrapper. Its **only callsite is commented out**
     (`effect_old.cpp:7648,7678,7689` all `/* */`'d) — see §13.4. The
     function still compiles into the binary but has no live caller in
     the snapshot. **Retail re-implements this inline** in fire effects
     (the `cls_0x5a9194::virt_meth_0x4e1770` call to
     `TCharacter::Burn()` on a hit character with `*piVar2 == 3` or
     `0x19` — OBJCLASS_PLAYER / OBJCLASS_MONSTER — is the live AoE-fire
     burn-poster path in retail).
  4. Commented-out paths (`effect_old.cpp:7683,7693`) inside the same
     FireCone region were earlier-draft posting sites — DEAD in the
     snapshot.

- **Damage type:** `DT_BURN = 7` (`rules.h:74`) — "Burn damage (i.e.
  lava, fileball, etc.)"; the comment confirms the broader use cases
  beyond just the Fire Cone spell.

- **Where to see it in the original game:** cast **Fire Cone** (talisman
  `"fp"`, `spell.def:141`) at an enemy at close range — on hit, the
  enemy lights on fire under the Burn overlay for ≈ 2 s of active burn +
  ≈ 2 s of smoke trail, periodically taking 1-5 HP damage, doing the
  "chicken-on-fire-dance-of-death" action animation. The Fire Cone is
  the "fp" priest spell (or rod variant); cast at melee range to see
  the Burn overlay attach. **Capture rig:** character-attached —
  needs a live victim with the "onfire" action animation in their rig
  (the `HasActionAni("onfire")` gate). Capture a **sequence** over the
  full life: ramp-up (frame 0-8, sparse particles), steady burn (frame
  8-50, dense column), ramp-down (frame 50-58, thinning), smoke drain
  (frame ~58-80, smoke trail only). Per memory
  `feedback-vfx-capture-framing`: motion-essential → sequence, frame
  to peak bbox.

- **Auto-cleanup on victim death/deletion:** `TBurnEffect::Notify(N_DELETING)`
  (`effect_old.cpp:3268-3284`) catches the victim's deletion via the
  notification bus and self-kills + clears the victim's `burning` latch.
  So if a burning victim dies mid-burn, the overlay vanishes immediately
  rather than rendering against a freed character.

- **Vestigial?** No — live status effect posted by a live spell (Fire
  Cone), a live console command, and (in retail) the merged fire-effect
  family's on-hit path.

---

## 13. Gaps & uncertainties

1. **`BURN_PART_MIN` / `BURN_PART_MAX` are DEAD constants.** Both
   `#define`'d at `:3201-3202` but never referenced in
   `TBurnAnimator::Initialize` or `Animate`. (Grep `BURN_PART_MIN`
   `BURN_PART_MAX` returns only the `#define` lines.) Likely leftover
   from an earlier draft where the animator pre-allocated `random(MIN,
   MAX)` particles up front; the current `to_add` ramp envelope
   superseded them. Reconstruction should **not implement them** —
   preserve the live `BURN_ADD=8` / `BURN_COUNT=70` semantics only.
   (Same status as I22's commented-out `SetFlag(OF_ICED/OF_DISABLED)`
   duplicates — DEAD in the snapshot, no behaviour to lose.)

2. **Double rotation tip in TParticleSystem::Render (intentional
   convention, not a bug).** The animator sets per-particle `rot.x =
   -90°, rot.z = -45°` (`effect_old.cpp:3398-3400`), and *then*
   `TParticleSystem::Render` applies **another** `-π/2` X-tip and `-π/4`
   Z-tip on top (`effectcomp.cpp:1088-1089`). So the net authored-XY-quad
   rotation per particle is `RotX(-90°+-90°) · RotZ(-45°+-45°)` = `RotX(-180°)
   · RotZ(-90°)` (in compose order). This is **the snapshot convention
   for ALL TParticleSystem clients** (FireCone, FireFlash, FireWind,
   MistEffect — they all set `rot.x = -90` per-particle while the
   shared Render path also tips by `-π/2`). The visual effect with a
   small soft sprite under additive blend is indistinguishable from
   ScreenAligned. Reconstruction should treat the particle as
   **ScreenAligned**; the literal matrix math is preserved for fidelity
   in case the asset is ever re-authored to need a specific tilt.

3. **`TBurnEffect::frame` vs `TBurnAnimator::frame` — two parallel
   counters (intentional, not a bug).** Both are zeroed independently
   (`TBurnEffect::Initialize` `:3244`, `TBurnAnimator::Initialize`
   `:3311`) and incremented in different methods (`Pulse` for the
   effect, `Animate` for the animator). They serve **different
   purposes**: the effect's `frame` gates the periodic damage tick
   (`Pulse`-driven); the animator's `frame` gates the particle ramp +
   end-of-life check (`Animate`-driven). They will be roughly in sync
   *if* the engine's Pulse and Animate run at the same cadence (the
   24 Hz sim tick — they do in the snapshot's design), but they are
   NOT shared storage. `TBurnEffect::ResetFrameCount` resets ONLY the
   animator's frame (`:3231-3238`), not the effect's — which means a
   re-ignite (`TCharacter::Burn()` on an already-burning victim, via
   `ResetFrameCount`) refreshes the **visual ramp** but does NOT
   refresh the **damage-tick window**. If the effect's `frame` has
   already passed `BURN_FRAME = 50` (which can happen if the smoke
   drain phase is still alive when re-ignited), the damage gate
   `frame < BURN_FRAME` would already be false and no damage would
   tick on the re-ignite. This may be an intentional anti-spam
   measure or an unintended artefact — unverifiable from source.
   **Reconstruction note:** preserve both counters as separate
   independent fields; do NOT consolidate them.

4. **`BurnCharactersInRange` is dead in the snapshot.** The function
   is defined at `effect_old.cpp:314-342` but its only callsite is at
   `:7648` — inside `/* … */` (commented out in
   `TFireConeAnimator::Animate`). Two more commented sites at
   `:7678,7689`. In retail this AoE wrapper does NOT survive as a
   callable function (no XREF in `_data.txt` matches its address); the
   live retail equivalent is **inline** burn-posting inside fire-effect
   methods (`cls_0x5a9194::virt_meth_0x4e1770` — see §2.1 + §12). So:
   reconstruction does NOT need to port `BurnCharactersInRange`; just
   wire `TCharacter::Burn()` direct from the fire effects.

5. **Fire/smoke sub-object mapping IS as labelled (not swapped).**
   `effect_old.cpp:3308-3309` reads `fire.Init(this, GetObject(1), ...)`
   and `smoke.Init(this, GetObject(0), ...)`. The I3D file order
   (verified by string-offset dump in §4) is `[0] = smoke, [1] =
   smoke01`. So the BRIGHT sprite (sub-object `smoke01`) drives the
   `fire` particle system and the DIM sprite (`smoke`) drives the
   `smoke` particle system. The naming is slightly counter-intuitive
   (the asset author named the bright fire cell `smoke01` and the dim
   smoke cell `smoke` — perhaps the asset started life as two-cells-of-
   smoke and `smoke01` was repurposed as fire later), but the code is
   correct. **Do not swap them**: reconstruction must call
   `GetObject(1)` for fire and `GetObject(0)` for smoke. If a port
   inverts these the visual will show dim smoke rising off the victim
   (wrong) and bright fire promoting to bigger bright fire (also
   wrong).

6. **Retail TCharacter has an extra burn-timer field at +0x1bc that
   the snapshot lacks.** `cls_0x5a7b98_TCharacter::meth_0x4d3590_Burn`
   writes `this->mbr_0x1bc = 0x32` (= 50) immediately after the
   `HasActionAni` gate (`recon/classes/cls_0x5a7b98.cpp:10797`). The
   snapshot TCharacter has no such field — only the `burning` pointer
   at the equivalent of retail's `mbr_0x1b8`. This is a **retail-only
   tuning re-work**: shipped Revenant probably uses this extra
   character-side timer to bound the burn duration from the
   character's side (perhaps in a `TCharacter::Pulse` body the
   snapshot doesn't show — that body is not recovered either). For
   reconstruction: this is a TCharacter concern, not a TBurnEffect
   concern; the burn visuals are tuned by `BURN_FRAME` inside the
   animator, and that value is corroborated as 50 by the retail
   character-side write. Note for the gameplay-burndown agent if
   character-side burn tracking comes up.

7. **No retail decomp of TBurnAnimator::Animate / Render /
   RefreshZBuffer.** The Ghidra merge into `cls_0x5a9194` did not
   isolate the BURN animator bodies; only the registrar XREFs +
   merged class allocator survived. So every per-particle physics
   constant in §3 (`BURN_COUNT`, `BURN_ADD`, `BURN_SPREAD`, the
   scale/z/life ranges, `BURN_DEC`, the fire→smoke promotion's
   `2/3` life shrink + `1.25×` scale boost, the 300×300 z-patch) is
   **snapshot-only and may have been re-tuned for the shipped game**.
   This is the standard fidelity risk pattern (cf. F07 fireball §13,
   I22 §13.8): reconstruction builds from snapshot values, then
   **verifies visually against in-game retail capture** (§12 capture
   rig) — particle density, column height, fire-vs-smoke balance,
   ramp-up/ramp-down feel, total duration. Adjust if shipped looks
   different.

8. **Periodic damage envelope (`random(0,17)<5` ⇒ 5/18 per tick × 50
   ticks of damage, with `random(1,5)` HP per hit) is snapshot-only.**
   The retail decomp of `TBurnEffect::Pulse` is not recovered. The
   snapshot averages ~14 damage events per burn × ~3 HP = ~42 HP
   total expected, but the variance is high. Retail might have a
   smoother or fixed damage tick. Visually irrelevant — but a
   gameplay-balance fact worth noting. Flag for the gameplay forensics
   agent if Demo 1 balance comes up.

9. **`burnbabyburn.I3D` per-sub-object materials.** The two
   sub-objects' embedded textures + materials are the **only** source
   of the warm-fire / dim-smoke colour separation. The animator never
   tints them. If reconstruction's I3D loader truncates / misreads the
   embedded material (e.g. loses the warm color channel), the effect
   will read pale. The I3D inspection in §4 confirms the file structure
   is single-state two-sub-object; deeper texture-format decode (e.g.
   confirming the exact palette / format) is an I3D-loader concern,
   not VFX scope.

10. **Animate runs ungated in the snapshot.** Both `TBurnEffect::Pulse`
    and `TBurnAnimator::Animate` run once per engine tick with no
    24 Hz gating in evidence. Per NOMENCLATURE §6 and the
    `feedback-framerate-independent-anim` memory, reconstruction
    converts per-tick values → per-second and integrates by real
    delta-time; `BURN_FRAME = 50 ticks @ 24 Hz` = ~2.08 s, the
    fire vz `[3.0, 7.5] wu/tick` becomes `[72, 180] wu/s`, the
    `BURN_DEC = 0.97/tick` becomes `pow(0.97, 24) ≈ 0.48/sec` for
    scale (so the per-second scale halves), etc.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\burnbabyburn.I3D (25908 B); address its TWO sub-objects by index:
      0 = `smoke` (the dim smoke sprite), 1 = `smoke01` (the bright fire sprite).
      Each is a single-quad single-cell — NO atlas grid, NO flipbook, full-quad UVs.
      Do NOT swap the indices (fire = GetObject(1), smoke = GetObject(0)). (§4, §13.5)

- [ ] Build TBurnEffect as a TEffect shell with a TCharacter* `character`
      pointer + an `int32_t frame` counter. Initialize: character=null, frame=0,
      SetNotify(N_DELETING). Set(ch) writes character + SetPos(ch->GetPos()).
      Notify(N_DELETING) on the victim: SetFlags(OF_KILL) + character->ClearBurn()
      + character=null. (§6.0)

- [ ] TBurnEffect::Pulse: chain TEffect::Pulse, then frame++; if
      random(0,17) < 5 AND frame < BURN_FRAME(=50): if spell then
      spell->Damage(character) else character->Damage(random(1,5), DT_BURN).
      Then re-set effect pos to character->GetPos() so it tracks the victim. (§6.1)

- [ ] Build TBurnAnimator as a T3DAnimator with TWO TParticleSystem instances
      (`fire`, `smoke`), each sized BURN_COUNT(=70); plus int32_t size, frame,
      to_add fields. Initialize:
      - Pull character via ((TBurnEffect*)inst)->GetCharacter(); if null,
        KillThisEffect() and bail.
      - size = ((TCharAnimator*)character->GetAnimator())->NumObjects().
      - fire.Init(this, GetObject(1), S3DPoint{0,0,0}, /*check_move=*/true)
      - smoke.Init(this, GetObject(0), S3DPoint{0,0,0}, /*check_move=*/true)
      - frame=0, to_add=0. (§6.0)

- [ ] TBurnAnimator::Animate(draw):
      - Re-fetch character; if null → KillThisEffect() + bail.
      - ++frame; ramp to_add: if frame<BURN_FRAME and to_add<BURN_ADD(=8) →
        to_add++; else if frame>=BURN_FRAME and to_add>0 → to_add--. (§6.2)
      - Promotion pass: for each live fire particle p where
        p.life == p.life_span → mark p.used=false, set
        p.life_span = (p.life_span*2)/3, scale p.scl *= 1.25 (all axes),
        smoke.Add(&p). (§6.3)
      - Run fire.Animate() then smoke.Animate() — per-system position
        integration (TParticleSystem::Animate: life++, pos+=vel, vel*=acc;
        kill on life>=life_span). (§6.3, effectcomp.cpp:1041-1068)
      - Spawn loop: build root matrix via ca->MakeMatrix(&mtx). For i in
        [0, to_add): pick random bone j=random(0, size-1); obj=ca->GetObject(j);
        if !obj || !ca->GetObjectMatrix(j, &obj->matrix) → continue. Compute
        p.pos = transform((0,0,0), obj->matrix × mtx). Add per-axis jitter
        ±BURN_SPREAD(=2). scale = random(BURN_MIN_SCL=15, BURN_MAX_SCL=40)*0.01;
        p.scl=(s,s,s). p.rot=(-90, 0, -45) deg. p.acc=(1,1,1). p.vel=(0, 0,
        random(BURN_MIN_Z=30, BURN_MAX_Z=75)*0.1). p.life_span=random(BURN_MIN_LIFE=5,
        BURN_MAX_LIFE=15). fire.Add(&p). (§5, §6.3)
      - Decay pass: for each live particle in both fire and smoke,
        p.scl *= BURN_DEC(=0.97f) (all axes). Track done=true; set
        done=false if ANY live particle found. (§6.3)
      - End-of-life: if frame>=BURN_FRAME AND to_add==0 AND done →
        KillThisEffect() + character->ClearBurn(). (§6.4)

- [ ] TBurnAnimator::Render: SaveBlendState(); SetAddBlendState()
      (AdditiveStraight = ONE/ONE per effect_old.cpp:235-244);
      fire.Render(/*flicker=*/false, /*abs_pos=*/true); smoke.Render(false,
      true); RestoreBlendState(); return true. (§7)

- [ ] TParticleSystem::Render per particle (the path BOTH systems share, from
      effectcomp.cpp:1070-1109): build object->matrix as RotX(p.rot.x*TORADIAN) ·
      RotY(p.rot.y*TORADIAN) · RotZ(p.rot.z*TORADIAN) · RotX(-π/2) · RotZ(-π/4) ·
      RotZ(facing=0) · Scale(p.scl) · Translate(p.pos); set OBJ3D_MATRIX |
      OBJ3D_ABSPOS (abs_pos=true); call animator->RenderObject(obj). DO NOT
      apply flicker scale (TBurnAnimator passes flicker=false). The DOUBLE
      -π/2 X-tip and DOUBLE -π/4 Z-tip is intentional snapshot convention —
      net result is ScreenAligned-like for small additive blobs. (§7, §13.2)

- [ ] Texture animation = NONE. Sprite is static per particle; motion is
      purely geometric (rise + scale decay + fire→smoke). (§8)

- [ ] RefreshZBuffer: project victim->GetPos() to screen, restore a 300×300 px
      patch centred on that point (offset 0,0). This is large enough for the
      whole rising column. (§7, effect_old.cpp:3462-3473)

- [ ] Color: warm yellow-orange fire from smoke01's embedded texture; dim
      gray-brown smoke from smoke's embedded texture; both summed additively
      into the framebuffer. NO per-vertex color, NO material zeroing, NO
      runtime tint. Pale/gray ⇒ stand-in/chroma-miss/wrong-blend. (§10)

- [ ] Associated dynamic light: NONE in pre-release. Optional warm point-light
      (e.g. RGB 255,130,0 INT 150) at victim pos is a polish deviation; flag
      if added. (§9)

- [ ] Audio: NONE code-driven. Check burnbabyburn.I3D S3DTag sounds and the
      `onfire` action animation in the audio phase for any per-frame sound
      triggers. (§11)

- [ ] Poster coupling: TCharacter::Burn() (character.cpp:4517-4552) is the
      single live posting site — gate on HasActionAni("onfire"), latch via
      `burning` ptr (one-burn-per-character), spawn "BURN" effect at victim
      pos, call burn->Set(this), ForceCommand(TActionBlock("onfire",
      ACTION_BURN)) with priority=true. If `burning` non-null already, call
      ((TBurnEffect*)burning)->ResetFrameCount() (refreshes ANIMATOR's frame
      only — does NOT refresh the EFFECT's damage-window frame; preserve this
      quirk). (§12, §13.3)

- [ ] Live snapshot callers of chr->Burn():
      (a) TFireConeEffect::Pulse @ effect_old.cpp:7301 — Fire Cone spell's
          on-hit branch (this is the live spell-cast Burn-poster in the
          snapshot).
      (b) CmdBurn @ command.cpp:1537-1542 — script/console.
      (c) BurnCharactersInRange @ effect_old.cpp:314-342 is DEAD in snapshot
          (only commented callers) — do NOT port the call path; retail
          re-implements this inline in fire effects (§2.1, §13.4). (§12)

- [ ] Do NOT reconstruct the DEAD `BURN_PART_MIN`/`BURN_PART_MAX` `#define`s
      (§13.1), and do NOT consolidate the two parallel `frame` counters in
      TBurnEffect vs TBurnAnimator (§13.3) — both quirks preserved as-is for
      fidelity to the snapshot.

- [ ] Visually vet snapshot-only physics against in-game retail (Fire Cone
      cast at a target dummy): particle density, column height, fire-vs-smoke
      ratio, ramp-up/down feel, total duration (~3.5-4.2 s). Adjust within
      the verified Additive/Unlit/TestNoWrite blend + asset frame. (§13.7,
      §12 capture rig)
```
