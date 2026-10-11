# The player bot: Locke played by code

A controller that plays Locke the way a person does, for auto-battles, combat
tests and demos. Unlike retail's player AI (`--playerai`, the monster brain
`TCharacter::AI` run on the player), it fights like a player:

- it **stands off** just outside the enemy's reach;
- it **comes in to strike** when the enemy is open, then backs off;
- it **reads incoming blows** and gets out of their reach or arc by moving
  (weaving, side steps), and **blocks** or **dodges** some of the time;
- it **steps out of the path** of missiles and spells;
- it **prefers open ground**: in a tight spot it falls back to more open
  ground nearby and makes its stand there.

It is a Revisited feature, not a retail one. It never reaches into combat
code: it **presses the player's keys** (the bindings in the control map, so
an INI's rebinding holds), through the same entry real keys take
(`IRuntimeMode::HandleKey`). Everything it does exercises the player's own
path: UpdateMove, Go and the orbit, ButtonAttack, Block, Leap, Dodge.

## 1. Pieces

| Piece | Job |
|---|---|
| `TBotInput` | A virtual keyboard: hold or release a held control (a direction, block, leap, run), tap a command (combat mode, swing, thrust, chop, a combo, dodge). Looks bindings up in `ControlMap` by command or flag, modifier keys first. Releases everything when the bot stops. |
| `TBotSenses` | What a player could see, each tick: Locke (position, facing, health, fatigue, root and doing block); the enemies near him (position, edge distance, facing, the action they're in and its attack's reach, arc and impact frame); things flying at him (any moving non-character object, its velocity measured from its last positions). Decisions read the senses as they were `reaction` ticks ago. |
| `TBotSpace` | The ground around Locke, from the walk grid (`MapPane.GetWalkGridHeight`): which cells a character of Locke's radius can stand on (retail's Blocked rule: no hole within the radius, no step over 0x20), each cell's clearance (cells to the nearest unstandable one), what's reachable from Locke (8-connected, steps within 0x20) and the paths there. Rebuilt every few ticks over a window round Locke. |
| `TBotBrain` | The tactics (below): each tick a state, a move direction (or none), and the keys to hold or tap. |
| `TPlayerBot` | Owns the four; runs once a tick before the play screen reads the held controls (`TPlayScreen::UpdateMove`); its own random stream (seeded from the session seed), so it never draws on the game's. |

## 2. Tactics

Every tick, in order:

1. **No enemy within awareness**: leave combat mode after a while; idle.
2. **An enemy within engage range**: enter combat mode (the combat key).
3. **A blow is coming** (an enemy in an ATTACK whose impact frame is
   ahead; Locke within its reach plus a margin and inside its arc):
   - if Locke can be out of its reach or arc by the impact tick at his
     speed (measured from his own last moves): **evade** (the direction
     that leaves the reach soonest, sideways preferred, toward open ground);
   - else **block** (held until the impact passes) with the profile's
     chance, else **dodge** with its chance, else take it.
4. **A missile is coming** (closest approach within Locke's radius plus a
   margin, inside a horizon): strafe across its path.
5. **Tight spot** (Locke's cell clearance under the profile's minimum, or
   enemies on two sides): **fall back** along the path to the best open
   cell within reach (clearance, then distance from the enemies, then
   path length), unless a blow is coming; then turn and fight.
6. **Offense**: Locke's attack is ready (not mid-swing, fatigue above the
   reserve) and the target is open (not winding up a blow that would land
   first): **close** to the chosen attack's range and **press** it (swing,
   thrust, chop or a combo, by range and the profile's mix), then **back
   off** to the standoff band once the swing ends.
7. Otherwise **stand off**: hold the band just outside the target's reach,
   **weaving** (a sideways strafe, its side and length random) so as not to
   stand still in front of it.

Moving is holding one of the eight direction keys: the move angle rounded to
the nearest 32 (UpdateMove's bit i is angle i * 32), chosen among those
whose next step is standable. In combat the orbit keeps Locke facing his
target (CombatFace), as for a player.

## 3. Profile

| Parameter | Default | Meaning |
|---|---|---|
| `reaction` | 5 ticks | how stale the senses decisions read are (about 0.2 s) |
| `aggression` | 0.6 | how readily it comes in to strike |
| `block` | 0.35 | chance to block a blow it can't step out of |
| `dodge` | 0.15 | chance to dodge one instead |
| `margin` | 24 | standoff margin outside the enemy's reach |
| `openness` | 4 cells | clearance under which a spot is tight |
| `weave` | 18..40 ticks | how long a weave strafe lasts |

## 4. Using it

- `--playerbot` (with `--playerbot=<profile file>` later) runs it from the
  start; the console's `bot on` / `bot off` toggles it in play.
- The combat arena (`tools/combatarena/`) takes `"playerbot": true` in a
  scenario in place of `"playerai"`.
- With `--combattrace` it writes its decisions (`bot` events: state, why,
  keys), beside the fight's.

## 5. Measures

Arena scenarios (Locke against an Araknid; an Ogrok; a Druid's spells; two
monsters; a corridor) with the bot. From the trace: damage dealt and taken,
the enemy's blows that missed because Locke had left their reach, blocks,
the time to kill, Locke's clearance over the fight. Each must stay
deterministic (two runs, identical traces).

## 6. Build order

1. `TBotInput`, `TBotSenses`, the strike / back-off / standoff loop against
   one Araknid.
2. Reading blows: evade, block, dodge.
3. `TBotSpace`: open ground, falling back.
4. Missiles and spells.
5. Scenarios, measures, the profile file.
