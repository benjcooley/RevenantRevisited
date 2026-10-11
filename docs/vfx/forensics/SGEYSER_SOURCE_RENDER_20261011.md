# Sgeyser: genuine loaded inline controller and authored burst

Exact EFFECT `sgeyser` `0xad92bd30`, `Cave/CavSGeyser.I3D`, now has
`SGeyser_LOADED_INLINE` and an independent standalone source adapter. It loads
its own three-object,45-frame resource, verifies literal keys/tags/texture
format, and retains both the104-slot `#steam` controller and authored `#blast`.
The original installed `Resources/vapor.txt` open fails. Only that observed
failed second controller is omitted; no vapor preset is fabricated. The old
unsupported geyser shell remains unchanged. No shared parser, controller,
resource admission, renderer or shader policy changes are made.

The reference loads the real class registry, resource pack, constructor,
CreateAnimator and complete default Init/Pulse/Animate/PreRender/Render/Post
in the source-matched HudWorld startup VM. It uses declared saved placement
(1019,11935,289), face0, quality0, post-initialization seed1 and camera
(1019,11935,0). Actual original BeginScene/EndScene run; the RAM target is
rebound after PreRender's real display binding. COM GetLightState1 is the
explicit material-handle interface; all other device behavior delegates to
the loaded implementation. No actor, terrain query or particle handler is
replaced in the reference.

Initialization retains an untouched zero emitter matrix/scale and previous
origin equal to the owner position. First Pulse evaluates PPS at cached
frame0 before Animate establishes the emitter pose. Subsequent Pulse uses
the previous Animate frame. All1,745 original walk-height queries return0
at their actual XYZ. Source uses that declared empty-ground result and logs
its independently computed query coordinates. The original null-spell owner
finishes at tick45/frame44; its final controller Pulse executes before kill,
and later updates/rendering stop. Gallery repeats use this real completion,
not a guessed period. Factories consume the live stream and never reseed it.

The two camera-facing prototype/burst objects and ordinary emitter retain
original matrix order. Whole loaded PreRender supplies previous state0/frame0;
`409950` interpolates the initial five frames, using x87 intermediates. A
separate pure leaf helper reproduces this without altering default decoding.
All135 object/frame poses pass:1,215 key and2,160 matrix channels. Default vs
explicit-false decoder controls remain identical. Both native and Metal
retain the actual startup, rise, rotating steam, burst, drain and blank tail.

The original controller emits FVF1e2 RGB565 additive packets, followed by the
FVF112 normal-lit burst. Scene mode starts0 (no explicit override); the first
particle selects16 and that mode remains for the burst. The tested black
background preserves that initial no-overlap case; arbitrary incoming scene
state remains outside this fixture. Existing integer RGB565 additive quads
and normal-lit helpers render the source; no fitted tint, scale or position
is introduced. Common projection remains a separate recorded diagnostic.

Validation includes243,672 compiled production particle fields over120ticks,
all RNG seeds, and negative controls for advanced cached frames and skipped
Render RNG. Both negatives fail. Actual compiled engine output independently
matches all1,745 live particle positions/velocities, every ground XYZ query,
and all45 owner-frame/RNG phases (10,560 checks, maximum4.85e-7). The observer
retains all120 original pools, seeds and ordered packets byte-identically to
the root positive reference;13 shared PNG hashes agree. The instrumentation
change preserves all51 preceding actual Metal PNGs byte-for-byte.

The source mapping admits only the exact new factory with Static style,
world callback and real liveness. The old null alias is not a fallback.
Bridge positive/four rejection controls and generator check pass. Actual
translated gallery runs746 full24Hz ticks:16 complete45-tick cycles, then
active teardown at26,28/32 sampled frames visible, clean exit and no reseed.
Only one fixed authored mesh key is allocated and released per context.

Private evidence is under `research/vfx-wide-20261010/hard-astra/sgeyser-*`.
The initial center(640,480) comparison retains its clipped transparent outer
corners as a diagnostic: submitted geometry spans approximately x583..697,
y-70..249. Final fixed center(640,640), same1280x960 viewport, includes every
submitted corner without changing world geometry, camera world coordinates,
scale, simulation or RNG. Complete final pairs and hashes are frozen by
`sgeyser_review_freeze.py`; independent root review/approval is separate.
Natural CavSGeyser activation, map sector/script residency, the1433 layered
placements, populated terrain, sound, hardware/device and exact framebuffer
pixels remain open.

Final review package: `sgeyser-review-frozen/manifest.json`, SHA256
`e267dea29d196463c8843548f1f97e5261fd88c02888a525259ef1bdcb2a3a9e`.
Captured binary SHA256 `f4068ab200f10adfd63b59e4a55332169b3c790bd92553b8e6893b5da4dd1f6a`.
It pins15 whole phase pairs and all source/runtime/pose/gallery controls.
The fixed-center change preserves all120 original pools, ordered packets,
ground coordinates and RNG states exactly. Sampling/edges and RGB565
framebuffer precision remain differences, not a claim of exact pixels.
