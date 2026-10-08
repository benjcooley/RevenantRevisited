# Forensics: player stats (derived values, modified copy, RefreshStats, level-up)

**Topic:** how the shipped game computes the player's maximum health,
fatigue and mana, armor, and the equipment- and spell-modified copy of the
player's stats; when that copy is refreshed; experience and level-up.
**Status:** forensics complete (2026-10-05); port in `src/player.*`,
`src/rules.*`, `src/object.*`.
**Evidence:** Ghidra decompiles and disassembly of the retail
`Revenant.exe` (addresses below), the TPlayer vtable `0x005b4f30` and the
TCharacter vtable `0x005a7848`, the stat registrations
(`recon/scripts/object_stats.py`), and the shipped data (`rules.def`,
`imagery.rvi`: `weapon.def`, `armor.def`, `class.def`; `resources.rvr`:
`english.def`, `spell.def`). The 1998 source had none of this: its
`MaxHealth` was `healthperlevel × Level × (100 + classmod + Cons%) / 100`.

Stat ids are retail's ([`src/charstats.h`](../../../src/charstats.h)):
Health 3, Fatigue 4, Mana 5, DmgResMisc 6, Level 17, Exp 18, NextExp 19,
AttackLevel 20, MaxHealthFlat 24, MaxManaFlat 25, MaxFatigueFlat 26,
MaxHealthPct 27, MaxManaPct 28, MaxFatiguePct 29, ACBonus 30,
attributes 34–39 (Strn, Cons, Agil, Rflx, Mind, Luck), skills 40–50,
skill experience 51–61, next skill experience 62–72, skill caps 73–83.

## 1. The modified copy (TPlayer `+0x34c`)

TPlayer keeps a second array of its object stats: `+0x34c` count (short),
`+0x34e` capacity (short), `+0x350` pointer to `{stat id, value}` pairs.
`ClearPlayer` (`0x00518750`) builds one pair per object stat of the
PLAYER class, in order (the filter `0x0051ff30` returns a stat definition
for every index), so pair *i* is stat *i*.

| Slot | TCharacter | TPlayer | What the TPlayer one does |
|---|---|---|---|
| `+0xdc` GetObjStat | `0x004d7520` (the stats, `+0xa8`/`+0xac`) | `0x0051ae30` | the copy's value; 0 past its end |
| `+0xe8` SetObjStat | `0x004d74b0` | `0x0051adb0` | writes the stat **and** the copy's entry; marks the stat pane dirty when this is the main player (`DAT_0065b190`) |

