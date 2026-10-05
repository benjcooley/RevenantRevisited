# Lighting Fidelity — Retail Lighting Model vs the Port

Status: forensics complete for the static-tile path and the 3D-object path;
Classic mode implements both (see §6). Open items needing a retail
reference are listed in §7.

This document records what the shipped game does when it lights the map and
the 3D objects on it, with every claim tied to the 1998 snapshot
(`/Users/benjamincooley/projects/Revenant/`, "1998" below) and confirmed
against the retail binary in Ghidra (`Revenant.exe`, project `RevenantDev`).
It then lists where the port diverged and what Classic mode does now.

The trigger was the New Game opening scene (the Keep, level 2, `--quickstart`):
the room read flat and evenly dim, the wall torch had no pool of light, Locke
was strongly orange, and debug green showed through the pit and the void.

Companion docs: [DEFERRED_LIGHTING.md](DEFERRED_LIGHTING.md) (depth and
world-position recovery), [RENDERER_ARCHITECTURE.md](RENDERER_ARCHITECTURE.md).

## 1. Retail has two tile-lighting paths, chosen by `RealTimeLight`

Retail `TMapPane::DrawUpdateRect` is `FUN_00456810` (1998 `mappane.cpp:3383`).
It branches on `TMapPane+0x9a8`, which is `RealTimeLight`:

- `FUN_00454390` copies `DAT_005d7a18` into `+0x9a8` through `FUN_0045b080`;
  `FUN_0044dca0` does the same at map-pane init.
- `DAT_005d7a18` is `[Options] RealTimeLight` (`FUN_00484ae0`, the Options
  reader). Its `.data` default is 1, and the GOG `Revenant.ini` sets `Yes`.

| `RealTimeLight` | Path | Functions |
| --- | --- | --- |
| No  | **DLS software lighting**: per-pixel, depth-aware, no normals | `DrawAmbientLight` `FUN_0043c7c0`, per-object `DrawLight` `FUN_00471930`, `TransferAndLight32to16` dispatcher `FUN_0043d1f0` |
| Yes (default) | **Light grid**: per-vertex light on a screen grid, drawn through Direct3D over the unlit buffer | `FUN_0045d260` → grid build `FUN_0045bd50` → grid draw `FUN_0045c8a0` |

The port's rendering direction is the DLS vision as GPU shaders
([PORT_PLAN.md](PORT_PLAN.md), "Renderer fork"). That plan calls DLS the
renderer "too slow to ship", but the decomp shows otherwise: **the DLS path
ships in retail and runs whenever `RealTimeLight=No`.**
Classic mode therefore reproduces the `RealTimeLight=No` image. The grid path
uses the same falloff table and the same distance metric (§3.4), but its
final blend could not be closed from the decomp, so it isn't reproduced.

Confidence that the 1998 DLS source matches retail: **high**. Every
function below was matched instruction-for-instruction or constant-for-constant.

## 2. Retail DLS tile path (RealTimeLight=No)

### 2.1 Buffers

The unlit buffer is 32 bits per pixel: the low 16 bits are the tile colour
(565/555), the top byte is light: 6 bits of intensity (`I`, 0..63) and 2
bits of light-table id (1998 `dls.cpp:74`).

1. **Clear.** `DrawUnlitObjects` (`FUN_00456cc0`, 1998 `mappane.cpp:3443`)
   boxes the update rect with colour **0** and z `0xFFFF`. Flags select
   `DM_NODRAW` only when `ClearBeforeDraw == 0 && level == 0`; the
   `ClearBeforeDraw` default (`DAT_005d79f4`) is 1. **Uncovered pixels are
   black on every level.**
2. **Ambient pass.** `DrawAmbientLight` (`FUN_0043c7c0` → `FUN_0043c700`)
   zeroes the light byte. Ambient isn't drawn; it lives in the light table.
