# Lighting Fidelity — Retail Lighting Model vs the Port

Status: forensics complete for the static-tile path and the 3D-object path;
Classic mode implements both (see §6). Retail shot S1 now exists and Classic
matches it on every static surface to within 3 of 255 per channel (§8).
Open items needing more retail shots are listed in §7.

This document records what the shipped game does when it lights the map and
the 3D objects on it, with every claim tied to the 1998 snapshot
(`/Users/benjamincooley/projects/Revenant/`, "1998" below) and confirmed
against the retail binary in Ghidra (`Revenant.exe`, project `RevenantDev`).
It then lists where the port diverged and what Classic mode does now.

The trigger was the New Game opening scene (the Keep, level 2, `--quickstart`):
the room read flat and evenly dim, the wall torch had no pool of light, Locke
was strongly orange, and debug green showed through the pit and the void.
The first pass (§4, F1–F11) fixed the falloff; the S1 capture then showed an
orange wash over the whole room, a black pit and glowing characters, which
§4's second table (F12–F18) traces to the MMX light table, the gamma ambient
offset, two missing draws and the 3D objects' light set.

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

The unlit buffer is 32 bits per pixel: bytes 0–2 are the tile colour, one
5-bit channel per byte (red, green, blue; step 4), and the top byte is
light: 6 bits of intensity (`I`, 0..63) and 2 bits of light-table id (1998
`dls.cpp:74`).

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
4. **Transfer.** `TransferAndLight32to16` (`FUN_0043d1f0`) picks the pixel
   loop by `IsMMX` (`DAT_006680f8`):
   - **MMX** (`FUN_0043c860` for a 16-bit display, `FUN_0043cbd0` for 15-bit)
     multiplies each colour byte by a byte of `MMXLightTable`
     (`DAT_00635144`, §2.4). `IsMMX` is CPUID leaf 1 EDX bit 23, read at
     boot (`0x004865d7`–`0x004865fc`); the command-line switch `NOMMX`
     (`0x005d8100`) clears it (`0x004841af`). Every Pentium MMX, Pentium
     II/III, K6 and later CPU takes this path, and so does dosbox-x with
     `cputype=pentium_mmx` (the retail lab).
   - **No MMX** (`FUN_0043cf10`) looks each channel up in `LightTable`
     (`DAT_0063da60`). Only pre-MMX CPUs ran it.

   The unlit buffer's colour bytes are 5-bit: `ZPut8` (1998 `graphics.cpp`
   `ZPut8`, the tile draw into the 32-bit buffer) writes the palette's
   `rgbcolors` entry shifted right by 3, and `rgbcolors` holds `c5 << 3` on
   disk. Pixels whose bitmap z is `0x7F7F` are skipped (transparent).
5. **Lit objects.** After the transfer, `DrawLitObjects` (1998
   `mappane.cpp:3489`) draws each object's imagery again through
   `DrawLit`. A 2D imagery state draws in the unlit pass, lit by the lights
   above, only if it has `ANIIM_UNLIT` (2), and in the lit pass, in its own
   colours, only if it has `ANIIM_LIT` (1) (1998 `animimage.cpp`
   `TAnimImagery::DrawUnlit` / `DrawLit`; `ZPutDim` with dim 0, depth
   tested). The retail object thunks are `0x00477ba0` (imagery vtable
   `+0x10`) and `0x00477bc0` (`+0x18`), vtable `0x5a7b98` slots 64 and 65.
   The resurrection pit's Elevator platform is `ANIIM_LIT`; the S1 capture
   shows it at exactly its own colours (§8).

Every object the update rect collects is drawn this way whatever its class:
`GetUpdateObjs` (`FUN_0045f800`, iterator flags `0x3f`) skips only
`OF_INVISIBLE` and moving objects (those draw in the animation pass), and
`DrawUnlitObjects` skips `OF_EDITOR` (`0x100`) outside the editor. Exits,
containers and items lying on the map draw exactly like tiles.

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

