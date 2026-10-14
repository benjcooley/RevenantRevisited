# Grinding: experience, skills, gold, loot and gear

This page is the retail facts a director needs to make Locke grind in the
shipped game: where experience, skill experience, gold and equipment come
from, how strong the monsters near the start are, how Locke heals, and how
the shops work. It is written for whoever builds the director's grind mode
(`tools/playtest/director.py`, `combat.py`, `knowledge.py`, `walk.py`,
`world.py`). The user set two requirements:

- Locke can grind for stats, gold, level and equipment.
- Fighting barehanded is a first-class style, not a fallback. The Ogrok
  arena ([STORY.md](STORY.md) §S6) is won with fists.

**Sources.** Retail first:

- The Ghidra decompiles of the shipped `Revenant.exe`, made from a read-only
  copy of the project with the `recon/ghidra_scripts` approach. Addresses
  are given throughout.
- The disassembly in `recon/retail_asm/baseline`.
- The forensics docs that the text links to.

The game's own data:

- `class.def`, `char.def`, `weapon.def` and `armor.def`, from imagery.rvi.
- `rules.def`, `spell.def`, `english.def` and `master.s`, from resources.rvr.
- The Ahkuilon module's scripts and sector files, read through
  `storytest.ModuleData`, `World(Path('data'))` and `revsave`.

A citation like `forest.s:794` is a line of that archive file, as in
STORY.md. The 1998 snapshot source was not used. `src/` is cited only where a
REVSYNC header or a passing kata ties it to retail, or to say what the port
lacks. The manual ([docs/MANUAL.md](../MANUAL.md)) is quoted where it agrees
with the code and where it doesn't. Nothing here was played.

**Confidence marks:**

- **[C]**: read in the decompile, the disassembly or the data.
- **[L]**: likely. Either a reading that wasn't run, or an inference.
- **[U]**: unconfirmed.

**In short:**

- **Experience** comes only from kills. The amount depends on the monster's
  class `Value` against Locke's level (§1).
- **Skills.** The skill of the weapon in hand gets the same amount per kill;
  barehanded, that skill is Hands. Invoke rises with each spell cast. Attack
  and Defense never move. Jong's lessons unlock weapon moves only (§2).
- **Monsters and loot.** Placed monsters are finite. A dead monster drops its
  inventory on the floor and vanishes about 14 s later. Retail's monster
  generators respawn one monster per slot every 70–91 s while off screen;
  the port has none (§3, §4).
- **Healing.** Health comes back at about one point a minute. The real heals
  are potions, level-ups (each refills health, mana and fatigue) and
  reloading a save (§6).
- **Shops.** Gear prices and damage come from `weapon.def` and `armor.def`,
  not `class.def`. Selling pays 30% (§5, §7).
- **Observation.** It lacks most of the numbers these decisions need (§8).

## 1. Experience and levels

### 1.1 What awards experience

| Source | Retail | What it gives | Conf. |
|---|---|---|---|
| A melee kill | For a player attacker, ResolveHit `0x004c62b0` ends with three calls on the target (`0x004c6d83`–`0x004c6da5`; [COMBAT_HIT.md](../gameplay/forensics/COMBAT_HIT.md) §3.3): KillExp (TPlayer `+0x414`, `0x0051a630`), SkillExp(WeaponType()+3) (`+0x41c`, `0x0051abe0`), StealthExp (`+0x420`, `0x0051ad80`) | Exp; the skill of the weapon in hand; Stealth, if the target never saw Locke | [C] |
| A spell kill | TSpell::Damage `0x0053f560` calls KillExp (`0x0053f664`; [SPELLS_MISSILES.md](../gameplay/forensics/SPELLS_MISSILES.md) §2.12) | Exp only | [C] |
| An area-damage kill | AreaDamage `0x004de3c0` (`0x004de536`). Two neighbours also call KillExp: `0x004de250` (`0x004de39d`) and `0x004de610` (`0x004de6f1`) | Exp | [C] call sites; [U] what the two neighbours are |
| Casting a spell | CastByTalismans `0x0053fe80` and CastByName `0x0053f920` award it once the skill roll passes, before the spell exists (SPELLS_MISSILES §2.5) | Invoke experience, not Exp | [C] |
| Picking a lock | On success, a container's unlock `0x004dd480` calls AddSkillExp(10, 50) (`0x004dd937`) | 50 LockPick experience | [C] |
| Quests | Nothing. No module `.s` script and not `master.s` writes `EXP`, and no script command awards experience (COMMAND_SYSTEM.md, appendix). The manual's "by killing foes and solving quests" (MANUAL.md, Character Status) is half true | — | [C], from a grep of every shipped `.s` |

The three calls run on **every** blow that lands. Each returns at once unless
the target's health is below 1 (`0x0051a683`, `0x0051abf0`). So experience
comes from the killing blow, never from damage dealt.

### 1.2 The kill formula `0x0051a630` [C]

The gain compares the target's class stat **`Value`** (`GetStat("Value")`,
vtable `+0xd4`) with Locke's `Level` L:

```
base = (5·L + 10)·20                        // = 100·L + 200
d    = Value − L
gain = d > 0        ? (base / 15) · d
     : d ≥ −3       ? base / (30 − 30·d)     // d = 0: base/30; −1: base/60; …
     :                1
```

A PLAYER target uses its `Level` × 1.5 in place of `Value`; that only
happens in multiplayer.

`Value` is the second number of each type's `{Radius, Value}` in the
CHARACTER TYPES list of class.def (`class.def:3003-3112`; the stat is
declared at `class.def:2976`). Examples:

| Type | Value |
|---|---|
| Under Druhg, Araknid, Mudukon | 1 |
| Mongrel Druhg, Kantha | 2 |
| Thrush Druhg | 3 |
| Hopper | 4 |
| Pale Ogrok | 5 |
| Skeleton | 7 |
| Ogrok, Baez | 8 |
| Yhagoro | 30 |

Townsfolk have Value −1, which gives d < −3, so a kill is worth 1.

`Value` also sets the monster's to-hit and defence (§2.4). It is the one
difficulty number per monster type.

Gain per kill, with retail's truncating integer arithmetic. The first column
is Locke's level, with the Exp needed for the next level in brackets:

| L (to next) | V=1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 (300) | 10 | 20 | 40 | 60 | 80 | 100 | 120 | 140 | 160 | 180 |
| 2 (400) | 6 | 13 | 26 | 52 | 78 | 104 | 130 | 156 | 182 | 208 |
| 3 (500) | 5 | 8 | 16 | 33 | 66 | 99 | 132 | 165 | 198 | 231 |
| 4 (600) | 5 | 6 | 10 | 20 | 40 | 80 | 120 | 160 | 200 | 240 |
| 5 (700) | 1 | 5 | 7 | 11 | 23 | 46 | 92 | 138 | 184 | 230 |
| 6 (800) | 1 | 1 | 6 | 8 | 13 | 26 | 53 | 106 | 159 | 212 |
| 8 (1000) | 1 | 1 | 1 | 1 | 8 | 11 | 16 | 33 | 66 | 132 |

How many kills a level costs, by d = Value − Level:

| d | Kills per level |
|---|---|
| +k (k > 0) | 15/k |
| 0 | 30 |
| −1 | 60 |
| −2 | 90 |
| −3 | 120 |
| below −3 | out of reach (1 point a kill) |

Grinding only pays against monsters whose `Value` is at least Locke's level.

### 1.3 Thresholds [C]

`TRules::Initialize` `0x0048b690` builds the player table at Rules `+0xf0`:
e[0] = 0, e[1] = 300, and e[i] = e[i−1] + (5i + 10)·20 after that.
`ExpForLevel(n)` (`0x0048cc40`) is 0 for n = 0, else e[min(n−1, 29)].

Going from level L to L+1 needs `Exp ≥ ExpForLevel(L+1)`. `NextExp` holds
that number.

| Level | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Total Exp | 300 | 700 | 1200 | 1800 | 2500 | 3300 | 4200 | 5200 | 6300 | 7500 | 8800 | 10200 | 11700 | 13300 | 15000 |

- Exp accumulates.
- A kill raises at most **one** level, because the check runs once per kill.
- The cap is level 30.

### 1.4 What a level-up does `0x0051a630` [C]

1. Level + 1, and `NextExp = ExpForLevel(old Level + 2)`.
2. **Attributes, chosen in advance.** TPlayer keeps two attribute markers,
   `+0x360` and `+0x364` (values 0 Strn … 5 Luck), and a cursor `+0x368`.
   - The new level adds 1 to each marked attribute, on the stats themselves.
   - Both markers count while the new level is 15 or below. If both point
     at one attribute, it gets +2. From level 16 only `+0x360` counts.
   - After the raise, a marker whose attribute is at 29 or more moves on to
     the next attribute below 29.
   - **Setting the markers.** Use the six `train*` buttons on the Stats page
     of the side panel. Key `2` shows that page (`Stats=2` in
     `data/Revenant.ini`).
     - Positions: the pane sits at screen (452, 0). Each button is 62×12
       at pane-local (24, 120 + 12k), with k = 0…5 for Strn, Cons, Agil,
       Rflx, Mind, Luck. That is screen x 476–538, y 120–191
       ([CharacterStatsPane_SPEC.md](../ui/forensics/CharacterStatsPane_SPEC.md) §4).
     - A click runs `TStatPane` `0x00549470`, which calls
       `0x0051fe50`(attribute). That refuses an attribute at 30 or more
       (text `STATMAX`). Otherwise it writes the attribute into the marker
       the cursor points at, then flips the cursor.
     - So two clicks on one attribute put both markers on it.
     - The pane shows a `+` beside the attribute for each marker on it
       (`0x00547240`).
   - `newgame.sav` starts with the markers on Strn and Cons and the cursor
     at 1 (revsave: `player[0].levelupstats = 0, 1, 1`). If the director
     never touches them, every level to 15 gives +1 Strength and +1
     Constitution.
   - The manual says: "Make sure you know what Stats you are working on
     before you hit your new level" (MANUAL.md, Tips).
3. Text-bar messages:
   - `LUPJONG` at new levels 3, 6, … 24, when Jong has a new lesson (§2.3);
   - `STATCFG<attr>` for each raised attribute, then `LUPBASE`.
4. The `invoke2` animation, a `PowerUp` effect and the `POWERUP` sound.
5. RefreshStats. Then **health, mana and fatigue go to their maxima**, so
   every level-up is a full heal.

Nothing else changes. Skills don't move, and `AttackLevel` (stat 20, the
gate for weapon moves, §2.3) isn't touched.

### 1.5 What a level is worth [C]

The maxima rise with Level ([PLAYER_STATS.md](../gameplay/forensics/PLAYER_STATS.md)
§6). For Locke, rules.def gives `HEALTHDATA 25`, `FATIGUEDATA 3` and
`MANADATA 25`, and class Revenant has MANAMOD 5.

```
MaxHealth  = (75 + 25·L + MaxHealthFlat)  · (100 + MaxHealthPct  + StatLevel(Cons) + HEALTHMOD) / 100
MaxFatigue = (75 +  3·L + MaxFatigueFlat) · (200 + MaxFatiguePct + FATIGUEMOD + 2·StatLevel(Cons)) / 200
MaxMana    = (75 + 25·L + MaxManaFlat)    · (100 + MaxManaPct    + StatLevel(Mind) + MANAMOD) / 100
```

More important for fighting, **Offense and Defense each rise 10 per level**
(`Level · TOHITRANGEPLYR`, §2.4).

With the default markers (Cons = 12 + L − 1, Mind 14):

| L | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|
| Max H / F / M | 100/78/105 | 125/81/131 | 150/84/157 | 175/87/183 | 210/94/210 | 236/97/236 | 262/100/262 | 302/108/288 | 330/112/315 | 357/115/341 |
| Offense and Defense from level | 10 | 20 | 30 | 40 | 50 | 60 | 70 | 80 | 90 | 100 |

Level 2's 125/81/131 matches the port's verified level-up (PLAYER_STATS §10).

## 2. Skills, and fighting barehanded

### 2.1 The skills and how each rises [C]

There are eleven skills, stat ids 40–50 (`class.def:2904-2914`):

| Skill | Attack | Defense | Invoke | Hands | Knife | Sword | Bludgeon | Axes | Bows | Stealth | LockPick |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |

Each skill also has an experience (stats 51–61), a next-level experience
(62–72) and a cap (73–83). The opening's `playerlevel 1` sets every skill to
0, its experience to 0 and its next to 300
([COMMAND_SYSTEM.md](../gameflow/forensics/COMMAND_SYSTEM.md) §6.1).