3. **Static lights.** For every light-bearing object in the rect,
   `TObjectInstance::DrawLight` (`FUN_00471930`, 1998 `object.cpp:1494`):
   - `slpos = WorldToScreen(pos + lightdef.pos)`
   - `lightid = NewLightIndex(lightdef.color, lightdef.multiplier)`
     (`FUN_0041d890`, 1998 `colortable.cpp:186`: tolerance 3, a multiplier of
     0 or less becomes **28**)
   - `DrawStaticLightNoNormals(slpos, color, intensity, surface, lightid)`
     (`FUN_0043c5a0`, 1998 `dls.cpp:496`). The normals variant is commented
     out even in 1998 (`object.cpp:1513-1515`).
4. **Transfer.** `TransferAndLight32to16` (`FUN_0043d1f0` → `FUN_0043cf10` /
   `FUN_0043c860` / `FUN_0043cbd0`) maps each pixel through `LightTable`
   (`DAT_0063da60`) or `MMXLightTable` (`DAT_00635144`).

### 2.2 Per-pixel light accumulation

The light block is `FUN_0043be50` (1998 `dls.cpp:74`). It reads `DistTable`
@`0x5e9200`, `IntTable` @`0x656620` and `LightDropOffTable` @`0x62a240`,
exactly as the 1998 asm does:

```
dxy   = DistTable[|Δsx|][|Δsy|]            screen-pixel offset from slpos
dz    = |zbuf(pixel) − slpos.z|            skip if dz > intensity
d     = DistTable[dxy][dz]                 = round(sqrt(dxy² + dz²))
normd = d + IntTable[d] ≈ d·254/intensity  skip on 8-bit carry (normd > 255)
byte  = (byte & 0xFC) + LightDropOffTable[normd]   saturate to 0xFC on carry
```

`LightDropOffTable[n] = (int(BrightnessTable[n]·63) << 2) | id`
(1998 `colortable.cpp:459`). The 6-bit intensities of overlapping lights
**add, saturating at 63**. The 2-bit id is overwritten by each light, so the
light drawn last wins the colour.

`BrightnessTable` (`FUN_0041dff0` MakeColorTables, 1998 `colortable.cpp:350`):
`exponent` −1.1 @`0x5c6e60`, `maxbrightness` 50.0 @`0x5c6e58`,
`minpower = pow(256, −1.1)`:

```
B(n) = min(1, 50·(pow(n+1, −1.1) − minpower) / (1 − minpower))
```

### 2.3 The distance metric is the retail iso basis, not Euclidean world

`Δsx, Δsy` are screen pixels and `dz` is the 16-bit screen-z
(`WorldToScreen` / `WorldToScreenZ`, 1998 `object.cpp:61-80`). For a world
delta `(Δx, Δy, Δz)`:

```
Δsx = Δx − Δy
Δsy = (Δx + Δy)/2 − 0.867·Δz
Δsz = −Δz/2 − 0.867·(Δx + Δy)
d²  = Δsx² + Δsy² + Δsz²  =  (Δx−Δy)² + 1.0017·((Δx+Δy)² + Δz²)
```

So `d ≈ sqrt(2Δx² + 2Δy² + Δz²)`. On the floor a light reaches only
`intensity/√2` world units; vertically it reaches `intensity`. This is a
fixed linear map of the world delta. It doesn't depend on the camera, pan or
viewport, so evaluating it on reconstructed world positions keeps the
world-space invariant of [DEFERRED_LIGHTING.md](DEFERRED_LIGHTING.md).

### 2.4 The light table (ambient and light gain)

`SetLightColor` is `FUN_0041da10` (1998 `colortable.cpp:236`). Retail
constants: 63.0 @`0x5a3a10`, 31.0 @`0x5a3a18`, 255.0 @`0x5a3a20`, 80.0
@`0x5a3a28`, `multiplierscale` 20.0 @`0x5c6e68`, `AmbientMultiplier` 1.0
@`0x5c6e70`, `LightMultipliers[]` default 28 @`0x5c6e78`. Ambient enters only
through `::SetAmbientLight` / `::SetAmbientColor` (`FUN_0041d730` /
`FUN_0041d780`), called from the map pane's `ambientchanged` block
(`0x4541ec`, 1998 `mappane.cpp:2635`). Neither applies `Ambient3D`.

With `A` = AMBLIGHT (the int `TMapPane::ambient`), `a` = AMBCOLOR normalised so
its largest channel is 1, `l` = the light colour normalised the same way,
`m` = the light's multiplier and `clr = 8·c5` for a 5-bit colour channel `c5`:

