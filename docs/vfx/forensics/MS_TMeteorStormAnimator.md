# MS TMeteorStormAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | MS (Meteor Storm) |
| **Class(es)** | `TMeteorStormEffect : TEffect` (object) + `TMeteorStormAnimator : T3DAnimator` (visual). Animator owns a single `TStormAnimator meteor_storm` helper (the generic storm-particle component, also used in `effectcomp.cpp` for any storm-style effect). |
| **Status** | forensics-complete (see §13 for the genuine unknowns — shipped retail uses a different code path) |
| **Retail fidelity** | **snapshot-only (unverified) — with confirmed asset+spell refactor between snapshot and shipped.** The retail binary contains NO `"MeteorStorm"` string XREFs (only `"meteor explode"` sound), the **asset was changed** from snapshot `Magic\Meteor.I3D` (138 KB, single sub-object `photon01`) to shipped `Magic\Comet.I3D` (44 KB, 5 populated sub-objects), and the **shipped spell routes Meteor Storm through a generic `"StrikeEffect"` builder** (not `"MeteorStorm"`). The snapshot `TMeteorStormAnimator`/`TStormAnimator` bodies are the only readable implementation, but **may not match shipped behavior**. The snapshot's own `legacy/spell.def:119` comment is `// doesn't work`. See §2.1 for the full evidence. |
| **Author / Date** | vfx-forensics-agent / 2026-05-31 |
| **Family** | magic (fire-themed projectile storm — meteor rain) |
| **Draws** | particle emitter — N transformed quads of one I3D sub-object (`GetObject(0)`), each in either a *particle* (falling-meteor sprite) or *impact* (ground explosion sprite) phase, all sampling an animated atlas frame. |
| **Archetype(s)** | (E) simple particle emitter, (F) custom procedural logic + 2-phase state machine per particle, (UV-anim) atlas-cell-selection texture animation (NOT framehtexs flipbook), (H) audio on impact. **NO associated dynamic light** in snapshot code; spell.def declares one but it is **parsed-and-discarded** in pre-release (see §9). |

---

## 1. Summary

Meteor Storm is the player's top-tier offensive spell: meteor projectiles
rain down from a high diagonal sky-direction onto an area, each meteor
flying as an animated streak that explodes into a ground-burst when it
reaches the walk-height of the floor. In the available **snapshot**
source, the effect is built on a generic helper `TStormAnimator` driven by
a parameter struct `SStormParams`; the `TMeteorStormAnimator` instantiates
the helper with meteor-storm-specific numbers and ticks it. Each meteor
follows two phases — a flying *particle* phase (gravity-accelerated falling
quad textured from an 8×2 atlas, frames 0..7, advanced 0.5/tick, looping)
and an *impact* phase (ground-flat quad textured from a 4×4 atlas, frames
8..15, advanced 0.7/tick once). Up to 20 meteors are in flight at once
(`METEOR_STORM_SIZE=20`); a new meteor is added every 10 ticks until
`METEOR_STORM_TICKS=100`, then they wind down 1-per-10-ticks. Each impact
plays `"meteor explode"` and deals `random(5,15)` fire damage in a
50-wu ground radius via `DamageCharactersInRange(..., DAMAGE_FIRE)`. The
helper draws one bound `GetObject(0)` quad per active particle using
`SetBlendState()` (Alpha / MODULATE / SRC_ALPHA / INV_SRC_ALPHA), with
per-vertex UVs rewritten each frame to pick the right atlas cell.

**Reference frames vs snapshot.** The shipped `data/imagery.rvi`
`Magic\Comet.I3D` mesh (5 populated sub-objects: `#comet`, `#sparks`,
`#$head`, `#trail`, `shock01`) is fundamentally different from the snapshot
asset `legacy/Imagery/Magic/Meteor.I3D` (1 sub-object `photon01`), and the
shipped spell.def routes Meteor Storm through `"StrikeEffect"`, not
`"MeteorStorm"`. The available gameplay reference frames (`images/vfx/
28_MeteorStorm/`) consequently show a composite **shipped behavior** the
snapshot animator does not produce — large dark falling rocks with ground
crater glow, sparkles, and a 5×5 shock-burst quad. That shipped behavior is
**not in the available source**; the snapshot's `TStormAnimator` is what
this doc captures. The red "swooshes" in reference frames 07–09 are video-
player skip-arrow UI overlays, not VFX. The reconstruction agent must
treat the snapshot bodies as a *hypothesis* and visually verify against
ground-truth.

---

## 2. Sources & evidence

- **Retail decomp (storm helper):** `recon/mappings/TStormAnimator_cls_0x5ab460_candidate.yaml`
  (MEDIUM confidence — but the candidate's XREFs map to `"Tornado"`, `"Sandswirl"`,
  `"Quicksand"`, **not** `"MeteorStorm"`; the retail storm-base class is registered
  under those three names, not Meteor Storm). `recon/mappings/SStormParams_cls_0x5b42f0_map.yaml`
  is **bogus** — the auto-mapper coincidentally matched offsets, but the
  decompiled body (`recon/classes_readable/SStormParams.cpp`) is plainly
  `TConsolePane`/`TTextPane` code (input-handling switch on ASCII codes,
  not a storm-params POD). Treat that mapping as a false positive.
- **Pre-release (snapshot — the only readable bodies):**
  - `TMeteorStormEffect` (object): `src/effect_old.cpp:6353-6364` — registration
    + empty Initialize + `TEffect::Pulse()` passthrough.
  - `TMeteorStormAnimator` (visual): `src/effect_old.cpp:6365-6502` —
    `Initialize` `:6367-6445`, `Animate` `:6447-6483`, `Render` `:6485-6497`,
    `RefreshZBuffer` `:6499-6502`.
  - `TStormAnimator` helper (generic storm-particle component):
    `src/effectcomp.cpp:38-329` — `Init` `:38-50`, `GetCount` `:53-62`,
    `Set/Get` `:65-77`, `Create` `:80-114`, `Animate` `:116-164`,
    **`Render` `:166-293`** (the D3D body to transcribe), `RefreshZBuffer`
    `:295-329`. Wrapped in `#if 0` (`effectcomp.cpp:17`/`:1556`) — the
    file does not currently compile; the bodies are preserved for forensics.
  - `SStormParams` struct: `src/effectcomp.h:21-50`.
  - `SStormInstance` struct: `src/effectcomp.h:56-70`.
  - Constants `METEOR_STORM_SIZE`, `METEOR_STORM_TICKS`: `src/effect.h:1071-1072`.
  - Constants `METEOR_DAMAGE_MIN`, `METEOR_DAMAGE_MAX`: `src/effectcomp.h:53-54`.
- **Game data (shipped):**
  - `data/imagery.rvi` member `class.def:3375` — `"MeteorStorm" "Magic\Comet.I3D" 0xf32bcfac`.
  - `data/imagery.rvi` member `class.def:3476` — `"StrikeEffect" "Misc\Dummy.i3d" 0xad92bd32` (the actual effect builder routed by retail spell.def).
  - `data/Resources/spell.def:251-289` — shipped Meteor Storm spell entry; effect-name
    `"StrikeEffect"`, IMAGERY `"magic\Comet.i3d"`, `LIGHT COLOR 255,180,0 INT 180 MULT 20`,
    `PLAY "meteorstorm"` (cast sound), and Strike CONTROLDATA block
    (`RADIUS 265`, `HITS 4`/`12`, `PATTERN "random"`, `DURATION 93`,
    `RANGEDAMAGE 150 30`/`150 25`, `WAIT 48`).
  - `data/imagery.rvi` member `Imagery/Magic/comet.I3D` — 44 428 B, MD5
    `978a4868ffc31d0618af6207063ec3c7`. Dumped via the i3ddump tool to
    `i3d_dump_all/comet/` (5 populated sub-objects; see §4).
