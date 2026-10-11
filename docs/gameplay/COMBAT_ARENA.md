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
| A1b (the Araknid 150 east) | it closes and bites: hit 7, glance 3, hit 10, glance 1, hit 9, hit 10, glance 4 = 44, Locke dead after ~38 s of lab time | hit 12, glance 5, glance 2, hit 11, glance 2, hit 10, double 19 = 61, dead at tick 382 (13.5 s after the first bite) | same blows (seven, hits and glances); port damage maybe higher (more runs); pace not yet comparable: retail's ticks a second in the lab unmeasured |

Open from A1b:
- **Retail's tick rate in the lab.** Two saves a known wall time apart give
  it (the save header's game time is frames × 100 / 24).
- **Damage.** Seven blows a side is too few: repeat A1b and compare the
  distributions (hit and glance amounts, doubles).
- **The port's combat results print raw tags** ("[BASEHIT] Locke [BASEDMG]:12
  [BASEDEF]:10 [BASEOFF]:12") where retail prints "Hit vs. Locke Dmg:7
  Def:10 Off:12": the text lookup for those lines fails.
