# Speed strict profile and integer decoder contract

Exact shipped EFFECT `speed:0xad92bd36` binds `magic\Speed.i3d`, SHA256
`65c45c03bcf3cd5cfe549232f88fb18550a30720c5b9500d0cfd5ef8154fce42`.
The production `ValidateRetailSpeedPartSysProfile`, `SkipAniKey32`,
`GetAniKey32` and `GetUninterpolatedAniKey` bodies compile and execute against
literal decoded asset records under ASan/UBSan. Real packed-key constants,
`SAniKey32`, vertex, face and material definitions are extracted from source.
The resource provider returns the complete original local quads, indices,
keys, materials, tags and texture descriptors. No engine source is edited.

Four literal/path cases admit the exact profile. All 258 mutation cases reject:
six path, eleven header, 53 individual key words, 21 object fields, nine texture
face bins, 51 material floats, three material-to-texture mappings, twelve tag,
six texture descriptor, ten pixel-format, 64 individual quad floats and twelve
local triangle indices. Texture tests cover descriptor identity; the production
profile does not hash raw texels. The exact archive asset SHA pins the unmodified
texels supplied by this fixture.

Two independent synthetic key-boundary controls verify that admitted Speed uses
the original inclusive integer traversal, while unrelated imagery preserves
the existing half-open policy. The actual shipped profile then supplies all
three objects at all 30 integer frames: 90 poses, 810 position/rotation/scale
channels. Original executable decoder `0x409430` and conversion `0x4105d0`
agree with compiled production with **zero absolute channel error**. One focused
native regression passes. All source hashes are stable before and after each run.

This establishes strict profile admission and integer keys only. Full imagery
loading, fractional/cross-state interpolation, owner/camera matrices, particle
geometry submission, lighting, software pixels, Metal and natural caller/runtime
remain separate. No new rendered A/B or full acceptance credit is granted.

```sh
PYTHONPATH=tools/retail_asm python tools/retail_runtime/speed_profile_contract.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <private-directory>
PYTHONPATH=tools/retail_asm python -m unittest discover \
  -s tools/retail_runtime -p test_speed_profile_contract.py
```

Retained manifest:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/speed-profile-contract/manifest.json`.
SHA256: `2328e4e88720dc7c184aef2cde9aaa4a28d78f4303af2bb728ffcefa062880ca`.