- **Game data (snapshot, for diff):**
  - `legacy/Class.Def:2042` — `"MeteorStorm" "Magic\Meteor.I3D" 0xf32bcfac`
    (same registry hash, different imagery path).
  - `legacy/spell.def:118-129` — snapshot Meteor Storm spell. Comment
    `// doesn't work` (`:119`). Effect-name `"meteorstorm"` (lowercase, routes
    to the snapshot's own `TMeteorStormEffect` builder, NOT to `StrikeEffect`).
    Snapshot has NO CONTROLDATA blocks; the retail CONTROLDATA parser
    (`src/spell.cpp:124,133`) skips them.
  - `legacy/Imagery/Magic/Meteor.I3D` — 138 464 B, MD5
    `a37db2ba3d3ae73b54654c6d1f9418c3`. Single sub-object `photon01` (string-
    dump confirms). Different file from shipped `Comet.I3D`.
- **Sister effects consulted:** `TSubParticleAnimator` /
  `TShockAnimator` (siblings of `TStormAnimator` in `effectcomp.cpp`) —
  share the helper-pattern: each owns its own params struct + an instance
  array + `Init`/`Animate`/`Render`/`RefreshZBuffer`. Confirms `TStormAnimator`
  is the canonical "generic emitter" Cinematix wrote — `TMeteorStormAnimator`
  is a thin wrapper that just sets the params.
- **Source-of-truth ranking:** Snapshot **`effectcomp.cpp`** (`TStormAnimator::Render`)
  + snapshot **`effect_old.cpp`** (`TMeteorStormAnimator::Initialize`/`Animate`)
  are the only readable bodies. Retail decomp is **silent on MeteorStorm
  specifically** — the candidate storm class
  (`cls_0x5ab460`) belongs to Tornado/Sandswirl/Quicksand. Snapshot is the
  best available evidence but is **likely not what shipped** (see §2.1).

### 2.1 Retail-vs-snapshot reconciliation (verdict: snapshot-only, with refactor evidence)

`src/effect_old.cpp` is a **pre-release development snapshot**, not the
shipped game. Four cross-checks against the retail Ghidra decomp + shipped
data:

**1. Asset identity — DIFFERENT.** The shipped game uses a different file
under a different name:

```
md5  a37db2ba3d3ae73b54654c6d1f9418c3   legacy/Imagery/Magic/Meteor.I3D       (138 464 B)
md5  978a4868ffc31d0618af6207063ec3c7   data/imagery.rvi:Imagery/Magic/comet.I3D (44 428 B)
```

The registry-name string match is `"MeteorStorm"` in **both** class.def
files (snapshot `legacy/Class.Def:2042` and shipped extract `class.def:3375`)
with the same hash `0xf32bcfac` — but the **imagery filename differs**
(`Meteor.I3D` vs `Comet.I3D`) and the asset bytes are unrelated. Sub-object
structure also differs: snapshot has 1 (`photon01`), shipped has 8 (5
populated: `#comet`, `#sparks`, `#$head`, `#trail`, `shock01`; 3 empty:
`comet`, `fgeys`, `head`). The author was clearly mid-redesign — the asset
name went from `Meteor` to `Comet` and the kit-of-parts grew. **A
reconstruction that renders only the snapshot's logic against the shipped
`Comet.I3D` will get the meteor sprite right but will leave 4 sub-objects
unused** (which may or may not match shipped behavior — see §13).

**2. Registration + naming — DIVERGED.** Retail Ghidra `_data.txt` contains:
- `"StrikeEffect"` at `005e4f60` with XREF from `00540b60` (a builder stub
  function, sibling to `0x540b80`=`"Spell"` and `0x540ba0`=`"Strike"`).
- `"Strike"` at `005e4f78`.
- `"meteor explode"` at `005e1624` with **one** XREF from `0050aa91` (the
  impact-sound call).
- **NO `"MeteorStorm"` string anywhere** in the retail binary
  (`grep -n MeteorStorm recon/ghidra/_data.txt` returns nothing).

Shipped `data/Resources/spell.def:264` routes Meteor Storm to `effect=
"StrikeEffect"` with `CONTROLDATA "Strike"`, **NOT** `"MeteorStorm"`. The
shipped engine wires Meteor Storm through `TStrikeEffect` (a generic
multi-hit area-strike) that reads `IMAGERY "magic\Comet.i3d"`. The
snapshot has **no TStrikeEffect class**; this is a retail-introduced
generalization. **The snapshot's `TMeteorStormAnimator` is almost certainly
dead code in retail.**

**3. Structure / vftable — DIVERGED (no shipped class for MeteorStorm).**
- `cls_0x5ab460` ("TStormAnimator" candidate, 2064 bytes, 7 methods) holds
  `"Tornado"`/`"Sandswirl"`/`"Quicksand"` string XREFs — not Meteor Storm.
  The `0x884` (2180 B) inner allocation matches the snapshot
  `TStormAnimator` size (an SStormParams + SStormInstance array, used by
  Tornado/Sandswirl/Quicksand in retail), confirming the shipped retail
  *kept* the generic `TStormAnimator` helper — but **bound to weather
  effects**, not Meteor Storm.
- The `SStormParams_cls_0x5b42f0_map.yaml` mapping is **bogus** (auto-mapper
  false positive — the decompiled body is text/console-pane code).
- No `cls_*` candidate identified for `TMeteorStormEffect` or
  `TMeteorStormAnimator`. The shipped binary likely has no such class —
  Meteor Storm became a `TStrikeEffect` instance with Comet.I3D imagery.

**4. Constant grep — NOT applicable.** With no shipped MeteorStorm function
to grep, the snapshot's magic numbers (`gravity=0.37`, `velocity.x=-10`,
`velocity.z=-15`, `pos_spread=100`, `particle_u=8`/`particle_v=2`,
`impact_u=4`/`impact_v=4`, `particle_begin=0`/`particle_end=7`,
`impact_begin=8`/`impact_end=15`, `particle_frame_inc=0.5`,
`impact_frame_inc=0.7`, scales `0.5/1.5/1.0`, `METEOR_STORM_SIZE=20`,
`METEOR_STORM_TICKS=100`, `METEOR_DAMAGE_MIN=5`, `METEOR_DAMAGE_MAX=15`) are
**snapshot-only** with no retail corroboration.

**Verdict — snapshot-only (with strong refactor signal).** The shipped
retail does not run the snapshot `TMeteorStormAnimator` code. The snapshot
is the only readable implementation; the reconstruction must (a) reproduce
the snapshot behavior against the *shipped* `Comet.I3D` asset, then (b)
visually compare against the in-game reference. If the in-game frames show
behaviors the snapshot can't produce (3D rocks, multi-sub-object compositing,
the shock01 burst), the reconstruction agent must escalate as a
**retail-vs-snapshot divergence requiring redesign** rather than papering
over with the snapshot logic.

---

## 3. Constants

All from the snapshot pre-release source. Retail bodies were **not
recovered** for Meteor Storm (§2.1), so every kinematic constant is
**snapshot-only** — none of them were found as retail immediates. The
*spell-data* constants (radius, damage, hits, timing) for shipped Meteor
Storm come from `data/Resources/spell.def:251-289` and ARE retail-confirmed
(the shipped data file is the live spec), but **they drive the retail
`StrikeEffect` path, not this animator**.

### 3a. TMeteorStormAnimator (wrapper) — `effect_old.cpp:6367-6502`

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `METEOR_STORM_SIZE` | 20 | count (max concurrent particles) | `effect.h:1071` | snapshot-only |
| `METEOR_STORM_TICKS` | 100 | ticks | `effect.h:1072` | snapshot-only — phase-boundary clock |
| add/remove rate | 1 particle every 10 ticks | tick gating | `effect_old.cpp:6452,6454-6458` | snapshot-only — `!(ticks%10)` |
| ratio (per-meteor scale jitter) | `random(5,20)/10.0` ∈ [0.5, 2.0] | scalar | `effect_old.cpp:6425` | snapshot-only — applied uniformly to both particle and impact scale |
| `impact_scale.{x,y,z}` (base) | `1.0 * ratio` | wu | `effect_old.cpp:6427-6429` | snapshot-only |
| `particle_scale.x` (base) | `0.5 * ratio` | wu | `effect_old.cpp:6431` | snapshot-only — additionally jittered ±50% per instance (`effectcomp.cpp:104`) |
| `particle_scale.y` (base) | `1.5 * ratio` | wu | `effect_old.cpp:6432` | snapshot-only — anisotropic; the meteor sprite is *taller than wide* |
| `particle_scale.z` (base) | `1.0 * ratio` | wu | `effect_old.cpp:6433` | snapshot-only |
| `params.rot` (initial) | (0, 0, 0) | rad | `effect_old.cpp:6435-6437` | snapshot-only — animator-internal `rot` member, unused in current Render |
| anim-internal `rot` (initial) | (0, 0, 0) | rad | `effect_old.cpp:6442-6444` | snapshot-only — unused |

### 3b. SStormParams instance for Meteor Storm — `effect_old.cpp:6374-6438`

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `params.particles` (initial) | 0 | count | `effect_old.cpp:6393` | snapshot-only — starts empty, ramps up via `tracker` |
| `params.tex_u` | 64 | px | `effect_old.cpp:6395` | snapshot-only — drives `RefreshZBuffer` rect, NOT the atlas grid |
| `params.tex_v` | 64 | px | `effect_old.cpp:6395` | snapshot-only — same |
| `params.particle_u` | 8 | cells (atlas cols, particle phase) | `effect_old.cpp:6397` | snapshot-only — `u_size = 1/8 = 0.125` |
| `params.particle_v` | 2 | cells (atlas rows, particle phase) | `effect_old.cpp:6398` | snapshot-only — `v_size = 1/2 = 0.5`; particle render always uses row 0 (`v=0`, `effectcomp.cpp:253`) |
| `params.particle_begin` | 0 | frame index | `effect_old.cpp:6400` | snapshot-only |
| `params.particle_end` | 7 | frame index | `effect_old.cpp:6401` | snapshot-only — particle loops [0..7], 8 frames |
| `params.impact_u` | 4 | cells (atlas cols, impact phase) | `effect_old.cpp:6403` | snapshot-only — `u_size = 0.25` |
| `params.impact_v` | 4 | cells (atlas rows, impact phase) | `effect_old.cpp:6404` | snapshot-only — `v_size = 0.25` |
| `params.impact_begin` | 8 | frame index | `effect_old.cpp:6406` | snapshot-only |
| `params.impact_end` | 15 | frame index | `effect_old.cpp:6407` | snapshot-only — impact plays [8..15] once, 8 frames |
| `params.gravity` | 0.37 | wu/tick² | `effect_old.cpp:6409` | snapshot-only — same value as Blood gravity (`effectcomp.cpp:1165 BLOOD_GRAVITY`) |
| `params.velocity.x` | -10.0 | wu/tick | `effect_old.cpp:6412` | snapshot-only — base lateral velocity, falls diagonally toward −x |
| `params.velocity.y` | 0.0 | wu/tick | `effect_old.cpp:6411` | snapshot-only |
| `params.velocity.z` (base) | -15.0 | wu/tick | `effect_old.cpp:6413` | snapshot-only — additionally jittered ×`random(100,150)/100` per instance (`effectcomp.cpp:99`), so per-meteor `vz ∈ [-22.5, -15.0]` |
| `params.pos.x` | `effect_pos.x + 120` | wu | `effect_old.cpp:6415` | snapshot-only — spawn origin OFFSET +120 in x from the cast pos (meteors enter from above-and-right) |
| `params.pos.y` | `effect_pos.y - 20` | wu | `effect_old.cpp:6416` | snapshot-only |
| `params.pos.z` | `effect_pos.z + 300` | wu | `effect_old.cpp:6417` | snapshot-only — spawn height 300 wu above the cast point (the "sky") |
| `params.pos_spread.x` | 100.0 | wu | `effect_old.cpp:6419` | snapshot-only — per-meteor random offset ±100 |
| `params.pos_spread.y` | 100.0 | wu | `effect_old.cpp:6420` | snapshot-only |
| `params.pos_spread.z` | 0.0 | wu | `effect_old.cpp:6421` | snapshot-only — meteors all spawn at the SAME altitude |
| `params.impact_frame_inc` | 0.7 | frames/tick | `effect_old.cpp:6423` | snapshot-only — impact anim plays [8..15] over `(15-8)/0.7 ≈ 10 ticks` |
| `params.particle_frame_inc` | 0.5 | frames/tick | `effect_old.cpp:6424` | snapshot-only — particle anim loops [0..7] over `(7-0)/0.5 = 14 ticks` |