Every rise goes through `AddSkillExp` (TPlayer `+0x418`, `0x0051ac90`):

- Below 30, the experience is added. When it reaches `SkillExp(level+1)`,
  the skill goes up **one** level.
- `SkillExp` (`0x0048cc90`, Rules `+0x168`) is 300, 700, 1200, 1800, 2500, …
  That is the player's table shifted one place: skill k rises to k+1 at the
  Exp that takes the player to level k+2.

The disassembly has exactly six calls of `+0x418` (`0x004dd937`, `0x0051ac85`,
`0x0051d930`, `0x0053fc1d`, `0x0054019a`, `0x00582ded`), so this list is
complete:

| Skill | Rises by | Amount | Where |
|---|---|---|---|
| Hands, Knife, Sword, Bludgeon, Axes | **killing** with that weapon type in hand (skill = WeaponType + 3) | the kill formula of §1.2. It runs after the kill's own level-up, so a levelling kill gives the new level's smaller amount | `0x0051abe0`, called from `0x004c6d9a` |
| (weapon types 5–7) | these would feed stats 48 Bows, 49 Stealth and 50 LockPick (WeaponType + 3 runs past Axes). No shipped weapon has these types: `weapon.def` gives every staff Type 3, bludgeon | — | [C], data |
| Bows | an arrow kill | kill formula | the arrow hit (SPELLS_MISSILES §2.18); the manual says bows are multiplayer only |
| Stealth | a melee kill of a target that never saw Locke (`0x004c58f0`) | kill formula | `0x0051ad80` |
| Invoke | **every successful cast** of a spell with SKILL > 0, whether the spell hits or not | `(clamp(SKILL − Invoke, −9, 9) + 10) · (((5·Invoke + 15)·20) / 8) · 10 / 100`. That is 40 for a SKILL-1 spell at Invoke 0, and 60 at Invoke 3 | `0x0053fe80`, `0x0053f920` |
| LockPick | a lock picked on a container | 50 | `0x004dd480` |
| Attack, Defense | **never, in play.** Only `playerlevel n` with n > 1 feeds them (`0x0051d840`). Offense and Defense don't read them (`0x0051a480`, `0x0051a4e0`), and no `push 0x28` / `push 0x29` comes before a stat read anywhere in `.text` [L] | — | [C] |

Blocking, dodging, landing blows and taking hits give nothing.

The manual says "Attack … rises every level" and "Defence … rises every
level" (MANUAL.md §VI). That describes the `Level · 10` term in Offense and
Defense (§2.4). The Attack and Defense skill stats themselves stay 0.

**What this means for a grind:**

- **Kills feed Level and the weapon skill in hand equally.** A Locke who
  only ever fights barehanded has Hands ≈ Level − 1. Switching weapons
  splits the skill experience, while Level always gets the whole amount.
- **Invoke is the only skill that rises without kills.**
  - Locke starts with the Life, Moon and Soul talismans (`keep.s:616-618`).
    Olihoot's choice 4 adds Sky (`forest.s:328`).
  - That allows Poison (Moon; 42 mana; SKILL 1) and, with Sky, Fire Flash
    (Moon+Sky; 31 mana; SKILL 1; 50–55 burn damage).
  - Invoke 0 → 1 takes about 8 casts.
  - Below the spell's SKILL, a cast succeeds 80 / 40 / 20 / 0 % of the time
    at 1 / 2 / 3 / 4+ short. A failed cast gives no experience.
  - Mana regenerates about 1 point a minute (§6.1). So Invoke grinding is
    paid for with mana potions, or with the mana refill of a level-up: spend
    mana freely just before a levelling kill.

### 2.2 What a skill is worth [C]

- **Weapon skills.** Each level adds **1 to Offense** (AttackModifier
  `0x0051a480` reads stat `43 + WeaponType`) and nothing to damage. No
  attack of Locke's is gated on it: every Locke ATTACK line has weaponskill
  0. One weapon-skill level is worth a tenth of a character level.
- **Invoke** decides which spells cast (§2.1).

### 2.3 AttackLevel and Jong's lessons [C]

Attacks are gated by **`AttackLevel` (stat 20)**, not by the Attack skill.
IsValidAttack requires all of:

- `1 << WeaponType()` is in the attack's weapon mask;
- `AttackLevel ≥ attackskill`;
- `stat 43+WeaponType ≥ weaponskill`.

([COMBAT_ATTACK_CHOICE.md](../gameplay/forensics/COMBAT_ATTACK_CHOICE.md)
§3.1 item 19; read at `0x004d1654`.)

The opening sets AttackLevel to 0 (`keep.s:488`), and only **Jong** raises
it (`forest.s:794-1690`). At his dialog's "Combat" choice he runs every
lesson whose requirement Locke meets, in order:

| Locke's level ≥ | AttackLevel set | Move to perform | Keys | Weapon mask | forest.s |
|---|---|---|---|---|---|
| 1 | 1 | Charging Thrust `chargethrust` (`chargebackswing`) | A at 79–300 | sword (axe, bludgeon) | 794-887 |
| 3 | 3 | Spinning Slash `spinswing` | S at 79–300 | two-hand (0x1e) | 889-982 |
| 6 | 6 | Jumping Chop `jumpchop` | D at 79–300 | 0x1e | 984-1078 |
| 9 | 9 | Combo A `comboa1`→`comboa2` | Q, Q | 0x1e | 1080-1176 |
| 12 | 12 | Thrust chain `thrustchain1-3` | A ×3, the first while stepping in | 0x1e | 1178-1278 |
| 15 | 15 | Swing chain `swingchain1-3` | S ×3 | 0x1e | 1280-1379 |
| 18 | 18 | Chop chain `chopchain1-3` | D ×3 | 0x1e | 1381-1481 |
| 21 | 21 | Combo B `combob1-3` | W | 0x1e | 1483-1583 |
| 24 | 24 | Combo C `comboc1-5` | E | 0x1e | 1585-1690 |

**How a lesson runs:**

1. AttackLevel is written **first**.
2. Jong shows the move on the training dummy MUDOKON1.
3. Control returns with a Training Sword equipped. EQUIPSTATE is 1, so
   MUDOKON1's ALWAYS block re-adds and re-equips the sword every second
   (`forest.s:1902-1911`).
4. The script waits until `player.lastattack` names the move
   (`forest.s:857`, …): Locke's last attack is that move and it struck
   its target (`0x0041f755`; FINDINGS F7). The target has to be one Locke
   may fight (IsEnemy `0x004c89c0`). MUDOKON1 isn't one unless it is
   fighting him: it has no GROUPS, and Locke's ENEMIES don't name it.
5. Locke bows, and the sword is unequipped and deleted.

Jong1 stands on level 0 at (965, 29901, 19) (STORY.md S1).

His other choice is **Spar**. It sets Locke on MUDOKON1 with the Training
Sword. MUDOKON1's health is reset to 1800 every second, and Locke's health is
held at its value from the start of the spar (`forest.s:1808-1846`). Sparring
is practice with no experience and no risk.

**Every lesson is a weapon move.** None of the moves Jong unlocks has WM_HAND
in its mask, so AttackLevel gives a barehanded Locke nothing.

**Port:** `player.lastattack` is ported (FINDINGS F7). A lesson completes
once the move lands, and the Training Sword is taken back.

### 2.4 To-hit and damage, in numbers [C]

Sources: [COMBAT_HIT.md](../gameplay/forensics/COMBAT_HIT.md) §1–3, the
player getters `0x0051a520`/`0x0051a550` and the monster getters
`0x004d72e0`/`0x004d72a0`.

```
Locke    Offense = 10·Level + StatLevel(Agil) + weapon skill (stat 43+WeaponType) + spell buffs
Locke    Defense = 10·Level + StatLevel(Rflx) + spell buffs
Monster  Offense = Defense = 12·Value       // no char.def block sets ATTACKMOD, DEFENSEMOD or ARMOR (COMBAT_DATA §5.1)
tohit  = clamp(50 − (target Defense + target Luck) + (Offense + Luck)
               + 25 if the target faces away or is mid-attack, 10, 100)
         (+5 one time in ten)
roll   = random(1,50) + random(0,50)        // 1..100, peaked: P(roll ≤ 24) = 12%, ≤ 48 = 46%, ≤ 76 = 88%
damage = target.CalculateDamage(WeaponDamage, type, attack damagemod)
         · (100 + DamageMod + StatLevel(Strn)) / 100
         [· (100 + EdgeBonus) / 100 for knife, sword, axe]
         · tier(tohit − roll)               // ≥40: ×1.5; ≥0: ×1; ≥−15: ×0.5; ≥−40: ×0.25; else ×0.1
CalculateDamage = max(1, (100 − DAMAGEMODS[type]) · ((100 + damagemod) · WeaponDamage / 100) / 100 − Armor)
```

**What each term means for a fight:**

- **A failed roll is a glance, not a miss.** The target still takes the
  tier-cut damage (at least 1) and can die of it.
  - A blocking target that faces the attacker cuts the attacker's to-hit by
    50 (ResolveHit, COMBAT_HIT §3.3).
  - A blocking **player** takes nothing from a glance.
- **Armor.** Every monster's Armor is 1, because no char.def block sets
  ARMOR. Locke's Armor is the sum of his armour's Protection (PLAYER_STATS
  §6), and it comes off every hit he takes.
- **Damage types.** The type of a blow decides which resistance applies.
  - Barehanded blows are type `hand` (`0x004c47d0`).
  - **Every weapon deals `misc`.** Locke's damage type (`0x0051b2f0`) is the
    weapon's DamageType, and `weapon.def` gives every weapon 0 = DT_MISC
    (§5.1). No monster resists `misc`, so **weapons ignore resistances**.
  - Fists meet each monster's `hand` column.
- **DAMAGEMODS are resistances in retail**, applied as `(100 − mod)`%.
  - The `hand` column: Thrush Druhg, Sidious and the Blue and Red Dragons
    +20; Ogroks, Vashar, Acolytes and the Green Dragon +10; Lurker −10; Earth Golem −25; Stone Golem −50;
    Yhagoro −80; Wraith, Demon Mage and Undead Sorcerer −100 (double damage).
  - `knowledge.py`'s `monster_brief` reads positive values as "takes extra
    damage", which is the reverse of what retail computes. Retail's formula
    is the port's REVSYNC'd `TCharacter::CalculateDamage`, which kata C1
    checked against retail (`src/character.cpp:1081`).
  - The data's authors may have meant it the other way: the manual tells the
    player to fight a fire monster with ice. Retail does what the arithmetic
    says.
- **Level is the big lever.** Each point of a monster's Value is 12 to-hit
  points both ways; each level of Locke's is 10. Take a level-1 Locke:
  Offense and Defense 10, and Agil, Rflx and Luck 14 give a modifier of 0.
  - Against Value V, his to-hit is `60 − 12V`, and the monster's on him is
    `40 + 12V`.
  - Against an Under Druhg (V1) that is 48 against 52.
  - Against an Ogrok (V8) it is 10 (floored) against 100.
- **Vary the button.** The same button pressed three times in a row on one
  target gets roll 100, the worst tier. It also has a 1-in-11 chance that
  the monster counters at once (ButtonAttack, COMBAT_ATTACK_CHOICE §3.2).
  Rotate A, S and D.
- **Fatigue.** A blow is paid for in fatigue at its impact frame (COMBAT_HIT
  §3.2). Below the cost of every normal attack, the game uses the
  **fatigued blow** instead (`hfatigue` and `cfatigue`, valid at fatigue ≤
  25; `char.def:651-713`). It lands at 16 ticks, recovers in 29, and does
  −80% damage.

### 2.5 Bare hands against weapons [C]

**What changes barehanded:**

- WeaponType is 0 (`0x00520810`: no WEAPON in hand).
- WeaponDamage is Locke's `WEAPONDAMAGE 12` (`0x00520830`, `char.def:120`).
- The damage type is `hand`, and the skill is Hands (stat 43).

**The attacks.** These are Locke's attacks with WM_HAND (`char.def:140-255`,
`730-897`). All have attackskill 0 and weaponskill 0, so **all are available
from the first fight**. Each cell gives the attack, its damage bonus and its
fatigue cost:

| Key (button) | 0–20 (close) | 19–75 (mid) | 74–300 (lunge) | Fallback, 0–10000 |
|---|---|---|---|---|
| A (1) | jab, +10%, 12 | mediumpunch, +12%, 12 | sidekick, +50%, 20 | mediumpunch |
| S (2) | punch, +14%, 14 | frontkick, +16%, 14 | jumpspin, +60%, 22 | frontkick |
| D (3) | uppercut, +18%, 16 | spinkick, +20%, 16 | jumpside, +70%, 24 | spinkick |

