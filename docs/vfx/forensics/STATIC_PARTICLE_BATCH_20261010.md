# Regeneration, SwiftStrike and Fspray: three exact particle frontends

This batch admits three previously unsupported shipped EFFECT rows through
their exact constant-emitter profiles. It preserves every authored emitter,
prototype, material, texture, key and tag; it adds no substitute billboard.

- `Regeneration:0x10ac03de`, `magic\Regen.i3d`, SHA256
  `42124796a4e5573f58e34d5afd96e735e132c87bd30d2047b5d74aeae21dff8f`.
  Thirty-two emitters, 34 objects, 37 slots, explicit friction0.
- `SwiftStrike:0xe0a3bc43`, `magic\SwiftStrike.i3d`, SHA256
  `45896fb4ff8d19253a242849b496b52023ca4a91fe5b2ec3a540dfde1b6b384c`.
  Thirty-two emitters, 33 objects, 37 slots, original default friction.
- `fspray:0x0c052638`, `misc\Fspray.i3d`, SHA256
  `1d883eeba4cb9ee857849fd9635084e84ae5ebf5f328c857fd4038a786fada4e`.
  One emitter, two objects, 80 slots, authored velocity/gravity/scale/color curves.

All three have one state0 partsys tag and one four-vertex/two-face RGB565
prototype. Other objects have no geometry. Their independent scalar root keys
are constant; no integer-key traversal policy or base-mesh helper is changed.
The strict source profile pins all names/key bytes/material floats, prototype
vertices/local indices, tags, header counts and texture descriptors. Its
typed gate rejects mismatched IDs. Only these profiles receive the recovered
`#` camera orientation, own RGB5 prelit submission and zero-scale draw gate.

## Original versus compiled production

Complete default constructor `0x406160` and all 92 named animator constructors
execute. Actual lookup selects default builder `0x5e8508`/factory `0x40dc00`
and actual factory creates leaf `0x5a370c`. Original NewObject punctuation,
full controller parser/Initialize/Pulse/live RenderSample, authored key/matrix
decoder, whole `#` CalcObjectMatrix and x87 point transforms run from the EXE.
Resource decoding, exact copied prototype records, RNG seed1 and flat ground0
are explicit fixture boundaries.

Actual production parser/State/SubmitPartSys compile under ASan/UBSan. Native
decoded emitter matrices are shared inputs, rather than an independent port
emitter-pose proof. Each type repeats its complete native state/pose trace in
two fresh runtimes, with identical hashes. Regeneration and SwiftStrike each
pass 102,596 meaningful state fields and 2,828 draws from 3,013 live samples;
Fspray passes 218,072 fields and 4,232 draws from 4,438 samples. The 576 total
zero-scale samples consume their original render sampling before both frontends
reject geometry. Never-born scale/alpha defaults are the only state exclusions.

All 15 sampled RGB565 image pairs are nonempty, have zero differing pixels and
equal depth, with two identical warm replays per pair. Native visible pixel
ranges are Regeneration228–1191, SwiftStrike335–1284 and Fspray130–667.
Maximum corner error is `2.10e-5`; maximum UV error is `3.42e-10`. Independent
raw-Z/native FIX checks retain the declared common MODELZ domain. Original
projection/raster/additive tables are shared; no raster or pixel equation is
replaced. These are bounded particle frontends, not full device/caller acceptance.

The actual compiled strict-profile contract passes 12 accepts and 1,572
mutations. It mutates every key word/material float/vertex channel/index plus
path/header/tag/type/texture bounds. Engine admission checks descriptors;
the exact archive SHA separately pins fixture texels.

## Real map and verification

Fresh private `build-merge/Revenant` builds with `-j4`, SHA256
`d95e9e476304f9d3c24c3bd14d80115a18c570b960f69cb5f627ba416e44b576`.
All three 48-frame FontRuntimeLab captures pass eight typed create/controller/
emitter/capacity/live draw/MOVE/DELETE/absence rows. Each has 30 distinct visible
frames and exact post-delete floor restoration, with clean exit0. Cold creation
and newborn scale0 correctly leave frames3/4 empty; first visibility is frame5.
The first map checker incorrectly required immediate visibility. Its failed
reports remain retained; the corrected checker reproduces identical images
with the unchanged binary. No geometry, lighting or timeline was fitted.

Two batch regressions and the existing same-source Speed and Quicksilver
regressions pass. Independent compiled emitter-pose matrices, actual natural
character/spell triggers, scene lighting/collision and native-versus-Metal
appearance remain open. Full acceptance remains false. YEnergy/YEnergyLose
are deferred for quoted identifiers/fractional emittersize; Ogrestrength and
trollblood need animated `#` emitters/additional geometry. The previously hard
Labback/Quick/Createfood/EnergySpray rows were not retried.

## Reproduce and retained evidence

```sh
PYTHONPATH=tools/retail_asm python tools/retail_runtime/static_particles_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <private-directory>
PYTHONPATH=tools/retail_asm python -m unittest discover \
  -s tools/retail_runtime -p test_static_particles.py
```

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-new-effect-batch-20261010/`.
Aggregate manifest SHA256 values:

- `render-final/manifest.json`: `dc2ef33d1880dfd60d0724298e6b525f6ac658e0e691f7b816913653881a0e13`.
- `profile-contract/manifest.json`: `3765052bd66d0b16daa0a2783fb466df7a2dc3c6d4abb314ecea4779679251f6`.
- `map-checked-final/manifest.json`: `6cdc61675388cb6c745cf6a460a3096fd258fadcd548c331459ca29d2e38a333`.

Per-type native/production packets, images, replay hashes and source/binary
fingerprints reside beside each manifest. Generated executables and licensed
assets stay local. No DOSBox or central-ledger change is used.