### 3c. TStormAnimator (component) — `effectcomp.cpp:38-329`

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `STORM_DEFAULT_MAX_INSTANCE` | 20 | count (helper instance-array size) | `effectcomp.h:19` | snapshot-only — matches `METEOR_STORM_SIZE`; the wrapper constructs the helper with the default |
| velocity z jitter | `× random(100,150)/100` | per-instance | `effectcomp.cpp:99` | snapshot-only — multiplies the base z velocity at Create |
| particle scale x jitter | `× random(50,150)/100` | per-instance | `effectcomp.cpp:104` | snapshot-only — multiplies particle_scale.x at Create (after the wrapper's ratio already applied) |
| impact scale x jitter | `× random(50,150)/100` | per-instance | `effectcomp.cpp:107` | snapshot-only — same pattern on impact_scale.x; impact_scale.y/.z are stored but **ignored at render** (impact uniform-scales using .x for all axes, `effectcomp.cpp:204-207`) |
| impact-phase position offset | +25 x, +35 y (vs particle phase pos) | wu | `effectcomp.cpp:220-221` | snapshot-only — impact quad drawn 25 wu east and 35 wu south of the particle's last position |
| damage radius (snapshot) | 50 | wu | `effectcomp.cpp:235` | snapshot-only — `DamageCharactersInRange(... pos, 50, ...)` |
| `METEOR_DAMAGE_MIN` | 5 | hp | `effectcomp.h:54` | snapshot-only |
| `METEOR_DAMAGE_MAX` | 15 | hp | `effectcomp.h:53` | snapshot-only |
| damage type | `DAMAGE_FIRE` | enum | `effectcomp.cpp:235` | snapshot-only — meteor is fire-themed despite spell.def's `DT_BLUDGEON`/`DT_BURN` |
| `TORADIANf` | π/180 (float) | rad/deg | `revdefs.h:25` (`TORADIAN`) | yes (retail) — engine-wide constant |
| `FIX_Z_VALUE(z)` | `z / 1.46` | scale-fix | `object.h:31` | yes (retail) — engine-wide z-correction for map→3D |

### 3d. Shipped spell-data constants (NOT consumed by the snapshot animator)

For reference — these drive the retail `TStrikeEffect` path, not this
animator, but the reconstruction agent may need them to wire Meteor Storm
into a shipped-faithful behavior:

| name | value | source |
|------|-------|--------|
| spell name | `"Meteor Storm"` / `"Priest Meteor Storm"` | `spell.def:251,264,277` |
| effect builder | `"StrikeEffect"` | `spell.def:264,277` |
| imagery | `"magic\Comet.i3d"` | `spell.def:267,280` |
| damage type | `DT_BURN` (7) | `spell.def:256` |
| spell delay | 5 | `spell.def:258` |
| **spell LIGHT** | RGB `(255, 180, 0)`, INT 180, MULT 20 | `spell.def:259` (orange-yellow point light) |
| cast sound | `"meteorstorm"` | `spell.def:269,282` |
| Strike `RADIUS` | 265 | `spell.def:268,281` |
| Strike `WAIT` | 48 | `spell.def:270,283` |
| Strike `HITS` (basic) | 4 | `spell.def:271` |
| Strike `HITS` (priest) | 12 | `spell.def:284` |
| Strike `PATTERN` | `"random"` | `spell.def:272,285` |
| Strike `DURATION` | 93 0 | `spell.def:273,286` |
| Strike `RANGEDAMAGE` (basic) | 150 30 | `spell.def:274` |
| Strike `RANGEDAMAGE` (priest) | 150 25 | `spell.def:287` |
| variant mana (basic) | 53 | `spell.def:264` |
| variant mana (priest) | 80 | `spell.def:277` |
| variant minmax damage (basic) | 74 / 94 | `spell.def:264` |
| variant minmax damage (priest) | 92 / 122 | `spell.def:277` |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| meteor mesh (shipped) | `data/imagery.rvi : Imagery/Magic/comet.I3D` | 44 428 B (MD5 `978a4868ffc31d0618af6207063ec3c7`) | the bound mesh for the storm-particle quad; sub-object 0 (`#comet`) is the quad the snapshot Render uses | registry `"MeteorStorm"` (`class.def:3375`); animator does `GetObject(0)` (`effect_old.cpp:6371`) |
| meteor mesh (snapshot, NOT shipped) | `legacy/Imagery/Magic/Meteor.I3D` | 138 464 B (MD5 `a37db2ba3d3ae73b54654c6d1f9418c3`) | the snapshot's bound mesh; single sub-object `photon01` | registry `"MeteorStorm"` (`legacy/Class.Def:2042`) — same hash `0xf32bcfac` but different filename/bytes |

### 4a. `Comet.I3D` (shipped) structural decode

From `i3d_dump_all/comet/manifest.txt` (empirical dump):

- **NumObjects:** 8 (5 populated, 3 empty: `comet`, `fgeys`, `head`)
- **NumTextures:** 2
- **NumMaterials:** 3

**Sub-objects (file order = `GetObject` index):**

| index | name | verts/idxs | bbox (local) | texslot | role |
|-------|------|------------|--------------|---------|------|
| 0 | `#comet` | 4/6 | `(-10,0,-9.99999)..(10,0,9.99999)` — **20×20 quad in XZ plane** (Y=0) | 1 → tex[0] | **The meteor sprite** — the snapshot Render binds this as the per-particle quad. UV occupies the **bottom-right quadrant** of texture[0]: `tu ∈ [0.491905, 0.991905]`, `tv ∈ [0.49679, 0.996789]`. NOTE — the snapshot Render **overwrites** these per-vertex UVs each frame (§8), so the asset's UVs are never used. |
| 1 | `comet` | 0/0 | — | -1 | empty placeholder |
| 2 | `#sparks` | 4/6 | `(-10,0,-0.158449)..(10,0,19.8415)` — **20×20 quad in XZ plane** (Y=0), asymmetric in z (sits at z≈0..20 not centered) | 1 → tex[0] | **A second sprite quad**, geometry positioned to sit "upright" with its base at z=0. UVs span the full **top half** of texture[0]: `tu ∈ [0, 1]`, `tv ∈ [0.00573, 0.50573]`. **NOT used by snapshot Render** (only `GetObject(0)`). |
| 3 | `fgeys` | 0/0 | — | -1 | empty placeholder |
| 4 | `#$head` | 4/6 | `(-106.168, ~0, -106.167)..(106.167, ~0, 106.167)` — **~212×212 quad in XZ plane** (Y=0) | 1 → tex[0] | **A huge ground-flat quad** (≈10× the meteor sprite). UV identical to `#sparks` (top half of tex[0]). NOT used by snapshot Render. Likely the shipped retail's ground-impact disc. |
| 5 | `#trail` | 4/6 | `(-10,0,-0.158449)..(10,0,19.8415)` — same as `#sparks` | 1 → tex[0] | **Trail sprite quad** geometry-identical to `#sparks`. UV occupies the **bottom-left quadrant** of texture[0]: `tu ∈ [0.0020397, 0.50204]`, `tv ∈ [0.495265, 0.995265]`. NOT used by snapshot Render. |
| 6 | `head` | 0/0 | — | -1 | empty placeholder |
| 7 | `shock01` | 4/6 | `(-2.41854, -2.40209, 0)..(2.41854, 2.40209, 0)` — **~5×5 quad in XY plane** (Z=0, screen-flat) | 2 → tex[1] | **Shockwave / impact burst sprite** — uses the 128×128 burst texture. UV near-full `[0.0005, 0.9995]` (full image). NOT used by snapshot Render. |

**Textures:**

- **texture[0]** — 64×64, RGBA (`i3d_dump_all/comet/texture_00_frame_00.png`).
  Visually inspected: a small atlas containing a horizontal yellow/orange
  streak on the left (the meteor sprite, motion-blurred), a yellow glow ball
  in the middle, and a cratered/smoky impact on the right. Likely the
  meteor-particle and impact-explosion frames laid out as:
  - **Particle phase** uses `particle_u=8 × particle_v=2 = 16` cells, each
    `(64/8, 64/2) = (8, 32)` pixels. Frames 0..7 occupy row 0 (`v=0`).
  - **Impact phase** uses `impact_u=4 × impact_v=4 = 16` cells, each
    `(64/4, 64/4) = (16, 16)` pixels. Frames 8..15: `frame % impact_u` →
    column (so column indices 0..3), `frame / impact_v` → row (so row index
    2 or 3 — i.e. impact uses the **bottom half** of the atlas).
  - Both phases share the **same 64×64 atlas** with overlapping geometry
    — the particle cells (top, 8×32) and impact cells (bottom, 16×16) are
    physically different sub-regions of one texture. See §8 for the
    derived UV math.
  - The asset's `material[0]` has `emissive=(1,1,1)` — full white emissive
    (self-glowing surface; the texture color comes through under MODULATE
    blend unattenuated by scene light).
- **texture[1]** — 128×128, RGBA (`i3d_dump_all/comet/texture_01_frame_00.png`).
  A single radial yellow burst image (not an atlas). Used by `shock01` and
  bound by `material[2]` (emissive `(1,1,1)`). NOT consumed by the snapshot
  Render path.

**Materials:**

- `material[0]`: `diffuse=(1,1,1,1)`, `emissive=(1,1,1)`, `texture=0` — the
  meteor atlas, self-lit white emissive.
- `material[1]`: `diffuse=(0.75,0.75,0.75,1)`, `emissive=(0,0,0)`,
  `texture=-1` — untextured gray (unused at render — the empty placeholder
  sub-objects have no faces).
- `material[2]`: `diffuse=(1,1,1,1)`, `emissive=(1,1,1)`, `texture=1` —
  the 128×128 burst, self-lit white emissive.

### 4b. Stand-in guard

The snapshot `TMeteorStormAnimator::Render` uses **only `GetObject(0)`**
(`effect_old.cpp:6371`) — the `#comet` 20×20 sprite quad. Sub-objects
`#sparks`, `#$head`, `#trail`, `shock01` are **unused by the snapshot code
path**. Either:
1. They are legacy kit-of-parts the shipped retail consumes (likely — the
   reference frames show a ground-disc + sparkles + a burst), via the
   retail `TStrikeEffect` route, OR
2. They were authored speculatively for a future iteration that never
   shipped, OR
3. The snapshot animator iterates `GetObject(0..N)` somewhere we missed
   (we did NOT find any other `GetObject` call in `TMeteorStormAnimator`
   or `TStormAnimator` — confirmed by full read of `effect_old.cpp:6367-
   6502` and `effectcomp.cpp:38-329`).

A reconstruction that draws only the `#comet` sprite is what the snapshot
specifies. If the in-game reference shows the other sub-objects rendered,
that is a **retail-vs-snapshot divergence** and the reconstruction agent
must either (a) extend the snapshot logic to include them or (b) implement
a `TStrikeEffect` path. Either choice should be escalated as a forensics
gap (§13), not invented silently.

---

## 5. Spawn & emit

- **Trigger semantics:** **fixed-duration** for the wrapper, **per-particle
  one-shot** for each meteor instance. The wrapper lives until
  `meteor_storm.IsDone() && ticks >= METEOR_STORM_TICKS`
  (`effect_old.cpp:6479`), which means all currently-flying meteors have
  finished their impact phase **and** `ticks >= 100`.
- **Count per trigger:** up to `METEOR_STORM_SIZE = 20` concurrent
  particles. The wrapper's `tracker` field ramps `params.particles`:
  - `tracker++` once every 10 ticks while `tracker < 20 && ticks <= 100`
    (`effect_old.cpp:6452-6455`).
  - `tracker--` once every 10 ticks while `ticks > 100 && tracker != 0`
    (`effect_old.cpp:6457-6458`).
  - The helper's `Create()` runs every Animate (called by `TStormAnimator::
    Animate` at `effectcomp.cpp:163`) and pads the active-particle count up
    to `params.particles == tracker`.
- **Initial direction / distribution:**
  - Meteors **rain from above-and-east** of the cast point. Spawn pos is
    `(cast_pos.x + 120, cast_pos.y - 20, cast_pos.z + 300)` with random
    offset `±100 wu` in x and y, **0** in z (`effect_old.cpp:6415-6421` +
    `effectcomp.cpp:93-95`). So meteors enter the volume at altitude 300,
    spread across a 200×200 wu horizontal area, biased +120 east of the
    target.
  - Initial velocity: `vx = -10`, `vy = 0`, `vz = -15 × random(100,150)/100`
    = `vz ∈ [-22.5, -15.0]` per meteor. So each meteor moves **westward and
    downward** at a fixed diagonal (no per-meteor lateral randomization),
    falling under gravity 0.37 wu/tick² (vz -= 0.37 each tick).
- **Emit anchor convention:** the spell's `SObjectDef::pos` (effect_pos) is
  set by `TSpell::Timer` (`spell.cpp:438-447`) to invoker pos + source-pos
  (hand) + `HEIGHT=100` fallback (variant height `:264` is 100). The
  animator reads it via `((PTEffect)inst)->GetPos(effect_pos)`
  (`effect_old.cpp:6380`) and uses it as the base for the spawn box.
- **Coordinate space:** **World (absolute)** — `obj->flags |= OBJ3D_ABSPOS`
  (`effectcomp.cpp:176`), meaning the matrix-derived position is the final
  world-space coordinate (the engine does not concatenate the parent
  object's transform). The snapshot writes `obj->pos = storm_instance[i].pos
  ± offset` directly (`effectcomp.cpp:213-223`), which is in world space
  because the meteor pos was seeded from `effect_pos` (world).
- **Spread / jitter:**
  - Position: ±100 wu in x and y, 0 in z (`effect_old.cpp:6419-6421`).
  - Velocity (per axis): only `vz × random(100,150)/100`; vx and vy are
    deterministic (`effectcomp.cpp:97-99`).
  - Particle scale (per axis): `part_scl.x × random(50,150)/100`;
    part_scl.y and part_scl.z stay at the params value
    (`effectcomp.cpp:104-106`).
  - Impact scale: `expl_scl.x × random(50,150)/100`; the helper uses
    `expl_scl.x` uniformly for all three impact axes at render
    (`effectcomp.cpp:204-207`), so impact size is uniformly random ∈
    [0.5×base, 1.5×base] (= per-meteor 0.5..1.5 × `1.0 × ratio` =
    `0.25..3.0`).

### Spatial diagram (sky → ground rain, world axes)

```
   world +z (up)
   │  ────  spawn band: z = cast_pos.z + 300, ±100 in x/y around (cast_pos.x+120, cast_pos.y-20)
   │   ☄    ☄  ☄   ← N meteors seeded here (N up to 20)
   │    ╲    ╲   ╲     vel = (-10, 0, -15..-22.5), accelerate by gravity 0.37 wu/tick² downward
   │     ╲    ╲   ╲
   │      ╲    ╲   ☄  (each meteor a 20×20 quad oriented diagonal — RotZ -45, RotX 0, RotY -90, scaled (.x, 1.5, 1.0))
   │       ╲    ╲
   │        ╲    ☄    ← falls until pos.z <= walk-height(pos.x, pos.y)
   │         ☄         then SWITCHES to impact phase:
   │  ────              - frame jumps to 8
   │   ◉                - pos shifts +25 x, +35 y, rotation flips to ground-flat (RotZ 0, RotX +90, RotY 0)
   │                    - quad uniform-scaled by expl_scl.x ∈ [0.25, 3.0]
   │                    - 8 atlas cells played frame_inc=0.7/tick over ~10 ticks
   │                    - DamageCharactersInRange(invoker, pos, 50, 5, 15, DAMAGE_FIRE) (once)
   │                    - PLAY("meteor explode") (once)
   │                    - then expires (particle slot freed)
   └────────────────────────────────► world +x
        cast_pos      (offset +120,-20 = spawn-band center)
```

---

## 6. Behavior & per-frame logic

### 6.1 Wrapper TMeteorStormAnimator::Animate — `effect_old.cpp:6447-6483`

```
on each tick:
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)                           // :6450

    if (ticks % 10 == 0):                                 // :6452 — every 10 ticks
        if (tracker < METEOR_STORM_SIZE(20) && ticks <= METEOR_STORM_TICKS(100)):
            tracker++                                      // :6454-6455 — ramp up
        if (ticks > METEOR_STORM_TICKS(100) && tracker != 0):
            tracker--                                      // :6457-6458 — wind down

        meteor_storm.Get(&params)                          // :6461
        params.particles = tracker                         // :6462
        meteor_storm.Set(&params)                          // :6463 — write target count back

    meteor_storm.Animate()                                 // :6476 — calls Create() at end (:163)

    if (meteor_storm.IsDone() && ticks >= METEOR_STORM_TICKS):
        ((PTEffect)inst)->KillThisEffect()                // :6479-6480

    ++ticks                                                // :6482