Everything that reads a player stat through the vtable reads the copy:
the OBJSTAT accessors (`Level()` is `0x005201e0`, `Health()` `0x004d6f00`
and so on, all `+0xdc` calls), `GetStat(name)` (`0x00473600`: object
stats through `+0xdc`, class stats directly), and the stat sheet's
fallback field (`0x00472f80`: a PLAYER's object stat through `+0xdc`,
anyone else's directly). `SetStat(name)` (`0x004736f0`) goes through
`+0xe8`.

What reads the stats themselves (`+0xac`): the save
(`TObjectInstance::Save` `0x00472980` writes the raw array; the copy is
never saved), `RefreshStats` when it rebuilds the copy, and the two places
that add an attribute point (`SetPlayerLevel` and the level-up, §6).

## 2. RefreshStats `0x0051c660`

1. Each copy entry gets its stat (0 when the id is past the stats array).
2. For each of the 11 equipment slots holding an item: the item's
   `WEAPON.DEF` / `ARMOR.DEF` record (lookup `0x0048cb50` by class and
   object type; records are bound to types by name, `0x0048c930`). A
   weapon's `STATLINE` (record `+0xcc`) or armor's (`+0xc8`) is applied
   to the copy (§3). Other classes have no line.
3. For each active stat effect (`+0x358` count, `+0x35c` `{line, expiry}`
   pairs) its line is applied with the effect's index (§4). An expired
   effect is removed and RefreshStats runs again from inside; the outer
   pass then carries on from the next index.
4. Health, fatigue, mana above their maximums are set to the maximums
   (`+0x1d8` / `+0x1c0` / `+0x1c4`, and the fatigue and mana pairs).
5. In the copy, attributes (34–39) and skills (40–50) above 30 are set to
   30. The stats themselves are untouched.
6. Stat pane dirty (`DAT_0065b190`) when this is the main player and the
   pane's subject; then `RefreshEquip` (`0x00519230`).

Callers (raw `E8` scan of `.text`):

| Call site | Function | When |
|---|---|---|
| `0x00518960` | `ClearPlayer` `0x00518750` | construction, `TPlayerManager::Clear` |
| `0x00518b22` | `Animate` `0x00518aa0` | each frame, once the play clock passes the next effect expiry (`+0x354`) |
| `0x00519e80` | `Equip` `0x005199b0` | after any change of slot (not when the item is already there) |
| `0x0051a0f3` | `Unequip` `0x00519ff0` | `unequip` command |
| `0x0051ab95` | level-up `0x0051a630` | §6 |
| `0x0051bd15` | `Load` `0x0051b960` | end |
| `0x0051bd7b`, `0x0051bd89` | `LoadInventory` `0x0051bd20` | before and after `RefreshEquip` |
| `0x0051c540` | `AddStatEffect` `0x0051c2c0` | §4 |
| `0x0051d2c7` | `ApplyStatLine` `0x0051cc40` | an effect found expired |
| `0x0051d3cd` | `0x0051d390` | the expiry check of `Animate`, as a function; no direct call found |
| `0x0051d994` | `SetPlayerLevel` `0x0051d840` | end |
| `0x00581648`, `0x00581658` | network player update | multiplayer only |

`RefreshEquip` (`0x00519230`) walks the inventory; an item at
`0x100 + slot` (slot < 11) goes into `equipment[slot]` and then through
`Equip(CanEquip(item, slot) ? item : nullptr, slot)`, so an item that may
be equipped returns at once and one that may not is unequipped.

`LoadInventory` (`0x0051bd20`): base `LoadInventory`, keep health, mana
and fatigue (read from the stats array), `RefreshStats`, `RefreshEquip`,
`RefreshStats`, set health, mana, fatigue back.

## 3. STATLINE

Data: `STATLINE` in a `WEAPON` / `ARMOR` block (`weapon.def`,
`armor.def`, read by `TRules::Load` `0x0048b990` with `rules.def` and
`char.def`; record loaders `0x0048ac30` / `0x0048b1a0`), and in
`spell.def` spell variants. The loader stores the line's tokens joined by
single spaces after a leading space (`" DmgResPoison 6"`). Examples:
`DmgResmagical 4 RFLX -1`, `MaxManaPct 15 MaxHealthPct 15`,
`Attack %2`, `STRN 2 TIME 2880` (spells).

`ApplyStatLine` (`0x0051cc40`, a TToken over the line, effect index or
-1) first sets `+0x354` (next expiry) to -1, then reads tokens to the end
of the line:

- **A stat name** (case-insensitive match on the PLAYER object stat
  *names*; `Strn` matches `STRN`, `LEV` and `FAT` match nothing). The next
  token decides:
  - a number *n*: copy[stat] += *n*;
  - `+`: copy[stat] += the number of the very next token, blanks not
    skipped -- in a stored line that is the blank, so nothing is added;
  - `%` then a number: copy[stat] = (copy[stat] × (100 + *n*)) / 100
    (anything else after `%` applies nothing and is read again as the
    next word);
  - a token starting `(`: copy[stat] = `atoi` of the text after `(` --
    the stored `( 5 )` gives 0;
  - anything else: nothing (main player: a "STATINVMOD" message), and the
    token is read again as the next word.
  Each stat appends `"<STATCFG<NAME>> <value text>"` to a message that
  starts with the `STATMODS` line ("Stat Mods:").
- **`TIME`** then a number *t*: the effect lasts *t* × 4.1666670 (the
  float `0x40855556` at `0x005b535c`, a hair over 100/24; truncated), *t*
  in 24 Hz frames: 2880 frames = 12000.
- **`INITIALIZE`**: this is the effect's first evaluation. The word is cut
  out of the stored line (the space before it stays).
- **`EFFECT`, `TAG`**: retail's token test does not consume, so either
  word hangs the loop (`EFFECT` does nothing, `TAG` keeps a pointer to the
  token text). No shipped line uses them.
- **Anything else**: the rest of the line is skipped and the function
  returns without the end-of-line handling.

At the end of the line, for an effect: without `TIME` the expiry is -1
(permanent); on the `INITIALIZE` pass the expiry is now + duration; later,
an effect whose expiry is before now is removed and RefreshStats runs
(§2.3). `+0x354` then becomes this effect's expiry when that is earlier
than `+0x354` or `+0x354` is -1 — and since every line resets `+0x354`
first, it ends up holding the last effect's expiry. On the `INITIALIZE`
pass the main player gets the message on the text bar.

"Now" is the player's play clock, `vtable +0x314` = `+0x370` (§5).

## 4. Stat effects

`AddStatEffect` (`0x0051c2c0`, the `statmod` script command
`0x00428200` → `0x0051c550`, and the spell effect constructor
`0x0053f090` when the target is the player and the spell has a STATLINE):
the line gets `" INITIALIZE"` appended unless it already has it; every
active effect whose stored line differs from the new line is removed
(a stored line has lost its `INITIALIZE`, so in practice all of them); the
new one is added; `RefreshStats`. One stat effect at a time, then.

`RemoveStatEffect` is `0x0051d4a0`. The list isn't saved.

## 5. The play clock

`+0x370` (game time, 1/100 s) and `+0x374` (frames) are set from the
game clock by `ClearPlayer` and read from a save by `Load` (`+0x374` =
`+0x370` × 24 / 100). `Animate` (`0x00518aa0`) advances them while the
player state (`+0x36c`) has bit 0 and not bit 1: `+0x374` += 1,
`+0x370` = `+0x374` × 100 / 24. It then refreshes the stats when
`+0x354` isn't -1 and is before `+0x370`; if `+0x354` is more than 50000
away from now it is first pulled to now − 1.

## 6. Derived values

The STATLEVEL tables (`rules.def`): six tables of 31 entries (values
0–30), unset entries `-2000000` (`0x0049c9b0`), parsed by `0x0049c9f0`
(`STATLEVEL "Strength" | "Constitution" | "Agility" | "Reflexes" | "Mind"
| "Luck"`, exact case, tables 0–5; `ENTRY level value` for level < 31).
`StatLevel(table, value)` (`0x0048cc20` → `0x0049cbf0`) is the entry for
a value below 31, else 0. Rules `0x0065d7a8`: `healthperlevel` `+0x68`,
`fatigueperlevel` `+0x6c`, `manaperlevel` `+0x70` (`HEALTHDATA`,
`FATIGUEDATA`, `MANADATA`). Class data `+0x64/+0x68/+0x6c`: `HEALTHMOD`,
`FATIGUEMOD`, `MANAMOD`. All stats below are read from the copy.

| | Address | Formula |
|---|---|---|
| MaxHealth | `0x00520630` | (MaxHealthFlat + 75 + healthperlevel × Level) × (100 + MaxHealthPct + StatLevel(Constitution, Cons) + HEALTHMOD) / 100 |
| MaxFatigue | `0x005206d0` | (MaxFatigueFlat + 75 + fatigueperlevel × Level) × (200 + MaxFatiguePct + FATIGUEMOD + 2 × StatLevel(Constitution, Cons)) / 200 |
| MaxMana | `0x00520770` | (MaxManaFlat + 75 + manaperlevel × Level) × (100 + MaxManaPct + StatLevel(Mind, Mind) + MANAMOD) / 100 |
| ArmorValue | `0x00519850` | ACBonus + Σ Protection of the ARMOR-class items in every equipment slot but ammo (8) |

Division truncates toward zero. TCharacter's are the character data's
(`+0x1e8`, `+0x1e4`, `+0x1e0`, armor `+0x1c4`).

The stat sheet's `armor` (`TCharacter::GetFieldText` `0x004d5260`) adds,
for the main player only, the `+0x134` of each object in the character's
list at `+0x170` (`0x005407d0`; spell effects attached to the character)
and `DmgResMisc` (`+0x260`).

Not ported here (for the record): AttackModifier `0x0051a480` =
attackmod + StatLevel(Agility, Agil) + Σ `+0x138` over `+0x170` +
weapon skill; DefenseModifier `0x0051a4e0` = defensemod +
StatLevel(Reflexes, Rflx) + Σ `+0x134` over `+0x170`.

### Locke (class Revenant: HEALTHMOD 0, FATIGUEMOD 0, MANAMOD 5; rules.def HEALTHDATA 25, FATIGUEDATA 3, MANADATA 25)

| | Level, Cons, Mind | Max H / F / M | Current |
|---|---|---|---|
| `newgame.sav` | 1, 12 (0%), 14 (0%) | 100 / 78 / 105 | 25 / 78 / 0 |
| after the opening's `playerlevel 1` | 1, 12, 14 | 100 / 78 / 105 | 100 / 78 / 105 |
| retail `New Game1` | 1, 12, 14 | 100 / 78 / 105 | 40 / 78 / 7 |

The saved fatigue, 78 in both files, is retail's MaxFatigue for level 1.
The 1998 formulas give 25 / 3 / 26.

## 7. Experience and level-up

Player experience table (Rules `+0xf0`, `0x0048b690`): 0, 300, then each
next entry adds (5i + 10) × 20. `ExpForLevel(n)` (`0x0048cc40`) is 0 for
n = 0, else entry min(n − 1, 29). NextExp at level 1 is 300.

The attack resolver (`0x004c62b0`) ends, when the attacker is a player,
with three TPlayer virtuals on the target:

- `+0x414` kill experience (`0x0051a630`): only for a dead target
  (Health < 1). Gain from the target's `Value` (a PLAYER target: its
  `Level`, then × 1.5) against the player's `Level`: base =
  (5 × Level + 10) × 20; d = value − Level; d > 0: (base / 15) × d;
  d < −3: 1; else base / (30 − 30d). Exp += gain. If Level < 30 and Exp
  ≥ ExpForLevel(Level + 1): Level + 1, **NextExp = ExpForLevel(Level + 2)**
  (old Level), messages (new level < 25 and divisible by 3: `LUPJONG`;
  ≤ 15: `STATCFG<stat>` for both level-up attributes, then `LUPBASE`;
  > 15: `LUPBASE` alone), the chosen attribute(s) +1 on the stats
  themselves (`+0x360`, and `+0x364` up to level 15), a chosen attribute
  at 29 or more in the copy moves on to the next one below 29, the
  `invoke2` animation, a `PowerUp` effect, the `POWERUP` sound,
  RefreshStats, health / mana / fatigue to their maximums.
