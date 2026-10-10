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

### 3.7 BeginFighting `0x4d3b90` (targ, action)

```
if root:
    if root->action == 3 && (root->Is("combatrun") || root->Is("handrun")) → 0
    if root->action == 0x19 && root->Is("bowrun") → 0
    if root->Is("run") → 0
if root && root->Is("sneak") → 0
if doing->action == 0xb INVOKE → 0
MP && NetOwner() < 1 && !host → 0
memory[k].noauto = 1 for k < 8          // marks everyone seen as "handled" (§3.11)
if targ && (targ->Health() <= 0 || targ+0x1a4) → targ = 0
if action == 0x19 && player && !this+0x2bc (ranged weapon) → 0
if !targ → targ = FindCharacters(&t, 1, −1, facing, 0x20, 7) ? t : 0
if !MP → TextBar(0x65c5d0)->0x54d390()  // UI refresh (seam)
if root->action == action → return SetFighting(targ)
NetSend 0x5840b0(this, targ, action)
name = charflags & 0x4000 ? WalkRoot(0) : action 0x19 ? BowRoot(0) : action 3 ? CombatRoot(0)
       : Error("Invalid fighting action")
if FindState(name, −1) < 0 → 0
if doing->action ∈ {2, 4, 0x1a} → Stop(0)
ab = new TActionBlock(name, action); ab->obj = targ
ab->flags = (flags & ~0x10) | 0x20 | (FindTransitionState(doing->name, name, −1) >= 0 ? 0x10 : 0)
if doing->Is(WalkRoot(0))                         → doing->flags &= ~0x10
else if action 0x19 && doing->action ∈ {3, 0xc}   → doing->flags &= ~0x10
else if action 3 && doing->action == 0x19         → doing->flags &= ~0x10
ab->angle = (targ && !retreating) ? AngleTo(targ) : facing
ab->moveangle = doing->action ∈ {2, 4, 0x1a} ? this->moveangle : ab->angle
if !SetDesired(ab, 0): free ab
if script(+0x84) && Trigger 0x492640(10, 0, 0, root->obj, "user", root->obj, "enemy"): charflags |= 2
+0x120 = −1
return 1                                          // also when SetDesired refused
```

Characters with `charflags & 0x4000` fight in their walk root (the AI
attacks from `doing->Is("walk")` for them, §3.10).

### 3.8 EndFighting `0x4d3fd0`

```
if root->action ∉ {3, 0x19} → 1
Health() <= 0 → 0;  interactive gate → 0;  MP gate → 0
if FindState(WalkRoot(0), −1) < 0 → 0
if doing->action ∈ {2, 4, 0x1a} → Stop(0)
if 0x57d9d0(this, 0x20, 1, 0, 0): 0x57dc70()      // net/script log; no-op without a session
ab = new TActionBlock(WalkRoot(1), 1 ANIMATE)
t = FindTransitionState(doing->name, ab->name, −1) >= 0
ab->flags = (flags & ~0x12) | (t ? 0x10 | 0x2 : 0) // priority and transition together
if doing == root → doing->flags &= ~0x10
if !SetDesired(ab, 0): free ab
if !(charflags & 4) → moveangle = facing
memory[k].noauto = 1 for k < 8
return 1
```

The root's target is not cleared here; the new walk root has none.

### 3.9 SetFighting `0x4d4790` (already ported)

MP / Health / interactive gates; refuses `targ == this` and a dead
target; not fighting and a target → `BeginFighting(targ, 3)`; same
target → 1; else net send, `doing/desired/root->obj = targ`, their angles
`= AngleTo(targ)` unless retreating, and for the player `+0x28c = −1`,
`+0x290 = 0`.

### 3.10 AI `0x4c8b60` — combat parts

Called from Pulse (`0x4c29a2`): with `charflags & 0x100000` it runs
Wander (slot `0x234`) then AI; otherwise AI only when `objflags & 0x20`.

```
if NetOwner() < 1 → return                          // slot 0x178; SP value: §7
if player && !(charflags & 0x100000) → return
if objflags & 0x40 || Health() <= 0 || cheat 0x668110 ("dummies"):
    if doing->action ∈ {2, 4, 0x1a}: Stop(0)
    return
if [0x668154] → return
if Aggressive(): AI_PerMonster()                    // 0x4c9b70, not read
targ = root(3|0x19)->obj
if !IsValidTarget(targ) || !targ:                   // §3.10.1 acquisition
    if [0x668154] || !Aggressive() || charflags & 4 || (GameFrame ^ id) & 0x1f → tail
    targ = FindCharacters(&t, 1, −1, −1, 0x20, 7) ? t : 0;  none → tail
    if !retreating(+0x254): BeginFighting(targ, 3)
if !doing → tail
if (doing->Is("walk") && !retreating && charflags & 0x4000)
   || (doing->action == 3 && !retreating)
   || (+0x128 & 1 && +0x128 & 6):    ATTACK BRANCH (3.10.2)
else:                                MOVE BRANCH (3.10.3)
tail:                                RETREAT STATE (3.10.6)
```

3.10.1 **Acquisition cadence.** Only when the current root target fails
IsValidTarget, and only on frames where `(GameFrame ^ this->id(+0x40)) &
0x1f == 0` (once every 32 frames, phase by id). The search has no
direction (`angle −1`), flags ENEMY|HEAR|SEE. A retreating monster takes
the target but doesn't BeginFighting.