```

**Timeline (24-Hz tick assumed; current snapshot is ungated, runs at render rate):**
- ticks 0..9: tracker=0 → no meteors (Create runs but tracker=0 means no spawns yet)
- tick 10: tracker→1 (1 meteor in flight after Create)
- tick 20: tracker→2 …
- tick 100: tracker→11 (or capped 20 if not yet)
- tick 110: ticks>100 so tracker--, now 10
- … decay continues until tracker=0
- effect dies when all meteors have finished AND ticks >= 100

The cap at 20 means at peak rate the storm has up to 20 meteors active —
in practice the cap is hit around tick 200 if no impacts happen. With
gravity 0.37 starting at vz=-15..-22.5, a meteor falling 300 wu takes
roughly `t = (-vz0 ± sqrt(vz0² + 2·g·dz)) / g` ≈ 40-60 ticks to impact
(rough), so steady state is well under 20.

### 6.2 Helper TStormAnimator::Animate — `effectcomp.cpp:116-164`

```
on each tick, for each i in 0..size:
    if (!storm_instance[i].used): continue              // :124

    if (storm_instance[i].is_particle):                 // PARTICLE PHASE
        new_pos = pos + velocity                         // :129-131
        height = MapPane.GetWalkHeight(int(new_pos))     // :136

        if (height >= int(new_pos.z)):                   // hit ground? :138
            frame = params.impact_begin (8)              // :140 — jump to impact frame
            is_particle = false                          // :141 — switch phase
        else:                                            // still flying
            pos = new_pos                                // :145-147
            velocity.z -= gravity                        // :148 — accel down
            frame += particle_frame_inc (0.5)            // :149
            if (frame >= particle_end(7)):
                frame -= (particle_end - particle_begin) // :150-151 — wrap, loop animation

    else:                                                // IMPACT PHASE
        if (int(frame) >= params.impact_end(15)):
            used = false                                 // :156-157 — particle dies
        else:
            frame += impact_frame_inc (0.7)              // :159

