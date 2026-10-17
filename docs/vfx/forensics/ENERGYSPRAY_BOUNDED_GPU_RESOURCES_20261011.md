# EnergySpray: bounded GPU resources without visual changes

The full 176-effect gallery exhausted Sokol's buffer pool because EnergySpray
registered and globally cached an immutable triangle for each changing packed
color. The per-owner cache could retain thousands of meshes; releasing its refs
did not erase the renderer-lifetime keyed cache. The recorded failure reached
4,086 cached meshes and failed keys beginning `0x454e5452`. Increasing the pool
would only postpone the failure.

Source `d5e6e94b` replaces this with exactly three fixed Energy meshes: the two
existing authored cards and one immutable authored triangle. The triangle keeps
its exact positions, indices and order. Its UV.y stores weights `(1,0,0)`;
an explicit default-false helper option supplies the draw's packed 20-bit
A5:R5:G5:B5 value. All20 bits are exactly representable as a float. The vertex
shader reconstructs precisely the old vertex0 RGB5/A5 and zero values at the
other vertices before the existing interpolation/fragment path. The existing
unused `retail_raster.zw` fields carry color and flag, so the 44-float vertex
uniform ABI does not grow. Ordinary material, untextured direct-vertex prelit,
source texture, blend, depth, triangle admission and draw order are unchanged.
No dynamic mesh uploads, replacement geometry, pooling increase or renderer
cache eviction policy is introduced.

The compiled actual three backend vertex-decode blocks match the old vertex
varyings for all 1,048,576 colors and both 0/1 weights. Actual helper submission
checks prove default-zero uniforms and admission only with explicit prelit
format/flag, including modern scene lighting. Existing helper UV/depth/alpha
controls and the Energy owner/lifecycle negative control pass.

Actual GPU regression (`energy_resource_pixels.py`) freezes 286 same-index PNG
matches: native 140 against a fresh old-policy baseline, common 140 against the
approved final-v2 movie, three existing prelit diagnostic frames and three
unrelated default Shriek frames. No temporal alignment is applied. Two stale
prior-frame captures in the old approved native film and one in the fresh
common baseline are preserved as separate historical exceptions; the complete
matching reference for each current 140-frame film is explicitly identified.
The isolated Energy resource log reports three cached meshes, 352 vertex bytes
and 30 index bytes. Original source/raster proofs and all previous visual limits
remain unchanged.

Private evidence is under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:
`energy-resource-{common,domain,literal,default}-{before,after}/`,
`energy-resource-pixels.json`, and `energy-resource-full 176/`.
The Energy-only regression binary is
`e2e4477d8628a7e56b1f0c6d60cd9159d42bac5c565eba349211d527721d2d0f`.
The preserved old-policy binary is
`5fec4b033d546e5bd47d7aef6f7962add76a9b0a8f8ad926c571296f9005761f`.

The first full 176 run completed 630 simulated seconds without exhaustion, but
its resource audit deliberately remained open: the second/third-loop Energy
samples grew from 630 to 672 meshes. Successful-allocation tracing then proved
all 42 additional meshes came from the unchanged authored-static preview
factory, which registered unkeyed buffers on every owner creation. The exact
set was 12 signs, Globe, StillWater, 14 watcher texture frames and 14 colored
ribbon parts; their 60,608 vertex bytes and 10,632 index bytes exactly explain
the measured growth. This was repeated allocation, not lazy animation warmup.
The traced pre-fix manifest is explicitly marked failed for global stability;
Energy's three-mesh fix and its independent pixel proof remain valid.

Source `2c3a690d` gives that factory a separate key domain containing source
asset UID, subobject, texture slot and texture frame, with matching owner refs.
Followup `c08897fd` keys a non-animated caller by the actual already-selected
imagery texture frame; its extra compiled control selects frame1 and reuses
the exact matching animated-frame buffer/albedo.
Vertex/index bytes, texture-frame selection, transforms and submission order
are unchanged. It does not change generic unkeyed registration, mutable mesh
semantics or renderer cache eviction. Optional `REVENANT_TRACE_MESH_ASSETS`
logging occurs only after successful new allocation and is off by default.
The compiled actual factory/destructor test covers 100 simultaneous static and
animated owner pairs, immutable buffer/albedo equality, distinct source assets,
balanced refs and partial-allocation failure cleanup.

`energy_resource_gallery.py` performs the full 176 replay at 24Hz and 96px/s,
with 22 sparse screenshots across 630 simulated seconds (loop 262.698 seconds).
It fails if any new mesh is allocated after the first complete loop, if later
resource totals differ, or if optional same-index reference screenshots differ.
It writes failed audits as evidence instead of treating a clean exit or stable
counts within one short Energy visit as proof of global stability.

The final full-gallery resource run passes:626 meshes loaded during the first
loop, zero allocations throughout the second loop and the observed third-loop
segment. All repeated Energy samples then remain626 meshes,387,584 vertex
bytes and63,282 index bytes. The actual630-second run exits cleanly without
pool failures. Its binary is
`feef921446480bc0310da54a0019a23b4d9d1a0820bd40886ef11564b05d49e5`.
The full-gallery report keeps resource checks separate from screenshot identity:
21/22 same-index PNGs match the traced old static-factory run; sample11 is
retained as a nonreproducible capture difference: an independent repeat of the
same binary/inputs through sample11 matches all11 original same-index PNGs.
Only the movie endpoint was shortened; no phase alignment was applied.

Final selected-frame-aware source run (`energy-resource-full176-final2`) passes
all resource and pixel checks:22/22 same-index screenshots are byte-identical
to the traced pre-static-cache reference, with zero allocations after the
first-loop626 meshes. It renders15,122 real frames across630.083 simulated
seconds, exits cleanly, and keeps387,584 vertex bytes/63,282 index bytes.
No stale sample is substituted into this final film.

Frozen closure record: `energy-resource-closure.json`, SHA256
`5a2d3f2f5838ddd48acbfbd9251c037e5c5fdf12da2f2eb0f3e499585bab745e`.
Final gallery manifest SHA256:
`06e00de6d9f05f9ef0e4b54e3aac18dd580d02a558bed489ce1c87024daad64e`.
Final binary SHA256:
`b9c09f6ad28ccdca9f624f6488008ecb09e8a11ffd73998cf023a7925119245b`.
This grants no additional effect appearance credit; it restores bounded
resources while retaining the independently approved visual configurations.

Root repeated the complete audit on the integrated127-pass build, including
both new Cataclysm adapters. All176 entries completed22 screenshots and entered
loop3 with clean exit. Initial warmup allocated636 meshes; loops2/3 allocated
none and retained404,352 vertex bytes/68,106 index bytes. Every resource check
passes, with no pool failure. Private evidence:
`research/vfx-review-survey-20261011/127-full-gallery-root/film/manifest.json`.
Its additional ten meshes belong to the two newly integrated five-mesh effects;
the difference from the earlier626-mesh catalogue is expected.
