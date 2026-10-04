# 03 — Rendering & Coordinate Conventions (original 1998/99 engine)

Reference doc for VFX forensics agents. Describes the **original** Revenant
effect renderer: its D3D-era render states, lighting/glow model, depth handling,
coordinate spaces, rotation conventions, per-vertex color packing, math helpers,
and strip/billboard/trail/beam geometry. Pure reference — answers "how did the
original do it," with `file:line` citations.

Vocabulary is [NOMENCLATURE.md](../NOMENCLATURE.md). Sister doc:
[01_ARCHITECTURE.md](01_ARCHITECTURE.md) (effect/animator architecture).

NOMENCLATURE's *descriptive labels* (Alpha / Additive / Decal; ScreenAligned /
WorldXY; Unlit / LitFlat; TestNoWrite / …) are used here only as **classification
tags** for the original's behavior. This doc names the actual D3D state the
original set; it does **not** map anything to the modern engine's enums.

> **Reading the source.** The 1998 render bodies are in the `#if 0` regions of
> `src/effect_old.cpp` (gate at `:126`), `src/stripeffect.cpp`,
> `src/missileeffect.cpp`, and `src/weapontrail.cpp`. The blend helpers appear
> twice in `effect_old.cpp`: modern no-op stubs at `:122-124` and the **real**
> 1998 bodies at `:181-244` — cite the real bodies.

---

## 1. The D3D-era render-state system

Every effect `Render()` brackets its draw with three helpers that poke
`Scene3D` (the global D3D scene) render states:

```
SaveBlendState();   // snapshot current states
SetBlendState();    // OR  SetAddBlendState();  -- choose the effect's mode
…RenderObject(obj)…
RestoreBlendState();// put the snapshot back
```

Pattern visible in essentially every animator, e.g. `TFlareAnimator::Render`
(`src/effect_old.cpp:588-609`), `THealAnimator::Render`
(`src/effect_old.cpp:772-830`).

### 1.1 What each helper sets

`SaveBlendState()` (`src/effect_old.cpp:181-191`) snapshots six render states:
`TEXTUREMAPBLEND`, `ZWRITEENABLE`, `ZENABLE`, `CULLMODE`, `SRCBLEND`,
`DESTBLEND`. `RestoreBlendState()` (`:200-210`) writes all six back.

**`SetBlendState()` — the default effect mode** (`src/effect_old.cpp:221-233`):

| state | value | cite |
|-------|-------|------|
| `D3DRENDERSTATE_TEXTUREMAPBLEND` | `D3DTBLEND_MODULATE` | `:223` |
| `D3DRENDERSTATE_ZWRITEENABLE` | `false` | `:224` |
| `D3DRENDERSTATE_ZENABLE` | `true` | `:225` |
| `D3DRENDERSTATE_SRCBLEND` | `D3DBLEND_SRCALPHA` | `:228` |
| `D3DRENDERSTATE_DESTBLEND` | `D3DBLEND_INVSRCALPHA` | `:229` |

i.e. **SRC_ALPHA / INV_SRC_ALPHA, depth-test on, depth-write off, texture
modulated by vertex color** — classic translucent **Alpha** blending. This is
the mode most fire/flame/flare/heal/glow effects use.

> **Correction to NOMENCLATURE §3.** The NOMENCLATURE table calls
> `SetBlendState()` the "`D3DTBLEND_DECAL = SRC_ALPHA/INV_SRC_ALPHA`" default.
> The actual 1998 body sets `D3DTBLEND_MODULATE` (not `DECAL`) for the
> texture-blend stage, with `SRCBLEND/DESTBLEND = SRC_ALPHA/INV_SRC_ALPHA`
> (`src/effect_old.cpp:223,228-229`). The alpha-blend factors match what
> NOMENCLATURE describes; the texture-stage op is MODULATE, meaning the sampled
> texel is multiplied by the interpolated per-vertex diffuse (so vertex color
> *tints* the texture). Classify it as **Alpha (modulated)**. Note `DECAL` does
> appear, but in the *additive* helper's texture stage (next).