```
out5 = min(31, clr·a·(A/255)  +  clr·l·(1 − A/255)·(I/63)·(m/20))
```

`clr·k = c5` when `k = 1/8`, so in normalised colour this is:

```
lit = min(1, albedo · ( (8A/255)·a  +  8·(1 − A/255)·(m/20)·l·(I/63) ))
```

- **Ambient gain** is `8A/255`, so `AMBLIGHT 32` is 1.0 (identity).
  Daytime outdoors (Forest/Misthaven `AMBLIGHT 30`, white) is 0.94. The Keep's
  `AMBLIGHT 4` gives 0.125 × (0.74, 0.74, 1.0), almost black with a blue cast.
- **Light gain** at full intensity is `8(1 − A/255)·m/20`. That is 4.72 for the
  Keep's `m = 12` lights and 11.0 for the default 28. Gain isn't capped; only
  the output channel saturates. A pixel is fully lit once `I/63 ≳ 1/gain`,
  which makes the bright, saturated pools retail shows.
- No N·L term on tiles (`NoNormals`).

## 3. Retail 3D-object path (characters and meshes)

Meshes are lit by Direct3D vertex lighting in `T3DScene` (1998
`3dscene.cpp`). The retail code changed several 1998 values.

### 3.1 INI (`GetINISettings` `FUN_00484500`, `[Lighting]`)

| Key | Retail default when missing | Global | Port before this change |
| --- | --- | --- | --- |
| `MaxLights` | **3** | `DAT_005c61b4` | 1 |
| `Ambient3D` | **130** | `DAT_005c61c0` | 100 |
| `LightRange3D` | 180 | `DAT_005c61c4` | 180 |
| `LightMult3D` | 250 | `DAT_005c61c8` | 250 |

`EnhancedLighting` (`DAT_005e91c0`, default 0) and `RealTimeLight`
(`DAT_005d7a18`, default 1) are **`[Options]`** keys (`FUN_00484ae0`). The port
read `EnhancedLighting` from `[Lighting]` with default `true`.

`LightRange3D` is used in exactly one place: `GetClosestLights`
(`FUN_00415b40`) divides every candidate distance by it before sorting.
That doesn't change the order, so **it has no visible effect in retail**.
`LightMult3D` is used only by `T3DLight::GetBrightness` (`FUN_00411b70`).
**Neither touches the tile path.**

### 3.2 Ambient

`T3DScene::SetAmbientLight` / `SetAmbientColor` (`FUN_00414310` /
`FUN_004143d0`), and the render-state setup in `FUN_00412db0`:

```
a256_c  = min(255, (AMBCOLOR_c << 8) / max(AMBCOLOR))
k       = 8 / s     s = 4 (EnhancedLighting + MODULATE4X caps),
                        2 (EnhancedLighting + MODULATE2X caps), else 1
level   = Ambient3D · A · k / 100               (integer)
chan_c  = min(255, (level · a256_c) >> 8)       (D3D colour byte)
ambint  = (avg_c of the same with k = 8) / 255  (FUN_00414310 → DAT_005e8914)
```

`UseDirLight` (`DAT_005c61b8` = 1) and `DirLightPercent` (`DAT_005c61bc` = 85)
split this between the D3D ambient and a directional light (`FUN_00412db0`):

```
D3D ambient   = chan_c · 15/100
dir colour_c  = min(1, chan_c · 0.85 · F / 256)   F = 1.5 if MaxLights && RealTimeLight, else 3.0
dir direction = (0, −0.78125, −0.625)  world space (scene init FUN_00411eb0, 1998 3dscene.cpp:431)
```

The view matrix (1998 `3dscene.cpp:590`, `SetCameraPos`) is the camera
transform, so D3D light vectors are in Revenant world space. The light comes
from `+y`, above: screen lower-left, over the viewer's shoulder.

`EnhancedLighting` selects texture-stage `MODULATE2X/4X` (`FUN_00417d60` →
`FUN_00417390(stage, D3DTSS_COLOROP, 5|6)`) and divides every light input by
`s`. The net effect is that the light sum can overbright to `s` instead of 1.

