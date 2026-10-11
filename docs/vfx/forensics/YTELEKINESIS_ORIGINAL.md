# Original YTelekinesis

Exact type0xaeaeeb28 uses Misc/chunk.i3d, four independently textured chunk
meshes. It binds animator factory509eb0/vtable5b1ab8: Init505f90,
Animate506240 and Render506510. The owner505f60 delegates to generic Effect
Pulse. This is distinct from the three-state JhagaAttack owner505840.

The bounded reference supplies a stationary global player position at origin,
a null spell and normal original Scene1 caller state. Original Init centers the
owner on that position, randomly chooses2–4 rocks, assigns initial radius480–640,
velocity6–12 toward the center, scale8.5, random rotation/angular velocity and
per-rock lifetime1000/speed. Animate accelerates linear/angular velocity by1.01,
marks an XY hit inside30 units, and kills the owner when every lifetime ends.
Dead rocks remain in Render until owner kill; the port must preserve this.
The null-spell branch marks hits without executing actor damage/reaction APIs.

Seed1 chooses four rocks and consumes45 RNG draws. A compiled production core
matches180 original ticks plus initialization:14,480 scalar fields, maximum
printed-float error4.532e-6, exact integer state/hit/lifetime/RNG. The original
owner kills at zero-based record165. Full original Render executes all four
normal-lit textured meshes; resource residency, base animator initialization
and map notification services remain declared fixture boundaries.

Private feasibility evidence is under RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/{ytelekinesis-first,ytelekinesis-core-first,ytelekinesis-raster-first}`.
This checkpoint grants no Metal appearance credit. Natural player movement,
spell damage/actors, device and exact framebuffer equality remain open.

The exact-profile adapter now registers four stable shared RGB565 meshes and
preserves their original normal-lit material streams. The original incoming
Scene1 call explicitly records ZENABLE1, ZWRITE1, CULL3, SRC2 and DST1. A new
default-off `SHelperMeshSubmit::retail_opaque` selects depth-writing opaque
pipelines while retaining the existing projection/light shader. Other helper
policies remain unchanged; the actual emission regression checks old defaults,
opaque selection, and alpha/additive negative controls. Build passes.

The full180-tick original regression passes (25.409s), including all four hit
flags, frozen expired rocks, and actual kill165. The24 sampled native Render
phases add1,344 matrix channels with maximum error3.477e-6. Full named owner
registration/factory and the actual Effect constructor also pass independently.
`YTelekinesis_ORIGINAL` has a real IsAlive callback. Its declared player center
is the spawn origin; no external actor is manufactured by the preview.

The1280x960 reference is a clipped diagnostic. Whole outward motion requires
multiple fixed supported1920x1080 windows. Unsupported4480x1920 falls back to
640x480 and was rejected by the capture collector. Tiled appearance review is
pending; no visual acceptance is claimed by this source checkpoint.


The completed capture fixture uses a regular3x2 grid of supported1920x1080
windows: X centers2700/1560/420 and Y centers960/420. These immutable windows
form a4200x1620 union with780px horizontal and540px vertical overlap. The
retained original packet trace verifies every one of39,600 complete triangles
at every active tick fits at least one window with2px margin. The compositor
uses fixed ownership cuts and retains all raw overlap images and disagreement
counts; it does not claim one exact giant framebuffer.

All twelve Metal films (six native-domain and six common diagnostics) render
at24Hz and save every eighth frame. The24 exact original record indices are
1,9,...185, checked from capture timing rather than inferred appearance. Native
depth is0..65535; common depth remains2238..3262. Actual Metal lighting reports
ambient38/255 on each channel, white directional1, and zero point lights.
The original fixture uses matching ambient38 and directional1. All six original
traces retain identical190-tick state. A fresh independent replay additionally
matches every state and selected native PNGs1/49/161/169 byte for byte.

Reproducible coverage and review tools are `ytelekinesis_grid_coverage.py` and
`ytelekinesis_visual_review.py`. Private final evidence is under
`hard-alpha/ytelekinesis-review-final`; fresh replay is under
`hard-alpha/ytelekinesis-native-repeat/repeat.json` (SHA256
`54586ab884c7926b382b7820e62d75c4d49e859f7667187d54de2dae31a9c4f6`).
Parent integration independently passed the native regression and confirmed six
existing helper/prelit GPU controls remained byte-identical with the default-off
opaque pipelines. Independent whole-appearance review remains pending.

Root independently reviewed whole contacts and actual-size chunk detail, verified477 frozen pair files, reproduced24 same-phase Metal PNGs byte-identically, and corrected the missing exact-ID review mapping. The translated gallery displays38/70 frames and restarts after real owner completion. Scoped appearance is approved; all stated natural/context/device/exact-pixel and raw overlap limits remain open.