**`SetAddBlendState()` — the additive/self-lit mode** (`src/effect_old.cpp:235-244`):

| state | value | cite |
|-------|-------|------|
| `D3DRENDERSTATE_TEXTUREMAPBLEND` | `D3DTBLEND_DECALALPHA` | `:237` |
| `D3DRENDERSTATE_ZWRITEENABLE` | `false` | `:238` |
| `D3DRENDERSTATE_SRCBLEND` | `D3DBLEND_ONE` | `:239` |
| `D3DRENDERSTATE_DESTBLEND` | `D3DBLEND_ONE` | `:240` |
| `D3DRENDERSTATE_ZENABLE` | `true` | `:241` |

i.e. **ONE / ONE straight additive, depth-write off** — classify as
**AdditiveStraight**. Used by the heavily-glowing effects: CreateFood
(`src/effect_old.cpp:1272-1273`), Cure (`:1914-1915`), FireFlash (`:2397-2398`),
FireWind (`:3096-3097`), Burn (`:3451-3452`), Aura (`:3659-3660`).

> Not every glow is additive. The fountains, SymGlow, FaultFire, ripple and drip
> all use the **Alpha** `SetBlendState()` default — `TFountainAnimator::Render`
> (`src/effect_old.cpp:3861-3863`), `TFaultFireAnimator::Render` (`:11184-11185`).
> Trace the helper, don't assume.

### 1.2 Blend-mode taxonomy used by effects

| original D3D state set | NOMENCLATURE label | which helper / site | typical effects |
|------------------------|--------------------|---------------------|-----------------|
| `SRCBLEND=SRCALPHA`, `DESTBLEND=INVSRCALPHA`, `TEXTUREMAPBLEND=MODULATE` | **Alpha** (modulated) | `SetBlendState()` `effect_old.cpp:221-233` | flare, flame, heal, symbol glow, ripple, drip |
| `SRCBLEND=ONE`, `DESTBLEND=ONE`, `TEXTUREMAPBLEND=DECALALPHA` | **AdditiveStraight** (ONE/ONE) | `SetAddBlendState()` `effect_old.cpp:235-244` | fireflash, firewind, burn, aura, cure, fountains |
| `SRCBLEND=SRCALPHA`, `DESTBLEND=INVSRCALPHA` (explicit, via `ERender3DState`) | **Alpha** | weapon swipe `src/weapontrail.cpp:589-593` | sword trail (alpha-faded ribbon) |
| `SRCBLEND=ONE`, `DESTBLEND=ONE` (lightning) | **AdditiveStraight** | strip note `src/stripeffect.cpp:362` (D3DBLEND_ONE/ONE) | lightning bolt strip |

> **`CULLMODE`.** Strip/ribbon effects additionally force two-sided rendering by
> setting `CULLMODE = D3DCULL_NONE` so either face of the strip draws —
> `TLightningAnimator::Render` (`src/stripeffect.cpp:830-831`), weapon swipe
> (`src/weapontrail.cpp:589`). This is restored from the snapshot afterward.

> **Forensics rule (from NOMENCLATURE §3).** Classify a blend by tracing the
> `SetBlendState`/`SetAddBlendState` helper actually called in that effect's
> `Render`, not by how it looks. F03 fire was wrongly assumed additive; the
> helper it calls is `SetBlendState()` (the Alpha/MODULATE default).

---

## 2. Lighting / glow (lit vs self-lit)

The original has no per-particle lighting flag. The distinction between
**scene-lit** and **self-lit/glowing** effects is expressed two ways:

1. **Material zeroing → self-lit.** An animator that wants pure emissive output
   zeroes the imagery's material (ambient/diffuse/specular/emissive all 0,
   `power=0`) at `Initialize`. `TFlareAnimator::Initialize`
   (`src/effect_old.cpp:527-549`) is the canonical example — it pulls material 0,
   zeroes every channel, writes it back. With the additive/modulate blend the
   visible color then comes entirely from the **per-vertex diffuse** (§6) and the
   texture, not from scene lights. Classify as **Unlit** (NOMENCLATURE §4): the
   color is literal.

