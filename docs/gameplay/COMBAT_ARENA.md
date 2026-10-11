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

On the arena, a test runs script lines both games take: the port's
`--exec`, retail's Enter prompt with `@` (RETAIL_CAPTURE.md §2):

```
pos 25375 3072 17 2          # Locke to the west side
addat 25825 3072 Araknid     # an Araknid 450 east
```

In the port the arena is a scenario key (`"arena": {}`) of
`tools/combatarena/arena.py`; `arena_bot_vs_araknid.json` runs the player
bot against an Araknid there.

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

1. the arena slot is uploaded into the guest's `Save\Single` (the guest
   agent's `copy_tree`);
2. Load Game loads it;
3. `prompt` types the set-up lines (`@pos ...`, `@addat ...`) and cheats;
4. `key` presses Locke's side of the test; `record` captures the AVI.

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
