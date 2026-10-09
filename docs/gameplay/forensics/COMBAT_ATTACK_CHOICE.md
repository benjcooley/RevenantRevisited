# Forensics: attack choice, fighting state and combat AI

**Topic:** how the shipped game decides *which* attack a character makes
and *when*: the player's attack buttons (chains, the same-button rule),
the attack search (button, percentage, specific, interactive), DoAttack,
entering and leaving the fighting state, the monster AI's combat
decisions and their cadence, target finding (enemy, hearing, sight,
memory), and the char.def attack record.
**Status:** 2026-10-08. Read from the disassembly only; no kata run yet
(C3 / M9 in [../COMBAT_DOJO.md](../COMBAT_DOJO.md)). Port not changed.
**Evidence:** `recon/retail_asm/baseline` (unchanged executable) read
with `rdis.py`; Ghidra decompiles in `recon/discovered/` and
`recon/classes/cls_0x5a7b98.cpp` used as a guide only (several labels
wrong, §8); retail `char.def` / `rules.def` from `imagery.rvi` /
`resources.rvr`.
**Not repeated here:** IsValidAttack's damage / to-hit / roll / tier
block, the `charflags & 0x80` gate and the `Block(−2)` side effect are in
[COMBAT_HIT.md](COMBAT_HIT.md) §3.1. SetFighting `0x4d4790` is already
ported as retail (summary in §3.9 only). Movement (Go, ResolveCombat) is
[COMBAT_MOVEMENT.md](COMBAT_MOVEMENT.md).

Conventions: `this+0xNN` is a TCharacter field (§2.3), `ad` an attack
record (§2.1), `GameFrame` = `0x47e920(0x65caf0)`. Action numbers are the
port's `ACTION` enum (3 COMBAT, 4 COMBATMOVE, 7 ATTACK, 0xb INVOKE,
0xd STUN, 0xe KNOCKDOWN, 0x19 BOW, 0x1a BOWMOVE, 2 MOVE, 1 ANIMATE).
"Interactive gate" below = `if !(charflags & 0x80000) && ((doing->attack
&& doing->attack->flags & CA_INTERACTIVE) || (doing->impact &&
doing->impact->flags & 0x80)) refuse` — the same five-line block opens
ButtonAttack, ButtonAction, RandomAttack, SpecificAttack, EndFighting,
SetFighting and Go.

## 0. Why the port attacks twice a tick, and what retail does instead

Retail never attacks from the AI more than once per tick, and normally
only once per `nextattack` interval:

1. **Attacks are only tried while standing in the combat root.** The AI's
   attack branch runs only when `doing->action == 3` (the combat stance
   itself, not a COMBATMOVE step, not an ATTACK) and the monster isn't
   retreating (`0x4c8d00`–`0x4c8d13`; walk-fighters and the `+0x128`
   request bits are the exceptions, §3.10). While an ATTACK block is
   doing, the AI goes to its move/idle branch, which never attacks.
2. **`nextattack` (`+0x120`) gates everything a monster does.**
   IsValidAttack refuses every non-magic attack while `+0x120 != 0` for a
   non-player (`0x4d13fe`–`0x4d140d`). The AI decrements it once per tick
   (`0x4c8d2b`), tries `RandomAttack` only when it reaches 0 (or the magic
   timer `+0x124` reaches 0, or request bit `+0x128 & 1`), then
   decrements it again (`0x4c8ea1`, unless the last attack was PLAYANIM)
   so it goes negative, and re-arms it the same tick:
   `+0x120 = random(ATTACKFREQ.min·24/100, ATTACKFREQ.max·24/100)`
   (`0x4c8ef1`). A successful attack, a failed attempt and an approach
   step all re-arm it.
3. **`still in prior attack`:** IsValidAttack refuses while `doing` is an
   ATTACK whose frame (`+0x5c`) hasn't reached `ad->nextwait` (`+0xc4`).
4. **The attack being done is skipped:** if `lastattack` (`+0x160`) is
   set and `doing->Is(lastattack->name)`, the AI does nothing this tick
   but count down (`0x4c8d89`).
5. **BeginFighting sets `+0x120 = −1`**, so the first attack after
   engaging waits a fresh random interval (`0x4d3fa2`).