// then refill
Create()                                                  // :163 — top up active count to params.particles
```

The particle's animation frame **wraps** during flight (loops the 8-frame
meteor anim continuously), but the impact frame plays **once and dies**
(no wrap). The phase transition is one-way (particle → impact, never back).

### 6.3 Helper TStormAnimator::Create — `effectcomp.cpp:80-114`

```
while GetCount() < params.particles:
    find first unused instance slot i
    instance[i].used = true
    instance[i].pos = params.pos + random(±pos_spread)             // :93-95
    instance[i].is_particle = true                                  // :96
    instance[i].velocity.x = params.velocity.x                      // :97 — no jitter on x
    instance[i].velocity.y = params.velocity.y                      // :98 — no jitter on y
    instance[i].velocity.z = params.velocity.z × random(100,150)/100// :99 — vz jittered ×1.0..1.5
    instance[i].gravity = params.gravity                            // :100
    instance[i].frame = params.particle_begin (0)                   // :101
    instance[i].particle_frame_inc = 0.5                            // :102
    instance[i].impact_frame_inc = 0.7                              // :103
    instance[i].part_scl.x = particle_scale.x × random(50,150)/100  // :104 — sx jittered
    instance[i].part_scl.y = particle_scale.y                       // :105 — sy fixed
    instance[i].part_scl.z = particle_scale.z                       // :106 — sz fixed
    instance[i].expl_scl.x = impact_scale.x × random(50,150)/100    // :107
    instance[i].expl_scl.y = impact_scale.y                         // :108 (stored, unused)
    instance[i].expl_scl.z = impact_scale.z                         // :109 (stored, unused)
    instance[i].explosion_sounded = false                           // :110
    //PLAY("meteor fall")                                            // :111 — commented out
```

The `//PLAY("meteor fall")` at spawn was disabled in the snapshot; only
the impact sound (`"meteor explode"`) plays (§11).

### 6.4 Damage + sound trigger (on first frame of impact phase) — `effectcomp.cpp:227-238`

This runs **inside** `TStormAnimator::Render` (not Animate), one tick after
the phase transition, gated by `!explosion_sounded` so it fires once
per-particle:

```
if (!is_particle && !explosion_sounded && ((PTEffect)inst)->GetSpell()):
    pos.x = int(particle.pos.x) + 25                                 // :232
    pos.y = int(particle.pos.y) + 35                                 // :233
    pos.z = MapPane.GetWalkHeight(pos)                               // :234 — snap to ground
    DamageCharactersInRange(spell->GetInvoker(), pos, 50,
                            METEOR_DAMAGE_MIN(5), METEOR_DAMAGE_MAX(15),
                            DAMAGE_FIRE)                              // :235
    PLAY("meteor explode")                                            // :236
    explosion_sounded = true                                          // :237
```

Note the `(+25, +35)` offset on the damage position — matches the offset
applied to the impact quad's render position (`effectcomp.cpp:220-221`),
so the damage epicenter is the **visible** impact quad's center, not the
flight's last position.

### 6.5 Temporal diagram (one meteor's life)

```
phase         |  particle (flying)                   |  impact (exploding)  |
              |                                      |                      |
frame index   |  0 → 0.5 → 1.0 → ... → 7 → 0.5 → ... |  8 → 8.7 → 9.4 → ... → 15 → dead
                ↑ loops 8-frame anim (atlas row 0)     ↑ plays once (atlas rows 2-3)

vz            |  -15..-22.5  → −0.37/tick (gravity)  |  N/A
pos.z         |  300 → ... (falls)                   |  ground (snapped)
rotation      |  RotZ -45°, RotY -90° (oblique)      |  RotX +90° (ground-flat)
scale         |  (sx, 1.5, 1.0) × ratio              |  uniform (sx × ratio)
draw pos      |  pos                                  |  pos + (25, 35, ground)

damage/audio  |  —                                   |  Once on first impact tick:
                                                       DamageCharactersInRange(5..15, DAMAGE_FIRE)
                                                       PLAY("meteor explode")

frame trigger |  Animate()                            |  Animate(): frame += 0.7
                                                       Render(): explosion_sounded gate
```

---

## 7. Rendering (original render state + geometry)

### 7.1 Wrapper: TMeteorStormAnimator::Render — `effect_old.cpp:6485-6497`

```
SaveBlendState();                  // :6487
SetBlendState();                   // :6488 — Alpha mode (see below)
meteor_storm.Render();             // :6492 — delegate to helper
RestoreBlendState();               // :6494
return true;                       // :6496
```

- **Blend mode (original):** **`SetBlendState()`** =
  `TEXTUREMAPBLEND=D3DTBLEND_MODULATE`, `ZWRITEENABLE=false`,
  `ZENABLE=true`, `SRCBLEND=SRCALPHA`, `DESTBLEND=INVSRCALPHA`
  (`effect_old.cpp:221-233`; knowledge/03 §1.1). **NOT additive** —
  classify as **Alpha (modulated)**.
  - **Blend sanity check:** the texture[0] atlas is a yellow/orange meteor
    sprite + impact on what looks like a non-black/transparent background
    (the cells have soft edges). Alpha blend on alpha-keyed sprite is
    consistent with what the sprite looks like. **No mismatch flag —
    snapshot-Alpha here is plausible**, but note: with `material[0]
    emissive=(1,1,1)` and Alpha+MODULATE, the sprite renders self-lit
    without scene shading. If retail wanted a stronger fire-glow look,
    SetAddBlendState (ONE/ONE) would be a sister-effect candidate. The
    snapshot blend is the documented choice.

### 7.2 Helper: TStormAnimator::Render (the body to transcribe) — `effectcomp.cpp:166-293`

For each used `storm_instance[i]`:

1. **Setup** (`:173-177`):
   ```
   animator->ResetExtents();
   obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS | OBJ3D_VERTS;
   D3DMATRIXClear(&obj->matrix);                  // identity
   ```
   `OBJ3D_MATRIX` = use the explicit `obj->matrix` (ignore anikey/pos/rot/scl
   members for transform; `3dimage.h:113`). `OBJ3D_ABSPOS` = pos is already
   world (`:127`). `OBJ3D_VERTS` = use the explicit vertex list (`:118`).

2. **Phase-dependent rotation** (`:180-193`):
   - **Particle phase** (`is_particle == true`):
     ```
     D3DMATRIXRotateZ(matrix, -45 * TORADIANf);       // :183
     D3DMATRIXRotateX(matrix,   0 * TORADIANf);       // :184 — no-op
     D3DMATRIXRotateY(matrix, -90 * TORADIANf);       // :185
     ```
     i.e. spin -45° around Z (tilt sprite obliquely), then spin -90° around Y.
     Net effect: the meteor sprite's local XZ plane is rotated so its
     long axis runs at a downward diagonal across the screen — the "streak"
     orientation matching the falling-meteor look.
   - **Impact phase** (`is_particle == false`):
     ```
     D3DMATRIXRotateZ(matrix,   0 * TORADIANf);       // :190 — no-op
     D3DMATRIXRotateX(matrix,  90 * TORADIANf);       // :191
     D3DMATRIXRotateY(matrix,   0 * TORADIANf);       // :192 — no-op
     ```
     i.e. rotate +90° around X — lays the sprite **flat on the ground**
     (WorldXY orientation; "ground decal" archetype).

3. **Phase-dependent scale** (`:196-208`):
   - **Particle phase:**
     ```
     obj->scl = storm_instance[i].part_scl;  // (sx_jittered, 1.5*ratio, 1.0*ratio)
     D3DMATRIXScale(matrix, &obj->scl);
     ```
   - **Impact phase** (`:204-207`):
     ```
     obj->scl.x = storm_instance[i].expl_scl.x;  // uniform scale from .x
     obj->scl.y = storm_instance[i].expl_scl.x;  // ← .x, not .y
     obj->scl.z = storm_instance[i].expl_scl.x;  // ← .x, not .z
     D3DMATRIXScale(matrix, &obj->scl);
     ```
     The impact phase uses **uniform** scale = `expl_scl.x` (per-instance
     jittered × `random(50,150)/100` × `ratio`). `expl_scl.y` and `expl_scl.z`
     are computed at Create but **ignored** here.

4. **Phase-dependent translation** (`:211-224`):
   - **Particle phase:**
     ```
     obj->pos.x = storm_instance[i].pos.x;
     obj->pos.y = storm_instance[i].pos.y;
     obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);   // z / 1.46
     D3DMATRIXTranslate(matrix, &obj->pos);
     ```
   - **Impact phase** (`:220-223`):
     ```
     obj->pos.x = storm_instance[i].pos.x + 25;       // +25 east offset
     obj->pos.y = storm_instance[i].pos.y + 35;       // +35 south offset
     obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);
     D3DMATRIXTranslate(matrix, &obj->pos);
     ```
     The (+25, +35) offset shifts the impact sprite away from the meteor's
     last airborne position toward the ground-impact center. The damage
     epicenter (§6.4) uses the **same** offset, so they align.