3.10.2 **Attack branch.**

```
+0x234 = 0                                         // drop the waypoint
if +0x120 > 0: +0x120--                            // nextattack
if +0x124 > 0: +0x124--                            // magic timer
if +0x120 == 0 || +0x124 == 0 || +0x128 & 1:
    +0x128 &= ~1
    if lastattack && doing->Is(lastattack->name): (skip)
    else if Distance(targ) > MAXATTACKRANGE(+0x160):
        if Go(AngleTo(targ)): NetLog 0x583e80(this, 0x1c, angle, 1, 0)
        lastattack = 0; +0x168 = 0
    else:
        ok = RandomAttack(random(1,100))           // 0x4c8dbb
        b = FindCharInLine(this, &pos, level, Radius()·2)   // 0x4d4db0
        if b && b != targ && !ok:
            d = FacingDiff(b)                      // 0x46ead0, AngleTo(b) − facing, ±128
            if 32 < d < 96: SideStep('l') elif −96 < d < −32: SideStep('r')
        if ok: +0x12c--
        else:  lastattack = 0; +0x168 = 0
    if !lastattack || !(lastattack->flags & CA_PLAYANIM): +0x120--   // second decrement
if +0x120 < 0: +0x120 = random(ATTACKFREQ.min·24/100, ATTACKFREQ.max·24/100)
if +0x124 < 0: +0x124 = random(MAGICFREQ.min·24/100,  MAGICFREQ.max·24/100)
```

