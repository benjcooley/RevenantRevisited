# Six Warp colors, Speaker and both LabGate barriers

This batch retains separate current Metal/native inspection pairs for all nine
types. Lane and independent root inspection pass their overall isolated
appearance in the tested source-software configuration, as recorded in the
[machine review](SMALL_FAMILY_VISUAL_BATCH_20261010.json); no verdict is
borrowed from the earlier blue Warp review.

The six Warp pairs separately preserve orange, purple, red, white, yellow and
green triangular portals, changing cloudy/perimeter texture and transparent
floor integration across four successive atlas states. Each retains all four
vertices, three triangles and its own pinned 256² ARGB4444 texture. The known
renderer projection discrepancy remains: current apex 7 pixels higher, height
138 versus 131 pixels (about 5.3%), with nearly equal width/lower edge. No shift,
scale, tint or phase fit removes it; exact geometry/device parity stays open.

Speaker and both barriers required a source correction. Their earlier thin
references supplied authored diffuse as vertex color: green-gray for Speaker,
red for the barriers. That is a consequential material fixture, but **not the
actual untextured software producer**. Original D3DVERTEX entry 56eb30 executes
normal transform, Illuminate 56ed9d and raster without reading material diffuse.
The true reference shows gray facets on Speaker and gray planes on both
barriers. Before correction, the current port showed solid-white Speaker and
red barriers. The retained initial pairs are diagnostics, not visual passes.

The correction admits only EFFECT IDs `ad92bc10`, `ad92bc37`, `ad92bc38` with
zero textures and the existing source-software mesh lighting policy enabled.
They use white tint and existing normal-prelit mode 1. The policy query requires
classic lighting mode 0 and enabled source RGB lighting; modern lighting and
positional-light fallback preserve their previous material/alpha behavior.
Unknown IDs, other classes and textured meshes retain their prior route.
No fitted color or lighting descriptor was introduced.

Final Speaker shows the original grayscale faceted cylinder; S/E show their
respective gray cards. Remaining bounds differ by at most 2 pixels per edge in
these fixed windows. Normal transform/raster/backend differences are retained;
this is an overall appearance review, not exact per-vertex or device parity.
Barrier pixels retain the original RGB565 green quantization difference:
native `(189,186,189)` versus current `(189,189,189)`, without brightness fitting.

## Evidence and reproduction

All nine actual map runs pass ten typed create/frame/move/delete observations,
16 frames, visible creation and exact clean-floor restoration, with clean exits.
Each Warp has four distinct stationary samples; the three untextured roots stay
constant. Source initialization/pose/raster state is independent of Metal output.

The own Release/Metal build is SHA256
`9c3fe0ab1f014ce5ea22485bc32a89a478535021f275e6eb17344a02cb763a7d`.
Captures use FontRuntimeLab, camera `(110,10000,10000,16)`, ambient 32 white,
24 Hz, seed 1, one warmup frame, 640×340 and private SAVE directories.
Native inspection uses 512², camera/owner origin 0 and the original projection.
The clean modern floor is quantized to RGB565 as the native draw destination;
this is a common inspection backdrop, not an original retail scene.
Fixed 200×210 windows recenter viewport origins without effect registration.

Warp references run original custom Init/Animate/Render and alpha raster with
white 31 vertex input; mode 2 preserves ARGB texels without normal/deferred-light
modulation. Untextured references run the whole original D3DVERTEX producer on
authored XYZ/normal/UV, with no host vertex color or diffuse. Ambient RGB 38 and
directional RGB 1 descriptors match the actual capture log: mode 0,
ambient`0.14902=38/255`, directional`1`, source RGB 1, no point lights. These follow
the actual ambient 32/white and INI Ambient3D 130/DirLight 85 source math.

```sh
cmake -S . -B build-merge -DCMAKE_BUILD_TYPE=Release
cmake --build build-merge -j4
python tools/test_map_texture_submission.py
python tools/retail_runtime/family_visual_review.py \
  --binary build-merge/Revenant \
  --data-root /Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/fountain-command-20261004/data \
  --output /tmp/small-family-new-run
```

The source capture tool produces evidence awaiting manual review; it never
automatically grants visual credit. `lit_software_probe.py` is the identical
shared original-vertex fixture from commit 4500135. The compiled actual map
submission regression passes exact-ID/source-policy tests and negative controls
for modern mode, positional fallback, unknown IDs, other classes, material alpha,
textured owners and immutable independently phased texture batches.

Final local pairs and manifests:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261010/family-visual-batch-final/{type}/same-floor-native-modern-sheet.png`.
Initial diagnostics remain under `family-visual-batch-current`. The machine
review pins every final pair, capture, asset and build hash.

Natural teleporter activation/story/visibility, saved hidden barriers, ambient
speaker sound/caller, positional-light selection, full original scene and
hardware/modern lighting parity remain outside these isolated reviews.