`SetLightColor` is `FUN_0041da10` (1998 `colortable.cpp:253`). Retail
constants: 63.0 @`0x5a3a10`, 31.0 @`0x5a3a18`, 255.0 @`0x5a3a20`, 80.0
@`0x5a3a28`, `multiplierscale` 20.0 @`0x5c6e68`, `AmbientMultiplier` 1.0
@`0x5c6e70`, `LightMultipliers[]` default 28 @`0x5c6e78`. Ambient enters only
through `::SetAmbientLight` / `::SetAmbientColor` (`FUN_0041d730` /
`FUN_0041d780`), called from the map pane's `ambientchanged` block
(`0x4541ec`, 1998 `mappane.cpp:2635`). Neither applies `Ambient3D`. The
ambient they receive is `TMapPane::ambient`, which already carries the
gamma offset (§2.5).

With `A` = AMBLIGHT (the int `TMapPane::ambient`), `a` = AMBCOLOR normalised so
its largest channel is 1, `l` = the light colour normalised the same way,
`m` = the light's multiplier, `I` the pixel's 6-bit intensity (§2.2) and
`c5` a 5-bit colour channel, `SetLightColor` fills two tables per light id
and intensity.

**`MMXLightTable`** (the path every MMX CPU and the S1 capture use). One
multiplier byte per channel, truncated twice (`0x0041dbfd`–`0x0041dcd2`):

```
Mamb = int(min(255, 80·a·(A/255)·AmbientMultiplier))
M    = int(min(255, 80·l·(1 − A/255)·(I/63)·(m/20) + Mamb))
```

The transfer (`FUN_0043c860`) multiplies each colour byte by its `M`
(`PMULLW`), saturates the product to a byte (`PACKUSWB`), keeps its top 5
bits (`PAND 0x00F800F8` / `0x0000F800`) and packs 565, doubling red and
green into place (`PADDUSW` with `0x7FE0`):

```
out5 = min(255, c5·M) >> 3          green: g6 = 2·out5 (its low bit is always 0)
```

so a byte of 8 is identity and the gain is `M/8`:

- **Ambient gain** is `int(80·a·A/255)/8`. Identity at `AMBLIGHT` 26–28;
  `AMBLIGHT 32` is 1.25. The Keep at `GammaLevel 3` (`A = 4 + 10`, colour
  155,155,210) has bytes (3, 3, 4): gains (0.375, 0.375, 0.5). Without the
  gamma offset (`A = 4`) the bytes are (0, 0, 1), near black.
- **Light gain** at full intensity is `80·(1 − A/255)·(m/20)/8`: 5.67 for the
  Keep's `m = 12` lights at `A = 14`, 13.2 for the default 28. Only the byte
  saturates (255, a gain of 31.9), so a lit pixel saturates once
  `c5·M ≥ 255`, which makes the bright, saturated pools retail shows.
- No N·L on tiles (`NoNormals`).

**`LightTable`** (pre-MMX CPUs only, `FUN_0043cf10`). A 5-bit lookup per
channel with `clr = 8·c5`, `out5 = min(31, clr·a·(A/255) + clr·l·(1 −
A/255)·(I/63)·(m/20))`: the same terms with 8 in place of 80/8 = 10, no
byte truncation, and green computed at 6 bits. Its ambient and its light
gain are 0.8× the MMX table's. The first Classic model (§4, F3–F4) used this
table; the S1 capture rules it out (§8): 98% of its map pixels have an even
6-bit green, which only the MMX transfer produces.

### 2.5 The gamma ambient offset

`TMapPane::SetAmbientLight` (`FUN_00453640`) stores

```
ambient = max(0, light + (GammaLevel·5 − 10)·2)
```

so level 2 adds nothing, 3 adds 10 and 4 adds 20 to every area ambient
([gameflow/forensics/OPTIONS.md](gameflow/forensics/OPTIONS.md) §7.11).
Its callers:

- area entry (`FUN_0041ba00`, `SetAmbientLight(light, 1)`) when the previous
  ambient isn't nearby, and the day/night update (`FUN_0041b770`,
  `SetAmbientLight(light, 0)`);
- the map's start value: map init `FUN_0044d5c0` inlines it with light 10
  (`GammaLevel·10 − 10`);
- the script's `ambient` command (`0x0042596d`);
- the Options pane's OK (`0x0053afbc`), on `MapPane.ambient`, which already
  carries the offset, so each OK adds it again (question 93).

`FadeAmbient` (`FUN_00453720`), the 3-second cross-fade between nearby
areas, stores its target without the offset; a `SetAmbientLight` during the
fade replaces the target with the offset one.

