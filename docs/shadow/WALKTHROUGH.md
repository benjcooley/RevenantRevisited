# The quest as players played it

Player walkthroughs and guides for the shipped game, condensed and checked
against [STORY.md](../playtest/STORY.md), which reads the quest from the
scripts. This page gives the order players actually take, the levels and
gear they bring to each stage and boss, and what the guides say about
STORY.md's open questions (§8). Shadow Smoke uses it to order the plan and
to set readiness targets.

**Sources.** Eight guides, listed in §7 with their URLs and authors. Each
fact names its source as [W1]…[W8]. Three are complete walkthroughs written
independently: Walsh [W1], the Spanish guide [W2] and Prima's Fast Track
guide [W3]. The Czech guide [W7] follows [W3] closely, so the two count as
one source. All facts are summarised in this page's own words.

**Marks.**
- [W] one source says so.
- [W+] two or more independent sources agree.
- [?] unclear, or the sources disagree.
- *data* marks a check against the game's own files, with the file named.

Nothing here was played for this page. The guides describe the shipped retail
game. [W1] played patch 1.22. [W3] was published around release, and in two
places describes something other than what the scripts do (§6). Directions
such as "north-east" are screen directions, with north at the top of the
screen. The Keep's doors (§1.1) suggest that screen north-east is world −y,
south-west +y, south-east +x and north-west −x. That is inferred from a few
points, not measured [?].

## 1. The progression in order

### 1.0 Overview

| # | Stage (guide name) | STORY.md | Level, [W1] advice | Level [W2] had | Boss or key event |
|---|---|---|---|---|---|
| 1 | The Keep | Keep chain | 1 | 1 | Rahul |
| 2 | Misthaven: Gus, Geralt, Jong, shops | S1, S1b | 1 | 1 | — |
| 3 | The forest: Olihoot, the Bone Yard | S1b; Bone Yard not in STORY.md | 1–4 | 1 | Styxx (optional) |
| 4 | The Druhg camp | S2 | 4–5 | 3 | — |
| 5 | The Ancient Tower | S3 | 5–7 (7 urged) | 3 | Sidious |
| 6 | Gus's raft, Arakna Island | S4, S5 | 6–7 | 5 | Widowett |
| 7 | The Ogrok camp | S6 | 6–8 | 7 | Baez, barehanded |
| 8 | The First Caves (30) | S7 | 7–9 | — | — |
| 9 | Kragg Mountain (32) | S7 | 9–11 | 9 | — |
| 10 | The Infested Swamp (39) | S7 | 10–12 | 10 | — |
| 11 | The Maze of Darkness (36) | S7 | 11–13 | 11 | — |
| 12 | The Spiral Arm (31) | S7 | 11–13 | 12 | Solifuge |
| 13 | The Queen's Crypt (37), report to the Keep | S8, S8b | 12–14 | 14 | — |
| 14 | The Ruins (5) | S9 | 12–15 | 14 | — |
| 15 | The Pit (48) | S10 | 13–15 | 16 | — |
| 16 | The City of the Children (41) | S10 | 15–16 | 17 | — |
| 17 | The Underground Pyramid (44) | S10 | 15–17 | 18 | — |
| 18 | The Chase of Sabu (45) | S10 | 16–18 | 19 | — |
| 19 | The Slave Camp (46) | S11 | 17–19 | 20 | Sabu (a scripted stab) |
| 20 | The Nahkranoth Lair (49, 47) | S12 | 18–20 | 22 | Jhaga |
| 21 | Olihoot's death, Tendrick's death | S13 | — | — | — |
| 22 | The Labyrinth, Red to Purple (51–57) | S14 | 19–27 | 22–28 | — |
| 23 | Yhagoro (58) | S15 | 25–30 | 29 | Yhagoro |

Every complete guide follows STORY.md's order [W+]. The differences are
extra steps and side trips, flagged below as **Not in STORY.md**.

### 1.1 The Keep (before S1)

Covered by the Keep chain ([STORY_TESTING.md](../gameflow/STORY_TESTING.md)
§5).
- Tendrick and Sardok explain Locke's task. Sardok gives the first magic
  scroll and the Life, Soul and Moon talismans. Tendrick gives the cloth
  shirt, pants, boots and the Short Sword. [W+ W1, W3]
- Rahul the monk attacks at once. Fists or the sword both win; right-click
  items to equip them. [W+ W1, W2, W3]
- Then talk to Rand in the jail and report to Tendrick in the throne room.
  Rand dies whatever Locke says. [W+ W1, W3, W4]

**Route from the summoning room to the jail** [W1]: through the south-west
door, down the south-west hall, then south-east into a room with stairs to
the lower floor. The blue-brick floor with iron bars leads to the jail, where
Gow and Rand are. [W2] puts the jail door north-east of the Keep's main
exit, past the railings.
- *data* (exit.def, through `tools/playtest/world.py`): the summoning room's
  door `ressexit` (1190, 1120) → level 2 (9992, 8966, 576). The jail door
  `jailenter` (12793, 11345, 160) → (6280, 4338, 33), the jail, a separate
  part of level 2. `KeepExit` is at (12236, 12742).

**Route from Rand to Tendrick** (the leg the plan needs):
- [W1]: leave the jail and head north-east along the main hall to the throne
  room.
- Two side branches leave that hall. To the south-east is the teleporter
  room (TownTel0), with a chest holding an Apple and 150 gp; north of it is
  a locked chest. To the north-west is a room with 100 gp. Neither is needed.
  [W1]
- *data*: `jailexit` (6258, 4434, 32) → (12810, 11392, 161). Then a walk to
  `TendrickT` (11920, 9138, 364). That is about 2,400 units towards world
  −y, and about 200 units up, by stairs. `World.route` from RandK gives
  exactly that: `jailexit`, then walking. TownTel0 is at (13200, 10768),
  just off the way, which fits [W1]'s south-east branch.
- [W2] says the same in short: talk to the guard and Rand, then go back to
  find Sardok and Tendrick.

Other facts:
- Keep chests hold 250 gp, then an Apple with 150 gp, then 100 gp. [W1]
- The locked chest north of the teleporter room opens only with the Wyrmm
  Key from level 44 (§1.17). [W+ W1, W2]

### 1.2 Misthaven (S1, S1b)

- **First house** (west of the Keep road, down the stairs). Its chest holds
  the Letter to Styxx, the Magical Rose and 144 gp. Keep the rose for the
  Bone Yard (§1.3). [W+ W1, W2, W3]
- **Jong's dojo.** Go through the one unlocked inner door to the yard. He
  teaches the Charging Thrust first (§4.4). [W+ W1, W3]
- **The shops** [W1]:
  - Hruthford, items, in the blue-roofed building;
  - Elahni, potions;
  - Gina, weapons;
  - Cronus, armour; his shop has a locked chest that the Gothic Key opens
    later (§1.19).
  - Shop stock is in §5.3.
- **Geralt, in the library.** The shelves hold two potions and six history
  books. [W1]
- **The Inn.** Gus sits at a table downstairs. Bayne is upstairs; he is a
  multiplayer character and doesn't join. The upstairs chests hold gold and
  a Pouch. [W1]
- **Gus = S1.** He tells Locke about the tower, the caves behind the Ogrok
  camp, and the Amulet of Rogoss on Arakna. He also tells of the ring
  Harowen took. He will sail Locke to Arakna in return for the ring, then
  leaves. [W+ W1, W3, W4] The guides don't record menu digits; STORY.md's
  come from the script.
- **Geralt = S1b.** Ask about the Ancient Tower. He names the Golden Sun Key
  (the "Solaria") and Jason, whom the Druhg took. [W+ W1, W3, W4] Every
  complete guide talks to Geralt before the camp. STORY.md calls S1b
  optional, and no guide contradicts that.