Consequences: a successful attack re-arms `nextattack` the same tick (0 →
−1 → random). When the attempt was triggered by `+0x124 == 0` (always
true for a monster with no MAGICATTACK: the magic timer only goes below
0 through IVA's magic branch) while `+0x120 > 0`, IVA refuses every
non-magic attack, lastattack is cleared, and `+0x120` loses 2 per tick —
the effective attack interval of a non-caster is about half the
ATTACKFREQ draw. The divisions are `·24/100` signed truncation.

3.10.3 **Move branch.**

```
if retreating && !(charflags & 4):
    +0x234 = 0; a = (AngleTo(targ) + 0x7f) & 0xff
    if Go(a): NetLog(0x1c, a, 1, 0)                // run directly away (127°)
    → tail
if doing->action ∈ {2, 4, 0x1a}:                    // stepping
    if !targ → tail
    if Distance(targ) < MAXATTACKRANGE && !+0x234: Stop(0) → tail
    if lastattack && doing->Is(lastattack) && lastattack is PLAYANIM → tail
    if CanSee(targ, −1)
       || (!(targ objflags & 0x80) && Distance(targ) <= HEARINGRANGE
           && targ->Noise() > 100 − Hearing(Distance(targ))):
        CHASE (3.10.4)
    else LOST (3.10.5)
else:                                               // standing, not in combat stance
    if accum.x == 0 && accum.y == 0
       && FindClearPath(&pos, &pos, 8, 0, &b) && b:
        s = FacingDiff(b) >= 0 ? 'r' : 'l';  SideStep(s)
        if b != root->obj: b->SideStep(s == 'l' ? 'r' : 'l')   // the blocker steps the other way
```

3.10.4 **Chase.** Waypoint commit `+0x248` counts down (clear waypoint at
0); last known position `+0x23c = targ->pos`; `doing->angle(+0x2c) =
AngleTo(waypoint if committed and the countdown non-zero, else targ)`.
Only `doing->angle` is written (the step's facing; Go/ResolveCombat
carry the movement).

3.10.5 **Lost.** Centre = last known position. If the target's own root
target `t2 != this` is visible: `s = random(0,1) ? 'l' : 'r'`;
`SideStep(s)`; `t2->SideStep(opposite)`. If a waypoint is committed and
`Distance(waypoint) < 5`, centre = the target's current position, else
keep the waypoint (→ face). Search: `Map(0x6668d8)->FindObjectsInRange(
&centre, level, out, 0xfa, 0, 0xf, 10, 0)` (`0x452060`, max 10), keep
objects whose type name is `"waypoint"` and `WaypointReachable(this)`
(`0x528850`); closest by `0x46de60` below 1000 (strict), calling
`0x470bc0(&pos)` on each new best (side effect unknown). Same as the
committed one → uncommit; else commit with countdown 6. Face the
waypoint, or with none `doing->angle = ConvertToFacing(pos, centre)`
(`0x46dc60`).

3.10.6 **Retreat state (tail, every tick the AI gets past its gates).**

```
v = (Health() <= RETREATAT(+0x440) || +0x258) && +0x25c != 0
+0x254 = +0x258 = v
if +0x25c > 0: +0x25c--
```

`+0x25c` is armed with RETREATFOR (`+0x448`) when a hit leaves the
character at `1 ≤ Health ≤ RETREATAT` (`0x4c57e9`, in Damage), and with
96 by StartRetreat `0x4d5fc0` (`SetFighting(0)`; `Stop(0)` unless already
retreating; `+0x25c = 0x60`; `+0x254 = +0x258 = 1`). It is cleared with
the flags when a retreating character is blocked (`0x4c45d1`, in the
mover). RETREATATMANA (`+0x444`) has no reader in the AI (unverified
elsewhere).

### 3.11 TCharacter::Pulse `0x4c1bb0`: chains and player auto-combat

Runs for a living character (or `charflags & 8`), before the AI:

```
la = lastattack
if la && GameFrame >= lastattackticks + la->nextwait
   && (la->flags & CA_AUTOCOMBO || (la->flags & CA_CHAIN && chainhits > 0)):
    if !(la->flags & CA_CHAIN) || GameFrame − lastattackticks < la->chainexptime:
        for i in attacks (index order):
            if stricmp(la->name, attacks[i]->chainname) == 0:
                if la->flags & CA_CHAIN: chainhits--
                SpecificAttack(i)                  // result ignored; retried next tick
                goto done
    chainhits = 0
done:
if player:                                         // auto-combat
    skip if root is combatrun/handrun/bowrun/run or Is("sneak")
    skip unless (GameFrame ^ id) & 0x1f == 0
    n = FindCharacters(&t, 1, −1, facing, 0x20, 7)
    if n <= 0 || !t: if !+0xe8: +0xe8 = 1; skip
    skip unless IsValidTarget(t)
    skip if root->action ∈ {3, 0x19} or root->Is("sneak")
    skip if !(GameType[[0x65a784]]->flags & 8) && t is a player     // no PvP
    skip unless AutoCombat (0x5d7a68) && +0xe8
    skip if t is in the memory with noauto set
    skip unless Distance(t) < COMBATRANGE(+0x158)
    skip unless |AngleDiff(facing, AngleTo(t))| < 0x30
    skip unless PlayScreen control (0x65d0d0) || 0x65d0c8
    BeginFighting(t, 3)
```

This is identical to the port's chain block; the auto-combat differs
(§6.8).

### 3.12 TPlayer::Pulse `0x518aa0`

Calls TCharacter::Pulse first; the rest is the play clock, stat refresh,
death countdown (`+0x39c`, 192) and the held light's flicker. **No
combat logic.** Its RNG (only with a lit light item, `slot 0x250` and
`+0x58`): `random(−5,5)` ×2 (`0x518c46`, `0x518c60`), then when the light
is not fading `random(0,2)` (`0x518cd3`) and, if that is 0,
`random(8,30)` (`0x518ce3`). These interleave with combat draws on the
tape.

### 3.13 OnAttacked `0x4cdce0` (slot `0x240`, attacker, victim, flag)

Called by ResolveAttack when an attack starts. Returns at once unless
`victim == this`, the interactive gate passes, **this is not a player**,
attacker non-null, not MP client.

```
if !root(3|0x19)->obj || Distance(attacker) < Distance(root->obj)·75/100:
    SetFighting(attacker)
a = attacker->GetDoing()->attack
if a: if a->button == +0x28c: if ++(+0x290) >= 3: repeat = 1
      else +0x290 = 1
      +0x28c = a->button
else: +0x28c = −1; +0x290 = 0
if a && a->flags & 0x10000 (FATIGUEATTACK): +0x128 |= 7; return   // attack now, interactive allowed
if repeat: Block(−1); return
if !flag && attacker == root->obj && random(1,100) <= BLOCK.freq(+0x194)
   && attacker passes the interactive gate: Block(−1)
```

### 3.14 Target finding

**FindCharacters `0x4cd690`** (chars, max, range, angle, cone, flags;
flags 1 ENEMY, 2 HEAR, 4 SEE):

```
if max < 1 → 0
chars[0] = 0; best = 10000; n = 0
if range < 0 || flags & 2: range = max(range, HEARINGRANGE(+0x1b8))
if range < 0 || flags & 4: range = max(range, SIGHTRANGE(+0x1a8))
for obj in MapIterator(&pos, range, 0xe0, 2, 0, level):   // 0x44ceb0 / 0x44d080
    if obj == this || obj+0x1a4: continue
    if flags & 1:
        if !IsEnemy(obj): SetHasSeen(obj); continue
        if !IsValidTarget(obj): continue                  // no SetHasSeen here
    d = Distance(obj); if d > range: continue
    bearing = AngleTo(obj)
    hear = 1
    if flags & 2:
        hear = !(obj objflags & 0x80) && Distance(obj) <= HEARINGRANGE
               && obj->Noise() > 100 − Hearing(Distance(obj))
    see = flags & 4 ? CanSee(obj, angle) : 1
    if flags & 6:
        if hear || see: SetHasSeen(obj)
        else if !(obj in memory && GameFrame − memory.frame < 0x438): continue   // 45 s
    score = d
    if angle >= 0:
        diff = |AngleDiff(angle, bearing)|; if diff > cone: continue
        score = d · (cone − diff + 1)          // an object at the cone's edge scores lower
    if chars[0] && score <= best: swap(obj, chars[0]); best = score
    if n < max: chars[n++] = obj
return n
```

`best` is only set by a swap, so the first object found never sets it:
with `max == 1` the second object found within `best ≤ 10000` replaces
the first whatever their distances. The port reproduces this. Iteration
order is the map iterator's; the fixture must give both sides the same
order.

**CanSeeCharacter `0x4cd540`** (chr, angle): chr `objflags & 0x80` → 0.
Eyes at `pos.z + 50` both; `dist = 0x46de60(eyeThis, eyeChr)` (octagonal
x/y approximation via table `0x5e9200`; not edge-to-edge);
`angle < 1` → facing; `diff = |AngleDiff(angle, AngleTo(chr))|`; `vis =
chr->Visibility()(+0x130)`, 10000 with CF_INFRAVISION, `100 − vis` with
CF_LIGHTBLIND. Refuse if `dist > SIGHTRANGE`, `diff > SIGHTANGLE`, no
line of sight (`Map->0x4533d0(&eyeThis, &eyeChr, level, 0, 0)`), or `vis <
100 − Sight(dist)`.

**Hearing `0x4cda80`** (dist): `d = max(0, dist − Radius() − 32)`; `d >
HEARINGRANGE` → 0; `v = ((random(−2,2) + 100)·d / HEARINGRANGE) / 100`
(0 or 1); halved when Sleeping; `return max(v, 100)` → **always 100 in
range**. HEARING min/max (`+0x1b0/+0x1b4`) are not read.

**Sight `0x4cdb30`** (dist): `d = max(dist, 0)` (no radius); Sleeping or
`d > SIGHTRANGE` → 0; `v = ((random(−2,2)+100)·d / SIGHTRANGE)/100`,
clamped 0..100 → **0, or 1 at the very edge**. SIGHT min/max not read.

So a target is *seen* only when its visibility is ≥ 99–100, and *heard*
whenever its noise is > 0 within HEARINGRANGE. In practice monsters find
the player by hearing.

**ResetStealthValues `0x4cdbb0`**: `base = doing is ATTACK ? 100 : 70`;
halved when `root->Is("sneak")`; `r = random(1,25)` (`0x4cdc02`);
`noise(+0x134) = 2·r·base/100`; `vis(+0x130) = clamp(AmbientLight() + r,
10, 100) · base / 100`, clamped 0..100, where `AmbientLight =
min([0x6671a4], 255)·100/255` (`0x4c5aa0`). Its caller (slot `0x210`,
`0x4c3260`) calls it every tick except on frames where `GameFrame % 24 ==
0` and (`+0x80 == 0` or root == doing) and both values ≥ 0 — i.e. almost
every tick, one draw per character.

**SetHasSeen `0x4c5940`**: (a dead `stricmp(chr->name, "Locke")`); if
`chr` is in the 8-entry memory, refresh its frame (noauto unchanged);
else replace the entry with the smallest frame (first on ties): `{chr,
GameFrame, noauto = 0}`. IsNewlySeen `0x4c59e0`: not in memory → 1, else
`noauto == 0`.

**IsEnemy `0x4c89c0`** (chr):

```
if chr is a player:
    if chr->state(+0x36c) & 2 → 0
    if this is a player:
        if this->team(+0x494)[0] && stricmp(team, chr->team) == 0 → 0
        if !chr->0x51e480() || !this->0x51e480() → 0     // PvP enabled on both (unverified name)
if chr->root(3|0x19)->obj == this → 1                   // he's fighting me
if ListIn(ENEMIES, chr->name) || ListIn(ENEMIES, chr->typename)
   || any ListIn(ENEMIES, group) for group in chr->GROUPS:
    return chr->Aggressive() || chr is a player
return 0
```

### 3.15 Wander `0x4c9790` (slot `0x234`, `charflags & 0x100000` only)

Combat-relevant parts (asm checked at the cited lines, rest from the
decompile): `+0x24c` idle countdown returns early; in a COMBAT root with
**no target** → EndFighting, `+0x24c = 100` (`0x4c9a7a`); running in a
combat/bow root with a target → SetWalkMode + `BeginFighting(target, 3)`
(`0x4c9899`); while retreating, `random(1,100) > 30` →
`+0x250 = random(800, 2000)` and SetRunMode, counting down to SetWalkMode.
Out of combat it patrols waypoints with run bursts (`random(1,100) ==
100` → `random(100, 800)` frames). Not needed for the monster katas.

## 4. RNG draws

All through `random(lo,hi)` `0x483300` (no draw when lo == hi).

| # | Site | Range | Decides | When |
|---|---|---|---|---|
| 1 | `0x4d2620` ButtonAttack | (0,10) | counterattack (== 0) | player, 3rd+ same-button press on a non-player |
| 2 | `0x4d2634` | (1,50) | counter's arg 2 (unused) | after #1 == 0 |
| 3,4 | `0x4d2746`, `0x4d2751` | (1,50) ×2 | 1998 dmgpcnt, unused | every press not taken by a chain or a counter |
| 5,6 | `0x4d286e`, `0x4d2879` ButtonAction | (1,50) ×2 | unused | every action press |
| 7,8 | `0x4d2953`, `0x4d295e` RandomAttack | (1,50) ×2 | unused | every AI attempt that passes the gates |
| 9,10 | `0x4d2b09`, `0x4d2b14` SpecificAttack | (1,50) ×2 | unused | after its optional FindCharacters |
| 11 | `0x4d1f49` FindPcntAttack | (0, n−1) | candidate index | each of up to 2n tries |
| 12–14 | IVA `0x4d1886`, `0x4d18b4`, `0x4d18bf` | (0,9), (1,50), (0,50) | to-hit +5, roll | COMBAT_HIT §4 |
| 15 | `0x4c8dbb` AI | (1,100) | RandomAttack's pcnt | attack branch, in range |
| 16 | `0x4c90ef` AI | (0,1) | side-step side | lost target, its opponent visible |
| 17 | `0x4c8ef1` AI | (AF.min·24/100, AF.max·24/100) | nextattack | `+0x120 < 0` |
| 18 | `0x4c8f4d` AI | (MF.min·24/100, MF.max·24/100) | magic timer | `+0x124 < 0` |
| 19 | `0x4cdac9` Hearing | (−2,2) | (result ends up 100 regardless) | each heard candidate within range |
| 20 | `0x4cdb64` Sight | (−2,2) | edge-of-range 1 | each CanSee that passed range, cone and LOS |
| 21 | `0x4cdc02` ResetStealthValues | (1,25) | noise, visibility | almost every tick, every character |
| 22 | `0x4cdea6` OnAttacked | (1,100) | block | attacked monster, not a repeat, flag 0, attacker is its target |
| 23–26 | TPlayer::Pulse | (−5,5)×2, (0,2), (8,30) | light flicker | player holding a lit light |
| 27+ | Wander `0x4c98e6`… | (1,100), (100,800), (800,2000) | run toggles | `charflags & 0x100000` |

Order inside one ButtonAttack: FindCharacters' draws (#19/#20 per
candidate, only when the root has no target) → #1 → (#2, the monster's
#11/#12–14) → #3, #4 → FindButtonAttack's IVA draws #12–14 (the roll
pair skipped when the repeat preset it to 100). Inside one AI tick:
FindCharacters (acquisition frame only) → #15 → RandomAttack #7, #8 →
#11 / #12–14 → (FindCharInLine) → #17 → #18.

