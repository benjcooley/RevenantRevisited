# FireCone original render packet diagnostic

2026-10-10 follow-up: [executed owner composition and normal-lit software
pixels](FIRECONE_OWNER_LIGHTING_20261010.md) identify and correct the relative
translation defect. The raw-stage results below describe the prior source.

2026-10-09: original FireCone Render `4eaae0` and shared particle Render
`50c220` execute in the thin runtime. The actual compiled production
`TFireConeEffect_Bespoke::SubmitPool` also executes against compiled production
states already verified by the [complete state proof](FIRECONE_THIN_STATE_20261009.md).
This bounded run retains raw packets and an unresolved coordinate-stage residual.
It does **not** establish rendered fidelity or justify a production correction.

## Executed boundaries

The native leaf calls fire, smoke, then burst, with both shared Render arguments
zero: `flicker=false`, `abs_pos=false`. Every used slot reaches the original
imagery submit boundary with `OBJ3D_MATRIX` (`0x100`). Native matrix helpers
execute; original `43ad80` transforms all four authored corners. Scene
Save/Set/Restore requests and imagery submit/extents are recorded boundaries;
the fixture does not invent a working device. Existing owner/base/audio/asset
boundaries remain as declared in the state proof.

The production driver compiles the current SubmitPool body and `math3d.cpp`.
Its only instrumentation identifies the particle slot immediately before the
real SubmitHelperMesh call. It loads both complete authored vertex/normal/UV
streams, local indices and materials, and retains both source textures in the
asset artifact. Candidate corners use compiled MtxTransform. An origin-zero
owner retains the existing common-world `WORLD3D_Z_SCALE=1.5` matrix; native
packets are captured before native imagery applies its owner/world stages.
These raw stages are deliberately named and never treated as equivalent.

The run covers face bytes 0, 64, 128 and 192 through tick 100. Every tick's native
state is checked against the compiled production state, including all 260 slots,
defined active fields and RNG count. Selected rendered ticks are
0,1,2,6,15,20,33,60,92,93,100. Genuine particles move and drain: face-zero draw
counts are 0,4,8,24,57,77,90,50,1,0,0, matching production at those ticks.
Native owner expiry occurs at tick 93. The complete state prerequisite retains
73,806 exact active float fields and 1,243 RNG calls.

## Residual and bounded deferral

At face 0/tick 1, maximum raw matrix X/Y column residuals are
`1.49054e-8` and `1.48069e-8`. Raw Z column residual is `0.10946013`;
maximum authored-corner Z difference is `7.66749764`. Production's linear Z
terms carry its explicit owner 1.5 stretch; translation matches the original
first particle `(21,4,-4)`. No inverse 1.46, common-Z rescaling, fitted matrix,
camera or software-pixel adapter is applied to erase this residual.

Native `50c374..50c394` copies raw particle position into the object and branches
on `abs_pos`; the Z conversion at `50c396..50c3b2` runs only for nonzero
`abs_pos`. FireCone's actual leaf passes zero. Consequently the historical
snapshot statement that this relative shared draw always translates by
FIX_Z_VALUE is not a native fact. Production's current translation combines
FIX/REV_FIX and the owner stretch and numerically matches here; this evidence
alone does not show a translation defect.

At nonzero facings, the production SubmitPool performs its projected-face cull
before SubmitHelperMesh, while the native capture is before downstream imagery
culling. For example face 128/tick 1 emits 4 native pre-cull packets and 0 production
packets. Without executing the original downstream cull/projector, that count
difference cannot be labeled missing final draws. Zero residuals in a case with
zero paired packets carry no geometric parity claim.

The owner/world/projector bridge needs separate grounded evidence before a
software pixel pair is meaningful. This task stops at the retained raw-stage
diagnostic, with no source/render/visual/device acceptance credit and no
production edit. White vertex illumination was not needed at this boundary.

## Reproduction and tests

```sh
python tools/retail_runtime/firecone_render_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output /tmp/firecone-render-new-run
python -m unittest discover -s tools/retail_runtime -p 'test_firecone_render.py'
```

The output directory must be new. Three tests pass in 7.4 seconds: native complete
used-slot order, moving state and drain plus the unresolved raw-stage residual;
an explicit negative-uniform-scale/flicker-flag boundary control; and a wrong
particle degree-conversion negative control. The latter produces a large X/Y
matrix residual before any bridge. The generated selected lifecycle states
contain no negative scale, so that separate scale control is explicitly
synthetic and gains no natural-lifecycle coverage.

Final retained report:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/firecone-raw-render-stage-verified/manifest.json`,
SHA256 `45ac39f6954e022e19649cbb9d9dde29ea27e6acb5fccb2cb1ca48ea61e908d2`.
It contains 44 cases, native/production matrices and transformed corners,
authored UVs, both texture hashes, all omitted packet keys, render argument and
scene entry order, source/body/driver/binary hashes and the complete state-proof
manifest hash. The original executable and FireCone asset remain respectively
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5` and
`8683cc1a0921f4c0bb7ef8544e94e1e391b180caca0db14260e8538fba948802`.

Live caster/target interactions, owner movement/rotation matrices, damage/burn,
audio, native lighting, downstream culling, pixel blend/raster, the complete
renderer and DragonFire remain outside this diagnostic.
