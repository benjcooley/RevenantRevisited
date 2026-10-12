# The combat arena: one behaviour at a time, retail against Revisited

A flat, lit square of floor with nothing on it but Locke and the monsters a
test puts there. Retail and the port load the same arena and run the same
set-up lines, so a behaviour seen in one can be put beside the other.

## 1. The arena

`tools/combatarena/make_arena.py` writes it into a save slot's `CurMap`:

- 2048 × 2048 units of floor (`KInFloor2`, 128 × 128 a tile, 16 × 16
  tiles) at floor height 17, with a 3 × 3 grid of neutral lights;
- on level 2's sectors 24..25 × 2..3, which the shipped map leaves empty
  (the Keep stops at sector x 18), so nothing else is near;
- cloned from the floor tile and light records of the module's own sector
  `2_1_0` (`Ahkuilon.rvm`), their positions moved: retail-format sector
  files (SAVE_GAME.md §11.6) with the state hash of a sector that holds no
  characters.

Retail loads a slot's `CurMap` sectors by name, and the port's load stamps
every map object's walkmap as retail's does: the floor walks the same in
both.

A test's set-up lives in its slot, not in typed lines (retail's prompt is
too fragile to type into reliably). `tools/combatarena/make_arena_slot.py` builds one:
the port loads the arena slot, puts Locke there (`pos`), adds the monsters
(`addat`) and saves through its in-game menu half a second later, before
any monster has noticed him. The slot holds Locke's place in `game.sav`
and the monsters in the arena sectors, written by the port's own save code,
which retail reads (checked with revsave: Locke's place, the state hashes):

```
make_arena_slot.py ArenaA1 --locke 25375,3072 --monster Araknid@25825,3072
```

The port runs a slot with `arena.py` (scenario key `"slotdir"`); the
arena alone, for the port only, is the key `"arena": {}` (with `pos` and
`addat` in `exec`), as `arena_bot_vs_araknid.json` does for the player
bot.

## 2. Behaviours, one at a time

Each test fixes Locke's side so the two games see the same thing: no input
(with the `alreadydead` cheat where the monster's side is the subject), or
the same key presses at the same times.

| # | Test | Set-up | Watched |
|---|---|---|---|
| A1 | A monster's approach | one monster at 450; Locke idle, `alreadydead` | when it notices him, its path in, where it stops, its first blow |
| A2 | Its blows | as A1, a minute | blow cadence, which attacks, side steps, jumps, back-ups |
| A3 | Its blows on a blocker | as A1, Locke holding block | block outcomes, its responses |
| A4 | Locke's swings | a monster at 60; Locke swings A, S, D on a fixed beat | hit / miss / block, damage, animations, the rising numbers |
| A5 | Retreat | a monster at low health | when it backs off, how far, when it comes back |
| A6 | The orbit | Locke in combat holding each direction | the circle, the facing, the speeds |
| A7 | Two monsters | two at 450 | how they share him |
| A8 | Spells and missiles | a caster (Druid), an archer | cast timing, missile flight, hits |

## 3. The retail side

The lab (`tools/retaillab/retail.py`, RETAIL_CAPTURE.md) drives the shipped
game:

1. `retail.py upload <slot dir>`: game.sav, the thumbnail and the arena's
   sectors into the guest's `Save\Single`;
2. `retail.py start`, then `load --row N` (the Load Game list's row; check
   it with `capture`);
3. `key` presses Locke's side of the test; `record` captures the AVI.
4. `save --row N` saves in game over the slot on that row (Esc, Save Game,
   the row, Save Game: the list starts on its last slot, not the loaded
   one); `download <slot> <file>` fetches a slot's game.sav (its header
   holds the game time).

Check that `start` took the lab before sending anything: another session
may hold it.

The lab has no memory reads: retail's side is its video and its game log
(the combat results it prints, `NoCombatResults=No`).

## 4. Comparing

- Side by side, the same beats: frames from the AVI and the port's
  filmstrip at 640 × 480.
- The game logs: the combat-result lines (who hit whom, for how much) over
  the same minute.
- Retail's random stream can't be fixed, so a comparison is of patterns and
  measures (cadence, distances, outcome rates over a minute or more), not
  of single frames.

## 5. Results

| Test | Retail | Port | |
|---|---|---|---|
| A1 (an Araknid 450 east, Locke idle, a minute) | it never comes: no blow | the same | match: out of its notice (it finds him by ear, M9b) |
| A1b (the Araknid 150 east) | run 1: hit 7, glance 3, hit 10, glance 1, hit 9, hit 10, glance 4; run 2: hit 9, glance 3, 2, 3, 2, hit 8, glance 2, hit 7, the kill (≥ 7), 66 ticks a blow | before the fix: glance 1, hits 9, 13, 15, glance 3, hit 19 (47 ticks a blow); after: 1, 6, 10, 11, 2, double 15 | **fixed**: the port's Locke had no armour (below); hits now in retail's range. Cadence: two retail runs against one port seed, not yet a measure |

**The lab's clock (2026-10-11).** Retail runs at 0.91 of real time in
the lab: 21.8 ticks a second. Measured from four in-game saves of ArenaA1
(header game time = ticks × 100 / 24) at known wall times, the in-game
menu pausing the clock (a model with the clock stopped from each Esc to
about 1.9 s after its save fits all three intervals to 0.3 s). Retail's
blow times from the lab are wall seconds × 21.8.

**Locke's armour (found by A1b).** Retail's hits were 7-10, the port's
9-19. The trace's damage roll (event `damage-roll`: the attacker's weapon,
the target's armour and resistance, what it takes) showed Locke's armour
0: class.def gives cloth Protection 0, and retail writes WEAPON.DEF /
ARMOR.DEF numbers over class.def's at load (BindTypeData `0x48c7f0`,
COMBAT_DATA.md §8), which the port skipped. ARMOR.DEF's cloth is 1 a
piece: Locke's shirt and pants are armour 2, an Araknid's 8 is taken as 6,
and a hit lands 6-12 (doubles to 24), as in retail. Ported as
`TRules::ApplyItemData`; kata D1 checks every weapon and armor type.