- **Order varies.**
  - [W1]: Gus, Olihoot, Geralt, the Bone Yard, the camp.
  - [W3]: Jong, Geralt, Elahni, Gus, Geralt again, the forest.
  - [W4]: the Ogroks first (turned away), then Gus, Geralt, Jason.
  - Only S1 before S2 matters.

### 1.3 The forest: Olihoot and the Bone Yard

- **Olihoot.** He lives east of the town gate, past a sign. After talking he
  gives the Sky talisman [W+ W1, W2, W3], which allows Fire Flash [W3]. A
  chest north-east of his hut holds 500 gp [W1]. [W4] says he gives Earth
  and scroll 2; that is wrong, both are in the Druhg camp chest (§1.4).
- **Not in STORY.md: the Bone Yard** (optional) [W+ W1, W2, W3].
  - It lies north-east of the town gate.
  - Kill Styxx, a skeleton and the strongest early foe. The long-reach swing
    works well against him. [W1, W2]
  - Use the Magical Rose on his crypt. A chest beside it opens with the
    Emerald Ring (poison resistance). [W7] says it guards against spider
    poison, which helps on Arakna.
  - A second, locked chest to the north holds 1,000 gp and needs the tower's
    Lockpicks. [W1]
  - *data*: GRIND.md §5.6 lists RunChestE `STYXXCHEST` (4268, 23080) with the
    Emerald Ring.
- **Other early chests.** A chest on the north-west shore holds a Sausage and
  1,000 gp. [W1]
- **Barehanded practice.** Fight barehanded often here: Baez (S6) is a fistfight.
  [W3] [W6] suggests a whole level's worth of barehanded kills.

### 1.4 The Druhg camp (S2)

- **The road.** Follow the "Ancient Tower / Druhg Camp" signs. [W1] gives
  the road and two chests on the way: Greater Healing, and 500 gp with
  Greater Healing.
- **Jason.** Clear the camp (Under, Mongrel and Thrush Druhg), then talk to
  Jason at the stake. He gives the Golden Sun Key. [W+ W1, W2, W3]
- **Two chests** [W+ W1, W2, W3]:
  - Animal Fur set and Ball Racker. [W1] puts it right of the stake, [W3]
    north-east of Jason.
  - 1,500 gp, the Earth talisman, the Bone Sickle and Magic Scroll 2. [W1]
    and [W2] put it north-west of the stake.
  - *data*: these are GRIND.md §5.6's ChestS (3218, 15576) and ChestE
    (2768, 15760).
- **Agrees with STORY.md.** The guides kill the Druhg before talking; STORY.md
  notes the script doesn't require it.

### 1.5 The Ancient Tower (S3)

- **Inside.** Use the key on the door [W+]. Harowen stands inside, next to a
  chest with 340 gp; Morganna is a multiplayer character [W1].
- **Two floor pads** [W+ W1, W2, W3]:
  - the town teleporter, to the Keep: left of Morganna, blue, to the
    south-west;
  - the warp to the dragon: right of Morganna, the northern one, with a star
    on the floor.
- **Sidious** (§2.2).
- **Five chests after the fight** [W1]:
  1. Diamond Ring, Ocean talisman, Lesser Healing;
  2. Lesser Mana, 300 gp;
  3. Lesser Mana, 250 gp;
  4. 1,500 gp, Lesser Restoration, **Lockpicks**;
  5. Fenrus Collar, 1,000 gp, Meat.
- **Leaving.** Back on the warp to the lower floor (DRAGONOUT). Talk to
  Harowen again if wanted, then take the teleporter to the Keep or walk out.
  [W+ W1, W3]
- **Agrees with STORY.md.** [W6] reports a save-and-reload trick in the
  dragon's room that brings Sidious and the loot back (§6, §3 Q8).

### 1.6 The raft and Arakna Island (S4, S5)

- **Gus's house** is by the docks, south-west of the armour shop. Hand over
  the ring and he rafts Locke to Arakna. [W+ W1, W2, W3]
- **One visit only.** After leaving, Locke can't come back. [W1] STORY.md
  lists no way back either.
- **The way round.** The island is a ring; the way south is blocked, so go
  round by the north and west [W3]. [W2] calls it a spiral that can't be
  lost.
- **Chests** stand on corners and spits [W1, W2]:
  - a locked one near the start, 1,000 gp (needs the Lockpicks);
  - Tulwar and Cure Poison;
  - Amulet of Channeling and 500 gp;
  - the Woodland set;
  - more gold and potions.
- **The central islet.** A glowing pillar at the end of the path teleports
  Locke there (Lv4Tel1). Three Recluses wait, then Widowett. [W+ W1, W3]
- **Not in STORY.md: Widowett guards the amulet.** STORY.md S5 lists her
  only as one of the island's spiders. She stands between the teleporter
  and the amulet's chest, so the smoke must beat her. [W+ W1, W2, W3, W4]
  *data*: GRIND.md §3.3 puts her at (8099, 10137).
- **The chest past her** (south or south-east) holds the Amulet of Rogoss
  and the Stars talisman. [W+] Then back through the pillar to Gus, who sails
  home. [W+]
- **After Arakna** Hruthford stocks more jewellery (§5.3). That fits
  GRIND.md §7.1: more stock at MISTSTATE ≥ 4.

### 1.7 The Ogrok camp (S6)

- **Route** [W1]:
  - from the tower, go east to a rock wall (chest: Greater Mana, 1,510 gp);
  - follow the wall south-east (1,000 gp, Lesser Restoration);
  - follow the river south and south-west to a bridge;
  - south to an east-west trail, then east to the gate.
  [W2] instead follows the "Ogroks" sign east, which stands before the tower.
- **The gate.** The gatekeeper sees the amulet and opens the gate. The chief
  demands a fight without weapons against Baez. [W+ W1, W2, W3]
- **Reward.** The Soul Cleaver, and the chief's promise of help. [W+]
  [W3] and [W7] call it an axe; [W8] and `weapon.def` (GRIND.md §5.4) make
  it a sword, Damage 45.
