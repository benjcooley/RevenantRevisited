# Flies: authored dark sprite swarm

CLASS EFFECT Flies `0xa21d9d00` uses `Misc/Flies.I3D`, SHA256
`f878ea797286e80d4f90ebe5454be2d57f2ecd84f83a7af4da323c9d815c1797`.
Executed full original registration resolves builder `0x66ce40`, factory
`0x4f6790`, animator `0x5aa754`. Full constructor/base Initialize,
TParticleSystem initialization, all pool/RNG updates, owner matrix, whole
RenderObject/FVF_VERTEX and original software alpha raster execute.

The port now uses the exact old-body four XYZ/normal/UV vertices, eight-by-eight
ARGB4444 texture and `(2,0,3,1,3,0)` triangles. It preserves the original
particle rotation, fixed X tip/Z spin, scale0.3 and raw relative Z before
the owner matrix; no screen-aligned billboard or guessed ten-unit footprint.
The original plane projects to roughly4.55×2.28 pixels. Normal source
rendering ignores light for this ARGB texture. Source alpha lookup and
existing software projector are used only through existing APIs; color,
brightness, scale and projection were not fitted.

Simulation now uses exact24Hz rather than truncated41ms gates. Dormant
particles keep moving and consume their original RNG draws; only one may
activate per tick. A real exact typed map builder/component installs this
renderer and replaces the default static mesh. Runtime owners retain their
common world/camera; native-domain projection is an isolated preview option.

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-map-controller-20261010/`

- `native-flies-full-v2/manifest.json`:91 complete pool/RNG states and six
  full image/depth/draw sets agree across two fresh VM replays. Native
  ticks0–2 are genuine empty controls. Active samples6,16,30,46,60,90
  draw4,9,20,20,20,20 particles,47–217 changed pixels; all geometry fits
  the declared1280×960 viewport.
- `flies-state-compare-v2/manifest.json`:18,382 actual compiled production
  state/RNG/draw-count checks against native, zero errors underASan/UBSan;
  all9floats/used values bit-exact. Render consumes no RNG; lazy retry and
  idempotent initialization plus6/24/30/60/144fps clocks pass.
- `flies-profile-contract/manifest.json`: actual exact path/header/root,
  all32vertex channels/6indices/facebins/17material floats/8×8texture
  descriptor compiled underASan/UBSan;3positive paths and83 independent
  mutation rejections. Old keys are unused by customOBJ3D_MATRIX rendering.
- `metal-flies-final-v3/manifest.json`:90 actual Metal frames/90distinct,
  clean exit, original seed1/24Hz/gray backdrop. Captures5,15,29,45,59,89
  pair with native tick=capture+1 for the existing late quad submission.
  Executed before-render RNG checkpoints establish the declared phase.
- `map-final/Flies/manifest.json`:48 same-binary real map frames, ten
  create/frame/MOVE/position/delete/absence rows pass, visible evolution
  and exact restored synthetic floor.
- `review-final-v2/manifest.json`:six unchanged whole/fixed-center pairs,
  every geometry/effect bound unclipped; first empty Metal capture retained.
  Fixed center `(384,288,896,672)` has no rescaling and is supplementary.
- `Flies-sampling-residual.json`:root observed sparse sampling/color
  residual, quantified on unchanged samples. Bounding deltas are0/1pixel;
  native47–217 changed pixels versusMetal38–219. Native affected mean
  RGB≈106/103/108 versusMetal≈97–103/95–101/98–105, with sparse warmer
  pixels. Declared gray backdrop blue123(native565) versus128(MetalRGBA8)
  is retained. No correction or brightness fit was applied.

Root independently approved scoped swarm position/motion/current overall
appearance across allsix paired phases, retaining that residual explicitly.
Tiny dark authored sprites have limited visibility on the black viewer
backdrop; no color change was made to force visibility. Natural corpse
context, native hardware, exact color/pixel equality and full acceptance
remain open. Licensed geometry and native rendering are never replaced by
host-generated shapes/poses/colors. Readonly old-body resources, one-frame
COM residency/material handles and nullspell owner command methods are
declared upstream API boundaries. Earlier completed films forcibly stopped
by the old two-second tool grace remain failed evidence; finalv3 uses root's
unchanged declared-budget clean-exit fix.
