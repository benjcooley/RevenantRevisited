# Dust and Fog: authored paths and bounded retail probes

Audit date: 2026-10-05. These effects remain unfinished. The retained software recordings are diagnostic; neither earns accurate-reference or visual-parity credit.

## Dust — `0x0c052637`

Shipped `Misc/Dustcloud.I3D` has four zero-vertex emitter objects (`ground01` through `ground04`) and the textured `#cloud` quad. Its state0/frame1 `partsys` tag specifies emission, velocity, lifespan, rotation, friction, gravity, color, scale, spread, bounce, `posjitter` and `scljitter`. Asset SHA256: `3701457bbf74285f32b8cd0f7bf8b9e42f81f958395dadf491f6e4e9285ca26d`.

The port's partsys parser does not implement these jitter fields, and runtime controller binding is currently restricted to the four audited water type IDs. The former `TDustEffect_Bespoke` preview invented a warm-brown expanding billboard with a600ms lifetime. That substitution is removed: its factory reports unsupported, submits no puff and returns null. `DustBespokeSpawn` releases the preview context when creation fails.

The corrected build passes. An actual12-frame capture confirms the unsupported diagnostics and exact empty-floor output. This verifies that the unavailable path is honest; it does not establish an implemented Dust controller or count as animated preview coverage.

Native capture starts before `addat 10000 10000 Dust`, followed by `deselect all`, a five-second observation tail and `Dust.delete`. The final150 samples at30Hz contain one stationary image, with viewport mean absolute difference0.193572 from the floor. The same residue remains after deletion. `map 10001 10000 16` followed by `map 10000 10000 16` restores the viewport exactly. Measurements use `(0,12)`–`(640,340)`, excluding the FPS band. The full AVI retains the earlier creation period; the static-tail measurement does not claim the effect never animated.

Defer the native residue investigation and authored jitter/controller implementation until the easy-reference queue has advanced. Do not accept accumulated native trails as the intended particle appearance.

## Fog — `0xc24b2a85`

Shipped `Misc/Fog.I3D` contains36 vertices,150 indices, one object and no textures. Asset SHA256: `8ff5a3f3718757241a267f0e494d1b4dcc75f67c40f182902a7d46bdde70e8cf`. Its body uses the legacy pre-versioned layout: the second word36 is the vertex count. Raw controller metadata was not parsed; no claim that controller tags are absent follows from this audit.

Original `effect.cpp:6415`–`6531` implements `TFogAnimator` as an untextured animated vertex-colored grid:20 anchored vertices and16 drifting vertices, source color/alpha velocity updates and XY scale7.475. The current `TFogEffect_Bespoke` textured WorldXY billboard does not reproduce that path. No guessed replacement texture or tint was introduced during this audit.

The bounded native `addat 10000 10000 Fog` probe produces no visible effect in the final150 samples. Deletion and camera cleanup both match the original floor exactly. A visible native trigger/context and the actual source grid implementation remain required; standalone invisibility is not evidence of parity.

## Retained evidence

Paths below are relative to `/Users/benjamincooley/RevenantRetailLab`:

- `captures/runs/sw-complete-dust-20261005/manifest.json` and `native-source.avi`.
- `captures/runs/sw-complete-fog-20261005/manifest.json` and `native-source.avi`.
- `research/dust-fog-20261005/asset-controllers.json`, decoded asset manifests and OBJ files.
- `captures/port-smoke/dust-explicit-unsupported-20261005/manifest.json`:12 exact-ground frames, expected unavailable diagnostic.
- `research/dust-fog-20261005/build-dust-explicit-unsupported.log`: successful build.

Each native manifest retains source, floor, metadata and restored-floor hashes. Capture metadata retains the exact terminal guest jobs. No map was saved and neither Windows nor the game was restarted for these probes.
