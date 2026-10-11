# Four exact particle profiles, 2026-10-10

YEnergy (`0xaeaeeb30`), YEnergyLose (`0xaeaeeb33`), YAbsorb
(`0xaeaeeb29`) and nullifier (`0xad92bd38`) now enter the existing bounded
particle frontend through literal asset profiles. This adds executable ports,
independent production geometry comparisons, and actual map lifecycle captures;
it does not establish full original-game visual acceptance.

## Source corrections

The original whole particle parser accepts quoted value names. Its numeric token
is type 8 even for decimal literals: ParseItem `404432..404443` copies the token's
integer view into controller `+108`. Thus the authored `emittersize=0.25` is zero,
not a quarter-unit sphere. The production parser now accepts quoted object,
prototype and emitter-type values and truncates decimal emitter-size literals
toward zero using the parsed double. Nine native diagnostics include negative
values, `0.99999999`, absent size, and original rejection of `1e2`; downstream
PPS45 and prototype selection also agree. These diagnostics receive no effect
coverage credit.

Each new profile pins every object's name, parent, material, packed keys,
vertex/face counts, state length/flags, literal tag order/frame/parameters,
material float fingerprints, the four prototype vertices and six face indices,
and the RGB565 texture descriptor. Literal CLASS EFFECT bindings and whole
asset SHA256 are checked by the probes. Texture texels are fixture-pinned,
not hashed by engine admission. Nullifier's prototype is object1/material1;
its eight hidden `*line` helpers contain vertices but zero faces. They remain
hidden; their unused vertex values are not engine fingerprint inputs.

Quoted profiles admit their exact `blendcont=litaddz` companion, whose original
Initialize immediately writes mode80. The narrow adapter restores `#` camera
and `*` hide flags, includes these exact profiles in the original inclusive
integer-key policy, and applies the existing sampled near-zero-scale rejection
to the pinned prototype regardless of object index. Rejection still follows
render RNG sampling; no particle generation or extra templates were introduced.
The merged fifth SubmitPartSys argument and native-projection ABI are preserved.

## Executed evidence

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-y-family-20261010/`.
No binary assets or generated evidence are committed.

- `parser-contract/manifest.json`: nine whole native parser diagnostics pass
  against actual compiled production parser under ASan/UBSan.
- `profile-final/manifest.json`: four literal profiles and twelve path variants
  accept; 1,462 independent mutations reject under ASan/UBSan.
- `pose-contract/manifest.json`: actual GetUninterpolatedAniKey, Skip/Get integer
  conversion, MakeMatrix and production math compile and execute. All 32,805
  TRS channels match original `409430`; all 51,040 root emitter matrix channels
  match whole original `40a420`. Maximum TRS error is below 5e-7 and matrix error
  below 6.1e-8. Nullifier's animated 33-word emitter tracks are included.
- `render-independent-final/manifest.json`: final renderer inputs come from the
  separately compiled production pose artifacts, including object0 origin,
  every emitter's TRS and live nonuniform matrix. Original full constructors,
  all93 registration constructors, literal factory `40dc00`, whole parser,
  Initialize/Pulse/RenderSample, prototype CalcObjectMatrix and x87 vertex
  transform execute. Two fresh native full-pool/pose replays must hash equally.
  Independent compiled State/SubmitPartSys checks every live quad and UV.
- `map-final/manifest.json`: all four real map runs pass 8 typed lifecycle
  observations and 48 frames each, with active particles, move `(16,-8,4)`,
  deletion and exact floor restoration. All four filmstrips were inspected:
  green/yellow authored textures, distinct single-emitter and scattered emitter
  patterns, visible motion and clean deletion. YAbsorb's first visible frame is
  7; the others first show on frame5. Native scene/device comparison is open.

Final per-type full-state checks / live samples / drawn quads / sampled scale
rejections: YEnergy 305258 / 6438 / 6270 / 168; YEnergyLose 295072 / 867 / 851 / 16;
YAbsorb 111766 / 1704 / 989 / 715; nullifier 336962 / 3779 / 3263 / 516.
Total 1,049,058 meaningful state checks and 11,373 complete native/port quads.
Five nonempty RGB565/depth pairs per type, repeated twice, require zero differing
pixels and identical complete depth surfaces.

All four use the same declared 1024-square original software viewport, camera0,
zdist1925, full literal authored mesh and texture. YEnergy/YEnergyLose raster
samples are ticks5/9/15/20/25; YAbsorb uses5/15/29/45/59; nullifier5/15/29/60/89.
No resizing or geometry fitting occurs. Long-lived negative0.3 gravity produces
floating-point accumulation below9.3e-5 in 90ticks; the two Energy rows explicitly
bound geometry to3e-4 while pixel equality remains exact. YAbsorb uses the same
3e-4 bound: independently compiled emitter matrices differ below6.1e-8 from
original x87 matrices, propagated into 17 late XY samples up to0.000184. The
common-native-pose control stays below1.1e-5. Nullifier retains6e-5. The initial
independent-pose attempt is retained in `render-independent-attempt/`; its four
RGB565/depth comparisons pass exactly, and its YAbsorb numerical bound failed.
An earlier later-life YEnergy raster attempt left the fixed viewport and is
retained in `render-final/`; `render-early-final/` is the superseded common-native-
pose comparison. Neither supplies the final independent-production credit.

## Validation and limits

Private `cmake --build build-merge -j4` passes; binary SHA256
`c33177a93ce0f969e0164e0e204ee305744035e7f92b7dc4c10ec5e60d18fbb0`.
The three quoted-parser/profile/pose unit tests pass, the previous three static
particle unit tests pass, and same-source Speed and Quicksilver packet/native
software regressions pass. Source and binary hashes are recorded in each report.

Original software projection/raster and its canonical additive white-light
fixture are shared after independent frontend geometry. Original RenderObject
normal lighting, original map/device projection, natural spell/caster/parent
hierarchy, collision walk-height and caller animation clocks remain open.
Nonloop YEnergyLose/YAbsorb trace frames clamp to their authored final frame;
this is a declared bounded clock, not natural spell lifetime acceptance.
Map screenshots use the actual production Metal path but do not claim original
scene equivalence. Full accepted rows remain zero and every manifest keeps
`accepted=false`. Central ledger/index ownership remains with integration.