- Timing up close: A, S and D land after 3, 4 and 5 ticks and are ready
  again after 8, 12 and 16.
- The lunges take 11–13 ticks to land and 18–26 to recover.
- Reach (hitmaxrange) is 25–50, against 50–70 for blades.

**Grapples.** These interactive moves work barehanded. All are
`CA_INTERACTIVE | CA_SPECIAL | CA_ONETARGET`, with +50% damage and 42
fatigue (`kick`: 32). FindButtonAttack searches RESPONSE, then SPECIAL, then
plain attacks, so a grapple is tried **before** the plain attacks of its key.

| Key | Moves |
|---|---|
| Ctrl+A | `aikidothrower` |
| Ctrl+S | `militarypresser`, `wheelbarrowflipper`, `monkeyflipper`, `atkthreehitdrop`, `beatupper`, `kick` |
| D, within 40 | `legbreaker`, `brokenlegger`. These are CA_DEATH: they fire only when the blow would kill, as finishers |
| A | `atkaraknidsmash`, against Araknids |

- Each grapple needs the target to have the matching impact animation.
  Otherwise the move is refused (IsValidAttack step 22). Which monster types
  have the animations is unconfirmed.
- `sunsetflipper` (A, only against `Baez`, `0x004d16a6`) has the **sword**
  mask (`char.def:851-857`). It can't fire in the barehanded arena fight
  [C]. Whether that is a data bug is a question for the author.

**What a weapon adds:**

- Jong's nine moves, the chains and the combos (all weapon-masked).
- EdgeBonus, for knife, sword and axe.
- Longer reach.
- Above all, **damage that scales**. Locke's fist stays 12 for the whole
  game. Weapons go 13 (Short Sword) → 35 (Tulwar) → 55 (Long Sword) → 85
  (Katana) → 150 (Wyrmm's Tooth), the `weapon.def` values.

Base damage per blow for a level-1 Locke (Strength gives +5%) against a
monster with Armor 1 and no resistance. Each cell lists A / S / D:

| | Close and mid | Lunge |
|---|---|---|
| Hands (12) | 12 / 12 / 13 | 17 / 18 / 19 |
| Short Sword (13) | 10 / 12 / 13 | 17 / 18 / 19 (Jong's charge moves) |
| Stone Axe (17) | 14 / 16 / 17 | |
| Tulwar (35) | 31 / 35 / 38 | 50 / 53 / 57 |
| Long Sword (55) | 50 / 56 / 61 | 79 / 85 / 91 |

So fists match the Short Sword early and fall behind by the first real
weapon. The Training Sword (Damage 2) is far worse than fists.

**Raising barehanded damage:**

- Strength: `StatLevel(Strn)` gives +5% for every 3 points from 16, up to
  +30% at 30 (rules.def STATLEVEL "Strength").
- DamageMod items: Amulet of Thunderbolts +10, Amulet of Power +20, and the
  Barbarian set +3 per piece (`armor.def:698-736`, `1465-1470`,
  `1598-1603`).
- Monsters with a negative `hand` column (§2.4).

**Raising barehanded to-hit:**

- Level, Agility and Hands.
- Spells, each lasting 2880 frames (120 s):
  - **Fist Mastery**: Life+Soul, 50 mana, SKILL 3, `STATLINE HANDS 4`
    (`spell.def:230-240`);
  - **Shadow Fist**: Stars+Moon, 125 mana, SKILL 9, `HANDS 8`
    (`spell.def:654-664`).
  - These STATLINEs sit inside the CONTROLDATA block. That they reach the
    variant's statline is [L], because ControlData has no statline field
    (SPELLS_MISSILES §2.17).
- The **Band of Shadows** bracelet: `Hands 15%` (`armor.def:1430-1435`). It
  probably adds a flat +15, since the line reads `Hands`, then the number
  15, and the trailing `%` ends the line (PLAYER_STATS §3 and §11 q4) [L].
- The Spiked set (`hands %2`) and Ninja set (`hands %5`) multiply Hands by
  1.02 and 1.05. Truncated, that adds nothing until Hands reaches 50 and 20
  (Hands is capped at 30) [C].

Only one stat effect, so one buff, is active at a time (PLAYER_STATS §4).

**The arena** (STORY.md S6) is the one fight that must be barehanded.
GIVEWEAPONS "CHIEF2" (`0x00477780`) moves every WEAPON, AMMO and
RANGEDWEAPON item Locke has, equipped ones included, and searches bags too.
Armour stays on [C].

Baez:

- Value 8, so Offense and Defense 96;
- `HEALTH 650`, `WEAPONDAMAGE 32`, `hand` resistance 0;
- wrestling throws with interactive impacts;
- CalculateDamage zeroes magic damage on him (`monsterkind 1`, `0x004c9b70`;
  COMBAT_DOJO §5 item 5);
- boss AI case 1.

With fists, Locke's to-hit is about `60 − 96 + 10·(L−1) + Hands`, with +25
while Baez attacks or is turned away.

- At level 1 that is the floor of 10, and every blow glances for a tenth.
- Around level 8–9 Locke's Defense (80–90) and to-hit (about 40–50, plus 25
  on a counter) make it a fight, and about 300 HP survive his throws.
- Plan the arena as a level target. [L]: arithmetic from the formulas
  above, not played.

## 3. Monsters to grind on

### 3.1 Supply: placed monsters are finite, generators are not

**Placed monsters never come back** [C]:

- A sector is read from the save's `curmap` if it is there, else from the
  base map, and it is written back to `curmap` when it unloads
  ([SAVE_GAME.md](../gameflow/forensics/SAVE_GAME.md) §2, §4).
- A character saved dead is deleted on load (`TCharacter::Load`
  `0x004d4eb0`; [FINDINGS.md](FINDINGS.md) F1).
- Even without a save, a corpse goes away. TCharacter::Pulse `0x004c1bb0`
  counts a dead character's ticks (`+0x228`, `0x004c2479`). At 240
  (`0x004c2489`) it fades the corpse out, 1 a tick from fully seen
  (`SetFade(100, 1, 0)`, `0x004c24c0`), then sets KILL (`0x004c24a0`), and
  PulseObjects deletes it: 340 ticks, 14.2 s, after death [C; measured in
  the emulator, [FINDINGS.md](FINDINGS.md) F9].
- Only reloading a save made before the kill brings a monster back.
- No script creates monsters. The module's `.s` files only toggle
  `AGGRESSIVE`, `INVISIBLE`, `POS` and `DELETE` on existing characters [C].

**Monster generators respawn** [C]. The HELPER type `MonsterGen`
(`class.def:3183`; vtable `0x005b4cd4`; constructor `0x005176a0`) holds up to
5 slots and a cap, default 2. Each slot is {type name, frequency, treasure
type, start time} (AddType `0x00517050`, Load `0x00516e00`).

Its Pulse `0x00516c30` does this for each slot:

- `elapsed = GameTime − start`, in hundredths of a second.
- If there is no start yet, or `elapsed > 2·freq`: restart the clock and
  don't spawn.
- If `elapsed > freq`: restart the clock and count the characters within
  **300 units** that aren't in an inventory (`0x00452060`). Spawn one if
  the count is below the cap.

When it runs:

- **Only while the generator is off screen.** OnScreen `0x00517660` clears
  `+0xf0`, and OffScreen `0x00517640` sets it. The map pane's animate pass
  (`0x00458750`) calls them each drawn frame: off screen is the generator's
  screen rect missing the 640×480 view, grown by 192 pixels on every side
  and anchored on the 64-pixel update grid. A generator whose sector the
  pass doesn't reach keeps its last state. In world units that is roughly
  450–500 up or down the screen from the view centre (along x + y) and 300
  each of x and −y across it, about 420 [C, measured in the port].
- Only in a loaded sector.
- Not in the editor, on a multiplayer client, or with the `0x668110`
  "dummies" toggle set.

What a spawn does (`0x00517240`):

- It makes up to 20 tries for a walkable spot 50–150 units from the
  generator.
- The monster gets a random facing and full char.def health: the spawn
  doesn't touch health, so it starts at its type's (a spawned Kantha has
  70) [C].
- It gets treasure by the slot's treasure type (§4.1).

Every shipped slot's frequency is 7000–9100, which is **70–91 s**. Caps are
1–3.

So a generator yields at most one monster per slot every 70–91 s. It needs
Locke far enough away that it is off screen, and fewer than its cap of
characters within 300 units of the spot.

Unconfirmed:

- Whether retail keeps a 3×3 or a 5×5 block of 1024-unit sectors loaded
  around the camera (EXITS.md).
- Whether a fading corpse counts toward the cap. Likely, since the count
  filters only "in an inventory" [L].

Four slots can never spawn, because their names match no type [L]:

- `"{mongrel druhg"` at (5943, 20515), level 0;
- `"pale ogork "` at (16291, 19917), level 0;
- an empty generator at (15699, 20671), level 0;
- `"ogrok   "` on level 32.

**The port has generators** (`TMonsterGen`, `src/monstergen.cpp`) [C]: the
pulse, spawn, kit, sector and save layout, and the `addmonstertype`,
`delmonstertype`, `maxmonsters` and `monstertypes` commands, as above. It
pulses the generators of the 3×3 sector window around the camera. It has
no multiplayer, so a server's player-distance test and spawn throttle
aren't ported. Its corpses drop their loot and fade out as retail's do
([FINDINGS.md](FINDINGS.md) F8, F9).

**The base map's supply** (hostile = `Aggressive` 1 and Value ≥ 0):

| Level | Placed hostiles | Exp if all killed at L1 | Generators (sum of caps) | Gold the monsters carry |
|---|---|---|---|---|
| 0 (outdoors) | 138 | 7,340 | 109 (178) | 4,443 |
| 3 (tower) | Sidious only | — | 0 | |
| 4 (Arakna island) | 50 | 3,580 | 14 (30) | 4,385 |
| 30 (first cave) | 45 | 5,940 | 18 (25) | 9,026 |
| 31 | 72 | | 7 | 18,523 |
| 32 | 27 | | 6 | 6,860 |
| 36 | 58 | | 13 | 15,133 |
| 39 | 28 | | 6 | 7,363 |

Levels 1, 2 and 6 have no hostiles. Rahul on level 2 is the opening's
scripted fight.

### 3.2 How strong each early type is

**Health:**

- A placed monster's health is its sector `HLT`, clamped to char.def
  `HEALTH` by `TCharacter::Load` (`0x004d4eb0`).
- Spawned monsters start at char.def `HEALTH` [L].
- class.def's `Health 25` is only the default for types with no HEALTH line.
- Some placed monsters start hurt: Vashar 75–103, Lurker 137 and 183,
  Under Druhg 44.

**Behaviour:**

- A monster attacks on sight only while its `Aggressive` stat is 1
  (COMBAT_ATTACK_CHOICE §3.10, §3.14). Sight is 640 for every type below.
- class.def's third column is the type's unique id, not a flag.
- char.def sets no WALKSPEED or RUNSPEED, so speed comes from the
  animations [U].

The table runs easy → hard for a level-1 Locke (100 HP; Offense and Defense
10). Notes on the columns:

- **V** is from class.def TYPES (`class.def:3003-3112`). **HEALTH** is at
  the char.def line given.
- **`hand`** is the resistance to fists. **Spells** lists the non-zero
  spell-type resistances.
- **To-hit** is each side's value before the +25 facing bonus, with the
  chance of a hit in brackets (§2.4). Locke's is `60 − 12V`, the monster's
  `40 + 12V`, both clamped to 10–100.

| # | Type | V | Exp at L1 / L3 / L5 | HEALTH | WEAPONDAMAGE | Reach | `hand` | Spells | To-hit (P hit): Locke / monster |
|---|---|---|---|---|---|---|---|---|---|
| 1 | Araknid | 1 | 10 / 5 / 1 | 30 (`char.def:4600`) | 8 | 0–11 | 0 | magical +10 | 48 (46%) / 52 (54%) |
| 2 | Under Druhg | 1 | 10 / 5 / 1 | 50 (`:6100`) | 8 | 0–80; retreats at 18 HP | 0 | — | 48 (46%) / 52 (54%) |
| 3 | Mongrel Druhg | 2 | 20 / 8 / 5 | 70 (`:6190`) | 11 | 0–80 | 0 | — | 36 (26%) / 64 (74%) |
| 4 | Kantha | 2 | 20 / 8 / 5 | 70 (`:8473`) | 11 | 0–100; charge 55–100 | 0 | — | 36 (26%) / 64 (74%) |
| 5 | Thrush Druhg | 3 | 40 / 16 / 7 | 80 (`:6281`) | 13 | 0–80; rollsweep 80–150 | **+20** | — | 24 (12%) / 76 (88%) |
| 6 | Arakna | 3 | 40 / 16 / 7 | 120 (`:4483`) | 12 | 0–17; casts SpiderPoison | 0 | magical +10 | 24 (12%) / 76 (88%) |
| 7 | Artikulid | 4 | 60 / 33 / 11 | 75 (`:4821`) | 16 | 0–10 | 0 | magical +10 | 12 (3%) / 88 (97%) |
| 8 | Hopper | 4 | 60 / 33 / 11 | 131 (`:7701`) | 18 | 0–85 | 0 | poison −100 | 12 (3%) / 88 (97%) |
| 9 | Styxx (undead) | 4 | 60 / 33 / 11 | 131 (`:9912`) | 14 | 0–60 | −5 | burn +20, poison −100 | 12 (3%) / 88 (97%) |
| 10 | Recluse | 5 | 80 / 66 / 23 | 114 (`:4706`) | 20 | 0–17; SpiderPoison | 0 | magical +10 | 10 (2%) / 100 |
| 11 | Ramage | 5 | 80 / 66 / 23 | 126 (`:9127`) | 22 (sword) | 0–200 | 0 | poison −10 | 10 (2%) / 100 |
| 12 | Pale Ogrok (club / mace) | 5 | 80 / 66 / 23 | 115 / 145 (`:4931` / `:5072`) | 19 / 22 | 0–60 | +10 | — | 10 (2%) / 100 |
| 13 | Acolyte | 6 | 100 / 99 / 46 | 102 (`:7038`) | 14 | 0–60; heals, tornado, bolt | +10 | −10 magic, burn, freeze | 10 (2%) / 100 |
| 14 | Earth Golem | 6 | 100 / 99 / 46 | 306 (`:6692`) | 28 | | **−25** | burn +10, freeze +10 | 10 (2%) / 100 |
| 15 | Skeleton | 7 | 120 / 132 / 92 | 160 (`:7794`) | 28 | | +5 | burn +20 | 10 (2%) / 100 |
| 16 | Vashar | 8 | 140 / 165 / 138 | 110 (`:9026`) | 23 (sword) | 0–100 | +10 | — | 10 (2%) / 100 |
| 17 | Lurker | 8 | 140 / 165 / 138 | 230 (`:6395`) | 32 | | −10 | burn, freeze +10; poison −50 | 10 (2%) / 100 |

Every level-up adds 10 to both of Locke's numbers, so each row gets about
one level easier per level Locke gains.

Further on:

- Ogrok variants: V8; 146, 155 or 196 HP; WEAPONDAMAGE 22–32.
- Issathi: V9, 180 HP, WEAPONDAMAGE 40.
- Cinderate: V9, 144 HP.
- Monk: V10, 234 HP.
- Widowett: V7, 640 HP; summons Arakna.

Bosses:

- Sidious: V5, 800 HP (`char.def:9731`). Only 80 Exp at level 1.
- Baez: V8, 650 HP.
- Solifuge: V14, 5000 HP.

### 3.3 Where, on level 0

Reference points:

- Locke leaves the Keep at (1842, 24388).
- Misthaven is the rect (765, 24974)–(3664, 27888), south of the gate. It
  holds no hostiles.
- Jong1 is at (965, 29901).

Distances are straight lines from the gate, ignoring walls.

| Area | Centre | From the gate | Placed monsters | Generator slots | Notes |
|---|---|---|---|---|---|
| R1, north road | (2078, 22068) | 2.0–2.7k | none | 4 Kantha (caps 2, 1, 2, 3) | respawns only; Value 2 |
| R2a, north-east of the gate | (3749, 23447) | 1.8–2.1k | 1 Arakna, 2 Under, 1 Mongrel; Styxx at its tomb (4285, 23050) | artikulid, arakna, mongrel, styxx, kantha (cap 7) | |
| R2b, east of the gate | (4521, 25180) | 2.4–2.8k | 8 Araknid, 1 Under | araknid ×2, under ×2 (cap 9) | densest at (4258, 24767): 3 Araknid and an araknid generator (cap 3) |
| R2e, south-east of town | (4711, 27645) | about 2.7k from (2912, 29630) | 8 Araknid, 2 Under, 1 Arakna | araknid ×4, under ×1 | |
| R2c, north-east woods | (5606, 22854) | 3.3–4.1k | 6 Mongrel, 4 Araknid, 2 Under, 1 Arakna, 1 Kantha | 6 | a knot at (5373, 22720): 9 monsters and 3 generators |
| R2d, hermit and nest | (5856, 25553) | 3.4–4.2k | 5 Under, 5 Araknid, 4 Arakna | 8 | Arakna nest at (5942, 26476): 4 Arakna and 3 generators. Olihoot1 is at (5837, 24805) |
| R4, Druhg camp (S2) | (2882, 16865) | 6–7.6k north | 8 Thrush, 8 Mongrel, 5 Under, 2 Hopper, 2 Vashar, 1 Kantha | 12 | multi-slot generators at (3088, 16160) and (3092, 15714), by jason1 |
| R3, northern forest band | (4341, 20173) | 3.9–4.9k | 2 Vashar, 1 Lurker, 1 Hopper, 2 Mongrel, 1 Thrush, 1 Kantha | 19 (hopper ×6, thrush ×5, …) | |
| R5, east forest | | | 7 Vashar, 3 Ramage, 2 Acolyte, 1 Lurker, 3 Hopper, 2 Pale Ogrok | includes 2 green dragon (cap 1, treasure 3) and an earth golem | harder |
| R6, Ogrok lands | more than 10k east | | 9 Pale Ogrok, 4 Vashar, 5 Skeleton, 3 Acolyte, 2 Earth Golem, 2 Lurker, 2 Ramage, 1 Ogrok | | around the arena |

Later levels:

- **Level 4** (Arakna island, after S4): 26 Recluse (V5), 23 Artikulid
  (V4), Widowett at (8099, 10137), and 14 generators.
  - A Recluse stands 670 units from the arrival point (10000, 10000).
  - An artikulid generator is at (9722, 9080).
- **Level 30** (the first cave, after S6): only V6–10 monsters. Two Vashar
  and a vashar generator are at (9900, 10609), near the arrival point
  (10560, 10496).

Sources for §3.1–3.3: the base map decoded with `storytest.scan_level` and
`revsave`, and the generator records decoded with the layout of Load
`0x00516e00`.

### 3.4 Don't kill, don't provoke

These add to STORY.md §6's list (Gus*, jason1, Gatekeeper1, CHIEF1, CHIEF2,
shegra1, the slaves, SardokT):