The S1 capture confirms the offset is in effect at area entry: the ambient
bytes it shows, (3, 3, 4), need `A` in 13–15 (§8).

## 3. Retail 3D-object path (characters and meshes)

Meshes are lit per vertex in `T3DScene` (1998 `3dscene.cpp`). The retail
code changed several 1998 values, and has two rasterisers behind it:

- **Its own software renderer** when `DAT_005d7a28` is set. Its `.data`
  default is 1; scene init (`FUN_00411eb0`, `0x00411edb`) clears it when a
  hardware Direct3D device was chosen (`DAT_00669ad8`), and the command-line
  switch `BLUE` (`0x005d80c0`, `0x004840ef`) sets it. `Software3D=Yes` keeps
  the hardware device off, so the S1 lab (and any machine without a 3D card)
  renders 3D objects this way. `FUN_00412db0` routes the ambient to
  `FUN_0056d570` and the lights to `FUN_0056cf00` / `FUN_0056d120` instead of
  Direct3D. Its vertex lighting (`FUN_0056eb30`, `0x0056ee3f`–`0x0056f0e9`)
  starts from the ambient colour, adds `N·L × colour` for each directional
  light and for each point light within its range (no distance
  attenuation), clamps each channel to 1 and stores it as a 5-bit vertex
  colour (`× 31.0` @`0x5b9cc4`).
- **Direct3D** otherwise (DX5 lighting, with the attenuation of §3.3).

Which lights reach an object is decided before either rasteriser:
`LightAffectObject` (`FUN_00415c70`) always enables the key light
(`UseDirLight`), but adds the object's nearest map lights **only when
`RealTimeLight` is on** (`DAT_005d7a18`, `0x00415ca6`). Under
`RealTimeLight=No`, the configuration Classic renders, a character is lit
by the ambient and the key light alone; the torches don't tint it.

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
It applies on the Direct3D path only: `FUN_00412db0` picks `s` from the
device caps only while `DAT_005d7a28` is 0, so the software renderer always
uses `s = 1`.

### 3.3 Point lights

`T3DScene::AddLight` (`FUN_00415790`) builds a `D3DLIGHT_POINT` with range
`1.3·intensity` (@`0x5a3838`), falloff 0.07 and attenuation (0.1, 0.8, 1.0).
The 1998 source used range 1e6 and attenuation (1, 0, 0). The colour is
normalised to max 1 (`FUN_004118e0`).

With `RealTimeLight` on, for each object, `LightAffectObject`
(`FUN_00415c70`) takes the `MaxLights` nearest lights. Each one's colour is
set to `l · b`, where `b` is `T3DLight::GetBrightness` (`FUN_00411b70`)
evaluated at the object position:

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

The first pass read Locke's orange as retail-correct: the two lights nearest
the pit (`(1178,566,60)` and `(1316,564,60)`, `int 220`, `mult 12`, colour
`(255,100,0)`) give `b = clamp(6·(1−d/r) − ambint, 0, 1) = 1`. That holds only
with `RealTimeLight` on; S1 showed otherwise (F18).

### 4.1 Corrections from shot S1 (2026-10-07)

| # | Retail | Port before | Effect in the Keep |
| --- | --- | --- | --- |
| F12 | Tiles transfer through the MMX table (§2.4): ambient byte `int(80·a·A/255)`, gain `M/8`, 5-bit output | The pre-MMX `LightTable` gains (`8A/255`, `8(1−A/255)m/20`), unquantised | Ambient 0.8× and truncation lost; light 0.8× |
| F13 | `TMapPane::SetAmbientLight` adds `(GammaLevel·5 − 10)·2` (§2.5) | Not ported | The Keep at `A = 4` instead of 14: ambient bytes (0, 0, 1) instead of (3, 3, 4), so unlit stone was black and the orange light was all there was: an orange wash over the room, the cool base gone |
| F14 | The map pane is a 1:1 window onto the 640×480 screen | `ComputeMapCameraViewport` fitted the 640×480 view into the play field (452×420 with the side panel and bottom bar), scale 0.875 | Everything 12.5% small, pools and pit included; no pixel matched the capture |
| F15 | Every object's imagery draws, whatever its class (§2.1) | Only `TILE`-class 2D imagery (and effects) drew | The pit's `EXIT` Elevator platform, chests, bookcases, barrels, vases and items lying on the map were invisible; the pit read black |
| F16 | `ANIIM_LIT` states draw after the light transfer, in their own colours; a state with neither `ANIIM_LIT` nor `ANIIM_UNLIT` doesn't draw (§2.1) | Every 2D state was lit by the light pass | The Elevator platform (`ANIIM_LIT`) would have been lit orange instead of its blue-violet |
| F17 | `OF_INVISIBLE` never draws on the map; `OF_EDITOR` only in the editor | Drawn | — in the Keep |
| F18 | Under `RealTimeLight=No` a 3D object gets ambient and key light only; map lights reach it only with `RealTimeLight` on (§3) | Every map light in range lit meshes | Locke and the NPCs glowed orange; retail shows natural skin under a white key light |

