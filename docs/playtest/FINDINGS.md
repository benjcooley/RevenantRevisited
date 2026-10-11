# Playtest findings

Bugs and gaps that Jev's sessions surfaced in the games, with what reproduces
each. Newest first. Each entry: what was seen, the evidence, the repro, and
who owns the fix.

## F1. A dead character saved and loaded is gone (port)

**Seen 2026-10-10.** Jev killed Rahul from the `opening` waypoint and the
director quick-saved at once (waypoint `rahul defeated`). Played from that
waypoint, Tendrick's scene never starts, so `ressexit` stays locked ("It seems
to be locked") and the Keep chain stops.

**Evidence.**
- The saved sector `2_1_0.DAT` holds Rahul (type `Rahul`, `<type name>`) with
  `HLT=0`, `state 19`, root action `combat` (revsave dump of the slot).
- After loading the slot, `rahul.stat` answers `rahul: Context not found`,
  and TendrickR's trigger line `If Rahul.stat health = 0` logs `Bad
  parameters` every pass, so its block never runs.
- Without a save and load in between, the same check passes and the scene
  plays (sessions `watch1`, `p3`).

**Repro.** Waypoint `~/RevenantRevisited-testsaves/waypoints/port/rahul-defeated--20261010-231733`:
copy its `slot/Quick Save 1` into `<save>/Save/Single/`, then
`REVENANT_SAVE_PATH=<save> build/Revenant --headless --quickstart="Quick Save 1"
--fixedstep --seed=1 --max-runtime=25 --exec "sleep 24; rahul.stat"`.

**Open.** Whether retail keeps the dead Rahul across a save (the emulator's
play slot can now answer: kill Rahul, quick-save, load). Owner: gameflow
(saves). Meanwhile the Keep milestone saves only after the scene that follows
the fight has played, which is also the better place for a waypoint.