The port's AI (`TCharacter::AI`) tries an attack when `nextattack == 0
|| waitticks == 0` (`waitticks` is its name for the magic timer). In
retail the magic timer only leaves 0 through IVA's magic branch (it
becomes −1 and the AI re-arms it from MAGICFREQ); the port does the same
decrement, but a monster without MAGICATTACK lines never takes that
branch, so once `waitticks` counts down to 0 it **stays 0 and the attack
branch runs every tick**. Retail reaches the same state (`+0x124 == 0`
every tick for a monster with no magic attack), but there IVA's
`nextattack` gate (item 2) refuses every non-magic attack until
`+0x120` is 0; the port's IsValidAttack has no such gate (it tests the
script's wait instead), so every tick's RandomAttack can succeed. Add the
chain/autocombo block in Pulse, which calls SpecificAttack in the same
tick (§3.11), and the port gets two DoAttacks per tick. The fixes, in
retail's terms: the `+0x120 != 0 && !player` gate in IsValidAttack, the
magic timer re-armed from MAGICFREQ (`chardata+0x1d8/+0x1dc`), the second
decrement and same-tick re-arm of `nextattack`, the `doing->action == 3`
entry condition, and the "still doing lastattack" skip.

## 1. Function table

Calling conventions from the `ret N`. Confidence: **A** = body read in
the asm, identity by behaviour + callers/vtable; **B** = asm read, name
inferred.

| Address | Identity | Convention | Conf. | Evidence |
|---|---|---|---|---|
| `0x4d2480` | TCharacter::ButtonAttack(button) | thiscall, ret 4 | A | called by Command cases 0x21–0x2f (`0x47d8a2`…), PLAYER_INPUT §3 |
| `0x4d27f0` | TCharacter::ButtonAction(button) | thiscall, ret 4 | A | Command (`0x47d865`, `0x47d8f6`…); passes isaction=1 |
| `0x4d1120` | TCharacter::IsValidAttack(i, &imp, &dmg, &tohit, &roll, tdist, button, pcnt, unused, mask, flags, targ) | thiscall, ret 0x30 | A | 4 callers, all attack searches |
| `0x4d1dd0` | TCharacter::FindButtonAttack(button, unused, &i, &imp, &dmg, &tohit, &roll, isaction, targ) | thiscall, ret 0x24 | A | 3-pass search; callers ButtonAttack/ButtonAction. Ghidra calls it `TPlayer::meth_0x4d1dd0` |
| `0x4d1eb0` | TCharacter::FindPcntAttack(pcnt, unused, &i, &imp, &dmg, &tohit, &roll) | thiscall, ret 0x1c | A | random index search |
| `0x4d1ff0` | TCharacter::FindInteractiveAttack(pcnt, unused, &i, …) | thiscall, ret 0x1c | A | considers only `CA_INTERACTIVE` attacks; **mislabelled FindButtonAttack** in recon |
| `0x4d2120` | TCharacter::DoAttack(i, imp, dmg, tohit, roll, targ) | thiscall, ret 0x18 | A | builds ACTION 7 block; 6 callers |
| `0x4d2900` | TCharacter::RandomAttack(pcnt) | thiscall, ret 4 | A | only caller AI `0x4c8dc6` |
| `0x4d2a60` | TCharacter::SpecificAttack(i) | thiscall, ret 4 | A | Pulse chain `0x4c2756`, debug cmd `0x427c93` ("Invalid Attack %d" `0x5cca20`) |
| `0x4d3b90` | TCharacter::BeginFighting(targ, action) | thiscall, ret 8 | A | "Invalid fighting action" `0x5e042c`; Command cases 1/2 |
| `0x4d3fd0` | TCharacter::EndFighting() | thiscall, ret 0 | A | Command cases 1/2, Wander |
| `0x4d4790` | TCharacter::SetFighting(targ) | thiscall, ret 4 | A | ported (§3.9) |
| `0x4d5fc0` | TCharacter::StartRetreat() | thiscall, ret 0 | A | **recon calls it BeginCombat — wrong**; sets the retreat state (§3.10.6); callers: debug cmd thunk `0x4282e0`, area object `0x50f5a0` (every class-0xc char in a box) |
| `0x4c8b60` | TCharacter::AI() | thiscall, ret 0 | A | vtable slot `0x230` (`0x5a7a78`); called from Pulse `0x4c29be`/`0x4c29d1` |
| `0x4c9790` | TCharacter::Wander() (slot `0x234`) | thiscall, ret 0 | B | Ghidra "WanderToWaypoint"/"TPlayer_Pulse"; patrol + run/walk toggles; only for `charflags & 0x100000` |
| `0x4c9b70` | TCharacter::AI_PerMonster() | thiscall | B | called from AI when Aggressive; not read |
| `0x4c89c0` | TCharacter::IsEnemy(chr) | thiscall, ret 4 | A | reads `chardata+0x70` (ENEMIES) and `+0x20` (GROUPS) |
| `0x4cd690` | TCharacter::FindCharacters(chars, max, range, angle, cone, flags) | thiscall, ret 0x18 | A | 13 callers |
| `0x4cd920` / `0x4cd960` / `0x4cda50` | FindCharacter(range,angle,cone,flags) / FindCharacterAhead(angle,cone) (range 0x200, flags 0) / FindClosestEnemy(angle,cone) (range −1, flags 7) | thiscall, ret 0x10 / 8 / 8 | A | one-line wrappers |
| `0x4cd990` | TCharacter::IsValidTarget(chr) | thiscall, ret 4 | A | COMBAT_MOVEMENT §3 |
| `0x4cd540` | TCharacter::CanSeeCharacter(chr, angle) | thiscall, ret 8 | A | |
| `0x4cda80` | TCharacter::Hearing(dist) | thiscall, ret 4 | A | vtable slot `0x2e0` |
| `0x4cdb30` | TCharacter::Sight(dist) | thiscall, ret 4 | A | vtable slot `0x2e4` |
| `0x4cdbb0` | TCharacter::ResetStealthValues() | thiscall, ret 0 | A | writes `+0x130`/`+0x134`; caller `0x4c32c0` (slot `0x210`) |
| `0x4c5940` | TCharacter::SetHasSeen(chr) | thiscall, ret 4 | A | |
| `0x4c59e0` / `0x4c5a20` | IsNewlySeen(chr) / SetSeenFlags(b) | thiscall, ret 4 | B | the 8-entry memory (§2.3) |
| `0x4cdce0` | TCharacter::OnAttacked(attacker, victim, flag) (slot `0x240`) | thiscall, ret 0xc | B | port `SignalAttack`; callers ResolveAttack `0x4c6e9e`/`0x4c6eb1` |
| `0x4c1bb0` | TCharacter::Pulse | thiscall | A | slot `0x110`; chain/autocombo `0x4c250b`–`0x4c25cb`, player auto-combat `0x4c25d5`–`0x4c285d`, AI call `0x4c29a2` |
| `0x518aa0` | TPlayer::Pulse | thiscall | A | TPlayer slot `0x110`; calls `0x4c1bb0` first; **no combat logic of its own** (§3.12) |
| `0x489850` | SCharData::Load (char.def block) | thiscall | A | strings ATTACK `0x5d8fbc`, IMPACT `0x5d9118`… |
| `0x4c47d0` | TCharacter::GetDamageType(wtype, flags) | thiscall, ret 8 | A | slot `0x2d4` (COMBAT_HIT) |

Vtable slots used here (TCharacter `0x5a7848`; the table runs to
`0x5a7b94`, i.e. 0x350 bytes, not 0x2a0): `+4` Distance `0x4d61b0`,
`0x138` FindState, `0x13c` FindTransitionState, `0x178` NetOwner
`0x4d6020`, `0x1a8` Aggressive (stat `0x66caa4` "Aggressive"), `0x1b8`
Sleeping (`0x66ca58`), `0x1c0` Health (`0x66ca4c`), `0x1c8` Fatigue
(`0x66ca3c`), `0x1cc` SetFatigue, `0x1d0` Mana (`0x66ca38`), `0x1e0`
MaxFatigue (chardata `+0x1e4`; TPlayer `0x5206d0`), `0x1f0` HasActionAni
`0x4d6c20`, `0x1fc` GetDoing, `0x208` SetDesired `0x4db3a0`, `0x218`
ForceCommand, `0x220` DefaultRootState, `0x224` CalculateDamage, `0x258`
Radius `0x4d6e40`, `0x2b8` BlockFreq (chardata `+0x194`), `0x2c0` Defense,
`0x2c4` Offense, `0x2cc` WeaponType, `0x2d0` WeaponDamage, `0x2dc`
AmbientLight `0x4c5aa0`, `0x2e0` Hearing, `0x2e4` Sight, `0x2f4`
StrengthMod (0 for TCharacter; TPlayer `0x520900` = StatLevel(Strength)),
`0x2f8` Visibility (`+0x130`), `0x2fc` Noise (`+0x134`), `0x304` combat
root name (`"combat"`; TPlayer `0x51b340`), `0x308` bow root (`"bow"`),
`0x30c` walk root (`"walk"`; TPlayer `0x51b3f0`: `"torch"` when holding a
light, else `0x5e2948`), `0x310` weapon prefix `0x4cdf60`, `0x320`
ResolveAttack `0x4c6dd0`.

## 2. Layouts

### 2.1 SCharAttackData (0x320 bytes; char.def `0x489850`)

The parser zeroes a 0xc8-dword local, `Parse`s one line into it
(`0x47a410`), and `attacks.Add` (`0x48d230`: `malloc(0x320)` + copy +
pointer-array add) stores a copy. The attack table is a pointer array at
`chardata+0xcc`: count `+0xcc`, item pointers `+0xdc`, fallback record
`+0xe0` (used when an item pointer is null). Every reader does
`p = items[i] ? items[i] : fallback`.

| Off | Field | Source column (ATTACK) |
|---|---|---|
| `+0x00` | name[0x20] | 1 (`%30s`) |
| `+0x20` | int, never set in the stored copy (the parser writes the array index into its local *after* the copy, `0x489a72`) | — |
| `+0x24` | flags (CA_*) | 2 |
| `+0x28` | button | 3 |
| `+0x2c` | attackpcnt | 22 |
| `+0x30` | mindist | 12 |
| `+0x34` | maxdist | 13 |
| `+0x38` | responsename[0x20] | 4 |
| `+0x58` | blockname[0x20] | 5 |
| `+0x78` | missname[0x20] | 6 |
| `+0x98` | chainname[0x20] | 7 (the file's comment calls it "death") |
| `+0xb8` | blocktime | 8 |
| `+0xbc` | impacttime | 9 |
| `+0xc0` | chainexptime | 11 |
| `+0xc4` | nextwait | 10 |
| `+0xc8` | hitminrange | 14 |
| `+0xcc` | hitmaxrange | 15 |
| `+0xd0` | hitangle | 16 |
| `+0xd4` | damagemod | 17 |
| `+0xd8` | fatigue (cost / minimum) | 18 |
| `+0xdc` | attackskill | 19 |
| `+0xe0` | weaponmask | 20 |
| `+0xe4` | weaponskill | 21 |
| `+0xe8` | numimpacts | (IMPACT lines) |
| `+0xec` | swipeframeon | 23 |
| `+0xf0` | swipeframeoff | 24 |
| `+0xf4` | fatiguemax (FATIGUEATTACK only: usable while Fatigue ≤ this) | FATIGUEATTACK col 23 |
| `+0xf8` | impacts[6], stride 0x5c (§2.2) | |

Tags and what they set (format strings `0x5d8fc4`, `0x5d9080`,
`0x5d903c`, `0x5d90fc`):

- `ATTACK` — 24 columns as above. If responsename is non-empty, flags =
  `(flags & ~CA_SPECIAL) | CA_RESPONSE` (`0x489a40`).
- `FATIGUEATTACK` — 25 columns: ATTACK's 1–21, then attackpcnt `+0x2c`,
  **fatiguemax `+0xf4`**, swipeon `+0xec`, swipeoff `+0xf0`; flags `|=
  0x10000` (call it CA_FATIGUEATTACK). No response fix-up.
- `MAGICATTACK` — `name, flags, button, attackpcnt, mindist, maxdist,
  spell(%31s) +0x38, srcx +0x58, srcy +0x5c, srcz +0x60, condition
  +0x64, value +0x68`; flags `|= CA_MAGICATTACK 0x800000`. Condition
  (char.def `MASTAT_*`): 1 none, 2 HEALTHLT (valid while Health ≤ value),
  3 HEALTHGT (Health > value), 4 MANALT (Mana ≤ value), 5 MANAGT (Mana >
  value).
- `PLAYANIM` — `name, flags, button, mindist, maxdist, attackpcnt`;
  flags `|= CA_PLAYANIM 0x1000000`.
- `IMPACT` — appended to the last ATTACK (error if none, if it was a
  MAGICATTACK, or if it already has 6); `CHARIMPACT` to
  `chardata+0x218` (count `+0x214`, max 6).

Retail char.def defines two flags the port lacks: `CA_NOINTERRUPT
0x80000000` and `CAI_INTERACTIVE 0x80`; the code also uses `0x10000`
(FATIGUEATTACK, set by the parser only).

### 2.2 SCharAttackImpact (0x5c bytes)

`+0x00` name[0x20], `+0x20` index (its position; stored), `+0x24` flags
(CAI_*; `0x1000` set for CHARIMPACT, cleared for IMPACT), `+0x28`
loopname[0x20], `+0x48` looptime, `+0x4c` damagemin, `+0x50` damagemax,
`+0x54` snapdist, `+0x58` snaptime. Format `0x5d91dc` `%30s, %i, %30s,
%i, %i, %i, %i, %i`; a loopname without STUN/KNOCKDOWN/DEATH warns
(`0x5d9200`).

### 2.3 SCharData fields used (size 0x590, `malloc(0x590)` at `0x48bf35`)

GROUPS `+0x20`, ENEMIES `+0x70`, FLAGS `+0xc8` (CF_INFRAVISION 4,
CF_LIGHTBLIND 8), attacks `+0xcc`, DAMAGEMODS `+0xe4`[10], PLAYERBLOCK
`+0x14c/0x150/0x154`, COMBATRANGE `+0x158` (and `+0x15c` = second value,
or `+0x158 + 0x40` when omitted), MAXATTACKRANGE `+0x160`, CLASS `+0x190`,
BLOCK `+0x194/0x198/0x19c` (freq, min, max), SIGHT `+0x1a0` min, `+0x1a4`
max, `+0x1a8` range, `+0x1ac` angle, HEARING `+0x1b0` min, `+0x1b4` max,
`+0x1b8` range, WEAPONTYPE `+0x1bc`, WEAPONDAMAGE `+0x1c0`, ARMOR
`+0x1c4`, DEFENSEMOD `+0x1c8`, ATTACKMOD `+0x1cc`, ATTACKFREQ
`+0x1d0/+0x1d4`, MAGICFREQ `+0x1d8/+0x1dc`, MANA `+0x1e0`, FATIGUE
`+0x1e4`, HEALTH `+0x1e8`, RETREATAT `+0x440`, RETREATATMANA `+0x444`,
RETREATFOR `+0x448`, RUNFATIGUE `+0x44c/+0x450`.

### 2.4 TCharacter fields used

| Off | Meaning | Evidence |
|---|---|---|
| `+0x08` | object flags: `0x20` has AI, `0x40` AI disabled, `0x80` invisible, `0x800000` paralysed, `0x2000000` iced (1998 OF_ numbering, consistent with every use here) | AI `0x4c8b8e`, Pulse `0x4c29c6`, DoAttack `0x4d2178` |
| `+0x0e` | level (word) | iterators, LOS |
| `+0x28/+0x2c` | accum.x / accum.y (1998 SObjectDef order) | AI stuck test `0x4c932d` |
| `+0x40` | object id | AI/Pulse cadence |
| `+0x84` | script | BeginFighting `0x4d3f1b` |
| `+0xb0` | moveangle | |
| `+0xd8/+0xdc/+0xe0` | doing / desired / root; **the fighting target is `root->obj` (`+0x44`)** when root is COMBAT or BOW | everywhere |
| `+0xe8` | auto-combat armed: init from INI `AutoCombat` (`0x5d7a68`, `0x4c18e4`); set 1 when the player's scan finds nobody (`0x4c285d`) | |
| `+0x110` | charflags: `0x4` (don't turn / don't acquire / don't flee — exact name unknown), `0x80` can't land a killing blow (COMBAT_HIT), `0x4000` fights in the walk root, `0x8000` never a valid target, `0x10000` PLAYANIM prefix checks, `0x80000` in an interactive attack, `0x100000` AI-driven (player too) | |
| `+0x120` | **nextattack** (frames) | AI, IVA, SpecificAttack, BeginFighting |
| `+0x124` | **magic timer** (frames); the port calls it `waitticks` | AI `0x4c8d42`, IVA `0x4d137c` |
| `+0x128` | request bits: `1` attack now, `2` blocks PLAYANIM attacks, `4` try an interactive attack | OnAttacked `0x4cde42` (sets 7), DoAttack `0x4d237c`/`0x4d2390`, RandomAttack `0x4d29be`, AI `0x4c8d7c` |
| `+0x12c` | counter decremented by the AI after each successful RandomAttack (`0x4c8e79`); meaning unknown (not chainhits) | |
| `+0x130` / `+0x134` | visibility ("glimpse") / noise, 0..100 | ResetStealthValues |
| `+0x160` / `+0x164` / `+0x168` / `+0x16c` | lastattack / lastattackticks / (cleared with lastattack) / chainhits | Pulse, ButtonAttack |
| `+0x1a4` | "not targetable" (skipped by FindCharacters, IsValidTarget, BeginFighting); unidentified | |
| `+0x1c0 + 0xc·k`, k<8 | seen-memory `{chr, frame, noauto}` | SetHasSeen |
| `+0x234/+0x238` | AI waypoint / previous waypoint | |
| `+0x23c..+0x244` | target's last known position | AI `0x4c92ee` |
| `+0x248` | waypoint commit countdown (6) | |
| `+0x24c` / `+0x250` | Wander idle countdown / run-mode timer | `0x4c9790` |
| `+0x254` / `+0x258` / `+0x25c` | **retreating / retreat latch / retreat frames left**. The port's names (`target_out_of_sight`, `…_prev`, `sight_lost_ticks`) are wrong | AI tail `0x4c93ce`, StartRetreat, damage `0x4c57e9` |
| `+0x264` | net owner | slot `0x178` |
| `+0x28c` / `+0x290` | last attack button / repeat count (player: own presses; monster: the attacker's button) | ButtonAttack, OnAttacked, SetFighting; init −1/0 (`0x4c1961`) |

TPlayer: `+0x2b0` hand item, `+0x2bc` ranged weapon, `+0x36c` player
state, `+0x494` team name.

## 3. Behaviour

### 3.1 The attack search order

Four searches, all ending in IsValidAttack (IVA). Every search shares one
set of out-values (`impact`, `damage`, `tohit`, `roll`) that the caller
initialises to −1 **once**; IVA resets only `*impact` per candidate. The
first candidate that reaches IVA's damage block fixes damage/to-hit/roll
for the rest of the search (COMBAT_HIT §3.1).

**FindButtonAttack `0x4d1dd0`** (button presses):

```
tdist = targ ? Distance(targ) : 10000
for mode in 0..2:                         // RESPONSE, then SPECIAL, then plain
    flags = {2, 1, 0}[mode] | (isaction ? 0x40000000 : 0)
    for i in 0..numattacks-1:             // index order
        if IVA(i, …, tdist, button, pcnt=0, arg2, mask=3, flags, targ): *out=i; return 1
