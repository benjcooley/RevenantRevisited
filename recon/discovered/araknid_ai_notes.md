# Araknid AI — porting notes

## TL;DR

**Araknid has no special-case branch in `TCharacter::AI_PerMonster`.**
Its behaviour comes entirely from the generic `TCharacter::AI`
(`FUN_004c8b60`) plus the data in `Imagery/char.def`'s `CHARACTER "Araknid"`
section. The only place "Araknid" is mentioned in `AI_PerMonster` at all
is inside the **Solifuge** boss case, which spawns Araknid (and
Artikulid / Recluse / Arakna) instances as minions.

A naïve port that just runs the generic AI loop on an Araknid will
behave correctly. No per-monster state buffer is allocated; the
dispatcher writes `mbr_0x280 = -1` and the switch's `default:` returns
0 immediately.

## Dispatcher table (verified by reading 0x4c9b70 prologue)

`AI_PerMonster` is a one-time init + per-frame switch. The init
string-compares `this->name` (`*(char**)(this->mbr_0x4c)`) against four
literals and assigns a type id + state-buffer size:

| name string | type id (`mbr_0x280`) | state buf size (`mbr_0x284`) | switch body |
| --- | --- | --- | --- |
| `"Baez"` (`DAT_005dfa14`) | 1 | 0x58 (22 ints) | falls out of switch via `break;` then runs the Baez party-AI body (lines 1117..end of fn) — chants/cheers, ringwall/ForBreakTable handling, big follow logic |
| `"Solifuge"` (`s_005dfa1c`) | 2 | 0x34 (13 ints) | full big-spider boss script: hp-band tracking, materials tint while casting, skywalk transitions, **spawn 3 minions** (slot 0: Recluse or Arakna 50/50; slots 1 & 2: Artikulid or **Araknid** 50/50) sleeping around the player at tick 0xc0, wake them with `sleeping_to_combat` at tick 0xa8, also drives a "stunned"/"pissedoff" sub-state machine |
| `"Jhaga"` (`s_005dfa28`) | 3 | 0x5c (23 ints) | corner-teleport priest: locates 4 `waypoint` objects, sorts them into NW/NE/SW/SE, picks one of cases 0..5, fires `cinv2` "jtele" effect, fireball / Priest_Fire_Wind, `cinv2_to_combat` exit |
| `"Yhagoro"` (`s_005dfa30`) | 4 | 0x0c (3 ints) | trivial: walk/combat root toggle by hp band (0x4c / 0x33 / 0x1a thresholds), uses `combat_to_walk` / `walk_to_combat` action block names |
| _anything else_ | -1 | (unallocated) | `default:` — `return 0` |

The caller (`AI` at `0x4c8b60`) **discards** the return value, so the
AI loop continues regardless. The `return 0` from -1 just means
"AI_PerMonster did nothing — let the generic AI handle it." That's the
Araknid path.

## What runs on an Araknid each frame

Generic `TCharacter::AI` only:

1. `WanderToWaypoint` (vt 0x4c9790) — patrols between `waypoint` objects.
2. `FindCharacters` (`meth_0x4cd690`) — looks for enemies in sight cone.
3. `BeginFighting` (`meth_0x4d3b90`) when an enemy is acquired.
4. From combat root, `RandomAttack(pcnt)` (`meth_0x4d2900`) picks an
   attack from `chardata->attacks` weighted by `attackpct`. Each attack
   is one of the entries in the Araknid char.def block:
   - `combatbite` (`pcnt 93`) — high bite, damage 10
   - `attackbitelow` (`pcnt 100`) — low bite, damage 11
   - `sidestepl` / `sidestepr` (`pcnt 25` each, `CA_PLAYANIM`) — pure animation, no damage; pure dodge
   - `jump` (`pcnt 70`, `CA_PLAYANIM`) — leap, no damage hitbox (range 35..75)
   - `backup` (`pcnt 20`, `CA_PLAYANIM`) — retreat
   - `combatf` (`pcnt 80`, `CA_PLAYANIM`) — move forward
   - `legs` / `cleaning` / `hissing` / `rearing` (`pcnt 10` each) — incidental flavor anims

So Araknid's "behaviour pattern" — sidestep dodging, leaping, backup,
incidental hissing — is **entirely data-driven** out of char.def. The
engine code does not need a hand-written Araknid branch. The same
generic `RandomAttack → DoAttack → ACTION_ATTACK` pipeline that drives
every other non-boss enemy handles all of these.