5. **One-shot damage/sound (impact, first frame only)** (`:227-238`):
   ```
   if (!is_particle && !explosion_sounded && spell):
       pos = (instance.pos.x + 25, instance.pos.y + 35, walkheight)
       DamageCharactersInRange(spell->invoker, pos, 50, 5, 15, DAMAGE_FIRE)
       PLAY("meteor explode")
       explosion_sounded = true
   ```

6. **Per-vertex UV rewrite (atlas cell pick)** (`:242-275`): see §8 for
   the cell-math. The 4 vertices of the sprite quad are assigned (tu, tv)
   to bound the chosen cell in the texture.

7. **Submit & finalize** (`:290-291`):
   ```
   animator->RenderObject(obj);
   animator->UpdateExtents();
   ```

### 7.3 Render properties summary

- **What it draws:** N transformed instances of the SAME bound sub-object
  (`#comet`, sub-object 0 of `Comet.I3D`, a 20×20 quad with 4 verts).
  Each instance has its own matrix (rotation + scale + translation) and
  its own UV-overwrite (cell pick).
- **Blend mode:** **Alpha (modulated)** —
  `MODULATE/SRC_ALPHA/INV_SRC_ALPHA`, depth-test ON, depth-write OFF
  (`SetBlendState()`, `effect_old.cpp:221-233`).
- **Lit vs self-lit:** **Self-lit** — material 0's `emissive=(1,1,1)`
  emits its texel unattenuated by scene light. The animator does **not**
  write `lverts[].color` (no per-vertex color packing), so the default
  vertex diffuse (white) modulates the texel as identity (no tint).
- **Depth / Z:** depth-test ON, depth-write OFF → **TestNoWrite**.
  `RefreshZBuffer` (§7.4) restores scene Z under each particle's footprint.
- **Orientation:**
  - Particle phase — oblique-streak via `RotZ(-45°) · RotY(-90°)` — call
    this **WorldOblique** (not a NOMENCLATURE-standard orientation; the
    sprite is neither ScreenAligned nor WorldXY-flat, it's tilted to look
    like a falling streak from the iso camera angle).
  - Impact phase — **WorldXY** (ground-flat) via `RotX(+90°)` (the
    `rot.x = ±π/2` tell — knowledge/03).
- **Per-quad transform:** per-instance scale (anisotropic in particle
  phase, uniform in impact) + translation (world-space).
- **Per-vertex color:** **none written** — vertex colors stay at whatever
  the I3D's authored vertex diffuse was (white). The texture passes
  through unchanged under MODULATE blend.

### 7.4 RefreshZBuffer — `effectcomp.cpp:295-329`

```
// pick the larger of particle vs impact scale per axis (worst case footprint)
scale.x = max(params.particle_scale.x, params.impact_scale.x);
scale.y = max(params.particle_scale.y, params.impact_scale.y);

// footprint in screen px — 1.5× safety margin
rect_u = int(params.tex_u(64) * scale.x * 1.5)
rect_v = int(params.tex_v(64) * scale.y * 1.5)

for each used instance:
    map_pos = int(instance.pos)
    WorldToScreen(map_pos, screen_pos)
    RestoreZ(screen_pos.x - rect_u/2, screen_pos.y - rect_v/2, rect_u, rect_v)
```

So each meteor restores a Z patch sized to `(96 × max_scl.x, 96 × max_scl.y)`
in screen pixels (assuming `tex_u=tex_v=64` and 1.5× safety) — the engine
re-paints scene Z under the sprite's footprint so the sprite can blend
correctly without writing Z.

---

## 8. Texture animation

**Mechanism:** **per-vertex UV-overwrite picks an atlas cell each frame**
(NOT a `framehtexs[]` flipbook handle swap, NOT a UV-scroll). Same bound
texture every frame; only the four `obj->lverts[0..3].tu/.tv` get
rewritten to point at the cell for the current frame.

**Atlas math** — `effectcomp.cpp:242-275`:

```
frame = int(storm_instance[i].frame)                  // current frame counter

if (is_particle):                                     // particle phase
    u_size = 1.0 / particle_u  = 1.0 / 8 = 0.125     // cell width in UV
    u      = u_size * (frame % particle_u)            // column = frame % 8
    v_size = 1.0 / particle_v = 1.0 / 2 = 0.5
    v      = 0                                         // row 0 (top half)
else:                                                  // impact phase
    u_size = 1.0 / impact_u  = 1.0 / 4 = 0.25
    u      = u_size * (frame % impact_u)               // column = frame % 4
    v_size = 1.0 / impact_v  = 1.0 / 4 = 0.25
    v      = v_size * (frame / impact_v)               // row = frame / 4

// rewrite the 4 vertex UVs to bound the cell rect
lverts[0].tu = u;             lverts[0].tv = v;
lverts[1].tu = u;             lverts[1].tv = v + v_size;
lverts[2].tu = u + u_size;    lverts[2].tv = v;
lverts[3].tu = u + u_size;    lverts[3].tv = v + v_size;
```

(There is a **commented-out alternative** vertex order at `effectcomp.cpp:
277-287` — swaps which vertex gets which corner. The live order is the
first block, `:265-275`.)

### 8a. Particle-phase cell layout (atlas 64×64, 8 cols × 2 rows)

```
       col 0   col 1   col 2   col 3   col 4   col 5   col 6   col 7
       u=0     u=.125  u=.25   u=.375  u=.50   u=.625  u=.75   u=.875
row 0  ┌──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┐
v=0    │ f0   │ f1   │ f2   │ f3   │ f4   │ f5   │ f6   │ f7   │  ← particle frames 0..7
       │ 8x32 │ 8x32 │ 8x32 │ 8x32 │ 8x32 │ 8x32 │ 8x32 │ 8x32 │     (each 8x32 px)
       ├──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┤
row 1  │  (impact phase uses 4x4 layout on the bottom half — see 8b)
v=.5   │
       └─────────────────────────────────────────────────────────┘
                                                                v=1
```

**Particle frame advance:** `frame += 0.5/tick`. Range `[particle_begin=0,
particle_end=7]`. At `frame >= 7`, `frame -= (7-0) = 7` → **wraps to 0**
(loops). At `frame_inc=0.5`, the integer frame advances every 2 ticks; full
8-cell loop = 16 ticks. Frames advance as int(frame), so 0,0,1,1,2,2,...

**Per-instance phase offset:** none — every meteor starts at `frame=0` at
Create (`effectcomp.cpp:101`). So all live meteors share the same anim
phase (no de-sync). If retail wanted per-meteor phase variation, it could
re-randomize `instance.frame` at Create — the snapshot does not.

### 8b. Impact-phase cell layout (atlas 64×64, 4 cols × 4 rows)

For impact: `frame ∈ [8, 15]`, `u = (frame % 4)`, `v_row = (frame / 4)`:
- frame 8 → u=0, row=2 (frame/4 = 2)
- frame 9 → u=1, row=2
- frame 10 → u=2, row=2
- frame 11 → u=3, row=2
- frame 12 → u=0, row=3
- frame 13 → u=1, row=3
- frame 14 → u=2, row=3
- frame 15 → u=3, row=3

```
       col 0   col 1   col 2   col 3
       u=0     u=.25   u=.50   u=.75
row 0  │  (particle 8x32 cells overlap here; row 0 = particle frames 0..3)
v=0    │
       ├──────────────────────────────┤
row 1  │  (particle row 0 = frames 4..7 — particle is 8 wide × 2 tall on the same 64×64)
v=.25  │
       ├──────┬──────┬──────┬──────┤
row 2  │ f8   │ f9   │ f10  │ f11  │   ← impact frames 8..11 (each 16×16 px)
v=.5   │ 16x16│ 16x16│ 16x16│ 16x16│
       ├──────┼──────┼──────┼──────┤
row 3  │ f12  │ f13  │ f14  │ f15  │   ← impact frames 12..15
v=.75  │ 16x16│ 16x16│ 16x16│ 16x16│
       └──────┴──────┴──────┴──────┘
                                  v=1
```

So **particle uses the TOP half of the atlas** (8×32 strips, rows 0), and
**impact uses the BOTTOM half** (4×4 grid of 16×16 cells, rows 2-3). Row
1 (v ∈ [0.25, 0.5]) is not used by either phase under these params —
suggesting either the atlas reserves it for unused intermediate frames OR
the particle's 8×32 cells actually span both rows 0 and 1 of the
4×4-impact-grid (i.e. the particle's `v=0..0.5` covers what the impact
treats as `v=0..0.5`). The asset's texture[0] visually confirms the
horizontal streak occupies the upper portion and impact occupies the lower
portion.

**Impact frame advance:** `frame += 0.7/tick`. Range `[8, 15]`. The impact
plays linearly from frame 8 to 15 over `(15-8)/0.7 ≈ 10 ticks`, then the
instance dies (`is_used=false`, `effectcomp.cpp:156-157`). **No wrap** for
impact.

### 8c. Empirical verification

The dumped texture[0] (`i3d_dump_all/comet/texture_00_frame_00.png`,
64×64) visually shows:
- Top half (rows of 8×32 strips) — a yellow/orange horizontal streak
  with a glow ball center: consistent with **8 meteor-streak frames** at
  varying intensity/length (the particle animation).
- Bottom half (4×4 16×16 cells) — a cratered/smoky impact image:
  consistent with **8 frames of explosion** (rows 2 and 3), playing once.

---

## 9. Associated light