## 5. Fixture plan

### 5.1 Katas

- **C3a search**: FindButtonAttack / FindPcntAttack /
  FindInteractiveAttack / SpecificAttack over a chardata built from the
  real char.def records (Locke, Rahul, Araknid): seams for HasActionAni,
  FindState, stats; compare chosen index, out-values, IVA reject reasons
  (by coverage), draws.
- **C3b ButtonAttack**: chain counting, the repeat rule and the counter
  (two characters), roll preset; DoAttack's block by meaning.
- **M9a AI tick**: attack branch timers over N ticks with RandomAttack
  original; the move branch with Go/Stop/SideStep seams; acquisition
  cadence; retreat tail.
- **M9b FindCharacters**: iterator seam returning an ordered list,
  LOS seam, memory contents, stealth values as inputs.

### 5.2 Callees

| Callee | Run as | Seam records / answers |
|---|---|---|
| `random` `0x483300` | tape | (lo, hi, value) |
| GameFrame `0x47e920` | original (PlayScreen `0x65caf0` frame set per case) | — |
| AngleTo `0x46ea90`, FacingDiff `0x46ead0`, AngleDiff `0x46ded0`, PointDist `0x46de60` (table `0x5e9200`), PointAhead `0x46f010` (tables `0x634d44`/`0x634f44`), ConvertToFacing `0x46dc60` | original | — |
| stricmp `0x59a530`, Is `0x4dab80`, list helpers `0x483460`/`0x4833a0`/`0x483330`, TActionBlock ctor `0x4da9f0`, malloc/free | original | — |
| GetDamageType `0x4c47d0`, CalculateDamage `0x4c4860`, IsEnemy, IsValidTarget, SetHasSeen, IVA, the searches, DoAttack, ButtonAttack | original | — |
| Distance slot 4 `0x4d61b0` | original | Radius via GetStat seam |
| GetObjStat slot `0xdc`, GetStat `0xd8`/`0xd4` | seam | stat id → case value (Health, Fatigue, Mana, Aggressive, Sleeping, DamageMod, AttackLevel 20, weapon skill 43+wt, Strn 34, EdgeBonus 33, "Value") |
| TPlayer Offense/Defense/Luck/WeaponType/WeaponDamage (`0x51a520`/`0x51a550`/`0x51a580`/`0x520810`/`0x520830`) | seam | case values |
| HasActionAni `0x1f0`, FindState `0x138`, FindTransitionState `0x13c`, BuildActionName `0x4ce1b0` (original; uses `0x310` and HasActionAni) | seam (state table) | names asked, in order |
| SetDesired `0x208` | original once M1 matches; until then seam | the block (by meaning) and its result |
| Go `0x4ce350`, Stop `0x4cee70`, SideStep `0x4d6220`, Block `0x4d2e30`, SetWalkMode/SetRunMode | seam | argument, on which object |
| FindCharacters | original inside C3/M9; its map iterator `0x44ceb0`/`0x44d080` seamed | ctor args (pos, range, 0xe0, 2, 0, level); the case's ordered objects |
| LOS `0x4533d0`, FindClearPath `0x4c39d0`, FindCharInLine `0x4d4db0`, FindObjectsInRange `0x452060`, GetInstance `0x452690`, WaypointReachable `0x528850`, `0x470bc0` | seam | args; case answers |
| Spell find `0x53f010`, Cast `0x4d5c20` | seam | spell name, target count, source |
| AI_PerMonster `0x4c9b70` | seam (no-op for Araknid; verify) | call |
| TextBar `0x54d390`, script Trigger `0x492640`, SetPlayerState `0x51d680` | seam | args; Trigger returns 0 |
| Net `0x584150`, `0x5840b0`, `0x583e80`, `0x583f60`, `0x57d9d0` | original with `[0x676828] = 0` (each returns at its first test) | — |

