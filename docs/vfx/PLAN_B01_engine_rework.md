# B01 Blood — engine-particle rework plan

## Why

Previous `feature/vfx` port submitted blood through per-instance `SubmitFxBillboard`
calls with `EFxBlend::Alpha` + `EFxBlend::AdditiveStraight` overlay — the user
rejected that direction and called out that blood is conceptually a particle
system with respawn + a reflection plane that flips droplets into splats.

Per the resettled forensics (`B01_TBloodEffect_RENDER_RESETTLED.md`) the
effective shipped behavior is:

- **Chroma-keyed opaque** droplets out of `Misc\Blood.I3D` (textured 4-vert quads).
- **World-space** per-droplet transform with stage-dependent orientation:
  FLY = tipped vertical 45°-spun, SPLAT/SHRINK = flat ground decal.
- **Single pass** (the k=1 ONE/ONE overlay is dead-code WIP).
- **Scene-lit** grayscale (LitFlat).

Rendering path -> `EParticleBlendMode::Alpha` + texture-loader's
keycolor->alpha conversion = chroma-key opaque. No new blend mode needed.

## Bucket schema

Two engine particle buckets:

### `blood_fly` — airborne droplet (FLY stage)
- `blend_mode = "alpha"` (chroma-keyed opaque via loader's 1-bit alpha)
- `light_mode = "lit_flat"`
- `depth_mode = "test_no_write"`
- `orientation = "screen_aligned"` — close enough to the snapshot's
  `RotateX(-90°)·RotateZ(-45°)` vertical-spun pose for a particle-system
  representation; per-particle `rot` carries the 45° spin.
- `imagery_path = "Misc\\Blood.I3D"`, texture slot 0
- `atlas_cols = 2, atlas_rows = 2, atlas_frames = 4` — 4 droplet variants
  in the FLY half of the sheet (box01..box04 cells)
- `spawn_count = 0` (driven by burst from the C++ shim) — we use a
  one-shot burst in `spawn_burst` instead.
- `spawn_burst` is sized at attach
- `tick_hz = 24` — matches the original sim cadence
- `default_life = 0` (no auto-kill; reflection-plane kill_expr handles it)
- `kill_expr` — fires when `pos.z <= emit_pos.z` (ground), with the
  reflection-plane mechanism translating that into a SPLAT spawn.
- `spawn_expr` — initial velocity cone (matches retail impale params:
  hangle/hspread/vangle/vspread), `pos = emit_pos + (0,0,height)`.
- `tick_expr` — gravity + drag + scale-grow per-tick.

### `blood_splat` — ground decal (SPLAT/SHRINK stages)
- Same blend / light / depth / imagery
- `orientation = "world_xy"` — flat on world XY, matching snapshot ground decal.
- `atlas_cols = 2, atlas_rows = 2, atlas_frames = 4` — 4 splat variants
  in the SPLAT half of the sheet (box05..box08 cells)
- `spawn_count = 0` (driven by chain-bucket spawn from blood_fly)
- `spawn_burst = 0`
- `tick_hz = 24`
- `default_life = 0` (kill_expr handles via scale)
- `spawn_expr` — peak splat scale 1.8, age/count reset.
- `tick_expr` — hold for ~1s (25 ticks) then shrink at 0.1/tick.
- `kill_expr` — fires when `scale.x <= 0`.

## Extensions to add (engine-generic)

### EXT-1: respawn / loop emission

Already supported via `SParticleBucketEffectDef::spawn_count` +
`EmitSpawnTopup`. Loop emission = continuous restock to N particles.
**Status: no change needed.**

For blood, we use `spawn_burst` (one-shot N particles) for `blood_fly`,
not `spawn_count` — blood is impact-driven, not steady-state. The
respawn mechanism the brief calls out is actually the FLY->SPLAT chain
(EXT-2), not a "respawn the same particle" loop.

### EXT-2: reflection plane / chain bucket

Add to `SParticleBucketEffectDef`:
```
std::string ground_z_expr;       // expression for ground_z (single scalar)
std::string chain_bucket;        // bucket name to spawn into on reflection
std::string chain_spawn_expr;    // spawn-expr to run in the chain bucket
                                 //  (resolves against the dying particle's
                                 //   pos / vel slots via emit_pos / emit_vel)
```

When `kill_expr` fires AND `chain_bucket` is non-empty, the integration
loop:
1. Reads the dying particle's `DrawPos` -> emit_pos for the chain.
2. Looks up the chain bucket in the same runtime.
3. Calls `AddParticle(owner_particle_id, default_life)` on the chain bucket.
4. Runs the chain bucket's spawn_expr (using the dying particle's
   pos/vel as emit_pos/emit_vel so the splat lands where the flier hit).

This is the FLY -> SPLAT transition without a per-particle state machine
in the VM. Stays generic: any future "ember on death" / "spark on hit"
chain works the same way.

### EXT-3: per-particle stage transitions

Picked **Option B** (two buckets + chain) per the brief's recommendation.
No VM changes for stage tracking.

## C++ shim outline (TBloodEffect)

```cpp
class TBloodEffect : public TEffect {
  ...
  // Replace ~370 lines of bespoke spawn + tick + submit with:
  //   - SpawnForTest: load Misc\Blood.I3D + attach TParticleEffectComponent
  //     configured with the "Blood" effect_def + DrawPulse once to seed.
  //   - TickAndSubmitForTest: DrawPulse + SubmitFxParticleBucket(fly) +
  //     SubmitFxParticleBucket(splat).
  //   - IsAlive: true until both buckets drain of particles owned by us.
  // Old bespoke body preserved under #if 0 (preserve-old-code rule).
};
```

## Test approach

1. `cmake --build build` clean
2. `./build/revenant --test=vfx` -> arrow to B01 (TBloodEffect)
3. Visual: dark-red droplet burst, ballistic arc, flat splats on landing
4. Capture single screencap, log path
5. Sanity check vs forensics §10 (color saturated, chroma-key cut, not
   pink-translucent, splats lie flat).

## Open items

- **Fizzle is hand-rolled.** TFizzleEffect uses a collapsed particle
  array on the class (`SFizzleParticle particles[]`), not the bucket
  system. User explicitly said "Fizzle is definitely a particle system
  effect." Migrating it is out of scope for B01 but should be tracked
  as a follow-up cleanup. (Same situation: Sparks/Spark, Smoke, etc.)
