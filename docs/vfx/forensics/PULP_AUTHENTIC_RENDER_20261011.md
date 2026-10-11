# Pulp genuine public-Set renderer — 2026-10-11

Source checkpoint `652e3733`. Exact shipped Pulp `0x152dafdd`, own
`Misc/Pulp.I3D` SHA-256
`98b64197c399ac1e31e659dcc143c8d6dcb31b600766e6875e85e7da0a693e0c`.
This replaces the empty preview with a bounded, explicit actor fixture. Root
appearance approval is separate; this document does not create it.

## Declared caller and retained boundaries

The fast original process boots real CRT/static constructors/rules/resources,
constructs a genuine Acolyte and its original animator (22 objects), creates
Pulp through its own typed factory, and calls public `Set4e96e0` with velocity
`(0,0,8)` and body/blood/splat counts `5/5/5`. The actor is at the declared
unresident position `(10000,10000,16)`. No bones, damage callback, successful
blood tile, actor fields, or generic emitter are fabricated. Source reads the
real actor animator's selected-object rotations and uses the exact Pulp mesh
resource. Original selected actor IDs and own chunk IDs remain independent.

Original owner Pulse includes custom animator Pulse4e8d10; base Animate40e460
and complete Render4e9400 execute separately, once per update. All 80 updates
execute the full original raster; images are persisted at0/1/2/3/5/10/20/40/60/80.
Five chunks settle at tick2 and persist. The last droplet expires; there is no
invented completion timer or gallery restart.

The original public setter marks two blood slots active but writes their data
to slot0. This fresh cached-VM heap supplies zero bytes for the other cold slot;
source reproduces that declared allocation state. Reused-heap behavior and the
natural death/corpse caller are outside this fixture. The unused splat array
has no visible submissions in the exercised branch.

All86 genuine ground queries return0. Their ordered consumed X/Y arguments
match compiled source exactly, including Set's unusual `(x,x,x)` temporary.
Nine decal-query Z words retain stale original stack values: GetWalkHeight452e10
reads only X/Y, so the unused words are recorded but not invented in source.
All10 genuine Blood/BloodS NewObject calls return−1. Source executes its real
terrain/creation APIs and rejects a nonzero height, successful allocation, or
unexpected constructor RNG instead of pretending the unproved branch works.

Even failed native creation calls execute MakeIndex44ce30:
`rand → srand(rand + GetTickCount) → rand`. The original fixture explicitly has
clock0. Actual native clock0/1/1000 controls prove the two calls and resulting
seeds. A Pulp-only callback retains this intrinsic side effect after a checked
failed source allocation; global modern map IDs remain unchanged. Gallery
factories never reseed, but these original intrinsic reseeds are logged plainly.
Mapped decal success and live-clock behavior remain open.

The original normal-scene caller413ba1/4149b1 executes Scene1: depth test/write,
CULL3, ONE/ZERO. Existing opaque helper policy carries the same draw order,
authored XYZ/normal/UV/indices, normal-light inputs, and ABSPOS float matrices.
No new shader/pipeline or global renderer changes were needed. Matrices use
ROT1/SCL2/POS3 directly; no owner Z correction is applied to absolute positions.
The fixed native viewport1280×960, center640/640, world camera10000/10000/0,
source ambient38 and directional1 are shared with native-domain Metal.
Ordinary-world projection is retained as a separate diagnostic.

## Validation

- ASan/UBSan compiled core:11,340 body/blood values bit-exact over81 states;
  every RNG state/count exact, including140 total draws and10 failed decals.
- All86 consumed ground-query arguments/results match. Ordered445 draws:
  7,120 matrix values, max error5.96e−8;386,144 authored vertex bytes and22,110
  indices match the independent original packets.
- Wrong clock1, omitted intrinsic reseed and double-Pulse controls are rejected.
- Actual GPU logs independently match all81 original body/blood hashes and RNG
  counts.81 active native-domain and81 ordinary frames exit0 on one binary.
- Cold GPU frame uses an explicit1-microsecond step; logs prove zero simulation
  updates. It is the actual initial public-Set image, not a fitted active phase.
- Native/Metal ten-phase whole/crop panels include every submitted geometric
  extent: x586.6968..681.9560,y491.7902..680.3739, inside1280×960.
- Current translated gallery:20/20 no-UI samples visible,458 real24Hz updates,
  one unseeded persistent factory,10 intrinsic reseeds/failed real allocations,
  clean exit and owned actor cleanup restoring flags406c028/health102.
- Build, review mapping/lifecycle unit, generator check, Python compilation,
  and diff whitespace checks pass. Existing renderer/defaults are untouched.

