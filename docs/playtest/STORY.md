# The main quest, stage by stage

The Keep chain ([STORY_TESTING.md](../gameflow/STORY_TESTING.md) §5,
`milestones.py` `KEEP`) ends with Locke on level 0 at (1842, 24388, 451),
outside the Keep gate, with `TENDRICKSTATE = 2` and `MISTSTATE = 1`. This page
carries the main quest on from there to the end credits, as stages a director
can play and check: what Locke has to do, where it is, which script lines gate
it and what they set, what the observation shows once it is done, and the
traps on the way.

**Sources.** The module's own files, read through
`storytest.ModuleData(Path('data'), 'Ahkuilon')` and decoded as Windows-1252:
`keep.s`, `town.s`, `forest.s`, `tower.s`, `arakna.s`, `cave.s`, `ruins.s`,
`dungeon.s`, `labyrinth.s`, `state.def`, `exit.def`, `area.def`, `english.def`.
`master.s` (the teleport stones) comes from `resources.rvr`, and `class.def`
(item types and key numbers) from `imagery.rvi`. A citation `town.s:2474` is
a line of that file. Positions are `World(Path('data')).find(name)` on the base
map (no slot). Door locks, key numbers and container contents come from the
sector files, decoded with `revsave` (the `LOCK`/`KEY` stats, and
`obj[N].inv[k]`). The manual (`docs/MANUAL.md` §V) covers the quest only as
far as leaving the Keep.

**Unconfirmed** marks anything the scripts and data don't settle. Nothing here
was played. Names are case-insensitive (`JASON1` is stored as `jason1`).

## 1. What the director can see

The director can't read script variables, so each stage's "done when" is given
in observation terms ([PROTOCOL.md](PROTOCOL.md) §7, `src/observation.cpp`):

- `player.level` and `pos`. Most story steps end in a fade and a
  `player.pos` jump, often to another level.
- `control`. Every scene and conversation turns it off. Saving is refused
  while it is off.
