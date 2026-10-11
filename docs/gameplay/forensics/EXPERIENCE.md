# Forensics: experience and the level-up

**Topic:** what the shipped game gives the player for a kill, a weapon
skill and a stealthy kill, and what reaching a level does. Builds on
[PLAYER_STATS.md](PLAYER_STATS.md) §7, which first described it from the
decompiles; this file is the asm reading for kata C7 of
[../COMBAT_DOJO.md](../COMBAT_DOJO.md) (`melee-exp`).
**Status:** 2026-10-11.
**Evidence:** the unchanged retail image through `rdis.py`, every claim
read in the asm. Strings: `0x5e2854` / `0x5e2860` "EXP", `0x5e2858` /
`0x5e2864` / `0x5e286c` "Level", `0x5e2874` / `0x5e28e0` "Value",
`0x5e287c` "LUPJONG", `0x5e2884` "LUPBASE", `0x5e288c` "STATCFG%s",
`0x5e28c8` "invoke2", `0x5e28d0` "PowerUp", `0x5e28d8` "POWERUP".

## 1. Function table

| Retail | Identity | Convention |
|---|---|---|
| `0x51a5b0` | TPlayer::KillExp(value) | thiscall ret 4 |
| `0x51a630` | TPlayer::AwardKillExp(victim), vtable `+0x414` | thiscall ret 4 |
| `0x51abe0` | TPlayer::AwardSkillExp(skill, victim), `+0x41c` | thiscall ret 8 |
| `0x51ac90` | TPlayer::AddSkillExp(skill, exp), `+0x418` | thiscall ret 8 |
| `0x51ad80` | TPlayer::AwardStealthExp(victim), `+0x420` | thiscall ret 4 |
| `0x48cc40` / `0x48cc90` | Rules ExpForLevel / SkillExpForLevel: 0 for level 0, else table entry min(level − 1, 29) at Rules `+0xf0` / `+0x168` | thiscall ret 4 |
| `0x4c58f0` | TCharacter::HasSeenMe(who): the 45-second memory (perception M9b) | thiscall ret 4 |
| `0x4d5900` | TCharacter::SetCast(name, target, delay) | thiscall ret 0xc |
| `0x5201f0` / `0x520220` / `0x520250` | TPlayer SetLevel / SetExp / SetNextExp (stats 17 / 18 / 19) | thiscall ret 4 |

## 2. Behaviour

```
KillExp(value):                                   // 0x51a5b0
    GetStat("EXP")                                // read, unused
    L = GetStat("Level")
    base = (5L + 10) · 20; d = value − L
    d > 0:  (base / 15) · d
    d < −3: 1
    else:   base / (30 − 30d)                     // signed, truncated

AwardKillExp(victim):                             // 0x51a630
    (network game: forwarded, or dropped for a remote player)
    victim Health > 0: return                     // (no null test)
    exp = GetStat("EXP"); L = GetStat("Level"); next = ExpForLevel(L + 1)
    gain = victim a player ? (int)(KillExp(victim.GetStat("Level")) · 1.5f)
                           : KillExp(victim.GetStat("Value"))      // inlined both
    SetExp(exp + gain)
    L >= 30 or exp + gain < next: return
    SetLevel(L + 1); SetNextExp(ExpForLevel(L + 2))
    (the six attributes are read and summed; the sum is not used)
    L + 1 < 25 and (L + 1) % 3 == 0: text bar LUPJONG
    L + 1 > 15:
        main player and LUPBASE has a line: text bar LUPBASE
        pick 0 (+0x360) + 1 on the stats themselves
    else:
        main player and LUPBASE has a line:
            text bar STATCFG<pick 0's stat name>, STATCFG<pick 1's>, LUPBASE
        pick 0 + 1, pick 1 + 1
    each pick at 29 or more (the modified copy) moves on to the next
        attribute (wrapping) below 29, six tries; the stat sheet redraws
    SetCast("invoke2", nullptr, 90)               // "<prefix>inv2", else "invoke"
    the POWERUP sound (no position), then a PowerUp effect at the feet
        (NewObject with whatever FindObjType gave)
    RefreshStats; health, mana, fatigue to MaxHealth, MaxMana, MaxFatigue

AwardSkillExp(skill, victim):                     // 0x51abe0
    victim null or Health > 0: return
    AddSkillExp(skill, KillExp(victim.GetStat("Value")))   // inlined

AddSkillExp(skill, exp):                          // 0x51ac90
    (network game: forwarded, or dropped for a remote player)
    have = stat 51 + skill; L = stat 40 + skill
    L >= 30: stat 51 + skill = SkillExpForLevel(30); return
    stat 51 + skill = have + exp
    have + exp >= SkillExpForLevel(L + 1):
        stat 40 + skill = L + 1; stat 62 + skill = SkillExpForLevel(L + 2)

AwardStealthExp(victim):                          // 0x51ad80
    victim and !victim.HasSeenMe(this): AwardSkillExp(9, victim)
```

The weapon skill ResolveHit passes is `WeaponType() + 3` (COMBAT_HIT.md
§3.3).

## 3. The 1998 code and the port before C7

- The port (gameflow's PLAYER_STATS work) had the formulas and the
  level-up; missing were the "invoke2" animation (SetCast had not been
  ported then) and the order of the sound and the effect (retail plays
  the sound first).
- The 1998 code had the experience stats (`Exp`, the skills' `...Exp`)
  but nothing that awarded them: no kill, skill or stealth experience,
  no level-up.

## 4. Port status / kata

### 4.1 The port (`player.cpp`)

KillExp (`playerstats.h` PlayerStats::KillExp), AwardKillExp,
AwardSkillExp, AddSkillExp and AwardStealthExp as §2 reads; C7 added the
level-up's `SetCast("invoke2", nullptr, 90)` and put the sound before the
effect. Not ported: the network forwarding; the stat sheet's redraw flag
(`0x65b190`, the UI's). The port reads the level and experience as stats
(Level(), Exp()), retail by name through the type's GetStat; the same
values.

### 4.2 Kata C7 `melee-exp` (285 cases, all match)

`targets_melee.py` `exp_cases`, the originals with `seam_exp` false.
KillExp for levels 1–30 against values six either side; AwardKillExp for
a living / dead victim, every level band (the 3rd-level JONG message,
the ≤ 15 picks and messages, one pick past 15, the cap at 30) with the
experience one short, short by much and exactly there, values above,
level with and below; the picks at 29 and 30 moving on (all at 29: no
move), a pick chosen twice; a player victim (Level × 1.5, and not the
main player then: no STATCFG messages); the invoke2 / invoke / none
animations. AwardSkillExp for every skill, a living victim, the KillExp
bands; AddSkillExp at levels 0–31 across the threshold, nothing, a loss,
a huge gain; AwardStealthExp over the victim's memory (none, 0, 1079,
1080, 1081, 5000 frames ago; another remembered instead).

The retail fixture gives the player a class record with the attribute
names (static init, which registers them in retail, doesn't run in the
emulator) and the stats themselves (`+0xa8` / `+0xac`, which the
level-up raises); RefreshStats is recorded, not run.

Mutations (cases that went wrong of 285): KillExp's floor at −2 instead
of −3: 7; AddSkillExp needing more than the figure: 5; stealth experience
for a kill that was seen: 7; invoke2 held 80 ticks: 90; the PowerUp
effect before its sound: 91.

Coverage: KillExp 7/7, AwardKillExp 99/113, AwardSkillExp all but the
KillExp bands the first run lacked (added since), AddSkillExp 15/21,
AwardStealthExp 5/5; the rest are network paths, the sound being found
(the fixture has none), and a stat index past the record.