**The combat results' text (fixed 2026-10-11).** The port printed raw
tags ("[BASEHIT] Locke [BASEDMG]:12 ...") because the arena's INI set
ClassDefPath to "." (to keep the lab's loose Resources/ files out of the
rules, from before the port's open looked in the archives first): the
game's message table, `ClassDefPath` + english.def, then resolved to the
module's dialog file. The arena now runs with the shipped paths
(`.\Resources`; the rules still come from resources.rvr, the traces are
unchanged) and the lines read as retail's: "Hit vs. Locke Dmg:8 Def:10
Off:12". The trace records each (event `result`), to set beside the lab's
combat log.

**Reading retail's blows.** The lab captures at ~35 frames a second
(`Control.capture`, 29 ms); each drop of Locke's health bar (y 22, x 68 +
health × 116 / 100) is a blow, and the combat log (the last five lines)
gives its amount.

**What the lab's video can time.** Retail draws few frames in the lab: the
picture changes 3-8 times a second at uneven gaps (none at all from 14.41 s
to 15.48 s of run 2) while the game ticks on at 21.8 a second. A blow seen
in a capture is placed within about half a second, so measures over many
blows hold (cadence, counts, amounts) but single moments don't: when the
Araknid starts its approach, or Locke draws, can't be told below a second.
Tick-level timing belongs to the emulator (a case's own random tape, both
sides tick by tick).

**A1b after the fixes (2026-10-11, port seeds 1-12).** Hits 7-10 (doubles
15-16), glances 1-5, half the blows glances, 7.8 blows to kill Locke, 55-61
ticks between blows: retail's two runs give hits 7-10, glances 1-4, 8 of 15
glances, 7 and 9 blows, 66 ticks. A match within what two runs can show.

**A1b after W1's fixes and the tag sounds (port seeds 1-12, 2026-10-11).**
Hits 6-10 (a few 11-15: doubles), glances 1-5, 43% of the blows glances,
7.6 blows to kill Locke (5-10), 58 ticks between blows (median 43), the
first blow at tick 59-191 (median 95); Locke falls into combat at tick 31
every seed. Retail's two lab runs: hits 7-10, glances 1-4, 8 of 15
glances, 7 and 9 blows, 66 ticks: still a match within what two runs show.
The opening is the emulator's to settle, and W1 does (below).

**A1b in the emulator (kata W1, 2026-10-11).** Retail's own tick over
A1b's world, run in the emulator against the port's
([forensics/SCENARIO_KATA.md](forensics/SCENARIO_KATA.md)): they agree
tick for tick. The Araknid finds Locke at tick 9 (its 32-frame window),
turns 8 a tick, walks in from tick 16, jumps at 38, bites at 54; Locke
falls into combat on his own at tick 30 (AutoBeginCombat on). So with
the option on retail's Locke does fall into combat; the lab's, still in
his walk stance after ~100 ticks, most likely had it off (the lab's INI
wasn't read).

Open from A1b: none.