### 5.3 Globals

| Address | Meaning | Fixture value |
|---|---|---|
| `0x65caf0` (+frame) | PlayScreen; GameFrame | per case (cadence phases) |
| `0x65d0d0` / `0x65d0c8` | player has control / `+0x5d8` | 1 / 0 |
| `0x66829c`, `0x67682c`, `0x676e5c`, `0x676828` | MP active, MP server, host byte, net session | 0 |
| `0x676838` | NetOwner's result for non-player characters; **AI returns unless ≥ 1** | 1 (no static writer found, §7) |
| `0x668108` | cheat "nahkranoth" | 0 (and 1 as a case) |
| `0x668110` | cheat "dummies" (AI stands down) | 0 |
| `0x668154` | AI paused (written at `0x43d6af`) | 0 |
| `0x5d7a68` | INI AutoCombat (default 1) | 1 |
| `0x65a784` / `0x65a77c` | game type index / table (flag 8 = PvP) | a valid entry, flag 8 clear |
| `0x667fcc` | the player object | the case's player |
| `0x6671a4` | ambient light 0..255 | per case |
| Rules `0x65d7a8` | TOHIT values, STATLEVEL tables | from retail rules.def (COMBAT_HIT §2) |
| `0x667c38` | spell list | only for magic cases |