- **NO dynamic light in the snapshot source.** The animator emits no light:
  there is no `AddLight`/`CreateLight`/`SLightInfo` call anywhere in
  `TMeteorStormAnimator` (`effect_old.cpp:6367-6502`) or `TStormAnimator`
  (`effectcomp.cpp:38-329`).
- **There IS a `LIGHT` line in spell.def** —
  `LIGHT COLOR 255, 180, 0 INT 180 MULT 20` (`spell.def:259`, the shipped
  data file). This is an **orange-yellow point light** consistent with the
  meteor's fire theme.
- **The pre-release spell parser discards `LIGHT`** — see sister effect I21
  forensics §9: `TSpellData::Load` only handles known tags and routes
  `LIGHT` into the "skipping unknown tag" branch (`spell.cpp:119-157`).
  Whether shipped retail honored `LIGHT` is unknown; the snapshot does not.
- **Reconstruction implication:** an orange-yellow dynamic light keyed to
  each meteor's position is **clearly the intended retail behavior** —
  RGB `(255, 180, 0)`, INT 180, MULT 20. But it is **not present in the
  snapshot code path**. Flag as an intended-but-unimplemented retail
  feature. If the reconstruction adds it, attach one light per active
  meteor (pinned to `pos`, flickering/intensity-pulsed to the impact frame
  if matching ground-truth requires it).

---

## 10. Color

- **Source:** the **authored textures + materials of `Comet.I3D`**. The
  meteor sprite + impact frames carry their color in texture[0] (the 64×64
  atlas). The animator writes **no** per-vertex color
  (`effectcomp.cpp:265-275` only writes `.tu`/`.tv`, never `.color`/
  `.diffuse`), so the per-vertex diffuse remains at whatever the I3D
  authored (default white). Under MODULATE blend, white × texel = texel
  unchanged.
- **Exact values:** the visible color is NOT a code constant — it's in the
  PNG/embedded texture data. From the dump, texture[0]'s meteor cells
  appear **warm yellow/orange** (saturated, classic fire-glow), and the
  impact cells appear smoky/yellow-brown. The 0–255 RGB values are inside
  the texture file — read empirically from
  `i3d_dump_all/comet/texture_00_frame_00.png`.
- **Spell-level light color** (separate from sprite, intended-but-not-wired
  in snapshot): RGB `(255, 180, 0)` 0–255 (`spell.def:259`).
- **Expected visual:** warm fire-orange / yellow — saturated, glowing.
  Magic family "warm" signature ([feedback-vfx-color-health-signal]).
  Pale/gray/desaturated at reconstruction time = bug; check:
  1. The `Comet.I3D` textures weren't loaded → procedural stand-in (the
     #1 failure mode here).
  2. Black-key chroma miss eating the orange edges.
  3. Wrong blend (Additive would brighten the sprite further but still
     warm; flipping to Decal/AlphaBlend with a pre-multiplied texture
     could desaturate).
- **Normalization / boosts:** none — no `NormalizeColors`/brightness math
  in this effect.

---

## 11. Audio coupling

- **`PLAY("meteor explode")`** — fired once per particle, on the first
  frame of the impact phase, gated by `!explosion_sounded`
  (`effectcomp.cpp:236`). Trigger: per-meteor impact (NOT a single sound
  at storm start; if 20 meteors land, you hear 20 impact sounds spread
  over the timeline). The string `"meteor explode"` is also XREF'd in the
  retail binary (`recon/ghidra/_data.txt:108007`, `s_meteor_explode_005e1624`
  XREF from `0x50aa91`), confirming the sound name is shipped.
- **`PLAY("meteor fall")` — commented out** (`effectcomp.cpp:111`). Author
  considered a launch sound but disabled it in the snapshot. The shipped
  spell.def has `PLAY "meteorstorm"` (`spell.def:269,282`) as the cast
  sound played by the spell system (not the animator), which is separate
  from `"meteor fall"`.
- **Shipped cast sound:** `PLAY "meteorstorm"` (`spell.def:269,282`) —
  played via the spell-system audio dispatcher, NOT the animator. The
  audio phase should wire this.

---

## 12. Triggers & in-game appearance

- **Spawned by (snapshot):** the **Meteor Storm** spell. `legacy/spell.def:
  120-129` (talismans `"escp"`) → effect builder `"meteorstorm"` →
  `DEFINE_BUILDER("MeteorStorm", TMeteorStormEffect)` (`effect_old.cpp:6353`)
  + `REGISTER_3DANIMATOR("MeteorStorm", TMeteorStormAnimator)` (`:6365`).
  Snapshot tagline `// doesn't work` (`legacy/spell.def:119`).
- **Spawned by (shipped retail):** the **Meteor Storm** spell. `data/Resources/
  spell.def:251-289` (talismans `"JI"` for basic, `"MD"` for priest) →
  effect builder `"StrikeEffect"` (NOT `"MeteorStorm"`) → routes to a
  retail `TStrikeEffect` class that reads `IMAGERY "magic\Comet.i3d"` from
  the CONTROLDATA `"Strike"` block. **The shipped binary contains no
  `"MeteorStorm"` string** — see §2.1.
- **Spell-cast pipeline:** `TSpellManager::CastByName` →
  `TSpell::Timer` builds the `SObjectDef` at caster pos + hand source +
  `HEIGHT=100` fallback, `facing` from invoker, `objtype =
  FindObjType(variant->effect)` (`spell.cpp:431-457`). The `variant->effect`
  is `"meteorstorm"` in snapshot, `"StrikeEffect"` in retail.
- **Where to see it in the original game:** cast Meteor Storm (talismans
  `JI` for basic Meteor Storm, `MD` for Priest Meteor Storm) at a target
  area. Capture rig — a target dummy in an open arena (e.g. dungeon
  courtyard from AGENT_GUIDE §4.2.1.7 BG selection), cast from ~300 wu
  away. Reference frames `images/vfx/28_MeteorStorm/01..10.png`. The
  storm runs ~100 ticks of ramp-up + ~50-100 ticks of wind-down at 24 Hz
  — roughly 6-9 s on screen.
- **Vestigial?** No — live spell in both snapshot and retail. But the
  snapshot's `TMeteorStormAnimator` itself **may be vestigial in shipped**
  (no string XREF, replaced by `TStrikeEffect` path) — see §13.

---

## 13. Gaps & uncertainties

1. **Shipped MeteorStorm uses a different code path than the snapshot.**
   Retail spell.def routes Meteor Storm to `"StrikeEffect"` (not
   `"MeteorStorm"`), and the retail binary contains NO `"MeteorStorm"`
   string XREF. The snapshot `TMeteorStormAnimator` is likely **dead code
   in shipped**. There is NO recoverable retail body for whatever
   `TStrikeEffect` does. *Reasonable guess*: `TStrikeEffect` is a generic
   timed-area-burst effect that loads spell-data IMAGERY (here `Comet.I3D`)
   and renders it as a multi-hit strike per the CONTROLDATA: `RADIUS 265`,
   `HITS 4`/`12`, `PATTERN "random"`, `DURATION 93`, `RANGEDAMAGE 150
   30`/`150 25`, `WAIT 48`. Whether it uses the snapshot's `TStormAnimator`
   helper internally is unknown — but the retail's surviving `TStormAnimator`
   candidate (`cls_0x5ab460`) is bound to Tornado/Sandswirl/Quicksand, not
   Meteor Storm. The reconstruction agent must either (a) reproduce the
   snapshot animator faithfully and ignore the shipped divergence, OR (b)
   reverse-engineer/invent `TStrikeEffect` against the in-game reference.
   This is the **single biggest forensics gap** for this effect.

2. **Asset changed between snapshot and shipped.** Snapshot
   `legacy/Imagery/Magic/Meteor.I3D` (138 KB, 1 sub-object `photon01`) was
   replaced with shipped `data/imagery.rvi:Imagery/Magic/comet.I3D` (44 KB,
   5 populated sub-objects: `#comet`, `#sparks`, `#$head`, `#trail`,
   `shock01`). Files are not byte-identical (different MD5s) and the
   sub-object layout is completely different. The snapshot animator's
   `GetObject(0)` would address the single `photon01` quad in
   `Meteor.I3D`; against `Comet.I3D`, it addresses `#comet` (a 20×20 XZ
   quad). Both ARE a quad-with-meteor-atlas, so the snapshot logic CAN run
   against the shipped asset, but the 4 unused shipped sub-objects
   (`#sparks`, `#$head`, `#trail`, `shock01`) are wasted geometry the
   snapshot code never touches.

3. **Unused sub-objects in shipped `Comet.I3D`.** If the shipped
   `TStrikeEffect` consumes them (e.g. spawns ground-flat `#$head` discs
   on impact, or `shock01` burst quads, or `#trail` motion trails), the
   reconstruction will be visually wrong without them. Reasonable guess
   (from the reference frames): the 212×212 `#$head` is a ground-impact
   glow disc, `#sparks` is an upward-rising spark sprite, `#trail` is a
   motion-blur trail behind the meteor, and `shock01` is the bright burst
   flash at the impact moment. But this is **speculation** — neither the
   snapshot code nor any recovered retail body confirms it.

4. **Whether snapshot's `TStormAnimator::Render` is ungated.** The snapshot
   helper runs whenever `Animate` is called, with no 24-Hz gate on the
   `frame` advance (`effectcomp.cpp:149,159`). At a fixed 24 Hz tick,
   particle anim = 16 ticks/loop = 0.67 s; impact anim = 10 ticks = 0.42 s.
   At higher framerate (ungated), it'd play faster. Per
   [feedback-framerate-independent-anim], the reconstruction should
   convert per-tick increments to per-second (×24 Hz) and use delta-time.
   The snapshot increments (`particle_frame_inc=0.5`, `impact_frame_inc=
   0.7`, `gravity=0.37 wu/tick²`) translate to per-second rates of `12 fr/s`,
   `16.8 fr/s`, and `212 wu/s²`.

