# TeleportDoorInside atlas family

The seven exact retail variants use a custom shared animator, not the default
static animator: B/O/P/R/W/Y/G, IDs `ad92bc28`, `ad92bc29`, `ad92bc30` through
`ad92bc34`, and their corresponding `Magic/Warp?.I3D` assets.

`tools/retail_runtime/warp_probe.py` executes the full static builder initializers
at `0x4ff8f0` through `0x4ff9b0`. Calling only `0x40db90` would erase the leaf
factory vtable: each initializer also installs its custom vtable. Actual lookup
`0x40dca0` returns the seven distinct builders; their actual factories are
`0x501010`, `0x5010e0`, `0x5011b0`, `0x501280`, `0x501350`, `0x501420`, and
`0x5014f0`. Their allocated 0x134-byte animators have separate leaf vtables but
share Init `0x4ff9d0`, Animate `0x4ffa60`, and Render `0x4ffae0`.

The assets contain one independent `gate` root, four authored vertices, **three
faces**, nine constant scalar keys, one 256×256 ARGB4444 texture, one STILL frame,
and no controller tags. Keep all three faces, including the overlapping triangle.
There is no random input or projectile motion. Owner placement changes the mesh
world position; animation changes only UVs.

Init saves the four original UV pairs, clears U/V, sets increments to +0.25/-0.25,
and sets delay to one tick. Animate advances U each retail tick, wrapping U >=1
to zero; each U wrap subtracts .25 from V and wraps **V <=0 to 1**. Thus the first
four frames have V=0, then the repeating rows are 1,.75,.5,.25. This is not a
conventional modulo-V atlas loop. Render adds offsets to saved UVs, sets object
flag `0x2000`, and calls actual imagery RenderObject `0x40a8f0` once.

An independent execution of original Scene `0x417d60(2,1)` observed setter
`0x417060`: SRCALPHA=5, INVSRCALPHA=6, ZEnable=1, ZWrite=0, Cull=NONE, texture
blend=MODULATE. Numeric mode2 must not be inferred as an additive blend.

The native preflight executes seven factories, seven Init calls, 280 Animate
calls and 287 Render calls over 41 samples for every exact variant. Generic base
construction, object binding and the base animator tick are named fixture
boundaries; the custom functions execute unchanged. This preflight alone grants
no rendered comparison or full-effect acceptance.

A feasibility sample of the blue authored mesh fits fixed 512×512, camera origin,
retail Z distance1925 without adjustment. Native atlas samples produced changing
nonempty images. Natural runtime trigger, full illumination and modern device behavior remain
separate from the bounded compiled-function comparison below.

Run from the VFX worktree using the lab's Unicorn/Capstone environment:

```sh
PYTHONPATH=tools/retail_runtime:tools/retail_asm \
  /Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/warp_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output /Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/warp-preflight
```

Generated EXEs, decoded assets, images and reports remain local. The probe is
source, and uses a separate VM per invocation; it does not use DOSBox-X.

## Production comparison

`src/effects/warp_atlas.h` contains the shared per-owner quarter-step state and
exact ID/path mapping. `State::Step` follows native arithmetic; `Advance` supplies
a 24 Hz scripted clock. `ConfigureDraw` writes the UV offset and alpha helper
payload. The normal map renderer uses the same header; its owner admission,
Pulse timing and helper shader dispatch are checked separately from this probe.

The comparison compiles the actual header, the production generic authored-mesh
extraction and static matrix/submission methods. It retains all nine indices and
compares native mutable UVs to compiled `baseUV + ConfigureDraw.uv_offset` bits.
It checks Step, Advance(1/24) and two Advance(1/48) calls produce identical state,
Reset clears the clock, and unknown IDs have no asset path.

For raster comparison both packets use the unmodified original software
projection and triangle dispatcher. The viewport remains 512×512, camera at
origin, Z distance1925, white five-bit vertex color, SRCALPHA/INVSRCALPHA, depth
test and no depth writes. No size, UV, geometry or camera fitting is permitted.
The original material/illumination vertex producer is not reproduced by the
white-color fixture, and the native raster is shared rather than a modern GPU
pixel comparison.

Run with `--compare` to capture all seven variants at ticks0,1,3,4,7,8,12,16,20,
32,40 for identity and translated `(16,-8,4)` owners. Each of154 cases repeats
both native and compiled packets twice, rejects empty images, and checks equal
color/depth bytes. Warm image hashes must repeat, owner placement must move the
visible image, atlas frames must change, and tick4 must equal tick20. Freezing
UVs at tick1 is an independent negative control for every variant.

```sh
PYTHONPATH=tools/retail_runtime:tools/retail_asm \
  /Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/warp_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --compare \
  --output /Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/warp-final-ab
```

The fast regression `test_warp.py` runs the complete native family and compiled
header state/payload proof without generating154 raster pairs. Full software
comparison remains an explicit evidence run rather than a required long capture
on every source edit. No full-world, natural teleporter activation, or GPU/device
acceptance follows from the function fixture.

Bounded software evidence passed154 cases with two warm replays each:574 state
scalar checks and2,296 UV scalars match the original float32 bits. Every image
has at least5,423 nonblack pixels. The maximum vertex-coordinate error is
7.66e-6; color and depth bytes match exactly. Frozen-UV negative controls change
6,840–8,082 color bytes without changing depth. Actual normal-map admission and
helper shader behavior are separate runtime evidence.

`--no-images` retains native/compiled rendering and all pixel/depth checks but
skips PNG encoding for faster repeat verification. A comparison failure exits
nonzero and writes a failed manifest; unsupported native contracts raise rather
than silently applying fallback behavior.

## Normal map and helper backend verification

The map path admits only the exact type/asset pair with the audited STILL1,
gate/root keys, vertex/face fingerprints, white material and single256² ARGB4444
texture. Each `T3DAnimator` stores independent state and steps once per normal
24Hz Pulse; source shared geometry and texture stay immutable. Unsupported
profiles do not fall back to a static opaque gate.

`SHelperMeshSubmit` carries UV offsets through a dedicated vertex uniform on
Metal, GLSL and HLSL. The compiled emission regression checks the40-float
uniform layout, alpha path, nine retained indices and unchanged cached albedo.
Only the Metal backend was exercised by actual map captures here.

The first blue map capture exposed clamped negative V: it produced solid-blue
samples and only two distinct stationary images. The source mode2 sampler now
fetches wrapped nearest ARGB texels without normal-light modulation. Final
independent map runs pass all seven colors: ten typed observations and16frames
per owner, four changing stationary GPU samples, movement and deletion, and
exact clean-floor restoration. See
`recon/retail_asm/runtime/effects/warp-map-final-20261009/manifest.json`.
These are controlled synthetic map checks, not independent original lighting,
natural teleport activation/visibility or RGB565-versus-Metal pixel acceptance.

As a zero-offset/RGB565 helper control, all96decoded FireFlash pixel hashes
remain identical to the prior validated capture after the uniform/sampler
changes. Full runtime regression suite:99passing tests.
