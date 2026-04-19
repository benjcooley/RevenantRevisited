# Deferred Lighting — Depth & World-Position Math

This document derives, from retail's own iso-projection formulas, how the
deferred tile renderer should translate Revenant's software tile blit into a
scene-z suitable for hardware z-test *and* a sampleable value from which the
light pass can recover the world position of any fragment.

Every identity here is an equality, not a fit. If an implementation disagrees
with these formulas, the implementation is wrong.

## 1. Retail conventions

### 1.1 World → screen projection

From `object.cpp:61` (`WorldToScreen`, 2:1 iso):

```
sx = wx − wy                                                                (A)
sy = (wx + wy)/2 − wz·cos30
```

With `cos30 = MYSTERYVAL1 / MYSTERYVAL2 = 867/1000 = 0.867`.

### 1.2 World → screen-z

From `object.cpp:82` (`WorldToScreenZ`):

```
spos.z = WORLDZOFFSET − wz/2 − (wx + wy)·0.867                             (B)
```

`WORLDZOFFSET = 0x3900 + 0x7FFF = 47359`, chosen so the output fits in `u16`
for typical world extents.

Retail does **not** upload that absolute value directly into a hardware depth
buffer. Its software tile blit does this instead:

```
final_native_z = (screenpos.z - regz) + bitmap_local_z                      (B')
```

where:

- `screenpos.z` is the draw call's anchor z from `WorldToScreen(world - pan_w)`
- `regz` is the imagery state's screen-z registration offset
- `bitmap_local_z` is the tile bitmap's stored per-pixel BM_ZBUFFER sample

So the bitmap zbuffer is a **local per-pixel offset relative to the draw
anchor**, not a self-contained absolute scene depth plane.

## 2. Camera-space depth

We need a per-fragment scalar that grows monotonically with distance from the
camera (near = small, far = large), so a fixed `[z_near, z_far]` window maps
cleanly to `d ∈ [0,1]` for the hardware z test.

For the sector test, the camera is anchored at a fixed world-space center
`C = sectorCenter` and debug pan is applied only as a **screen-space pixel
offset** after projection. Define:

```
rel = world − C
```

Then the camera-space forward depth is:

```
scene_z = kcam − (rel.x + rel.y)·0.867 − rel.z/2                           (C)
```

`kcam` (constant `kCamForwardWU = 2750`) is a virtual camera offset along the
iso view axis, chosen so that `scene_z > 0` for fragments inside the frustum.

## 3. Why bitmap-local z stays local but must be signed-decoded

Retail's tile zbuffer stores **local offsets from the imagery anchor**, not
absolute scene-z. That remains true in the deferred path.

If we project the same `rel` through retail's `WorldToScreenZ`, we get:

```
native_rel_z = WORLDZOFFSET − rel.z/2 − (rel.x + rel.y)·0.867             (D)
```

Subtract (D) from (C):

```
scene_z − native_rel_z = kcam − WORLDZOFFSET                               (E)
```

That difference is a **global constant**. So camera-space depth and retail's
native screen-z use the same scale; they differ only by a constant offset.

One detail matters, though: retail performs `(screenpos.z - regz) +
bitmap_local_z` in a **16-bit register**. So authored negative local z deltas
arrive in the bitmap as wrapped `u16` values and are interpreted correctly by
the modulo-65536 add. Once we move into signed camera-space floats, we must
decode those samples back to `int16` first:

```
bitmap_local_z_signed = int16(bitmap_local_z_u16)                          (F)
```

Then the authored registration offset and bitmap-local z sample apply in
camera space as:

```
anchor_scene = CameraDepth(world_anchor − C) − regz                        (G)
frag_scene_z = anchor_scene + bitmap_local_z_signed                        (H)
```

No `screenpos.z` is required in the runtime path. The tile base depth comes
from the tile's world anchor transformed into camera space, and the bitmap
zbuffer contributes the same authored local delta retail used, just decoded
out of its wrapped `u16` storage first.

### 3.1 Tile fragment shader output

The hardware depth value is therefore:

```
d = clamp( (frag_scene_z − z_near) / zspan , 0, 1 )                        (I)
```