## 6. Port divergences

Port lines are `src/character.cpp` as of 2026-10-08 (the file is being
edited; search by function name). To-hit / damage / roll / tier /
`Block(−2)` divergences are COMBAT_HIT §6.1 and not repeated.

### 6.1 Attack data (`rules.cpp:293`, `rules.h:98`)

- FATIGUEATTACK is parsed as a plain ATTACK: no `0x10000` flag, column 23
  (fatiguemax `+0xf4`) dropped. Retail makes these usable **only** while
  `Fatigue ≤ fatiguemax`; the port makes them ordinary attacks needing
  `Fatigue ≥ fatigue`.
- MAGICATTACK's condition / value (`+0x64/+0x68`) are dropped; retail
  gates on Health/Mana (MASTAT 2–5).
- swipeframeon/off (`+0xec/+0xf0`) are dropped (VFX).
- The record layout differs (no `+0x20` int, no `+0xec..+0xf4`, impacts
  without the `+0x20` index); only matters for fixtures that build the
  struct in guest memory (§2.1).

### 6.2 Searches (FindButtonAttack `:4318`, FindPcntAttack `:4408`, IsValidAttack `:3954`)

- FindButtonAttack refuses when `!IsFighting()` and uses `Fighting()`'s
  distance; retail has no fighting test (target may be null, tdist 10000)
  and takes the target from the caller (root target, else
  `FindCharacters(facing, 32, 7)`).
- FindPcntAttack doesn't require a root target; retail returns 0 without
  one.
- RandomAttack has no interactive-first path (`+0x28c ≥ 0 && +0x128 & 4`
  → FindInteractiveAttack). FindInteractiveAttack `0x4d1ff0` doesn't
  exist in the port (its comment calls it a "chain-retry helper").
- IsValidAttack order and gates: retail tests HasActionAni first (and
  again later); `+0x128 & 2` PLAYANIM refusal (port TODO); FATIGUEATTACK
  and the "nahkranoth" cheat; PLAYANIM prefix checks only with
  `charflags & 0x10000` (port always); the **`+0x120 != 0` nextattack gate
  for non-players is missing** (port tests `IsScriptWaiting()`, `:4086`);
  magic conditions; the interactive test's `charflags & 0x80000`
  exemption on the target; sunsetflipper/Baez.
- Impact choice: the port picks by `damagemin..damagemax` and never
  refuses; retail ignores the ranges, takes the first impact (a later
  CAI_DEATH one when the damage kills), and **refuses the attack** when
  the target lacks the impact, its built death name, or the loop state,
  or when the snap point is blocked (`CAI_INTERACTIVE` + snapdist).
- One set of out-values per search (damage fixed by the first candidate
  that reaches the damage block); the port recomputes per candidate
  (`damage = 0` sentinel).

### 6.3 ButtonAttack `:4484`, ButtonAction `:4506`, SpecificAttack `:4532`

- No Health / interactive gates in any of them.
- ButtonAttack: no target selection before the chain test (retail's
  FindCharacters runs even when the chain takes the press — a seam call
  and RNG draws); **no same-button rule** (roll 100, 1-in-11 counter,
  `+0x28c/+0x290`); the 1998 dmgpcnt is used as a damage percentage.
- ButtonAction: retail targets anything ahead (`range 0x200, flags 0`);
  both sides never find an action (mask/flag mismatch), so no visible
  effect.
- SpecificAttack: retail clears `nextattack` before validating, uses
  tdist 10000 with no target (port 0, which fails every `mindist > 0`),
  and falls back to FindCharacters for the target.

### 6.4 DoAttack `:4435`

- PLAYANIM: retail ACTION_ATTACK (7); port ACTION_COMBAT.
- Priority = `CA_SPECIAL`; interrupt cleared for a CA_CHAIN attack with a
  chainname; port: interrupt always, no priority.