- `dialog`: `speaker` (the NPC's instance name, or its type name when it has
  none), `text` (the newest NPC line, not Locke's), and the `choices` text
  while a menu is up. A choice is the digit of its position, 1 to 6, in the
  order of the script's `Choice` lines.
- `objects` within 800 units. Invisible objects are left out
  (`observation.cpp:256`), so a `TOGGLE INVISIBLE = 1` NPC "vanishes".
  Effects are always left out, which includes the ruins and labyrinth
  barriers. Characters carry `health`.
- `player.pack`. Only the twelve inventory cells in view are listed, so a
  quest item on another page of the pack won't show. `player.weapon` and
  `equipped` are listed too.
- `messages`: text-bar lines. A click on a locked door prints "It seems to be
  locked" ([EXITS.md](../gameflow/forensics/EXITS.md) §1.8).
- `screen`: `movie` during the two story FMVs.

There is no "locked" field on exits. A locked door shows as the text-bar
message and Locke staying where he is.

## 2. The story variable

`MISTSTATE` (`state.def:6`, starts at 1) is the quest's spine. Every other
`*STATE` either gates one conversation or remembers that it was had. Each
value is set in exactly one place:

| MISTSTATE | Set by | Where | Sign the director can see |
|---|---|---|---|
| 1 | new game | `state.def:6` | `jason1` is invisible at the Druhg camp (`forest.s:2014-2018`); Gus1 sits in the bar |
| 2 | Gus1's "Deal" choice | `town.s:2474` | Gus1 gone from the bar; Gus2 sitting in his house by the docks (`town.s:305-313`); `jason1` visible and tied up (`forest.s:1993-2004`) |
| 3 | Sidious (the tower dragon) at health 0 | `tower.s:25-28` | DRAGONOUT now takes Locke back down (`tower.s:6-14`) |
| 4 | Gus4's "Found" choice on Arakna | `arakna.s:64` | Locke back on level 0 at (2912, 29630, 160) |
| 5 | Baez at health < 1 in the Ogrok arena | `forest.s:2287-2314` | "Soul Cleaver" given; the arena spectators deleted |
| 6 | Sardok4's scene in the Queen's Crypt | `cave.s:63` | Sardok4 fades out on level 37 |
| 7 | the NAKRA altar on level 49 | `dungeon.s:96` | four Nakranoth pieces in the pack; Locke on level 0 by Olihoot's shack |
| (end) | Yhagoro at health < 1 | `labyrinth.s:9-24` | movies `Mix_Fmv3English.smk`, `Mix_Credits.smk`, `ENDGAME` |

## 3. The stages

Each stage gives **Do**, **Where** (World.find: level, position),
**Way in** (exits from the previous stage; `exit.def` names, or the script
that moves Locke), **Gate** and **Sets** (script lines), **Done when**
(observation), and **Traps**. A stage marked *optional* gates nothing later.

### S1. Gus's deal: passage to Arakna for the ring (MISTSTATE 1 → 2)

- **Do.** Go to the tavern and talk to Gus. Press 1 at each of three menus.
- **Where.** `Gus1`, level 1 (4090, 20436, 0), sitting. `Rubold1`
  (3899, 20040) and `Kylie1` (3772, 20394) are in the same room, and
  Gus1's conversation uses both (`town.s:2350`, `2361-2364`).
- **Way in.** From the Keep gate, walk south into Misthaven (area rect
  765,24974 to 3664,27888 on level 0, `area.def`). `barin` door, level 0
  (3258, 25270, 64) → level 1 (4162, 20280, 33) (`exit.def:20`). Back out:
  `barout` (4229, 20431) → (3306, 25298, 67) level 0 (`exit.def:24`).
- **Gate.** None. Gus1's DIALOG has no `IF` (`town.s:2348-2497`).
- **Menus** (`town.s:2368-2460`):
  1. "What seems to be the trouble?" / "So, you must be the town drunk." /
     "I am beginning to have something against you as well." All three lead
     on.
  2. **1** "I do not suppose you would know how I could enter the caves?"
     (`Cav`). Choice 2, "What is this about an ancient tower?", loops back to
     this menu (`town.s:2426-2431`).
  3. **1** "I get the ring back and you get me into Arakna island. Is that
     the deal?" (`Deal`). Choice 2, "I have more important things to take
     care of right now.", loops back (`town.s:2462-2470`).
- **Sets.** `MISTSTATE = 2` (`town.s:2474`). Gus walks out through `BAROUT`
  and goes invisible (`town.s:2480-2493`). GUS2DUMMY then puts `Gus2` in
  his house and sits him down (`town.s:305-313`).
- **Done when.** Gus's last line is "Excellent.  I'll wait for you at my
  house.  It's the one by the docks." (II3GUS35). Gus1 is no longer listed,
  and control is back on.
- **Traps.** None of the loops dead-end: another key leaves them. Once
  Gus1 is invisible he can't be talked to again. BAYNE1 has a CUBE scene in
  the tavern at (3335–3440, 20211–20231) (`town.s:2315-2342`): Bayne
  knocks Keelan down with control off for about 5 s, if Locke walks there.

**S1b (optional, recommended for Jev's objective text).** Geralt in the
library, `Geralt1` level 1 (8581, 11389, 18), reached by `libraryin` level 0
(2149, 25797) → (8857, 11603, 11) (`exit.def:19`).
- At MISTSTATE 1 he points to the hermit (`town.s:722-733`).
- At MISTSTATE 2–3 with `JASONSTATE < 3`, choice 1 "What can you tell me
  about the ancient tower?" gives the plan: the tower needs the Solaria
  ("Golden Sun") key, and its owner Jason was taken by the Druhg, whose
  camp is "somewhere in the forests on this side of the island"
  (`town.s:798-865`).

Also optional:
- The hermit `Olihoot1`, level 0 (5837, 24805, 44). His choice 4 "Nothing
  right now." gives the `SKY` talisman (`forest.s:324-335`); choices 1–3
  loop.
- The combat master `Jong1`, level 0 (965, 29901, 19). His training
  (`COMBAT`) waits until Locke lands the move he teaches
  (`player.lastattack`, [GRIND.md](GRIND.md) §2.3). It needs a target
  Locke may fight: the dummy MUDOKON1 only while it fights him.

### S2. Free Jason, get the Golden Sun Key

- **Do.** Fight through the Druhg camp and talk to Jason. There are no
  choices.
- **Where.** `JASON1`, level 0 (2996, 15846, 35), tied to a tree. Around
  him: Mongrel Druhg (2901, 16097), Thrush Druhg ×3 (2746, 15950;
  3146, 16142; 3089, 15739), Under Druhg (2751, 15669).
- **Way in.** Walking on level 0. There are no exits between the town and
  the camp.
- **Gate.** JASON1 is invisible while `MISTSTATE = 1` and becomes visible
  once it is above 1 (`forest.s:1998-2018`), so S1 comes first. His DIALOG
  has no `IF`. The script doesn't require the Druhg to be dead
  (`forest.s:1950-1990`), but they will be on Locke.
- **Sets.** `player.ADDINV "GOLDEN SUN KEY"` (`forest.s:1976`),
  `JASONSTATE = 3` (`forest.s:1985`). Jason walks off, then goes invisible
  and is moved away (`forest.s:2005-2010`).
- **Done when.** "Golden Sun Key" is in the pack, and Jason says "Thanks
  again for saving me!" (III16JAS08) and leaves.
- **Traps.** A second, unnamed `Jason` (type Jason, no instance name)
  stands at (2995, 15837, 33). No script attaches to it (`OBJECT "JASON1"`
  matches by instance name, then type name "Jason" ≠ "JASON1"), so talking
  to it does nothing, and it stays visible at MISTSTATE 1. Talk to the one
  named `jason1`. What the twin is for is unconfirmed.

### S3. The Ancient Tower: kill the dragon, take the Diamond Ring (MISTSTATE 2 → 3)

- **Do.**
  1. Open the tower door with the key: drag "Golden Sun Key" from the pack
     onto the door, which uses it on the door
     ([MAP_INPUT.md](../gameflow/forensics/MAP_INPUT.md) §4).
  2. Inside, step onto the elevator at the end of the hall.
  3. Kill Sidious.
  4. Take the Diamond Ring from the chest in the dragon's room.
  5. Step back onto the elevator, then leave by the tower door.
- **Where.**
  - `TOWERIN`, level 0 (5125, 19639, 45): KeepDoor, `LOCK=1 KEY=42`.
    "Golden Sun Key" is key 42 (`class.def:3208`).
  - Inside, level 3: `Harowen1` (10368, 9968) and `Morganna1` (9923, 10189),
    both optional.
  - `DRAGONIN`, an elevator at (10016, 10032, 16), walk-on → (5130, 5130, 18)
    on the same level (`exit.def:44`).
  - Sidious (type name, no instance name) at (4809, 4821, 0).
  - The ring is in the Chest at (4724, 5092, 17), the only Diamond Ring in
    the module.
  - `DRAGONOUT`, an elevator at (5136, 5136, 16).
  - `TOWEROUT` (10535, 10080, 18).
- **Way in.** TOWERIN's USE block → level 3 (10412, 10080, 18)
  (`forest.s:623-642`). There are a Kantha (4928, 19968) and a Mongrel Druhg
  (5682, 20206) near the door.
- **Gate.**
  - A plain click on the locked door only prints "It seems to be locked".
    The key has to be used on it (EXITS.md §1.8).
  - BOSS1DUMMY sets `MISTSTATE = 3` when `SIDIOUS.STAT HEALTH = 0`; it checks
    every 24 ticks (`tower.s:19-31`).
  - DRAGONOUT is a CUBE (5125–5155, 5130–5155) that only returns Locke to
    (10006, 10022, 16) once `MISTSTATE > 2` (`tower.s:4-16`). So the dragon
    has to die before Locke can leave its floor.
  - TOWEROUT → level 0 (5240, 19638, 74) (`tower.s:35-54`).
- **Sets.** `MISTSTATE = 3`.
- **Done when.** Sidious is listed with health 0, "Diamond Ring" is in the
  pack, and DRAGONOUT moves Locke back to (10006, 10022) on level 3. Harowen
  and Morganna now have their "you killed the Dragon" lines
  (`tower.s:112-131`, `177-192`).
- **Traps.**
  - The dragon fight has to be won.
  - **Don't save between Sidious's death and the state change.** A
    character loaded dead is removed ([FINDINGS.md](FINDINGS.md) F1), so a
    save made before BOSS1DUMMY's next check would leave `MISTSTATE` at 2
    and the elevator dead. The same reasoning as Rahul's, unconfirmed here.
  - The teleport stone `towntel1`, level 3 (10033, 10484, 32), is about 450
    units from the arrival point. Stepping on it sends Locke to the Keep (§5).

### S4. The ring to Gus, the raft to Arakna Island

- **Do.** Go to Gus's house by the docks and talk to Gus. At his one menu,
  press either key.
- **Where.** `Gus2`, level 1 (7743, 8927, 23). `portin` level 0
  (2268, 27446, 64) → level 1 (7936, 8976, 17) (`exit.def:22`).
- **Gate.** `MISTSTATE < 4` and `player.GETITEMAMOUNT "DIAMOND RING" > 0`
  (`town.s:143`, `155`). Without the ring he only repeats the deal
  (`town.s:145-154`).
- **Menu.** "Uh... no, not really." / "She said she was sorry for hurting
  you." Both lead on (`town.s:167-182`).
- **Sets.** `DELINV "DIAMOND RING"` (`town.s:164`). After the "Got my
  sword!" scene, Locke is moved to level 0 (531, 28273, 18)
  (`town.s:213`). That point is inside GUS3's CUBE (500–600, 28250–28300),
  which plays the raft scene and moves Locke to **level 4 (10000, 10000, 16)**
  (`forest.s:1917-1939`).
- **Done when.** Level 4 after GUS3 says "Arakna island, dead ahead!"
  (II13GUS16). The ring is gone from the pack.
- **Traps.** Two scenes back to back, Gus2's and then the raft's
  (`town.s:290`, `forest.s:1921`). Control comes back only for a moment
  between them, so don't save until level 4.

### S5. The Amulet of Rogoss (MISTSTATE 3 → 4)

- **Do.** Find the amulet in a chest on Arakna Island, then talk to Gus and
  press **2**.
- **Where.**
  - Arakna is level 4 ("Arakna Island", `area.def`). The amulet is in the
    RunChestS at (8579, 9933, 40), the only Amulet of Rogoss in the module.
  - `Gus4` is at (10127, 9975, 5), next to the arrival point.
  - The island is full of spiders (Recluse, Artikulid, Widowett).
  - Two CUBE teleports link its halves: `Lv4Tel1` (9631–9661, 10461–10491)
    → (6896, 10192, 37) and `Lv4Tel2` (6742–6772, 10068–10098) → (9600,
    10672, 37) (`arakna.s:4-25`).
- **Gate.** Without the amulet Gus only says he'll wait. With it, he offers
  a menu (`arakna.s:41-52`): 1 "I have to find that amulet first." (ends),
  **2** "I have the amulet, we should leave." (`Found`).
- **Sets.** `MISTSTATE = 4` (`arakna.s:64`), then Locke is moved to level 0
  (2912, 29630, 160) (`arakna.s:68`).
- **Done when.** Back on level 0 at that point, after Gus says "Finally!
  Let's make like Druhg and cartwheel our asses out of here."
- **Traps.** Choice 1 ends the conversation without leaving. That isn't a
  dead end; talk again.

### S6. The Ogrok village and the arena (MISTSTATE 4 → 5)

- **Do.**
  1. Talk to the Ogrok gatekeeper with the amulet in the pack.
  2. At the chief, press **1** and then **1**.
  3. Win the arena fight against Baez, **unarmed**.
- **Where.** `Gatekeeper1`, level 0 (14019, 18446, 34). `CHIEF1`
  (13449, 17577, 69), whose conversation is a CUBE at (13499–13501,
  17727–17729) (`forest.s:2162`). `CHIEF2` (12161, 16985, 80). Baez at
  (12482, 16986, 32). The arena spot is AUSTIN's CUBE (12190–12194,
  17514–17518, z 110–114) (`forest.s:17`).
- **Way in.** Walking on level 0. The village is far to the east of the
  town.
- **Gate, step by step.**
  1. Gatekeeper with `GETITEMAMOUNT "AMULET OF ROGOSS" = 1` (exactly 1): "Open
     the gate! The Amulet of Rogoss has returned!", "Follow me.", then Locke
     is moved to (13500, 17728, 32) (`forest.s:2090-2115`). Without the
     amulet he only refuses, with a two-choice menu that ends either way
     (`forest.s:2038-2088`).
  2. That point is inside CHIEF1's cube, which fires with the amulet
     (`forest.s:2164`). Menu 1: **1** "I recovered your amulet. I demand
     passage through the caves!" (choice 2 loops, `forest.s:2189-2193`).
     Menu 2: **1** "And how do I prove that I am worthy?" (choice 2 loops,
     `forest.s:2208-2214`).
  3. `WORTHY` sets `VINCESTATE = 1`, removes the amulet, and moves Locke to
     (12192, 17516, 112) (`forest.s:2216-2238`). CHIEF2's ALWAYS block then
     takes Locke's weapons, `LOCKE.GIVEWEAPONS "CHIEF2"` (`forest.s:2282-2286`).
  4. The arena point is inside AUSTIN's cube: Baez roars and turns
     aggressive, the spectator chicks are deleted, and `RINGSTATE = 1`
     (`forest.s:15-48`). `RINGWALL.TOGGLE NOWALK = 0` presumably makes the
     ring wall solid: STYXXCHEST pairs `NOWALK = 1` with going invisible
     (`forest.s:251-252`). Unconfirmed.
- **Sets** (when `BAEZ.STAT HEALTH < 1`, `forest.s:2287-2329`):
  `ADDINV "SOUL CLEAVER"`, weapons returned (`GIVEWEAPONS "LOCKE"`),
  `CHIEFSTATE = 1`, **`MISTSTATE = 5`**. The spectators mark, vince, matt,
  redo, riz, austin, jason, hersh, rex, paul and carl are deleted.
- **Done when.** CHIEF2's last line is "I wish you success against the
  cult." (III9CHI06). "Soul Cleaver" is in the pack, the weapon is back in
  hand, and control is on.
- **Traps.**
  - The fight has to be won with no weapon: `player.weapon` is null after
    step 3. The director's equip pass must not count that as a missing
    weapon to re-equip. Which items `GIVEWEAPONS` moves is unconfirmed.
  - Don't kill Gatekeeper1, CHIEF1 or CHIEF2. CHIEF2 holds Locke's weapons
    during the fight.
  - Don't save between Baez's death and CHIEF2's scene (the F1 risk, as in
    S3).
  - How Locke leaves the ring afterwards is unconfirmed: RINGWALL is made
    solid (`forest.s:42`) and no script reverts it.
  - With `MISTSTATE > 4`, the gatekeeper just moves Locke into the village
    again (`forest.s:2119-2132`).