## State buffer fields used (Solifuge case 2 only)

For completeness, since the Araknid string sits inside Solifuge's case:
the Solifuge `mbr_0x284` buffer (size 0x34) is laid out as:

| offset | meaning |
| --- | --- |
| 0x00 | phase / tick (decremented) |
| 0x04 | sub-phase |
| 0x08 | tick counter (decremented; key landmarks at 0xc0, 0xa8) |
| 0x0c | hp-band (0 = >=67%, 1 = 34..66%, 2 = <=33%) |
| 0x10 | tick / used in default-RestoreMaterials path |
| 0x14 | stunned-flag |
| 0x18 | materials_saved (1 after `SaveMaterials`, 0 after `RestoreMaterials`) |
| 0x1c | minion 0 instance index (Recluse or Arakna) |
| 0x20 | minion 1 instance index (Artikulid or **Araknid**) |
| 0x24 | minion 2 instance index (Artikulid or **Araknid**) |
| 0x28 | skywalk-substate flag |
| 0x2c | pissedoff-flag |
| 0x30 | combat-mode bit |

The minions are spawned by reading their name string into a local
`TObjectDef` (`local_100`), calling `FindObjType` (`FUN_00475210`),
positioning at `(player.x ± Random(0..combat_range)*4, player.y ± ..., player.z)`,
then `TMapPane::NewObject` + initial `sleeping` action block.

Action-block name strings used in the Araknid-spawn path:
- `"sleeping"` — initial state for spawned spiders
- `"sleeping_to_combat"` — wake transition when player is close

These are standard TActionBlock names that the generic action system
already supports — no new effects, no new scripted features.

## Animation/state names referenced from this file (Araknid is none)

Just to be explicit: across all 2307 lines of `AI_PerMonster` the only
mention of "Araknid" is line 445 inside Solifuge's spawn loop. There
are no `SetDoing("Araknid…")` calls, no `Force("araknid…")` calls, no
`TActionBlock_Is("araknid…")` queries. The animation states an Araknid
visits are only the ones its `chardata->attacks` table names, which the
generic combat AI sets up via `DoAttack`.

## What the porting agent needs

For Demo 1 to run an Araknid:

1. Generic `TCharacter::AI` (`FUN_004c8b60`) operational — already in
   the migration plan.
2. `RandomAttack` / `DoAttack` paths working — already a Demo 1
   prerequisite for any combat-capable enemy.
3. `char.def` parser must read the `CHARACTER "Araknid"` block and
   attach the attack list to its `chardata`. The `CA_PLAYANIM`
   anim-only attacks need to be honored (they have damage 0 and skip
   `ResolveHit`).
4. `CHARIMPACT "cimpacthighheavy"` (flyback knockdown) and
   `"heavydeath"` reactions — generic char-impact path.

**Nothing in this monster requires effects, scripts, or systems that a
Demo 1 build wouldn't already need for any other small enemy.** The
Solifuge boss is what would gate full Araknid encounters — but
plain-spawned Araknids (e.g. via the level / mapscript) work fine on
generic AI alone.

## What we did NOT find

- No leap-attack physics special-casing.
- No "side-step dodge when player attack incoming" code in
  `AI_PerMonster`. Sidestep is just a `CA_PLAYANIM` attack the random
  picker can choose; it's circumstantial dodge-flavor, not a reactive
  dodge. (If reactive dodge exists at all in retail, it's elsewhere —
  probably driven by `FindButtonAttack` / impact-prediction in the
  generic AI.)
- No retreat-when-low-hp. Yhagoro (case 4) has hp-banded behaviour but
  Araknid does not.

## Files

- Dispatcher body: `recon/discovered/cls_0x5a7b98_TCharacter_AI_PerMonster_4c9b70.cpp`
- Solifuge-spawn helper (string class-prefix builder):
  `recon/discovered/cls_0x5a7b98_TCharacter_Araknid_StringHelper_4ce1b0.cpp`
- TMapPane::NewObject body:
  `recon/discovered/cls_0x5a7b98_TCharacter_Araknid_NewObject_450e40.cpp`
- char.def Araknid block: `data/Imagery/char.def` lines 4591..4691
