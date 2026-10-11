# Genuine loaded Drip lifecycle

`tools/retail_runtime/drip_loaded_lifecycle_probe.py` reuses the source-matched
retail startup cache, original PlayScreen prefix and actual class factory.
The real Drip owner has vtable `0x5ac630`, its public SetParams receives
`(20,128,1)`, and original CreateAnimator loads the resource and produces
animator vtable `0x5ac83c`. Original NextFrame and custom animator Pulse execute
for96 ticks. No owner, resource, animation or child-request handler is replaced.

The actual `0x4f11e0` method occupies Pulse slot `0x2c`; default Animate is
`0x40e460` in slot `0x30`. The earlier source-snapshot Animate label is corrected
here. This state probe calls Pulse once directly; full owner Pulse already
invokes it, so a full-render scheduler must not call both. Owner bookkeeping
and default Animate/transform are outside this state-only probe.

The actual AddNewRipple requests occur at24,48,72,96 with `(0,0,0,20)`.
Read-only entry/return hooks show all four original map-factory calls return
`0xffffffff`, because this cached startup fixture has no loaded map sectors.
The parent lifecycle therefore works through genuine resource construction,
but no child residency or complete composite appearance is proved. Do not
count the missing children as a VFX pass or replace their state with a request
counter. A declared standalone child-residency boundary or a loaded sector is
the next step, followed by whole original/Metal rendering.

The local reports are
`/Users/benjamincooley/RevenantRetailLab/research/vfx-drip-loaded-20261011/first/manifest.json`
and `map-observed/manifest.json` in the same parent directory. Both retain
`accepted=false`; Drip remains among the29 unreviewed rows. The audio call runs
on the original path, but this diagnostic does not claim audible output or
audio-device accuracy.

Run the probe with the retail laboratory Python environment and
`--boot-image PATH --output NEW_DIRECTORY`. The executable/assets/cache stay
local, outside the source commit. The cache filename must match current boot
sources and the rebuilt original executable.

Full loaded renderer and explicit child schedule
----------------------------------------------

The vtables correct the earlier terminology: Drip4f11e0 and Ripple4f08b0 are
animator Pulse overrides at slot0x2c; Animate at0x30 is40e460. Whole Effect
Pulse4de800 invokes the override already. The full fixture therefore performs
NextFrame, whole owner Pulse once, Animate, original BeginScene/PreRender,
custom Render/PostRender and EndScene. A duplicated-Pulse diagnostic produced
incorrect12-tick births and is not accepted evidence.

The empty-map allocation failure remains original and observed. After each
actual request, a declared managed scheduler separately executes the genuine
Ripple class NewObject (owner5ac3a4), public SetLength slot0x204, and actual
CreateAnimator (5ac5b0). Existing children advance before their parent; a newborn
renders at age0, then receives its first owner Pulse on the next tick. This
is not a claim of natural sector insertion, map iteration or residency.

The144-tick fixture uses SetParams(20,128,1), then changes only the public period
to1000000 before tick121 to observe the drain. Births occur24/48/72/96/120;
real child kills45/69/93/117/141. Length20 has no splash recursion. Actual loaded
droplet and ripple vertices, normals/prelit carriers and COM textures enter the
original software renderer. Camera0, source0, ambient38/dir1 and1280×960 RAM
center(640,640) are declared; no fabricated white vertex input is used.

The compiled existing production composite matches1,825 fields over all144
records, including exact active float32 motion, child counts/ages, births and
deaths. Render leaves simulation/CRT state unchanged. Original audio selection
consumes RNG while the silent port omits it; period1 physics consumes no random
draw, and this equivalence does not extend to arbitrary periods or splash
lengths. Actual Metal appearance remains pending.

Private artifacts under `hard-alpha/`:
- `drip-loaded-corrected/manifest.json`, SHA256
  `cf13ce752245244c842ace0e14dc0c7e0993de4b95a61f39de334057bf0f805c`.
- `drip-source-state/manifest.json`, SHA256
  `160ca075d695ad3ee4f5772f56468639ae33751be4ec0409586ffd201e177bd3`.
- `drip-loaded-corrected/source-comparison.json`, the1,825-field comparator.