- **Camp chests** [W1]:
  - three with 1 gp and food or potions;
  - across the bridge south-east of the arena: Band of Regeneration and
    Magic Scroll 3 (*data*: GRIND.md's ChestS (13312, 17520));
  - one outside the cave mouth.
- **The caves** lie south-east of the camp. [W+ W1, W2]

### 1.8 The caves (S7)

**Level 30, the First Caves.**
- Olihoot waits at the entrance with the Sun talisman. [W+ W1, W2, W3] A town
  teleporter stands just west (towntel2). [W+ W1, W2]
- Fire geysers burn. [W+ W1, W3]
- Chests off the trail hold Red Scalemail pieces; a green dragon guards the
  chest plate. [W+ W1, W2]
- A graveyard with skeletons comes near the end. The exit is at the end of
  the winding trail to the north-west. [W1]

**Level 32, Kragg Mountain.**
- *Optional* side cave. A hidden entrance at the mountain's foot leads to a
  chamber with two green dragons and chests. They hold the Celtic Leg Plate
  [W3], plus the Silver Scalemail set [W2]. This is probably the
  `Lv32ENT2`/`Lv32EX2` pair [?].
- The Blackened Axe and the Amulet of Thunderbolts are in chests. [W+ W1, W3]
- A town teleporter stands up a staircase off the trail (towntel3). [W+ W1,
  W3]
- **The exit**:
  - [W4]: down a stairway just south-east of the teleporter;
  - [W2]: on the map's north side, a little east of the teleporter;
  - [W1]: past the teleporter, east, down three flights, north-west, up one
    flight north-east, then west.
  - *data*: `Lv32EX` sits at z 248, below towntel3 at z 525, which fits a
    way down.

**Level 39, the Infested Swamp.**
- Siroccos (fast red ninjas) attack at once. Block until they tire, then
  strike. [W1]
- **Not in STORY.md as a route; optional:** two small "fire caves" and a
  mushroom ramp [W+ W1, W2, W3].
  - The first cave holds the **Bottle of Spores** and a Greater Restoration.
  - The second holds the Celtic Boots, and its far exit comes out higher up.
  - Near that exit stands an odd machine. Use the Bottle of Spores on it and
    a mushroom ramp grows.
  - The ramp leads to a ledge in the forest (level 0) with an otherwise
    unreachable chest: the Blue Scalemail set, about 2,500 gp, potions.
  - *data*: the ledge is LOCASH (39) → level 0 (9760, 16222). The chest is
    GRIND.md §5.6's ChestS (9820, 16180), and FAIRYENT is the way back down.
    The mapping is inferred from positions [?].
  - The cave pairs are presumably the `Lv39ENT*`/`Lv39EX*` jumps [?].
- **Main route** [W1, W2]:
  1. Back to the start, take the south path, then west.
  2. Go down the stairs south to a lily-pad path east. Its chest holds
     Greater Mana and **Magic Scroll 4**.
  3. Back to the double staircase, north-west past the tombs, then south and
     east.
  4. A journal scroll lies on the ground, and the exit is at the end.
  So the fire caves are off the required path. [W+]

**Level 36, the Maze of Darkness.**
- It is almost dark. Follow the yellow sparkles ("pixies"); at each fork,
  the way to the exit has them. [W+ W4, W6]
- [W1] gives a route to every chest, the town teleporter and the exit. The
  exit room's chest holds 500 gp, Greater Healing and the Celtic Helmet.
  [W2] draws a map.
- Spider Queens appear here. [W+]
- [W3] uses the teleporter for a town trip: Jong, Barbarian armour and the
  Broadsword at level 9.

**Level 31, the Spiral Arm.**
- **The lake room.** A winding passage leads to a room with a lake and five
  gates in a row across one corridor. Up the stairs to the north-west, a
  chest holds 25 Ice Arrows. [W1] [W3] puts a longbow there too.
- **All five levers are needed.** [W+ W1, W2, W3, W4]
  - Each corridor ends in a lever, and each lever opens one gate.
  - The order doesn't matter.
  - Druids and Zombies guard them.
  - This settles STORY.md S7's open point: the `Lv31Lever*`/`Lv31Gate*`
    gates are on the required path.
- **[W1]'s order:**
  1. the lava area south-west (chest: Ranger Sword);
  2. west;
  3. the swamp area north;
  4. north-east along the lake past the gates, then over the lake bridge
     (Cestus of Slicing and Celtic Chest Plate chests);
  5. south.
  [W4] names the sections by colour; [W2] maps the levers A–E.
- **The Solifuge's room.**
  - Past the gates come Spider Queens, then a gate that opens with a plain
    click. [W+ W3, W7]
  - Save before entering: the door shuts behind Locke [W4]. That is
    SOLDUMMY.
  - A chest along the south-east (right) wall holds a Longbow, 25 Ice Arrows
    and a Greater Restoration. [W+ W1, W2, W5]
  - Hugging the walls reaches it before the Solifuge wakes [W5]. The fight
    is in §2.2.
- **The key.** The Solifuge drops the "**Copper Key**". [W+ W1, W2] Use it
  on the portcullis in the room's north-west corner [W1], or the west door
  [W2]. A short path beyond leads to the Queen's Crypt.
  - Agrees with STORY.md: key 886 opens InportEW (628, 9222).
  - The name "Copper Key" is [?]: STORY.md gives shegra1's key 44 the same
    name.
- **Not in STORY.md S7: the bow.** The guides treat the bow and ice arrows
  as the way to win. [W+]

### 1.9 The Queen's Crypt and the report (S8, S8b)

- **The vision.** Locke sees his past, faints, and wakes before Andria's
  spirit; she answers "in this life". [W3, W7] [W1] places the vision at the
  raised platform.
- **Chests** [W+ W1, W2]:
  - south-west of the platform: Magic Scroll 5;
  - north: Greater Mana, Greater Healing, Spirit Pendant;
  - south-east: 1,000 gp.
- **The dagger.** [W3] puts the Sacrificial Dagger in the dais chest with
  the scroll, and says to keep it. [W1] and [W2] don't mention it. STORY.md
  has it added by ANDRIA4's scene (`cave.s:114`).
- **Sardok.** He appears at the north-east exit, beside a town teleporter
  (towntel5). He could have teleported Locke here all along, and tells him
  to report to the Keep. [W+ W1, W2, W3]
- **The report (S8b).** Every complete guide then reports to Tendrick and
  Sardok, shops, and returns by the teleporter to the cave exit. [W+ W1, W2,
  W3] Optional visits on the way: Geralt about the Ruins [W1], Olihoot [W5].
  STORY.md calls S8b optional; the guides don't contradict it.

### 1.10 The Ruins (S9)

- **The way in.** North-east over two bridges. Navarro Al Xace is there; he
  is no use. [W+]
- **Foes:** Spider Queens, Druids, Zombies, White Dragons, and Wraiths.
  Wraiths shrug off weapons but fall to magic [W3, W7]. Statue heads shoot
  fire [W3].
- **What to collect** [W1, W2]:
  - outside the maze, left (north-west): the **Root**;
  - outside, right (south-east): an **Empty Vial**, then the **Blue Flower**
    north-east of it, guarded by a White Dragon;
  - in the maze: an **Empty Vial** and the **Water Vial**, plus the Valkyrie
    Sword, potions and gold;
  - in the graveyard:
    - the **Bone** (north corner);
    - 5,000 gp and the Ring of the Arcane;
    - the **Issathi Claw** (south-east, then south-west);
    - the **Acid Vial** (north-east end, two Wraiths);
    - an **Empty Vial** with 1,000 gp (the Wraith room);
    - the Nightshade set;
  - by the wide stairs before the altars: an **Empty Vial**.
  - A chest near the altars holds the Gate Portal Scroll, which explains the
    offering.
- **The count.** Four Empty Vials, the Water and Acid Vials, and four
  ingredients. [W+ W1, W2, W3] [W5]'s "five empty vials" is wrong. Agrees
  with STORY.md's data.
- **The ritual** [W+ W1, W2, W3, W5]:
  1. In the pack, drag each ingredient onto an Empty Vial.
  2. Drag one vial onto each of the six altars. The order doesn't matter
     [W2].
  3. The force field over the exit vanishes.
- **Beyond:** a zombie, a White Dragon and a staircase. The dungeon entrance
  is at the top of the stairs. [W+ W1, W2, W3] That is RUINSEXIT (§3 Q1).

### 1.11 The Pit (48), S10

The guides agree on the order. [W+ W1, W2, W3] The STORY.md object each step
appears to be is inferred from positions [?]:

1. South-east from the arrival: the Jhaga and Sabu cutscene (Jhaga1's cube).
2. A room with the town teleporter (TOWNTEL6) and a chest.
3. Down the stairs, past arrows from the walls, to a big room. On its central
   island stands a locked building (`Lv48D1`, 9737, 9794).
4. Two rooms with Druids:
   - south-west: Blackened Morningstar, and a locked gate down to the
     basement;
   - north-east: the Ward talisman and Magic Scroll 6, and another locked
     basement gate.
   Spike traps line the corridors.
5. Through the south-east garden to the south-west corner (White Dragon,
   locked chest with 1,500 gp). Then a red warp tile, guarded by a Dark
   Revenant, leads to an island with three warp tiles:
   - one goes back;
   - the left one goes to an islet chest with **Temple Key #1** (ChestS
     (11798, 9902), key 52374);
   - the right one goes to an islet with a **lever** (`Lv48Lev6`,
     11686, 9218).
6. That lever unlocks the basement gate. [W1] means the Ward-talisman room's
   gate; [W2] says it opens both. STORY.md: `Lv48Lev6` unlocks `lv48d4`.
7. **The basement.** A six-way room with a Dark Revenant and 2,000 gp on the
   dais. North-west is a temple guarded by an Earth Golem. Temple Key #1
   opens its door (`LV48D8`, key 52374).
8. **The temple.** Take the centre path to a White Dragon. A lever in the
   north-west corner (`Lv48Lev88`) unlocks the island building. A warp tile
   there leads to the Ninja armour (+2 Agility [W1]).
9. **The island building** (`Lv48D1`). Up the stairs, a chest holds the
   Black Dragon set and **Temple Key #2** (ChestS (15142, 14952), key
   47325).