- **Baez** (12482, 16986) is already `Aggressive` 1 in the base map.
  CHIEF2's ALWAYS block checks only `MISTSTATE < 5` and
  `BAEZ.STAT HEALTH < 1` (`forest.s:2280-2331`). Killing him early would
  likely play the arena's ending and set MISTSTATE 5 out of order [L].
  Whether Locke can reach him before the gatekeeper lets him in is [U].
- **MUDOKON1**, Jong's dummy at (1155, 29902): `Aggressive` 1, Value 1.
  Its own script resets its health to 1800 every second
  (`forest.s:1828-1832`). It can't be killed and gives nothing. Keep the
  director's target picker off it outside a spar.
- **JONG1** (V30, 725 HP, not aggressive), and GUSDUMMY, an invisible
  Jong-type script object at (2079, 27465).
- **The arena spectators.** All are `Aggressive` 0 and are deleted at
  MISTSTATE 5 (`forest.s:2316-2326`, `39-41`). Leave them alone:
  - Ogroks mark, redo, jason2, riz, carl;
  - Pale Ogroks vince, austin, paul, hersh;
  - Dark Ogroks matt and rex (V19, about 470 HP, WEAPONDAMAGE 54);
  - chick1–3;
  - an unnamed Dark Ogrok at (13984, 17274), by the cave mouth.
- **Sidious and the Solifuge must die** for the story.
- **No other dormant or cube-woken monsters on levels 0–6 and 30–39.**
  Every invisible character there is an NPC or a script dummy. The only
  cubes that wake monsters are SOLDUMMY (the Solifuge) and AUSTIN (Baez).

## 4. Loot

### 4.1 What a dead monster leaves: everything, on the floor [C]

**When the drop runs.** TCharacter::Pulse calls DropInventory `0x004d6570`
(at `0x004c21e4`) on every pulse in which all of these hold:

- the character's health is below 1;
- charflags `& 8` (can't die) is clear;
- it isn't the player (single player);
- it carries at least one item (`0x00470040`).

**What DropInventory does:**

- It plays the "gold" sound.
- It takes every tradeable item (`Value ≠ −1`) out of the corpse and puts it
  on the map:
  - **Gold** goes to the corpse's (x + 30, y + 30), with no walk check.
  - **Every other item** goes in a random direction, in 16-unit steps that
    must be walkable. There are up to `2·(Radius/16) + 1` steps, so items
    land up to about 48 units away for most monsters, and 80 for the
    Solifuge. The walk test checks the spot a step leaves, so the item lies
    one step past the last spot that passed. Each item then plays its class
    sound (`0x00473a10`).
  - The item is **thrown** there (`TObjectAnimator::Toss` `0x00445ef0`, its
    pulse `0x00445b20`) [C]: from 60 above the corpse (armour from the
    corpse's own height), it lands raised by its registration height + 2,
    settles a step short of its spot about 1.5–2.5 s later and faces where
    its spin left it. Every item has an animator for this, 2D or 3D
    (`TAnimImagery::NeedsAnimator` `0x00418fc0`). The observation shows the
    item at its spot at once and at its resting place once it settles.
- Items with `Value −1` stay in the corpse. No placed monster on any of the
  33 levels carries one, so every corpse empties.
- The corpse fades and is deleted 14.2 s later (§3.1). The items on the
  floor are ordinary map objects, so they are probably saved with the
  sector and survive a reload [L].
- In retail, the solifuge key (a `Copper Key`, Value 0) lands on the floor
  when the Solifuge dies. STORY.md S7's "take it from the body" is the
  port's behaviour.

**Placed monsters carry fixed loot.** Each drops what its sector record
holds: designer-placed gold, and sometimes food or a potion. Gold on level
0's placed monsters [C, decode]:

| Type | Gold |
|---|---|
| Araknid | average 34, max 97 |
| Pale Ogrok | average 51, max 184 |
| Mongrel Druhg | average 28 |
| Under Druhg | average 18 |
| Thrush Druhg | average 18 |
| Lurker | average 85, max 234 |
| Ramage | average 70 |
| Acolyte | average 55 |

On level 4, Recluses average 131 and Artikulids 68; Widowett carries 400.

**Spawned monsters get a random kit at spawn** (`0x005169b0`, tables at
`0x005e1fb4`–`0x005e2068`):

- 7% a Pouch.
- 30% one food, from a table of 16.
- A potion, in a cascade: Lesser at L%; if not, Greater at G%; if not,
  Enchanted at E%. The kind is Healing, Mana or Restoration at random.
- Cure Poison at C%. The game asks for `"Cure Posion"`, which matches no
  type, so this probably never appears [L].
- Gold in the tier's range.

The tier is the generator slot's treasure type. Tier 0 gives nothing.

| Tier | L% | G% | E% | Cure% | Gold |
|---|---|---|---|---|---|
| 1 | 5 | 0 | 0 | 5 | 0–50 |
| 2 | 7 | 5 | 0 | 7 | 25–75 |
| 3 | 10 | 7 | 0 | 10 | 50–125 |
| 4 | 7 | 10 | 5 | 10 | 50–175 |
| 5 | 5 | 10 | 7 | 10 | 75–200 |
| 6 | 0 | 7 | 10 | 10 | 100–300 |
| 7 | 0 | 5 | 10 | 10 | 100–500 |

Level 0's slots are tiers 1–3: 78 at tier 1, 28 at tier 2 and 7 at tier 3.
For example, Druhgs are tier 1, and Lurkers and green dragons tier 3.

**The port** drops and throws as retail does ([FINDINGS.md](FINDINGS.md)
F8). Its renderer doesn't draw the flight (the item shows at its spot
until it settles), the gold pile, or a 2D item put on the map in play, so
a player sees less loot than is there; the observation lists it all. Its
`TCharacter::Use` still takes one item per click from a corpse
that holds any; retail's corpse-looting path (`0x004d4a60`) has an inverted
free-slot test and never works. Neither matters while corpses empty
themselves.

### 4.2 Picking things up [C]

**With the mouse** (`TMapPane::MouseClick` `0x0044f140`;
[MAP_INPUT.md](../gameflow/forensics/MAP_INPUT.md) §4):

1. Hover the item until the **hand** cursor shows. The hover pick is
   redone every 8th frame (`0x00454450`), so step a few ticks and check the
   observation's `cursor`.
2. Left press and release on the same object:
   - Within 96 units: `Pickup` (`0x004cfef0`) at once.
   - Farther: Locke walks to a point 64 units short of the item, if the
     straight line is walkable, and picks it up on arrival. Otherwise the
     text bar says "You are too far away."
3. **Combat mode is fine.** A left press attacks or targets only when the
   hover cursor is none or "swords". With the hand showing, the click
   picks up.
4. The 3D pick prefers meshes, so a corpse drawn over an item takes the
   click. Point at uncovered pixels of the item [L].

**With keys** (retail control table at `0x005d5500`; INI bindings from
`data/Revenant.ini`):

- **G** (Get, and Combat Get) works in normal, combat and bow modes. It
  runs `0x004d0430`:
  - It looks 40 units ahead along Locke's facing, within an 80-unit
    sphere, for up to 10 candidates.
  - The nearest candidate wins: anything with a cursor, or an inventory
    item, that isn't Locke or invisible.
  - It prints "Get <name>" and picks the item up.
  - A nearer door, NPC or chest silently blocks G, because Pickup only
    takes inventory items. With no candidate it says "There is nothing to
    get".
- **U** (Use; normal mode only, `0x004d01e0`) looks 60 units ahead, within
  60, for a container, an exit or a non-enemy character.

Pickup's messages: "29 Gold Picked up.", "Short Sword Picked up.", and on
failure "Can't carry any more." (CARRYFULL2).

**Port:** U, G and Combat Get work as in retail ([FINDINGS.md](FINDINGS.md)
F10).

**Containers** (`TContainer::Use` `0x004dc180`). The hand cursor shows when
a container is Openable or holds items (`0x004dd370`). Each use does one
thing:

1. A key or lockpicks dropped on it runs `CheckKeyUse` `0x004dd480`.
2. If it is locked: "It seems to be locked."
3. If it is Openable and closed: it opens, with its open sound. A trap flag
   (Explosive or Poisonous) fires a fireflash or a poison cloud.
4. If it is open: **one item per use** goes into the inventory pane's
   current container (the pack, or the bag being viewed), with "<item>
   taken from <name>." If there is no room, "Can't carry any more." and
   nothing moves.
5. If it is empty and Openable: it closes.

The strings and branches are [C]. The order of the steps is [L]: the
decompile is garbled, but it matches the 1998 code.

- Chests are Openable. Bookcases, crates, buckets and stands aren't, so you
  take from them directly.
- The manual's tip: "Keep pulling items until the chest shuts" (MANUAL.md,
  Tips).

