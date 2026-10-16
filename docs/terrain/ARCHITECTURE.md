# Terrain System — Architecture

Replaces Revenant's exterior ground tiles with a heightmap terrain rendered
through the existing deferred pipeline. The retail tiles stay the ground
truth: the terrain's shape, palette and material layout are *derived* from
them, and the retail sprite path remains untouched for Classic mode.

Companion: [PLAN.md](PLAN.md) (phases, acceptance tests). Context docs:
[RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md),
[DEFERRED_LIGHTING.md](../DEFERRED_LIGHTING.md),
[revisited/README.md](../../revisited/README.md).

## 1. What the retail ground actually is

Everything below was checked against `src/` and the shipped level data
(`data/Modules/Ahkuilon_unzipped/Map/<level>_<sx>_<sy>.DAT`). It changes the
problem from "replace some sprites" to "recover authored terrain data".

### 1.1 The forest was generated from a heightmap

`GenerateMap` ([command.cpp:1043](../../src/command.cpp#L1043)) built the
exterior from two 8-bit bitmaps, `map.bmp` and `overmap.bmp`, sampled at
**plate corners**. A plate is 128 world units (2 m at 64 wu/m), 8×8 plates
per 1024-unit sector ([revdefs.h:83-88](../../src/revdefs.h#L83-L88)).

Each map byte is `terrain_type << 4 | elevation_level`
([command.cpp:807-817](../../src/command.cpp#L807-L817)):

| Type | Value | Tile letter | Meaning |
|---|---|---|---|
| GRASS | 1 | `g` (`k` = grass one level up, `i` = four up) | grass |
| DIRT | 2 | `d` | dirt |
| MOUNTAIN | 3 | `m` | rock face, centre plates left empty |
| LAKE | 4 | `e` (retail `class.def` only) | still water |
| RIVER | 5 | `r` | river bed / gravel |
| OCEAN | 6 | `o` | shore sand |
| STREET | 7 | | paved; any street corner forces a solid street tile |

A tile type's `CODE` stat packs the four corner deltas, NW,NE,SW,SE, relative
to the lowest corner ([command.cpp:847-857](../../src/command.cpp#L847-L857)),
so `Forgddg` = `0x10202010`. The generator picks a random tile whose CODE
matches, falls back through wildcard corners, stacks an `EXTRA` tile on top
when the type asks for one, and places the result at the plate centre with
`z = lowest_level * ELEVATIONCHANGE + zstart[type]`
([command.cpp:859-882](../../src/command.cpp#L859-L882),
[command.cpp:998-1016](../../src/command.cpp#L998-L1016)). `overmap.bmp` adds
path tiles with N/E/S/W connectivity
([command.cpp:966-996](../../src/command.cpp#L966-L996)), and
`TTerrainTemplate` scatters props over each plate.

The shipped level 0 data matches this layout: 9,163 of 10,159 forest terrain
tiles sit at plate centres (`x%128 == 64`, `y%128 == 64`), with `z` in
{0, 32, 64}. The snapshot's `ELEVATIONCHANGE = 46` is not what shipped;
the data says 32. Everything the bake needs comes from the data, not the
constants.

**Consequence.** The authored terrain is a 128-unit material/elevation grid,
a path overlay, and prop templates. The tiles are a *rendering* of it. We
reconstruct that source, refine it with the per-pixel relief the artists
painted into each tile, and render it with a real terrain renderer.

### 1.2 What a tile contributes beyond the grid

- **Per-pixel depth.** Every ground tile carries a u16 z-buffer, a signed
  local offset from the draw anchor (int16 after decode; see
  [DEFERRED_LIGHTING.md §3](../DEFERRED_LIGHTING.md)). Flat ground drops
  1.734 per screen row; banks (`k` tiles), lumps and mounds carry real
  relief. Through the iso inverse, each pixel is a world-space surface
  sample. This is the sub-plate heightfield.
- **Albedo.** Hand-painted transitions: grass clumps bleeding into dirt, not
  bilinear ramps. This blob layout is a big part of "feels like Revenant".
- **Walkmap.** A u8 height per 16-unit cell on each tile type, stamped into
  the sector's 64×64 u16 grid ([sector.cpp:575](../../src/sector.cpp#L575)).
  Authored in the editor by probing the z-buffer
  ([mappane.cpp:4448](../../src/mappane.cpp#L4448)). Collision never reads
  pixel depth at runtime.

### 1.3 Scale

| Quantity | Value |
|---|---|
| World units per metre | 64 |
| Plate | 128 wu, diamond 256×128 px at 1× |
| Sector | 1024 wu, 8×8 plates, 64×64 walkmap cells |
| Screen density at 1× | ≈1.4 px per wu along the ground |
| Ground tile image | ≈256–272 × 130–175 px, 8×8-cell walk footprint |
| Visible window | 3×3 sectors (`SECTORWINDOWX/Y`) |
| Camera | fixed 2:1 iso ortho; optional 6° perspective (`ParallaxMode`) |

The camera never sees a horizon. The visible ground is a bounded window at a
bounded zoom. This is the single biggest simplification versus open-world
terrain systems.

## 2. State of the art, and what applies here

| Technique | Where it comes from | Verdict |
|---|---|---|
| Geometry clipmaps, CDLOD, chunked quadtree LOD | Hoppe/Asirvatham GPU clipmaps; Strugar CDLOD; Witcher 3, Terrain3D | **Not needed.** No horizon, bounded zoom. Uniform grid per sector, one global LOD per frame. |
| Hardware tessellation / Nanite landscape / virtual heightfield mesh | UE5 | Not available (GL 3.3 / Metal / D3D11 via sokol) and not needed at our view distance. |
| Virtual texturing (SVT/RVT) | id Tech, Frostbite, UE RVT | **Not now.** The resident set is 9 sectors; everything fits. Revisit only if macro colour memory bites. |
| Texture arrays + per-texel weights (splat) | Universal; Terrain3D control map (base/overlay/blend per texel) | **Adopt weights**, not a two-material control map. Painted blobs need >2 materials in a texel. |
| Height-based blending | Unity/UE common practice, ambientCG height maps | **Adopt.** Grass blades sit over dirt, pebbles poke through sand. |
| Procedural shader splatting (rules from slope/height) | Frostbite, Andersson SIGGRAPH 2007 | **Adopt as a secondary modulation** only. Primary weights are authored (retail). |
| Stochastic / hex tiling | Heitz & Neyret 2018; Mikkelsen, JCGT 11(3) 2022 | **Adopt** Mikkelsen hex-tiling for albedo and normal. Needed: a 4 m repeat is visible across a 3-sector view. |
| Triplanar | GPU Gems 3 ch.1 | **Skip.** Slopes are shallow banks; `m` rock faces stay as sprites/meshes. |
| Macro variation (distance/low-frequency tint) | Far Cry, Ghost of Tsushima, every terrain shader | **Adopt, with a twist:** the macro layer is the retail albedo itself. |
| Spline/decal roads on RVT | UE RVT decals | **Adopt the idea**: paths and streets are decal-style weight layers written by the bake. |
| GPU procedural grass | Ghost of Tsushima, GDC 2021 | Later phase. Independent of this design. |
| Terrain-blended meshes (rocks, props) | Common since Horizon/Far Cry | Later phase: 3D rocks replace `ForRock*` sprites; blend by depth. |

Sources: Hoppe & Asirvatham, *GPU Geometry Clipmaps* (GPU Gems 2);
Strugar, *CDLOD* (2010); Andersson, *Terrain Rendering in Frostbite using
Procedural Shader Splatting* (SIGGRAPH 2007 course); Mikkelsen, *Practical
Real-Time Hex-Tiling* (JCGT 2022); Terrain3D `controlmap_format.md`; Epic
*Using Nanite with Landscapes*; GDC Vault, *Terrain Rendering in Far Cry 5*
(Moore, 2018) and *Samurai Landscapes* (Pohlmann, 2021).

## 3. Design

### 3.1 Principles

1. **Retail is the reference, not the template.** Shape, palette and blob
   layout are measured from retail tiles. The renderer is modern.
2. **Classic mode is untouched.** Terrain exists only under `--revisited`
   with `[Revisited] Terrain=1`. The tile path keeps rendering everything
   else, and renders the ground too when terrain is off.
3. **Gameplay does not move.** The walkmap stays the collision truth; saves
   stay interoperable. Terrain is a drawable.
4. **Derived, not shipped.** Per-sector terrain data is derived from the
   user's retail install into the engine cache (overlay README, Phase 3).
   Only the material library (CC0 textures) and the palette ship.
5. **Verifiable at every step.** With the retail albedo as the only texture,
   the terrain must render pixel-close to the tile path. Every enhancement
   is a knob on top of that baseline.

### 3.2 Data model: one terrain tile set per sector

```
STerrainSector                       (loaded with TSector, Revisited only)
  level, sx, sy
  height      R16     512×512   2 wu/texel   world z, 1/16 wu fixed point
  weights[2]  RGBA8   256×256   4 wu/texel   8 material layers, sum = 255
  macro       RGBA8   512×512   2 wu/texel   retail albedo reprojected to
                                             world XY, A = coverage
  holes       R8      256×256   4 wu/texel   1 = terrain absent (unconverted
                                             ground tile, interior, void)
  plates[8][8]                                 inverse-generator record:
      corner types, corner levels, overlay, supertile id, source tile ids
```

Why these resolutions: at 1× a texel of 2 wu is under 3 px, so the height
field reproduces the painted relief the walkmap was probed from; 4 wu for
weights is finer than any painted transition edge once height blending
sharpens it. Memory per sector on GPU is ≈1.9 MB; the 3×3 window is ≈17 MB.

The material palette is per module (`TModule`), not per sector:

```
STerrainPalette                      revisited/resources/Terrain/<module>.def
  layer[8]: name, albedo+height array slice, normal+rough+ao slice,
            world repeat (wu), tint, height-blend sharpness,
            retail code letters it stands in for
```

### 3.3 The bake: tiles → terrain data (CPU, deterministic)

A new module `src/terrainbake.{h,cpp}` driven by `--bake-terrain=<level>`
(and by the overlay's derived-cache step on first boot). No GPU, so it runs
in CI on every platform and its output is bit-stable.

1. **Classify.** Walk each sector's objects. A tile instance is *terrain* if
   its type is in the terrain set: types with a `CODE` stat, supertiles,
   path/overlay tiles, and a curated list for named ground
   (`ForHutground*`, `ForRing*`, `Run*Ground*`, `TwnSt_*`, …). The list is
   data (`revisited/builders/terrain_tiles.toml`) with a contact-sheet
   report, so classification is reviewed by eye, not inferred. Everything
   else (trees, rocks, fences, pebbles, bridges, buildings, water frames)
   stays a sprite.
2. **Inverse generator.** From type name/CODE and position, recover each
   plate's corner types and levels, overlay, supertile membership. Record
   per plate. This is the semantic prior for weights and the authored
   elevation.
3. **Software z-buffer.** Project the sector's terrain tiles with retail
   math (`WorldToScreen`, [object.cpp:146](../../src/object.cpp#L146);
   anchor minus registration; `regz`; per-pixel int16 depth) into a CPU
   z-buffer over the sector's screen footprint with padding. Nearest
   `scene_z` wins, exactly as the hardware tile pass resolves overlaps.
   The decode path already exists (chunk decoder, `TBitmap`); the
   projection math has a CPU twin in
   [tools/cpu_tile_perspective_test.cpp](../../tools/cpu_tile_perspective_test.cpp).
4. **Invert to world.** Each surviving screen pixel `(S,T,scene_z)` maps to
   a unique world point via the iso inverse
   ([DEFERRED_LIGHTING.md §4](../DEFERRED_LIGHTING.md)). Scatter `z` into
   the height grid and the albedo into the macro grid (average per texel;
   fill isolated gaps from neighbours; record coverage).
5. **Weights.** Per 4-wu texel: start from the bilinear corner prior of
   step 2, then refine with the retail colour at that texel against the
   palette's per-layer reference colours (measured once from the pure
   tiles `Forgggg`, `Fordddd`, `Foroooo`, `Forrrrr`, …). The prior decides
   which layers may appear; the colour decides where the blob edge falls.
   Overlay plates add the path layer; `TwnSt_*` adds street.
6. **Holes.** Texels under ground tiles that are *not* in the terrain set
   (and texels with no coverage) are holes, so the sprite keeps the pixels
   and there is no z-fighting.
7. **Water.** Lake/river/ocean tiles are *not* converted in the first
   phases; they stay sprites. The bake extrapolates height under them
   (shore slope continued, floor at surface − 48 wu) so banks are
   continuous and a later water pass has a bed to refract against.
8. **Write** `<cache>/Terrain/<level>_<sx>_<sy>.rvt` (one little-endian
   blob, versioned header; PNG-compressed planes) and a per-level
   `report.csv` with coverage, height range, max |z − walkmap| per tile.

Everything in steps 3–4 is the same math the GPU tile shader runs
([tile.glsl.h](../../src/shaders/tile.glsl.h)), so the baseline test in
§3.7 is a genuine equality check, not a fit.

### 3.4 Geometry

- One shared grid patch in a static vertex buffer: 32×32 quads, unit
  coordinates. A chunk is 256 wu (one quarter of a sector per axis, 4×4
  chunks per sector). Vertex spacing is 8 wu at LOD 0, 16 at LOD 1, 32 at
  LOD 2, by instancing the same patch with a different chunk scale.
- The vertex shader fetches `height` (bilinear) at the vertex's world XY and
  projects with the mesh formula already in
  [mesh.glsl.h](../../src/shaders/mesh.glsl.h):
  `S = relx − rely`, `T = (relx + rely)/2 − wz·0.867`,
  `scene_z = kcam − 0.867(relx + rely) − wz/2`, plus the perspective branch.
- LOD is **global per frame** from the camera zoom. Neighbouring chunks are
  always the same LOD, so there are no cracks and no morphing. Skirts are
  unnecessary for the same reason.
- Chunks are culled by their projected screen rect against the padded
  G-buffer. Submission is one instanced draw per sector per frame.
- Budget: 3×3 sectors at LOD 0 is 144 chunks × 2,048 triangles ≈ 295k
  triangles. Trivial.

### 3.5 Surface shader

Fragment stage, writing the same four G-buffer targets as the tile and mesh
passes (albedo RGBA8, world normal RGBA16F, scene_z R32F, obj_id RGBA8):

1. `w = weights[0..1]` at the fragment's world XY, bilinear; `if (hole) discard`.
2. For each layer with `w_i > 1/255` (dynamic branch): hex-tiled sample of
   `albedo_height[i]` and `normal_rough_ao[i]` from the two 2D array
   textures at `world_xy / repeat_i`.
3. Height-blend: `b_i = h_i + w_i`, `m = max b_i − sharpness`,
   `w'_i = max(b_i − m, 0)`, normalise. Composite albedo, tangent normal,
   roughness, AO.
4. Macro tint: `albedo *= macro_rgb / palette_mean_rgb(w')`, where `macro`
   is the retail colour low-passed at ≈24 wu. Clamp the gain. This hands
   the retail palette and blob layout to the detail layers.
5. Normal: central differences on `height` give the heightfield normal in
   world space; the detail normal perturbs it in the heightfield's own
   tangent frame (X, Y, N). Write world normal.
6. `scene_z` from the interpolated world position, the same formula as the
   vertex stage. `obj_id` = a reserved terrain id with the chunk index in the
   low bits so picking can route ground clicks as it does today.

Sample count: with 2–3 layers active, ≈18 array fetches per fragment. The
material arrays are 2K RGBA8 (BC7 later): 8 layers × 2 arrays ≈ 256 MB
uncompressed at 2K, so start at 1K (64 MB) and measure before going up.

Three hand-written shader copies, `terrain.{metal,glsl,hlsl}.h`, per the
convention in [shaders.h](../../src/shaders/shaders.h).

### 3.6 Integration with the existing renderer

Layering per [RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md):

| Layer | Addition |
|---|---|
| (1) `TRenderer` | terrain pipeline + shaders, material array registry, `SubmitTerrainChunks(...)`, drawn inside the G-buffer fill pass before tiles. |
| (1.5) drawable | `TTerrainRenderer`: per-sector GPU images (height, weights, macro, holes) + chunk instance data. Owns nothing game-side. |
| (2) `TMapRenderer` | loads/unloads `STerrainSector` with the sector, submits visible chunks, marks tile instances covered by terrain so their draw records are skipped. |
| Settings | `[Revisited] Terrain=1`, `TerrainMacroTint`, `TerrainHeightBlend`, `TerrainLod`, `TerrainDebug` (weights / height / macro / holes / wire). |

Nothing downstream changes. The light pass, AO, sun-shadow ray march and
the iso inverse all consume the G-buffer contract and already handle mesh
fragments with true normals. Sprites (trees, rocks, characters) keep their
authored depths and still sort against the terrain in the one hardware
depth buffer, so a tree's feet land where they landed on the tile, because
the terrain height at that point *is* the tile's depth.

Draw order inside pass 1: terrain, then opaque tiles, then meshes. Ground
tiles that the bake converted are skipped by `TMapRenderer`; the rest draw
as before. Holes in the terrain are where unconverted ground lives.

### 3.7 Verification ladder

1. **Baseline equality.** `--test=terrain-ab --sector=0_2_25`: render the
   sector twice, tile path and terrain path with `macro` as the only
   texture and no detail layers. Compare `scene_z` (expect |Δ| < 2 wu on
   99% of ground texels) and albedo (expect SSIM > 0.95 after the 2-wu
   resample). This proves the bake, the mesh and the depth formula.
2. **Walkmap agreement.** Per sector, histogram of `height − walkmap` at
   cell centres; the stamping rule is `max(existing, walkbyte + z)` so the
   terrain should sit within the probe noise (≈ ±8 wu) of the walkmap.
3. **Sprite footing.** Scripted camera sweep with `--filmstrip`; trees,
   rocks and Locke must not float or sink versus the tile path.
4. **Lighting parity.** Sun/AO/shadow on both paths; differences should be
   improvements only (true normals instead of reconstructed).
5. **Palette check.** Mean colour per material region, terrain vs retail,
   within a tolerance; failing this is the "pale port" smell.

## 4. Material library

Source: Poly Haven 4K sets (CC0), already downloaded. Each carries
`diff` (JPG), `nor_gl` (EXR), `rough` (EXR), `disp` (PNG); some add a mask.
A builder (`revisited/builders/build_terrain_materials.py`) packs them to
two RGBA8 array textures per palette and writes the palette `.def`.

Proposed forest/town palette (8 layers):

| Layer | Retail code | Candidate sets |
|---|---|---|
| grass | `g`, `k`, `i` | leafy_grass, forest_ground_06 |
| dirt | `d` | brown_mud_leaves_01, mud_forest, farm_soil |
| shore sand | `o` | dense_sand, damp_beach_sand |
| gravel / river bed | `r` | rocky_trail, rocks_ground_02 |
| rock | `m` | dark_rock, rock_wall_17 |
| path | overlay PATH/ROAD | rocky_trail (packed), farm_soil |
| street | STREET, `TwnSt_*` | cobblestone_floor_09, stone_tiles_03, large_grey_tiles, rock_tile_floor |
| rubble (ruins) | `Run*Ground*` | rocks_ground_02, rock_tile_floor |

Colour matching is the builder's job: per layer, a target mean/variance is
measured from the retail pure tiles and the set is colour-transferred in
Lab before packing. The runtime macro tint handles the spatial variation;
the builder handles the global palette. Repeat sizes start at 256 wu (4 m)
for ground layers and 128 wu for street; hex-tiling hides the repeat.

Geometry that is *not* terrain: `ForRock*`, `ForAmount*`, `ForTCav`,
bridges, fences, trees. These stay sprites now and become 3D meshes in a
later phase (Poly Haven and other CC0 rock scans; trees are a separate
track). Pebble/debris scatters (`ForPeb*`, `ForDK*`, `ForDrub*`) stay
sprites until a decal layer exists.

## 5. Scope boundaries

- **In:** levels 0, 4, 5, 66 (forest, ruins, town streets as a street
  layer). Interiors (town, keep, dungeon, cave, labyrinth) keep tiles.
- **Out, for now:** water surfaces, mountain faces, props, grass geometry,
  editor painting. Each has a slot in [PLAN.md](PLAN.md).
- **Never:** changing walkmaps, sector files, saves, or the Classic render
  path.

## 6. Open questions (settle in Phase 0 with data)

- Is `TFLG` in retail `class.def` a reliable ground category? It would make
  the terrain-set list mostly automatic. The reading "1/2/4/5/8 = ground,
  rugs, streets" is unverified.
- Whether `ELEVATIONCHANGE = 32` holds on levels 4/5/66 or only level 0.
- How much retail ground overhangs its plate (padding for the CPU
  z-buffer); measured from the dump, not assumed.
- Whether the `k` bank tiles' relief is consistent enough across variants
  that the height field needs no per-variant seam fix.
