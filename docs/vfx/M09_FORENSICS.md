# M09 `TTeleporterAnimator` — Forensics

Per [AGENT_GUIDE.md §3.0](AGENT_GUIDE.md). 7-point forensic write-up for
the Misthaven recall / Teleport spell visual. Authored before any port
code is written.

**Sources of truth**

- Pre-release source: `src/effect_old.cpp:5444-5880` (`TTeleporterEffect`
  + `TTeleporterAnimator`, both bodies intact and uncommented). Per
  AGENT_GUIDE §3.1 this is authoritative for *intent*; retail behaviour
  wins if it diverges.
- Recon decomp: **not extracted**. The "Teleporter" string at
  `recon/classes/_data.txt:107455` is XREF'd from `0x004e6a20` (builder
  for `TTeleporterEffect`) and `0x004e6a40` (builder for
  `TTeleporterAnimator`), but neither function is split out as its own
  `recon/classes/cls_*.cpp` file. INVENTORY M09 already flags this as
  `recon file: not-extracted`. Per AGENT_GUIDE §3.1 we proceed on the
  pre-release source as authoritative for behaviour with this divergence
  recorded; structural facts (builder XREF locations, registered name
  "Teleporter") are recon-confirmed.
- Caller / dispatch wiring: `data/Resources/spell.def` (16 VARIANT
  references to the `"Teleporter"` animator — see §5).
- Asset evidence: `data/imagery.rvi` (zip) ships
  `Imagery/Magic/teleportation.I3D` (105357 bytes — the cylindrical
  glow mesh + texture), `Imagery/Magic/vortex.i3d` (17700 bytes),
  `Imagery/Magic/setvortex.i3d` (3760 bytes), `Imagery/Magic/gvortex.I3D`
  (110394 bytes — Misthaven recall "VortexM" variant uses this),
  `Imagery/Magic/jtele.I3D` + `jteled.I3D` (28523 bytes each — the
  "Priest Teleport" / `MAC` talisman variant). Per-spell-variant
  imagery selection is via the spell.def `EFFECT_NAME` slot which
  resolves to the imagery the spell's animator binds.

---

## 1. Retail constants

Pulled from `src/effect_old.cpp:5444-5880`. Pre-release authoritative;
no retail recon to cross-check; flagged in §7 as gap.

### Effect-side (state machine — `TTeleporterEffect`)

```
TELE_STATE_INIT   = 1
TELE_STATE_OUT    = 2     // grow phase
TELE_STATE_MOVE   = 3     // single-tick payload (the actual character teleport)
TELE_STATE_IN     = 4     // shrink phase
```

```
life increment              = 1 / Pulse() call            (sim-rate)
out_duration                = 50 Pulse ticks              (effect_old.cpp:5538)
in_duration                 = (no explicit gate)          // ends when animator KillThisEffect's on life >= 100
animator self-kill life     = 100 ticks                   (effect_old.cpp:5818)
fade-out (start of OUT)     = 100 alpha target, step  10  (SetFade(100, 10))
fade-in  (start of MOVE)    =   0 alpha target, step -10  (SetFade(0, -10))
```

### Animator-side (visual envelope — `TTeleporterAnimator`)

