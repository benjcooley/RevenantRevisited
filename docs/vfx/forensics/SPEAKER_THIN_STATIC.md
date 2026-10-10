# Speaker: bounded generic-mesh comparison

The shipped `speaker` EFFECT (`0xad92bc10`) now has a passing thin-emulator comparison for its complete constant root mesh, material decoding, and software pixels. This is one bounded frontend result, not full gameplay, lighting, audio, or device acceptance. No game source correction was needed.

## Exact admission

The retail archive's CLASS row is:

```text
"speaker" "Misc\Speaker.I3D" 0xad92bc10 {0,0} {}
```

The exact asset SHA-256 is `a0255572172177816cdaf3f81185ef7d398df7063faf0c5d5db33547e8f42949`. The asset has one independent `chamfercyl` root, 55 vertices, 30 faces, one material, zero textures, zero controller tags, and a constant `STILL` state with 100 frames. Its nine compressed scalar keys preserve authored translation and scale. Body version 2 uses flags `0xfc`; the extra `HASPLAYSOUND` bit is retained rather than silently replacing the asset with a version 3 fixture.

Retail explicitly registers `Speaker` at `0x4f48da`, builder `0x66cdc0`, builder vtable `0x5a359c`. Actual registry constructor `0x40db90` and lookup `0x40dca0` resolve it. The builder's factory `0x40dc00` assigns the generic `T3DAnimator` vtable `0x5a370c`. This is an explicit generic registration, not the absent-name fallback used by some other assets.

## Compared behavior

[The probe](../../../tools/retail_runtime/speaker_static_probe.py) executes original key decoding, object matrix calculation and point transformation, then compares them with compiled current production `GetAniKey`, `CalcObjectMatrix`, mesh extraction, generic static `SubmitMesh`, and `LoadMaterial`. All 55 vertices and all 30 faces are supplied from the pinned authored asset. The material helper emits hexadecimal floating point values so its fractional binary32 diffuse channels are compared without decimal formatting loss.

Frames 0, 50 and 99 are sampled at owners `(0,0,0)` and `(16,-8,4)`. Each of the six cases is replayed twice. The fixed viewport is 512×512; camera `(0,0,0)` and projection distance 1925 remain unchanged. Geometry is not resized or fitted. Original software projection and its untextured Gouraud raster are shared by both frontend packets, with culling disabled and depth test/write enabled. Authored diffuse color is supplied directly as five-bit vertex input; the native illumination producer is outside this fixture.

The October 9, 2026 run passes:

- Six image/depth pairs, each repeated twice; zero differing RGB565 pixels and byte-identical depth.
- At least 613 visible pixels in every case; owner translation visibly changes the image.
- Maximum transformed-position error `9.563256835321e-7`; UV error zero.
- Constant pose/image at frames 0, 50 and 99 for each owner.
- 24 original untextured raster calls and zero textured calls.
- Median complete warm pair about 28 ms in this run.

The exact-flags compiled fixture's complete trace also equals the common fixture's `0xdc` trace, proving that the additional sound bit does not change these mesh packets. Both production source spans and the material span are checked for changes during the run.

## Reproduce

From the VFX worktree:

```sh
/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/speaker_static_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/speaker-static-ab-20261009
```

The local manifest is `recon/retail_asm/runtime/effects/speaker-static-ab-20261009/manifest.json`, SHA-256 `23709f25db614c631f8b74256d5a50bd01dc1d3eca7da43f03183435247771b9`. It records the retail executable, asset, registry, probe, compiled adapter and production source hashes, plus every packet and pixel/depth hash. Images, generated executables and manifests remain ignored local artifacts.

## Remaining gates

The actual map lifecycle, natural speaker owner/caller, authored sound behavior, original vertex illumination, camera/culling policy, modern GPU renderer, and rendered game scene remain unverified. This result establishes the small generic mesh frontend and shared original software raster only. It does not claim an animated particle effect, audio correctness, or full visual acceptance.
