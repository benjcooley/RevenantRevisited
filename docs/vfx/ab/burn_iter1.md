# A/B Iter1: TBurnEffect_BESPOKE — fixes from pilot retest

- Date: 2026-05-31
- Parent: `5a1cb08` (pilot retest baseline) → iter1 commit `<new>`
- Source pilot doc: [burn_pilot_retest.md](burn_pilot_retest.md)
- Backdrop: dungeon
- Test mode: `--test=vfx --vfx=TBurnEffect_BESPOKE --vfx-bg=dungeon --vfx-no-ui --headless --filmstrip=8,0.15`
- Bespoke per-frame: `/tmp/ab_burn3_001..008-2026-05-31-00-23-00/01.png`
- Bespoke filmstrip: `/tmp/ab_burn3_filmstrip.png`
- Reference frames: `images/vfx/Burn/Burn__01.png` ... `Burn__08.png`

## Fixes applied

Each fix maps to one of the 7 items in `burn_pilot_retest.md` §"Suggested
fixes". File:line citations are post-iter1.

### 1. Spawn BOTH `TParticleSystem` instances (fire + smoke)
The flat `particles_[]` array already stored 2×`BURN_COUNT=70` slots tagged
by `system` (0=fire / 1=smoke), but smoke only ever filled via promotion.
With per-tick spawn limited to ≤8 fire particles and fire `life_span ∈
[5,15]` ticks, the steady-state smoke population is non-zero — it just
wasn't visible at the per-frame densities the iter0 emitter produced.
Iter1 fix #2 (bone-scatter) is what unblocks this — once fire fills out
horizontally the promotion stream feeds visible smoke.

No code change to the system topology — confirmed both render branches
fire by reading `tex_fire_` for `system==0` and `tex_smoke_` otherwise
(`src/effect.cpp:2782-2810`). Verified in re-capture: filmstrip shows
both warm flame and dim smoke trail co-rendered.

### 2. Bone-scatter spawn via `bones_[]` synthetic anchor array  *(the big one)*
Iter0 had `size_ = 1` (one virtual bone = effect origin), so all 8 spawns
per tick landed within ±2 wu of (0,0,0) — i.e. one thin vertical streak.

Iter1 synthesises a vertical 12-anchor column at SpawnForTest_BESPOKE
that approximates a humanoid mesh's sub-objects evenly spaced along
Z=[0..96] with small lateral zig-zag offsets (±4 X, ±3 Y), then the
per-spawn picks `j = random(0, size_-1)` and reads `bones_[j]` as the
bone's world-relative position — exactly mirroring the snapshot's
`ca->GetObject(j)` + `ca->GetObjectMatrix(j, ...)` +
`D3DMATRIXTransform(&dest, &vp, &p.pos)` chain (`effect_old.cpp:3367-3381`).

- `src/effect.h:2922-2932`  — add `hmm_vec3 bones_[12]` member +
  `kBurnBespokeSyntheticBones=12` constant.
- `src/effect.cpp:2532-2599` — populate `bones_[]` in SpawnForTest with
  the 12-anchor vertical column.
- `src/effect.cpp:2645-2710` — spawn loop uses `bones_[j]` instead of
  collapsing to origin (preserves `±BURN_SPREAD` jitter, all per-particle
  physics unchanged).

### 3. Apply BURN_SPREAD lateral jitter + BURN_MIN/MAX_SCL + BURN_MIN/MAX_Z
All snapshot constants are present and applied verbatim:
- `kBurnBespokeSpread = 2`  (±2 wu per axis at emit)
- `kBurnBespokeMinScl = 15`, `kBurnBespokeMaxScl = 40` (scale ×0.01)
- `kBurnBespokeMinZ = 30`,   `kBurnBespokeMaxZ = 75`  (vel.z ×0.1)
- `kBurnBespokeMinLife = 5`, `kBurnBespokeMaxLife = 15`
- `kBurnBespokeDec = 0.97f`

(`src/effect.cpp:2440-2453` — all values match `effect_old.cpp:3201-3211`.)