### 4.3 Pack capacity, merging, a full pack [C]

- **Capacity.** The pack has 255 top-level slots (0–0xfe;
  [INVENTORY.md](../gameflow/forensics/INVENTORY.md) §2). Bags nest more.
- **No weight or encumbrance.** No such string exists in the exe or the
  defs.
- The pane shows 12 cells (4×3) per page.
- **Merging** (INVENTORY.md §3):
  - Gold joins the first "Gold" item, bags included.
  - Food and potions join the first item of their type.
  - Weapons, armour and ammo never merge.
  - A new item with no slot goes into a Pouch whose first item is of the
    same class.
- **When the pack is full:**
  - Pickup removes the item from the map **before** the add fails, and
    nothing puts it back: "Can't carry any more." and **the item is lost**
    [C; emulator, [FINDINGS.md](FINDINGS.md) F11]. Gold too: the free-slot
    test comes before the merge into Locke's pile [C]. The port does the
    same since F11.
  - A take from a container checks for room first.
  - A shop buy needs an empty slot (§7.2).

### 4.4 Gold [C]

- Gold is an item: class MONEY, one type "Gold" (`class.def:544-567`), with
  an object stat `Amount`.
- The HUD figure is `GetInventoryAmount("Gold")`, which counts bags too.
  The observation's `player.gold` is the same value.
- It is picked up like any item and merges into Locke's pile.

**Where the gold is: in chests, more than on monsters** [C, decode]:

- Level 0's containers hold about **16,600 gold**, against 4,400 on its
  placed monsters.
- The Keep (level 2) chests hold 500 in all: 250; 100 and a Lesser
  Healing; 150 and an Apple.
- Unlocked gold chests on level 0:

| Position | Gold |
|---|---|
| (1658, 22072) | 1000, about 2.3k north of the gate on the R1 road |
| (5740, 24394) | 500 |
| (4044, 17822) | 500 |
| (1712, 17152) | 1500 |
| (10323, 16367) | 1000 |
| (10882, 18472) | 1500 |
| (15244, 17106) | 1000 |
| (8704, 16348) | 1500 |

- Gold in chests that also hold gear:
  - ChestE (2768, 15760): 1500;
  - ChestS (9820, 16180): 2840;
  - ChestE (12615, 21103): 1000.
- Locked chests (`PICK 1`) need lockpicks: (13672, 19970) holds 1500, and
  ForChestE (3884, 22848) 1000.

Looting the chests early pays for armour that a monster grind takes hours
to buy.

## 5. Equipment

### 5.1 Where an item's numbers come from [C]

class.def's TYPES rows are only defaults. At load, BindTypes (`0x0048cab0`
→ `0x0048c7f0`) overwrites each type's stats with its `weapon.def` or
`armor.def` BASICMODS, matching names case-insensitively:

- Weapons (`0x0048af30`): EqSlot, Type, Damage, Combining, Poison, Value,
  DamageType (the column the file comments as "damagemod"), MinStrength.
- Armour (`0x0048b4a0`): EqSlot, Protection, Combining, ResistPoison,
  Stealth, Value, MinStrength, MinConstitution.
- SaleType stays as class.def has it.

**Use the `weapon.def` and `armor.def` numbers.** Examples:

- The Short Sword is worth 195, not class.def's 100.
- The Tulwar's Damage is 35, not 42.
- Animal Fur needs Str 16 and Con 14, not 18 and 16.
- Every shipped weapon's DamageType is 0 (misc), so weapon hits ignore
  resistances (§2.4).

Only armour has STATLINEs. While an item is equipped, its line is applied to
Locke's modified stat copy (PLAYER_STATS §2–3).

The port makes the same copy (`TRules::BindTypeData`, `src/rules.cpp`;
FINDINGS F6). Before that, its prices, damage values and Gina's and Cronus's
stock came from class.def.

### 5.2 What Locke may equip: CanEquip `0x00519300` [C]

CanEquip(item, slot) is called by Equip `0x005199b0`, and RefreshEquip
`0x00519230` re-checks it. It requires:

1. The item's `EqSlot` stat equals the slot.
2. For a **weapon**:
   - Locke has the combat animation for it;
   - the class weapon mask allows its type (Revenant allows all,
     `rules.def:302`);
   - **MinStrength ≤ Strength**. Otherwise: "This item is not equipable.
     ( Min Strn: N )".
3. For **armour**: **MinStrength ≤ Str** and **MinConstitution ≤ Con**.

The attributes come from the modified copy, which includes bonuses from
equipped items and is capped at 30. So the Earthen Gauntlets' STRN +2
likely count toward MinStrength [L].

**Equipment slots** (`src/player.h:25-35`; retail has 11):

| Slot | Name |
|---|---|
| 0 | head |
| 1 | neck |
| 2 | chest |
| 3 | gloves |
| 4 | weapon |
| 5 | bracelet |
| 6 | ring, also light sources |
| 7 | bow |
| 8 | ammo |
| 9 | legs |
| 10 | boots |

**How to equip and unequip:**

- **Equip.** Right-click (on button up) an item in the pack. Key `5` shows
  the pack (`Invent=5` in `data/Revenant.ini`). The item goes to its own
  slot (`0x00538210`), and the slot's previous item takes its pack cell.
  Dropping an item anywhere on the equip pane does the same
  ([PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md) §6).
- **Unequip,** for example to fight barehanded. Drag the item from its slot
  on the equip pane (key `3`) onto the pack below it
  ([EquipPane_SPEC.md](../ui/forensics/EquipPane_SPEC.md) §1). The
  observation doesn't give the equip pane's slot positions (§8).

**Choosing what to wear:**

- **Weapons:** by Damage. Every weapon deals misc, so there are no type
  matchups; the style rule is in §9.
- **Armour:** by Protection per slot. Each point takes 1 off every hit Locke
  takes, and monsters hit for 8–32 early.
- **Accessories:** by their STATLINE.

### 5.3 Locke's start [C]

- `newgame.sav`'s inventory is empty.
- The opening adds the Life, Moon and Soul talismans and the spell pouch,
  the Short Sword, Black Cloth Boots, Brown Cloth Pants and Red Cloth Shirt
  (`keep.s:616-629`), plus the scroll MGCSCRL1 (`player.get MGCSCRL1`).
- They land in the pack, and the player equips them.
- Shirt and pants give Armor 2; cloth boots give 0.
- After the opening Locke has **Str 18, Con 12** (PLAYER_STATS §6). That
  allows:
  - weapons up to MinStrength 18;
  - **cloth and leather armour only.**
- With the default markers (+1 Str and +1 Con per level), Con reaches 14 at
  level 3, which allows Studded and Animal Fur; 16 at level 5, which allows
  Chainmail and Woodland; and 18 at level 7, which allows Bone and
  Barbarian.

### 5.4 Early weapons (`weapon.def`) [C]

| Weapon | Type | Damage | MinStr | Value (sells for 30%) | SaleType | Where |
|---|---|---|---|---|---|---|
| Training Sword | sword | 2 | 16 | 30 | 2 (never sold) | Jong's lessons |
| Short Sword | sword | 13 | 16 | 195 | 0 | the start kit; Gina |
| Club | bludgeon | 14 | 16 | 210 | 0 | Gina, level 1+ |
| Bone Pickaxe | axe | 15 | 16 | 225 | 0 | Gina, level 1+ |
| Stone Axe | axe | 17 | 16 | 255 | 0 | Gina, level 1+ |
| Ball Racker | bludgeon | 18 | 16 | 270 | 1 | level 0, ChestS (3218, 15576) |
| Studded Bat / Spiked Bat | bludgeon | 19 / 21 | 17 | 570 / 630 | 0 | Gina, level 3+ |
| Bone Sickle | axe | 22 | 17 | 660 | 1 | level 0, ChestE (2768, 15760) |
| Maul / Morning Star / Sickle | bludgeon / bludgeon / axe | 25 / 30 / 30 | 18 | 1500–1800 | 0 | Gina, level 6+ |
| Tulwar | sword | 35 | 18 | 2100 | 1 | level 4, RunChestE (8795, 9343) |
| Battle Axe / Flanged Mace / Pointed Maul | axe / bludgeon / bludgeon | 39 / 40 / 42 | 18 | 2340–2520 | 0 | Gina, level 6+ |
| Soul Cleaver | sword | 45 | 19 | 4050 | 1 | the arena reward (S6) |
| Long Sword / Broadsword | sword | 55 / 60 | 19 | 4950 / 5400 | 0 | Gina, level 9+ |
| Wyrmm's Tooth Sword | sword | 150 | 25 | 67500 | 1 | level 2 Chest (12705, 10333, 304), locked, KEY 33 |

- Daggers, the OgrokAxe and the quest blades exist only in class.def, with
  placeholder rows (MinStrength 48 or EqSlot −1). They can never be
  equipped.
- The staves (Oak Branch … Iron Bo) are bludgeons with Damage 10–23 and
  MinStrength 12–15 (`weapon.def:286-328`). They are SaleType 1.

### 5.5 Early armour (`armor.def`) [C]