2. **Sampling ambient light → scene-lit.** Effects that should read the
   environment fold `MapPane.GetAmbientLight()` into their per-vertex color. E.g.
   `TWaterFallAnimator` mixes ambient into the vertex color:
   `temp = MapPane.GetAmbientLight()/255.0f; r += temp; … object->lverts[j].color
   = D3DRGB(r,g,b)` (`src/effect_old.cpp:11801-11810`); the water animator does
   the same (`:12010-12012`). Classify as **LitFlat**: the effect's brightness
   tracks scene ambient.

Most magic/fire/spark effects are **Unlit** (self-lit glows drawn additive); the
water / waterfall / fog family is the main **LitFlat** group.

> The blend choice and the lit/self-lit choice are linked but distinct: a
> self-lit glow is almost always `SetAddBlendState()` (ONE/ONE) with a zeroed
> material; a scene-lit translucent surface is `SetBlendState()` (Alpha) with
> ambient mixed into the vertex color.

---

## 3. Depth / Z handling

### 3.1 Render-time depth states

Every effect mode **disables depth write** (`ZWRITEENABLE=false`) but **keeps
depth test** (`ZENABLE=true`): `SetBlendState` (`src/effect_old.cpp:224-225`),
`SetAddBlendState` (`:238,241`), weapon swipe (`src/weapontrail.cpp:593`). So a
translucent effect is *occluded by* solid scene geometry already in the depth
buffer, but does **not** write its own depth — classify as **TestNoWrite**
(NOMENCLATURE §5). This is the universal effect depth mode in the original.

No effect in the gated bodies sets `ZENABLE=false` (always-on-top); could not
find a **None** (no-test/no-write) effect in the available source.

### 3.2 `RefreshZBuffer` — why it exists

Because effects don't write depth (3.1), but the engine's deferred/tile renderer
needs a coherent Z under moving translucent objects, each animator can override
`RefreshZBuffer()` to *restore* the scene Z under the effect's screen footprint.

- Default: `T3DAnimator::RefreshZBuffer` → `T3DImagery::RefreshZBuffer(inst)`
  (`src/3dimage.h:476-477`, `src/3dimage.cpp:1941`).
- Override pattern: compute the effect's world origin, project with
  `WorldToScreen`, then `RestoreZ(screenx, screeny, width, height)` over the
  bounding patch. `THealAnimator::RefreshZBuffer`
  (`src/effect_old.cpp:835-850`): `size_x=100, size_y=220`, patch centred on the
  projected origin.
- `RestoreZ(x,y,w,h)` builds an `SRect` and calls `Scene3D.RestoreZBuffer(r)`
  (`src/effect_old.cpp:162-171`).

So `RefreshZBuffer` is the "repair the depth buffer where this no-depth-write
effect drew" hook. Forensics: an effect that visibly composites *behind* later
geometry but doesn't itself occlude is exhibiting TestNoWrite + RefreshZBuffer.

---

## 4. Coordinate spaces & the iso projection

Three spaces (NOMENCLATURE §1): **world** `(wx,wy,wz)` (wz up), **local**
(relative to the effect's anchor / object matrix), **screen** (framebuffer
pixels).

### 4.1 The iso projection formula

`WorldToScreen` is a fixed 2:1 isometric projection (`src/object.cpp:126-140`,
constants `src/object.cpp:117-120`):

```
sx = wx − wy                                                  // object.cpp:130,137
sy = (wx + wy)/2 − wz·(867/1000)                              // :130,138   (cos30 ≈ 0.867)
sz = WORLDZOFFSET − wz/2 − (wx + wy)·(867/1000)              // :139        (screen-z, u16)
```

with `MYSTERYVAL1/MYSTERYVAL2 = 867/1000 = 0.867` (`src/object.cpp:119-120`) and
`WORLDZOFFSET = 0x3900 + 0x7FFF = 47359` (`:117`). The screen-z is the same form
exposed as `WorldToScreenZ` (`src/object.cpp:145-147`). Inverse:
`ScreenToWorld` (`:149-160`). Full derivation: [DEFERRED_LIGHTING.md
§1](../../DEFERRED_LIGHTING.md).

