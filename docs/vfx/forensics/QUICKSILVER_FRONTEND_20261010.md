# Quicksilver: exact profile, particle frontend and map lifecycle

Follow-up: [independent production emitter matrices and original normal-lit
base pixels](BUFF_BASE_LIGHTING_20261010.md) now replace the shared emitter-pose
boundary in the particle probe. The earlier report below remains historical.

EFFECT `quicksilver:0xad92bd35` binds shipped `magic\Quicksilver.i3d`, SHA256
`07c6a0a48c32d891ccd29367b12a882a4244a3eea004e4b06a7c2dee7aa609b4`.
It shares Speed's three-object controller architecture, but has its own
38-word nonuniform emitter scale stream, base quad normals/UVs, blue base
texture and blue particle color curve. Those differences remain authored inputs.

The new strict profile pins the exact path, headers, keys, tags, material
floats, quad bytes, local faces and texture descriptors. Only exact Speed
`0xad92bd36` and Quicksilver `0xad92bd35` with their corresponding profile
flags enter the shared matrix/submission policy. Unsupported variants do not
draw static substitutes. Quicksilver now uses the existing source-backed
camera matrix helper, inclusive integer keys, base mode80 and particle mode16,
with the zero-scale draw rejection applied after render RNG sampling.

The actual compiled profile and integer decoder pass four accepts, 258
mutation rejects, two traversal-policy controls and all 810 authored TRS
channels against original `0x409430`; maximum channel error is zero.
The original full parser/Initialize/Pulse/live RenderSample compared with
independent production parser/State passes 85,614 meaningful fields over
31 slots and 90 ticks. Only never-born scale/alpha defaults are excluded.

The particle frontend compiles actual production `SubmitPartSys` under
ASan/UBSan. Original `#` CalcObjectMatrix and x87 vertex transforms produce
the independent geometry. All 2,221 live samples yield 2,109 matching draws
and 112 zero-scale rejections, retaining the render RNG sequence. Maximum
corner/UV errors are `3.07e-6`/`2.63e-10`. All 4,218 raw-center/native FIX checks
pass. Five nonempty shared-original RGB565 raster pairs have zero differing
pixels and equal depth, with two identical warm replays per pair.

Scope is **particle-only** rendering in the declared common MODELZ domain,
using exact native-decoded authored emitter matrices as shared inputs.
Independent production emitter/base matrix submission, full base material
illumination/software pixels, fractional poses, Metal projection parity and
natural caster/caller/context remain separate. The profile's descriptor tests
do not hash raw texels; the pinned archive SHA establishes fixture texel identity.
No full-effect visual acceptance is granted.

The fresh private Metal build `build-quicksilver/Revenant` passes eight typed
ADDAT/controller/capacity/MOVE/DELETE/absence observations. Its 48-frame
FontRuntimeLab capture contains 31 distinct active images and restores the
initial floor exactly after deletion. Filmstrip inspection shows the authored
blue composite and its movement. This is controlled runtime evidence, without
a native full-base appearance comparison. Binary SHA256:
`52748e34f6bc58b6454b17a0996bb8ae35354df3c79d0ef1125cce3b1136d8ff`.

Focused Quicksilver, same-source Speed packet and Speed profile regressions
pass. The game builds successfully. Generated assets/captures remain local;
no DOSBox, geometry/color fitting or central-ledger edit is used.

## Reproduce and evidence

```sh
PYTHONPATH=tools/retail_asm python tools/retail_runtime/quicksilver_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <private-directory>
PYTHONPATH=tools/retail_asm python -m unittest discover \
  -s tools/retail_runtime -p test_quicksilver.py
```

Retained under `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261010/`:

- `quicksilver-particle-merge-final/manifest.json`, SHA256
  `a074542fc38e0525f54511a8c09eaab262177c974cd6fad3bee36166ea21600c`.
- `quicksilver-profile-contract/manifest.json`, SHA256
  `330095bb55dcb366542cfb158ae08b28c113061ed5b2e62ad65e5c84d2ac9eec`.
- `quicksilver-map-final/manifest.json`, SHA256
  `ae8ee36b6071c089175578909648e46a24e46620687928549df1d0e3456a6574`.

The interrupted `quicksilver-particle-final` directory has no completed
manifest and supplies no credit. Earlier feasibility reports remain diagnostics.
