# Scrolling VFX review

Run `Revenant --test=vfx-review` from the game installation/source directory.
The mode creates a disposable dungeon-floor map and places the loaded EFFECT
catalogue in name/type-ID order with per-effect clearance. It never saves this map.

The camera follows the row toward the upper right, so effects scroll toward
the lower left. The default slope is 2.5 horizontal units per vertical unit.
The base clearance is 420 logical screen pixels. Broad effects, storms and
projectiles receive 720–1000 pixels by default; scrolling runs at 16 pixels/sec.
There are 720 pixels of empty lead-in and tail before the view loops.
Effects stay in view for roughly 40 seconds at the default viewport. A full
176-entry pass takes over an hour; select a smaller group while fixing it.

```sh
./build-merge/Revenant --test=vfx-review \
  --vfx-review-effects=Flame,FireBall,CyanFont,SymGlow \
  --vfx-review-offset=720
```

The offset starts at the first station, skipping the initial empty lead-in.
Omit the filter and offset to review the entire catalogue from the beginning.
Filter entries accept case-insensitive full names or hexadecimal retail type
IDs, separated by commas. Duplicate names retain separate type-ID entries.
Unknown entries are rejected so typos cannot silently change the review.

Options:

- `--vfx-review-speed=16`: logical pixels/sec, range 0.1–240.
- `--vfx-review-ratio=2.5`: horizontal/vertical slope, range 2–3.
- `--vfx-review-spacing=420`: base clearance, range 180–800.
- `--vfx-review-spacing-overrides=FireCone:1200,FireBall:900`: per-effect
  clearance in pixels, range 180–2400; names or hexadecimal type IDs.
- `--vfx-review-path-tilt=0.25`: upward path with a slight rightward tilt,
  independent of the scroll; 0 is straight up, negative values tilt left.
  For an exact perpendicular to a 2.5:1 scroll, use `-0.4`.
- `--vfx-review-path-length=200`: source/target separation in logical pixels,
  range 80–640.
- `--vfx-review-gap=720`: empty distance at both ends, range 640–1600.
- `--vfx-review-offset=0`: initial distance into the scrolling route.
- `--vfx-review-effects=Flame,0x63fd382a`: restrict the catalogue.
- `--scene-ambient=32,255,255,255`: override scene lighting.
- `--vfx-lighting-mode=classic|modern`: use the existing lighting diagnostic.

Space pauses **scrolling** while effects continue cycling. Left/Right jump to
the previous/next station. Home returns to the lead-in. Escape exits.

Labels below each station show its name, retail type ID, asset, cycle and
runtime support, plus its spacing. Adjacent stations use the larger of their
two clearances; the first/last clearance also increases end padding as needed.
Spacing changes layout only, never effect geometry or physics.
Effects are made with the normal map factory, advanced at
24 Hz, and drawn by the normal map renderer. A completed one-shot is recreated
after cleanup; a looping effect continues its authored animation. The mode
does not shorten a live effect to force a retrigger.

Endpoint-enabled effects launch mostly vertically from a lower-left source
toward an upper-right target, independently of the review row. The labels identify entries still
missing endpoint/runtime support. A target marker is an aim point; normal
projectile physics and collision determine where the projectile ends.

Authored animation sound tags use the normal sound system and the listener
tracks the camera. Run visibly for audio: the existing headless mode silences
output. Spell-specific audio/caster branches still need their actual spell
context; this mode does not guess replacement sounds.

This is a port review screen, not a retail parity result. The thin retail
emulator remains the reference for comparisons. The screen uses the ordinary
map projection, not the isolated `--vfx-native-domain` diagnostic.

## Verification

The Metal build and retained schedule, factory/endpoint ASan/UBSan, and
transient-map persistence tests pass. The final recorded catalogue sweep
visits all 176 type IDs (400 frames); seven endpoint-unavailable entries are
explicit labels. A separate 180-frame run verifies variable 900/1200-pixel
clearances, mostly vertical FireBall launch, retriggering and automatic wrap.
Pause keeps floor pixels stationary while FireFlash respawns. No review-map
sector files are saved. Audio output is silenced in these headless captures.

Local recordings and hashed results:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-review-mode-20261010/verification.json`.
These checks grant no additional retail appearance passes.