```
flares           = 5                              // 5 stacked cylinder copies
inst->Face       = -32                            // base facing offset (Initialize line 5768)

per-flare init pos (effect_old.cpp:5770-5779):
  p[0] = ( 0,  20,  45)
  p[1] = (-5,  20,  45)
  p[2] = (+5,  20,  45)
  p[3] = ( 0,  20,  65)
  p[4] = ( 0,  20,  25)
  cylsize[i] = 0.0

iterations_cap    = 50                            // grow phase length (effect_old.cpp:5804)
mid_widening_at   = 25                            // when iterations >= 25, p[1]/p[2] expand
mid_widening_step =  0.5  / tick                  // p[1].x += 0.5, p[2].x -= 0.5
vertical_squeeze  = -1.0f / tick on p[3].z, +1.0f on p[4].z  // converge to mid-height
rotation_step     =  0.1f / tick                  // continuous spin
growx             = 15 / 50 = 0.30                // (computed in Initialize; not used after pre-release WIP)
growy             = growx / 3 = 0.10

per-flare render envelope (effect_old.cpp:5841-5860):
  phase_period   = 100 ticks                      // curticks = ticks % 100
  grow_phase     = curticks  < 50                 // expansion
  shrink_phase   = curticks >= 50                 // mirror back
  effective_t    = curticks  < 50 ? curticks : (50 - (curticks - 50))   // triangle wave, 0..50..0
  per-cyl scl.z  = 10 - 5.0f * (effective_t / 30.0f) - z   // each of 5 cyls slightly shorter than the last
  per-cyl scl.xy = (0.5f * z) + 2.5f * (effective_t / 30.0f)// each of 5 cyls slightly wider than the last
  per-cyl rot.z  = rotation + z * 0.5f            // each cyl offset 0.5 rad from previous for shimmer
  cull           = scl.z > 0.01                   // skip degenerate squashed cyls
```

### Render-state constants

```
blend     = SetBlendState (pre-release default, NOT additive — see effect_old.cpp:5824)
            -- this is the standard Alpha blend, distinct from the additive
            blend most other effects in this file use (SetAddBlendState).
ZBuffer   = RestoreZ rect 100×200 px around screen-projected pos (effect_old.cpp:5869-5880)
            -- the screen-space depth restore for the rotating glow vs world geometry.
obj flags = OBJ3D_ROT1 | OBJ3D_SCL2                     // per-instance Z-rotation + XY/Z scale
imagery   = bound by spell.def variant; ships at
            data/Imagery/Magic/teleportation.I3D (105KB cylinder mesh +
            its glow texture). Misthaven recall ("VortexM" level=0)
            ships at Magic/gvortex.I3D.
```

### Spell-variant level mapping (`Pulse()` lines 5513-5535)

