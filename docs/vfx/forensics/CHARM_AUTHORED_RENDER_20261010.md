# charm moving star and clover, 2026-10-10

Exact `charm:0xad92bd37` uses `Magic\Charm.I3D`, SHA256
`680c30d3d07cf4204fed4391c6b04fd2b3824f0472b49930b0b05bfff3e9b478`.
The complete original 93-builder registry selects the default factory 40dc00
and animator 5a370c. Its one PartSys controller emits `#clover` from the moving
`#star`; the authored `*path` is hidden. There is no fabricated actor or emitter.

The source preview replaces the old tinted billboard with the actual star
mesh, independently decoded 30-frame orbit, original clover quad/UVs and the
production PartSys simulator. Its 75-slot pool persists across animation wraps.
Authored `scljitter=-0.5:1.5` uses the original integer-tenths implementation
introduced for Fairy/LightStrip. The browser's previous SpellGround policy
reset the placeholder after three seconds; the verified persistent animation
now uses Static policy and the replay asserts exactly one factory spawn.

The declared incoming scene mode is normal 1. At cold ticks 0/1, no valid particle
is drawn and the base remains normal. Subsequent valid particle draws leave
mode 16 active for the normal-lit base. The adapter preserves this color/blend
transition. Cold normal draws use SubmitHelperMesh; the additive-only after-FX
queue would silently reject them. Original cold depth writes versus helper no-write remain an
explicit device/depth limitation in the isolated black fixture. No natural
shared-depth acceptance is inferred.

Validation:

- All 90 object/frame poses match 810 key fields and 1,440 matrix fields from
  original 409430/40a420. Default key traversal negative controls remain intact.
- Whole original Animate 40e2e0/Render 40e8d0 over 122 ticks matches 77,831 live
  source fields and every RNG seed. Accumulated float Z error peaks at
  0.0000305659; the declared state tolerance is 0.00005. No pose/color/time fit.
- Original software images execute the full draw/lighting/kernel at selected
  phases. Other frames retain complete render caller/samples and end at the
  explicit 56eb30 device boundary. Render consumes zero RNG in all 122 ticks.
- Actual native-domain and ordinary projection films each contain 122 nonblack,
  distinct frames with clean exit and one spawn. Cold tick 1 is separately
  captured at 48 Hz after one renderer warmup frame. Eleven active phases include 29→0 and 119→0 wraps.
- Production state/pose probes use ASan/UBSan; previous/current default-zero
  PartSys traces are byte-identical. Focused native cold-state test and full
  build pass. Actual emitter translations/draw counts are checked in GPU logs.

Configuration: 640x480 black; owner/camera 0, source ambient 38/directional 1,
quality 0, MSVC seed 1, 24 Hz, flat ground 0. Native-domain projection is explicit;
ordinary projection residuals are retained separately. Immutable decoded
resources and exact RGB565 texture COM residency are reference boundaries.
Natural buff attachment/trigger/expiration, audio, scene lighting, shared depth
and exact raster pixels remain outside this scoped fixture.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Metal binary:
`a54b80418d6ad58465f28ffbda0d3b89c6465eecafa882cf801ad15bbf3a085e`.

Frozen manifests (SHA256):

- `charm-state-final/manifest.json`: `07296437f2ec8c5cc96da8e6adb5e9f35ef76e6b2403cc982ff2145908affdef`.
- `charm-poses-first/manifest.json`: `e375b3eb47b74352005313972354cf6a34ca143b8c9589e35a65bf1343c048ea`.
- `charm-native-final/manifest.json`: `a5f3fd62ba385ce0b26f6d934bf17a1d046278cd224735f24bd7488dd8060528`.
- `charm-metal-complete-domain/manifest.json`: `2751a787a6ae1e622c63066a3e737c0ce0a96d4f6f63c00b9cab61ea4eafd85d`.
- `charm-metal-complete-common/manifest.json`: `ff32ba52bb3fc83848b004fa46dfde1f29a03668977cbc64edaa5038429b7c70`.
- `charm-metal-cold-final/manifest.json`: `b183c4118887f924e086faa089bb3b031cc8ad7b3d8bb6ab98db3d19605fc23f`.
- `charm-review-complete/manifest.json`: `393ade9753a49fc7554e537e5c350ba47f70c0b0fdf499cab710f0a9fd801ef0`.

The earlier `charm-metal-cold` warmup-zero film contains an initialization
sentinel and is historical, not cold-start evidence. The final cold capture
uses 48 Hz simulation steps with one warmup frame: the first two images both
show tick 1, before any valid particle, and have identical pixels. All 122
active native-domain pixel hashes are unchanged after the cold submission fix.

Appearance is awaiting independent review; no natural caller or exact-pixel
acceptance is inferred from these probes.