return 0
```

`isaction` sets a bit the mask (3) can never match, so **ButtonAction
never finds an attack** in retail (the port's FindButtonAttack does the
same: `CA_ACTION` outside `CA_RESPONSE|CA_SPECIAL`).

**FindPcntAttack `0x4d1eb0`** (monsters): requires `root->action ∈ {3,
0x19}` **and `root->obj != 0`**, else 0. `tdist = Distance(root->obj)`.
Up to `2·n` tries of `i = random(0, n−1)` (`0x4d1f49`; no draw when n =
1), each `IVA(i, …, tdist, button=−1, pcnt, arg2, 0, 0, root->obj)`.

**FindInteractiveAttack `0x4d1ff0`**: same root/target requirement;
index order over attacks with `flags & CA_INTERACTIVE` only;
`IVA(i, …, tdist, −1, pcnt, arg2, 0, 0, targ)`.

**SpecificAttack `0x4d2a60`**: one IVA on the given index (§3.6).

IVA's own order (the shape gates COMBAT_HIT leaves to C3), `0x4d1120`:

1. `*impact = −1`; `i >= numattacks` → 0.
2. `this+0x128 & 2` and PLAYANIM → 0.
3. `!HasActionAni(ad->name, 0)` → 0 (**first**, before fatigue).
4. Fatigue: player with cheat `0x668108` ("nahkranoth"): FATIGUEATTACK
   → 0, else no fatigue test. Otherwise FATIGUEATTACK needs `Fatigue() ≤
   ad->fatiguemax`; others need `Fatigue() ≥ ad->fatigue`.
5. `(ad->flags & mask) != flags` → 0.
6. `button >= 0 && ad->button != button` → 0.
7. `ad->attackpcnt < pcnt` → 0.
8. `ad->maxdist > 0 && targ`: `tdist ∉ [mindist, maxdist]` → 0.
9. Mode: SNEAKMODE → `root->Is("sneak")`; WALKMODE → `root->Is("walk")`;
   BOWMODE → `root->action == 0x19`; otherwise `root->action == 3 &&
   !root->Is("walk")`. Null root → 0. **No `IsFighting` test**: the
   target may be null.
10. PLAYANIM: if `!(charflags & 0x10000)` → **valid (1)**; else name
    starting `c/C` needs `root->Is("combat")`, `w/W` needs
    `root->Is("walk")`, any other → 1.
11. MAGICATTACK: `+0x124 != 0` → 0; condition `+0x64` (§2.1) against
    Health/Mana and `+0x68`; then `+0x124 −= 1` (to −1) and **return 1**
    (no target gates, no damage).
12. `+0x120 != 0` and not a player → 0 (**nextattack gate**).
13. doing is ATTACK and `frame < doing->attack->nextwait` → 0.
14. `HasActionAni(ad->name, 0)` again → 0 if missing.
15. CA_MOVING: doing ∈ {2, 4, 0x1a}. CA_RUNNING: root COMBAT and
    `Is("combatrun")`/`Is("handrun")`, or BOW and `Is("bowrun")`, or
    `root->Is("run")`.
16. With a target: ATTACKSTUN → `targ->doing->action == 0xd`;
    ATTACKDOWN → `== 0xe`; responsename → `targ->doing->Is(response)`.
17. `(flags & (CHAIN|AUTOCOMBO))` and chainname: `lastattack` set,
    `stricmp(lastattack->name, chainname) == 0`, and `GameFrame −
    lastattackticks <= lastattack->chainexptime`.
18. INTERACTIVE: needs a target; unless `targ->charflags & 0x80000`,
    refuse when the target's doing attack is INTERACTIVE or its impact has
    `0x80`. Any target whose doing impact has `0x80` → 0.
19. Player: `1 << WeaponType()` in weaponmask; `stat 20 (AttackLevel) ≥
    attackskill`; `stat 43+WeaponType ≥ weaponskill`; then arg 9 ← 
    StrengthMod; attack `"sunsetflipper"` only against a target named
    `"Baez"` (`0x4d16a6`).
20. Damage / to-hit / roll: COMBAT_HIT §3.1.
21. CA_DEATH: needs target and `*damage ≥ targ->Health()`. (Then two
    min/max expressions call `targ->Health()` 2–4 times with the results
    discarded, `0x4d1963`–`0x4d19ba`, `0x4d19d6`–`0x4d1a16`: seam calls
    only.)
22. **Impact choice** (`numimpacts > 0` and a target), for `k` in order:
    considered when `(damage ≥ targ->Health() && imp.flags & CAI_DEATH)
    || *impact < 0`. **damagemin/damagemax are never read.** A considered
    impact:
    - `flags & 0x80` and `snapdist > 0`: point `snapdist` ahead of the
      attacker (`0x46f010`); `targ->FindClearPath(&targ->pos, &pt, 0, 0,
      &blocker)` blocked by anything but the attacker → **attack refused**.
    - CAI_DEATH: `targ->HasActionAni(imp.name)`; else built names:
      `imp.name == "combat to dead"` → `<targ root> to <prefix|"c">dead`
      (prefix = slot `0x310` for a player target), any other name must be
      `"dead"` → `BuildActionName(targ, "dead")` (`0x4ce1b0`); missing →
      **attack refused**. A loopname must exist as a state
      (`FindState(loop, −1)`), with `"dead"` falling back to the built
      name; missing → refused.
    - Otherwise: name = `imp.name` if `flags & 0x80`, else
      `BuildActionName(targ, imp.name)`; `targ->HasActionAni(name)` and
      the loop state must exist, else **attack refused**.
    - Accepted → `*impact = k`. So the first impact wins, a later death
      impact overrides it when the damage kills, and a target missing the
      animation makes the whole attack invalid.
    After the loop it tests `impacts[numimpacts].flags & 0x80` (one past
    the last; zero unless numimpacts = 6, when it reads past the record)
    with `damage ≥ Health` → refuse.
23. `charflags & 0x80` gate and `Block(−2)`: COMBAT_HIT §3.1. Return 1.

### 3.2 ButtonAttack `0x4d2480`

```
if Health() <= 0 → 0;  interactive gate → 0
repeat = 0
targ = root(3|0x19)->obj  or  FindCharacters(&t, 1, −1, facing, 0x20, 7) ? t : 0
                                              // the search happens even if a chain follows