## 5. Revisited (mode 1)

Mode 1, "modern", is unchanged: sun plus AO plus normals plus the world-space
`pow` falloff, with the eyeballed ambient divisor. It is a Revisited art path,
not a fidelity claim. Two corrections apply to it:

- **Sun only where retail has a day/night cycle.** The sun is enabled only in
  areas with `AREA_DONIGHT` (the area defines `NIGHTAMBLIGHT`). The Keep has
  no night values, so it gets no sun. Classic never applies a sun.
- `[Revisited] LightingMode` now reaches `TMapRenderer`.

## 6. Classic mode (mode 0) after this change

The G-buffer normal target's alpha carries the **surface class**:
1 = tile (DLS model), 0 = mesh (3D-scene model). A 2D imagery state with
`ANIIM_LIT` also sets `kObjFlagSelfLit` in its id (`renderer.h`; bit `0x20`
of the id target's alpha), and the light pass shows such a pixel at its own
colour in both lighting modes (§2.1 step 5). Sector lights are submitted as
*retail lights* (authored radius, authored colour, retail multiplier).
Lights added through `AddPointLight` (VFX) stay *direct lights* with modern
semantics, added on top of the table result.

What the map draws follows retail's draw list (§2.1): every map object's 2D
imagery whatever its class (`TMapRenderer::RebuildForCurrentMap`), the
current state's still (refreshed when the state changes), skipping
`OF_INVISIBLE`, `OF_EDITOR` outside the editor, and states with neither
`ANIIM_LIT` nor `ANIIM_UNLIT` (`SSectorDrawableInst::Submit`). The camera
draws the 640×480 view 1:1 at 640×480 (`ComputeMapCameraViewport`, §8.1).

Tiles (§2, the MMX table, exact to the integer):

```
for each retail light: d = iso screen metric; if d < r: I += floor(63·B(d·254/r))
I     = min(63, I)
lm    = Σ(I_i · l̂_i · m_i) / ΣI_i                  the I-weighted colour × multiplier
M     = floor(min(255, 8·tile_ambient + 8·tile_gain_per_mult·(I/63)·lm))
c5    = round(albedo·255) / 8                       the tile's 5-bit colour
out5  = floor(min(255, c5·M) / 8)
lit   = (out5.r/31, 2·out5.g/63, out5.b/31) + albedo·direct
```

`ComputeClassicLightModel` (`src/classiclighting.cpp`) supplies
`tile_ambient` = `Mamb/8` and `tile_gain_per_mult` = `80(1 − A/255)/20/8`.
The `ΣI`-weighted colour average replaces "last light drawn wins the id".
They are identical for a single light, and for lights that share a colour
and multiplier (true of every overlap in the Keep).

Meshes (§3, `RealTimeLight=No`):

```
light = ambient + key colour · max(N·L_key, 0)
lit   = albedo · min(light + direct, s)
```

Map lights would add `Σ l̂_i · s·clamp(b_i/s − ambint, 0, 1) · max(N·L_i, 0)`
with `RealTimeLight` on; `SClassicLightModel::mesh_map_lights` carries that
switch and Classic leaves it off.

Both models live in `src/shaders/lightmodel.*.h` (`shade_surface`). The
deferred light pass and the translucent mesh pass (a character fading in or
out) both call it, so a fading mesh is lit exactly like an opaque one
([RENDERER_ARCHITECTURE.md](RENDERER_ARCHITECTURE.md), "Translucent meshes").

The ambient both models read is `TMapPane::ambient`, which now carries the
gamma offset (§2.5, `TMapPane::SetAmbientLight` in `src/mappane.cpp`,
`GammaAmbientOffset` in `src/gameoptions.h`). The Options pane's OK re-sets
it as retail does (`TOptionsPane::Apply`).

Known simplifications, each to confirm with a retail shot (§7):

- Meshes are lit per pixel with interpolated normals; retail's software
  renderer lights per vertex and stores a 5-bit vertex colour.
- `F` (the key-light factor) uses `RealTimeLight=No` (3.0).
- `EnhancedLighting` keeps the Direct3D path's overbright (§3.2), though the
  software renderer of the S1 lab ignores it. The GOG install renders 3D
  objects through Direct3D (dgVoodoo), so the D3D behaviour is the one its
  players see.

## 7. Open items — dosbox-x reference shots that would settle them

All shots: New Game → the opening Keep scene, Locke in the resurrection pit,
default camera, before he moves. Take each pair with the Options menu or the
INI `[Options]` set as stated. Capture lossless (PNG) at 640×480.
`tools/retaillab/retail.py` drives the lab
([gameflow/RETAIL_CAPTURE.md](gameflow/RETAIL_CAPTURE.md)).

1. **S1 — DLS baseline:** done (§8).
2. **S2 — Direct3D characters:** `Software3D=No` (a hardware device in the
   lab, if dosbox-x offers one), `RealTimeLight=No`, then
   `EnhancedLighting=Yes`. Shows the D3D key light and the overbright `s`
   against S1's software renderer.
3. **S3 — shipped default:** `RealTimeLight=Yes`, `EnhancedLighting=Yes`.
   The grid look most players remember, and characters lit by the map's
   lights. Settles whether Classic should also offer the grid's `1.375`
   radius and `K` gain (§3.4).
4. **S4 — map lights on characters:** in S3's setup, walk Locke from the pit
   towards the wall torch, taking a shot every ~1 character width. His
   brightness as a function of distance distinguishes "constant inside
   range" (the software renderer) from the DX5 attenuation.
5. **S5 — key-light direction:** an outdoor shot (Misthaven, daytime,
   `RealTimeLight=No`) with Locke facing each of the four screen diagonals.
   Confirms which side of him the `(0, −0.78, −0.625)` key light hits.

## 8. Shot S1 and verification in the port

### 8.1 The retail reference

Captured from `Rev98.exe` (the shipped exe with a 3-byte Win98 patch) in the
dosbox-x lab, 640×480, `Software3D=Yes`, `RealTimeLight=No`,
`EnhancedLighting=No`, `GammaLevel=3`, `MaxLights=3`, `Ambient3D=130`,
`LightRange3D=180`, `LightMult3D=250`; CPU `pentium_mmx`.

- S1: [gameflow/reference/opening/05_s1_pit.png](gameflow/reference/opening/05_s1_pit.png)
  (Locke standing in the pit, default camera); the "Who are you people?"
  beat is frame `op/f022` of the same session. Recordings:
  `~/RevenantRetailLab/captures/gameflow/opening-20261006-run{1,2}.avi`.
- The display gamma ramp is **not** in effect in these frames, nor anywhere
  in the sessions: the HUD's pixels are identical before and after the
  Options pane is used (OPTIONS.md §9). The gamma ambient offset is.

What the capture settles. Each region was fitted per pixel to
`out5 = min(255, c5·M) >> 3`, with `c5` read from the port's albedo view
(light-pass view mode 1) at the same camera, for the integer `M` that
matches most pixels:

- **The MMX transfer.** 98% of the map's pixels have an even 6-bit green,
  which only the MMX path produces (§2.4).
- **The ambient bytes.** Unlit floor and wall fit `M = (3, 3, 4)` exactly,
  the MMX ambient at `A = 14`: the Keep's `AMBLIGHT 4` plus GammaLevel 3's
  10. `A = 4` would give (0, 0, 1).
- **The falloff.** The light intensity `I` recovered from retail's red
  channel matches the port's per region to within 1–2 of 63 (floor below
  the pit 14 vs 15, near the torch 5.3 vs 5.8, right of the pit 6.2 vs 5.8):
  the iso metric and `pow` curve of §2.2–2.3 were already right.
