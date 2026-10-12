# Fire: qualified hardware-packet reference, four construction facings

Exact `Fire` `0x119f01e3` / `Misc/Fire.I3D`, retaining the installed static
quad, authored old-format key, texture selector0 and persistent default owner.
No production source change is needed beyond the already-integrated exact Fire
adapter. This is not a33-frame flame animation and no clock is added.

`fire_device_reference.py` runs the genuine cached factory with construction
facings0/64/128/192, actual CreateAnimator and67 full NextFrame/Pulse/Animate/
PreRender/Render/PostRender cycles per facing. It executes the original scene
alpha-enable producer and actual hardware `DrawIndexedPrimitive` dispatch4174fd.
It records the selected COM material and unmodified ARGB4444 texture0, original
FVF0x112 vertices/indices, world matrix, states and texture-stage operations.
The real calls select MODULATE color/alpha, SRCALPHA/INVSRCALPHA, CULL_NONE,
Z-test/noZwrite. All268 updates retain frame0, selector0 and zero RNG calls.

The **actual selected material**, observed at IDirect3DMaterial3::SetMaterial,
has diffuse/ambient/specular/emissive color components all1. Its white emissive
term saturates hardware diffuse to1 independently of nonnegative light terms.
`emissive_saturation` strictly rejects any other material rather than inventing
lighting or feeding a fabricated white carrier. Original transform execution
with zero triangle indices supplies native-domain projected coordinates; the
qualified device does not import production geometry, shaders or render code.

The independent device decodes nibble/15 ARGB4444, performs MODULATE with the
proven saturated material, and applies source-alpha/inverse-source-alpha to a
declared linear RGB8 UNORM target with nearest rounding per fragment. All65,536
words ×4 destination controls pass786,432 channel comparisons against integer
rational arithmetic. Material rejection, zero-alpha, shared-edge ownership,
integer/half-center and coordinate-convention translation tests also pass.

Both point and bilinear filters and both fixed sample-center conventions are
retained. Cached software startup never initialized hardware filtering, so no
period-GPU filter is inferred. This distinction is causal at the tiny quad size:
[Microsoft documents legacy integer pixel centers](https://learn.microsoft.com/en-us/windows/win32/direct3d9/rasterization-rules),
while [Apple documents Metal centers offset0.5](https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Render-Ctx/Render-Ctx.html).
These are explicit device configurations, not fitted vertex/UV offsets. The
qualified modern-half-center/point interpretation is pixel-exact to current
Metal at all four facings:

- 0: pixel633,469 RGB1,0,0.
- 64: pixel632,462 RGB56,24,8.
- 128: pixel646,462 RGB1,0,0.
- 192: pixel647,469 RGB56,24,8.

The legacy integer/point interpretation has only facing0's pixel633,470
RGB23,5,0; its other facings are blank. Bilinear controls differ too. The actual
original RGB565 software span/UV reference remains a separate, different result.
No unchanged D3D6 driver, software-raster or arbitrary sampler fidelity is claimed.

Private evidence under `research/vfx-wide-20261010/hard-alpha/`:

- `fire-device-first/manifest.json`: first genuine four-facing hardware capture.
- `fire-device-final/manifest.json`: fresh full268-frame run and four device
controls; all12 retained full packets/materials/textures/states replay exactly.
- `fire-device-metal/source-checkpoint.json`: immutable copy of root source build,
SHA`f2afa790c115ef5d5596a0bcc760a3da29d851e3378a9d8dc78a810123d61d44`.
- `fire-device-metal/face{0,64,128,192}/sparse/`: actual67-render source runs,
sampled at updates1/34/67. All4,288 logged matrix channels match original within
1.8249512123702516e−8; all frames/texture selection/RNG remain0.
- `fire-device-review-final/pair-manifest.json`:88 pinned files,12 exactRGB image
pairs, fixed40×40 crop12×nearest detail with no intensity boost. Pair SHA
`a26c1ae328a39d0e861a0b26fdd5d415846c7f902a422d08a7792808ebd004a9`.

The native record0 follows the first actual NextFrame/Pulse, so `record.tick+1`
is matched to `round(source simulation_seconds*24)` directly. No phase fitting
or skipped Render occurs. The earlier consecutive3-frame captures are retained
and agree with the longer sparse runs.

Reproduction, using the existing root `fire_cardinal_native_probe.py`:

```sh
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" tools/retail_runtime/test_fire_device.py
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" tools/retail_runtime/fire_device_reference.py --boot-image "$BOOT" --output "$OUT"
```

Natural map/caller/audio, animated-fire expectations, original software span
behavior, period-device implementation and general material lighting remain
outside this exact static qualified-device scope. Root acceptance is separate.

## Root independent qualified approval

Exact Fire 0x119f01e3 installed oldbody staticquad/key/defaultowner/textureselector0, no invented33frame clock. Fourgenuine constructionfacings0/64/128/192 execute268originalupdates/actualhardwareDrawIndexedPrimitive FVF112/COMmaterial+ARGB4444texture0, allframe0/selector0/RNG0. Captured all-white diffuse/ambient/specular/emissive material proves fixedfunctiondiffuse saturation; strictmaterial rejects unrelated lighting inputs. IndependentRGBA4444 MODULATE/SRCALPHA-INVSRCALPHA qualifiedRGB8linearUNORM device uses explicitlydeclaredmodernhalfcenters/pointfilter; legacyinteger/point andbothlinear controls retained, no periodGPUclaim. Foursemanticunits pass786432blendchannels, materials/zeroalpha/sharededge/centertranslation controls. Rootfreshoriginal12packets/state/48samplerimages repeatexact, source268renderedupdates/4288worldmatrixfields max1.825e-8, all12sampledRGBframes exact toqualifieddevice. All13whole/detailpanels byteidentical,88pins verified, independentlyinspected. Actualsource unchanged fromexactFireadapter, no color/geometry/UV/phasefit. Originalsoftware span/UVdiffers, naturalscene/caller/audio/animated-fire expectations/generalmateriallighting/filter/perioddevice/exactunchanged-retailfidelity remainopen. qualified_reference=true,semantic_device_reference=true,device_identical=false; exactRGBclaim onlydeclaredmoderncenter/point device configuration.

Report: `recon/retail_asm/runtime/effects/current-fire-qualified-device-20261011/root-approval.json`. Scoped visual passes173/176 include3modified-retail and2qualifiedsemantic-device configurations.