- `+0x41c` (`0x0051abe0`) weapon-skill experience, skill
  `WeaponType() + 3`, same gain formula, dead targets only.
- `+0x420` (`0x0051ad80`) stealth experience (skill 9) when the target
  has not seen the player (`0x004c58f0`).

`SetPlayerLevel` (`playerlevel`, `0x0051d840`) does not touch Exp or
NextExp. `AddSkillExp` is `0x0051ac90` (see
[COMMAND_SYSTEM.md](../../gameflow/forensics/COMMAND_SYSTEM.md) §6.1).

## 8. Corrections to earlier ports

- `ClearPlayer` writes 30 to stats `0x49 + s` (73–83), the **skill
  caps**, not the skills; and spreads the 84-point attribute budget over
  `(84 − Σ|STATREQS|) / 6` per free attribute (divisor 6, not the number
  of free attributes).
- `STATREQS` is stored in file order (`0x004891c0`) and column *i* is
  attribute 34 + *i*. `rules.def`'s comment calls columns 5 and 6
  "luck, mind"; the code makes them Mind and Luck.
- `SetSkill(s, v)` is stat `SK_FIRST + s`.
- TPlayer's `SetHealth` / `SetFatigue` / `SetMana` (`0x0051a420`,
  `0x0051a440`, `0x0051a460`) only set the stat, as TCharacter's do (the
  port drops its overrides). The 1998 versions also pushed the value into
  the old status bars, dividing by the maximum.

