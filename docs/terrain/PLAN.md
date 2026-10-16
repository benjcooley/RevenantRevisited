# Terrain System — Plan

Phased delivery for [ARCHITECTURE.md](ARCHITECTURE.md). Each phase ends
with a test mode or a report someone else can run; nothing ships on
eyeballing alone. Work lives on `feature/terrain`
(`worktrees/terrain`), PRs into `main` per the CI gate.

## Phase 0 — Forensics and census

Goal: know exactly which tiles are terrain and what they contain.

- `--dumpterrain=<dir>`: per level, list every `OBJCLASS_TILE` type seen,
  its CODE/EXTRA/SUPERTILE/TFLG stats, instance count, position residue
  mod 128, z histogram, image size, walk footprint. Contact sheets per
  theme, grouped by the proposed classification.
- `revisited/builders/terrain_tiles.toml`: the reviewed terrain set with a
  reason per entry (code tile, supertile, path, named ground, street).
- Confirm or drop the open questions in ARCHITECTURE §6. Record the
  shipped elevation step per level.
- Measure the per-pixel relief of the `k` bank tiles and the lumps so the
  2-wu height resolution is a decision, not a guess.

Done when: the census report exists for levels 0/4/5/66 and the terrain set
covers ≥ 95% of ground-footprint tile instances on level 0.

## Phase 1 — Bake and baseline

Goal: terrain geometry that is indistinguishable from the tiles.

- `src/terrainbake.{h,cpp}` + `--bake-terrain=<level>`: inverse generator,
  CPU z-buffer, iso inverse, height/macro/holes planes, `.rvt` writer,
  `report.csv`. Pure CPU; unit tests on synthetic tiles (flat, ramp, two
  overlapping tiles) registered in CMake and CI.
- `TTerrainRenderer` + `TRenderer` terrain pipeline with the macro plane as
  the only texture. `terrain.{metal,glsl,hlsl}.h`.
- `TMapRenderer`: load `STerrainSector` with the sector, skip converted
  tile instances, submit chunks. `[Revisited] Terrain` toggle.
- `--test=terrain-ab`: the baseline equality test (ARCHITECTURE §3.7 items
  1–3), headless via `--filmstrip`, reporting depth and albedo deltas.

Done when: baseline deltas are within tolerance on three level-0 sectors
(forest, shore, town edge) and on one level-66 sector, and a filmstrip
sweep shows no sprite footing change.

## Phase 2 — Materials

Goal: the "better" half, with the palette held to retail.

- `revisited/builders/build_terrain_materials.py`: Poly Haven sets →
  array textures (albedo+height, normal+rough+ao), Lab colour transfer to
  retail reference colours, palette `.def`.
- Weights from the corner prior + colour classification; path and street
  layers.
- Shader: hex tiling, height blending, macro tint, detail normals.
  Debug views for weights/height/macro/holes/wire.
- A/B gallery: the same sector at 1× and at 1080p, tile path vs terrain,
  through `tools/hud_ab.py`-style capture, plus the palette check.

Done when: all debug views are clean on level 0, the palette check passes,
and the author signs off on three reference screenshots (this is the one
visual-verify step that is the user's call).

## Phase 3 — Ground features

- Water: a pass-4 water surface over the baked bed for lake/river/ocean
  tiles; retail water frames retired where the surface covers them.
- Decal layer for pebbles/debris scatters and for path edges.
- 3D rocks replacing `ForRock*`/`ForAmount*` sprites, depth-blended into
  the terrain; CC0 scans colour-matched the same way as materials.
- Town streets: curb/sidewalk height from `TwnSdWlk*` tiles folded into the
  height plane.

## Phase 4 — Authoring

- Editor brushes for height and weights with the walkmap re-derived from
  the height plane (`CalculateWalkmap` already probes depth; point it at the
  terrain).
- A reborn generator: `map.bmp`/`overmap.bmp` → terrain planes directly,
  for new exterior content, keeping the retail tile set as the fallback
  renderer so Classic mode still has something to draw.

## Risks

- **Overhang and occlusion in the bake.** Trees overlap ground in screen
  space but are excluded from the z-buffer, so no holes; the risk is
  ground tiles whose painted edge extends past their footprint and hides a
  neighbour. The nearest-wins rule reproduces retail here, so the result
  is at worst identical to the game.
- **Readback is Apple-only.** The bake avoids the GPU for this reason; the
  A/B test mode needs readback and runs on macOS, with CI on Linux/Windows
  covering the CPU bake and shader compilation only.
- **Three shader copies.** Accepted per existing convention; keep the
  terrain shader small and push shared math into the uniform block, not
  into per-backend code.
- **Memory at 2K materials.** Start at 1K; the array format makes the bump
  a one-line change.