### S7. The caves (levels 30, 32, 39, 36, 31)

- **Do.** Enter the cave mouth in the village and work through five cave
  levels to the Queen's Crypt.
- **Way in.** CAVENTER's CUBE, level 0 (14030–14115, 17150–17195), next to
  the `ForAcave1` object (14058, 17120, 58), moves Locke to **level 30
  (10560, 10496, 16)** (`forest.s:2339-2351`). Back out: CAVEXIT (cube
  10530–10575, 10360–10390) → level 0 (14064, 17326, 66)
  (`cave.s:205-214`). None of these are exit-list exits. All are CUBE
  scripts, which `World.route` can't see (§4).
- **The forward chain.** Each step is a CUBE on the named object. The return
  cube sits beside each arrival point, checked against its pair.

  | From | Object (position) | Cube | To | Lines |
  |---|---|---|---|---|
  | 30 The Caverns | `Lv30EX` (6432, 1416, 16) | 6440–6465, 1380–1430 | 32 (10520, 8525, 380) | `cave.s:216-225` |
  | 32 Kragg Mountain | `Lv32EX` (9138, 8122, 248) | 9140–9165, 8090–8135, z 144–300 | 39 (9855, 7847, 147) | `cave.s:420-429` |
  | 39 The Infested Swamp | `Lv39EX` (10946, 10268, 144) | 10920–10960, 10265–10300, z 100–200 | 36 (10237, 9985, 16) | `cave.s:348-357` |
  | 36 The Maze of Darkness | `Lv36EX` (5296, 10054, 529) | 5340–5363, 10011–10098, z 500–600 | 31 (10011, 10005, 16) | `cave.s:372-381` |
  | 31 The Spiral Arm | `Lv31EX` (68, 9474, 418) | 60–90, 9440–9490, z 350–500 | 37 (9957, 10769, 82) | `cave.s:396-405` |

  Internal jumps exist on 32 (`Lv32ENT2`/`Lv32EX2`) and on 39 (`Lv39ENT1L`,
  `Lv39EX1L`, `Lv39ENT1H`, `Lv39EX1H`, `Lv39ENT2`, `Lv39EX2`)
  (`cave.s:240-345`). Whether the route needs them is unconfirmed.
- **Level 31: the Solifuge.**
  - SOLDUMMY's CUBE (1380–1390, 8370–8966, z 400–500) shuts and locks
    `soldoor` (1446, 8919, 406) behind Locke and sets the Solifuge
    (977, 8699, 423) on him (`cave.s:3-31`).
  - The Solifuge carries the **solifuge key** (KEY 886, its inventory in
    the sector). Key 886 also opens the locked InportEW at (628, 9222, 403).
    By position, that door stands between the Solifuge and `Lv31EX`.
  - So: kill it, take the key from its body, and use the key on the door.
