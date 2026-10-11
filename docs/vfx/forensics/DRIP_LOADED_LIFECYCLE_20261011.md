# Genuine loaded Drip lifecycle

`tools/retail_runtime/drip_loaded_lifecycle_probe.py` reuses the source-matched
retail startup cache, original PlayScreen prefix and actual class factory.
The real Drip owner has vtable `0x5ac630`, its public SetParams receives
`(20,128,1)`, and original CreateAnimator loads the resource and produces
animator vtable `0x5ac83c`. Original NextFrame and full custom Animate execute
for96 ticks. No owner, resource, animation or child-request handler is replaced.

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