or, in the existing shader form:

```
zparams.x = (anchor_scene − z_near) / zspan            ; per-tile anchor
zparams.y = depth_mul / zspan                          ; signed local-z scale
```

`depth_mul` remains a tuning knob on the bitmap-local contribution only; at
`depth_mul = 1.0` the formula matches retail depth spacing.

## 4. World-position recovery (light pass)

The light pass runs fullscreen over the G-buffer (albedo, world normal,
scene-z). Each fragment's screen position `(S, T)` and `scene_z` uniquely
determine its world `(wx, wy, wz)` via the iso inverse.

### 4.1 Setup

Because pan is applied after projection, `(S, T)` are just the fragment's
screen coordinates relative to the projected camera origin `(ox, oy)`:

```
S = px − ox = rel.x − rel.y
T = py − oy = (rel.x + rel.y)/2 − rel.z·0.867
```

And from (C):

```
K ≔ kcam − scene_z = (rel.x + rel.y)·0.867 + rel.z/2
```

### 4.2 Solving for `wz`

Using `sum_r ≔ rel.x + rel.y = 2·(T + wz·0.867)` (from T), substitute into K:

```
K = (2T + 2·wz·0.867)·0.867 + wz/2
  = 2T·0.867 + wz·(2·0.867² + 0.5)
```

Retail uses the fixed approximation `867/1000`, so the denominator is:

```
2·0.867² + 0.5 = 2.003378
```

Solving:

```
wz = (K − 2T·0.867) / 2.003378                                             (I)
```

### 4.3 Solving for `wx, wy`

Once `wz` is known:

```
sum_r = 2·(T + wz·0.867)                                                   (J)
rel.x = (sum_r + S) / 2
rel.y = (sum_r − S) / 2
world = rel + C
```

### 4.4 Invariant

`W = (wx, wy, wz)` recovered this way is the same world position that drove
the tile pass. Point lights passed in world coordinates therefore evaluate
their distances against the correct fragment position, while debug pan has no
effect on reconstructed depth.

## 5. Uniform summary

Per-frame constants uploaded to the light pass:

| name         | value                                            | used by |
| ------------ | ------------------------------------------------ | ------- |
| `(ox, oy)`   | projected camera origin, including debug pan     | light FS |
| `z_near`     | fixed game near plane `-1024 wu`                 | tile FS, light FS |
| `zspan`      | fixed game span `16384 - (-1024) = 17408 wu`     | tile FS, light FS |
| `center.xy`  | fixed world-space camera center                  | light FS |
| `kcam`       | `kCamForwardWU = 2750`                           | light FS |

Per-tile uniforms:

| name              | value                                      | used by |
| ----------------- | ------------------------------------------ | ------- |
| `anchor_scene`    | `CameraDepth(world_anchor − center) - regz` | tile FS |
| `bitmap_local_z`  | sampled BM_ZBUFFER local z, decoded as `int16` | tile FS |

Normal reconstruction still uses the tile-local data (`tile_root`,
`tile_sprite`, `filter.x = normal_radius`), but depth is now anchored in true
camera space derived from the tile's world position.

## 6. What this supersedes

Two incorrect assumptions caused most of the deferred depth failures:

- treating the tile anchor as `screenpos.z + C_depth` instead of a world-space
  anchor transformed into camera space
- letting debug pan leak into depth/reconstruction math, even though pan is a
  screen-space offset and should not move the camera

Either mistake compresses the effective depth range, breaks z-fit, and makes
tiles behave like painter-sorted sprites. The corrected contract is:

```
world anchor -> camera-space base depth
bitmap local z -> raw local delta on top of that base
bitmap local z storage -> decode wrapped `u16` back to signed local delta
screen pan -> xy offset only
```

One more trap: an earlier draft of this note simplified the light-pass
inverse to `/ 2` by implicitly treating `cos30` as the exact `sqrt(3)/2`.
That is close numerically, but retail does not use that constant; it uses
`867/1000`. The world-position recovery in the light pass must keep the
retail denominator `2.003378` if we want reconstruction to match the original
engine.