- **Level 31: levers.** Five levers, `Lv31Lever1`–`5` at (4957, 13236),
  (5770, 12664), (1000, 11656, 289), (926, 6463) and (8050, 9156), each
  open one portcullis, `Lv31Gate1`–`5`, in a row at x 3346–4079, y ≈ 9220
  (`cave.s:432-669`). A lever's USE moves Locke there invisibly and back.
  Whether the route to the Solifuge needs the gates is unconfirmed.
- **Optional.** `Olihoot2` on level 30 (10426, 10522, 31), by the arrival
  point. Every path through his menus ends with the `sun` talisman and
  him leaving (`cave.s:781-857`).
- **Done when.** Level 37.
- **Traps.**
  - **Loot the solifuge key before any save.** A dead character is removed
    on load (F1), and its inventory with it. That would leave key 886
    unobtainable; unconfirmed, by the same mechanism.
  - Teleport stones near the arrival points: `towntel2`, level 30
    (10380, 10940), about 470 units from the arrival; `towntel3`, level 32
    (8704, 8432, 525); `towntel4`, level 36 (8048, 10112, 286). See §5.
  - Fire cubes (`flamer1`–`4`, `flameon`, `Flameon2`) burn Locke
    (`cave.s:671-778`).
  - The Maze of Darkness (36) has ambient light 3 (`area.def`), so frames
    are nearly black.

### S8. The Queen's Crypt: Andria's vision, Sardok (MISTSTATE 5 → 6)

- **Do.** Walk south through level 37. Two scenes fire on the way, both
  automatic CUBEs. At Sardok's menu either key works.
- **Where.** Level 37 ("The Queen's Crypt"). Arrival (9957, 10769, 82). The
  `Lv37ENT` return cube is beside it (`cave.s:408-417`).
  1. ANDRIA4's CUBE (9880–10600, 10500–10545, z 0–144) (`cave.s:88-122`):
     - `PLAYMOVIE "Mix_Fmv2.smk"` (screen `movie`).
     - Andria (`ANDRIA4`) fades in and speaks, ending "Soon we will be
       together." (IV4AND04).
     - `PLAYER.ADDINV "SACRIFICIAL DAGGER"`, then the trigger pauses.
  2. SARDOK4's CUBE (9750–9900, 9400–9423, z 0–144) (`cave.s:34-85`):
     - Sardok fades in: "I see you are in one piece. I trust everything went
       well?"
     - Menu: "How did you get here?" / "I made it, let's leave it at that."
       Choice 2 falls through to choice 1's lines (`cave.s:56-63`).
     - Both set **`MISTSTATE = 6`** (`cave.s:63`).
     - He ends "Come now, it is time we return to the castle and report to
       Tendrick." (IV3SAR07) and fades out.
  3. `LV37EX` (9794, 9028, 80), cube 9765–9816, 9000–9040 → **level 5 "The
     Ruins" (21920, 14830, 32)** (`cave.s:125-134`).
- **Done when.** Sardok4 vanishes and "Sacrificial Dagger" is in the pack.
  Then level 5.
- **Traps.**
  - Keep the Sacrificial Dagger. Olihoot later says it is "the only way to
    release her" (`forest.s:528`), and must never be shown to Sardok
    (`forest.s:499-500`). No script reads it: `ADDINV` at `cave.s:114` is its
    only use, so whether the ending needs it is unconfirmed.
  - Both cubes are unnamed. An unnamed CUBE fires for any moving object other
    than its owner ([SCRIPT_ENGINE.md](../gameflow/forensics/SCRIPT_ENGINE.md)
    §3), so a creature wandering in can start the scene.
  - `towntel5`, level 37 (10144, 9200, 96), lies between SARDOK4's cube and
    LV37EX.

**S8b (optional): report to the castle.** Sardok says to, but nothing later
tests the result.
- `towntel5` (§5) sends Locke to the Keep.
- `SardokT`, level 2 (12000, 9144, 364): with `MISTSTATE = 6` and
  `SARDOKSTATE < 3`, Tendrick and Sardok talk (I9); `SARDOKSTATE = 3`
  (`keep.s:883-937`). `SARDOKSTATE` is read only by SardokT itself.
- A second talk offers "Sardok, what do you know about the ancient queen?"
  (loops) or "I'm ready to head back to the Ruins now." (ends).
- `towntel0` in the Keep returns Locke to stone 5.
- At MISTSTATE 6, `Olihoot1` (level 0) has the "I have completed the caves"
  scene about the dagger (`forest.s:420-507`). It is optional, but its
  dialog explains the dungeon goal.

### S9. The Ruins: six vials on six pedestals (level 5)

- **Do.** Collect six vials, use one on each pedestal, then go through the
  opened way to the dungeon entrance.
- **Where** (level 5):
  - Pedestals `PedOne`–`PedSix` (RunACofre), (21755–22104, 9166–9389, 155).
  - `WarpRuin` (21943, 9179, 155) and the barrier `barruin` (21938, 9176,
    150), both effects.
  - `RUINSEXIT`'s CUBE (21100–21240, 8500–8525, **z 500–600**) → **level 48
    (7329, 9664, 144)** (`ruins.s:3-15`).
- **Vials.** Each pedestal takes any of the six kinds: Water, Acid, Root, Blue
  Flower, Issathi Claw or Bone Vial. A `USE "<vial>"` block sets its state
  and deletes the vial (`ruins.s:127-431`).
  - In level-5 chests: Water Vial (21912, 11560, 80) and Acid Vial
    (23560, 9460, 209).
  - The ingredient items, `INVCONTAINER` class, also in chests: Root
    (20768, 12064, 241), Bone (21103, 10547, 32), Blue Flower
    (23033, 11353, 208), Issathi Claw (24345, 11871, 33).
  - Four "EmptyVial" items, also in chests: (21195, 9809, 111),
    (22096, 12736, 74), (23266, 13110, 32), (23120, 9808, 111).
  - Each ingredient's class line names the vial it makes: Root →
    0xdefbc438 Root Vial, and so on (`class.def:3586-3589`). So an empty
    vial combined with an ingredient presumably makes the matching vial.
    How a player combines them is unconfirmed.
- **Gate.** WarpRuin's ALWAYS block waits for all six `Ped*.state = 1`, then
  deletes `barruin` and toggles WarpRuin visible (`ruins.s:433-462`). How
  that leads up to the RUINSEXIT cube at z 500+ is unconfirmed: the
  barrier stands by the pedestals, about 900 units east of the cube. The
  arrival from level 48 (21174, 8594, 568) shows the cube's ledge is
  walkable.
- **Done when.** The vials leave the pack one per pedestal. Then level 48.
  The barrier and the warp are effects, so the observation can't show
  them.
- **Optional.** `Varro`, level 5 (21836, 13358, 0): Navarro's banter, no
  effect (`ruins.s:31-124`). `MUDUKON9`'s cube (21690–21966, 14965–15000),
  next to the arrival point, goes back to level 37 (`ruins.s:19-28`).
- **Traps.** Don't sell or drop vials or ingredients. Six are needed and the
  data shows exactly two vials and four ingredient/empty-vial pairs.