| Spell variant name | `level` | Imagery (per spell.def EFFECT_NAME slot) | Notes |
|--------------------|--------:|------------------------------------------|-------|
| `"Teleport"`       |       1 | `Magic\teleportation.I3D`                | Player-cast level-1 |
| `"Teleport2"`      |       2 | `Magic\teleportation.I3D` (assumed)      | Player-cast level-2 (priest variant uses jtele) |
| `"VortexM"`        |       0 | `Magic\gvortex.I3D`                      | **Misthaven recall** (the user's headline ask) |

Level **only** modifies destination-resolution behaviour in `MOVE`
(level 0/1: nearest enemy within 300wu or random walkable spot 400wu
away; level 2: saved teleport coords); the **visual envelope is
identical** across all three levels.

---

## 2. Spawn shape + emit anchor convention

### Where it originates (the emit anchor)

- **The effect's own `SetPos()` is the visual anchor.** Pre-release Pulse
  always re-pins the effect at the resolved destination (`MOVE` writes
  `SetPos(new_position)` before transitioning to `IN`). For the **OUT**
  phase the effect lives at whatever pos the spell.def caller spawned
  it at — which the engine convention is **caster feet** (the spell
  spawn path attaches the effect to the caster instance at the moment
  of cast invocation).
- For the **IN** phase the effect lives at the destination, again
  pinned to feet (the destination character / caster has just been
  `SetPos`-moved to that point, and the effect was `SetPos`-ed there
  in the MOVE tick).
- So the visual emits at **(x, y, ground_z) of the caster's feet** for
  the OUT phase and **(x, y, ground_z) of the destination** for the IN
  phase. There is no character-bone attachment; the engine treats the
  effect instance as standalone-positionable.

Per AGENT_GUIDE §3.0(2) "many pulse effects emit from the ground
center of the target emitter point": **yes — feet center is the
correct anchor**. The spell-cast-from-rhand convention (spell.cpp:399)
applies to the *cast pose*, not to the teleport burst that engulfs
the caster's body.

### Shape it sweeps

A **vertical glow column** of 5 stacked cylinders rotating around Z,
centred on the caster, growing radially outward from 0 to ~2.5×
diameter and ~10 wu tall over 50 ticks, then mirror-shrinking back.
The "5 cylinders" are vertically displaced (`p[i].z = 45±20`) and
slightly horizontally offset (`p[1].x = -5..-5+0.5*25`,
`p[2].x = 5..5-0.5*25`) so the column visibly *thickens* in its
middle as the iteration counter passes 25.

The rotation (`rotation += 0.1f / tick`) gives ~95 °/s spin —
**continuous and smooth**, not a snap. With 5 cylinders at
`rot.z = rotation + z*0.5f`, each cylinder is offset by ~29° from its
neighbour, producing a shimmer-stripe pattern as the textured cylinder
walls cross each other.

### Vertical extent

`p[i].z = 25..65` (i.e. centred around z=45 with a ±20 wu spread above
feet). After 50 iterations the `p[3]/p[4]` z-axis squeeze brings them
both to z≈45 (mid-converged). So the column is ~40 wu tall initially
and squeezes to ~0 tall at the apex of the OUT phase — visually, it
contracts vertically as it expands radially, then mirrors back during
shrink phase.

### Per-flare envelope (triangle-wave)

```
effective_t       = ticks_in_phase < 50 ? ticks_in_phase
                                        : (50 - (ticks_in_phase - 50))
per-cyl height_z  = 10 - 5 * (effective_t / 30) - z_index
per-cyl radius    = 0.5 * z_index + 2.5 * (effective_t / 30)
per-cyl spin      = rotation + z_index * 0.5
```

At `effective_t = 30` (peak): height_z = 5 - z_index → 5,4,3,2,1 wu;
radius = 0.5*z + 2.5 → 2.5, 3.0, 3.5, 4.0, 4.5 wu. These are the
**scale factors** applied to the underlying I3D cylinder mesh which is
some baseline size (likely ~20-40 wu diameter judging from the asset
size). The visible column at peak is therefore roughly 40-100 wu wide
and 5-20 wu tall — a *low, wide rotating shimmer disc*, not a tall
spire.

Beyond `effective_t = 30` (ticks 30-50 and the mirror 50-70), the
formula keeps growing radius (to ~6.6 at ticks=50) while heights
continue shrinking past zero — the `scl.z > 0.01` cull on line 5846
hides those degenerate flares so the column gracefully *fades out* by
losing flares one at a time as their height collapses past zero.

---

## 3. Integration

### Tick rate

- Animator `Animate()` (effect_old.cpp:5787-5820) runs every render
  frame in pre-release. Per AGENT_GUIDE §3.1.1 the project convention
  is to **gate animation-driven kinematics to the 24 Hz sim tick** so
  modern 60 fps doesn't run the envelope 2.5× too fast (same fix
  applied in F01 / H03 / M05 / L02 / F03).
- Effect `Pulse()` (effect_old.cpp:5508-5754) runs at sim rate
  already (engine convention — `TEffect::Pulse` is on the sim-tick
  cadence).

### Lifecycle

```
spawn (by spell)
  → INIT     [1 tick]              // resolve level from spell variant
  → OUT      [50 ticks ≈ 2.08 s]   // fade caster to white (SetFade 100/10)
                                   // + rotating column GROWS
  → MOVE     [1 tick]              // PAYLOAD: caster->SetPos(new_position)
                                   // + fade reset (SetFade 0/-10 → caster
                                   // becomes invisible during transit and
                                   // returns to opaque over ~10 ticks)
                                   // + effect SetPos to new_position
  → IN       [≤ 49 ticks]          // rotating column SHRINKS at destination
                                   // until animator life >= 100 → KillThisEffect
```

Total visible duration: ~100 sim ticks ≈ **4.17 seconds** at 24 Hz.