if lastattack && lastattack->flags & CA_CHAIN
   && GameFrame − lastattackticks <= lastattack->chainexptime && chainhits < 3:
    chainhits++; return 1                     // the press only feeds the chain (§3.11)
if this is player && targ && targ is not a player && (!MP || MP server):
    if button == +0x28c: if ++(+0x290) >= 3: repeat = 1
    else: +0x290 = 1
    +0x28c = button
    if repeat && random(0,10) == 0:           // 0x4d2620, 1 in 11
        r = random(1,50)                      // 0x4d2634
        targ+0x120 = 0;  targ->SetFatigue(targ->MaxFatigue())
        a,imp,dmg,tohit = −1;  roll = 100     // preset: no roll is drawn
        for k in 0..9:                        // the monster's counter
            found = targ->FindInteractiveAttack(0, r+50, …) || targ->FindPcntAttack(0, r+50, …)
            if found && !(attacks[a]->flags & (CA_PLAYANIM|CA_MAGICATTACK)): break
            found = 0
        if found && targ->DoAttack(a, imp, dmg, tohit, 0 /*roll*/, this): return 0
r = random(1,50) + random(1,50)               // 0x4d2746 / 0x4d2751, unused (COMBAT_HIT)
a,imp,dmg,tohit = −1;  roll = repeat ? 100 : −1
if FindButtonAttack(button, r, &a, &imp, &dmg, &tohit, &roll, 0, targ):
    return DoAttack(a, imp, dmg, tohit, roll, targ)
