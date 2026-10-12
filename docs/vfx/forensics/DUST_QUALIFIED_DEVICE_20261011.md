# Dust: qualified hardware semantics and exact RGB8 policy correction

Exact `Dust` `0x0c052637` / `Misc/Dustcloud.I3D`. The original software
RGB565 additive leaf remains mathematically blank. This work does not brighten
that renderer or claim unchanged retail-device fidelity.

`dust_device_reference.py` executes the genuine cached NewObject/CreateAnimator,
NextFrame/full owner Pulse/Animate, original scene alpha-enable producer
`4132c7..413342`, and full default Render through original hardware dispatch
`4174fd`. The explicit device boundary records actual `DrawIndexedPrimitive`
FVF `0x1e2`, world matrices, RGB8 diffuse, faces and original COM RGB565 pixels.
The original calls select texture × diffuse MODULATE, ONE/ONE, CULL_NONE,
Z-test and no Z-write. No effect/particle/texture/vertex handler is replaced.
The original transform is separately executed with zero triangle indices to
supply native-domain screen coordinates. No production renderer code is imported.

The qualified CPU device independently implements orthographic affine triangles,
integer-center/top-left coverage, RGB565 normalization, RGB8 diffuse modulation
and additive blending to an explicitly declared linear RGB8 UNORM target with
round-to-nearest after each fragment. Both point and bilinear samplers are
retained: cached software startup did **not** initialize hardware filters.
Neither sampler nor this target's precision is claimed as a particular period GPU.
The semantics follow Microsoft's [MODULATE definition](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dtextureop)
and [ONE blend factor](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dblend).

This exposed a narrow port error: Dust's modern path also truncated its actual
RGB8 diffuse to RGB5. Only Dust now selects RGB8 when
`!renderer.UsesRetailSoftwareMeshLighting()`. Software policy still uses RGB5;
projection mode does not select the color policy. All other profiles are unchanged.
The compiled actual submission block passes 21,504 channel/policy controls.
Independent complete-byte modulation and shared-diagonal/zero raster tests pass.
Private full build passes; actual modern Metal has18 visible phases, software
policy remains blank. The two modern-policy captures differing only in ignored
legacy INI Software3D setting are byte-identical (not a software-policy control).
The actual software control explicitly uses `--scene-ambient=32,255,255,255`.

Private evidence below `research/vfx-wide-20261010/hard-alpha/`:

- `dust-device-second/manifest.json`:25 actual controller/device frames.
- `dust-device-repeat/manifest.json`: fresh25-frame replay, all packets, owner
  states, RNG, pool hashes and both sampler pixels identical.
- `dust-device-software-control.json`:25 original software frames blank, with
  all25 state/RNG/pool hashes identical to the qualified hardware-dispatch run.
- `dust-device-metal/modern/film/manifest.json`: actual25-image Metal film.
- `dust-device-metal/software/actual-software/manifest.json`: actual software
  policy25-image blank control (visible-validator rejection is expected).
- `dust-device-review-final/pair-manifest.json`:23 phase-matched pairs, source
  film extra blank tail retained. Tick is `round(simulation_seconds*24)` directly;
  there is no fitted phase offset, geometry, color, UV, seed or lifetime.
- `dust-device-second/capture-source.py` preserves the exact capture tool before
  the later CLI-only software-control entry was added.

Reproduction (existing root tools provide `dust_loaded_context_probe.py`):

```sh
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" tools/retail_runtime/test_dust_device.py
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" tools/retail_runtime/dust_device_reference.py --boot-image "$BOOT" --output "$OUT"
PYTHONPATH=tools/retail_runtime:tools/retail_asm "$PY" tools/retail_runtime/dust_device_reference.py --boot-image "$BOOT" --software-control-reference "$OUT/manifest.json" --output "$SW"
```

This is a qualified semantic-device appearance reference, not a patch to the
original software kernel. Original software blankness, hardware initialization,
coverage/interpolation/target rounding, natural caller/map/audio, period-device
behavior and exact pixels remain explicit limits. Root approval is separate.