- Retail copies `doing->angle` into root angle/moveangle and the new
  block's moveangle (and angle when interrupting); after PLAYANIM or
  `charflags & 4` it turns to face the root target. Port: only
  `SetMoveAngle(GetFace())`.
- Missing gates: INVOKE doing, Health, paralysed/iced object flags,
  player state bit 2 clear, `+0x128` bit updates.
- No `tohit` / `roll` in the block (COMBAT_HIT). Retail frees a refused
  block and still returns 1; the port ignores SetDesired's result
  (whether it frees the block there: unverified).
- Magic: retail checks immobile/paralysed/iced and Health again and looks
  the spell up by name; port `CastByName` (equivalent intent, unverified).

### 6.5 BeginFighting `:4878`, EndFighting `:4950`, StartRetreat

- BeginFighting: port marks the seen-memory before any gate (retail
  after); no refusal while running, sneaking or invoking; target check
  is `IsDead || IsInvisibleSpell` (retail Health and `+0x1a4`); no walk
  root for `charflags & 0x4000`; the doing-priority clear only for
  `Is("walk")` (retail: walk root by name, and bow↔combat switches);
  angle = target even when retreating; moveangle = `GetMoveAngle()`
  (retail: this->moveangle only while stepping, else the new angle);
  no `charflags |= 2` from the script trigger; no UI refresh.
- EndFighting: same ANIMATE block, but without the transition flag
  (retail sets priority and transition together when a transition exists);
  no Health / interactive gates; walk root literal `"walk"` (TPlayer's
  can be `"torch"`); moveangle set even with `charflags & 4`.