| Set | Protection: gloves / helm / chest / legs / boots | Total | MinStr / MinCon | Cost of the full set | Lines |
|---|---|---|---|---|---|
| Cloth (shirt, pants, boots) | – / – / 1 / 1 / 0 | 2 | 14 / 10 | 30 | `armor.def:8-85` |
| Leather | 1 / 1 / 2 / 2 / 1 | 7 | 15 / 12 | 1750 | `:94-192` |
| Studded | 1 / 2 / 3 / 3 / 1 | 10 | 16 / 14 | 5000 | `:265-363` |
| Animal Fur (DmgResPoison 6) | 1 / 2 / 3 / 3 / 1 | 10 | 16 / 14 | 7500 | `:222-257` |
| Woodland (`Fatigue %4`) | 1 / 2 / 3 / 3 / 1 | 10 | 18 / 16 | 15000 | `:372-407` |
| Chainmail (AGIL −1 on chest, legs and boots) | 2 / 3 / 4 / 4 / 2 | 15 | 18 / 16 | 14000 | `:415-570` |
| Bone (chest, legs, boots only) | – / – / 5 / 3 / 2 | 10 | 19 / 18 | 5000 | `:199-216` |

- **Leather is the first big upgrade:** +5 Protection over the starting
  cloth for 1750 gold, wearable from level 1.
- **Animal Fur ×5 lies free** in level 0's ChestS (3218, 15576), wearable
  from Con 14.
- Woodland ×5 lies in level 4's RunChest (5672, 10396).

**Accessories with no requirement** (MinStr and MinCon 0):

| Item | Slot | Effect | Value |
|---|---|---|---|
| Ivory Pendant | neck | ACBonus 2 (+2 armour) | 500 |
| Bracelet of Fortune | bracelet | LUCK +2 | 750 |
| Emerald Ring | ring | DmgResPoison 10 | 800 |
| Serpent Ring | ring | MaxHealthFlat 100 | 1000 |
| Spirit Pendant | neck | two STATLINE lines, CONS +2 and DmgResFreeze 60; whether the record keeps both is [U] | 1500 |
| Earthen Gauntlets | gloves | Protection 2, STRN +2 | 2000 |
| Fenrus Collar | neck | MaxManaPct 50 | 2000 |
| Amulet of Thunderbolts | neck | DamageMod +10 | 2500 |
| Band of Regeneration | bracelet | HealthPct 60 (+60 HP a minute) | 3000 |
| Cestus of Slicing | gloves | Protection 2, EdgeBonus 25 (blades only) | 5000 |
| Amulet of Power | neck | DamageMod +20 | 10000 |
| Band of Shadows | bracelet | Hands +15 [L] (§2.5) | 800 |

Hruthford sells some of these (§7.1). Free ones lie in chests (§5.6).

### 5.6 Gear in containers, levels 0–4 (base map; unlocked unless marked) [C]

| Level | Container (position) | Contents |
|---|---|---|
| 0 | ChestS (3218, 15576, 40) | Animal Fur ×5, Ball Racker |
| 0 | ChestE (2768, 15760, 40) | Earth talisman, 1500 gold, Bone Sickle, magic scroll 2 |
| 0 | ChestS (9820, 16180, 69) | Blue Scalemail ×5, 2840 gold, Greater Restoration, Enchanted Mana |
| 0 | ChestS (13312, 17520, 38) | Band of Regeneration, magic scroll 3 |
| 0 | ChestE (12615, 21103, 48) | Fenrus Collar, 1000 gold, Cure Poison, Sausage |
| 0 | RunChestE "STYXXCHEST" (4268, 23080, 39) | Emerald Ring |
| 1 | ChestS (11271, 8477, 144) | Gothic armour ×5; locked, KEY 11 |
| 1 | ChestS (4199, 19572, 170) | Pouch, 22 gold |
| 2 | ChestE (9587, 10459, 432) | Gothic Leg Plate; locked, KEY 0 |
| 3 (tower) | chests | Diamond Ring and Ocean (4724, 5092); Fenrus Collar and 1000 gold (5097, 4729); **Lockpicks** and 1500 gold (4910, 4708) |
| 4 (Arakna) | RunChests | Woodland ×5 (5672, 10396); Tulwar (8795, 9343); Amulet of Channeling and 500 gold (7589, 9583); Amulet of Rogoss and Stars (8579, 9933) |

### 5.7 What not to sell or drop [C]

- **Items with Value −1 can be neither sold nor dropped.**
  - Sell lists skip Value −1 and SaleType 2 (`0x00531b70`).
  - A drop on the map returns early for an item that can't be traded
    (`TMapPane_MouseClick_44f140.cpp:578-579`).
  - This covers the Golden Sun Key, the Diamond Ring, the Amulet of Rogoss,
    the vials, the crystals, the Sacrificial Dagger and the Lockpicks.
- **Keys can't be sold**, because no shop handles class 16.
- **Keys with Value 0 can be dropped**, and losing one is the real risk.
  That covers the solifuge key, the Temple, Sanctuary and Guardroom keys,
  and the Silver and Gold keys.
- The Soul Cleaver can be sold (Value 4050). STORY.md §6 doesn't list it as
  a keep.

## 6. Healing, rest and death

### 6.1 Regeneration [C]

TCharacter::Pulse `0x004c1bb0` does the regeneration; TPlayer::Animate
`0x00518aa0` calls it first. The decompile is in
`recon/discovered/cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp:563-800`,
whose filename label is wrong.

Each value has an accumulator. Every pulse adds to it, in units of
1/10000 point, according to the play-clock time that passed. The rates come
from rules.def (Rules `+0x74` … `+0x88`, parsed at `0x0048c30f`):

| Value | rules.def | Rate | When |
|---|---|---|---|
| Health | `HEALTHDATA 25, 1, 6000` | **about 1 HP a minute**, plus `HealthPct` (stat 21) HP a minute (`0x004c2b81`, `0x004c2bb2`) | below max and alive; **in combat or not, walking or running**; not while paused |
| Mana | `MANADATA 25, 1, 6000` | about 1 MP a minute, plus `ManaPct` (stat 22) a minute | below max |
| Fatigue | `FATIGUEDATA 3, 10, 75` | **about 13 a second**, plus `FatiguePct` (stat 23) (`0x004c2ee0`). Locke's 78 refills in about 6 s | whenever Locke isn't running, attacking included |
| Running | Locke's `RUNFATIGUE 10, 100` (`char.def:127`) | **−10 a second**, with no fatigue regeneration (`0x004c2e26`) | while the action's state is `run`, `combatrun`, `handrun` or `bowrun` |
| Poison | `POISONDATA 6, 100` | −1.44 HP a second × (100 − DmgResPoison)/100 (`0x004c3002`) | while poisoned. Never below 1 HP, and it cures itself under 2: **poison can't kill** |

- **Passive health and mana don't matter for a grind.**
- **Fatigue is the fast clock**, and it paces a fight. A barehanded blow
  costs 12–24 (§2.5).
- Band of Regeneration gives +60 HP a minute.
- The Regeneration spell (`HealthPct 120 TIME 2880`) gives +120 a minute
  for two minutes.

### 6.2 Potions and food [C]

`TFood::Use` `0x0050e780` handles both (the FOOD and POTION vtables
`0x005b2cd4` and `0x005b2ee4`, slot `0xbc`):

- The base Use runs first (the item's and the user's USE triggers). A dead
  character can't eat; nothing is used with another object (`with` must
  be −1), nor an item whose state isn't 0.
- Health += the item's Health and Mana += its Mana, each capped at the
  maximum.
- An item with Poison ≠ 0 cures poison: the user's Poisoned mark is
  cleared, and every effect within 0x80 of the user whose spell has the
  user as its first target and DT_POISON as its damage type is ended
  (`KillThisEffect`), so a running Poison spell stops re-poisoning.
- The item's Fatigue stat is **never applied**.
- The player hears "food" for an item whose imagery has an inventory icon
  (`GetInvImage(0, 0)`), else "potion".
- In an inventory, the amount drops by one, and the last unit is deleted
  from its owner by name (`DeleteFromInventory(name, 1)`). Nothing is left
  behind, whatever the manual says about apple cores and bottles. An item
  on the map is not used up.