### Payload coupling

The actual character-teleport `SetPos()` fires in the **MOVE** tick
(effect_old.cpp:5625, 5658, 5692, 5724). This is the single payload
frame; the visual half is split symmetrically around it (50 OUT, 1
MOVE, ≤49 IN). The VFX primitive **must expose a payload hook** so
gameflow can fire its `caster->SetPos(destination)` at the same instant
the effect transitions OUT → IN.

For the harness port we do **not** wire the actual SetPos — per the
task brief "Don't add character teleport semantics (position move).
The VFX is the visual; the engine wires the position change
separately." We do expose a callback that gameflow will hook later
(or, equivalently, gameflow can observe state transitions on the
effect).

### Frame-rate independence

`Pulse()` is sim-tick-gated by the engine. The animator's `Animate`
becomes sim-tick-gated by us via `sim_accum_ms_` accumulator (the
established pattern). Net effect: the 100-tick lifetime is **4.17 s
real time on any framerate**.

---

## 4. Render passes

| Property              | Pre-release value                                  | Port mapping |
|-----------------------|----------------------------------------------------|--------------|
| Pipeline              | I3D mesh draw with `RenderObject`                  | **FB** (billboard) — we approximate the cylinder geometry with stacked screen-aligned glow billboards rather than load the I3D mesh; rationale in §7. |
| Blend                 | `SetBlendState()` — standard D3D Alpha (NOT additive — pre-release explicitly chose alpha vs the surrounding effects' `SetAddBlendState`) | `EFxBlend::AdditiveStraight` for the glow appearance. **Divergence** from pre-release Alpha; flagged here. AdditiveStraight reads better against the harness's dark background and is consistent with other "spell glow" effects in this port (L02 halo, F03 fire). The pre-release Alpha was likely chosen for compatibility with the original I3D's pre-multiplied texture against the in-game scene lighting; we use a procedural premultiplied texture so AdditiveStraight is the correct pair (matches L02). |
| Lighting              | `LitFlat` (default I3D mesh lighting was scene-affected — pre-release didn't disable it) | `EFxLightMode::Unlit` (the glow IS the light source; same choice as L02 / F03). |
| Depth mode            | `RestoreZ` 100×200 rect — screen-space depth-test against geometry, no write | `EFxDepthMode::TestNoWrite` (standard transparent path, matches H03/L02/F03). |
| Orientation           | I3D mesh in world space, rotated around Z by the animator | `EFxBillboardOrientation::ScreenAligned`. **The pre-release rotation around Z is invisible from a fixed iso camera viewing a cylindrical billboard** — the cylinder's silhouette is the same regardless of spin. The shimmer effect that the spin produces in pre-release comes from the *texture* on the cylinder walls scrolling as the cylinder rotates; we approximate that with per-billboard rotation jitter and texture-uv animation if needed. ScreenAligned correctly preserves the "tall vertical glow" intent. |
| Color                 | Texture-driven (from teleportation.I3D / gvortex.I3D); no per-vertex tint that's evident | Procedural texture, tint = blue-violet for VortexM / cyan-white for Teleport (per spell.def `LIGHT COLOR 50, 50, 255` at line 809 — *the spell light color is blue, so the visual reads blue*). |

### Per-spell-variant tint (from spell.def)

| Spell             | `LIGHT COLOR` (spell.def) | Effect tint we use |
|-------------------|---------------------------|---------------------|
| `Teleport`        | `50, 50, 255` (deep blue) | cool blue-violet    |
| `Teleport2`       | (Priest variant — light data on its own SPELL block) | (Phase B port covers Teleport variant; Misthaven recall is VortexM) |
| `VortexM` (Misthaven recall) | (Vortex SPELL block at spell.def:1471 area) | cool blue-violet — same family |

We ship a **single cool-blue-violet tint** for the v1 port. The
per-variant color can hook in later by parameter when the in-game
spell.def parse drives it.

---

## 5. Callers (spawn sites)

`data/Resources/spell.def` references the `"Teleporter"` animator name
**16 times** across 16 distinct VARIANT entries:

| Line  | VARIANT                      | Spell block        | Notes                       |
|-------|------------------------------|--------------------|-----------------------------|
| 814   | `"Teleport"`                 | `SPELL "Teleport"` | Player level-1, cyan tint   |
| 820   | `"Priest Teleport"`          | `SPELL "Teleport"` | Player level-1 priest (MAC talisman) |
| 1471  | `"VortexM"`                  | (Vortex spells)    | **Misthaven recall** — user's headline ask |
| 1528  | `"SummonArakna"`             | (Summon)           | Monster summoning visual    |
| 1551  | `"SummonArtikulid"`          | (Summon)           | Monster summoning visual    |
| 1574  | `"SummonRecluse"`            | (Summon)           | Monster summoning visual    |
| 1597  | `"SummonIssathi"`            | (Summon)           | Monster summoning visual    |
| 1620  | `"SummonLurker"`             | (Summon)           | Monster summoning visual    |
| 1644  | `"SummonDark Revenant"`      | (Summon)           | Monster summoning visual    |
| 1667  | `"SummonDark Ogrok"`         | (Summon)           | Monster summoning visual    |
| 1689  | `"SummonDark Ogrok "` (trailing space — distinct SKU) | (Summon) | Monster summoning visual |
| 1711  | `"SummonOgrok"`              | (Summon)           | Monster summoning visual    |
| 1734  | `"SummonOgrok "`             | (Summon)           | Monster summoning visual    |
| 1758  | `"SummonSkeleton"`           | (Summon)           | Monster summoning visual    |
| 1781  | `"SummonZombie"`             | (Summon)           | Monster summoning visual    |
| 1804  | `"SummonWraith"`             | (Summon)           | Monster summoning visual    |

**Takeaway**: the Teleporter animator is heavily reused for both *self-
teleport recall* (Misthaven) and *enemy summoning* (12+ summon spell
variants). The visual primitive is shared; the *payload* (move caster
vs. spawn summoned monster) is what differs per spell. The VFX primitive
we ship covers all 16 callers — gameflow / spell wiring chooses what
the MOVE-tick payload does.

The pre-release `Pulse()` body distinguishes `Teleport` / `Teleport2`
/ `VortexM` via the `level` variable; summon variants would resolve a
spawned-monster instead of a teleport-destination during MOVE — that
hook lives in the spell.cpp dispatch, not in the effect itself.

The original brief specifically calls out **Misthaven teleport** — that
maps to **spell.def line 1471 `VortexM`**. The visual is identical to
the player Teleport; the difference is the payload (recall to the
*saved teleport position* stored on the character at level 2 / level 0)
+ the bound imagery (`gvortex.I3D` instead of `teleportation.I3D`).

---

## 6. Rig category

**Confirmed: `EVfxPreviewStyle::CharacterCast` + `SVfxAnchor{ CharacterRoot }`** per
the prior agent's CHARACTER_RIG_FORENSICS.md §6 mapping.

- **CharacterCast** style: rig spawns Locke (default rig character),
  cycles a `cast*` / `magic*` / `invoke*` animation on loop. The
  spell.def Teleport block sets `ANIMATION "invoke4"` (line 807), which
  the rig's `cast*`-family scan will pick up if Locke has it.
- **`CharacterRoot` anchor**: the teleport visual originates at the
  caster's feet (§2 analysis). `CharacterRoot` returns `inst->GetPos()`
  which is the character's grounded position. **Correct anchor.**
- **No hand bone needed** for the v1 port. Pre-release source doesn't
  do a hand-flourish sub-effect; the entire visual is the
  caster-encompassing column. CHARACTER_RIG_FORENSICS §6 mentions an
  *optional* hand bone for a sub-effect — that's a future polish, not
  v1.

The rig is the right test rig and is already in place; no rig-side
changes needed.

### Re-trigger cadence

`CharacterCast` re-triggers via `RetriggerInterval()` in
`src/vfxtest.cpp:619` — default ~3 s (`SpellGround`-family cadence).
The teleport's natural lifetime is 100 ticks ≈ 4.17 s, so the harness
should let it complete before re-firing. Either:
- bump the retrigger interval ≥ 4.5 s for this entry, OR
- let the cycle re-fire mid-flight (which destroys + respawns the
  effect — slightly off but visually still legible: the column resets
  to OUT phase).

We pick option (1) — set `inactive_ttl_pulses` / a per-effect
override if available, else accept the ~3 s retrigger and let the
restart override mid-flight (the visual is dramatic enough that a
restart reads as "another teleport" rather than a glitch).

---

## 7. Gaps / unknowns

### 7.1 No retail recon body

INVENTORY M09 flags `recon file: not-extracted`. The two builder XREFs
at `0x004e6a20` (effect) and `0x004e6a40` (animator) are visible in
the global string table but neither function is split out as its own
`recon/classes/cls_*.cpp`. Per AGENT_GUIDE §3.1 we accept pre-release
source as authoritative for behavioural detail; we do **not** have
retail-side cross-checks for:

- The exact triangle-wave envelope clamps (could be different).
- Whether retail uses the same 100-tick total lifetime (could be
  shorter / longer post-WIP).
- Whether retail Alpha vs. Additive blend choice changed.
- Per-spell-variant tint overrides (pre-release passes raw texture
  color; retail might multiply by spell `LIGHT COLOR`).

Tracked as **M09a** (post-Ghidra-rerun cross-check) — same pattern as
B01a / M05a. Ship the pre-release-faithful port now; reconcile when
Ghidra splits this animator out.

### 7.2 I3D mesh load vs. procedural billboards

Pre-release uses `RenderObject(GetObject(1))` to draw a real cylinder
mesh from the bound I3D (`teleportation.I3D` for Teleport variants,
`gvortex.I3D` for Misthaven recall). The asset ships in
`data/imagery.rvi` and the in-game spawn path will load it once spell
dispatch is wired (out of scope for this row — gameflow's job).

For the **VFX primitive** the brief asks for, we **do not load the I3D
mesh**. Reasons:

- The renderer's mesh-submit path (`SubmitMesh`) requires registered
  sokol meshes; registering an I3D's geometry inside an FB effect is a
  big lift (would need imagery-loaded mesh extraction, similar to
  `--test=char3d`'s 100+ lines of mesh-walk code).
- The visual *intent* — a "rotating vertical glow column engulfing
  the caster" — is faithfully expressed by 5 stacked screen-aligned
  glow billboards with the same triangle-wave envelope. Same pattern
  as L02 halo (procedural ring texture) and F03 fire (procedural
  flame patch texture).
- The Phase B port is the **visual primitive** for gameflow to call;
  when the full retail mesh path lands (alongside in-game spell
  dispatch and other I3D-mesh effects per AGENT_GUIDE §3.5 "direct
  retail mesh port" track) we can swap the implementation behind the
  same effect class without changing callers.

**Decision**: ship procedural billboard stack now; flag I3D-mesh
upgrade as **M09b** for when the mesh-load infrastructure is ready
(would land alongside M07 photon, M06 spellblock, X09 shield, and
other glow-mesh effects). Preserve the pre-release body in
`src/effect_old.cpp` (already there — no `#if 0` wrapping needed).

### 7.3 The grow/shrink envelope phase wrap

Pre-release `ticks % 100` (effect_old.cpp:5838) means the visual
envelope is **periodic with period 100**, but the effect lifetime is
also 100 ticks (animator self-kills at life>=100, effect_old.cpp:5818).
So the visual only ever sees one full triangle wave cycle before the
effect dies. The `% 100` is defensive code for a "hypothetical longer
lifetime"; we honour it but it doesn't change visible behaviour.

### 7.4 Pre-release `growx`/`growy` are computed but unused

`Initialize` computes `growx = 15.0/50` and `growy = growx/3`
(effect_old.cpp:5783-5784) but **neither field is referenced** in
`Animate()` or `Render()`. Looks like a WIP holdover from an earlier
envelope formulation that got replaced by the inline `2.5f * (t/30.0f)`
formula in Render. We drop these fields in the port; the inline formula
is what produces the visible behaviour.

### 7.5 Caster fade coupling (SetFade)

`Pulse()` lines 5531 and 5626/5659/5693/5725 fire `SetFade(100, 10)`
on cast start (fade caster to invisible-white over 10 ticks) and
`SetFade(0, -10)` on MOVE (fade caster from invisible back to opaque
over 10 ticks during IN). This **belongs to the caster, not the
visual** — the fade is a property of the character mesh, not the
teleport effect.

For the harness port we **do not** drive caster fade. The CharacterRig
owns the rig character; calling `SetFade` on it would require:
- the fade plumbing to work in the harness scene (it doesn't today
  — the rig draws via its own submit path, not the full game render
  pipeline that respects character fade);
- coordination of fade timing with the effect's tick (would require
  cross-component state).

The fade is a **gameflow-layer concern** when wiring the actual spell
cast. The VFX primitive emits the visual; the spell dispatch fires the
caster fade alongside, same as it fires the SetPos payload. Flagged
as **M09c** carve-out for the gameflow agent.

### 7.6 Payload hook surface

The teleport visual ends its OUT phase at tick 50 and transitions to
MOVE on tick 51. Gameflow needs to know **the moment OUT ends** so it
can fire the actual `caster->SetPos(destination)` + `effect->SetPos(
destination)`. Options:

- (a) **State enum on the effect** that gameflow polls. Simple but
  pollish from gameflow's side.
- (b) **Callback** registered on the effect (`SetPayloadCallback(
  std::function<void(TTeleporterEffect*)>)`). Cleaner but adds a
  std::function field to the effect.
- (c) **Component event** posted on the OUT→MOVE transition for any
  registered listener. Most general but heaviest.

For v1 we ship **(a)** — expose `GetState()` returning the
TELE_STATE_* enum. Gameflow polls; if it wants tighter coupling later
it can register a callback (additive API). The MOVE state is exactly
one tick (effect_old.cpp:5541-5736), so the polling window is small
but reliable at 24 Hz.

### 7.7 Per-spell-variant imagery

The 16 callers (§5) use **at least 3 distinct imageries**:

- `teleportation.I3D` — Teleport / Teleport2
- `gvortex.I3D` — VortexM (Misthaven recall)
- `jtele.I3D` / `jteled.I3D` — Priest Teleport (MAC talisman)
- Per-monster summoning variants likely each have their own (TBD;
  unverified)

The pre-release `TTeleporterAnimator` doesn't switch geometry by
variant — it just draws whatever imagery the spawning code bound. So
**one animator class, many possible imageries**.

For our procedural-billboard port the "imagery" collapses to "which
glow texture to use", driven by spell tint. Different per-variant
textures could be added later as a polish pass; v1 ships **one
cool-blue-violet glow texture** that reads correctly for both the
player Teleport and the Misthaven recall (which is the headline
ask).

### 7.8 Sub-cast hand flourish (CHARACTER_RIG_FORENSICS §6 hint)

CHARACTER_RIG_FORENSICS §6 mentions an *optional* `Bone("rhand")`
sub-effect for a "cast flourish". Pre-release source has **no such
sub-effect** for Teleporter — the animator only draws the cylinder
glow column. The hand-flourish hint was speculative polish, not a
pre-release feature. We **do not** add one in v1; if it's wanted as
polish later, it'd be a separate sub-bucket on the same effect
instance, following the M07 photon pattern (when M07 lands).

### 7.9 RestoreZ rect (depth restore around glow)

Pre-release `RefreshZBuffer()` restores a 100×200-pixel screen-space
depth rect around the effect's projected position (effect_old.cpp:5869-
5880). This is a D3D-era hack to make the alpha-blended cylinder
draw correctly z-test against scene geometry. In the modern port,
`EFxDepthMode::TestNoWrite` handles this declaratively — no per-effect
ZBuffer restore needed.

---

## Engine-vs-bespoke decision (per AGENT_GUIDE §3.2.1)

**Decision: bespoke C++ on the FB pipeline, with a procedural cylinder-
glow texture and 5 stacked billboards.** Mirrors L02 halo and F03 fire
in shape.

### Why not engine effects.def

The Teleporter is **fundamentally a state-machine effect**:

1. Multi-phase state machine (INIT → OUT → MOVE → IN) with a payload
   on the OUT→MOVE transition that the spell dispatch couples to.
   AGENT_GUIDE §3.2.1 "stay bespoke" criterion: "**effect-specific
   gameplay state machines** (multi-phase spells with LAUNCH→FLY→
   EXPLODE stages, hit-resolution callbacks, owner-state coupling)."
   This is exactly that shape.
2. The 5-flare envelope uses 5 simultaneously-rendered billboards with
   *per-billboard time-varying* triangle-wave scale curves coupled to
   per-billboard z-index. The current VM doesn't have rand-per-
   particle (per VM_EXTENSION_FORENSICS.md §2 gap 1) needed for the
   spawn-side variation between flares — though here the variation is
   **deterministic by index**, not random, so this argument is weaker.
3. The "engine extension" track in §3.5 explicitly carves out I3D-mesh
   effects: the **real** retail-faithful port (M09b) is a direct mesh
   port; the current port is a billboard-approximation stand-in for
   the eventual mesh port. Wiring it through the engine VM only to
   tear it out when the mesh port lands is wasted plumbing.

### Why not direct I3D mesh port now

Per §7.2 above: I3D mesh load + register infrastructure is a separate
piece of work (M09b). The current bespoke billboard stand-in lets us
ship the *visual primitive* gameflow can call today, without blocking
on mesh-render infrastructure that doesn't exist yet for this code
path. The mesh port is the right end-state; the billboard port is the
right *first* port.

### Bespoke + engine composite?

The 5-flare stack could plausibly be one PE bucket of 5 particles with
deterministic-by-index init. Pros: reusable infrastructure, declarative.
Cons: the per-flare envelope formula needs per-particle scale curves
that read the particle's index — the current VM doesn't have `index`
as a per-particle identifier (`seed` exists but isn't used as an
index). Adding it for one effect's benefit isn't justified (§3.2.1
"if the engine needs three new ops just for one effect's quirks, that
effect is probably bespoke").

The **OUT and IN state envelope** *could* be a single growing-then-
shrinking PE bucket if we just want "a glow that grows then shrinks
over 4 s" — but that loses the 5-flare shimmer / convergence detail
that makes the Teleporter visually distinct from a generic spell halo.

---

## Summary

- Pre-release source: full body available at `src/effect_old.cpp:5444-
  5880`. Retail recon: missing. Constants in §1 are pre-release-
  authoritative.
- Anchor: caster feet (`SVfxAnchor::CharacterRoot`). No hand bone.
- Lifecycle: INIT (1) → OUT (50) → MOVE (1, payload) → IN (≤49) at
  24 Hz. Total ~4.17 s.
- Visual: 5 stacked screen-aligned glow billboards with deterministic
  per-flare triangle-wave envelope; cool blue-violet tint.
- Render: FB pipeline, AdditiveStraight, TestNoWrite, Unlit,
  ScreenAligned.
- Payload: gameflow polls `GetState()` for the OUT→MOVE transition
  and fires `SetPos(destination)` then.
- Carve-outs: M09a (post-Ghidra cross-check), M09b (real I3D mesh
  port), M09c (caster SetFade timing in gameflow).