### 3.3 Point lights

`T3DScene::AddLight` (`FUN_00415790`) builds a `D3DLIGHT_POINT` with range
`1.3·intensity` (@`0x5a3838`), falloff 0.07 and attenuation (0.1, 0.8, 1.0).
The 1998 source used range 1e6 and attenuation (1, 0, 0). The colour is
normalised to max 1 (`FUN_004118e0`).

For each object, `LightAffectObject` (`FUN_00415c70`) takes the `MaxLights`
nearest lights. Each one's colour is set to `l · b`, where `b` is
`T3DLight::GetBrightness` (`FUN_00411b70`) evaluated at the object position:

```
d = |object − light|                      (world units, Euclidean)
b = 0                                      if d ≥ intensity
b = 0.2·(intensity − d)/intensity · m · LightMult3D · 0.01
b *= 1/s                                   (EnhancedLighting, see above)
b = clamp(b − ambint, 0, 1)
```

This falloff is **linear**, not the tile `pow` curve, and it saturates. With
`m = 12` and `LightMult3D = 250`, the raw value is `6·(1 − d/r)`, so a
character is fully lit out to 83% of the radius.

### 3.4 For reference: the realtime light grid (RealTimeLight=Yes)

`FUN_0045bd50` computes light per grid vertex with the same `DistTable`
screen metric and the same `BrightnessTable`, through
`GetLightBrightness(1.375·d, intensity, m)` (`FUN_0041d7f0`), which returns
`B·m/10`. Each vertex accumulates `K·A·a` for ambient and
`colour_byte·B·m/10·K` per light, with `K` = 6 / 3 / 1.5 for
MODULATE / 2X / 4X (@`0x5a4a70`, `0x5a36dc`, `0x5a3810`). So grid lights
reach 1/1.375 as far as DLS lights, and they don't use normalised colour. The
grid draw (`FUN_0045c8a0`: two-pass multitexture, a 0x200 per-vertex
threshold) wasn't closed, so the grid's absolute output scale is unknown.

## 4. What the port did, and why the Keep looked wrong

All of these are in `TMapRenderer::RenderFrame` (`src/maprenderer.cpp`) and
the light shader (`src/shaders/light.*.h`).

| # | Retail | Port before | Effect in the Keep |
| --- | --- | --- | --- |
| F1 | Uncovered pixels are black (`FUN_00456cc0` colour 0) | Lit-pass clear `(0.12, 0.16, 0.10)`, a debug green | Green through the pit and the off-map void |
| F2 | No sun in either path | Sun from `TimeOfDay()` in every area. The light shader applies it only in mode 1, but FX LitFlat particles always get it | Inert in Classic tiles; would light interiors in mode 1 |
| F3 | Tile ambient `8A/255 · a/max(a)`, no `Ambient3D` | `A·Ambient3D/(20·100) · a/255`, with the divisor chosen by eye | 0.2 instead of 0.125 at A=4, and the colour isn't normalised |
| F4 | Tile light gain `8(1−A/255)·m/20 · l/max(l)`, uncapped | `m/100 · LightMult3D/100 = 0.3`, raw colour, capped at 1.5 | Torch pools **~16× too dim** |
| F5 | Tile radius `intensity` in the iso screen metric (floor reach `r/√2`) | `intensity · LightRange3D/100 (1.8)`, Euclidean world | Pools **~2.5× too wide**, so dim light is smeared over the whole room ("flat, evenly dim") |
| F6 | Tile intensity sum saturates at 63 (`I ≤ 1`); one colour per pixel | Per-light colours summed | Only matters where differently-coloured lights overlap |
| F7 | Tiles: no N·L | `mix(1, N·L, 0.5)` on tiles | Speckled half-lambert on floors |
| F8 | Meshes: the 3D model of §3 (linear `b`, `LightMult3D`, `Ambient3D`, dir light) | Meshes lit exactly like tiles | Locke got the dim world-space tile falloff, so his orange read against a dim, flat, almost unlit floor |
| F9 | `MaxLights` default 3, `Ambient3D` default 130 | 1, 100 | Defaults only; the shipped INI sets `Ambient3D=100`, `MaxLights=3` |
| F10 | `EnhancedLighting` / `RealTimeLight` are `[Options]` keys | `EnhancedLighting` read from `[Lighting]`, default `true`; `RealTimeLight` not read | — |
| F11 | — | `[Revisited] LightingMode` reached only `TRenderer`, and `TMapRenderer` overwrote it every frame | `--revisited` couldn't select the modern model |