- The text bar shows `FOODGOOD1` ("The item was consumed. You feel
  rejuvenated.") when the user is the player.

Ported 2026-10-13 (`TFood::Use`, src/food.cpp); a potion is a `TFood`.

| Item | Where | Buy / sell, gp | Effect |
|---|---|---|---|
| Lesser Healing / Lesser Mana | Elahni | 250 / 75 | +100 H / +100 M |
| Lesser Restoration | Elahni | 500 / 150 | +100 H, +100 M |
| Greater Healing / Greater Mana | Elahni | 900 / 270 | +400 H / +400 M |
| Greater Restoration | Elahni | 1750 / 525 | +400 H, +400 M |
| Enchanted Healing / Enchanted Mana | Elahni | 1500 / 450 | +800 H / +800 M |
| Enchanted Restoration | Elahni | 2750 / 825 | +800 H, +800 M |
| Cure Poison | Elahni | 100 / 30 | cures poison |
| Meat / Fish | Hruthford | 150 / nobody buys food | +50 H / +50 M |
| Bread / Apple / Cheese | Hruthford | 120 / 100 / 100; nobody buys food | +40 H / +10 H +20 M / +20 H +10 M |

Sources: `class.def:385-396` and `3535-3544`. Healing costs 1.9–2.5 gold per
HP at Elahni.

**How to use an item:**

- **From the pack.** Left press and release on the item's cell, moving less
  than 2 pixels, calls `item->Use(player, −1)` (`0x00538210`). The pane is
  at (452, 306), and cell (c, r) is centred at (480 + 45c, 368 + 44r)
  ([InventoryPane_SPEC.md](../ui/forensics/InventoryPane_SPEC.md) §3, §10).
  That centre is the observation's `pack[].screen` point.
- **From the belt, F5–F9** (commands 24–28; dispatch `0x0047cf40` →
  TPlayer `0x0051e420`(n)). The key acts on the item in belt slot
  `0x10b + n`:
  - a weapon or armour is equipped;
  - anything else is Used, so a potion **directly** in a belt box is drunk;
  - a Pouch in a belt box opens instead of drinking [L] (`0x004dc180`; worth
    an emulator check).
- **Getting an item onto the belt.** Drag it from a pack cell onto box N, at
  about (220 + 45N, 430)
  ([BarInvPane_SPEC.md](../ui/forensics/BarInvPane_SPEC.md) §4). [L]: the
  drop handler isn't decoded.
- **Healing spells.** All are out of reach early: Locke has 105 mana and
  Invoke 0 at level 1 (`spell.def:156-384`, `744-750`, `1065-1073`).

| Spell | Talismans | Mana | Invoke | Effect |
|---|---|---|---|---|
| Regeneration | Life, Earth | 107 | 5 | +120 HP a minute for two minutes |
| Heal | Sun, Life, Soul | 140 | 11 | about +203 H [L] |
| Troll Blood | Law, Life | 333 | 17 | `HealthPct 240` for two minutes |
| Cure Poison | Life, Sky | 25 | 2 | cures poison |
| Swift Strike | Ocean, Moon | 61 | 5 | about +13 fatigue a second for 27 s |

- **Merging.** Potions join a stack of the same type (`MergeInto`
  `0x0050ea80`). A new item with no slot goes into a matching Pouch
  ([INVENTORY.md](../gameflow/forensics/INVENTORY.md) §0).

### 6.3 No healers, no rest [C]

- **No NPC heals.** No module script or `master.s` sets the player's health,
  except Jong's spar, which holds it, and the opening.
- Elahni greets Locke with "Come inside and be healed my child" (module
  `english.def`:828), but that leads only to her potion shop
  (`town.s:1462-1532`).
- **There is no sleep, inn or bed** [L: absent from the scripts and from the
  code paths read].
- **The full heals are the level-up (§1.4) and potions.**

### 6.4 Death [C]

- **The countdown.** TPlayer::Animate keeps a counter (`+0x39c`) at 192
  ticks while Locke lives. Once his health is below 1, it counts down. At 0,
  about 8 s later, it switches to the death screen `0x0066f680` and closes
  the play screen, which tears down the world (`0x00518ba4`, `0x0048ea40`).
- **No single-player respawn.** The respawn path `0x0047e3c0` is multiplayer
  only.
- The observation shows `screen: "death"`.
- The screen has three buttons, mouse only in retail [L]:

| Button | Screen rect | What it does |
|---|---|---|
| Restart | (22–192, 404–466) | `0x00533950` restarts the play screen in its stored start mode. After a loaded game this is probably a **new game** ([SCREEN_SYSTEM.md](../gameflow/forensics/SCREEN_SYSTEM.md) §5) [L] |
| Load | (232–404, 404–466) | `0x00533970` opens the load screen ([LoadGameDef_SPEC.md](../ui/forensics/LoadGameDef_SPEC.md) §3). Rows are at x 291–563, y 92 + 18N; the "Load Game" button is at (158, 406), 140×30 |
| Exit | (438–610, 404–466) | `0x00533990` goes to the title |

**To recover:**

1. Wait for `screen: "death"`.
2. Click Load, at about (318, 435).
3. Click the newest save's row.
4. Click Load Game, at about (228, 421).

There is no quick load, so the director must **save before taking a risk**:

- Quick Save is Ctrl+Backspace (`QuickSave=CTRL-BS`; `0x0047e850`). It
  writes a new "Quick Save N" each time.
- Saving is refused while control is off
  ([SAVE_GAME.md](../gameflow/forensics/SAVE_GAME.md) §7).
- A save made while a corpse lies nearby deletes the corpse on load
  (FINDINGS F1). In retail its drops are already on the floor, so they
  survive; in the port they are still in the corpse and are lost.

## 7. Shops

### 7.1 Who sells what [C]

All four merchants stand in the House Interior, level 1 (`town.s`):

| Merchant | Position (level 1) | Door from Misthaven (level 0) | Sells | Buys from Locke |
|---|---|---|---|---|
| Elahni (potions) | (10270, 9830, 11); stays put | `healerin` (1506, 26556) → (10524, 9818); out by `healerout` | the ten potions (`town.s:1494-1504`) | the ten potions, by name (`town.s:1510-1520`) |
| Hruthford (food, jewellery) | (9228, 9466, 33); walks HRUWAY01–03 | `itemin` (2051, 26279) → (9300, 9754) | Pouch, food, jewellery; more at MISTSTATE ≥ 4 and at 7 (`town.s:1552-1693`) | Pouches, and items for EqSlot 1, 5 and 6: neck, bracelet, ring (`town.s:1602-1605`) |
| Gina (weapons) | (9138, 10684, 36); walks GINAWAY1–3 | `weaponin` (2297, 25440) → (9487, 10866) | weapons with `minstrength` in [1, VALUE] (`town.s:1830-1831`) | every weapon Locke may sell |
| Cronus (armour) | (10860, 8775, 146); walks CRONUSWAY1–3 | `armorin` (2265, 26865) → (11373, 8651) | armour with `minstrength` in [1, VALUE], except EqSlot 1, 5 and 6 (`town.s:2039-2042`) | armour, except EqSlot 1, 5 and 6 (`town.s:2048-2051`) |

`VALUE = player.STAT "LEVEL" / 3 + 16` (`town.s:1808`, `2017`):

| Locke's level | 1–2 | 3–5 | 6–8 | 9–11 | 12–14 | … |
|---|---|---|---|---|---|---|
| VALUE | 16 | 17 | 18 | 19 | 20 | … |

So the weapon and armour stock grows every three levels.

**Stock rules** (`0x00530670`, `0x00530af0`):

- SaleType 0 is always stocked.
- SaleType 1 is stocked only once Locke has sold one, to any shop. The
  save's merchant table records it (`0x0048e630`).
- SaleType 2 is never stocked.
- Stock is endless.

**What each shop stocks:**

- **Gina** (weapon.def prices; damage in brackets):

| From level | Adds |
|---|---|
| 1 | Short Sword 195 (13), Club 210 (14), Bone Pickaxe 225 (15), Stone Axe 255 (17) |
| 3 | Studded Bat 570 (19), Spiked Bat 630 (21) |
| 6 | Maul 1500 (25), Morning Star 1800 (30), Sickle 1800 (30), Battle Axe 2340 (39), Flanged Mace 2400 (40), Pointed Maul 2520 (42) |
| 9 | Long Sword 4950 (55), Broadsword 5400 (60) |
| 12 | Bastard Sword, Claymore |
| 15 | Katana, Flamberge |

- **Cronus:**

| From level | Adds |
|---|---|
| 1 | cloth at 10 gp, leather at 250 or 500, studded at 500–1500 |
| 6 | chainmail, 1800–3600 |
| 9 | Barbarian and Bone |

- **Hruthford:**
  - always: Pouch 100, food, Bracelet of Fortune 750, Ivory Pendant 500;
  - more rings and amulets later in the story;
  - Band of Regeneration (3000) only after Locke has sold one.

### 7.2 Prices [C]

- **Buy** at the type's `Value`. **Sell** for `floor(0.3 · Value)`
  (`0x0052da90`, `0x0052efe4`).
- No haggling: Luck and level play no part.
- **Not enough gold:** the merchant says the no-gold line (for example
  Gina's II9GIN03), and nothing changes (`0x0052ff40`).
- **A buy needs a free pack slot**, even for an item that would stack. With
  a full pack nothing happens, silently.
- **Locke can't sell** anything equipped or in a belt box, SaleType 2
  items, or `Value −1` (quest) items. Items inside bags are offered.
- **Selling a stack deletes the whole stack for one unit's price**
  (`0x005330a0`; [BuySellScreen_SPEC.md](../ui/forensics/BuySellScreen_SPEC.md)
  §11.2). Never sell a potion stack.

### 7.3 Driving a shop [C]

1. Walk in through the shop's door, an exit
   ([EXITS.md](../gameflow/forensics/EXITS.md)). Leave combat mode first: in
   combat a left click attacks.
2. Start talking, either way:
   - Left-click the merchant
     ([MAP_INPUT.md](../gameflow/forensics/MAP_INPUT.md) §4). Whether
     `TCharacter::Use` `0x004d4a60` checks distance is unconfirmed.
   - Or face him and press **U** (`0x004d01e0`). U takes the nearest
     non-enemy character, exit or container within 60 of a point 60 units
     ahead. In the port, U does nothing (§4.2).
3. Control goes off for the greeting. Then the choices come up: **1 Buy
   Items, 2 Sell Items, 3 Exit Shop** (BSBUY2, BSSELL2, BSEXIT2). Press the
   digit ([DIALOG.md](../gameflow/forensics/DIALOG.md) §4.4).
4. The shop opens as the bottom drawer, 452×178 at (0, 302). The world keeps
   running.

   | To | Keys | Mouse |
   |---|---|---|
   | Select row 1, 2 or 3 | **1**, **2**, **3** | click (100, 366), (100, 410) or (100, 454) |
   | Buy or sell the selection | **A** | click Activate (0–106, 302–343) |
   | Scroll, by pages of 3 | ↑ / ↓ | Up (425–447, 352–373) / Down (425–447, 427–473) |
   | Exit | **E** | click (340–452, 302–343) |

   - After a buy the selection stays, so A again buys another.
   - After a sale the row goes and the selection clears.
   - Scrolling clears the selection.
5. Back at the three choices, **3** leaves, and control returns.
6. The bottom bar (belt, quick spells) stays closed. Press **B**
   (`BottomPane=B`) to bring it back.

**Reading the shop.** The screen shows "Gold N" at (125, 302), and for each
row the name, a price like "195gp", a stats line ("Damage : +13 Min
Strength: +16") and the description. Reading that from the screen needs OCR;
memory has all of it (§8, `shop`).

## 8. What the observation lacks

The observation ([PROTOCOL.md](PROTOCOL.md) §7; port `src/observation.cpp`;
retail host `tools/retail_runtime/slots/play/observe.py`) was built for
walking, talking and fighting. A grind also needs the numbers below.

The retail column uses read paths the host already uses:

- the player's modified stat copy: count at `+0x34c`, {id, value} pairs at
  `+0x350` (GetObjStat `0x0051ae30`);
- an object's stats at `+0xa8` / `+0xac`;
- `GetStat(name)` (vtable `+0xd4`, `0x00473600`);
- inventories: count at `+0x68`, items at `+0x78`, each item's slot at
  `+0x7c`.

**A bug first.** `player.skills.attack` is stat 40, the Attack skill, which
never changes in play (§2.1). `combat.py:181-183` passes it to
`knowledge.locke_attacks` as the `attackskill` gate, but retail gates on
**AttackLevel, stat 20** (§2.3). Today the two agree only because both are
0 until Jong's first lesson. After that the director would never offer the
taught moves.

| Proposed field | Meaning | Retail | Port |
|---|---|---|---|
| `player.attacklevel` | stat 20, the gate for Jong's moves | copy pair 20 | `GetObjStat(PLRVAL_FIRST + PLRVAL_ATTACKLEVEL)` |
| `player.stats` | `{strn, cons, agil, rflx, mind, luck}`, modified values (ids 34–39) | copy pairs 34–39 | `GetObjStat(34..39)` |
| `player.skills`, all of them | each skill as `[level, exp, next]`: attack, defense, invoke, hands, knife, sword, bludgeon, axes, bows, stealth, lockpick (ids 40–50, 51–61, 62–72) | copy pairs | `Skill`, `SkillExp`, `GetObjStat(SKN_FIRST + s)` |
| `player.levelup` | the two attribute markers and the cursor, e.g. `[0, 1, 1]` = Strn, Cons | TPlayer `+0x360`, `+0x364`, `+0x368` | `levelupstats` (`src/player.h:471`) |
| `player.combat` | `{offense, defense, armor, damage}`: the stat sheet's Atk%, Def%, Armour and Dmg | vtable `+0x2c4`, `+0x2c0`, `+0x2bc`, `+0x2d0` (`0x0051a520`, `0x0051a550`, `0x00519850`, `0x00520830`) | the same virtuals |
| `player.effect` | the active stat effect: its STATLINE and the ticks left (Fist Mastery, Might, …) | TPlayer `+0x358` count, `+0x35c` {line, expiry}; now = `+0x370` | `stateffects` (`src/player.h:465`) |
| `player.items` | **every** carried item, not only the 12 cells in view: name, class, slot, amount, and the item numbers below. The pack has up to 255 slots and bags | inventory `+0x68` / `+0x78`, slot `+0x7c`; bags recursively | `GetInventorySlot` over all slots |
| item numbers | weapon: Type, Damage, MinStrength, Value. Armour: EqSlot, Protection, MinStrength, MinConstitution, Value, STATLINE. Food and potions: Health, Mana, Poison | `GetStat(name)` on the item; the ARMOR record (`0x0048cb50`) for the STATLINE | `GetStat`; `TRules` WEAPON / ARMOR entries |
| `equip_slots` | the screen centre of each equipment slot on the equip pane, or null while the pane is hidden. Needed to unequip by drag | EquipPane `DAT_0065b7e0` layout ([EquipPane_SPEC.md](../ui/forensics/EquipPane_SPEC.md)) | `EquipPane` |
| `belt_boxes` | the screen centre of each belt box, to drag a potion onto | BarInvPane layout ([BarInvPane_SPEC.md](../ui/forensics/BarInvPane_SPEC.md) §4) | the bar inventory pane |
| `objects[].value` | a character's class `Value` (Exp and to-hit, §1.2) | `GetStat("Value")` | `GetStat("Value")` |
| `objects[].dead` | health below 1: a corpse that is fading out (§3.1) | the health stat | `Health() < 1` |
| `objects[].items` | for an item on the ground: its name, amount and item numbers | as items | as items |
| `objects[].open`, `locked`, `contents` | a container's state and what it holds (taking from a chest is one item per click) | container stats (`Openable`, `LOCK`), inventory `+0x68` / `+0x78` | `TContainer` |
| `objects[].generator` | a MonsterGen helper's slots (type, frequency, cap, elapsed) and whether it is active | its slots (in the layout Load `0x00516e00` reads), `+0xec`, `+0xf0` | `TMonsterGen` (`src/monstergen.h`) |
| `shop` | while the buy/sell drawer is up: buy or sell, the rows as `{name, type, price, amount}`, the first row in view, the selection, the customer's gold | TBuySellScreen `0x0065a3b8`: type `+0x17c` (bit 1 buy, bit 2 sell), first row `+0x180`, selection `+0x188`, row count `+0x194` (int16), rows `+0x198` (0x48-byte records: display name `+0x04`, type name `+0x08`, price `+0x38`, amount `+0x3c`), in use `+0x1b0` | `BuySellPane` (`src/buysell.*`) |

None of these reads changes a game. Each is a stat or list lookup, with no
draw and no random draw.

The director can already get some numbers without a protocol change:

- a monster's Value, HEALTH, WEAPONDAMAGE, DAMAGEMODS and attacks, from
  class.def and char.def by `objects[].type`, through `knowledge.py` (fix
  the sign of `monster_brief`, §2.4);
- shop rows and item numbers by type name, from `weapon.def`, `armor.def`
  and class.def;
- the damage of Locke's blows, from the `FULLCOMBATRES` text-bar line in
  `messages` (COMBAT_HIT §3.3), unless `Revenant.ini` sets
  `NoCombatResults`.

## 9. The grind loop

What the facts above make of a grind mode, step by step. Each step points
back to the facts it rests on.

**Budget first.** Health comes back at about one point a minute (§6.1). So
every hit Locke takes is paid for by one of:

- a potion, at 2–2.5 gold per HP;
- food;
- a level-up's full refill (§1.4);
- reloading a save.

Fatigue, by contrast, comes back in seconds. So a good grind fights monsters
it can beat **without being hit much**, wears all the armour it can afford,
and times its level-ups as heals.

1. **Gear up from chests before grinding** (§4.4, §5.5–5.6).
   - Loot the Keep chests (500 gold) and the near level-0 gold chests:
     (1658, 22072) and (5740, 24394).
   - Buy leather from Cronus for 1750: Armor 2 → 7.
   - Fetch the Animal Fur set from ChestS (3218, 15576) once Con is 14.
   - Wear the Ivory Pendant (+2 armour) when gold allows.
2. **Choose the ground** (§1.2, §2.4, §3).
   - Exp wants monsters with `Value ≥ Level`. A level costs 30 kills at
     `Value = Level`, and a kill below `Level − 3` is worth 1.
   - Survival wants Locke's to-hit near 50 and the monster's low. Every
     point of Value is 12 to-hit points both ways; every level of Locke's
     is 10.
   - Hunt Value from `Level − 1` to `Level + 1`, nearer the top as armour
     and potions allow:

   | Locke's level | Hunt | Where |
   |---|---|---|
   | 1 | Araknid and Under Druhg (V1); Mongrel Druhg and Kantha (V2) | R1, R2a, R2b, 2–3k from the Keep gate |
   | 3–4 | Thrush Druhg and Arakna (V3); Hopper, Artikulid, Styxx (V4) | |
   | 5 | Pale Ogrok, Recluse, Ramage (V5) | |
   | 8 | Vashar, Lurker, Ogrok (V8) | |

   - Barehanded, skip Thrush Druhg (`hand +20`). Prefer Earth Golem
     (`hand −25`) and Lurker (`−10`) when the level allows.
3. **Farm generators, not the map** (§3.1).
   - Placed monsters are finite and gone for good.
   - Generators restock one monster per slot every 70–91 s, but only while
     off screen, and only below their cap of 1–3 within 300 units.
   - A farm is two or three generator sites a few screens apart. Clear one
     and walk to the next; once the first is off screen, its clock runs.
     Then come back. R1's four Kantha generators and R2b's araknid
     generators are the first farm. Track each site's last visit.
4. **Fight** (§2.4–2.5; [PLAN.md](PLAN.md) §11).
   - Rotate A, S and D: the third press of one key in a row gets roll 100.
   - Strike while the monster is mid-attack or turned away (+25 to-hit).
   - Block during its wind-up: −50 to its to-hit, and a blocking Locke
     takes nothing from a glance.
   - Keep fatigue above the next blow's cost (12–24 barehanded). Step back
     to refill it (about 13 a second) rather than throw the −80% fatigued
     blow.
   - Disengage below a health floor that covers one more of the monster's
     best hits.
5. **Loot** (§4.1–4.2).
   - In retail, drops land on the floor on the first dead tick: gold at
     (+30, +30), everything else within about 48 units.
   - Pick everything up before moving on and before saving: hover for the
     hand cursor and click, or face the items and press G.
   - In the port, click the corpse once per item.
6. **Equip** (§5). After each pickup or purchase, compare in code:
   - weapons by Damage, using the style rule below;
   - armour by Protection per slot;
   - jewellery by its STATLINE;
   - and never past MinStrength or MinConstitution.
7. **Heal** (§6).
   - Below the floor: drink a potion from the belt (F5–F9; the potion
     directly in a box, not in a pouch).
   - Otherwise retreat beyond sight range (640) and eat or drink from the
     pack.
   - If a level-up is close, the next kill heals fully: take a safe kill
     rather than a potion. Spend mana on Invoke casts just before it
     (§2.1).
8. **Shop** (§7), when gold allows.
   - Sell spare weapons and armour for 30% of Value. Never sell a stack or
     a quest item.
   - Buy Lesser Healing (250) and Lesser Restoration (500), then armour from
     Cronus, then weapons from Gina.
   - Re-check the stock every three levels.
9. **Set the level-up stats** (§1.4). Before each level, set the two
   markers on the Stats page (key 2). Two clicks on one `train*` button put
   both markers on it.
   - Strength adds damage: +5% for every 3 points from 16.
   - Agility adds to-hit, at the same rate.
   - Constitution adds health and unlocks armour (Con 14, 16, 18).
   - Mind only if spells matter.
10. **Save.** Quick-save when control is on, health is high, no monster is
    in sight, and nothing lies unpicked on the floor. Keep the last few
    saves. Death costs everything since the last save (§6.4).

### 9.1 Hand-to-hand as a first-class style

Kills feed the skill in hand (§2.1), so a mixed grind needs a policy, not a
fallback.

**Fight barehanded from the start:**

- Every hand attack and the grapples are available at once.
- A fist (12) equals the Short Sword (13).
- The lunging kicks hit for 17–19.

**Keep a share of kills barehanded all game.**

- Hands gets experience only from barehanded kills.
- The arena (S6) needs fists, about level 8–9 and about 300 HP (§2.5).

**Switch to a weapon:**

- against monsters with a high `hand` resistance;
- once Locke has a weapon several times 12 (the Tulwar's 35 and up);
- for Jong's lessons, which force the Training Sword anyway.

To unequip, drag the weapon from the equip pane to the pack (§5.2).

**Buffs for fists:**

- Fist Mastery (Hands +4) or Shadow Fist (Hands +8);
- Might (Str +2);
- the Amulet of Thunderbolts or Amulet of Power (DamageMod +10 / +20);
- the Band of Shadows (Hands +15 [L]).

Only one spell buff is active at a time.

## 10. Where the port grinds differently

Jev plays both games, so a grind recorded in one won't replay in the other
until these are closed. All are [C] from reading the port's code unless
marked.

| Gap | Retail | Port |
|---|---|---|
| Monster generators | MonsterGen respawns (§3.1) | ported (`TMonsterGen`); a multiplayer server's player-distance test and spawn throttle aren't |
| Death loot | the inventory drops to the floor; the corpse fades out from 10 s and goes at 14.2 s | the same since FINDINGS F8, F9; the items' flight isn't drawn |
| U and G keys | Use and Get (§4.2) | the same since FINDINGS F10 |
| A pickup into a full pack | the item is lost (§4.3) | the same since FINDINGS F11 |
| Item numbers | `weapon.def` and `armor.def` BASICMODS copied over class.def (§5.1) | the same since FINDINGS F6 |
| Jong's lessons | `player.lastattack` completes the lesson | the same since FINDINGS F7 |
| Boss AI (Baez, the Solifuge, Jhaga, Yhagoro) | `AIPerMonster` cases 1–4 | the boss kind is set, but its behaviour isn't ported (`src/character.cpp:3065`) |
| Kill Exp, skill Exp, level-up, Invoke Exp | §1–2 | ported. The `invoke2` level-up animation isn't played (PLAYER_STATS §9); Invoke experience matches retail (spell-cast kata, 2071/2071) |

## 11. Open questions

1. **Generators.** Whether retail loads a 3×3 or 5×5 block of sectors.
   Check the respawn cadence in the emulator: stand off screen from an R1
   Kantha generator for about 75 s. Also whether a fading corpse counts
   toward a generator's cap. (How far off screen reaches: §3.1.)
2. **Floor drops.** Do they survive a save and reload? They should, being
   sector objects.
3. **Full pack.** Answered: the item is lost, gone from the map and not
   carried (FINDINGS F11).
4. **Chests.** The exact order of open, take and close. Does a take always
   say "Can't carry any more" while the pack pane is closed (the current
   container `DAT_0065d674`)?
5. **The belt.** What F5–F9 does with a Pouch in the box.
6. **Death.** What Restart does after a loaded game. Whether the death
   screen's buttons really have no keys.
7. **Grapples.** Which monster types have the impact animations that
   Locke's barehanded grapples need.
8. **sunsetflipper.** It is masked to swords but only valid against Baez,
   in a barehanded fight. Is that a data bug? A question for the author.
9. **Band of Shadows.** Does `Hands 15%` add a flat +15? Equip it in the
   emulator and read stat 43 in the copy.
10. **Spells.** Does a STATLINE inside a CONTROLDATA block reach the
    variant (Fist Mastery, Shadow Fist, Might)? The Heal amounts, and what
    Nourish creates.
11. **Talking.** Does `TCharacter::Use` `0x004d4a60` have a distance check?
    That decides whether a merchant can be clicked from across the room.
12. **Baez.** Can Locke reach Baez before the gatekeeper lets him in? Would
    killing him early break S6?
13. **Speed.** Monster walk and run speeds come from the animations: can
    Locke outrun what he disengages from?
14. **"Cure Posion".** Does the generators' misspelled Cure Poison ever
    drop?
15. **Corpse fade.** Answered: `+0x194` is the character's fade (0..100,
    what it draws with), 100 unless a fade is under way; the corpse's fade
    sets it to 100 first. The corpse is gone 340 ticks after death in the
    emulator (FINDINGS F9).
16. **Notes for STORY.md** (not edited here):
    - S6: GIVEWEAPONS takes every weapon, ammo and ranged weapon, equipped
      or in bags, and leaves the armour (`0x00477780`).
    - S7: in retail the solifuge key drops to the floor (§4.1).

## 12. Summary

**Key facts:**

- **Experience** comes only from kills, never from quests. The amount is
  `(100L + 200)/15 · d` for a monster with `Value = L + d` (d > 0), and
  `/30`, `/60`, `/90`, `/120` for d = 0 … −3; below that, 1. A level costs
  30 kills at `Value = Level`.
- **A level-up** gives +25 max health, +25 max mana and +3 max fatigue, +10
  Offense and +10 Defense, and +1 to each of two pre-chosen attributes (one
  from level 16). It refills health, mana and fatigue. The `train*` buttons
  choose the attributes; the default is Strength and Constitution.
- **Skills.** The weapon skill in hand gets each kill's Exp again: Hands
  when barehanded, and Hands ≈ Level − 1 for a fists-only Locke. A skill
  level is worth +1 Offense. Invoke rises with each successful cast.
  Attack and Defense never rise. Weapon moves are gated on AttackLevel,
  which only Jong's lessons raise, and every lesson teaches a weapon move.
- **Fists.** Damage 12 (a Short Sword is 13), type `hand`, so monster
  `hand` resistances apply. Weapons deal `misc`, which no monster resists.
  All nine hand attacks and the grapples are available from the start, but
  hand damage never scales with gear. Baez in the arena wants about level
  8–9.
- **Monsters.** Each type's Value sets its Exp and its to-hit: 12 points per
  Value against Locke's 10 per level. Level 0 has 138 placed hostiles and
  109 generators, 2–5k from the Keep gate.
- **Respawn.** Placed monsters are finite. MonsterGen respawns one per slot
  every 70–91 s while off screen, under its cap.
- **Loot.** A dead monster drops everything to the floor at once; the corpse
  fades out about 14 s later. Pick up with a click on the hand cursor, or
  with G. Level 0's chests hold four times the gold its monsters carry.
- **Healing.** Health regenerates about 1 HP a minute, so the heals are
  potions (Elahni, 250 gold for +100) and level-ups. There are no healers
  and no rest. Death sends Locke to the death screen; recovery is loading a
  save.
- **Shops.** Buy at `Value`, sell at 30%; selling a stack sells only one
  unit's worth. The stock is gated on `Level/3 + 16` against MinStrength.
  Real prices and damage come from `weapon.def` and `armor.def`.

**Gaps:**

- **In the observation:**
  - `skills.attack` is the wrong gate; it should be AttackLevel, stat 20;
  - all skills with their experience; the attributes; the level-up markers;
  - Offense, Defense, Armour and Damage; the active buff;
  - every pack item and each item's numbers;
  - corpses; ground items; container state and contents; generators;
  - the shop rows;
  - screen positions for the equip-pane slots and the belt boxes.
- **In the director:** `monster_brief`'s inverted DAMAGEMODS sign.
- **In the port:** no death drop, no U or G keys, no boss AI.
- **Unconfirmed in retail:** the sector window; floor
  drops across a save; a full-pack pickup; the order of chest steps; belt
  pouches; Restart after a load; which monsters take the grapples; the
  Band of Shadows parse.