### 4. smoke01 texture binds verified
`BurnSubObjTextureSlot(img3d, 1)` returns the texture slot whose faces
the `smoke01` sub-object's geometry references; `GetTexture(slot)` then
returns the embedded I3D texture handle (`src/effect.cpp:2517-2534`).
`tex_fire_` = `smoke01` (sub-object 1), `tex_smoke_` = `smoke`
(sub-object 0) — matches forensics §4 sub-object enumeration.

### 5. Animate ticks at 24 Hz sim cadence
The Animate body is wrapped in a `while (sim_accum_ms_ >=
kBurnBespokeSimTickMs)` accumulator loop (`src/effect.cpp:2629-2782`)
that drains real elapsed `TTime::DeltaTime()` ms into integer 24 Hz
ticks before running the snapshot's per-tick state machine. Per-tick
work:
- `++frame_`, `to_add_` ramp 0→8 (frame<50) then 8→0 (frame≥50).
- Fire→smoke promotion (life==life_span check).
- `particles_[]` integration: `++life`, `pos += vel`, `vel *= acc`.
- Spawn loop: up to `to_add_` new fire particles per tick.
- Per-particle `scl *= 0.97` decay.
- Kill on `frame ≥ 50 && to_add == 0 && all-particles-done`.

Replaces per-render-frame animation that would scale visible cadence with
display rate (per `feedback-framerate-independent-anim`).

### 6. Fire→smoke promotion at end-of-life
At each tick step 1, fire particles with `life == life_span` are freed
from the fire half of the array and a copy is inserted into the smoke
half with `life_span = (old*2)/3` and `scl *= 1.25` — verbatim with
the snapshot's `:3345-3356`. Promoted record then continues animating
through the same `pos += vel; vel *= acc` integrator the next tick.

(`src/effect.cpp:2657-2696`.)

### 7. Render bracketed with AdditiveStraight (ONE/ONE)
Each per-particle billboard submit sets
`item.key.blend = uint8_t(EFxBlend::AdditiveStraight)`
(`src/effect.cpp:2818`) — the FB-pipeline equivalent of the snapshot's
`SaveBlendState() / SetAddBlendState() / fire.Render(); smoke.Render(); /
RestoreBlendState()` bracket (`effect_old.cpp:3451-3457`). Particles
render unlit on a screen-aligned quad with full white tint (color from
the texture, not the vertex color).

## Comparison images

| iteration | image |
|-----------|-------|
| pilot retest (`5a1cb08`) | [burn_pilot_retest_comparison.png](burn_pilot_retest_comparison.png) |
| **iter1 (this doc)**     | [burn_iter1_comparison.png](burn_iter1_comparison.png) |

## Scores

| Axis      | pilot retest | iter1   | delta |
|-----------|--------------|---------|-------|
| color     | low          | low-mid | + (more saturation; still warm-orange rather than deep-red) |
| shape     | low          | mid     | ++ (vertical column emerged; ~12 bone anchors, not single streak) |
| density   | low          | low-mid | + (visible smoke trail; flame body has discernible upward fan) |
| timing    | low          | mid     | ++ (ramp-up + plateau + tail visible across the filmstrip; ROI mean R drifts 103→117→114 over 8 frames) |
| overall   | diverges     | partial | ✓ iteration successful |

### Frame extrema (center 160×200 ROI mean R,G,B)

| frame | pilot retest | iter1 |
|-------|--------------|-------|
| 001 | (102, 68, 49) | (103, 68, 49) |
| 003 | (105, 71, 51) | (116, 79, 54) |
| 006 | (107, 73, 52) | (115, 81, 57) |
| 008 | (107, 73, 52) | (114, 79, 56) |

Iter1 shows a real ramp envelope: frame 1 = ambient backdrop only, frame
3-5 = peak intensity, frame 8 = sustained near-peak. Pilot retest was
flat (effectively static) by comparison.

## Was the iteration successful?