### 4.2 Where effect transforms sit relative to projection

Effect animators compute geometry in **world / local space** and let the engine
project. An effect builds its quad/strip vertices in object-local coordinates,
sets an `obj->matrix` (or pos/rot/scl with `OBJ3D_*` flags, §5) that places it in
world space, and calls `RenderObject(obj)`; the iso projection (4.1) is applied
downstream by the scene. The pre-release transforms in `Render` bodies are
therefore **pre-projection** world/local placements — never screen-space.

The one screen-space touch is `RefreshZBuffer` (§3.2), which deliberately
projects to pixels to size the Z-restore patch. Per NOMENCLATURE §1, screen math
must not leak into the emit/depth/light logic — in the original it doesn't,
except for that explicit Z-patch.

---

## 5. Rotation & orientation conventions

Effect quads are oriented by the `S3DAnimObj` transform fields (`pos`, `rot`,
`scl`, or a full `matrix`) gated by `OBJ3D_*` flags (`src/3dimage.h:99-130`).
`rot` is in **radians**.

### 5.1 The canonical rotations

| rotation | meaning | NOMENCLATURE orientation | cite |
|----------|---------|--------------------------|------|
| `rot.x = -(M_PI/2)` | tip an authored-XY quad onto the ground plane (lies flat on world XY, foreshortens under iso) | **WorldXY** | flare `src/effect_old.cpp:596`; heal bubble `:795` |
| `rot.x = 0` (no tip) | quad stays upright / camera-facing | **ScreenAligned** | heal glow at feet, `n==0` branch `src/effect_old.cpp:787-792` |
| `rot.y = π` | flip 180° about up — face the quad the other way | (facing flip) | (general; e.g. ice-bolt spiral `src/effect_old.cpp:8531`) |
| `rot.z` | in-plane spin of the quad about its own facing axis | **in-plane rotation** | flare `rot.z=-(M_PI/4)` `src/effect_old.cpp:598`; fireball burst `rot.z=-(M_PI/3)` `src/missileeffect.cpp:814` |

So `rot.x=-π/2` is the **"tip onto ground = WorldXY"** tell; `rot.z` is a static
or animated in-plane spin (the heal cylindrical glow spins by drawing twice with
`rot.z = ±rotation`, `src/effect_old.cpp:820-826`).

### 5.2 Byte-angle facing — `Face(n)` / `GetFace()`

Object facing on the XY plane is a **byte angle**: `facing` is a `uint8_t`,
0–255 over a full turn (`src/object.h:644`). `Face(n)` sets it
(`src/object.h:972`), `GetFace()` returns it (`src/object.h:976`). Effects
convert it to radians for the in-plane spin:

```
rot.z = -((facing / 256.0) * M_2PI)        // e.g. src/effect_old.cpp:1295, 1328, 1359
```

Direction↔vector helpers also use byte angles: `ConvertToVector(angle, speed,
vec)` (`src/object.h:41`) and `ConvertToFacing(pos, target)` (`src/object.h:42`).
`TEffect::GetAngle` resolves aim via `ConvertToFacing` (`src/effect_old.cpp:449`).

> There is a degree-domain overload `GetFace(scaledArg)` seen as
> `GetFace(f*360)/256` in some old bodies (e.g. `src/effect_old.cpp:799`); treat
> the underlying stored facing as the 0–255 byte angle.

### 5.3 How quad orientation is expressed in the transforms

The original does **not** carry an "orientation enum." Orientation is *implicit*
in the `rot.x` the `Render` body sets each frame: tip to ground (`-π/2`) for
WorldXY (ground rings, ripples, halos, scatter), leave upright for screen-facing
glows. To classify a quad's orientation in forensics, read the `obj->rot.x` (or
the `D3DMATRIXRotateX` argument) in that effect's `Render`.

---

## 6. Per-vertex color packing

Effect color is written **per vertex** into the diffuse field of a lit-vertex
(`D3DLVERTEX` / `S3DLVertex`). Two packers appear:

| packer | signature | output layout | cite |
|--------|-----------|---------------|------|
| `D3DRGBA(r,g,b,a)` | floats 0..1 → packed ARGB8 | A<<24 \| R<<16 \| G<<8 \| B | used throughout `effect_old.cpp` (e.g. `:6713`, `:12896`, `:13511`) |
| `D3DRGB(r,g,b)` | floats 0..1, alpha implicit | RGB packed, opaque | water/waterfall `effect_old.cpp:11809,12012` |
| `PackARGB(r,g,b,a)` | floats 0..1 → ARGB8 (port equivalent of `D3DRGBA`) | `(a<<24)\|(r<<16)\|(g<<8)\|b`, round-half-up, clamp 0..255 | `src/charanimator.cpp:19-27`; used by weapon swipe `src/weapontrail.cpp:442` |

The diffuse is written to `obj->lverts[i].color` (or `.diffuse`) and, under the
MODULATE texture stage (§1.1), multiplies the sampled texel — so the per-vertex
color *is* the effect's color/brightness/alpha curve.

### 6.1 Color / alpha curve patterns

- **Brightness ramp from a scalar.** Fire/vortex pack a single intensity into all
  three channels: `D3DRGBA(min(0.15f*r,1), min(0.15f*r,1), min(.5f*r,1), 0.0f)`
  (`src/effect_old.cpp:12896,12930,...`) — a blue-biased glow whose magnitude is
  `r` and whose **alpha is 0** (relying on additive ONE/ONE so alpha is moot).
- **Ambient-mixed (LitFlat).** Water folds `GetAmbientLight()/255` into `r` then
  `g=b=r` (`src/effect_old.cpp:11801-11810`, §2).
- **Trail fade envelope.** The weapon swipe fades both color toward the base
  swipe color and alpha toward 0 along the ribbon length:
  `alpha` starts `0.3`, decremented by `fadeoutstep = start*2/maxverts` per vert;
  `startr/g/b` decremented by `wfade = 4/maxverts` toward `r/g/b`
  (`src/weapontrail.cpp:425-461`). This is the per-vertex *envelope* of a trail
  (NOMENCLATURE §6 *envelope/curve*).
- **Per-ring color stops.** The fireball shockwave sets four ARGB color stops
  with varying alpha (`SetRingColor(0..3, D3DRGBA(...))`,
  `src/missileeffect.cpp:728-731`) — a 4-key color curve across the ring's life.

### 6.2 NormalizeColors-style helpers

`TWeaponSwipe::NormalizeColors` (`src/weapontrail.cpp:536-551`) divides r/g/b by
their channel-max so the brightest channel becomes 1.0 (preserving hue, maxing
saturation), then clamps to 1.0. This is how the sword swipe takes a character's
raw `swipecolor` and makes it a fully-saturated tint. `ChangeColor` re-runs it
(`:553-559`).

---

## 7. Math helpers (`math3d.{h,cpp}`)

These replace the retired `D3DMATRIX*`/`D3DVECTOR*` helpers with **identical byte
layout / semantics** (`src/math3d.h:6-11`), so the original `D3DMATRIX*` calls in
the gated bodies map 1:1.

| helper | does | cite |
|--------|------|------|
| `MtxClear(m)` | set identity | `src/math3d.cpp:26-30` |
| `MtxMultiply(d,a,b)` | `d = a * b` (may alias) | `:32-47` |
| `MtxTranslate(d,v)` | right-mul translate, `d = d*T` | `:49-56` |
| `MtxMove(d,v)` | translate by `-v` | `:58-65` |
| `MtxScale(d,v)` | right-mul scale | `:67-74` |
| `MtxRotateX/Y/Z(d,a)` | right-mul rotation about axis (radians) | `:76-110` |
| `MtxTransform(m,v,d)` | `d = v * m` (row-vector transform) | `:112-120` |
| `MtxInvert(d,a)` | transpose-as-inverse (pure rotations only) | `:122-146` |
| `MtxSetRotation(m,dir,up)` | orient: `dir`→new z-axis, `up`→new y-axis | `:148-173` |
| `VecNormalize(v)` | unit vector (no-op on zero) | `:175-187` |
| `VecCross(d,a,b)` | cross product | `:189-195` |
| `Spline(p,t,p1,p2,p3,p4)` | **Catmull-Rom** point on the segment between `p2` and `p3` at `t∈[0,1]` | `:197-212` |