### S10. The dungeons: levels 48, 41, 44, 45

The scripts gate parts of these levels on keys and levers but don't fix a
route. Below are the transitions (all `exit.def`, so `World.route` can plan
through them) and the scripted events. The order inside each level is
unconfirmed.

**Arrival pads.** Every arrival point sits on or beside the way back:
- 48 `LV48ENT`'s cube (7200–7300, 9600–9700, z 170–200), just west of the
  arrival;
- 41 `Lv41Enter` (10000, 9952);
- 44 `Lv44D3` (9749, 9987, 400);
- 45 `Lv45Enter` (10000, 10000);
- 46 `Lv46Enter` (3088, 12976).

Arriving on an exit doesn't fire it, but stepping off and back on goes
back.

**Level 48, The Pit.**
- Arrive at (7329, 9664, 144).
- Jhaga1's CUBE (7440–7500, 9520–9800, z 0–250) right away plays the Jhaga
  and Sabu cutscene. There are no choices (`dungeon.s:260-299`).
- Way on: `LV48EXIT` (25184, 25040) → level 41 (10008, 9960, 16)
  (`exit.def:88`).
- `Lv48D5` (19512, 19876), locked with `KEY=47325`, leads there:
  → (24976, 25184) (`exit.def:71`).
- `LV48D8` (3406, 6770), locked with `KEY=52374`, reaches Lv48D5:
  → (20083, 20022) (`exit.def:70`).
- Both keys are "Temple Key" items: KEY 52374 in the ChestS at
  (11798, 9902, 15), and KEY 47325 in the ChestS at (15142, 14952, 184).
- Lever pairs:
  - `Lv48Lev88` (16507, 19922, 274) unlocks `Lv48D1` (9737, 9794) through
    the Lv48Lev88B cube scene (`dungeon.s:154-204`).
  - `Lv48Lev6` (11686, 9218) unlocks `lv48d4` (9569, 8409)
    (`dungeon.s:206-256`).
- `TOWNTEL6` (8652, 10072, 160) is near the arrival point.

**Level 41, The City of the Children.**
- Way on: `Lv41D12` (13370, 16059) → level 44 (9828, 10004, 402).
- Optional talkers through the 41/42/43 doors: `Torturer`, level 42; `Chef1`
  and `Wanet`, level 43 (`dungeon.s:302-466`).
- `TOWNTEL7` (10960, 13024).

**Level 44, The Underground Pyramid.** Way on: `Lv44Exit` (13840, 8128, 784)
→ level 45 (10008, 10008, 16). `TOWNTEL8` (9904, 10288, 416) is about 300
units from the arrival point.

**Level 45, The Chase of Sabu.** Way on: `Lv45Exit` (4112, 9120, 144) →
level 46 (3096, 12984, 144). Scripted events:

| Event | Where | What it does | Lines |
|---|---|---|---|
| Sabu1 | cube 8200–8333, 9540–9666 | Sabu's introduction; menu VII2LOC02/03, both lead on | `dungeon.s:587-653` |
| `Lv45L5` lever | (8230, 9416) | unlocks `Lv45D98`, `Lv45D99` | `dungeon.s:121-152` |
| Sabu3 | cube 8500–8700, 7400–7462 | Sabu goes through `Lv45D5` and locks it (`KEY=77`) | `dungeon.s:658-698` |
| Horatio | cube 6928–7216, 5776–6448 | ghost scene; menu VII3LOC01/02, both end; gives "Wolfsbane Pendant" | `dungeon.s:521-582` |
| Mort | cube 9280–9456, 6368–6544 | Mort, Slagg and Urgg turn aggressive | `dungeon.s:471-517` |
| Sabu4 | cube 7216–7312, 7600–7696 | Kantha3 and Kantha4 released (fight) | `dungeon.s:703-759` |
| Sabu5 | cube 7357–7410, 8600–8800 | Sabu's spell fails ("That talisman…"), he teleports out | `dungeon.s:763-811` |

- Keys: "Exit Room Key" (KEY 66) and "Hallway Key" (KEY 77) are in the ChestE
  at (6924, 6142, 16) in Horatio's room. They open the InportEW at
  (8454, 7069) and `Lv45D5` (8310, 7205). "Door Key" (KEY 55) is in the
  ChestE at (9504, 9744, 145) and opens the InportNS at (9460, 9995).
- No script checks for the pendant, although Sabu5's lines credit it.

### S11. The slave camp: kill Sabu (level 46)

- **Do.**
  1. Get caught by Sabu. Press any key; he sends Locke to the slave camp.
  2. Talk to **`shegra1`** and press 2, then 1 or 2, then 1. She gives the
     poisoned dagger and the entrance key.
  3. Use the entrance key on the door `Lv46D5`. It lands Locke behind Sabu,
     and the stab scene kills him.
  4. Fight the two War Kanthas.
  5. Back in the camp, talk to a freed druhg and press 1 for the guardroom
     key.
  6. Take `LV46T3` to level 49.
- **Where** (level 46):
  - Sabu6 (10001, 9458, 145). Shegra: two objects (see Traps).
  - The druhg: `Druhgslave1` (7417, 7691), `Druhgslave2` (7140, 7688),
    `Druhgslave3` (6767, 7687), `Slave1` (6928, 7696), `Slave2`
    (7538, 7690).
  - `Lv46D5` (7899, 6022, 13), PortNS, `LOCK=1 KEY=44`.
  - `LV46T3` (7616, 9456, 16) → level 49 (5099, 5119, 514) (`exit.def:99`).
    Next to it are two InportNS doors, also `KEY=44`, at (7137, 9451) and
    (7138, 9548).
- **Gate and sets.**
  1. Sabu6's second CUBE (9850–10150, 9570–9700, z 0–100) runs while
     `SABUSTATE = 0` (`dungeon.s:852-924`). It sets `SABUSTATE = 1`, shows a
     three-choice menu where all choices lead on, ends "You will be sent to
     the slave camp at once…", and moves Locke to (7031, 10300, 16).
  2. Shegra's menus (`dungeon.s:928-1041`):
     - Menu 1: choices 1 "What the hell are you talking about!" and 3 loop.
       **2** "I am human." leads on.
     - Menu 2: either choice.
     - Menu 3: **1** "Well... fine." Choice 2 "No way." loops
       (`dungeon.s:1017-1021`).
     - `Fine`: `player.addinv "poisoned dagger"`, `give locke "entrance
       key"` (`dungeon.s:1026-1027`), `shegrastate = 1`. Last line "Wonderful,
       wasn't it."
  3. `Lv46D5` → (10008, 9314, 144) (`exit.def:64`). That point is inside Sabu6's
     first CUBE (10000–10016, 9306–9322, z 100–200), which needs
     `SABUSTATE = 1` (`dungeon.s:816-851`). Locke stabs Sabu, `SABU6` dies
     and vanishes, LV46KAN1 and LV46KAN2 turn aggressive, and
     `SABUKILLSTATE = 1`.
  4. With `SABUKILLSTATE = 1` and `DRUHGKEYSTATE = 0`, `Druhgslave2`,
     `Druhgslave3` or `Slave2` offer **1** "Just tell me where I can find
     Yhagoro." Choice 2 loops. Choice 1 gives `ADDINV "GUARDROOM KEY"`
     (class.def KEY 44) and sets `DRUHGKEYSTATE = 1` (`dungeon.s:1123-1166`,
     `1233-1276`, `1360-1403`).
  5. Ways back to the camp from Sabu's platform: `Lv46Tel2` (9904, 9520) →
     (7048, 10312) and `Lv46D42` (10000, 9271) → (7952, 6032)
     (`exit.def:65,101`).