- StartRetreat `0x4d5fc0` (recon's "BeginCombat") has no port
  counterpart; the port's `BeginCombat` is an alias of BeginFighting.

### 6.6 AI `:2517`

- Gates: no NetOwner, no `charflags & 0x100000` (port returns for every
  player), `NoAI` stands in for "dummies"; port lacks the `[0x668154]`
  pause.
- Target from `desired->obj` (retail `root->obj`) without IsValidTarget;
  acquisition keyed on `FrameCount() ^ charflags` (retail `GameFrame ^
  id`) and only when Aggressive and not `target_out_of_sight` (retail:
  not retreating; not `charflags & 4`).
- **Attack branch**: entered for `doing->action == COMBAT && target &&
  !target_out_of_sight`; retail also needs not retreating, and enters for
  walk-fighters and request bits. Port attacks when `nextattack == 0 ||
  waitticks == 0` without IVA's nextattack gate; no "doing lastattack"
  skip; second decrement only `if nextattack <= 0` (retail unless
  PLAYANIM); magic timer re-armed from ATTACKFREQ (retail MAGICFREQ);
  failed attempts don't clear lastattack/`+0x168`; decrements
  `chainhits` on success (retail `+0x12c`, a different field). Together:
  the double attack per tick (§0).
- **Retreat** missing: `+0x254/+0x258/+0x25c` are mislabelled
  target_out_of_sight / prev / sight_lost_ticks; no flee step
  (`AngleTo + 127`), no RETREATAT / RETREATFOR, no StartRetreat.
- Move branch: port Stops when in range and not out of sight (retail:
  and no committed waypoint); chase writes moveangle and angle (retail
  angle only, waypoint first); hearing doesn't count as contact in the
  port; lost-target sidestep pair with the target's opponent missing;
  waypoint search uses a 250 iterator without the 10-object cap,
  reachability, `0x470bc0`, or the "same as committed → uncommit" rule.
- Stuck side-step: port runs it before everything, also in the combat
  root, and sends **both** sidesteps to itself; retail only in the move
  branch with a non-move doing, and the second sidestep goes to the
  **blocker**.

### 6.7 Target finding (FindCharacters `:2921`, CanSee `:2856`, CanHear `:2838`, Hearing `:3111`, Sight `:3134`, ResetStealthValues `:3149`)

- FindCharacters: skips `IsInvisibleSpell` (retail `+0x1a4`); treats a
  dead enemy as a non-enemy and calls SetHasSeen on it (retail: an
  IsValidTarget failure is skipped silently, and IsValidTarget also
  applies COMBATRANGE max, `charflags & 0x8000` and the control flag);
  hearing adds a line-of-sight test and uses `>=` (retail none, `>`);
  memory window 240 frames (retail 1080).
- CanSee: no invisible-object test; edge/centre `Distance` instead of the
  eye-point distance.
- Hearing / Sight: the 1998 min/max interpolation, no RNG; retail's are
  effectively 100 / {0,1} with a `random(−2,2)` draw each.
- ResetStealthValues: 1998 formula (`random(1,100)`, stealth rules);
  retail `random(1,25)` with base 100/70 and sneak halving.
- IsEnemy: no player-target checks (state bit 2, team, PvP).

### 6.8 Pulse `:219` (chain block `:328`, auto-combat `:363`)

- Chain/autocombo block: matches retail.
- Auto-combat: cadence `GameFrame % 24` (retail `(GameFrame ^ id) &
  31`); runs in run/sneak roots; dead/invisible test instead of
  IsValidTarget; facing cone 32 (retail 48); no player-control test; no
  PvP test.
- AI is called for every character (retail only with `objflags & 0x20`,
  or after Wander for `charflags & 0x100000`).

### 6.9 OnAttacked / SignalAttack `:3181`

Retail ignores attacks on the player; the port puts the player into
combat. For monsters the port always retargets (retail only if the
attacker is under 75% of the current target's distance), draws the block
roll before retargeting, and lacks the repeat-button block, the
FATIGUEATTACK `+0x128 |= 7` response and the interactive gate.

## 7. Open questions

- `[0x676838]` (NetOwner for non-player characters): no direct writer
  found; the AI returns unless it is ≥ 1, so in a running single-player
  game it must be ≥ 1. Read it from a capture.
- `+0x12c` (decremented per successful AI attack), `+0x1a4` (not
  targetable), `+0x168`, and charflags `0x4` / `0x8`: meanings not
  identified.
- `0x470bc0`, called on each closer waypoint candidate: side effect not
  read.
- `0x51e480` in IsEnemy (player-vs-player enable): not read.
- AI_PerMonster `0x4c9b70` and its gate on Aggressive: not read here.
- Other writers of `+0xe8` (`0x4d63b0`, `0x4dbd30`, `0x4de800`) not
  analysed.
- Whether the `0x57d9d0` result is 0 without a session (EndFighting then
  skips `0x57dc70`): assumed, not traced.

## 8. Recon labels corrected

- `0x4d1ff0` is FindInteractiveAttack (only CA_INTERACTIVE candidates),
  not FindButtonAttack; FindButtonAttack is `0x4d1dd0` (labelled
  `TPlayer::meth_0x4d1dd0`).
- `0x4d5fc0` is StartRetreat, not BeginCombat.
- `+0x124` is the magic timer (MAGICFREQ), not waitticks;
  `+0x254/+0x258/+0x25c` are the retreat state; `+0x160` is lastattack
  (field_map.md puts lastattack at `+0x16c`, which is chainhits);
  nextwait is `+0xc4`, chainexptime `+0xc0`.
- `0x4c9790` (slot `0x234`) is TCharacter's Wander, not TPlayer::Pulse.
- The TCharacter vtable is 0x350 bytes (`0x5a7848`–`0x5a7b94`); slots
  `0x2a0`–`0x34c` hold the stat/sense accessors and the resolvers.

## 9. Port status / kata

### 9.1 C3 `melee-attack-choice` (5143 cases)

Ported from the asm (`src/character.cpp`) and held to the original by the
A/B kata (retail `tools/retail_runtime/slots/combat/melee_attack.py`, port
`src/retailab_melee.cpp`, cases `tools/retail_ab/targets_melee.py`):
IsValidAttack `0x4d1120` whole, FindButtonAttack `0x4d1dd0`, FindPcntAttack
`0x4d1eb0`, FindInteractiveAttack `0x4d1ff0`, DoAttack `0x4d2120`,
ButtonAttack `0x4d2480`, ButtonAction `0x4d27f0`, RandomAttack `0x4d2900`,
SpecificAttack `0x4d2a60`, the prefix `0x4cdf60` and CombatAnimName
`0x4ce1b0`, Offense / Defense (`0x4d72e0` / `0x4d72a0`, TPlayer's
`0x51a520` / `0x51a550`), TPlayer's AttackModifier, DefenseModifier,
LuckMod, StrengthMod, WeaponDamage, Resist, HoldsLight; and the AI's
attack branch (`0x4c8cd9`–`0x4c8f5b`: the next-attack and magic timers,
re-armed from ATTACKFREQ / MAGICFREQ). The cases run every attack of the
real char.def tables of 14 pairs (plain, chain / autocombo, interactive
with held and death impacts, fatigue, sneak / walk mode, PLAYANIM,
MAGICATTACK with each condition, death attacks) at every range edge and
fatigue threshold, synthetic records for the shapes the data lacks, and
every random branch by tape. Coverage: 606 of 646 basic blocks; not
reached: multiplayer, a null item, `malloc` failing, the torch prefix (a
light in hand).

Corrections to this document:

- IsValidAttack's 9th argument (`dmgpcnt`) **is** read: damage =
  d·(DamageMod + dmgpcnt + 100)/100 for every attacker; only a player
  replaces it (by StrengthMod) first. (COMBAT_HIT.md §3.1 said "unused".)
- The impact checks in IsValidAttack treat FindState returning **0**
  (state index 0) as "no such animation", as they test the result for
  zero.
- A death impact is taken at once when its raw name exists; the loop
  name is checked only on the prefixed fallback.
- OnAttacked `0x4cdce0` is (attacker, victim, flag).
- ClearChar `0x4c18a0` sets `+0x120` and `+0x124` to 1.

Divergences (none noticeable): IsValidAttack guards a doing block with
no attack; the impact one past a full list (six) reads as empty; a
nameless target is guarded in the Baez (sunsetflipper) rule.

Arena (Locke vs an Araknid, 1440 ticks): 74 attacks after C3 against
1812 before; with C4 and C2, 27 attacks, 3 hits, 2 deaths; deterministic
(two runs, identical traces).

Left: the rest of AI() (C3b), on the movement callees as they land
(StartRetreat, Leap, KnockBack are in; Wander, which only the player AI
reaches, is not).