## 9. Port notes and deviations

Where it lives: `TPlayer` (`src/player.*`: the copy `modstats`, the
`GetObjStat` / `SetObjStat` overrides, `RefreshStats`, `ApplyStatLine`,
`AddStatEffect`, the maxima, `ArmorValue`, the play clock in `Pulse`, the
experience awards), `TObjectInstance::GetObjStat` / `SetObjStat` made
virtual as retail's are (`+0xdc` / `+0xe8`), `TRules` (`src/rules.*`:
STATLEVEL, `WEAPON` / `ARMOR` entries, `ExpForLevel`), and the arithmetic
in `src/playerstats.*` (tested by `tools/test_playerstats.cpp`). The kill
experience is called from `TCharacter::ResolveHit`.

- The copy is a `std::vector<int32_t>` indexed by stat id (retail's pairs
  carry the same id).
- EFFECT and TAG are consumed and ignored instead of hanging.
- WEAPON / ARMOR entries are found by type name at lookup instead of being
  bound to object types at load; a name given twice keeps the later entry
  (retail stops with an error).
- STATLEVEL lookups of a negative value answer 0 (retail reads outside the
  table).
- The level-up's `invoke2` animation (`0x004d5900`) isn't played; its
  messages, `PowerUp` effect, `POWERUP` sound and stat changes are.