- **Done when.**
  - Position jumps to (7031, 10300).
  - "Poisoned Dagger" and the entrance key are in the pack.
  - Sabu6 is no longer listed.
  - The druhg line "Human take key, it is only way to find Jhaga."
  - Level 49.
- **Traps.**
  - **Two Shegras.** The OBJECT "Shegra" script attaches to both: one by
    instance name, one by type name (SCRIPT_ENGINE §6).
    - `shegra1` (8824, 5118, 17) carries the "entrance key" (Copper Key,
      KEY 44).
    - The unnamed Shegra (8784, 5072, 26) carries "guardroomkey" (KEY 9).
    - Talked to through the unnamed one, `give locke "entrance key"` has
      nothing to give, presumably. Then only the dagger arrives, and Lv46D5
      stays shut until a druhg's guardroom key. But that key needs Sabu
      dead, which needs Lv46D5.
    - Talk to `shegra1`, and check the key is in the pack. Unconfirmed:
      what `give` does when the giver lacks the item.
  - Captured, Locke stands next to `LV46T2` (7040, 10304), a DunTeleport with
    no exit-list entry that does nothing, and `TOWNTEL9` (7424, 10208),
    which goes to the Keep.
  - The camp's monsters can kill Locke while he talks
    ([STORY_TESTING.md](../gameflow/STORY_TESTING.md) §7.1).
  - The druhg menus loop on choice 2.
  - `Lv49Boss`/`lv49boss` on level 49 return to level 46 (7606, 9450)
    (`exit.def:103`).

### S12. The Nahkranoth lair: Jhaga, Andria, the Nahkranoth (MISTSTATE 6 → 7)

- **Do.**
  1. Take the teleport west and confront Jhaga. There is no menu.
  2. Kill him.
  3. Watch Andria's vision (automatic).
  4. Climb to the altar.
- **Where** (level 49, "The Nahkranoth Lair"):
  - Arrival (5099, 5119, 514). `Lv49bTel2` (4135, 5133, 606) → (3504, 5136,
    626) (`exit.def:102`).
  - `JHAGA2` (2976, 5104, 625).
  - The altar: the NAKRA CUBE (9800–10000, 9800–9994, **z 240–400**) around
    the "DunKC Altar" tile (9899, 9751, 307) and the throne.
- **Gate and sets.**
  1. Jhaga2's CUBE (3250–3300, 4880–5390, z 600–700): his speech, then
     `JHAGATELSTATE = 1` and he turns aggressive (`dungeon.s:57-86`).
     LV49BTEL1's cube (3555–3622, 5095–5166, z 600–700) is the way back
     east to (4224, 5126, 606), except during the fight, while
     `JHAGATELSTATE = 1` (`dungeon.s:3-15`). So Locke can't flee Jhaga.
  2. Jhagadummy: when `JHAGA2.STAT HEALTH = 0`, control goes off for 240
     ticks, then Locke is moved to (10086, 11660, 16) (`dungeon.s:33-54`).
     That point is inside ANDRIA2's CUBE (10000–10120, 11450–11700), which
     sends him to **level 47** (10083, 11580, 16) (`dungeon.s:1427-1456`).
  3. ANDRIA3's CUBE on level 47 (10000–10120, 11500–11600): Andria appears,
     ending "The Nahkranoth will lead you to me.", then `ANDREASTATE = 3`
     and Locke is moved back to level 49 (10083, 11474, 32)
     (`dungeon.s:1458-1486`). He lands in ANDRIA2's cube again, says
     "Again???", and `ANDREASTATE = 4`.
  4. NAKRA (`dungeon.s:89-117`): **`MISTSTATE = 7`**, a 672-tick (28 s)
     acquisition, `ADDINV` Nakranoth Boots, Leg Plate, Chest Plate and
     Gauntlets, then **level 0 (5826, 24871, 32)** by Olihoot's shack.
- **Done when.** The four Nakranoth pieces are in the pack, and Locke is on
  level 0 near `OLIHOOT1` (5837, 24805).
- **Traps.**
  - The Jhaga fight has to be won.
  - Jhagadummy tests health `= 0`, not `< 1`. Don't save between Jhaga's
    death and the teleport (F1).
  - World.find doesn't see `ANDRIA3`: level 47's sector objects stay raw
    blocks in `revsave`. The sector bytes put it at about (10084, 11424, 17).
  - NAKRA needs Locke up on the altar (z ≥ 240), not just under it.

### S13. Olihoot's death, Sardok's betrayal, into the Labyrinth

- **Do.**
  1. *Optional:* talk to Olihoot. Sardok appears and burns him.
  2. Walk to the Keep and talk to Sardok. There are no choices.
- **Where.**
  - `OLIHOOT1`, level 0 (5837, 24805, 44).
  - `KEEPIN`, level 0 (1850, 24334, 443), USE → level 2 (12212, 12688, 161)
    (`forest.s:601-620`).
  - `SardokT`, level 2 (12000, 9144, 364), and `TendrickT` (11920, 9138, 364),
    up in the throne room. The Keep chain reached them with
    `player.pos 11973 9645 304 2`.
- **Gate and sets.**
  - Olihoot at MISTSTATE 7 (`forest.s:508-586`): the backstory, then
    `Sardok9` fades in, casts napalm, Olihoot dies and vanishes. Sardok9's
    last line is "You have no choice my Revenant. Come.", and he vanishes.
    It sets nothing, and the teleport to the Keep is commented out
    (`forest.s:580-584`).
  - SardokT at MISTSTATE 7 (`keep.s:940-1001`):
    - Tendrick and Sardok argue.
    - Sardok kills Tendrick with an electric bolt (`keep.s:965-981`).
      TendrickT fades out and goes invisible.
    - `SARDOKSTATE = 5`, so SardokT goes invisible afterwards
      (`keep.s:1006-1013`).
    - Sardok's last line is "As you wish." (VIII9SAR02). Fade, then **level
      51 (9904, 10000, 576)** (`keep.s:998`).
- **Done when.** Level 51.
- **Traps.**
  - TendrickT at MISTSTATE > 5 only has his I11 lines (`keep.s:1161-1173`).
    The scene is Sardok's.
  - The town NPCs all have MISTSTATE-7 lines; none matter.
  - There is no way back from the Labyrinth: `LabEnterOut` (9856, 10000) has
    no exit-list entry. Save before talking to SardokT.

### S14. The Labyrinth: seven crystals (levels 51–57)

- **Do.** On each level, take that level's crystal from its chest and carry
  it into the level's marker cube. Each placement shows a barrier on level
  57 dissolving, then returns Locke. Once all are placed, take `LabPyrin` on
  level 57 to the pyramid.
