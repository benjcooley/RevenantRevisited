# Shadow Smoke: calibrating the duel model

The duel model (ARCHITECTURE.md §6) settles Shadow Smoke's fights without
playing them. It is useful only if its fights cost what real fights cost:
health, time and potions. This page measures that. It sets the model's fights
beside real ones in the same matchups, from the same game state, and records
what was tuned and what still differs.

**The short answer (level 1, Short Sword, no potion, on open ground).** An
Araknid costs 1.1 health a fight on average, and an Under Druhg 0.3: over 90%
of those fights cost nothing. A Mongrel Druhg costs 3.3, a Kantha 7, a Thrush
Druhg 7. An **Arakna** costs 33 and kills Locke one fight in four: its
SpiderPoison spell is the difference, and the model has no monster spells yet
(§5). The model agrees within the gates on every level-1 sword cell except the
Arakna.

## 1. Method

The same matchup is fought both ways from one setup. The setup is the console
lines of an arena scenario (`tools/combatarena/scenarios/calibrate/`), run on
the flat test arena (COMBAT_ARENA.md):

- **The slot** is `build/arena_slots/ArenaLocke`: Port Resave's Locke, saved
  standing on the arena, so a run loads level 2 alone. `calibrate.py` builds
  it with `make_arena_slot.py` when it is missing.