- **The pit.** The platform inside the resurrection circle fits `M = (8, 8,
  8)`, identity: the `EXIT`-class Elevator (`(1216,544,16)`, state 0
  `ANIIM_LIT`) drawn after lighting. The `KInHellMouth` rim around it is
  hollow (bitmap z `0x7F7F`), and so is the `KInElevatorFloor` tile.
- **The camera.** Map features line up with the capture at scale 1 (pit,
  pillar, emblem, torch base); the port at scale 0.875 matched nothing. At the
  S1 beat the port's view sits 10 px higher than retail's (it matches at the
  "Who are you people?" beat), which is camera follow, not lighting (§9).
- **Characters.** Locke and the NPCs show natural skin and cloth under a
  white key light with no torch tint: no map lights on 3D objects under
  `RealTimeLight=No` (§3).

### 8.2 The port after this change

Side by side, retail | port before (feature/gameflow `5a4d9ea`) | port
after: [lighting/s1_pit_compare.png](lighting/s1_pit_compare.png) and
[lighting/choices_compare.png](lighting/choices_compare.png). The port runs
`--headless --resolution=640x480 --quickstart` with an INI holding the
retail paths, `GammaLevel=3`, `RealTimeLight=No`, `EnhancedLighting=No` and
no `[Lighting]` overrides; the S1 frame is a snapshot 20 s into the play
screen, the choices frame 34 s.