- **Gates.** Levels chain through `exit.def` gates, so `World.route` works
  here: 51 `LabGateDin` → 52 `LabGateEin` → 53 `LabGateBout` → 54
  `LabGateHin` → 55 `LabGateJout` → 56 `LabGateTin` → 57 `LabPyrin` → 58
  (`exit.def:116-148`).

  | Crystal | Chest | Marker cube | Barrier on 57 | Lines |
  |---|---|---|---|---|
  | Red | 51 (9913, 11599, 144) | 51 11241–11474, 8945–9154, z 240–320 | `Lv57GateR` (11248, 9488) | `labyrinth.s:30-77` |
  | White | 52 (12772, 7440, 432) | 52 11031–11246, 8433–8660, z 700–750 | `Lv57GateW` (11248, 9376) | `labyrinth.s:79-126` |
  | Green | 53 (9892, 10942, 432) | 53 10012–10237, 8698–8927, z 250–310 | `Lv57GateG` (11248, 8992) | `labyrinth.s:128-176` |
  | Blue | 54 (10711, 12495, 576) | 54 10350–10587, 9865–10114, z 710–750 | `Lv57GateB` (11008, 8848) | `labyrinth.s:178-226` |
  | Orange | 55 (13872, 11102, 144) | 55 10354–10594, 9870–10120, z 550–600 | `Lv57GateO` (10720, 8848) | `labyrinth.s:228-275` |
  | Yellow | 56 (10950, 10490, 432) | 56 10832–11079, 9207–9442, z 710–750 | `Lv57GateY` (10624, 8848) | `labyrinth.s:277-324` |
  | Purple | 57 (12103, 10269, 144) | 57 10358–10607, 9883–10124, z 100–180 | `Lv57GateP` (10480, 8720) | `labyrinth.s:326-374` |

  Each marker tests `getitemamount "<colour> crystal" = 1` and deletes the
  crystal. The BARRIER cube on 57 deletes that colour's warp effect, sets
  `WARPxSTATE = 1`, flips the barrier's `NOWALK` flag, and returns Locke to
  the marker.
- **Done when.** Each crystal leaves the pack, and Locke is back at the
  marker point after a fade. The barriers are effects the observation can't
  show. Then level 58.
- **Traps.**
  - The seven barriers stand in a line on level 57, from (11248, 9488) to
    (10480, 8720), before `LabPyrin` (10480, 8304). That all seven block
    the way is likely but unconfirmed.
  - Dragons, golems and demon mages throughout.
  - The `TOWNTEL10`–`14` stones on 51, 53, 54, 56 and 58 go to the Keep
    (§5).

### S15. Yhagoro (level 58)

- **Do.** Walk up to Yhagoro and win the fight.
- **Where.** Level 58, arrival (10256, 10016, 16). Yhagoro (type name)
  (10156, 8374, 265). Andria (10150, 7850, 472) has no script.
- **Gate and sets.**
  - Yhagoro's CUBE (9952–10284, 8518–8634, z 250–300): his speech, then he
    turns aggressive (`labyrinth.s:376-409`).
  - YOURMOMS: when `YHAGORO.STAT HEALTH < 1` and `ENDSTATE = 0`, control
    goes off, then `PLAYMOVIE "Mix_Fmv3English.smk"`,
    `PLAYMOVIE "Mix_Credits.smk"`, `ENDGAME`, `ENDSTATE = 1`
    (`labyrinth.s:3-28`).
- **Done when.** The movie screens, then the game ends.

## 4. Transitions `World.route` can't see

`world.py` routes only through exits that `exit.def` lists. Most quest
transitions are scripts that move Locke with `player.pos`. A director needs
these as hand-made legs, or `World` needs to learn them:

| Name (level) | Trigger | Area or object | To | Lines |
|---|---|---|---|---|
| KeepExit (2) | USE, unlocked at TENDRICKSTATE > 1 | (12236, 12742) | 0 (1842, 24388, 451) | `keep.s:1257-1290` |
| KEEPIN (0) | USE | (1850, 24334) | 2 (12212, 12688, 161) | `forest.s:601-620` |
| TOWERIN (0) | USE, key 42 | (5125, 19639) | 3 (10412, 10080, 18) | `forest.s:623-642` |
| TOWEROUT (3) | USE | (10535, 10080) | 0 (5240, 19638, 74) | `tower.s:35-54` |
| DRAGONOUT (3) | CUBE player, MISTSTATE > 2 | 5125–5155, 5130–5155 | 3 (10006, 10022, 16) | `tower.s:4-16` |
| Gus2 (1) | DIALOG with the ring | (7743, 8927) | 0 (531, 28273), then GUS3 → 4 (10000, 10000, 16) | `town.s:213`, `forest.s:1917-1939` |
| Gus4 (4) | DIALOG "Found" | (10127, 9975) | 0 (2912, 29630, 160) | `arakna.s:64-68` |
| Lv4Tel1 / Lv4Tel2 (4) | CUBE player | 9631–9661, 10461–10491 / 6742–6772, 10068–10098 | 4 (6896, 10192) / (9600, 10672) | `arakna.s:4-25` |
| Gatekeeper1 (0) | DIALOG, amulet or MISTSTATE > 4 | (14019, 18446) | 0 (13500, 17728, 32) | `forest.s:2113`, `2130` |
| CHIEF1 (0) | CUBE, amulet | 13499–13501, 17727–17729 | 0 (12192, 17516, 112) | `forest.s:2236` |
| CAVENTER (0) / CAVEXIT (30) | CUBE / CUBE player | 14030–14115, 17150–17195 / 10530–10575, 10360–10390 | 30 (10560, 10496, 16) / 0 (14064, 17326, 66) | `forest.s:2339-2351`, `cave.s:205-214` |
| FAIRYENT (0) / LOCASH (39) | CUBE / CUBE player | 9600–9646, 16134–16233 / 7880–7932, 9100–9130, z 700–800 | 39 (7900, 9200, 760) / 0 (9760, 16222, 69) | `forest.s:3-12`, `cave.s:138-147` |
| The cave chain (30, 32, 39, 36, 31, 37) | CUBE player | S7 table | | `cave.s:216-429` |
| LV37EX (37) / MUDUKON9 (5) | CUBE player / CUBE | S8, S9 | 5 (21920, 14830, 32) / 37 (9800, 9100, 32) | `cave.s:125-134`, `ruins.s:19-28` |
| RUINSEXIT (5) / LV48ENT (48) | CUBE | 21100–21240, 8500–8525, z 500–600 / 7200–7300, 9600–9700, z 170–200 | 48 (7329, 9664, 144) / 5 (21174, 8594, 568) | `ruins.s:3-15`, `dungeon.s:18-30` |
| Sabu6 capture (46) | CUBE player, SABUSTATE = 0 | 9850–10150, 9570–9700, z 0–100 | 46 (7031, 10300, 16) | `dungeon.s:852-924` |
| Jhagadummy, ANDRIA2, ANDRIA3 (49, 47) | ALWAYS / CUBE player | S12 | 49 → 47 → 49 | `dungeon.s:33-54`, `1427-1486` |
| NAKRA (49) | CUBE player | 9800–10000, 9800–9994, z 240–400 | 0 (5826, 24871, 32) | `dungeon.s:89-117` |
| SardokT (2) | DIALOG at MISTSTATE 7 | (12000, 9144) | 51 (9904, 10000, 576) | `keep.s:998` |
| Marker* / BARRIER* (51–57) | CUBE player | S14 table | to 57 and back | `labyrinth.s:30-374` |
| Teleport stones | ACTIVATE (`master.s`) | §5 | | |