A fresh144-tick loaded replay matches every before/after state, submitted bounds,
birth/death/request record and original failed map result. Seven common actual
software PNGs (7/25/49/73/97/121/141) are byte-identical.
Replay manifest SHA256: `4fa5ad9a4742008164026961a621fd61a94f9ee103bf36b2f5ff16d4f046e36c`.

Actual Metal comparison and scoped water submission
-------------------------------------------------

The controlled `TDripEffect_LINKED_NO_SPLASH` preview retains the public
SetParams(20,128,1) continuous parent and managed children. The separate
`TDripEffect_LINKED_DRAIN` fixture changes the public period before121 and
retains the real ambient-owner liveness; no lifetime timer is invented.
Both use the existing real production Drip/Ripple classes. Their optional
native-domain flag selects the proven original owner/projector. Strict16-bit
masks R0x0f00/G0x00f0/B0x000f/A0xf000 additionally select the existing software
ARGB4444 quad policy. Ordinary paths and unknown/1555 formats retain defaults.
No renderer/shader, color, geometry or controller timing was changed.

Actual loaded submitted geometry additionally matches900 channels: all original
triangle indices and float32 UVs agree, world positions differ at most
2.9988321e-6 from compiled production geometry. Native draw states are
CULL_NONE/source-alpha/ZWRITE_OFF. Drip uses actual FVF112 and Ripple actual
LVERTEX0x1e2; neither reference substitutes a white carrier or texture.

The36-frame actual Metal film samples ticks1,5,...141 while still advancing
and rendering at24Hz. Fourteen native/Metal common ticks are selected by their
recorded simulation times, with no phase adjustment. The full1280×960 images
and identical fixed4x detail crops retain the droplet, ring growth, five cycles,
final drain and blank141. Original and corrected Metal bounds differ by about
one pixel. Low-intensity native green versus Metal gray, sample coverage and
RGB565 framebuffer quantization remain explicit residuals; exact pixels,
software/device equivalence, natural map residency, arbitrary randomized
periods and recursive-splash appearance are not claimed.

The earlier common-policy36-frame film remains the diagnostic control. All36
same-binary common frames after the change are decoded-RGB identical to that
baseline. The actual production policy extraction test passes ASan/UBSan with
strict4444 positive, five format-field mutations,1555 rejection, explicit native
flags, and ordinary/default world/flag controls. Existing linked composite
4tests and recursive Ripple4tests also pass. The private application build passes.

Review artifacts under `hard-alpha/drip-review-final/`:
- `pair-manifest.json`, SHA256
  `89264ffbcbff8842b57f1b016dd10f484434dda0cc3f4ce50297e0d08bdc2d59`.
- `whole-life.png`, `detail-1.png` through `detail-4.png`, and14 raw whole pairs.
- Frozen binary SHA256
  `b21b2d8613b3c0067e56034845e098ffe032e3365c12fc24596c9f25d6eaceb6`.

The manifest pins the original/Metal manifests, fresh replay, state/geometry
comparison and common-image regression. Independent root appearance review and
integrated repeat/gallery remain separate; this source checkpoint does not
record approval or grant a natural-map claim.

## Independent scoped approval

Root verified42 original/Metal/pair image hashes and independently inspected
all14 phases in four fixed-crop detail pages. The integrated executable repeats
all36 sampled PNGs byte-identically through tick141, retains24Hz simulation and
rendering, and exits cleanly. Linked composite4tests, recursive Ripple4tests,
the sanitized water-policy test,19 mapping tests and the compiled preview
bridge with four invalid Drip-route controls all pass.

The actual current viewer selected the normal map factory with default
`(64,128,48)` parameters. Its100 label-free samples include66 visible frames
over397 ticks and clean shutdown. This is supporting default-map liveness,
not a claim that the configured no-splash gallery route was selected. Viewer
route preference remains a separate follow-up. The parent retains its true
ambient lifetime; only the controlled drain fixture changes its public period.

Approval is pinned at
`recon/retail_asm/runtime/effects/current-drip-loaded-20261011/root-approval.json`,
SHA256 `0d776d56dbd85d7dba27cf581fc196e72d66bffac4b00ba2e30b8aa01b8a0956`.
Root captures remain outside Git under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-drip-root-20261011/`.
The reviewed total advances to149/176, with27 remaining. The recorded hue,
edge, audio/RNG, natural sector, arbitrary period and splash limits remain open.
