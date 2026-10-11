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