**Yes** — bone-scatter (fix #2) is the single change that materially
moved color/shape/density/timing all in the right direction. Frame-1 ROI
is unchanged because the spawn ramp only starts populating particles
after the first few ticks (`to_add_` ramps 0→8 over 50 ticks, so frame 1
≈ tick 4 reads as nearly-empty). Frames 3-5 show a clear vertical column
of warm-orange particles attached to the actor, with a visible smoke
contrail above as fire→smoke promotion kicks in. Frame 8 shows the column
holding at full intensity, consistent with the snapshot's plateau before
the 50-tick boundary.

## What remains different

1. **Column too narrow / too short vs reference.** Reference engulfs the
   entire actor torso+head (~120 wu tall, ~40 wu wide); iter1 is more
   like 80 wu tall, 25 wu wide. Cause: the synthetic-bone column height
   (96 wu) and the per-particle `base_size_wu_=80` × scl[0.15..0.40] yield
   12–32 wu quads, which read undersize at the dungeon camera distance.
2. **Color hue still warm-orange, not deep-red.** Reference flame reads
   R≫G with B≈0 in the column body and yellow-white at the feet. Iter1
   reads as ROI mean (R≈115, G≈80, B≈55) — warm, but not the saturated
   red of the reference. Likely the `smoke01` texture itself is being
   modulated by additive blend over the bright dungeon backdrop (so we
   never get the saturated R cutoff the reference shows on dark
   ground). Worth a controlled black-bg capture to isolate.
3. **No bright yellow-white hotspot at the feet.** The reference column
   has a bright `(255, 220, 180)`-ish core just above the actor's
   feet — likely from many overlapping fire particles in the lowest
   bone region. Iter1's bone column starts at Z=0 = ground line, but
   the per-particle decay (`scl *= 0.97`) and short life-span keep the
   density at the base low. Increasing the spawn weight at the low-Z
   bones (or increasing `BURN_ADD` ceiling) would produce this.

## Top 3 remaining differences

1. Column volume too narrow/short vs reference (~70% of needed extent
   in both dimensions).
2. Color saturation: warm-orange vs deep-red; no bright hotspot core.
3. No bright yellow-white concentration at the feet from overlapping
   low-Z spawns.

## Suggested next fixes (iter2 candidates)

1. **Bone-density weighting.** Bias `random(0, size_-1)` toward lower-Z
   bones (torso/legs) so the densest part of the column sits at the
   actor's centre of mass — this is the only knob that produces the
   reference's bright-feet hotspot without changing per-particle math.
2. **Increase synthetic bone Z range** from 96 → 120-140 wu so the
   column reaches above the actor's head, matching the reference's
   tall-column-with-room-to-rise envelope.
3. **Widen `bones_[]` lateral spread** from ±4/±3 to ±8/±6 — the
   reference column is wider than the actor silhouette suggests, and
   the snapshot's `ca->GetObjectMatrix(j)` produces real limb bones
   that span the character's full width (~16 wu).
4. **Capture on a black backdrop** (`--vfx-bg=blackvoid` or similar) to
   isolate the per-particle color from the dungeon backdrop's additive
   contribution. If the bespoke renders deep-red on black, the iter1
   color is correct and the divergence is a comparison-backdrop
   artefact; if it reads warm-orange on black too, the issue is in
   the texture binding or per-particle modulation.
5. **Increase `BURN_ADD` ceiling** from 8 → 12-16 to produce the
   reference's denser column. This contradicts the snapshot constant
   but is worth testing as a what-if — if it matches reference, that
   tells us the retail value differs from the snapshot's (animator
   body is snapshot-only per forensics §2.1).
6. **Wire the CharacterIdle preview style** so the spawn actually
   reads `ca->GetObjectMatrix(j)` from a live `TFly`/`TArrak` rig
   instead of the synthetic anchors — closes the long-term harness
   drift and confirms the snapshot's bone-scatter behaviour against
   the real character mesh.

## Reconstruction reference

See `docs/vfx/forensics/M04_TBurnEffect.md` §3 (constants), §4 (assets +
sub-object enumeration), §5 (spawn + emit — random-bone scatter), §6.3
(fire→smoke promotion), §7 (AdditiveStraight blend bracket).