return 0
```

**The same-button rule.** The third and later consecutive presses of the
same button against the same target (SetFighting resets `+0x28c = −1`,
`+0x290 = 0` on a target change, `0x4d48e2`) get roll 100 — the worst
damage tier, and usually the `Block(−2)` trigger — and each has a 1-in-11
chance that the monster counterattacks at once instead (its cooldown
cleared, fatigue refilled; the counter's damage uses roll 100 but its
block carries roll 0) and the press is swallowed. The monster side of the
same rule is OnAttacked (§3.13): on the third repeat it blocks.

### 3.3 ButtonAction `0x4d27f0`

Health / interactive gates; `targ = FindCharacters(&t, 1, 0x200, facing,
0x20, 0)` (anything ahead within 512, not just enemies);
`r = random(1,50)+random(1,50)`; FindButtonAttack(button, r, …,
isaction=1, targ) → DoAttack. Always fails at the flag test (§3.1).

### 3.4 DoAttack `0x4d2120`

```
if doing->action == 0xb INVOKE → 0;  Health() <= 0 → 0
if objflags & 0x2800000 (paralysed | iced) → 0;  i >= numattacks → 0
MP && NetOwner() < 1 && !host(0x676e5c) → 0
player with state bit 2 → TPlayer::SetPlayerState(state & ~2)   // 0x51d680
ad = attacks[i]
if ad->flags & CA_MAGICATTACK:
    Health/interactive gates again; objflags & 1 → 0; & 0x2800000 → 0
    spell = SpellList(0x667c38)->Find(ad+0x38)  (0x53f010);  none → 0
    return Cast(spell+0x24, &targ, targ != 0, ad+0x58)   // 0x4d5c20