`Spline` is the Catmull-Rom basis (`m1..m4` weights, `0.5*` factor,
`src/math3d.cpp:204-207`) used to smooth strips/trails through control points (§8).
Matrix convention is **right-multiply local-into-dst** (`d = d * T`) and
**row-vector** transform (`result = v * m`) — see the header note
(`src/math3d.h:9-11`).

In the gated 1998 bodies the equivalent calls are spelled `D3DMATRIXClear`,
`D3DMATRIXScale`, `D3DMATRIXRotateX/Y/Z`, `D3DMATRIXTranslate` (e.g.
`src/effect_old.cpp:786-805`) and `D3DRGBA`/`D3DRGB` (§6) — same semantics.

---

## 8. Strip / billboard / trail / beam geometry

### 8.1 Billboard quads (the GetVerts/GetFaces 4-vert pattern)

A billboard is a 4-vertex quad. Animators build the quad's UVs and let the
transform (§5) orient/scale it. Vertex order is consistent across effects:
verts 0,1,2,3 with UVs picking an atlas cell. E.g. fireball
`SetAnimFrame(frame,obj)` writes the four `tu/tv` from a frame index
(`src/missileeffect.cpp:761-779`), Quicksand at `src/effect_old.cpp:9255-9262`,
Vortex at `:12727-12734` (the latter inset by `.05` to avoid edge bleed).

### 8.2 Strip / ribbon (`TStripAnimator`, lightning)

The connected quad-strip builder is `TStripAnimator` (`src/effectcomp.cpp`),
driven by the lightning effect (`src/stripeffect.h:7-11`). Geometry shape:

- **2 verts per anchor point**, `(curpoints-1)*2` triangle faces:
  `obj->numverts = curpoints*2`, `obj->numfaces = (curpoints-1)*2`
  (`src/effectcomp.cpp:802,807,944-945`).
- **Triangle-list winding** built once in a `v % 4` switch
  (`src/effectcomp.cpp:816-849`): the four cases lay down two triangles per
  segment with index offset `4*(v/4)` advancing down the strip.
- **`GenerateStrip(angle)`** (`src/effectcomp.cpp:873-920`) is the per-frame
  build: for each point it offsets the two verts perpendicular to the strip by
  `±(z * myx[ang], z * myz[ang])` where `z` is the (tapering) half-width and
  `myx/myz[ang]` is a precomputed perpendicular direction from the strip angle
  (quantized to `angle/32`). Width tapers hilt→tip via
  `sz = (startsize-endsize)/maxpoints * curpoints / (maxpoints-1)`
  (`:883-888`, set by `SetWidth(start,end)` `:1009-1012`).
- **Growth / shrink:** `AddPoint(p)` appends an anchor (`:922-947`),
  `DelEndPoint` / `DelStartPoint` trim (`:950-987`) — the LAUNCH/FLY/EXPLODE
  state machine grows then shrinks the strip (`src/stripeffect.h:33-55`).
- **Per-vertex jitter:** the lightning animator re-randomizes every
  `SMOOTH_SIZE`-th anchor by `random(-13,13)` each frame and interpolates the
  intermediate anchors, giving the crackle (`src/stripeffect.cpp:741-809`;
  `SMOOTH_SIZE` smoothing).
- **Smooth subdivision:** `maxpoints *= SMOOTH_SIZE` (`src/stripeffect.cpp:638`)
  inserts `SMOOTH_SIZE` interpolated points between hard anchors.

### 8.3 Strip UV scrolling (`ScrollTexture` / `SetTextureRange`)

A strip animates its texture by scrolling U along its length, **not** by swapping
frames:

- `SetTextureRange(startu, endu)` sets the U range and per-point frequency
  `ufrequency = (endu-startu)/maxpoints` (`src/effectcomp.cpp:852-861`).