The earlier3185-coordinate gallery fails its terrain guard because its real
floor returns17. That retained negative is `pulp-gallery-final`; it is not a
pass. The explicit unresident source position is independent of the displayed
station translation. Full original/source/GPU proofs were rerun there.
The all-query observer replay preserves all ten previously reviewed native PNG
hashes. Original software sampling/depth/framebuffer quantization still differs
by roughly1–2 edge pixels and some surface shading; exact pixel identity,
natural mapped actor death/body attachment/audio/device behavior are open.

## Frozen evidence and reproduction

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Pinned actual binary:
`4d7ab116f8b79922d1305e3b4ace921a38e6f05e22a3c4d04154544106bda655`.

- `pulp-native-allqueries-final/manifest.json`: `96579e694672c63ac5403f87efce8e3568a551472356a7dce3fc24a3c72c6f75`
- `pulp-state-allqueries-final2/manifest.json`: `25d0f010752b679d36c328ad72d7523929e2c45b12566528b56b09ec9f84cf91`
- `pulp-map-rng-final/manifest.json`: `38ccc11edd186532eff0c6c3576306e3c61d2112733555cd79bec683d6465402`
- `pulp-metal-unresident-final/manifest.json`: `fc89dfc8c800db4a9f17fbd466cdd259e393ed423f2aee1a47afdafa68ac01b4`
- `pulp-cold-unresident-final/manifest.json`: `33984b19e842ec719dd43b0f0f833c2f30702fd44f63c8a22429c85efac867e8`
- `pulp-common-unresident-final/manifest.json`: `5bb11489206198357ad3d3aca676bf03ef970bd0dd71b2c148c0e56ed257fb0f`
- `pulp-gallery-unresident-final/manifest.json`: `5249640fbed71733cb960c23f23d0e91b1fc4e371db79de1be789ef691e38e7e`
- `pulp-review-allqueries-final/manifest.json`: `312acc88d649e975b51fbff12709138de682bdfb0a4f149e1389fb5f535c29c2`

Original and source checks:

```sh
$PY tools/retail_runtime/pulp_target_visual_reference.py --boot-image "$BOOT" --output "$OUT/native" --frames 80
$PY tools/retail_runtime/pulp_state_probe.py "$OUT/native/manifest.json" --output "$OUT/state"
$PY tools/retail_runtime/pulp_map_rng_probe.py --boot-image "$BOOT" --output "$OUT/map-rng"
```

GPU scenarios are frozen in the evidence root as `pulp-scenario.json`,
`pulp-cold-scenario.json`, and `pulp-common-scenario.json`. Override only the
binary/output path for integration repeats; the manifests retain full argv,
actual dimensions, timing CSV, file hashes and clean exits. Review construction:

```sh
$PY tools/retail_runtime/port_capture.py --scenario "$SCENARIO" --output "$OUT/metal"
$PY tools/retail_runtime/pulp_visual_review.py --native "$OUT/native" --metal "$OUT/metal" --cold "$OUT/cold" --state "$OUT/state" --output "$OUT/review"
$PY tools/retail_runtime/pulp_gallery_smoke.py --binary "$BINARY" --data-root "$DATA" --output "$OUT/gallery"
```

The gallery route is `TPulp_CONFIGURED_ACTOR`, exact retail ID only. Its static
style plus actual alive callback prevents a fabricated retrigger period.

## Independent root review

Genuine loaded Acolyte/animator and Pulp public Set velocity(0,0,8),counts5/5/5 at declared unresident source(10000,10000,16); full original80ticks plus cold0. Scene1 opaque cull/depth mode and complete own chunk/blood meshes, no procedural body. Source11340poolfields/81RNGrows exact,7120matrixchannels max5.96e-8,386144vertexbytes/22110indices exact. All86 consumed XY terrain queries/result0 and10 real Blood/BloodS allocation failures checked; nine unused native stackZ words retained as differences. Clock0 intrinsic MakeIndex rand/srand/rand reseeds preserved. Wrongclock/missingreseed/doublePulse negatives reject. Ten original/actualMetal whole/detail phases independently reviewed,27pins verified; root81active plus1cold PNGs byte-identical/cleanexit. Roottranslatedgallery20/20visible through458updates, one unreseeded persistent factory,10 intrinsic reseeds, restoredactorflags406c028/health102/cleanexit. Natural death caller/mapped tile success/liveclock/audio/device/light context and1-2pixel coverage/surface quantization remainopen; settledchunks persist, no inventedkill.

Root approval SHA256 `9666ad39e0110b32f049f82af47a3cb19ec0858f8eac6205f8606d7022dc67e2`. Full acceptance remains open.