10. Back in the temple, Temple Key #2 opens the gate at the foot of the
    stairs (`Lv48D5`). A Druid waits; the chest holds the Holy Boots and
    500 gp. A warp tile leads to the City of the Children (`LV48EXIT`).

[W3] tells the same beats but frames step 6 differently: the islet key
doesn't fit the portcullis, and the lever opened it instead.

### 1.12 The City of the Children (41)

- **No keys or levers.** [W+ W1, W2, W3]
- **Optional rooms** [W1, W2, W3]:
  - the Torturer;
  - the Ogrok Chef, who says the creatures serve under a control spell;
  - Wanet the priest (Holy Gauntlets), who claims to work against the cult;
  - an armoury with the **Law talisman** [W1]; [W2] puts Law in the second
    Chamber of Horrors instead (§6);
  - a raised platform with the Gauntlets of Chaos.
- **Town teleporter** (TOWNTEL7). [W+]
- **Route** [W1]:
  - across the water north-west;
  - Shadow Minions, who turn invisible (run until they show again);
  - a chain of warp tiles; one side trip ends at an altar with the
    Flamberge;
  - a long room with two White Dragons;
  - the exit gate on its north-east wall (`Lv41D12`).
  [W2]'s version: reach the south part of the map.

### 1.13 The Underground Pyramid (44)

- A town teleporter at the start (TOWNTEL8). [W+]
- Down the stairs, north-east, through a gate: a chest holds the **Wyrmm
  Key**. [W+ W1, W2, W3]
- **Not in STORY.md (optional): the Wyrmm's Tooth Sword.** Teleport to the
  Keep and open the locked chest north of its teleporter room (§1.1). It
  holds the Wyrmm's Tooth Sword: Damage 150, MinStr 25 [W8]. [W+ W1, W2, W3]
  *data*: GRIND.md §5.4 lists the level 2 Chest (12705, 10333), key 33.
- **On to the pyramid** [W1, W2, W3]:
  1. A warp tile, then south-west past arrow traps, Dark Ogroks and
     Dorogars. [W1] calls the Dorogar the hardest ordinary foe; [W3] uses
     Lightning Bolt and Fireball on them.
  2. A Bastard Sword lies on an altar.
  3. A small room to the south-east holds the Holy Leg Plate.
  4. Then the pyramid: one long switchback path, a chest on most tiers (up
     to 5,000 gp), and the Dragon Eye Pendant at the top.
  5. A warp at the top leaves the level (`Lv44Exit`, z 784).

### 1.14 The Chase of Sabu (45)

The guides' order matches STORY.md's events one for one. [W+ W1, W2, W3]

1. Kill two Dark Ogroks. Their chest holds the Door Key and 2,000 gp. The
   Door Key opens the gate (key 55, InportNS (9460, 9995)).
2. First door on the north-west: Sabu taunts Locke and teleports away
   (Sabu1). Flip the switch (`Lv45L5`); the chest beside it holds the
   Ninja-To.
3. A room with pools: Arcanists, who freeze and paralyse. Then the four-way
   room: Sabu throws a switch, leaves to the north-west and locks the door
   behind him (Sabu3). Its chest holds the Golden Morningstar.
