# FireCone relative translation and original normal-lit software pixels

The production leaf now translates by raw `particle.pos`, matching original
`50c374..50c3bc`. FireCone passes `abs_pos=false`; only the other branch performs
FIX conversion. The old code incorrectly treated relative particle Z as FIXed
and canceled the owner Z stretch for translation alone. Authored vertices and
particle translation must enter the same owner matrix in the same local domain.
No particle state, RNG, scale, rotation, texture, camera, or light was tuned.

The new probe executes native owner position/FIX/facing `40e470..40e546`, owner
matrix `40e740..40e83d`, real FireCone/shared Render, relative RenderObject
composition `40aad5..40ab92`, Scene/software SetTransform, then the complete
original `56eb30` D3DVERTEX transform/normal illumination/raster path. The
compiled actual SubmitPool receives the executed native owner matrix as an
explicit shared boundary. It uses independently verified production particle
states and exact authored XYZ, normals, UVs, indices, and RGB565 textures.

44 cases span four facings and ticks 0,1,2,6,15,20,33,60,92,93,100. **30 nonempty
pairs have zero differing RGB565 or depth pixels**; the empty onset/drain and
edge-facing cases are retained without visible credit. Every original image and
depth buffer repeats identically. All 73,806 meaningful simulation fields remain
exact. Maximum paired matrix residual is 0.0009891 at world coordinates near
10,000; this includes float matrix arithmetic and decimal trace serialization.
The original culler executes; production pre-cull omissions receive no assumed
draw credit. The native normal-lit vertex colors are `(21,21,21,31)`, not white.

Scene inputs are explicit: owner `(10000,10000,116)` after Initialize, camera
`(10000,10000,0)`, source mode8, CCW culling, depth test/no write, ambient RGB38,
directional RGB1 and direction `(0,-0.78125,-0.625)`. The lighting descriptors
correspond to ambient32/white/Ambient3D130/DirLight85; complete scene light
selection is not executed. The original software Illuminate ignores material
diffuse/emissive and consumes authored normals plus scene lights. The reusable
`lit_software_probe.py` supplies descriptors, never replacement vertex colors.

The old translation is restored only in a compiled negative-control fixture;
it produces both matrix and visible pixel differences at ticks1 and20. A second
control flips an authored normal: original illumination changes RGB5 from23 to4
under the same light. Focused state/raw-render/owner regressions cover eight
tests, including the existing wrong-degree conversion control.

This closes the local translation defect and extends the bounded frontend proof
through original owner composition, normal lighting, culling and pixels. It
does **not** establish actual map/Metal appearance or full FireCone acceptance.
The production game's common-world owner conversion and projector remain a
different boundary: native software unit-axis projection is approximately
X/Y ±1.0101526 and vertical Z -1.2371792, whereas the common projector uses
X/Y ±1 and vertical Z -0.867 before the owner's1.5 authored Z stretch. No fitted
scale, inverse-Z adapter, or per-effect compensation erases that residual.
Live spell/caster, sound, point lights and DragonFire are separate.

Reproduce with the pinned retail executable and thin-runtime Python:

```sh
python tools/retail_runtime/firecone_owner_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <new-private-directory>
python -m unittest discover -s tools/retail_runtime -p 'test_firecone*.py'
```

Retained report:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/firecone-owner-lit/manifest.json`,
SHA256 `b1412f7272923fa2bc88994f5ee0f36d51f91b1e0cbbe1e230fc1ac666482a68`.
The older raw-stage diagnostic remains historical evidence; its false-FIX
translation comment and absence of executed owner/lighting are superseded here.