Why Locke is orange: the two nearest lights to the pit
(`(1178,566,60)` and `(1316,564,60)`, `int 220`, `mult 12`, colour
`(255,100,0)`, class `TILE`) are orange. Retail lights him with
`b = clamp(6·(1−d/r) − ambint, 0, 1) = 1`, i.e. full orange on lit faces. So
the hue is retail-correct. What was wrong is that the floor around him wasn't
lit to match: in retail the floor at his feet has `ΣB ≈ 0.27` → red gain ≈ 1.3,
a saturated orange pool. In the port it got a dim, wide wash.

## 5. Revisited (mode 1)

Mode 1, "modern", is unchanged: sun plus AO plus normals plus the world-space
`pow` falloff, with the eyeballed ambient divisor. It is a Revisited art path,
not a fidelity claim. Two corrections apply to it:

- **Sun only where retail has a day/night cycle.** The sun is enabled only in
  areas with `AREA_DONIGHT` (the area defines `NIGHTAMBLIGHT`). The Keep has
  no night values, so it gets no sun. Classic never applies a sun.
- `[Revisited] LightingMode` now reaches `TMapRenderer`.

## 6. Classic mode (mode 0) after this change

The G-buffer normal target's alpha now carries the **surface class**:
1 = tile (DLS model), 0 = mesh (3D-scene model). Sector lights are submitted
as *retail lights* (authored radius, authored colour, retail multiplier).
Lights added through `AddPointLight` (VFX) stay *direct lights* with
modern semantics.

Tiles (§2, exact apart from 5-bit quantisation):

```
for each retail light: d = iso screen metric; if d < r: B6 += floor(63·B(d·254/r))/63
I     = min(1, ΣB6)
light = (8A/255)·â + I · Σ(B6_i · l̂_i · m_i) / ΣB6 · 8(1−A/255)/20
lit   = saturate(albedo · light)
```

The `ΣB6`-weighted colour average replaces "last light drawn wins the id".
They are identical for a single light, and for lights that share a colour
and multiplier (true of every overlap in the Keep).

Meshes (§3):

```
light = D3D ambient + dir colour · max(N·L_dir, 0)
      + Σ_retail l̂_i · s·clamp(b_i/s − ambint, 0, 1) · max(N·L_i, 0)
lit   = albedo · min(light, s)
```

Both models live in `src/shaders/lightmodel.*.h` (`shade_surface`). The
deferred light pass and the translucent mesh pass (a character fading in or
out) both call it, so a fading mesh is lit exactly like an opaque one
([RENDERER_ARCHITECTURE.md](RENDERER_ARCHITECTURE.md), "Translucent meshes").

Known simplifications for meshes, each to confirm with a retail shot (§7):
`b` is evaluated per pixel instead of at the object origin; every light in
range counts instead of the `MaxLights` nearest; the D3D distance attenuation
(0.1, 0.8, 1.0) is taken as 1 inside the range, because its DirectX 5
semantics can't be settled from the binary; `F` uses `RealTimeLight=No` (3.0),
to match the tile path Classic renders.

## 7. Open items — dosbox-x reference shots that would settle them

All shots: New Game → the opening Keep scene, Locke in the resurrection pit,
default camera, before he moves. Take each pair with the Options menu or the
INI `[Options]` set as stated. Capture lossless (PNG) at 640×480.

1. **S1 — DLS baseline:** `RealTimeLight=No`, `EnhancedLighting=No`. Settles
   the absolute tile ambient (expect near-black blue-grey walls), the torch
   pool radius and saturation, and the black pit. Compare 1:1 with Classic.
2. **S2 — DLS + enhanced:** `RealTimeLight=No`, `EnhancedLighting=Yes`.
   Shows Locke's overbright (`s`) and the dir-light share on his far side.
