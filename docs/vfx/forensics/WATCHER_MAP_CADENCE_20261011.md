# Ogrok watcher actual map cadence

The three shipped Ogrok watchers advance through their four-frame loop on every
original map tick. No frame-hold rule was found in their actual owner/default
animator path. Under the original 24 Hz scheduler, this is six loops per second.
The field called `framerate` is a signed frame increment (here +1), not frames
per second. No production speed change is justified by this result.

`tools/retail_runtime/watcher_map_cadence_probe.py` restores genuine original
startup, loads each exact class25 registry identity (`aeaeeb23/24/25`), and uses
actual MapPane.NewObject `450e40` / GetInstance `452690` and CreateAnimator
`46e8b0`. Each owner is generic EFFECT `5a85ac`; each animator is default3D
`5a370c`. For every tick it executes whole map PulseObjects `4580f0`,
MoveObjects `458390`, AnimateObjects(draw=false) `458750`, and finally
NextFrameObjects `457ef0`. The test does not directly call owner NextFrame or
write owner frame/rate fields.

All three owners passed 240 ticks, 720 map-dispatched NextFrame calls and 60
wraps each. Each tick enters the original generic/base Pulse, default Animate,
and NextFrame exactly once. Owner frames follow 1,2,3,0; animator cached frames
follow 0,1,2,3, matching retail's end-of-tick frame advancement. Twelve ticks
without the map NextFrame stage hold all frames. Twelve ticks with the real
NoAnimateObjs global set suppress Animate/NextFrame and hold all frames.
Restoring the gate advances exactly once. Pulse remains active in both controls.

The cached startup has no background-update thread. The fixture explicitly
publishes the original SetMapPos scroll result into the map's rendered-scroll
pair, so the screen rectangle admits the objects at declared (3185,3185,16).
This viewport boundary is recorded; no clock/effect/geometry/rate is replaced.
No pixels, OS timer, natural map placement, background worker or hardware pacing
were executed. The 24 Hz rate comes from the independently traced original
scheduler: PlayScreen `47c38d..47c39e` computes game hundredths as frames*100/24;
TimerTick calls the map advance stage once after Animate. See
`docs/gameplay/forensics/COMBAT_MOTION.md`, section 2. This is a per-tick cadence
proof, not a claim that slow retail hardware always sustained 24 wall-clock FPS.
`RealTimeLight` in the retail INI controls lighting, not an owner animation hold.

Frozen report:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/watcher-map-cadence/final/manifest.json`
SHA256 `054b0ac21ba62b2a20fa7f6bbe1cc83fc0fde6838068a74bdfd16a2755e29b12`.
It pins the original executable, startup image, probe source and all observed
per-tick calls/state plus negative controls. Existing watcher pixel/format
approvals are unchanged; other effects require their own cadence diagnosis.