5. **Damage type mismatch.** Snapshot helper hardcodes `DAMAGE_FIRE`
   (`effectcomp.cpp:235`); shipped `spell.def:256` declares
   `DAMAGETYPE DT_BURN` (which is value 7, `spell.def:40`). DT_BURN and
   DAMAGE_FIRE may be the same enum or different (engine has both names);
   record for gameplay phase but visually irrelevant.

6. **Snapshot damage range mismatch.** Snapshot uses `radius=50`
   (`effectcomp.cpp:235`); shipped Strike `RANGEDAMAGE 150 30` declares
   `radius=150` (and damage 30 / 25). Snapshot damage 5..15 vs shipped 30
   (or `mindamage..maxdamage` 74..94 from VARIANT). These are NOT the same
   spell tuning. The shipped values drive the retail `TStrikeEffect`
   path; the snapshot values drove the dead `TMeteorStormAnimator`.

7. **Associated light is retail-data-only.** `spell.def:259` declares
   `LIGHT COLOR 255,180,0 INT 180 MULT 20`. The snapshot's spell parser
   discards `LIGHT`. Whether shipped honors it is unverified; the
   reference frames show warm fire glow on the ground around impacts,
   consistent with light. Reasonable for the reconstruction to add it.

8. **`PLAY("meteor fall")` was disabled in snapshot.** Whether shipped
   has a per-meteor launch sound is unknown. Only `"meteor explode"` has
   a confirmed retail XREF; the cast sound `"meteorstorm"` is on the
   spell side.

9. **`obj->flags` flag combination.** `OBJ3D_MATRIX | OBJ3D_ABSPOS |
   OBJ3D_VERTS` is set every render. `OBJ3D_VERTS` (`3dimage.h:118`)
   overrides the imagery-default vertex list. The snapshot `Render` rewrites
   only the 4 vertices' UVs, leaving positions intact — so `OBJ3D_VERTS`
   may not strictly be necessary (the matrix transforms the existing
   vertex positions). Recorded as a subtlety; the reconstruction can
   safely set the same flag combination.

10. **`obj->lverts[0..3]` vertex order**. The snapshot uses one of two
    commented orderings (`:265-275` live, `:277-287` commented out). The
    live order pairs `lverts[0]` with `(u, v)` (UL), `lverts[1]` with
    `(u, v+v_size)` (LL), `lverts[2]` with `(u+u_size, v)` (UR), `lverts[3]`
    with `(u+u_size, v+v_size)` (LR). This is consistent with a `0=UL,
    1=LL, 2=UR, 3=LR` quad-vertex layout — verify against the asset's
    actual vertex order for `#comet` (`subobj_00_#comet.obj` lists verts
    `(10,0,9.99)`, `(-10,0,9.99)`, `(-10,0,-9.99)`, `(10,0,-9.99)`,
    i.e. quadrant order `+x+z, -x+z, -x-z, +x-z`. With the asset's UVs
    `(0.99, 0.99)`, `(0.49, 0.99)`, `(0.49, 0.49)`, `(0.99, 0.49)` —
    the asset's `lverts[0]` had UV `(0.99, 0.99)` = LR. So the snapshot's
    runtime UV-rewrite ABANDONS the asset's UV ordering and substitutes
    `0=UL, 1=LL, 2=UR, 3=LR`. The reconstruction should follow the
    snapshot's rewrite, not the asset's authored UVs.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\Comet.I3D (44 428 B, MD5 978a4868ffc31d0618af6207063ec3c7). Address
      GetObject(0) = #comet (20×20 XZ quad). Sub-objects 2/4/5/7 (#sparks/#$head/#trail/
      shock01) are NOT used by the snapshot Render path — flag as gap if visually needed. (§4)
- [ ] Bind texture[0] (64×64 atlas) as the rendered texture for #comet (via material[0],
      which has emissive=(1,1,1) — the sprite renders self-lit white-emissive). (§4, §10)
- [ ] Spawn as fixed-duration TEffect. Empty Initialize / TEffect::Pulse passthrough on
      the parent object class. Anchor at caster pos via TSpell::Timer pipeline. (§5, §6)
- [ ] Wrapper Initialize seeds SStormParams with the exact constants from §3b:
      particles=0 (ramps via tracker); tex_u=tex_v=64; particle_u=8, particle_v=2,
      particle_begin=0, particle_end=7, particle_frame_inc=0.5;
      impact_u=4, impact_v=4, impact_begin=8, impact_end=15, impact_frame_inc=0.7;
      gravity=0.37; velocity=(-10, 0, -15); pos=cast_pos+(120,-20,300);
      pos_spread=(100, 100, 0); ratio=random(5,20)/10;
      particle_scale=(0.5, 1.5, 1.0)*ratio; impact_scale=(1.0, 1.0, 1.0)*ratio. (§3, §5)
- [ ] Per-tick wrapper ramp (every 10 ticks): if (tracker<20 && ticks<=100) tracker++;
      if (ticks>100 && tracker!=0) tracker--. Write tracker→params.particles. (§6.1)
- [ ] Wrapper kill condition: meteor_storm.IsDone() && ticks >= 100. (§6.1)
- [ ] Per-particle Create: pos = params.pos + random(±pos_spread); vel.z = base × random(100,150)/100
      (per-instance jitter on vz only); part_scl.x and expl_scl.x ×= random(50,150)/100
      per-instance; frame = 0; is_particle=true; explosion_sounded=false. (§6.3)
- [ ] Per-particle Animate (PARTICLE phase): new_pos = pos + vel; if walk-height >= new_pos.z:
      transition to IMPACT (frame=8, is_particle=false); else: pos=new_pos, vel.z -= 0.37,
      frame += 0.5, wrap [0..7]. (§6.2)
- [ ] Per-particle Animate (IMPACT phase): if int(frame) >= 15: used=false; else frame += 0.7. (§6.2)
- [ ] PARTICLE-phase Render: Clear matrix; RotateZ(-45°), RotateX(0°), RotateY(-90°);
      Scale (part_scl.x, 1.5*ratio, 1.0*ratio); Translate(pos.x, pos.y, FIX_Z_VALUE(pos.z));
      Rewrite UVs to atlas cell (u_size=1/8, u=u_size*(int(frame)%8), v=0, v_size=1/2),
      vertex order 0=UL/1=LL/2=UR/3=LR; flags = OBJ3D_MATRIX|OBJ3D_ABSPOS|OBJ3D_VERTS. (§7.2, §8a)
- [ ] IMPACT-phase Render: Clear matrix; RotateZ(0°), RotateX(+90°), RotateY(0°);
      uniform Scale (expl_scl.x, expl_scl.x, expl_scl.x); Translate(pos.x+25, pos.y+35,
      FIX_Z_VALUE(pos.z)); Rewrite UVs (u_size=1/4, u=u_size*(int(frame)%4), v_size=1/4,
      v=v_size*(int(frame)/4)). (§7.2, §8b)
- [ ] Damage + sound on first IMPACT tick (gated by !explosion_sounded): pos.x +=25,
      pos.y +=35, pos.z = walk-height; DamageCharactersInRange(invoker, pos, 50, 5, 15,
      DAMAGE_FIRE); PLAY("meteor explode"); explosion_sounded=true. (§6.4, §11)
- [ ] Blend = Alpha modulated (SetBlendState: MODULATE/SRC_ALPHA/INV_SRC_ALPHA, depth-test on,
      depth-write off — TestNoWrite); lit-mode = Self-lit (material 0 emissive=(1,1,1));
      do NOT write per-vertex color (vertex diffuse stays white identity). (§7, §10)
- [ ] RefreshZBuffer per active particle: rect = (64 × max(particle_scale.x, impact_scale.x) × 1.5,
      64 × max(particle_scale.y, impact_scale.y) × 1.5); RestoreZ at WorldToScreen(pos). (§7.4)
- [ ] Color: warm yellow/orange from texture[0]; verify saturated warm (pale/gray ⇒ stand-in /
      chroma-key / wrong blend). (§10)
- [ ] Texture animation: per-vertex UV-overwrite (NOT framehtexs flipbook, NOT UV-scroll);
      particle uses 8×2 cell grid, impact uses 4×4 cell grid, same atlas; no per-instance
      phase offset. (§8)
- [ ] Associated dynamic light: NOT present in snapshot. OPTIONAL retail addition from
      spell.def:259 — RGB(255, 180, 0), INT 180, MULT 20 — one light per active meteor
      pinned to pos, pulsed/attached to impact frame. Flag if added vs. snapshot-faithful. (§9, §13.7)
- [ ] Audio: code-driven PLAY("meteor explode") on impact (§11). Spell-side
      PLAY "meteorstorm" (cast sound) is wired by the spell system, not this animator.
- [ ] Do NOT use sub-objects #sparks (2), #$head (4), #trail (5), shock01 (7) unless
      escalating the retail-shipped-divergence gap (§13). The snapshot binds only #comet (0).
- [ ] Do NOT route the spell through "StrikeEffect" or implement TStrikeEffect — the snapshot
      uses "MeteorStorm" directly. If a retail-shipped faithfulness pass is requested later,
      that becomes a new forensics task (§13.1).
- [ ] Snapshot was ungated; reconstruct as time-based (delta-time) per
      [feedback-framerate-independent-anim]: frame_inc per second = inc_per_tick × 24;
      gravity_per_second² = 0.37 × 24². (§13.4)
```