4. To the south-east, three Ogroks chat, then fight: Mort, Urgg and Slagg
   (Mort's cube).
5. Two Shadow Minions guard the Earthen Gauntlets.
6. The burial chamber. A ghost (Horatio) calls Locke king and accuses him.
   The chests hold the Hallway Key, the Exit Door Key and Magic Scroll 7.
   [W3] adds an amulet against Sabu's magic, STORY.md's Wolfsbane Pendant.
7. The exit key opens the gate back to the four-way room (key 66). The
   Hallway Key opens Sabu's door (key 77, `Lv45D5`).
8. Sabu sets two War Kanthas on Locke (Sabu4). Later his blast fails (Sabu5;
   [W3] credits the amulet). A Stone Golem, then the warp to the Slave Camp.

### 1.15 The Slave Camp (46), S11

- **Not in STORY.md: a long maze before Sabu.** [W+ W1, W2, W3]
  - It runs from the arrival (3096, 12984) to Sabu's room near
    (10001, 9458).
  - Foes: Stone Golems, Burning Praetors, traps. Avoid the middle path at a
    three-way fork; it is full of traps [W2].
  - Many chests: the **Gothic Key**, the Holy Helmet (completing the Holy
    set), the Magic Sceptre, up to 5,000 gp.
- **Capture.** Sabu, with two Kanthas, sends Locke to the slave camp
  (Sabu6). [W+]
- **Town trip.** A town teleporter stands just south-east of the arrival
  (TOWNTEL9) [W1]. The guides use it to open Cronus's chest with the Gothic
  Key (the Gothic set) and the Keep's Wyrmm chest. [W+ W1, W2]
- **The camp.**
  - Along the first corridor: two locked portcullises on one side, two open
    ones on the other, with three Dark Ogroks behind the open ones. [W+ W2,
    W3]
  - The slaves dig in the green-floored room. They won't flee while Sabu
    lives. [W+]
  - North-east: War Kanthas, stairs, then Shegra in a side room. [W1, W3]
- **Shegra.** She asks for a kiss; agree. She gives the poisoned dagger.
  [W+] Where the Entrance Key comes from is in §3 Q4.
- **Sabu dies.** Use the Entrance Key on the gate at the corridor's
  south-west end. A cutscene shows Locke stabbing Sabu. [W+ W1, W2, W3]
- **After the stab.** Kill the Kanthas, then **re-equip the weapon**: [W2]
  had to equip the sword again. Loot two chests (3,000 gp with Bread;
  2,000 gp with Meat), and take the warp back to the camp. [W+ W1, W2, W3]
- **Telling the slaves.** Tell them Sabu is dead. [W+]
- **The way on.** The locked room near the town teleporter holds the warp to
  the Nahkranoth Lair (`LV46T3`). [W+]
  - [W1] and [W2] open it with the **Entrance Key**.
  - [W3] says the slaves hand over a key they stole.
  - That fits STORY.md's data: the entrance key and the guardroom key are
    both key 44. So the druhg's guardroom key is a spare.

### 1.16 The Nahkranoth Lair (49, 47), S12

- From the start, go north-east to a warp tile (`Lv49bTel2`). Jhaga waits
  beyond. [W+ W1, W2] The fight is in §2.2.
- Afterwards Locke is moved away and sees Andria. [W+ W1, W3]
- **Chests** [W+ W1, W2]:
  - 1,000 gp, Enchanted Healing, Magic Scroll 8;
  - the Alpha Gem Necklace and 1,000 gp.
- **The throne.** Across a bridge and up the stairs sits a skeleton on a
  throne. Locke reaches for something and passes out. [W+ W1, W3, W7] That
  is the NAKRA altar, which needs Locke up on it (STORY.md S12).
- **Waking.** Locke wakes at Olihoot's shack with the Nahkranoth set in the
  pack. It has no helmet; each piece gives +15 % health and mana [W1].

### 1.17 Olihoot and the Keep (S13)

- **Olihoot.** He tells more of Locke's past. Before he can explain Sardok,
  Sardok arrives, kills him, and summons Locke to the Keep. [W+ W1, W3]
  Both guides treat the talk as the next step; STORY.md marks it optional.
- **Town.** Hruthford has new stock (§5.3) [W1]. The townsfolk react to
  Locke's darkened arm, and Jong says he can't trust him [W3]. Jong still
  teaches afterwards: [W1] used the level-24 move against Yhagoro [?].
- **The Keep.** Sardok kills Tendrick. Only Sardok can take Locke to
  Anserak, and he does. [W+ W1, W3, W7]

### 1.18 The Labyrinth (51–57), S14

- **The goal.** Each of the seven levels holds a crystal. Carry it onto that
  level's glowing circle and one of seven gates opens. [W+ W1, W2, W3]
  - Locke places the crystal by himself [W2]; that is the marker cube.
  - A note from Sardok in the first level explains it [W3]. If a crystal is
    missed, Locke has to come back for it [W3].
- **Per level** [W1, W2]:
  - **Red (51):** a town teleporter; the Red Crystal in the middle chest of
    three; Magic Scroll 9 and the **Death** talisman; the Berserker Sword.
  - **White (52):** the **Hilt**; the Amulet of Power and the **Chaos**
    talisman (a Blue Dragon guards them); the White Crystal, past a Stone
    Golem.
  - **Green (53):** the **Handle** and Magic Scroll 10; the Wyrm Tooth
    Necklace; the Dwarven Axe; the Green Crystal; a teleporter near the
    exit.
  - **Blue (54):** the Blue Crystal, guarded by a Red Dragon; the Barbarian
    Sword; a teleporter.
  - **Orange (55):** the Orange Crystal, guarded by a Red Dragon and an
    Undead Sorcerer; the **Blade**.
  - **Yellow (56):** a teleporter; the Yellow Crystal in the middle of a big
    room.
  - **Purple (57):** the circle near the start; the Purple Crystal.
- **Angsaar.** Combine the Hilt, Handle and Blade in the pack to make it:
  Damage 200, MinStr 23 [W8]. [W+ W1, W2, W3]
- **The way out.** After the seventh crystal, walk north-east through the
  seven open gates to the exit. [W+ W1, W3]
- The crystal colours by level match STORY.md's table.

### 1.19 Yhagoro (58), S15

- A corridor "in space" with a town teleporter (TOWNTEL14). Then stairs up
  to Yhagoro on his throne. He talks, then fights. [W+ W1, W2, W3]
- The fight is in §2.2.
- **The ending.** It plays after his death. [W+] No guide mentions the
  Sacrificial Dagger here.

## 2. Readiness

### 2.1 Levels

- The level columns of §1.0 give [W1]'s advice and the level [W2]'s author
  had on arrival.
- [W2] ran about two levels under [W1]'s advice early on and one to three
  over it late.
- Other data points:
  - [W4] beat Sidious at level 8;
  - [W5] was about level 16 after the Solifuge;
  - [W1] urges level 7 for Sidious, or as high as Locke can get.
- [W1]'s experience table matches GRIND.md §1.3 exactly. Level 30 is the
  cap, and [W1] says it is reachable in normal play.
- **Stat points** [W+ W1, W3, W6]: two points per level up to 15, then one.
  Put the early ones into Strength and Constitution: armour needs both, and
  weapons need Strength.

### 2.2 Bosses

| Boss | HP (guides) | Level | How players win | Bring |
|---|---|---|---|---|
| **Sidious** (tower, S3) | 800 [W+ W1, W2] | 5–7, 7 urged [W1]; 8 [W4]; ≥ 5 [W2] | Dodge the fire breath: it hurts and stuns. Block [W1, W2]. Soften him with spells and keep Regeneration up [W3]. | ~10 healing potions [W2]; Regeneration (Earth), Might, Fire Flash (Sky). [W2] also lists Swift Strike, which needs Ocean, the dragon's own loot. |
| **Widowett** (Arakna, S5) | — | 6–7 [W1]; 5 [W2] | Kill the Recluses first [W3]. Sidestep her poison bolts, close in, back off [W2, W3]. She summons spiders until her mana is gone [W2]. Easier than Sidious [W1]. | Fire Flash [W3]; Might, Swift Strike [W2]; Bone Sickle or Tulwar [W3] |
| **Baez** (arena, S6) | — | 6–8 [W1]; 7 [W2] | No weapons, but spells are allowed. [W2]: Regeneration, then Quicksand again and again (80–90 a cast), kicking while he is stuck. [W3]: Fist Mastery, Might, Regeneration and Quicksand on the hotkeys. | Hand skill 3 [W1]; a level of barehanded kills and the Poison spell [W6]; a belt full of health and mana potions [W3] |
| **Solifuge** (31, S7) | 5,000 [W+ W1, W2]; ~4,500 [W4] | 12–14 [W1]; 12 [W2] | Ice arrow to freeze it, then melee combos, then back off and freeze it again [W+ W1, W2, W3, W5, W6]. X draws the bow, Z fires, C goes back to melee [W+ W1, W2, W5]. At half health it leaps and drops on Locke; watch its shadow. It calls small spiders [W2, W3]. | The Longbow and arrows from the room's chest. [W5] used about 20 arrows. Drink potions in the lower-left alcove [W6]. |
| **Sabu** (46, S11) | — | 17–19 [W1] | Not fought. The poisoned dagger kills him in the cutscene [W+]. His earlier blast fails (Sabu5) [W1, W3]. | The Entrance Key and the dagger |
| **Jhaga** (49, S12) | 12,000 HP, 6,000 mana [W2] | 18–20 [W1]; 22 [W2] | He teleports between fixed points, casts a bolt on each jump and summons Dark Revenants [W+ W1, W2, W3]. Stand mid-room, dodge, and hit him when he lands there [W1]. Out of mana he is helpless [W2]. | A strong sword for the Revenants; [W1] used the Wyrmm's Tooth |
| **Yhagoro** (58, S15) | 15,000 [W+ W1, W2]; mana 10,000 [W1] / 9,999 [W2] | 25–30 [W1]; 29 [W2] | Green magic bolts; telekinesis that throws objects; Nullifier, which cancels Locke's spells [W1, W3]. Hard melee too [W1, W3]. Stay close and use the Dragon and Spider combos; he casts less up close [W1]. Invisibility leaves him unable to hit or dodge [W1]. | [W2]: Ogre Strength, Invisibility, combos, Ice Bolt. [W3]: Ice Bolt and Full Paralysis, then combos; Fireballs when tired; potions freely. |

Lesser fights the guides single out:
- White Dragons: Physical Paralysis, cast several times [W3]; Quicksand
  [W5].
- Druids: wait until they are visible again [W3, W5].
- Wraiths: magic [W3, W7].
- Stone Golems: Quicksand or Swamp Pit, then Electric Bolt [W2]; bludgeons
  [W3].
- Red Dragons: Invisibility [W2].
- Spider Queens: dodge sideways, strike once, back off [W5].

### 2.3 Gear by stage

Weapon damage is [W8]'s, which matches `weapon.def` (GRIND.md §5.4).

| Stage | Found | Shop buys the guides make |
|---|---|---|
| Start | Short Sword (13), cloth | Leather from Cronus [W1] |
| Druhg camp | Animal Fur set, Ball Racker (18), Bone Sickle (22) [W+] | — |
| Tower | Fenrus Collar, Lockpicks [W1] | — |
| Arakna | Tulwar (35), Woodland set, Amulet of Channeling [W1] | Chainmail at level 6, "a very good buy" [W2]. [W3] suggests Bone armour, but it needs Str 19 / Con 18 and comes at level 9 (GRIND.md §5.5, §7.1), so not yet. |
| Ogrok | Soul Cleaver (45) | — |
| Caves | Red Scalemail, the Celtic pieces, Blackened Axe (47), Amulet of Thunderbolts, Ranger Sword (65), Cestus of Slicing | Broadsword (60) and Barbarian armour at 9 [W3, W4]; Claymore (73) at 12 [W2, W3] |
| Ruins | Valkyrie Sword (75), Ring of the Arcane, Nightshade set | — |
| Pit | Ninja set, Black Dragon set, Holy Boots, Blackened Morningstar (80) | — |
| City | Flamberge (90), Gauntlets of Chaos, Holy Gauntlets | Flamberge and Katana at 15 [W1, W2] |
| Pyramid | Wyrmm's Tooth Sword (150) through the Keep chest; Bastard Sword; Holy Leg Plate; Dragon Eye Pendant | — |
| Chase of Sabu | Ninja-To (105), Golden Morningstar (100), Earthen Gauntlets, Holy Chest Plate | — |
| Slave Camp | Holy Helmet (the full Holy set gives +15 % health [W1]), Magic Sceptre (110), Gothic set | — |
| Nahkranoth | the Nahkranoth set, Alpha Gem Necklace | — |
| Labyrinth | Berserker Sword (125), Amulet of Power, Dwarven Axe (135), Angsaar (200) | — |

Advice on gear:
- [W1]: keep Sword as the main weapon skill; the best weapons are swords.
- [W5]: always carry the best available weapon, bought if need be.
- [W5]: prefer the Barbarian pieces (damage bonus, no Agility penalty) to
  heavier chest and leg armour.

### 2.4 Potions

- Two or three Lesser Mana before the forest. [W3]
- Ten or more Lesser Healing before Arakna. [W2]
- About ten Lesser Mana before the Ogroks. [W2]
- Stock up before the tower, the caves and the Solifuge. [W1, W4]
- Elahni's prices [W1] match GRIND.md §7.1:
  - Lesser Healing or Mana 250; Lesser Restoration 500;
  - Greater Healing or Mana 900; Greater Restoration 1,750;
  - Enchanted Healing or Mana 1,500; Enchanted Restoration 2,750;
  - Cure Poison 100.

## 3. STORY.md §8, answered where the guides settle it

**Q1. The Ruins exit.** [W+ W1, W2, W3]
- With all six vials placed, the glowing field over the exit portal goes.
  Locke walks through.
- A staircase climbs past a zombie and a White Dragon, and the dungeon
  entrance is at the top.
- This fits STORY.md's geometry: RUINSEXIT's cube at z 500–600 sits up from
  the pedestals at z 155, and its ledge is walkable.
- [?] Whether stepping into WarpRuin moves Locke, or `barruin` only blocked
  a walkway, is unclear. [W1] says "portal", [W3] "force field", and none
  mentions a jump. For the nav graph: `barruin` is a blocker that the six
  pedestal states remove.

**Q2. Making vials.** [W+ W1, W2, W3, W5] In the pack, drag an ingredient onto
an Empty Vial; the filled vial results. Then drag each vial onto an altar,
in any order [W2]. This agrees with STORY.md's reading of `class.def`. The
guides can't speak to the port. The same drag-onto-item move builds Angsaar
(§1.18), so the port needs inventory item-on-item combining for both.

**Q3. The arena.**
- No guide says which items Baez's fight takes, or how Locke leaves the
  ring.
- [W1] goes straight on to loot the camp, and no one reports being stuck.
  That is a weak hint that the ring doesn't trap Locke in retail [?].
- [W4] says Locke keeps the amulet. That contradicts WORTHY's `DELINV`
  (STORY.md S6), and is probably loose wording [?].

**Q4. Shegra's twin.**
- No guide notices two Shegras.
- [W1] and [W2] talk to "Shegra" and then open the gate with the Entrance
  Key. Neither reports a missing key, but neither says where the key came
  from.
- [W3] says the Entrance Key lies in a chest in Sabu's castle before the
  capture, and calls it careless of Sabu to leave it about. [W7] repeats
  [W3].
- [W1]'s full chest list for level 46 has no such chest.
- So [?]: the scripts give the key through Shegra (STORY.md), and [W3] may
  describe a pre-release layout.
- Keep STORY.md's plan: talk to `shegra1`, then check the key is in the
  pack.

**Q5. Jason's twin.**
- No guide mentions two Jasons.
- [W4] had cleared the camp earlier and found no Jason there. After talking
  to the librarian, Jason was at the stake.
- [W4] saw Gus before Geralt, so this is consistent with jason1 appearing at
  MISTSTATE 2. It also suggests the scriptless twin isn't noticeable in
  retail [?].

**Q6. Routes inside levels.**

| Level | What the guides say | Mark |
|---|---|---|
| 30 | Linear with side branches; exit at the end of the north-west trail | [W1] |
| 32 | The side cave (probably `Lv32ENT2`/`EX2`) is optional; the exit is down a stair near the teleporter | [W+] / [?] for the id |
| 39 | The fire caves and the spore ramp (LOCASH/FAIRYENT) are optional; the exit is via the south and west paths | [W+] |
| 36 | Follow the yellow sparkles | [W+] |
| 31 | All five levers are needed, in any order; then the Solifuge's key opens the way out | [W+] |
| 48 | Islet lever and Temple Key #1, basement, temple door, temple lever, island building and Temple Key #2, temple gate, warp (§1.11) | [W+] for the order; [?] for the ids |
| 41 | No keys or levers; a chain of warp tiles to the north-east gate | [W+] |
| 44 | The Wyrmm Key is optional; a warp tile, then the pyramid climb to the top warp | [W+] |
| 45 | Door Key 55, Sabu1 and the lever, Sabu3, Mort, Horatio and keys 66/77, `Lv45D5`, Sabu4, Sabu5, exit (§1.14) | [W+] |

**Q7. The Sacrificial Dagger.** [W3] says to keep it. No guide uses it later,
and the ending follows Yhagoro's death directly. [W+, from silence] This
agrees with STORY.md: no script reads it.

**Q8. Health `= 0` checks.**
- The guides can't settle this.
- [W6] does report that saving in the dragon's room near its centre, after
  the kill and the looting, and reloading, brings Sidious and his loot back.
  A variant reportedly restarts the game but keeps the items. Retail plainly
  mishandles saves around this fight.
- Treat the "don't save between Sidious's death and the state change" trap
  (STORY.md S3) as real.

**Q9. The labyrinth barriers.** [W+ W1, W2, W3] All seven gates stand on the
way to the exit. Players walk through them after the seventh crystal. This
confirms STORY.md's "likely".

**Q10. World data gaps.** Nothing in the guides. No guide mentions a Spirit
Guardian.

## 4. Spells and talismans

### 4.1 Where each talisman is

| Talisman | Where | STORY stage |
|---|---|---|
| Life, Moon, Soul | From Sardok at the start [W+] | Keep |
| Sky | Olihoot in the forest [W+] | S1b, optional but advised |
| Earth | The Druhg camp chest with scroll 2 [W+] | S2 |
| Ocean | The tower's first chest, with the ring [W+] | S3 |
| Stars | Arakna, the amulet's chest [W+] | S5 |
| Sun | Olihoot at the cave mouth (level 30) [W+] | S7 |
| Ward | The Pit, the north-east room's chest [W+ W1, W2] | S10, 48 |
| Law | The City of the Children: the armoury [W1] or the second Chamber of Horrors [W2] [?] | S10, 41 |
| Death | Red Labyrinth [W+ W1, W2] | S14, 51 |
| Chaos | White Labyrinth [W+ W1, W2] | S14, 52 |

### 4.2 The spell scrolls

The scrolls list recipes; they aren't needed to learn spells (§4.3).

| Scroll | Where [W1] |
|---|---|
| 1 | Sardok, at the start |
| 2 | Druhg camp chest |
| 3 | Ogrok camp, across the bridge (with the Band of Regeneration) |
| 4 | Infested Swamp, the lily-pad chest |
| 5 | Queen's Crypt |
| 6 | The Pit, with the Ward talisman |
| 7 | Chase of Sabu, Horatio's room |
| 8 | Nahkranoth Lair, after Jhaga |
| 9 | Red Labyrinth, with Death |
| 10 | Green Labyrinth, with the Handle |

### 4.3 Learning and casting

- **Learning.** Pick talismans in the spell pouch (key 1) and press cast. A
  correct combination joins the known list even when the cast fails, the
  mana is short or the Invoke skill is too low. [W+ W1, W2] No scroll is
  needed. [W+ W1, W4]
- **Casting** [W1]:
  - Invoke more than two below the spell's skill: Locke can't cast it.
  - One or two below: casts sometimes fail.
  - Above: casts rarely fail.
- **Invoke rises** with each successful cast, and faster with harder spells
  [W2].
- **Order matters** (*data*). The game's recipes are ordered strings, and
  several spells share a set of talismans in a different order:
  - Heal (Life, Soul, Sun) and Blood Drain (Soul, Sun, Life);
  - Charm (Stars, Sun) and Fireball (Sun, Stars);
  - Tornado (Earth, Stars, Sky) and Armageddon (Stars, Earth, Sky);
  - Advanced Healing (Law, Soul, Life) and Restore Life (Life, Soul, Law);
  - Neural Paralysis and Aura.
  
  The port looks spells up by ordered string (`src/spell.cpp:371`). [W2]
  says only the right order teaches a spell. [W1] lists sets, and gives
  Charm and Fire Ball the same set. Whether retail's pouch compares in order
  should be checked on retail [?]. Until then, the smoke should click
  talismans in the order below.

The recipes below are from `Resources/spell.def` (*data*). "Usable from"
means the stage at which Locke holds every talisman. The guides' recipe
tables [W1, W2, W4] agree with these except where §6 says otherwise.

| Spell | Talismans, in order | Invoke skill | Mana | Usable from |
|---|---|---|---|---|
| Poison | Moon | 1 | 42 | start |
| Fire Flash | Moon, Sky | 1 | 31 | Sky |
| Nourish | Life | 2 | 40 | start |
| Cure Poison | Life, Sky | 2 | 25 | Sky |
| Might | Soul | 3 | 31 | start |
| Fist Mastery | Life, Soul | 3 | 50 | start |
| Meteor Storm | Earth, Sky | 4 | 53 | S2 |
| Quicksand | Earth, Ocean | 4 | 71 | S3 |
| Regeneration | Life, Earth | 5 | 107 | S2 |
| Swift Strike | Ocean, Moon | 5 | 61 | S3 |
| Stone Skin | Moon, Earth | 6 | 125 | S2 |
| Anti-magic | Stars, Sky | 6 | 53 | S5 |
| Dexterity | Soul, Stars | 6 | 53 | S5 |
| Swamp Pit | Ocean, Life, Earth | 7 | 104 | S3 |
| Tornado | Earth, Stars, Sky | 7 | 78 | S5 |
| Quicksilver | Sky, Ocean, Earth | 8 | 228 | S3 |
| Speed | Soul, Life, Stars | 8 | 78 | S5 |
| Physical Paralysis | Life, Moon, Soul | 9 | 78 | start |
| Shadow Fist | Stars, Moon | 9 | 125 | S5 |
| Electric Bolt | Sun, Sky | 10 | 105 | S7 |
| Fire Wind | Sun, Soul, Stars | 10 | 128 | S7 |
| Heal | Life, Soul, Sun | 11 | 140 | S7 |
| Charm | Stars, Sun | 12 | 105 | S7 |
| Teleport | Ocean, Sky, Stars | 12 | 84 | S5 |
| Fireball | Sun, Stars | 13 | 135 | S7 |
| Blood Drain | Soul, Sun, Life | 13 | 179 | S7 |
| Mana Drain | Moon, Soul, Stars | 14 | 135 | S5 |
| Warrior Born | Sun, Moon, Stars, Sky | 14 | 135 | S7 |
| Invisibility | Sky, Soul | 15 | 108 | Sky |
| Rock Storm | Earth, Sky, Law | 16 | 167 | 41 |
| Cataclysm | Law, Life, Earth, Sky | 16 | 222 | 41 |
| Iron Skin | Ward, Earth, Moon | 17 | 267 | 48 |
| Troll Blood | Law, Life | 17 | 333 | 41 |
| Neural Paralysis | Ward, Soul, Life, Stars | 18 | 167 | 48 |
| Ice Storm | Ocean, Life, Law | 19 | 256 | 41 |
| Napalm | Sun, Sky, Law | 19 | 192 | 41 |
| Ogre Strength | Earth, Moon, Law, Ward | 20 | 192 | 41 |
| Nullifier | Ward, Soul | 21 | 192 | 48 |
| Weapon Mastery | Moon, Sun, Soul, Sky | 21 | 192 | S7 |
| Ice Bolt | Ocean, Sky, Life | 22 | 228 | S3 |
| Armageddon | Stars, Earth, Sky | 22 | 303 | S5 |
| Advanced Healing | Law, Soul, Life | 23 | 303 | 41 |
| Full Paralysis | Ward, Life, Soul, Moon | 24 | 228 | 48 |
| Vampiric Drain | Death, Life, Soul | 25 | 338 | 51 |
| Crystalism | Stars, Sky, Death, Chaos | 25 | 338 | 52 |
| Aura | Ward, Life, Soul, Stars | 26 | 406 | 48 |
| Essence Drain | Soul, Chaos | 26 | 254 | 52 |
| Immortal Might | Soul, Earth, Moon, Chaos | 27 | 293 | 52 |
| Maelstrom | Death, Chaos | 27 | 750 | 52 |
| Restore Life | Life, Soul, Law | 27 | 587 | 41 |

The spells the guides lean on, by stage:
- Regeneration, Might and Fire Flash early;
- Quicksand and Fist Mastery for Baez;
- Physical Paralysis for White Dragons;
- Lightning (Electric Bolt) and Fireball from the caves on;
- Invisibility, Ice Bolt, Ogre Strength and Full Paralysis at the end.

### 4.4 Jong's moves

| Locke's level | Move [W1] | GRIND.md §2.3 |
|---|---|---|
| 1 | Charging Thrust (thrust from a distance) | same |
| 3 | Spinning Slash (swing from a distance) | same |
| 6 | Jumping Chop (chop from a distance) | same |
| 9 | Tiger combination | Combo A |
| 12 | Puma chain (thrust ×3 while moving) | thrust chain |
| 15 | Cobra chain (swing ×3 while moving) | swing chain |
| 18 | Scorpion chain (chop ×3 while moving) | chop chain |
| 21 | Spider combination (W ×3) | Combo B |
| 24 | Dragon combination (E ×5) | Combo C |

- Return to Jong as soon as each level is reached. [W1] [W2] picked them up
  on town trips: Spinning Slash before Arakna, Jumping Chop at 6, Tiger and
  Puma about 12, Cobra after 15, Scorpion and Spider about 20.
- The chains only fire when Locke moves into the target from a short
  distance. Standing still, three thrusts are just three thrusts. [W3]
- [W5] skipped Jong until about level 16.
- [W1]'s key text for the Tiger combination (Ctrl + swing ×2) differs from
  GRIND.md's Q, Q. Trust GRIND.md.

## 5. Money and grinding

### 5.1 Gold, in the order players find it

| Stage | Gold and saleable finds |
|---|---|
| The Keep | 250 + 150 + 100 gp [W1] |
| Misthaven | 144 gp (first house); small sums in the Inn and the houses; a Pouch [W1]. [W4] had about 600 gp by the time he left town. |
| The forest | 500 gp by Olihoot; 1,000 gp and a Sausage on the north-west shore; 1,000 gp in the Bone Yard's locked chest (Lockpicks) [W1] |
| The Druhg camp | 500 gp on the way; 1,500 gp in the camp [W1] |
| The tower | 340 + 300 + 250 + 1,500 + 1,000 gp; the Fenrus Collar [W1] |
| Arakna | about 4,000 gp in chests; Tulwar, Woodland set, Amulet of Channeling [W1] |
| The forest, after Arakna | Seven chests, about 5,000 gp and a Fenrus Collar. [W1] lists them by landmarks (one matches GRIND.md's ChestE (12615, 21103)). [W1] says Locke can now roam the whole forest. |
| On the way to the Ogroks | 1,510 + 1,000 gp [W1] |
| Later | Chests of 1,000–5,000 gp on almost every level; the pyramid and the labyrinth hold the most [W1] |

[W4] had 13,090 gp and chainmail by Arakna.

### 5.2 Where players grind

- **Levels 1–4:** the western forest near town. Monsters get stronger to the
  east, so a monster not seen before means Locke has gone too far. [W1] This
  matches GRIND.md §3.3: R1/R2 near the gate, R5/R6 east and harder.
- **For Baez:** fight barehanded in the forest, to Hand skill 3 [W1], or for
  a whole level [W6]. Use bare hands as often as possible early [W3]. (Jong's
  moves don't help barehanded, GRIND.md §2.3.)
- **After Arakna (6–7):** the whole forest. [W1]
- **Before Sidious:** gain levels until 7. [W1]
- **Stock follows level.** [W1] levelled in one area to unlock Gina's and
  Cronus's stock, and so always had a weapon better than those found in the
  dungeons. Their stock grows every three levels (GRIND.md §7.1).
- **Cheats** [W6]: the one-hit-kill code and sparring with Jong. They are
  cheats, and the smoke's no-cheat ledger excludes them.

### 5.3 Shop stock and what to buy

- **Gina, from level** [W1, as GRIND.md §7.1]:
  - 3: Spiked Bat, Studded Club;
  - 6: Maul, Morning Star, Sickle, Flanged Mace, Pointed Maul (GRIND.md adds
    the Battle Axe);
  - 9: Long Sword, Broadsword;
  - 12: Bastard Sword, Claymore;
  - 15: Katana, Flamberge;
  - 18: Charred Claymore, Spiked Maul;
  - 21: Shining Razor Mace.
- **Cronus** [W1], cost of a full set:
  - 6: Chainmail, 14,000;
  - 9: Bone, 5,000 (no helmet or gloves), and Barbarian, 48,000;
  - 15: Plate and Spiked;
  - 21: Samurai.
- **Hruthford** [W1]:
  - from the start: Pouch, food, Bracelet of Fortune (750), Ivory Pendant
    (500);
  - after Arakna: Sigil Ring 4,000, Emerald Collar 3,200, Fire Ruby Ring
    2,500, and a 2,000 amulet [W1 names it both Amulet of Cunning and of
    Channeling];
  - after Jhaga: Band of Fury and Emerald Bracelet at 10,000 each, Gloves of
    Striking 4,000, Necklace of Courage 6,000, Earthen Gauntlets 2,000.
  - This matches GRIND.md's "more at MISTSTATE ≥ 4 and at 7".
- **Buys the guides make:**
  - Lesser Mana early [W3];
  - Leather, then Chainmail at 6 [W2];
  - the Broadsword and Barbarian armour at 9 [W3, W4];
  - the Claymore at 12 [W2, W3];
  - the Flamberge at 15 [W2] (one is found in 41 soon after).
- **The locked shop chests.** The Gothic Key opens Cronus's chest for the
  Gothic set (§1.15). [W+]

## 6. Discrepancies and doubts

**Between the guides and the scripts.**
- **The Entrance Key's source.** STORY.md: Shegra gives it. [W3] and [W7]: a
  chest in Sabu's castle. [W1] and [W2] don't say. (§3 Q4)
- **The Sacrificial Dagger.** STORY.md: added during Andria's scene. [W3]: in
  the dais chest with the scroll. [W1] and [W2] never mention it.
- **The amulet after the arena.** [W4] says Locke keeps it; the script
  deletes it.
- **The slaves' key.** [W3] says the slaves hand over a key; [W1] and [W2]
  use the Entrance Key. Both fit the data, since both are key 44.
- **Jason.** [W4] ties his appearance to talking to the librarian. The script
  ties it to MISTSTATE > 1, which is Gus's deal.

**Between the guides.**
- **The Law talisman:** the armoury [W1] or the second Chamber of Horrors
  [W2].
- **The Solifuge's HP:** 5,000 [W1, W2] or about 4,500 [W4].
- **Where the longbow is:** the lake room [W3]; the Solifuge's room [W1, W2,
  W5, W6]. Ice arrows are in both [W1, W6].
- **The Pit's basement lever:** opens one gate [W1, STORY.md's `lv48d4`] or
  both [W2].
- **The Soul Cleaver:** an axe [W3, W7] or a sword [W8, data].
- **Vials:** [W5] says five empties and opens with "three vials"; the others
  say four empties and six vials.
- **Recipes.**
  - [W4]'s Quicksand (Earth, Sky) is wrong; the data has Earth, Ocean, as
    [W2] says.
  - [W1] leaves Quicksand out.
  - [W1] gives Blood Drain the same set as Heal; they differ only in order.
  - [W1]'s mana and skill numbers come from the manual and differ from
    `spell.def`.
- **Swift Strike against Sidious** [W2] isn't possible on the first visit:
  Ocean is the dragon's own loot.

**Retail quirks the guides report.**
- [W6]'s dragon-room save trick (§3 Q8).
- Monks sometimes jump onto fire geysers [W1].
- Some chests are empty [W1, W2].

**Version.** [W1] played patch 1.22. [W2] (2007) and [W4] don't say. [W3]
predates or matches release. Where [W3] alone disagrees with the scripts,
trust the scripts.

**Not settled anywhere.**
- Which items `GIVEWEAPONS` takes.
- How Locke leaves the ring.
- What the scriptless Jason is for.
- Whether retail's spell pouch matches talismans in order.

## 7. Sources

| Id | Title | Author | Date | URL |
|---|---|---|---|---|
| W1 | *Revenant* Guide and Walkthrough v1.1 (FAQ of the Month, July 2001); complete, with shop, combo, experience and spell charts; patch 1.22 | Michael Walsh | 2001-07-10 | https://gamefaqs.gamespot.com/pc/139180-revenant-1999/faqs/12645 (read through the Wayback Machine, snapshot 2026-09-24) |
| W2 | *Revenant guía* v0.8c (Spanish); complete, with talisman locations and maps | "antes" (GameFAQs user dolounk) | 2007-10-27 | https://gamefaqs.gamespot.com/pc/139180-revenant/faqs/49241 (Wayback, 2019-05-07) |
| W3 | Prima Fast Track Guide to *Revenant*, presented by CNET Gamecenter; 18 pages | Prima Publishing (no author named) | c. 2000 | http://www.gamecenter.com/Features/Guide/Revenant/ss01.html … ss18.html (Wayback, Nov–Dec 2000) |
| W4 | *Revenant* walkthrough, uncompleted; up to the Solifuge, with a spell list | FunkyChicken | c. 1999–2000 | the-spoiler.com/ACTION/Eidos.interactive/revenant.1.html, now https://coregamers.com/walkthrough/revenant |
| W5 | Addendum to FunkyChicken's walkthrough: the Solifuge, after the crypt, the Ruins | Sean D. Self | c. 2000 | https://coregamers.com/walkthrough/revenant-2 |
| W6 | *Revenant* cheats and hints | anonymous contributors | updated 2026 | https://www.cheatbook.de/files/revenant.htm |
| W7 | *Návod – Revenant* (Czech); follows W3's structure | anonymous | — | https://www.abcgames.net/?id=3344&p=navody_zobraz |
| W8 | Revenant Weapon List v0.2; weapon statistics | Stinger 3:16 (Stinger_316) | 2000-12-28 | https://gamefaqs.gamespot.com/pc/139180-revenant/faqs/8571 (Wayback, 2019-05-07) |

**Looked for, not obtained.**
- The Stratos Group's *Unofficial Guide to Revenant* (68-page PDF; the
  Wayback Machine has its index page but not the PDF).
- Prima's full printed guide.
- A Scribd copy of W4 (https://www.scribd.com/document/238288972/Revenant).

**The game's own files checked here.**
- `Resources/spell.def`: the recipes, skills and mana.
- The exit list and sector files through `tools/playtest/world.py`: the
  Keep's doors.
- GRIND.md and STORY.md, for container contents and object positions.