3. **S3 — shipped default:** `RealTimeLight=Yes`, `EnhancedLighting=Yes`.
   The grid look most players remember. Settles whether Classic should also
   offer the grid's `1.375` radius and `K` gain (§3.4).
4. **S4 — D3D attenuation:** in S2's setup, walk Locke from the pit towards the
   wall torch, taking a shot every ~1 character width. His brightness as a
   function of distance distinguishes "constant inside range" from the DX5
   normalised-distance formula.
5. **S5 — dir-light direction:** an outdoor shot (Misthaven, daytime,
   `RealTimeLight=No`) with Locke facing each of the four screen diagonals.
   Confirms which side of him the `(0, −0.78, −0.625)` key light hits.

## 8. Verification in the port

- `build/test_classiclighting` pins `ComputeClassicLightModel` to the
  formulas above with hand-worked values: `AMBLIGHT 32` gives identity, the
  Keep's ambient is `8·4/255·(0.74, 0.74, 1)`, and the light gain for
  multiplier 12 is 4.72. The mesh tests cover the integer `chan`/`ambint`
  steps, EnhancedLighting, and the key-light direction.
- Opening scene, `--quickstart`, Classic: wherever no light reaches, the blue
  channel came out at exactly 0.76× the previous build (0.1255/0.165, the
  ratio of the two ambient mappings). Uncovered pixels went from
  (30, 40, 25) to (0, 0, 0). Torch and pit pools are saturated orange,
  reaching about `r/√2` on the floor.
- `--revisited` without an overlay INI still resolves to Classic
  (`[Revisited] LightingMode` defaults to 0). With `LightingMode = 1` the
  modern look is unchanged and the Keep gets no sun.
- Misthaven at night (`--quickstart --sector=0_2_25`, AMBLIGHT 35 with
  colour 75,100,255) renders as a saturated blue night, slightly
  overbright (`8·35/255 = 1.10`).

## 9. Related issues not addressed here

- **Effect lights.** VFX ports add their own lights through
  `AddPointLight` with tuned intensities, using the modern falloff in both
  modes. Retail effects lit through their object's `SLightDef`, which runs
  the same light table as map lights. Moving them to `AddRetailPointLight`
  belongs to the VFX track.
- **`T3DLight::GetBrightness` in `src/3dscene.cpp`** still uses the 1998 `pow`
  falloff. Retail `FUN_00411b70` is linear with `LightMult3D` (§3.3). Effect
  code (blood tint, light sampling) calls it.
- **FX LitFlat particles** take the scene ambient and sun from `SLightState`.
  Classic now feeds them the tile ambient and no sun, which matches retail.
  Modern still uses the time-of-day sun only where the area has a day/night
  cycle.
- **Point-light cap.** The light pass takes the 16 lights nearest the view
  centre. Retail DLS draws every light that touches the update rect, with no
  cap.
- **Mesh light selection.** Retail lights each object with its `MaxLights`
  nearest lights (`GetClosestLights`). The light pass evaluates every
  submitted light per pixel.

## 10. Confidence summary

| Claim | Evidence | Confidence |
| --- | --- | --- |
| DLS tile path ships and runs with `RealTimeLight=No` | `FUN_00456810` branch on `+0x9a8` = `DAT_005d7a18` | High |
| Light table maths (ambient `8A/255`, gain `8(1−A/255)m/20`, normalised colours) | `FUN_0041da10` constants = 1998 `colortable.cpp` | High |
| `pow(−1.1)·50` falloff, 6-bit saturating sum, iso screen metric | `FUN_0041dff0`, `FUN_0043be50` = 1998 `dls.cpp` asm | High |
| Uncovered pixels black | `FUN_00456cc0` Box colour 0 | High |
| Mesh ambient / `ambint` / `b` formula, `LightMult3D` | `FUN_00414310`, `FUN_004143d0`, `FUN_00411b70` | High |
| Mesh dir-light share and direction | `FUN_00412db0`, `FUN_00411eb0`, 1998 view matrix | Medium (frame inferred from 1998 `SetCameraPos`) |
| D3D point-light attenuation | `FUN_00415790` values; DX5 semantics unknown | Low — needs S4 |
| Grid path output scale | `FUN_0045c8a0` not closed | Low — needs S3 |