Some CUBEs above are unnamed: CAVENTER, FAIRYENT, CHIEF1, AUSTIN, ANDRIA4,
SARDOK4, SOLDUMMY, RUINSEXIT and LV48ENT. An unnamed CUBE fires for any
moving object inside it other than its owner (SCRIPT_ENGINE §3), and these
blocks then move *Locke*. A creature walking into FAIRYENT's box sends Locke
to level 39 wherever he is.

## 5. The teleport stones

Fifteen `TwnTeleStone` exits run `master.s` ACTIVATE blocks (walk onto them):
- `TOWNTEL1` to `14` each raise `TOWNTELSTATE` to their number and send
  Locke to the Keep, level 2 (13177, 10747, 314) (`master.s:185-210` and the
  blocks after it).
- `TownTel0`, in the Keep at (13200, 10768, 320), sends him to the
  highest-numbered stone touched (`master.s:2-183`).

| Stone | Level and position | Stage it is near |
|---|---|---|
| towntel1 | 3 (10033, 10484) | S3, tower hall |
| towntel2 | 30 (10380, 10940) | S7, cave arrival |
| towntel3 | 32 (8704, 8432, 525) | S7 |
| towntel4 | 36 (8048, 10112, 286) | S7 |
| towntel5 | 37 (10144, 9200, 96) | S8, beside Sardok4 |
| TOWNTEL6 | 48 (8652, 10072, 160) | S10, near the arrival |
| TOWNTEL7 | 41 (10960, 13024) | S10 |
| TOWNTEL8 | 44 (9904, 10288, 416) | S10, near the arrival |
| TOWNTEL9 | 46 (7424, 10208) | S11, in the slave camp |
| TOWNTEL10–14 | 51, 53, 54, 56, 58 | S14, S15 |

They are both a hazard and a shortcut:
- An exploring walker that steps on one lands in the Keep.
- They are the intended way back to the Keep for S8b and S13: towntel5,
  then the Keep, then TownTel0 back.
- The arrival in the Keep is 30 units from TownTel0. Retail's anti-bounce
  (OF_ONEXIT) stops a direct re-trigger (EXITS.md §8).

## 6. Keep alive, keep hold of, don't save yet

- **Must stay alive** (their scripts carry the quest):
  - Gus (Gus1 until the deal, then Gus2, then Gus4);
  - jason1;
  - Gatekeeper1, CHIEF1, CHIEF2;
  - shegra1;
  - one of Druhgslave2, Druhgslave3 or Slave2, unless the entrance key
    already opens the guardroom doors (both are KEY 44);
  - SardokT.

  The director only engages `enemy` characters, which should keep these out
  of fights. That they all have `enemy` false is unconfirmed.
- **Must keep:**
  - Golden Sun Key, until the tower door is open;
  - Diamond Ring, until Gus2;
  - exactly one Amulet of Rogoss, until CHIEF1 (both checks use `= 1`);
  - the solifuge key;
  - the six vials, or their makings;
  - the entrance key and the guardroom key;
  - the Sacrificial Dagger (story; no script reads it);
  - the seven crystals (each check uses `= 1`);
  - the Temple Keys and the level-45 keys, until their doors are open.

  Never sell these in a shop.
- **Don't save** between a gating death and the block that reacts to it:
  Sidious (S3), Baez (S6), Jhaga (S12). Don't save while a key is still on
  a corpse (the Solifuge, S7). FINDINGS F1: a character loaded dead is
  deleted, with what it carries. Retail refuses saves while control is off.
  These windows are the moments when control is still on.

## 7. Summary: milestones after "left the keep"

| # | Milestone | Goal object | Done when (observation) | Save |
|---|---|---|---|---|
| S1 | gus deal | Gus1 | Gus1 gone; II3GUS35 said; control on | yes |
| S2 | sun key | jason1 | "Golden Sun Key" in pack | yes |
| S3a | in the tower | TOWERIN | level 3 | yes |
| S3b | dragon dead | Sidious | Sidious health 0, "Diamond Ring" in pack, DRAGONOUT returns to (10006, 10022) | yes, after DRAGONOUT works |
| S4 | on arakna | Gus2 | level 4 | yes |
| S5 | amulet, back | Gus4 | "Amulet of Rogoss" picked up; level 0 at (2912, 29630) | yes |
| S6 | arena won | Gatekeeper1 | "Soul Cleaver" in pack; weapon back; control on | yes, after the speech |
| S7 | through the caves | the cave chain, then Lv31EX | level 37 | yes, with the solifuge key in pack |
| S8 | sardok in the crypt | SARDOK4's cube | Sacrificial Dagger in pack; Sardok4 gone; then level 5 | yes |
| S9 | into the dungeons | the pedestals, then RUINSEXIT | level 48 | yes |
| S10 | to the slave camp | LV48EXIT, Lv41D12, Lv44Exit, Lv45Exit | level 46 | yes, per level |
| S11 | sabu dead | shegra1, Lv46D5 | Sabu6 gone; guardroom key; then level 49 | yes |
| S12 | nahkranoth | JHAGA2, then the NAKRA altar | four Nakranoth pieces; level 0 at (5826, 24871) | yes |
| S13 | into the labyrinth | SardokT | level 51 | yes, before talking |
| S14 | seven barriers | Marker* | each crystal used; level 58 | yes, per crystal |
| S15 | the end | Yhagoro | end movies, ENDGAME | — |

## 8. Open questions

1. **Ruins exit (S9).** How removing `barruin` leads to the RUINSEXIT cube
   at z 500–600, about 900 units west and 350 up from the pedestals.
2. **Making vials (S9).** How an Empty Vial and an ingredient
   (`INVCONTAINER`, `class.def:3583-3589`) make the vial, and whether the
   port supports it.
3. **The arena (S6).** Which items `GIVEWEAPONS "CHIEF2"` takes. How Locke
   leaves the ring once `RINGWALL` is solid (`forest.s:42`).
4. **Shegra's twin (S11).** What `give locke "entrance key"` does when run by
   the unnamed Shegra, who doesn't carry that key.
5. **Jason's twin (S2).** What the scriptless `Jason` at the camp is for.
6. **Routes inside levels.** Whether the internal teleports on 32 and 39 and
   the level-31 lever gates are on the required path. The order of keys and
   levers on 48 and 45, and the routes across 41 and 44.
7. **The Sacrificial Dagger.** Whether anything outside the scripts needs it
   at the end. No `.s` file reads it.
8. **Health `= 0` checks.** BOSS1DUMMY (`tower.s:25`) and Jhagadummy
   (`dungeon.s:39`) test equality, not `< 1`. If the port lets health go
   below 0, they never fire.
9. **The labyrinth.** Whether all seven level-57 barriers block the way to
   `LabPyrin`.
10. **World data gaps.**
    - `World.find` misses ANDRIA3: level 47's sector objects stay raw in
      `revsave`.
    - It doesn't list container contents. This page read them from
      `obj[N].inv[k]`.
    - `SpiritGuardian` (`cave.s:862-889`) has no object on any level, so its
      cube never fires.