10×10 region means (top-left corner given, retail coordinates). The port
after this change is sampled 1 px right and 10 px up, where its pixels line
up with retail's (§8.1). The first three regions are the ones the task
first measured; the port before had a different camera (scale 0.875), so
its samples land elsewhere in the room.

| Region | Retail | Port before | Port after |
| --- | --- | --- | --- |
| "Floor in shadow" (60,330): an NPC stands here in retail | (31, 15, 20) | (68, 35, 20) | (57, 29, 34) |
| Floor in the torch pool (200,250) | (100, 47, 39) | (63, 29, 11) | (103, 48, 40) |
| "Wall" (60,60): HUD edge in one image | (27, 25, 46) | (7, 6, 10) | (28, 19, 30) |
| Wall (120,70) | (26, 26, 44) | | (26, 26, 45) |
| Wall (100,110) | (28, 27, 50) | | (29, 28, 52) |
| Wall, back (250,40) | (33, 32, 51) | | (33, 32, 53) |
| Floor in shadow, far left (30,150) | (28, 22, 40) | | (29, 22, 40) |
| Floor, upper right (300,120) | (27, 26, 46) | | (26, 25, 44) |
| Floor (150,300) | (48, 34, 48) | | (49, 36, 48) |
| Floor, pit-light pool (340,290) | (129, 59, 40) | | (129, 60, 40) |
| Floor, torch pool (100,360) | (178, 82, 44) | | (175, 82, 46) |
| Pit platform (175,195) | (80, 65, 107) | black | (78, 65, 104) |
| Pit platform (265,200) | (100, 85, 124) | black | (99, 86, 123) |

Every static surface is within 3 per channel. The differences left are
objects, not lighting: NPC positions at the beat, Locke's shadow on the
platform (retail draws one), and the torch flame (§9).

### 8.3 Tests and other scenes