NetSend 0x584150(this, i, imp, dmg, tohit, roll, targ)   // no-op when [0x676828]==0
ab = new TActionBlock(ad /*name*/, 7 ATTACK)             // always ATTACK, PLAYANIM too
ab->obj = targ; ab->attack = ad
ab->impact = imp >= 0 ? ad+0xf8+imp·0x5c : 0
ab->damage(+0x50) = dmg; ab+0x54 = tohit; ab+0x58 = roll
ab->flags: priority(0x10) = ad->flags & CA_SPECIAL; |= interrupt(0x20)
           if (ad->flags & CA_CHAIN) && chainname[0]: clear interrupt
if ad->flags & CA_INTERACTIVE: +0x128 &= ~4
if !(ad->flags & CA_PLAYANIM): +0x128 &= ~2
root->angle = root->moveangle = doing->angle
ab->moveangle = doing->angle;  if interrupt: ab->angle = doing->angle
ab->flags |= noroot(0x400)
if SetDesired(ab, 0):
    if !(PLAYANIM) && !(charflags & 4): this->moveangle = facing; return 1
    if root(3|0x19)->obj: facing = AngleTo(root->obj)   // PLAYANIM / charflags 4
    return 1
free ab; return 1                                       // refused still returns 1
```

### 3.5 RandomAttack `0x4d2900` (AI)

Health / interactive gates; `r = random(1,50)+random(1,50)`; outs −1. If
`+0x28c >= 0` and `+0x128 & 4`: `FindInteractiveAttack(pcnt, r, …)`, then
`+0x128 &= ~4` whatever the result. If that found nothing:
`FindPcntAttack(pcnt, r, …)`; nothing → 0. `DoAttack(…, root->obj)`.

### 3.6 SpecificAttack `0x4d2a60`

Health / interactive gates; `targ = root(3|0x19)->obj`, else
`FindCharacters(&t, 1, −1, facing, 0x20, 7)`; `tdist = targ ?
Distance(targ) : 10000`; `r = random(1,50)+random(1,50)`; outs −1;
**`+0x120 = 0`**; `IVA(i, …, tdist, button −1, pcnt −1, r, 0, 0, targ)` →
DoAttack.
