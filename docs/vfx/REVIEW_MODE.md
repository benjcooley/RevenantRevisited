# Scrolling VFX review

Run `Revenant --test=vfx-review` from the game installation/source directory.
The mode uses a black background and places the loaded EFFECT
catalogue in name/type-ID order with per-effect clearance. It never saves this map.
An invisible disposable floor supplies normal walk-height/collision queries.

The camera follows the row toward the upper right, so effects scroll toward
the lower left. The default slope is 2.5 horizontal units per vertical unit.
The default minimum clearance is 240 logical screen pixels: about three small
station centers per view, with partial neighbors at the edges. An absolute
240-pixel floor prevents dense rows even when a smaller override is requested.
Padded stored culling bounds contribute at most a quarter of a viewport to layout.
Known broad-emitter hints and explicit overrides increase clearance as needed. Automatic spacing is capped at 1.15 viewport lengths, including large effects.
Explicit per-effect overrides can reserve more room.
Scrolling runs at 48 pixels/sec.
There are 720 pixels of empty lead-in and tail before the view loops.
Effects stay in view for roughly 14 seconds at the default viewport. A full
176-entry pass takes over an hour; select a smaller group while fixing it.

```sh
./build-merge/Revenant --test=vfx-review \
  --vfx-review-effects=Flame,FireBall,CyanFont,SymGlow \
  --vfx-review-first
```

`--vfx-review-first` starts at the first station, skipping the empty lead-in.
Omit the filter and first flag to review the entire catalogue from the beginning.
Filter entries accept case-insensitive full names or hexadecimal retail type
IDs, separated by commas. Duplicate names retain separate type-ID entries.
Unknown entries are rejected so typos cannot silently change the review.

Options:

- `--vfx-review-speed=48`: logical pixels/sec, range 0.1–240.
- `--vfx-review-ratio=2.5`: horizontal/vertical slope, range 2–3.
- `--vfx-review-spacing=240`: requested minimum, range 180–800; effective
  clearance respects the 240-pixel floor and viewport density limit.
- `--vfx-review-spacing-overrides=FireCone:1200,FireBall:900`: per-effect
  requested clearance in pixels, range 180–2400; names or hexadecimal type IDs.
  Overrides can increase spacing; they cannot bypass minimum/footprint clearance.
- `--vfx-review-path-tilt=0.25`: upward path with a slight rightward tilt,
  independent of the scroll; 0 is straight up, negative values tilt left.
  For an exact perpendicular to a 2.5:1 scroll, use `-0.4`.
- `--vfx-review-path-length=200`: source/target separation in logical pixels,
  range 80–640.
- `--vfx-review-gap=720`: empty distance at both ends, range 640–1600.
- `--vfx-review-offset=0`: initial distance into the scrolling route.
- `--vfx-review-first`: start at the first station, regardless of automatic padding.
- `--vfx-review-effects=Flame,0x63fd382a`: restrict the catalogue.
- `--scene-ambient=32,255,255,255`: override scene lighting.
- `--vfx-lighting-mode=classic|modern`: use the existing lighting diagnostic.
- `--vfx-review-source-lighting`: retain original scene directional/ambient
  balance instead of the default bright ambient fill and soft directional light.

Space pauses **scrolling** while effects continue cycling. Left/Right jump to
the previous/next station. Home returns to the lead-in. Escape exits.

Labels show only a large, centered name beneath each station. Missing drawables
remain in the catalogue, with an amber name marking their empty slot.
Factory/type/asset diagnostics remain in the log. Unavailable entries retain a name-only slot; endpoint-unavailable entries
use minimum spacing rather than reserving large invisible bounds.
Adjacent stations use the larger of their
two clearances. End padding grows enough to clear the first/last footprint,
without inheriting the entire large-effect isolation gap.
Spacing changes layout only, never effect geometry or physics.
Dedicated map effects and admitted authored controllers use the normal map
factory. FireBall uses its complete standalone initializer, 24Hz simulation,
billboard submission and world-ring submission; it does not also receive map
Pulses. Other implemented effects use exact-type registered preview
initializers/controllers through the same world render pass. Bare asset geometry
is not presented as an implemented effect. Effects advance at their existing
24 Hz/runtime cadence and draw through the normal renderer. A completed one-shot is recreated
after cleanup; a looping effect continues its authored animation. Fallbacks retain their existing browser repeat policy.

Endpoint-enabled effects launch mostly vertically from a lower-left source
toward an upper-right target, independently of the review row. Unavailable
endpoint/runtime entries retain their names. FireBall uses the destination as an
explicit synthetic target collision (original 32-unit hit radius), then plays
its impact, finishes its tail and waits 1.5 seconds before the next cast. The
ordinary standalone preview and natural map collision paths remain unchanged.

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
Pause keeps the camera stationary while FireFlash respawns. No review-map
sector files are saved. Audio output is silenced in these headless captures.

Local recordings and hashed results:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-review-mode-20261010/verification.json`.
These checks grant no additional retail appearance passes.

Black-background update: footprint/density and schedule tests pass. Fresh Metal
captures verify a 180-pixel spacing request is held at 320, FireCone receives
1785 pixels of isolation clearance and only 941 pixels of end padding, and
`--vfx-review-first` opens on the selected first station. Blank background pixels
are exactly RGB(0,0,0); normal collision tiles remain hidden. The full 176-row
catalogue initializes with automatic footprint spacing. Local hashed evidence:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-review-black-20261010/verification.json`.

Readable-review update: default speed is now 48 logical pixels/sec (three times
faster). The screen shows only the 26-pixel centered effect name below each
station, with amber names for unavailable visuals. All 176 entries remain.
Known endpoint-unavailable entries use minimum spacing to avoid long invisible
slots. Bright lighting records ambient 0.745/directional 0.372; the optional
source-lighting flag retains ambient 0.149/directional 1.0. Three captured runs
verify both lighting presets and an empty, labeled arroweffect slot.
Local results: `/Users/benjamincooley/RevenantRetailLab/research/vfx-review-readable-20261010/verification.json`.

Small-effect spacing is now 240px (previously 420). Stored culling bounds are capped for layout so inflated rectangles cannot create multi-minute empty gaps. All unavailable entries remain named slots.