- `build/test_classiclighting` pins `ComputeClassicLightModel` to hand-worked
  values: the MMX ambient bytes (identity at `AMBLIGHT 26`, 1.25 at 32, the
  Keep's (3, 3, 4)/8 at `A = 14` and (0, 0, 1)/8 at 4), the light gain 5.67
  for multiplier 12 at `A = 14`, the gamma offset per level, no map lights on
  meshes, and the mesh `chan`/`ambint` steps, EnhancedLighting and key-light
  direction.
- `--revisited` without an overlay INI still resolves to Classic
  (`[Revisited] LightingMode` defaults to 0). With `LightingMode = 1` the
  modern model is unchanged, but its ambient input now carries the gamma
  offset too, and self-lit imagery shows at its own colour there as well.
- Misthaven at night (`--quickstart --sector=0_2_25`, AMBLIGHT 35 with
  colour 75,100,255): the model gives `A = 45` at GammaLevel 3, ambient
  bytes (4, 5, 14), a saturated blue night (1.75 on blue). Not yet compared
  with a retail shot.

## 9. Related issues not addressed here

- **Camera follow at the S1 beat.** The port shows the room 10 px higher
  than retail while Locke stands in the pit, and the same as retail once the
  camera pans for the dialog. Camera follow belongs to the gameflow track.
- **Torch flames** (`EFFECT` `Flame` billboards at z 166) draw about 35 px
  higher than retail's, above the stand instead of on it. VFX track.
- **Character shadows.** Retail draws a dark blob under Locke on the pit
  platform; the port draws none.
- **The paper doll** (the equipment panel's 3D Locke) renders brighter and
  more saturated than retail's. UI track.
- **Animated 2D objects** draw their state's still, not their animation
  frames (tiles always did; containers and exits now draw at all).
- **Translucent (alpha) tiles** don't carry ids, so an `ANIIM_LIT` alpha
  bitmap would be lit. None in the Keep.
- **Effect lights.** VFX ports add their own lights through
  `AddPointLight` with tuned intensities, using the modern falloff in both
  modes. Retail effects lit through their object's `SLightDef`, which runs
  the same light table as map lights. Moving them to `AddRetailPointLight`
  belongs to the VFX track.
- **`T3DLight::GetBrightness` in `src/3dscene.cpp`** still uses the 1998 `pow`
  falloff. Retail `FUN_00411b70` is linear with `LightMult3D` (§3.3). Effect
  code (blood tint, light sampling) calls it.
- **FX LitFlat particles** take the scene ambient and sun from `SLightState`.
  Classic feeds them the tile ambient gain (`Mamb/8`) and no sun. Modern
  still uses the time-of-day sun only where the area has a day/night cycle.
- **Point-light cap.** The light pass takes the 16 lights nearest the view
  centre. Retail DLS draws every light that touches the update rect, with no
  cap.
- **The display gamma ramp** isn't rendered in either mode (OPTIONS.md §9).

## 10. Confidence summary

| Claim | Evidence | Confidence |
| --- | --- | --- |
| DLS tile path ships and runs with `RealTimeLight=No` | `FUN_00456810` branch on `+0x9a8` = `DAT_005d7a18` | High |
| MMX transfer on every MMX CPU; its byte maths and 5-bit output | `FUN_0043d1f0`, `FUN_0043c860`, `FUN_0041da10`, CPUID bit 23; S1: even green, ambient bytes (3, 3, 4) | High |
| Gamma ambient offset `(GammaLevel·5 − 10)·2` in `SetAmbientLight` | `FUN_00453640`, `FUN_0044d5c0`; S1 ambient needs `A` 13–15 | High |
| `pow(−1.1)·50` falloff, 6-bit saturating sum, iso screen metric | `FUN_0041dff0`, `FUN_0043be50` = 1998 `dls.cpp` asm; S1 intensities within 1–2 of 63 | High |
| Uncovered pixels black | `FUN_00456cc0` Box colour 0; S1 void (0, 0, 0) | High |
| Every class's imagery draws; `ANIIM_LIT` after lighting | `FUN_0045f800` flags `0x3f`, `FUN_00456cc0`, 1998 `animimage.cpp`; S1 platform at identity | High (the retail `TAnimImagery` bodies themselves not located) |
| Map lights reach 3D objects only with `RealTimeLight` | `FUN_00415c70` test of `DAT_005d7a18`; S1 characters untinted | High |
| Software 3D renderer: no point-light attenuation, clamp 1, 5-bit vertex colour | `FUN_0056eb30` | High (for `RealTimeLight=Yes` lighting, unverified by a shot) |
| Mesh ambient / `ambint` / `b` formula, `LightMult3D` | `FUN_00414310`, `FUN_004143d0`, `FUN_00411b70` | High |
| Mesh dir-light share and direction | `FUN_00412db0`, `FUN_00411eb0`, 1998 view matrix; S1 lit faces | Medium (S5 would close it) |
| D3D point-light attenuation | `FUN_00415790` values; DX5 semantics unknown | Low — needs S4, and only matters with `RealTimeLight` |
| Grid path output scale | `FUN_0045c8a0` not closed | Low — needs S3 |