- `+0x170` (character spell-effect list) doesn't exist in the port, so
  the stat sheet's `armor` lacks that term, and AttackModifier /
  DefenseModifier stay the 1998 ones.
- Network forwarding in AddSkillExp and the level-up isn't ported.
- The `statmod` command (`0x00428200`, `0x0051c550` =
  `PlayerStats::ReadStatLine` then `AddStatEffect`) is ported
  (`--exec "statmod STRN 2 TIME 48"` raises Str by 2 for two seconds); the
  spell effect constructor (`0x0053f090`) is not, so spells add no stat
  effects yet.

## 10. Verification (2026-10-05)

- `build/test_playerstats`: the maxima, STATLEVEL parsing, kill
  experience, TIME, and the STATLINE grammar against hand-worked values.
- `--quickstart` (newgame.sav): `[player] loaded: L1 Exp=0/300 STR=16
  CON=12 ... H=25/100 F=78/78 M=0/105`, then the opening's
  `[player] playerlevel: L1 ... STR=18 ... LCK=14 H=100/100 F=78/78
  M=105/105`. The stat sheet and status bar show the same (filmstrip).
- `--quickstart="New Game1"` (retail save): `H=40/100 F=78/78 M=7/105`,
  sword, shirt and pants still equipped.
- Save round trip (SAVE_INTEROP_TEST.md, run by hand under a scratch
  save path): `Port Resave` differs from `New Game1` only in the AI flag;
  sectors as documented; `Port New Game` as documented.
- With a temporary console hook (not committed): equipping Animal Fur
  Gauntlets and Chest Plate gives DmgResPoison 6 then 12 in the copy, 0
  in the save; Armor 1, 4, then 6 with ACBonus 2; `STRN 2 TIME 48` raises
  Str 18 → 20 and expires two seconds later; Exp 299 plus a kill → level
  2, Exp 302/700, Str 19 (+2 effect) Con 13, H 125/125 F 81/81
  M 131/131.

## 11. Questions for the author

Mirrored in [docs/gameflow/AUTHOR_QUESTIONS.md](../../gameflow/AUTHOR_QUESTIONS.md)
(36–39). The port keeps retail's behaviour until answered.

1. rules.def's comment names the STATREQS columns "... luck, mind", but
   the shipped code gives the fifth value to Mind and the sixth to Luck
   (§8). Which was intended?
2. ClearPlayer divides the free attribute budget by six, so each free
   attribute gets about 6–7 points and a character totals 63 rather than
   84 (§8). Intended?
3. Adding a stat effect removes every other active one, so only one buff
   is in force at a time (§4). Intended?
4. Some armor.def lines never worked as written: `FAT %15`, `LEV n`, and
   the trailing `%` in `Hands 15%`; and `Fatigue %4` scales *current*
   fatigue (§3). What were they meant to do?