- `ScrollTexture(deltau)` advances `uoff` by `deltau` and wraps within
  `[ubeg,uend]` (`src/effectcomp.cpp:863-871`).
- In `GenerateStrip`, each point's `tu` is `pnt*ufrequency + uoff` (wrapped)
  (`src/effectcomp.cpp:908-915`).
- The lightning animator calls `mystrip->ScrollTexture(-0.1)` every frame
  (`src/stripeffect.cpp:817`) → the bolt's texture streams along its length.

This is the **UV-coordinate animation** mechanism (see §8.6) for strips.

### 8.4 Trails — ring-buffer of past positions

Two distinct trail constructions:

**(a) Mesh/billboard trail (fireball).** A fixed-length array shifts each frame so
older entries are stale positions, with size decaying down the trail:

```
for i = FIREBALL_TRAIL_SIZE-1 .. 1:        // src/missileeffect.cpp:627-630
    trail[i] = trail[i-1];                  //   shift older
    trail[i].scale *= FIREBALL_TRAIL_SCALE; //   shrink (0.85, missileeffect.h:156)
trail[0] = fireball;  trail[0].pos += effect_pos; trail[0].scale *= 0.85  // :632-637
```

`FIREBALL_TRAIL_SCALE = .85f` (`src/missileeffect.h:156`),
`FIREBALL_TRAIL_SIZE` from `FIREBALL_TRAIL_NUM` (`:169-171`). `IsTrail()` checks
whether the ring's head and tail differ (i.e. the ball moved)
(`src/missileeffect.cpp:781-791`). Each trail entry is drawn as its own
glow quad (`RenderFireBallBurst`-style, `:793-829`).

**(b) Spline ribbon trail (weapon swipe).** `points[2][maxpoints]` is a
double-rail ring (rail 0 = hilt, rail 1 = tip). `CycleStrip()` shifts every rail
back one slot each frame (`src/weapontrail.cpp:561-577`); `Animate()` writes the
current weapon hilt/tip into slot 0 (`:495-501`). `GenerateStrip()` then runs
`Spline` (Catmull-Rom, §7) through the rail points with `smooth` subdivisions,
packing per-vertex color+alpha that fades down the trail (`:415-463`, §6.1). The
hilt/tip are picked from the weapon mesh's extreme verts along Z by
`GetWeaponExtents` (`:504-534`).

### 8.5 Beams — two-endpoint stretch (ice-bolt)

A beam is a mesh stretched between two points by scaling along its length axis.
`TIceBoltAnimator::Render` (`src/effect_old.cpp:8464-…`) draws the freeze beam as
I3D cylinder meshes:

- Core cylinder(s): `scl.z = length/64.0f` (`src/effect_old.cpp:8485`) stretches
  the unit cylinder to the beam length; `scl.x/y = cylscale/2` set thickness;
  tipped with `D3DMATRIXRotateX(π/2)` (`:8488`) so the cylinder lies along the
  beam axis.
- Wrapped detail: spiral meshes stepped down the length (`numrevs =
  length/100`, `ypos = -j*zscale`, `src/effect_old.cpp:8513-8538`) and rings
  spaced along it (`for i in 0..length/100`, `:8548-…`).

So the beam length lives in a single scale factor (`length/64`) on a unit mesh,
plus repeated decorative meshes positioned along the axis — the IM-pipeline
"two-endpoint stretch" (NOMENCLATURE *StretchedAlongVelocity* is the particle
analogue; the ice beam is a mesh stretch, not a billboard).

### 8.6 Texture animation: three mechanisms (tell them apart)

The original animates textures **three** different ways. Forensics must identify
which:

1. **UV-coordinate scroll** — incrementally offset `tu`/`tv` over time so the
   texture *streams* across a static quad/strip. The author confirms this was
   used **extensively**. Tells / cites:
   - Per-frame additive scroll: `obj->verts[n].tu += u` (SymGlow, `u =
     random(2,8)/100` per frame, `src/effect_old.cpp:4632,4652`); `obj->verts[n].tu
     += du` (FaultFire, `du = random(2,8)/100` per frame in `Animate`, `:11176-11179`);
     `obj->lverts[i].tu -= 0.06f` (Quicksand, `:9350`); `obj->lverts[i].tu -= 0.02f`
     (Vortex, `:12812`); `o->lverts[i].tv -= 0.1f` (`:9162`).
   - Setup-time per-vertex bias then drift: SymGlow seeds `o->lverts[v].tv -=
     0.01f` per vert at `SetupObjects` (`src/effect_old.cpp:4601-4602`) then
     scrolls `tu` each Render; FaultFire seeds `o->verts[v].tv -= 0.01f` and
     caches it in `tvs[]` (`:11153-11155`), then *modulates* `tv = tvs[n]*scale`
     with a cosine each Render (`:11193-11196`) — a flame-flicker via UV scaling.
   - Strip U-scroll via `ScrollTexture` (§8.3, `src/effectcomp.cpp:863-871`,
     `src/stripeffect.cpp:817`).
   - Barrier scrolls both axes: `lverts[v].tu/tv += 0.01f`
     (`src/effect_old.cpp:6177-6178`).
2. **UV atlas-cell pick (flipbook via UV sub-rect)** — choose a cell of a tiled
   atlas by computing `tu/tv` from a frame index, *without* swapping textures.
   E.g. fireball `SetAnimFrame`: `u = (frame%4)*.25; v=(frame/4)*.25` selects a
   cell in a 4×4 atlas and writes the four corner UVs
   (`src/missileeffect.cpp:761-779`). Distinguish from (1): here UVs **snap** to
   discrete cells per frame index, rather than drifting continuously.
3. **Per-frame texture-handle swap (framehtexs flipbook)** — the imagery holds an
   array of engine texture handles, one per animation frame
   (`S3DTex::framehtexs`, `src/3dimage.h:50`; current frame selected via
   `framenum`/`htexture`, `:53-55`); `SetTextureFrame(texnum, framenum)`
   (`src/3dimage.h:284`) advances which handle is bound. This is a true texture
   swap, NOT a UV change. (NOMENCLATURE §6 *flipbook/atlas frame* note: some
   Revenant assets use `framehtexs[]`, not a UV sub-rect atlas.)

> **How to tell them apart in source.** (1) mutates `…->tu`/`…->tv` over time;
> (2) recomputes `tu`/`tv` from a frame index but the values are quantized to
> cell boundaries; (3) calls `SetTextureFrame` / sets `framenum` and never
> touches `tu/tv`. Many effects combine them (e.g. an additive glow that both
> scrolls UVs *and* swaps frames).

> **Caution — `framenum` overload.** In the gated animator bodies a local
> variable named `framenum` is usually a *timeline counter* driving the effect's
> state machine (e.g. `TFireFlashAnimator`, `src/effect_old.cpp:2169-2386`), NOT
> the texture frame index. The texture frame index is `S3DTex::framenum`
> (`src/3dimage.h:48`) reached via `GetTextureFrame`/`SetTextureFrame`. Don't
> conflate them.

---

## Gaps / could-not-determine

- **`SetBlendState` texture-stage op vs NOMENCLATURE.** The 1998 body sets
  `D3DTBLEND_MODULATE` (`src/effect_old.cpp:223`), not `D3DTBLEND_DECAL` as
  NOMENCLATURE §3 states. Flagged in §1.1; recommend NOMENCLATURE be corrected.
- **A "None" (no depth-test) effect** — not found in the gated source; every
  effect mode keeps `ZENABLE=true` (§3.1).
- **Retail (Ghidra) blend confirmation.** Blend taxonomy here is from the
  pre-release helper bodies. Where a class is mapped in `recon/classes/`,
  cross-check the retail render-state calls (the strip note at
  `src/stripeffect.cpp:362` already cites retail's ONE/ONE for lightning).
- **`framehtexs` advance cadence** — the per-frame texture-swap mechanism exists
  (`src/3dimage.h:50-55`), but which specific effects drive it (vs. UV animation)
  was not exhaustively traced; per-effect forensics must confirm per asset.
