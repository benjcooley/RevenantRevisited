# Playtest findings

Bugs and gaps that Jev's sessions surfaced in the games, with what reproduces
each. Newest first. Each entry: what was seen, the evidence, the repro, and
who owns the fix.

## F1. A dead character saved and loaded is gone (retail behaviour)

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

**Cause: retail removes a character loaded dead.** The port follows it
([SAVE_GAME.md](../gameflow/forensics/SAVE_GAME.md) §11.3, "A character
loaded dead"):
- `TCharacter::Load` (`0x004d4eb0`) sets KILL (`0x1000`) on a character
  whose health is below 1, whether it comes from a save or a sector
  reloaded during play. The port does the same (`src/character.cpp:5451`).
- The first `PulseObjects` (`0x004580f0`) deletes it (`0x00451840`). The
  port nukes it there too (`src/mappane.cpp:3809`).
- With `Rahul` gone, the expression evaluator (`0x0041f230`) skips the
  rest of the line with no value and returns 0. `if` (`0x0041fb70`) then
  returns `0x84`, and the interpreter prints "Bad parameters." The port does
  the same (`src/scriptvalue.cpp:386`, `src/command.cpp:1225`).
- **Retail, run on the same slot.** The emulator's play slot loaded `Quick
  Save 1`. Rahul came in with flags `0x0006d028` (the saved `0x0006c028`
  plus KILL) and was deleted on the second tick. TendrickR's `if` returned
  `0x84` with "Bad parameters." at ticks 25, 100, 125, … 225. Control
  stayed on, and no scene ran in 240 ticks.
- **Retail host gap found on the way.** As committed, the host
  (`playtest/retail`) starts every slot as a new game: retail reports
  "Unable to load saved game... starting new game". Two things cause it.
  The slot list that QUICKSTART's name is looked up in (`0x0048d6d0`) was
  built before the slot was placed, and the HUD machine's `winfs` matches
  `*.*` only against names that contain a dot, so even a rescan
  (`0x0048d260`, `Save\Single\*.*`) misses `Quick Save 1`. The run above
  rescanned after placing the slot and gave `*.*` the Windows meaning.

The window is narrow in retail too. It runs from the blow that leaves Rahul
at 0 health to TendrickR's next pass of the `If`, about 25 ticks. The `If`
turns control off, and while control is off retail ignores the Quick Save
control and the menu's Save
([INGAME_MENU.md](../gameflow/forensics/INGAME_MENU.md) §2–3). Once the scene has run
`ressexit.stat locked=0`, a save is safe, because the door keeps the lock
in its saved stats.

**Repro (port; matches retail).** Waypoint `~/RevenantRevisited-testsaves/waypoints/port/rahul-defeated--20261010-231733`:
copy its `slot/Quick Save 1` into `<save>/Save/Single/`, then
`REVENANT_SAVE_PATH=<save> build/Revenant --headless --quickstart="Quick Save 1"
--fixedstep --seed=1 --max-runtime=25 --exec "rahul.stat; sleep 24; rahul.stat"`.
The first `rahul.stat` lists `Flags: MOVING AI KILL ANIMATE PULSE COMPLEX
NOTIFY`. The second answers `rahul: Context not found`.

**Resolution.** This is retail behaviour, so the engine is unchanged. The
playtest saves after Tendrick's scene: `rahul defeated` is
`after_scene(character_dead('Rahul'))` (director.py), done once the scene
has played and control has been back for two seconds. Waypoints from before
that change (`rahul-defeated--20261010-230646`, `-231018`, `-231733`) are
the half-settled kind and should not be played from.