- **The lines** are `playerlevel N` (a fresh level-N Locke at full health),
  `delinv "Lesser Healing"` (no potion: the player bot doesn't drink),
  `unequip "Short Sword"` for the barehanded cells, then `addat` the monster
  150 east of Locke, inside its hearing, so it comes at once (the opening the
  emulator checked in kata W1).

**The real fights.** `arena.py` runs the scenario headless with `--fixedstep`,
`--seed N` and `--playerbot`: Locke is played by the player bot pressing his
keys (PLAYER_BOT.md), and the monster runs the port's retail-checked AI. The
run stops a second after the first death (scenario key `until_deaths`), or
after 4 minutes. The combat trace gives each fight's numbers:

- a **swing** is an `attack` event;
- a **blow** is a swing that struck, a `hit` or `miss` event from ResolveHit
  (a glance strikes; a swing out of reach or arc strikes nothing);
- damage comes from the `damage` events, guards from the bot's `block`
  decisions;
- the fight runs from the tick of the `addat` to the death.

These are 100 seeds a cell, at about 15 s of wall time a fight, 8 at once.

**The model's fights.** The duel harness, `--shadow-duel=<foe type>[,...]
--shadow-duel-runs=N` (`src/shadow/shadowbench.{h,cpp}`), runs the same
scenario lines. Once the queue is done and the foes are there, it throws N
Trial fights with seeds 1..N against the state they set up, and writes each
report as a line of `duels.jsonl`. These are 200 seeds a cell, about 5 s for
all of them. `--shadow-duel-trace=N` writes the first N fights' timelines
(`trace-<seed>.txt`): every action begun, every blow's fall with its distance
and angle, and where both stand each tick.

**The comparison.** `tools/shadow/calibrate.py` does both sides and the
table:

```
tools/shadow/calibrate.py scenarios                   # (re)write the matrix's scenarios
tools/shadow/calibrate.py real   [CELL ...] [--seeds 1-100] [--jobs 8]
tools/shadow/calibrate.py model  [CELL ...] [--runs 200] [--trace N]
tools/shadow/calibrate.py report [CELL ...] [--markdown]
tools/shadow/calibrate.py all    [CELL ...]
```

Results go to `build/calibrate/<cell>/` (`real.jsonl`, `model.jsonl`, each
real fight's trace). A real sweep resumes where it stopped.

## 2. The matrix

| Locke | Weapon | Monsters |
|---|---|---|
| level 1 | fists, Short Sword | Araknid, Under Druhg, Mongrel Druhg, Kantha, Thrush Druhg, Arakna |
| level 4 | Short Sword | Thrush Druhg |
| level 4 | fists | Mongrel Druhg |
| level 8 | Short Sword | Pale Ogrok, Vashar |

`playerlevel` gives a level 4 or 8 Locke two attribute points a level at
random. The real side rolls them per seed, while the harness builds one Locke
on seed 1. That is a small spread, worth a few to-hit points at most, on the
level-4 and level-8 rows.

## 3. The gates

A cell passes when each gated measure is inside its tolerance.

| Measure | Tolerance | Why |
|---|---|---|
| Win rate | 10 points, or 2 standard errors of the difference | With 100 real fights, a win rate near 50% has a standard error of 5 points. 10 points is about the noise floor, and anything wider is a modelling fault. |
| Median duration | 25%, or 24 ticks | Duration is the game time a fight takes: regeneration, generator clocks and poison run on it. A quarter is the error the plan's time budget can carry. |
| Locke's hit rate, the foe's hit rate | 10 points, or 2 standard errors | These come straight from retail's dice. A gap means the model has the guard or the opening wrong. A rate over a few dozen blows has a standard error near 9 points, so the wider bound decides there, and a rate over fewer than 30 blows isn't judged. |
| Damage per blow | 20%, or 1 point | Again retail's arithmetic, including the compounding of the button search. A difference means the wrong attack or the wrong tier. |
| Health lost | 25%, or 5 points | This is what the plan prices a fight by, in potions and rest. 5 points is half a Lesser Healing's worth, below what the plan can notice. |

## 4. Results

The final model: 200 trial fights a cell against 100 real ones. Each pair is
real / model, and **✗** marks a measure outside its gate.

| Cell | win % | median ticks | Locke hit % | dmg / blow | foe hit % | HP lost | Locke swings | swings struck % | |
|---|---|---|---|---|---|---|---|---|---|
| l1-fists-araknid | 100 / 100 | 120 / 104 | 67 / 68 | 12 / 12 | n/a / n/a | 0.7 / 0.7 | 4.2 / 3.9 | 75 / 81 | pass |
| l1-sword-araknid | 100 / 100 | 98 / 78 | 61 / 67 | 12 / 12 | n/a / n/a | 1.1 / 0.5 | 4.0 / 3.2 | 87 / 94 | pass |
| l1-fists-underdruhg | 100 / 100 | 570 / 474 | 71 / 80 | 12 / 13 | 52 / 33 **✗** | 2.4 / 0.9 | 19 / 15 | 25 / 30 | **fail** |
| l1-sword-underdruhg | 100 / 100 | 194 / 182 | 76 / 80 | 12 / 13 | n/a / n/a | 0.3 / 0.2 | 6.7 / 6.4 | 70 / 69 | pass |
| l1-fists-mongreldruhg | 100 / 100 | 840 / 654 | 60 / 65 | 10.0 / 11 | 54 / 50 | 13 / 7.9 **✗** | 27 / 20 | 29 / 36 | **fail** |
| l1-sword-mongreldruhg | 100 / 100 | 326 / 314 | 65 / 65 | 10 / 10 | 56 / 48 | 3.3 / 1.7 | 11 / 10 | 67 / 73 | pass |
| l1-fists-kantha | 98 / 98 | 786 / 619 | 55 / 65 | 9.3 / 11 | 62 / 67 | 27 / 31 | 20 / 14 | 42 / 49 | pass |
| l1-sword-kantha | 100 / 100 | 368 / 370 | 53 / 59 | 8.9 / 9.8 | 65 / 53 | 7.1 / 3.9 | 11 / 9.0 | 82 / 87 | pass |
| l1-fists-thrushdruhg | 96 / 98 | 1695 / 1032 **✗** | 41 / 46 | 5.5 / 6.0 | 64 / 61 | 43 / 22 **✗** | 52 / 31 | 29 / 44 | **fail** |
| l1-sword-thrushdruhg | 100 / 100 | 543 / 451 | 43 / 47 | 7.3 / 7.7 | 57 / 65 | 6.9 / 5.8 | 17 / 14 | 68 / 77 | pass |
| l1-fists-arakna | 77 / 100 **✗** | 708 / 624 | 30 / 36 | 6.1 / 6.7 | 78 / 88 | 29 / 3.4 **✗** | 20 / 20 | 87 / 92 | **fail** |
| l1-sword-arakna | 77 / 100 **✗** | 752 / 611 | 30 / 36 | 5.7 / 6.5 | 80 / 70 | 33 / 3.5 **✗** | 22 / 20 | 90 / 97 | **fail** |
| l4-sword-thrushdruhg | 100 / 100 | 265 / 254 | 86 / 91 | 16 / 15 | n/a / 34 | 1.0 / 1.3 | 8.8 / 7.9 | 66 / 73 | pass |
| l4-fists-mongreldruhg | 100 / 100 | 441 / 402 | 89 / 96 | 18 / 18 | 20 / 22 | 2.8 / 2.6 | 15 / 13 | 31 / 34 | pass |
| l8-sword-paleogrok | 100 / 100 | 362 / 338 | 70 / 76 | 13 / 13 | 25 / 10 **✗** | 5.0 / 3.2 | 11 / 9.4 | 90 / 96 | **fail** |
| l8-sword-vashar | 100 / 99 | 766 / 801 | 49 / 49 | 9.4 / 9.8 | 41 / 31 | 72 / 83 | 17 / 15 | 76 / 77 | pass |

10 of 16 cells pass. Health lost per fight at level 1 with the Short Sword,
real / model:

| Cell | mean | fights costing nothing | 90th percentile | worst | Locke died |
|---|---|---|---|---|---|
| Araknid | 1.1 / 0.5 | 93% / 96% | 0 / 0 | 28 / 26 | 0 / 0 |
| Under Druhg | 0.3 / 0.2 | 97% / 98% | 0 / 0 | 10 / 10 | 0 / 0 |
| Mongrel Druhg | 3.3 / 1.7 | 81% / 90% | 14 / 0 | 30 / 29 | 0 / 0 |
| Kantha | 7.1 / 3.9 | 69% / 78% | 24 / 16 | 48 / 48 | 0 / 0 |
| Thrush Druhg | 6.9 / 5.8 | 71% / 78% | 24 / 24 | 60 / 61 | 0 / 0 |
| Arakna | 33 / 3.5 | 52% / 89% | 111 / 12 | 126 / 72 | 23% / 0 |

The player bot fights carefully. It holds off just outside the monster's
reach, comes in only when the monster isn't winding up, and steps out of
what it can. So the early monsters rarely touch it, and a player who fights
the same way does as well. The model is a little kinder than the bot at
level 1 (smaller losses, shorter fights). Two habits of the bot that the
model doesn't copy account for part of that: see §5.

## 5. What differs, and why

### 5.1 Monster spells and poison

These are not modelled. The Arakna has a MAGICATTACK, SpiderPoison: a
missile, then poison. In the real fights it casts about 0.4 times a fight.
The poison deals 7–9 health a fight before the fight ends, and keeps ticking
afterwards, so the true cost is higher than the table's 33. The cast also
takes the fight's shape away from the bot. While it strafes across the
missile's path (10% of the fight), it stops holding its distance, and half
of the Arakna's 3.2 swings a fight come in those moments. The model's Arakna
swings 0.3–0.5 times a fight and never casts. **Shadow must not trust the
model for any monster with a spell** until monster casts and poison are in
the duel (ARCHITECTURE.md §6.8, §8.7). The plan's sizing up should treat such
a monster as unknown, not as easy.

### 5.2 Fist positioning

Barehanded against the Mongrel and Thrush Druhgs, the model's fights are 22%
and 39% shorter, and cost half the health. The model's Locke lands 7–15
points more of his swings: 36% of them against the bot's 29% for the
Mongrel, 44% against 29% for the Thrush. Fist blows reach 45. The bot presses
its attack key once the foe is inside the *longest* reach of that key's
attacks: 50–70, counting lunges and sword moves that a fist can't make. So it
swings from about 45–55 and many swings fall a few units short. The model
reproduces that rule, and real swings that land are explained in full by
reach and arc at the impact frame (94%). What remains is a few units of
positioning, from the bot's movement details. Against the Thrush Druhg (fists
do 20% less, so the fight is long) those whiffs add up. With the sword
(reach 55–70) the same few units hardly matter, and every sword cell passes
except the Arakna's.

### 5.3 The bot's buttons

The bot picks its attack key at random each time it comes in. The model's
Locke follows the player's rule never to press the same key a third time
running (GRIND.md §2.4). One press in nine of the bot's is such a third
press, which rolls 100: the worst tier, and a 1-in-11 chance of the monster's
free counter. That makes the bot's fights a few percent longer than a
careful player's. The model keeps the player's rule.

### 5.4 The foe's hit rate at level 8

Against the Pale Ogrok and the Vashar, the model's Locke guards more often
than the bot when a blow strikes him: his guard is up for 52–64% of the blows
that reach him, against the bot's 36–39%. So the foe's hit rate is 10–15
points low. Health lost still agrees (Vashar 72 / 83, Pale Ogrok 5.0 / 3.2),
because a guarded glance is cut to almost nothing either way.

### 5.5 Monsters swing a little less often

Outside the Arakna, the model's monsters leave 10–40% more time between
blows than the real ones: Mongrel and Thrush Druhgs 10–16%, Kantha 14–40%,
Under Druhg and Araknid 18–42%. Their moves come at the real rate, so the
gap is in when a blow is allowed: the attack timer's interplay with the
moves, the flinches Locke's hits cause, and the fatigue a blow needs (the
Druhgs' cost 50). The cost of a fight follows the blows that strike, which
agree within the gates for the sword cells. But this gap, like §5.2, makes
the model a little easy.

### 5.6 What the arena doesn't cover

- **Open ground.** The real forest has trees. The bot falls back to open
  ground in tight spots, and the model has no walls.
- **The opening.** The monster starts 150 away, inside its hearing. Monsters
  that engage from their sight range are not measured.
- **One monster.** Fights against several at once (at most three in reach)
  are not calibrated.
- **Into combat.** The real Locke's 10-tick walk-to-combat transition at the
  start isn't modelled.

## 6. What was tuned

The model began as a timeline with no positions. That model:

- fought at the monster's MAXATTACKRANGE, where the Araknid's bites (reach
  10) are never valid, so it never struck;
- had Locke swing every `nextwait`, two to three times the bot's rate, with
  every swing striking;
- treated a monster as open only mid-blow;
- left Locke's StrengthMod out of his damage;
- stalled when a retreating monster was caught.

The real traces showed what to replace each with. In order:

1. **Positions and root motion.** Each fighter is a point with a facing.
   Each action plays its animation's frames from the imagery and moves the
   fighter by their root motion. The real traces confirm the displacement
   per monster move: for the Mongrel Druhg, real / model in units toward
   Locke and across, the side step −8 / −13 and 37 / 34, the run −33 / −38
   and 64 / 56, the handspring lunge −57 / −69 and 9 / 1.
2. **Monster moves.** RandomAttack picks among the moves (PLAYANIM records)
   as well as the blows. A move needs no timer, re-aims at its target each
   tick (ResolveAttack), so side steps circle Locke, and leaves the monster
   open (+25). Real: 92% of Locke's swings at an Araknid get the +25. Model:
   74%, against 0% before. Moves cost no fatigue (the real monsters' fatigue climbs
   straight through them; COMBAT_HIT.md §3.2).
3. **Locke plays like the bot.** He reacts after 5 ticks and comes in at a
   12-tick window with 60% aggression. He holds off at the foe's reach + 24.
   He backs off after a swing. A blow he can step out of, he steps out of;
   else he guards (35%) or rolls (15%) each tick. The cadence that gives
   matches: 27–54 ticks a swing in the model against 28–47 real, within 2
   ticks in every cell but the Kantha's and the Vashar's.
4. **The last step carries him on.** The bot is two steps closer at the swing
   than the tick before it decided to come in. A press takes his step and
   then the swing.
5. **He reads the world a tick late.** The bot reads its senses before the
   tick, so the model's Locke decides before anything moves.
6. **Eight keys, and the facing they leave.** He steps one of the eight ways
   the direction keys point. A new key sets his facing: the forward key faces
   the way it points, rounded off the foe (the real trace: facing 32 with the
   foe at 43), and another faces the foe. He turns at the game's turn rate.
   So a swing often starts 10–20 off the foe, which costs real swings.
7. **The monster's attack timer.** The AI decrements its timer a second time
   unless "the last attack" was a move, and the game records an attack only
   as it starts to play. So that check reads the attack *before* the current
   try. A blow after a move leaves the timer at 0, and a second blow can
   follow at once: 21% of real blows do. This took the monsters' swing
   interval from 30–50% too long to 10–40% (§5.5).
8. **Guards.** A block decided on the impact tick still counts (real: 20 of
   21 and 37 of 37). A roll needs the combat stance, so one the weaving bot
   tries mid-step does nothing. Real: 4 of 38 rolls on the impact tick
   guarded anything.
9. **Smaller fixes.**
   - The button search tiers the dice again at every candidate that reaches
     them (retail's compounding: real crits of 12 → 18 → 27 → 40 → 90).
   - Locke's StrengthMod is in his damage.
   - A lunge stops 10 short of its target (ResolveAttack).
   - A caught retreating monster runs for RETREATFOR and comes back.
   - Potions come from the belt only (`ShadowBeltHealing`), the stock
     counted in trial fights.

No retail number was changed: RollAttack, WeighBlow, LandBlow and char.def
are the game's. The new habits are the bot's (PLAYER_BOT.md) or the game's
own rules.

**The Keep regression.** Locke now beats Rahul barehanded in 2130 ticks
(8 of 50 swings strike) where the old model took 214. The plan's first
stages pass (`shadow.py --until potions`).
