# Porting an effect to a prefab — the recipe

Operational companion to [PREFAB_SYSTEM.md](PREFAB_SYSTEM.md). One effect
per agent; the gate is a side-by-side with the existing harness entry.

## 1. What a prefab is

A file `assets/prefabs/<Name>.def`. Its body is the root node: components,
child `node` blocks. It is baked to bytes once and stamped by name. The
harness lists every file automatically as `Prefab:<Name>`.

```
particles                         // the engine particle effect, inline
{
    effect <Name>
    {
        inactive_ttl_pulses = 12
        bucket <bucket_name> { ... }        // repeat for several buckets
    }
}

node <child>                      // optional children, each with their own components
{
    pos = [0, 0, 20]
    particles { effect ... }
}
```

Reference files: `assets/prefabs/Mist.def` (steady state),
`Blood.def` (one-shot burst + chain), `Sparks.def` (delay, life range,
trails, bounce, burst frame).

## 2. Bucket fields

Member names in [src/particledef.h](../../src/particledef.h) are the
keys; the trailing comments are the descriptions. The ones that matter:

| Key | Meaning |
|---|---|
| `imagery_path` | I3D whose texture slot `texture_slot` is the sprite sheet (e.g. `"Misc\\Sparks.I3D"`); always set it for a prefab |
| `atlas_cols` / `atlas_rows` / `atlas_frames` | sprite grid; `random_start_frame` picks a cell per particle, `burst_frame` one cell per burst |
| `width` / `height` / `scale` | default sprite size in world units (spawn_expr may set `scale` per particle) |
| `blend_mode` | `alpha` / `additive` (src-alpha, one) / `additive_straight` (one, one; retail SetAddBlendState) / `premul_alpha` |
| `light_mode` | `unlit` (self-lit effects) / `lit_flat` |
| `depth_mode` | `test_no_write` (normal) / `test_write` / `none` |
| `orientation` | `screen_aligned` / `world_xy` (flat on the ground: rings, decals, dust) |
| `spawn_burst` / `spawn_count` | one-shot N at attach, or a steady population of N (respawned when killed) |
| `spawn_delay_min` / `spawn_delay_max` | ticks a new particle is held before it starts (also the way to spread a burst over time) |
| `tick_hz` | 24 for everything transcribed from retail; tick_expr literals are per tick |
| `default_life` / `life_max` | seconds; `life_max > default_life` gives each particle a random life in that range; `default_life <= 0` = until kill_expr |
| `chain_bucket` | bucket a dying particle spawns one particle into (emit_pos / emit_vel = the dying particle's) |
| `trail_count` / `trail_gravity` | motion-streak copies drawn along the velocity |
| `spawn_expr` / `tick_expr` / `kill_expr` | the VM programs, below |

## 3. The expression VM

Statements (`spawn_expr`, `tick_expr`): `var = expr;` with writable
`pos`, `vel`, `scale`, `color`, `rot`, `frame`; read-only `emit_pos`,
`emit_vel` (the owner's world position / zero, or the dying particle's
for a chain), `age` (seconds), `age01`, `time_frame`, `seed`. Single
lanes read and write: `pos.z`, `color.a`, `scale.x`.

Functions: `vec3 vec4 rand(lo,hi) rand01() step(edge,x) select(cond,t,f)
floor sin cos min max clamp lerp`. **`step(edge, x)` is 1 when `x >= edge`**,
so `step(pos.z, emit_pos.z)` reads "pos.z is at or below the ground".
`select(c, t, f)` is `c != 0 ? t : f`. There are no temporaries: order
statements so a test on a value runs before the statement that changes it
(see the bounce in `Sparks.def`).

`kill_expr` is one expression; nonzero kills (a `spawn_count` bucket
respawns the particle in place instead). `frame` written per tick animates
the atlas cell.

Units: positions in world units, velocities in wu per tick, gravity in wu
per tick squared, at `tick_hz = 24`. Retail's per-frame constants
transcribe as they are.

## 4. Where the numbers come from

Retail is the reference, in this order: `recon/classes_readable/`,
`src/effect_old.cpp` (the 1999 animator bodies), the forensics doc under
`docs/vfx/forensics/`, then the existing port (`src/effects/<name>.cpp`
shim or the `T<Name>Effect` class in `src/effect.cpp`), which usually has
every constant with a citation. Sprite geometry: dump the I3D and read the
quad's bbox and UV cells:

```
./build/Revenant --headless --dumpi3d="Misc\\Sparks.I3D" --dumpi3dout=<scratch>/dump --max-runtime=20
cat <scratch>/dump/manifest.txt
```

Note the vertical world scale: a retail animator draws through the
object's matrix, which carries WORLD3D_Z_SCALE on local z, so a local
height of 45 lands at 67 world units. Prefab positions are world units.

## 5. Verify

No rebuild is needed for a prefab file. Capture the prefab and the
existing entry (ids from the catalogue log) and compare the same frames:

```
S=<scratch>
for fx in "Prefab:<Name>" T<Name>Effect; do n=$(echo $fx | tr -d ':');
  ./build/Revenant --headless --test=vfx --vfx=$fx --filmstrip=6,0.25 --vfx-bg=dungeon \
     --snapprefix=$S/${n}_ --max-runtime=6 > $S/$n.log 2>&1; done
python3 -I tools/vfx/crop_compare.py $S/cmp.png 250 150 200 130 3 $S/Prefab<Name>_00[1-3]*.png $S/T<Name>Effect_00[1-3]*.png
```

Read `cmp.png` (left: prefab, right: the entry). The gate is "a stranger
could not tell which is which": placement, size, colour, cadence,
lifetime. A per-burst random pick (variant cell, direction) is not a
difference. Check the log for `[particle]` errors (a typo in the file is
a load-time error with a line).

Done = the file, a one-paragraph note of what retail values it carries
and any divergence left, and the two filmstrips. If the engine cannot
express something, stop and report the missing generic capability
(a new bucket field or VM function) rather than approximating.
