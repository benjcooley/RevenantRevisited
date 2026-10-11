# Blast authored animation; appearance credit held

`Blast` / `0x0ea34fa3` / `Magic\blast.I3D` uses the actual default animator
builder `5e8508`, factory `40dc00`, vtable `5a370c`. Its shipped animation is
56 root objects, 120 frames, 1,208 vertices, 676 triangles, three one-frame
RGB565 textures and eight `#` camera-facing parts. It is not the expanding
billboard previously registered as `TBlastEffect_BESPOKE`.

The registered preview now renders those authored meshes and keys. Its owned
generic effect runs actual `NextFrame`/`Pulse` at 24 Hz; world submission does
not advance it again. Natural completion supplies the gallery liveness hook.
The exact resource structure, texture formats and two tags gate this route.
Original `RefreshControllers` executes `scrolltex` on sparks42 and
`blendcont(litaddz)`: the latter sets mode80 immediately on state entry despite
its frame20 tag. Source rendering retains that additive/no-depth policy and
the actual fixed camera orientation for `#` parts. No renderer code changed.

Original `4096bc` uses an inclusive compressed-key run boundary. The new
optional `GetUninterpolatedAniKey(..., retail_inclusive)` argument selects it
for this audited caller. Default behavior and existing profile choices remain
unchanged. ASan/UBSan executes the actual source decoder and math: all 56×120
poses match original `409430`/`40a420`, with 60,480 key fields (maximum error
5e-7) and 25,680 active matrix fields (maximum error 2.287e-5). The default
argument equals explicit `false` for all 6,720 poses; correct inclusive decoding
changes 1,439. Actual Metal logs independently match 480 matrix channels and
draw order at ticks16/60. This fixes the observed one-frame scale/rotation and
active-part errors without adjusting time or geometry.

The native reference executes the complete default `Render40e8d0`, controller
Render, `RenderObject40a8f0`, authored normal lighting and original RGB565
software raster. Full registration constructors and controller parser/Init run.
Decoded immutable resources, three exact COM texture surfaces, material handles
and one-frame residency are explicit fixture boundaries. Source ambient38,
directional1, origin/camera0, facing0, 640×480 are declared inputs. Ten native
sample phases each replay identically twice. Mode1 restores native incoming
state before every sample: mode0 is a no-op and must not be followed by manual
depth/cull resets while the native mode80 cache remains set. Earlier diagnostic
references are superseded by `blast-native-final`.

Both common-world and opt-in native-domain Metal films contain 120 frames,
118 nonblack and 119 distinct, including the blank tail. The actual-owner
clock produces identical image hashes to the initial integer-clock diagnostic.
The native integer clock is an explicit input; original outer owner lifecycle,
natural spell/lighting/audio context and gallery integration remain separate.

Appearance credit is **held**. At tick60 the actual textured software raster
rejects both triangles of outer shock8 and four triangles each from flare
objects2/3/4/28 because of its fixed 640×480 edge limit. Inner shock9 survives;
the native depth buffer is untouched. Metal clips and renders this correct
full geometry, leaving a visible extra outer ring. No scale, vertices or ring
were removed to imitate that renderer limit. The gallery entry remains partial
with an updated explanation. Root requested integration and a separate actual
gallery capture; this record grants no primary appearance pass.

Frozen private evidence under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:

- `blast-native-final/manifest.json`: `969f29d5cee7ef41725d4056c2f82109608fcf99c79fbb0d2fd7a1a6a8562cf1`
- `blast-poses-final/manifest.json`: `5467643ddd4c78a4116358970bda58366c46f8682bc01b2cab530018aeb037c3`
- `blast-metal-final/manifest.json`: `02fc05589d75de35d66b43d1c22933cebdd3931062ca00118d12a376d039c677`
- `blast-metal-common/manifest.json`: `99fb972c062b5f6f211677a5e934e5b2e8efc6b848da9e1899ba6e55cd206752`
- `blast-review-final/manifest.json`: `3b5a97fa994d9b9096c8dfb3a06ca69feea715b7dab82b5e09a7a69ba76d061e`
- Capture binary: `30d01bf2e43fb1b19b0c10b4e3aca8c68ba261015d911b8cde498548ffbdb634`
- Asset: `9b796f41bd9113be6bf0d2d8ec9b230227296dad1fa09ed612ec5fc46588532d`
- Original EXE: `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`

Validation: full private build; `test_blast.py` (two tests); Speed profile,
Quicksilver profile and water profile regressions; two clean actual Metal
captures; generated preview-map check. Only source/tools/this record are
committed; imagery, binaries and generated reference artifacts remain private.

## Native primitive-span correction (2026-10-11)

The native-domain/software adapter now applies the proven original640/480
edge-span admission per authored triangle, retaining face order and all ordinary
geometry. At phase60, changing only the original raster constants to6400/4800
restores18 rejected triangles and increases visible pixels from231,887 to303,854.
Owner, flags, complete draw packets, source vertices, transformed colors, UVs
and matrices remain identical. The widened diagnostic needs a400-million
instruction budget; the failed100-million attempt remains private evidence.

Both fresh120-frame Metal captures exit cleanly. All120 ordinary-frame RGB
images are byte-identical to the pre-change capture. The eight640x480
original/native-domain phase pairs now agree on the previously extra outer ring.
Both existing Blast runtime tests pass:60,480 key and25,680 matrix fields, plus
full original warm-replay controls.

Appearance credit remains held pending whole-geometry supported-view coverage.
A full original1920x1080 phase60 replay succeeds twice with a400-million budget,
but the native visible bounds still touch the top edge. Submitted geometry spans
x[36.40,1902.36],y[-673.48,1624.28]. Fixed vertically overlapping views can cover
that extent without changing world geometry, lighting or the original span
limits. This checkpoint does not silently accept the clipped contact as complete.

Evidence root: `research/vfx-next-blast-20261011`. `native-bounds-budget400m`
records the constants-only differential; `common-before`,
`metal-common-bounded`, `metal-native-bounded` and `review-first` retain the
fresh captures and phase checks. `native-wide-budget400m` records the expanded
original replay; failed attempts remain separate. Natural callers, other
lighting/device behavior and precise framebuffer equality remain open.
