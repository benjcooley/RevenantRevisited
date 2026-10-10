# Forensics: melee hit resolution and damage

**Topic:** what happens in the shipped game from the moment an attack is
chosen to the moment the target plays its impact or death: the to-hit roll
and damage tier (chosen at attack time), the impact frame (ResolveAttack),
the per-target resolution (ResolveHit), the damage entry point (Damage)
with its impact / death choice, the impact / block / death resolvers,
block, dodge, knock back, and the fatigue an attack costs.
**Status:** 2026-10-07. Read from the disassembly only (no emulator runs
yet); kata C1/C2/C4/C5 of [../COMBAT_DOJO.md](../COMBAT_DOJO.md) build on it.
**Evidence:** the unchanged retail image through `rdis.py` (every claim
below was read in the asm unless marked *unverified*), Ghidra decompiles
in `recon/classes/cls_0x5a7b98.cpp` (used as a map only; their labels
and argument lists are wrong in several places, §7), the genuine
`rules.def` from `resources.rvr` (8634 bytes, 1999-09-24).

Conventions: `this` is the attacker in ResolveAttack / ResolveHit and the
victim in Damage. Action numbers are the port's `ACTION` enum, which
matches retail (3 COMBAT, 7 ATTACK, 8 BLOCK, 9 DODGE, 0xa MISS, 0xc
IMPACT, 0xd STUN, 0xe KNOCKDOWN, 0xf FLYBACK, 0x13 DEAD, 0x19 BOW).
Action-block flags are retail's (dojo §5.4): firsttime `0x1`, priority
`0x10`, interrupt `0x20`, stop `0x100`, noroot `0x400`, and **`0x800` =
loop** (inferred: the bit after noroot, and the two places it is set are
the ones the port sets `loop`: Block, and a death that isn't a
transition). "Health()" etc. are the virtual stat getters (§6.2).

## 1. Function table

| Retail | Identity | Convention | Confidence, evidence |
|---|---|---|---|
| `0x4c6dd0` | TCharacter::ResolveAttack | thiscall(ab, bits) ret 8 | high: vtable `+0x320` (TCharacter `0x5a7b68`, TPlayer same), ResolveAction dispatches action 7 to it (`0x4c3561`), calls ResolveHit twice, FindCharacters |
| `0x4c62b0` | TCharacter::ResolveHit | thiscall(targ, attack, impact, damage, tohit, roll) ret 0x18 | high: only callers `0x4c6fbb`, `0x4c7025` (ResolveAttack), strings BASEDOUBLE…FULLCOMBATRES, TPlayer exp slots at the end |
| `0x4d1120` | TCharacter::IsValidAttack (damage / to-hit block only, `0x4d16db`–`0x4d1937`, `0x4d1d3f`–`0x4d1da4`) | thiscall(attacknum, &impact, &damage, &tohit, &roll, tdist, button, pcnt, unused, flagmask, flags, target) ret 0x30 | high for the part read; the rest of the function is C3 |
| `0x4d2120` | TCharacter::DoAttack | thiscall(attacknum, impact, damage, tohit, roll, target) ret 0x18 | high: stores args 4/5 at `ab+0x54/+0x58` (Ghidra) and is called with them by RandomAttack `0x4d2a3a`, ButtonAttack `0x4d2732`/`0x4d27c8` (asm) |
| `0x4c4860` | TCharacter::CalculateDamage | thiscall(damage, type, mod) ret 0xc | high: slot `+0x224`; formula in dojo §5.5; reads slots `0x2c8` (Resist) and `0x2bc` (Armor) |
| `0x4c4950` | TCharacter::Damage | thiscall(damage, type, mod, ab, attacker) ret 0x14 | high: slot `+0x228`; strings "dead", " to ", "blockimpact", "impact"; called by ResolveHit `0x4c6ceb` with `(dmg, −1, 0, hitab, this)` |
| `0x4c5810` | TCharacter::FatigueDamage | thiscall(damage, type, mod, unused) ret 0x10 | high: slot `+0x22c`; CalculateDamage then Fatigue −= d. Callers are all in `0x524…–0x528…` (spell effects), none in melee |
| `0x46e970` | TObjectInstance::Damage | thiscall(damage, type) ret 8 | high: slot `+0x48` (TCharacter), only direct call `0x4c4c29` from Damage; ICED/PARALIZE flags (`object.h` OF_ICED `1<<25`, OF_PARALIZE `1<<23`). The Ghidra label "Animate" is wrong |
| `0x5191e0` | TPlayer::Damage (slot `+0x48` override) | thiscall(damage, type) ret 8 | high: TPlayer vtable `0x5b4f78`; body is `Damage(damage, type, 0, 0, 0)` (`0x4c4950`). recon file name "ResolveAttack2" is wrong |
| `0x4c74b0` | TCharacter::ResolveImpact | thiscall(ab, bits) ret 8 | high: slot `+0x338`, ResolveAction sends actions 0xc/0xd/0xe (`0x4c368b`) |
| `0x4c7810` | TCharacter::ResolveDead | thiscall(ab, bits) ret 8 | high: slot `+0x33c`, action 0x13 (`0x4c364f`) |
| `0x4c77a0` | TCharacter::ResolveBlock | thiscall(ab, bits) ret 8 | high: slot `+0x334`, action 8 (`0x4c3595`) |
| `0x4d2e30` | TCharacter::Block | thiscall(frames) ret 4 | high: string "block", action 8, `random(blockmin, blockmax)`; called by UpdateMove `0x47e0b0`, IsValidAttack `0x4d1da4` (`frames = −2`), AI `0x4cdee5` |
| `0x4d30f0` | TCharacter::StopBlock | thiscall() ret | high: only caller UpdateMove `0x47e0e0`; zeroes a BLOCK doing's wait |
| `0x4d3150` | TCharacter::Dodge | thiscall(dir) ret 4 | medium-high: string "crollb", action 9; callers `0x47d90c` (PlayScreen command), `0x582659` (network) |
| `0x4d3750` | TCharacter::KnockBack | thiscall(const S3DPoint *from, variant) ret 8 | high: strings impk/imphh/imph/implh/impl/impb, action 0xc, 20 callers in spells/effects |
| `0x4c85d0` | TCharacter::EffectBurst | thiscall(name, height) | medium: "blood"/"sparks" spawns (decompile + call sites); ret not read |
| `0x4c8500` | TCharacter::EffectCombatFlash | thiscall() | medium: spawns the "combatflash" effect (decompile); callers ResolveImpact, ResolveDead, AI `0x4cb4ea` |
| `0x4d6570` | TCharacter::DropInventory (death loot) | thiscall(?) | medium: strings "pouch", "gold"; callers Pulse `0x4c21e4`, network `0x5825f7`; args not read |
| `0x519850` | TPlayer::ArmorValue (slot `+0x2bc`) | thiscall() | high, already in [PLAYER_STATS.md](PLAYER_STATS.md) §6 |
| `0x5208d0` | TPlayer::Resist(type) (slot `+0x2c8`) | thiscall(type) ret 4 | high: `modstats[type + 6].value` when `type + 6 < count (+0x34c)`, else 0 (pairs of 8 bytes at `+0x350`) |
| `0x4d7320` | TCharacter::Resist(type) (slot `+0x2c8`) | thiscall(type) ret 4 | high: `chardata->damagemods[type]` (`+0xe4 + 4·type`) |
| `0x4d7290` | TCharacter::ArmorValue (slot `+0x2bc`) | thiscall() | high: `chardata+0x1c4` |
| `0x4ce1b0` | TCharacter::CombatAnimName(buf, name) | thiscall(buf, name) ret 8 | high: `buf = prefix + name`; prefix "c" for characters, the player's from slot `+0x310` (`0x4cdf60`) |
| `0x4c47d0` | TCharacter::GetDamageType(weapontype, flags) (slot `+0x2d4`) | thiscall ret 8 | high; same table as the port |
| `0x4833c0` | listrnd(list, buf, len) | cdecl | high: comma count, `rand() % (n+1)` |

Virtual slots used throughout (TCharacter vtable `0x5a7848`; TPlayer
`0x5b4f30` where different):

| Slot | TCharacter | TPlayer | Meaning |
|---|---|---|---|
| `+0x04` | `0x4d61b0` | | Distance (edge to edge, COMBAT_MOVEMENT §5) |
| `+0x1c0/+0x1c4` | `0x4d6f00/10` | | Health / SetHealth (stat `0x66ca4c` "Health") |
| `+0x1c8/+0x1cc` | `0x4d6f30/40` | | Fatigue / SetFatigue (stat `0x66ca3c` "Fatigue") |
| `+0x1d8` | `0x4d7250` | | MaxHealth |
| `+0x1f0` | `0x4d6c20` | | HasActionAni(name, 0) |
| `+0x1f4/+0x1fc/+0x204` | | | root / doing / desired getters (`+0xe0/+0xd8/+0xdc`) |
| `+0x208/+0x218` | | | SetDesired(ab, 0) / ForceCommand(ab, 0, 0) |
| `+0x2b0` | `0x4d7220` | same | DamageMod (obj stat "DamageMod", `0x66ca28`) |
| `+0x2bc` | `0x4d7290` | `0x519850` | ArmorValue |
| `+0x2c0` | `0x4d72a0` | `0x51a550` | **Defense**: character `defensemod (+0x1c8) + Value·TOHITRANGECHAR + Σ spell +0x134`; player `Level·TOHITRANGEPLYR + DefenseModifier (0x51a4e0)` |
| `+0x2c4` | `0x4d72e0` | `0x51a520` | **Offense**: character `attackmod (+0x1cc) + Value·TOHITRANGECHAR + Σ spell +0x138`; player `Level·TOHITRANGEPLYR + AttackModifier (0x51a480)` |
| `+0x2c8` | `0x4d7320` | `0x5208d0` | Resist(type) |
| `+0x2cc` | `0x4d7340` | `0x520810` | WeaponType (character `chardata+0x1bc`; player the equipped weapon at `+0x2b0`, its slot `+0x1f0`) |
| `+0x2d0` | `0x4d7350` | | WeaponDamage (`chardata+0x1c0`) |
| `+0x2ec` | `0x4d7370` (0) | `0x51a580` | Luck modifier: player `StatLevel(5 Luck, stat 0x27)` |
| `+0x2f4` | `0x4d7390` (0) | `0x520900` | Strength modifier: player `StatLevel(0 Strength, stat 0x22)` |
| `+0x3d4` | — | `0x5204e0` | EdgeBonus (stat "EdgeBonus", `0x66d950`) |
| `+0x244/+0x248` | empty (`ret 4`) | `0x518ed0` / `0x518f90` | killed-someone / died notifications |
| `+0x414/+0x41c/+0x420` | — | `0x51a630/0x51abe0/0x51ad80` | kill / weapon-skill / stealth experience (PLAYER_STATS §7) |

## 2. Rules: the TOHIT values

Parsed by the rules loader (`0x48c43d`–`0x48c5d7`, keyword match
case-insensitive, `%i` per value) into the Rules object `0x65d7a8`:

| Tag | Offset | Global | rules.def |
|---|---|---|---|
| TOHITCENTER | `+0x94` | `0x65d83c` | 50 |
| TOHITRANGECHAR | `+0x98` | `0x65d840` | 12 |
| TOHITRANGEPLYR | `+0x9c` | `0x65d844` | 10 |
| TOHITBLOCK | `+0xa0` | `0x65d848` | 25 — **parsed, never read** (no reference in the image) |
| TOHITFACE | `+0xa4` | `0x65d84c` | 25 |
| TOHITDAMAGE | `+0xa8 + 8i` key, `+0xac + 8i` pct, i < 5 | `0x65d850`… `0x65d874` | (40, 50) (0, 0) (−15, −50) (−40, −75) (−50, −90) |

TOHITDAMAGE: `BEGIN`, then up to five `ENTRY key pct` lines (`"%i %i"`);
a line that fails to parse leaves its entry; a missing `ENTRY` ends the
table. The image's statics are zero (BSS), so with no rules.def every
key and percentage is 0.

The table is the damage tier (rules.def: "FinalDamage = (BaseDamage *
(100 + ToHitDamage)) / 100"). `Tier(d)` below means: clamp `d` to
`[T[4].key, T[0].key]`, then the first `i` with `d >= T[i].key`, and the
factor `(T[i].pct + 100)`. The text tiers in ResolveHit use `>` on the
unclamped difference instead (§3.3), so the two disagree at exactly 40, 0,
−15, −40.

The port parses none of these (`rules.cpp:841` skips unknown tags with a
warning).

## 3. Behaviour

### 3.1 Choosing an attack fixes its damage, to-hit and roll (IsValidAttack `0x4d1120`)

The value ResolveAttack later hands to ResolveHit is decided here, when
the attack is selected, and carried in the attack's action block:
`ab->damage` (`+0x50`), `ab+0x54` = **tohit**, `ab+0x58` = **roll**
(DoAttack `0x4d2120` stores its args 3/4/5 there; the TActionBlock ctor
`0x4da9f0` doesn't initialise `+0x54/+0x58`).

The callers start all five out-values at −1 **once per press**, not per
candidate: RandomAttack (`0x4d2969`–`0x4d297f`), ButtonAttack
(`0x4d273f`–`0x4d2771`). Both still draw the 1998 `dmgpcnt =
random(1,50) + random(1,50)` (`0x4d294f`/`0x4d2958`, `0x4d2742`/`0x4d274b`)
and pass it as IsValidAttack's 9th argument, which **is never read**
(the slot is overwritten at `0x4d169d` before any use). ButtonAttack sets
the roll to 100 instead of −1 when the player pressed the same button
three times running on the same target (`+0x28c` / `+0x290`,
`0x4d25d4`–`0x4d2777`); that and its 1-in-11 counterattack path are C3.

With a target (`ebp`), after the attack-shape gates (C3):

```
if (*damage == -1)                                     // 0x4d16ea
    d  = targ->CalculateDamage(WeaponDamage(),
             GetDamageType(WeaponType(), attack->flags), attack->damagemod /*+0xd4*/)
    *damage = d * (DamageMod() + StrengthMod() + 100) / 100          // slots 0x2b0, 0x2f4
    if (player && WeaponType() in {1 knife, 2 sword, 4 axe})
        *damage = *damage * (EdgeBonus() + 100) / 100                // slot 0x3d4
targ->FaceAngleTo(this)                                // result unused
def = targ->Luck() + targ->Defense()                   // slots 0x2ec, 0x2c0
off = this->Luck() + this->Offense()                   // slots 0x2ec, 0x2c4
if (*tohit == -1)
    t = TOHITCENTER - def + off
    if (|targ->FaceAngleTo(this)| >= 0x30 || targ->doing->action == 7 ATTACK)
        t += TOHITFACE                                 // target not facing us, or swinging
    if (player && cheat 0x668108 "nahkranoth") t += 100
    t = clamp(t, 10, 100)
    *tohit = t + (random(0,9) == 0 ? 5 : 0)            // 0x4d1886
if (*roll == -1)
    *roll = random(1,50) + random(0,50)                // 0x4d18b4, 0x4d18bf: 1..100
*damage = max(1, *damage * Tier(*tohit - *roll) / 100) // 0x4d18d2
```

Divisions are signed and truncate (`imul 0x51eb851f; sar 5`). The tier
is applied **every time a candidate reaches this block**, while
CalculateDamage, the to-hit and the roll are computed once: when the
search goes on past a candidate rejected later in IsValidAttack (CA_DEATH
`0x4d1942`, impact selection `0x4d19c0`…), the next candidate's damage is
the previous one's times the tier again. Retail quirk; the fixture should
show it with a two-candidate case.

At the end (`0x4d1d3f`): if `this->charflags & 0x80` and the damage
would kill, the attack is refused unless it is an INTERACTIVE CA_DEATH
attack; then **if `tohit − roll <= T[3].key` (−40) and the target is a
character (class 0xc), the target is told to `Block(−2)`** (`0x4d1da4`)
— a monster raises its guard against a swing that will glance, at the
moment the swing is chosen.

### 3.2 ResolveAttack `0x4c6dd0` (every tick of an ATTACK block)

```
if (this->flags & (OF_ICED|OF_PARALIZE) /*0x2800000*/) return 0
if (this->charflags & 4) return 0
impact = ab->impact; attack = ab->attack; targ = ab->obj
if (ab firsttime):  +0x160 lastattack = attack; +0x168 = 0; +0x164 = GameFrame()
dist = targ ? Distance(targ) : 0            // edge to edge
if (targ) FaceAngleTo(targ)                  // result unused
if (firsttime && targ && attack && !(attack->flags & CA_PLAYANIM))
    targ->SignalAttack(this, ab->obj, attack->flags & CA_MAGICATTACK ? 2 : 0)   // slot 0x240
if (targ):
    (attack == NULL here crashes: 0x4c6ec1 -> 0x4c741d reads attack+0x24)
    if (!(attack->flags & (CA_INTERACTIVE|CA_NOPUSH)) && !(this aniflags & AF_FLY)
        && !(targ aniflags & AF_FLY) && !(impact && impact->snapdist > 0)
        && targ->Health() > 0 && !targ->invisible_spell(+0x1a4)
        && dist <= 10 && frame < attack->impacttime(+0xbc))
        GetNextMove(nm); nm.x = nm.y = 0; SetNextMove(nm)    // 0x470c30 / 0x470bc0
if (frame != attack->impacttime || attack->flags & (CA_PLAYANIM|CA_MAGICATTACK)) goto end
hit = targ ? ResolveHit(targ, attack, impact, ab->damage, ab->tohit, ab->roll) : 0
+0x168 = hit                                  // main target only; read by the AI (0x4cd1a1, 0x4cd3f1)
if (!(attack->flags & CA_ONETARGET))
    n = FindCharacters(buf, 32, attack->hitmaxrange(+0xcc), facing, attack->hitangle(+0xd0), 1 /*ENEMY*/)
    for each c != targ: hit |= ResolveHit(c, same args)
if (!hit):
    if (!(attack->flags & CA_NOMISS)):
        missab = HasActionAni(attack->missname(+0x78)) ? new AB(missname, 0xa MISS) with attack
                                                      : new AB(root->name, 3 COMBAT)
        missab->obj = ab->obj
        doing->flags &= ~priority; missab->flags |= priority
        +0xb4 (movedist) = 0
        ForceCommand(missab, 0, 0)
        PlayWave(listrnd(chardata->misssounds(+0x12c)), 0, 0x7f, -1)    // both cases
    if (targ && targ->doing->action == 8 BLOCK && Distance(targ) <= attack->hitmaxrange):
        if (WeaponType() && targ->WeaponType()) EffectBurst("sparks", 0x32)
        r = root
        if (r && (r->action == 3 || r->action == 0x19) && r->obj):
            a = WeaponType(); d = r->obj->WeaponType()
            name = "block" + itoa(random(1,2))                           // 0x4c723f
            name += (a in {1,2,4}) ? (d != 0 ? "sword" : "dull")
                  : (a in {3,5,6,7}) ? (d in {1,2,4} ? "sword" : "dull") : "dull"
            id = Sounds(0x667548).Find(name); if (id >= 0 && Sounds.Ok(id))
                Sounds.Play(id, 0x7f, 1, &pos, 0x50, 0x2bc)
    f = (attack->flags & CA_INTERACTIVE) ? attack->fatigue/4 : attack->fatigue
else:
    f = attack->fatigue                                   // +0xd8
SetFatigue(max(0, Fatigue() - f))                          // 0x4c73a1 / 0x4c73f2
end:
if (attack->flags & CA_PLAYANIM && root is COMBAT/BOW with obj) facing = AngleTo(root->obj)
return 0
```

**Fatigue is paid here, at the impact frame**, in full on a hit or a
normal miss, a quarter on a missed interactive attack, never below 0
(`fatigue/4` is signed division). Nothing is subtracted when the attack
never reaches its impact frame, or for PLAYANIM / MAGICATTACK attacks.
(IsValidAttack only checks `Fatigue() >= attack->fatigue`, or for flag
`0x10000` attacks `Fatigue() <= attack+0xf4`.)

### 3.3 ResolveHit `0x4c62b0` (one target)

No RNG. The roll is the one fixed at attack time.

```
if (!targ) return 0
if (!IsEnemy(targ)) return 0                                // 0x4c89c0
dist = Distance(targ); ang = FaceAngleTo(targ)
if (dist < attack->hitminrange(+0xc8) || dist > attack->hitmaxrange(+0xcc)) return 0
if (|targ->pos.z - pos.z| > 40) return 0
if (|ang| > attack->hitangle(+0xd0)) return 0
if (!IsValidTarget(targ)) return 0                          // 0x4cd990 (COMBAT_MOVEMENT §3)
if (!(targ->charflags & 0x80000)):                          // target held in an interactive move?
    if (targ->doing->attack is INTERACTIVE || targ->doing->impact->flags & 0x80):
        if (targ's COMBAT/BOW root target != this) return 0
targ->SetHasSeen(this)                                      // 0x4c5940
targ->vel += ConvertToVector(facing, 0x40000)               // 4·ROLLOVER
hit = tohit >= roll; dmg = damage; blocked = 0
ta = targ->FaceAngleTo(this)
if (!(impact && impact->flags & CAI_DEATH) && |ta| < 0x30
    && targ->doing->action in {8 BLOCK, 9 DODGE}):         // blocking / dodging, facing us
    if (dmg > 0) dmg = dmg * 100 / Tier(tohit - roll)       // undo the tier
    tohit = clamp(tohit - 50, 10, 100)                      // the hard-coded block penalty
    dmg = max(1, Tier(tohit - roll) * dmg / 100)            // redo at the new margin
    hit = tohit >= roll; if (!hit) blocked = 1
if (attack->flags & CA_INTERACTIVE && (targ->accum.x(+0x28) || targ->accum.y(+0x2c))
    && targ->doing->action != 8) hit = 1                    // +0x28/+0x2c: accum, medium confidence
def = targ->Luck() + targ->Defense(); off = Luck() + Offense()     // for the text only
dmg = max(1, dmg); hitab = NULL
if (hit && attack is INTERACTIVE && targ->Health() - dmg < 1
    && !(impact && impact->flags & CAI_DEATH)) hit = 0     // an interactive can't kill without a death impact
if (hit):  text = Tier text of (tohit - roll)              // see below
           BuildHitBlock(beginfight = true)
else:      if (attack is INTERACTIVE) return 0             // no damage, no text
           text = "BASEGLANCE"
           BuildHitBlock(beginfight = false)
```

BuildHitBlock:

```
if (targ->Health() - dmg < 1):                              // lethal
    if (impact && impact->flags & CAI_DEATH && targ->HasActionAni(impact->name)):
        hitab = new AB(impact->name, 0x13 DEAD)            // raw name, no prefix
        hitab->flags |= priority; ->attack = attack; ->obj = this; ->impact = NULL (!)
        hitab->damage = dmg; targ->SetFighting(this)        // 0x4d4790
    targ->+0x224 = 5                                        // combat flash ticks, even with no block
else:
    if (beginfight && !(targ's root is COMBAT/BOW with a target) && this->+0xe8 == 0
        && dist <= targ->chardata->combatrangemin(+0x158))
        targ->BeginFighting(this, 3)                        // 0x4d3b90
    if (impact):
        name = (impact->flags & 0x84) ? impact->name : targ->CombatAnimName(impact->name)
        if (targ->HasActionAni(name)):
            targ->doing->flags &= ~(priority|interrupt)
            a = flags&1 ? 0xd STUN : 0xc | (flags&2)        // KNOCKDOWN 0xe
            hitab = new AB(name, a); flags |= priority|interrupt
            hitab->attack = attack; ->impact = impact; ->wait = impact->looptime(+0x48)
            hitab->obj = this; ->damage = dmg; targ->SetFighting(this)
```

Hit text tiers (`0x4c6970`): `attack->flags & 0x10000` → "BASEFATIGUE";
else on `d = tohit − roll`: `d > 40` "BASEDOUBLE", `> 0` "BASEHIT",
`> −15` "BASEMINOR", `> −40` "BASESMALL", else "BASEGLANCE" (keys read
from T[0..3]). On the hit path `d >= 0` unless forced, so in practice
DOUBLE, HIT, or MINOR at exactly 0.

Then:

```
if (!blocked || (attack is INTERACTIVE && hit)):
    if ((this == Player || targ == Player) && !NoCombatResults(0x668194)):
        if (Text.Has("FULLCOMBATRES"))
            line = sprintf(Text("FULLCOMBATRES"), Text(tag), targ->name(+0x38), dmg, def, off)
        else
            line = sprintf("%s %s %s:%d %s:%d %s:%d", Text(tag), targ->GetName(),
                           Text("BASEDMG"), dmg, Text("BASEDEF"), def, Text("BASEOFF"), off)
        TextBar(0x65c5d0).Add(line)                         // 0x54d170
if (hit || !(targ is player && targ->doing->action == 8 BLOCK))
    targ->Damage(dmg, -1, 0, hitab, this)                   // slot 0x228
if (hitab && hitab not in {targ->root, doing, desired}) free hitab
if (targ is player && targ->root->action == 0x19 BOW) targ->BeginFighting(NULL, 3)
if (this is player && (!net 0x676828 || 0x67682c)):
    KillExp(targ) (0x414); SkillExp(WeaponType()+3, targ) (0x41c); StealthExp(targ) (0x420)
return hit
```

So a failed roll is a **glance, not a miss**: the target still takes
`dmg` (at least 1; the tier has already cut it by 50–90%), plays the
impact for it, and can die of it. Only a **blocking player** takes no
damage from a failed roll. ResolveHit returns 0 for a glance, so the
attacker plays its miss (§3.2) while the target flinches.

`NoCombatResults` is the `Revenant.ini` option (`0x484dbd`, also the
Options screen `0x53af62`); `0x667fcc` is the Player pointer. Text is the
dialog-text table at `0x65d4d0` (`0x49d6d0` index, `0x49d800` get).

### 3.4 Damage `0x4c4950` (slot `+0x228`)

```
oldhealth = Health()                                         // stored, never used
if (attacker && attacker->flags & (OF_ICED|OF_PARALIZE)) return
if (cheat 0x668104 "alreadydead" && this is player) return
if (this->flags & 0x10000000) return
if (!multiplayer && cheat 0x668108 && attacker is player) damage = 100000
if (attacker && !IsEnemy(attacker)) return
if (this is player && player state +0x36c & 2) return
[multiplayer gates on 0x66829c/0x676828/0x67682c/0x676e5c: all 0 in single player]
if (type >= 0):
    damage = CalculateDamage(damage, type, mod)
    if (this is player && WeaponType() in {1,2,4}) damage = damage * (EdgeBonus()+100)/100
        // the victim's EdgeBonus: looks like a copy of IsValidAttack's attacker line
if (charflags & 8 && damage >= Health() - 1) damage = Health() - 1          // can't die
if (slot 0x24()):                                            // floating damage number
    pt = screen pos (0x46eb40), pt.y -= 135
    p = max(0, Health() - damage) * 100 / MaxHealth()
    colour = p < 25 ? red : p < 50 ? yellow : white
    slot0x24()->Show(-min(Health(), damage), &pt, colour)    // 0x4da3b0
if ((charflags & 0x80000 || !(doing->attack INTERACTIVE || doing->impact & 0x80)) && damage)
    TObjectInstance::Damage(damage, 0)                       // 0x46e970: held victims take nothing
dir = 0; if (root is COMBAT/BOW with obj):
    a = AngleTo(root->obj)        // ABSOLUTE bearing 0..255 (0x46ea90), not relative to facing
    dir = a in [0x20,0x60) ? 0x400 : a in [0x60,0xa0) ? 0x200 : a in [0xa0,0xe0) ? 0x800 : 0
if (Health() <= 0 && !(charflags & 0x40000)):
    charflags |= 0x40000; Died(attacker) (slot 0x248); if (attacker) attacker->Killed(this) (0x244)
if (held as above) goto tail                                 // no impact or death block
if (Health() <= 0) Death() else Impact()
tail:
snapimp = the impact chosen below
if (snapimp && snapimp->snapdist(+0x54) > 0 && attacker):
    +0x220 = snapimp->snaptime(+0x58)
    facing = moveangle = (attacker->facing - 0x80) & 0xff
    GetSnapPos(attacker, snapdist, &p) (0x46f010); MoveTo(&p) (slot 0xc)
[network send 0x584b30 when 0x67682c]
if (1 <= Health() <= chardata+0x440) +0x25c = chardata+0x448     // unidentified low-health timer
```

`pct(d)` below is `d·100 / max(1, max(Health(), d))` with **Health()
after the damage**: the share of what's left, capped at 100. The
char.def CHARIMPACT `mindmgpcnt` / `maxdmgpcnt` (`+0x4c/+0x50` of a
0x5c-byte impact; `chardata+0x214` count, `+0x218` array) are compared
against it.

Death():

```
ab = given ab; dimp = NULL
if (ab && !(ab->impact && !(ab->impact->flags & CAI_DEATH)
            && !(ab->attack->flags & CA_INTERACTIVE))):       // a death block, or interactive
    dimp = ab->impact
    if (!dimp && ab->attack): dimp = the attack's impact whose name == ab->name
        // (+0xe8 count, +0xf8 array) -- or one past the last when none matches (retail quirk)
else:
    ab = NULL
    for imp in chardata impacts:
        name = HasActionAni(imp->name) ? imp->name : CombatAnimName(imp->name)
        if ((imp->flags & dir) == dir && imp->flags & CAI_DEATH
            && (!(flags & CAI_WHENSTUNNED) || doing->action == 0xd)
            && (!(flags & CAI_WHENDOWN) || doing->action == 0xe)
            && HasActionAni(name) && (!imp->loopname[0](+0x28) || HasActionAni(loopname))
            && imp->damagemin <= pct(damage) <= imp->damagemax) break
    dimp = imp   // the match, or &impacts[count] when none (a zeroed slot; retail quirk)
    if (none matched) name = first that exists of:
        CombatAnimName("dead"), root->name + " to " + CombatAnimName("dead"),
        root->name + " to dead", "dead"; none -> no death block at all
    if (name) ab = new AB(name, 0x13); ab->impact = dimp
if (attacker) facing = moveangle = AngleTo(attacker)
if (ab):
    ab->damage = damage; ab->obj = doing->obj; flags |= priority|interrupt
    if (!strstr(ab->name, " to ")) flags |= 0x800 (loop)
    ab->angle = ab->moveangle = doing->angle
    if (attacker && attacker->doing) ab->attack = attacker->doing->attack
    if (doing->obj && dimp && dimp->snapdist == 0 && !(dimp->flags & 0x100)):
        a = AngleTo(doing->obj) + (flags&0x200 ? 0x80 : flags&0x400 ? 0x40 : flags&0x800 ? -0x40 : 0)
        ab->angle = ab->moveangle = a & 0xff
    doing, desired: clear priority; ForceCommand(root); ForceCommand(ab)
```

Impact():

```
ab = given ab; imp = NULL
if (doing->action == 8 BLOCK):
    if (!(ab && ab->impact && (ab->attack INTERACTIVE || ab->impact->flags & 0x80))):
        name = CombatAnimName("blockimpact") if it exists, else "blockimpact" if it exists
        if (name): free the given ab; ab = new AB(name, 0xf); flags |= priority
                   ab->impact = NULL; ab->angle = facing
else if (ab && !(ab->impact && ab->impact->flags & CAI_DEATH)):
    imp = ab->impact                                         // use the caller's block
else:
    name = "impact"; act = 0xc
    for imp in chardata impacts:                             // raw names, no prefix
        if (damagemin <= pct(damage) <= damagemax && (flags & dir) == dir && !(flags & CAI_DEATH)
            && WHENSTUNNED/WHENDOWN as above && HasActionAni(imp->name)
            && (!loopname[0] || HasActionAni(loopname))):
            name = imp->name; act = flags&1 ? 0xd : flags&2 ? 0xe : 0xc; break
    if (none) imp = NULL
    if (imp || HasActionAni("impact")):
        free the given ab; ab = new AB(name, act); ab->impact = imp
        if (imp) ab->wait = imp->looptime
    // else: keeps the given ab (one carrying a CAI_DEATH impact), or none
if (ab):
    ab->damage = damage; ab->obj = doing->obj; flags |= interrupt
    if (attacker && attacker->doing) ab->attack = attacker->doing->attack
    direction angle exactly as in Death() when imp qualifies
    doing, desired: clear priority; ForceCommand(root); ForceCommand(ab)
    facing = moveangle = ab->angle          // 0 when nothing set it (no impact data): ctor default
```

Damage frees the block ResolveHit passed in when it replaces it
(blockimpact, a non-lethal hit carrying a CAI_DEATH impact), and
ResolveHit then frees the same pointer unless it is now root / doing /
desired. Retail survives because `malloc(100)` right after `free` hands
back the same block, which ForceCommand has just made desired. The port
must model "Damage replaced the hit block" without the double free.

CAI flags seen in retail beyond the port's list: `0x80` (interactive
impact: the victim is held), `0x100` (keep the current angle), `0x200`
(turn 180), `0x400` (+64), `0x800` (−64); `0x400/0x200/0x800` are also
the direction filter.

### 3.5 TObjectInstance::Damage `0x46e970`, TPlayer::Damage `0x5191e0`, FatigueDamage `0x4c5810`

```
TObjectInstance::Damage(damage, type):
    if (flags & OF_ICED && type != 13 /*DAMAGE_ICE*/) SetFlags(flags & ~OF_ICED)     // slot 0x40
    if (flags & OF_PARALIZE && (!multiplayer || 0x67682c) && random(0,3) == 0)       // 0x46e9b1
        SetFlags(flags & ~OF_PARALIZE)
    SetHealth(damage < Health() ? Health() - damage : 0)
TPlayer::Damage(damage, type) = Damage(damage, type, 0, NULL, NULL)       // the full 0x4c4950
FatigueDamage(damage, type, mod, -):
    d = type >= 0 ? CalculateDamage(damage, type, mod) : damage
    SetFatigue(Fatigue() > d ? Fatigue() - d : 0)
```

For characters slot `+0x48` stays TObjectInstance::Damage (health only,
no impact); only the player's goes through the full Damage.

### 3.6 CalculateDamage `0x4c4860`

As dojo §5.5. Callers: IsValidAttack `0x4d1726` (on the target, at
attack time), Damage `0x4c4a67` (when `type >= 0`; melee passes −1 so
melee damage is never recalculated), FatigueDamage `0x4c582b`.

### 3.7 ResolveImpact `0x4c74b0`

```
ratio = (float)ab->damage / chardata->health(+0x1e8)
if (ab->Is("impale") && random(0,5) == 1                    // drawn only for "impale"
    && !(chardata+0xc8 & 0x10) && chardata+0x164)
    EffectBurst("blood", ab->Is("impale") ? 40 : 50)
if (firsttime && !(chardata+0xc8 & 0x10) && chardata+0x164 && ratio > 0.01f
    && (!ab->attack || ab->attack->flags & CA_BLOOD))
    EffectBurst("blood", 40 or 50 as above)
if (firsttime) EffectCombatFlash(); +0x224 = 5
if (commanddone(+0x80)):
    if (ab->wait <= 0):
        doing &= ~priority; root |= interrupt; SetDesired(root); net 0x583de0(this, 0)
    else if (ab->impact && ab->Is(impact->name) && impact->loopname[0]
             && FindState(loopname, -1) >= 0):
        n = new AB(*ab, loopname, ab->action); flags |= priority|interrupt
        desired, doing: clear priority
        if (!(lastattack(+0x160)->flags & CA_PLAYANIM)            // no null check on +0x160
            && stricmp(lastattack->name, doing->name) != 0) SetDesired(n) (free if refused)
        else ForceCommand(n)
if (commanddone && ab->wait <= 0):
    n = new AB(root->name, 3); n->flags |= interrupt; SetDesired(n) (free if refused)
return 0
```

### 3.8 ResolveDead `0x4c7810`, ResolveBlock `0x4c77a0`

```
ResolveDead:
    if (firsttime && (!ab->attack || ab->attack->flags & CA_BLOOD)
        && chardata+0x164 && (chardata+0xc8 & 0x10))       // note: 0x10 SET here, CLEAR in ResolveImpact
        EffectBurst("blood", 40 or 50)
    if (firsttime) EffectCombatFlash(); +0x224 = 5
    if (commanddone && ab->impact && loopname[0] && !ab->Is(loopname) && FindState(loopname) >= 0):
        n = new AB(*ab, loopname, ab->action); flags |= priority|interrupt
        desired, doing: clear priority; SetDesired(n); SetRoot(n)
    return 2
ResolveBlock:
    if (doing && doing->obj) facing = moveangle = AngleTo(doing->obj)     // face the attacker
    if (ab->wait > 0 && !(doing && doing->flags & stop)) return 2
    ab->wait = 0; SetDesired(NULL); return 0
```

A death built by ResolveHit has `impact = NULL` (§3.3), so it never
moves on to a loop state; one built by Damage's own search does.

### 3.9 Block `0x4d2e30`, StopBlock `0x4d30f0`

```
Block(frames):
    if (!(root is 3 COMBAT or 0x19 BOW)) return 0
    if (doing->action == 0xc IMPACT) return 0            // can't block while flinching
    if (Health() < 1) return 0; refuse while held (charflags/0x80 rule); multiplayer gates
    if (root has a target && Distance(target) > 120) return 0
    t = doing->obj
    if (!player):
        if (!t) return 0
        if (!(t->doing->action == 7 && t->frame < t->doing->attack->blocktime(+0xb8))
            && frames != -2) return 0                    // -2: IsValidAttack's forced block
    name = CombatAnimName("block"); if (!HasActionAni(name)) return 0
    ab = new AB(name, 8); ab->obj = doing->obj
    ab->wait = frames < 0 ? random(chardata->blockmin(+0x198), blockmax(+0x19c)) : frames
    ab->flags = (flags & ~priority) | interrupt | 0x800 (loop)
    net 0x583f60(this, 0x43, wait, 1); SetDesired(ab) (free if refused)
    if (!(charflags & 4)) moveangle = facing
    return 1
StopBlock(): if (root is COMBAT/BOW && doing->action == 8) { doing->wait = 0; return 1 } return 0
```

The attack's `blockname` isn't used. The player's block needs no
attacker timing.

### 3.10 Dodge `0x4d3150`

`Dodge(dir)`, `dir` 0–7 (the held direction): needs root COMBAT/BOW,
doing action 3, Health ≥ 1, not held, multiplayer gates; a player with
state bit 2 gets it cleared. Name from the template `"crollb"`:
`[0] = 'h'` when the root is `"hand"`, `[5]` one of `l r f b` chosen by
two lookups: the facing, rotated by the camera angle when `0x6671f0` is
set (`0x60 − ftol(0x667208 · k)`), falls in one of eight sectors
(`0x10–0x30`, `0x31–0x50`, … `0xd3–0xf2`, else), and each sector has an
8-entry jump table on `dir` (`0x4d3474`… `0x4d3570`; *not transcribed*).
If `HasActionAni(name)`: `new AB(name, 9 DODGE)`, `obj = doing->obj`,
priority set, interrupt cleared, SetDesired. Network notify first.

### 3.11 KnockBack `0x4d3750`

`KnockBack(const S3DPoint *from, variant)`: player state bit 2 → 0;
multiplayer gates; `flags & OF_PARALIZE` → return 1; `+0x224 = 5`;
`a = AngleToPP(pos, *from)`, `d = |facing − a|` (larger minus smaller,
no wrap). If `d > 72` and the from-behind name (prefix + one of
`impk imphh imph implh impl impb`, built on the stack at
`0x4d3790`–`0x4d3860`; *which one is not transcribed, likely "impb"*)
exists, play it without turning; else `variant` (or `random(0,4)` when
outside 0–4, `0x4d3912`) picks one of the other five, prefixed, and
`facing = moveangle = a`. `HasActionAni` or return 0;
`new AB(name, 0xc)`, priority, ForceCommand; network notify; return 1.

### 3.12 EffectBurst `0x4c85d0`, EffectCombatFlash `0x4c8500`, death loot `0x4d6570`

From the decompiles (not re-read in asm): EffectBurst(name, height)
returns at once for "blood" while burning (`+0x1b8`); "blood" spawns the
named effect at `pos + (0,0,height)` and calls its slot `0x200(height,
facing+0x80, 3, 5, 5, random(1,5))`; "sparks" spawns it between the
character and `doing->obj` with `random(−80,80)` on the direction and
`random(15,25)` on the particle count. EffectCombatFlash spawns
"combatflash" at `pos + (30,30,80)` and starts it on frame
`min(2·random(0, frames/2), frames − 2)`. The death loot (`0x4d6570`,
from Pulse `0x4c21e4`) plays the "gold" sound and scatters the
inventory: gold at `pos + (30,30)`, every other item along
`random(0,256)` until the walk map accepts it; in multiplayer the
player's items go into a "pouch".

## 4. RNG draws

All through `random(lo,hi)` `0x483300` unless noted. In order of a melee
exchange:

| # | Site | Range | Decides | When drawn |
|---|---|---|---|---|
| 1 | ButtonAttack `0x4d2742`, `0x4d274b` / RandomAttack `0x4d294f`, `0x4d2958` | (1,50) ×2 | 1998 dmgpcnt, **unused** | every press / AI attack try |
| 2 | ButtonAttack `0x4d261c` (`0,10`), `0x4d2630` (`1,50`) | | repeat-press counter (C3) | third same-button press |
| 3 | FindPcntAttack `0x4d1f49` | (0, n−1) | candidate attack | per try, up to 2n (C3) |
| 4 | IsValidAttack `0x4d1886` | (0,9) | +5 to-hit when 0 | first candidate reaching the damage block with a target |
| 5 | IsValidAttack `0x4d18b4`, `0x4d18bf` | (1,50), (0,50) | the roll | same, after #4 |
| 6 | Block `0x4d3049` | (blockmin, blockmax) | block duration | Block with frames < 0, incl. IsValidAttack's `Block(−2)` |
| 7 | TObjectInstance::Damage `0x46e9b1` | (0,3) | 0 breaks paralysis | victim has OF_PARALIZE |
| 8 | listrnd `0x4833ee` (CRT `rand()` direct, `% (commas+1)`) via ResolveAttack `0x4c714b` | (0, commas) | miss sound | miss with no CA_NOMISS; no draw when the list has no comma |
| 9 | EffectBurst("sparks") | (−80,80), (15,25) | spark spray | miss against a blocker, both armed, effect spawned |
| 10 | ResolveAttack `0x4c723f` | (1,2) | block sound 1 or 2 | miss against a blocker, attacker root COMBAT/BOW with target |
| 11 | ResolveImpact `0x4c74f5` | (0,5) | impale blood when 1 | each tick of an "impale" impact |
| 12 | EffectBurst("blood") | (1,5) | blood | when spawned (#11, impact firsttime, death firsttime) |
| 13 | EffectCombatFlash | (0, frames/2) | flash start frame | impact / death firsttime, when spawned |
| 14 | KnockBack `0x4d3912` | (0,4) | variant | variant outside 0–4 |
| 15 | death loot | (0,256) per non-gold item | drop direction | death (Pulse) |

ResolveHit, Damage (apart from #7), CalculateDamage, ResolveBlock,
ResolveDead (apart from #12/#13) and StopBlock draw nothing. Global
reseeding (dojo §3) happens when an effect object gets an id, so #9,
#12, #13 also reseed in a live game; fixtures pin `GetTickCount`.

## 5. Fixture plan

### 5.1 Katas

- **C4a, attack-time numbers:** run the whole IsValidAttack on a
  one-attack chardata whose C3 gates pass (answered by the case), with
  the out-values at −1 and also pre-set. Compared: damage, tohit, roll,
  draws #4–#5, the `Block(−2)` seam call. A two-candidate case through
  FindPcntAttack shows the tier compounding.
- **C4b, ResolveAttack + ResolveHit** with Damage as a seam (records
  `(dmg, type, mod, hitab meaning, attacker)`): impact-frame gate,
  multi-target, glance vs hit, block/dodge re-tier, interactive rules,
  miss block, sounds, fatigue, the text line, exp calls.
- **C2, Damage** run as original with ForceCommand original (M1 core):
  impact/death choice over a case's impact table, direction flags,
  name fallbacks, snap, the floating number seam.
- **C5, resolvers:** ResolveImpact / ResolveDead / ResolveBlock / Block /
  StopBlock / Dodge / KnockBack each alone.

### 5.2 Callees

Original (pure or only touching the objects in the case): FaceAngleTo
`0x46ead0`, AngleTo `0x46ea90`, AngleToPP `0x46dc60`, ConvertToVector
`0x46db20`, Distance (slot 4, needs Radius: existing type-stat seam),
CalculateDamage, GetDamageType, CombatAnimName `0x4ce1b0` and the
player prefix `0x4cdf60` (read root names; call HasActionAni), the
TActionBlock ctors `0x4da9f0` / `0x4daae0`, malloc/free (`0x482fb0`,
`0x482f80`, `0x4830f0`), `strstr`/`stricmp`/`sprintf`/`_itoa`, listrnd,
SetDesired / ForceCommand / SetRoot (M1), TObjectInstance::Damage (with
SetFlags slot `0x40` recorded), the stat getters through the existing
GetObjStat / GetStat seams, ResolveHit's TOHITDAMAGE lookups.

Seams (record the arguments):

| Seam | Retail | Record |
|---|---|---|
| state exists | HasActionAni slot `0x1f0`, FindState slot `0x138` | name |
| enemy test | IsEnemy `0x4c89c0` | other |
| valid target | IsValidTarget `0x4cd990` (or run it with the M5 seams) | target |
| world | FindCharacters `0x4cd690` | range, angle, arc, flags |
| combat stats | Luck / Defense / Offense / Armor / Resist / WeaponType / WeaponDamage / DamageMod / StrengthMod / EdgeBonus (slots in §1) | which, value |
| has-seen, fighting | SetHasSeen `0x4c5940`, SetFighting `0x4d4790`, BeginFighting `0x4d3b90`, SignalAttack slot `0x240` | args |
| movement | GetNextMove / SetNextMove `0x470c30` / `0x470bc0`, GetSnapPos `0x46f010`, MoveTo slot `0xc` | points |
| sound | PlayWave `0x473990`, Sounds `0x49c430` / `0x49b650` / `0x49b990` | name, volume |
| effects | EffectBurst, EffectCombatFlash (and the RNG they draw, §4) | name, height |
| UI | floating number (slot `0x24`, `0x46eb40`, `0x4da3b0`), text table `0x49d6d0` / `0x49d800`, text bar `0x54d170` | amount/colour, line |
| notifications | slots `0x244` / `0x248`, exp slots `0x414` / `0x41c` / `0x420` | args |
| network | `0x583de0`, `0x583f60`, `0x584b30`, `0x5874f0`, `0x587570` | must not be called in single player |
| Block from IsValidAttack | Block `0x4d2e30` | frames (−2) |

### 5.3 Globals

| Global | Meaning | Realistic value |
|---|---|---|
| `0x65d83c`–`0x65d874` | TOHIT rules (§2) | 50, 12, 10, 25, 25; table from rules.def |
| `0x65caf0` | PlayScreen (GameFrame) | case |
| `0x667fcc` | Player | the case's player or NULL |
| `0x668104` | cheat "alreadydead" (player immune) | 0 |
| `0x668108` | cheat "nahkranoth" (player 100000 damage, +100 to-hit, no 0x10000 attacks) | 0 |
| `0x668194` | `NoCombatResults` | 0 (run 1 too) |
| `0x66829c`, `0x676828`, `0x67682c`, `0x676e5c` | multiplayer / host / net flags | 0 |
| `0x6671f0`, `0x667208` | camera rotation (Dodge) | case |
| `0x667548`, `0x65d4d0`, `0x65c5d0` | sound manager, text table, text bar | seams |

## 6. Port divergences

Port: `src/character.cpp` (as of `a604a52`; line numbers drift).

### 6.1 Attack-time numbers (IsValidAttack `character.cpp:3798`, ButtonAttack `:4328`, RandomAttack `:4362`)

- Damage is `CalculateDamage(...) · dmgpcnt / 100` with `dmgpcnt =
  random(1,50)+random(1,50)` (`:4034`, `:4340`…). Retail ignores dmgpcnt
  and applies `(100 + DamageMod + StrengthMod)/100`, the player's
  EdgeBonus for knife/sword/axe, and the TOHITDAMAGE tier.
- No tohit / roll: the port draws no `random(0,9)`, `random(1,50)`,
  `random(0,50)`; TActionBlock has no `+0x54/+0x58` (the port's block is
  `data`/`flags` there: dojo §6.2 layout differs at these two fields).
- The tier compounding across candidates and the 0x80 charflag refusal
  are missing; so is `Block(−2)` on a glancing swing.
- The port's "only when damage == 0" sentinel differs from retail's −1.

### 6.2 ResolveAttack (`:1734`)

- No ICED/PARALIZE or `charflags & 4` early-out.
- Firsttime doesn't zero `+0x168`; nothing stores the hit flag.
- SignalAttack passes no type, and is sent for PLAYANIM attacks too.
- The before-impact freeze uses `dist <= Radius + targ->Radius + 10`
  (the same test once Distance is edge to edge); the port also has an
  **after-impact branch that turns and pushes the target** (`:1793`)
  that retail doesn't have.
- `hit` is uninitialised without a main target (`:1800`).
- Miss: the port uses SetDesired with interrupt; retail ForceCommand
  with priority (doing's priority cleared) and `movedist = 0`. The port
  plays the miss sound only when there is no miss animation (`:1838`);
  retail always. The port's block reaction keys sparks on `CA_SPARKS`
  and plays `listrnd(blocksounds)`; retail: sparks when both are armed,
  within hitmaxrange, sound `block{1|2}{sword|dull}` from the two weapon
  types.
- **Fatigue is never subtracted** (`:1854`); retail subtracts
  `attack->fatigue` (¼ on a missed interactive) at the impact frame.
- No PLAYANIM re-facing at the end.
- PLAYANIM/MAGICATTACK attacks are not excluded from the impact frame.

### 6.3 ResolveHit (`:1586`)

- The 1998 to-hit (`Armor + Defense + FatigueModifier − Attack`, 25-sided
  roll drawn here, `:1632`) instead of the attack-time tohit/roll; no
  RNG belongs here.
- A failed roll does 0 damage; retail deals the (tier-reduced, ≥ 1)
  damage as a glance except to a blocking player.
- Blocking/dodging: the port shifts the hit chance by ¾; retail takes 50
  off the to-hit and re-tiers the damage, and skips this for CAI_DEATH
  impacts.
- Missing: IsEnemy and IsValidTarget gates (the port checks IsDead and
  invisibility), the interactive-target rule, the forced hit for moving
  interactive targets, the interactive-can't-kill rule, the glance path,
  the text-bar line, `+0x224 = 5` on a lethal hit, the bow-mode player's
  BeginFighting, the multiplayer exp gate.
- Lethal: the port's death block keeps `impact`; retail sets it NULL.
- Non-lethal: the port uses the raw impact name; retail prefixes it
  (`CombatAnimName`) unless the impact has flags `0x84`; the port
  doesn't clear the target's doing priority/interrupt.
- Engaging: the port calls `targ->BeginCombat(this)` when the target
  isn't fighting and `!GetAutoCombat()`; retail `BeginFighting(this, 3)`
  when the target's root has no target and `this+0xe8 == 0`, and only on
  a real hit.
- The port doesn't free an unused hit block (leak); retail does.

### 6.4 Damage (`:1010`), TObjectInstance::Damage (`object.cpp:891`), TPlayer::Damage (`player.cpp:726`)

- Missing gates: frozen attacker, cheats, object flag `0x10000000`,
  IsEnemy, player state bit 2; the immortal flag (`charflags & 8`); the
  floating damage number; the held-victim exemption; the death
  notifications and `charflags 0x40000`; the low-health `+0x25c` timer.
- Impact / death choice: the port compares **raw damage** with
  `damagemin/damagemax`; retail compares `pct` of the remaining health.
  The port has no direction filter (`0x200/0x400/0x800`), no
  CombatAnimName fallback, no death-name chain (only "dead"), no
  blockimpact for a blocking victim, keeps a given impact block in cases
  where retail replaces it, and sets `loop`/`priority` on every death
  (retail: loop only without " to ").
- Facing: retail faces the attacker on death and sets the facing from
  the impact block's angle (direction-adjusted) on an impact; the port
  leaves the facing alone.
- Snap: retail also stores `snaptime` (`+0x220`) and faces away from the
  attacker's facing.
- TObjectInstance::Damage lacks the 25% paralysis break and its
  `random(0,3)` draw.
- TPlayer::Damage matches (full Damage with no block/attacker). The
  port's `TPlayer::GetResistance` returns 0 and nothing calls it; the
  player's Resist (DmgRes stats) isn't wired into CalculateDamage (dojo
  §5.5).

### 6.5 Resolvers and actions

- ResolveImpact (`:1862`): impale blood and firsttime blood ignore the
  chardata `+0xc8 & 0x10` / `+0x164` gates; firsttime blood requires
  `damage > 0` instead of `damage/maxhealth > 1%`; combat flash is
  `combatflashticks = 3` (`:1893`) instead of EffectCombatFlash and 5
  ticks; the loop branch doesn't test FindState(loopname) and always
  SetDesires (retail ForceCommands when the last attack is PLAYANIM or
  named like doing); no network notify; refused blocks leak.
- ResolveDead (`:1962`): no chardata blood gates, no combat flash;
  returns 1 (retail 2).
- ResolveBlock (`:1950`): doesn't face the attacker each tick; returns
  2/1 where retail returns 0/2 (the meaning of retail's values is
  *unverified*).
- Block (`:4435`): the port allows blocking from IMPACT, uses the
  attack's `blockname`, requires an attacker in the blocktime window for
  the player too, has no 120 range limit, no `−2` forced block, no
  prefix.
- Dodge (`:4495`): a fixed "dodge" animation; retail's directional
  rolls, argument and priority flag are missing.
- KnockBack (`:4644`): one "cimpk" animation, no variants or RNG, no
  HasActionAni check, no flash ticks, takes the point by value.

## 7. Recon labels corrected

- `0x4c4950` is TCharacter::Damage (slot `0x228`), not "ResolveAttack";
  `0x4c1bb0` (labelled ResolveAttack in `recon/discovered/`) is Pulse
  (slot `0x110`).
- `0x5191e0` is TPlayer::Damage (slot `0x48`), not "ResolveAttack2".
- `0x4c5810` is FatigueDamage (slot `0x22c`), not TPlayer::Damage.
- `0x46e970` is TObjectInstance::Damage (slot `0x48`), not Animate.
- `0x4ce1b0` "BuildActionName" builds `prefix + name` (CombatAnimName).
- `recon/discovered/player_combat_notes.md` §3/§4 describe these under
  the wrong names; its "cheat / debug toggles" are `alreadydead`
  (`0x668104`) and `nahkranoth` (`0x668108`); `0x66829c` is multiplayer.

## 8. Open questions

1. KnockBack's name table: which of the six names is the from-behind
   one, and the variant order (stack setup `0x4d3790`–`0x4d3860`).
2. Dodge's 8×8 direction table (`0x4d3474`–`0x4d3570`) and the camera
   constant at `0x5a497c`.
3. Retail resolver return values (ResolveBlock 2 while blocking, 0 when
   done; ResolveDead 2): what ResolveAction (`0x4c3490`) does with them.
4. Action 0xf (blockimpact) has no case in ResolveAction's dispatch as
   read (`0x4c3556`–`0x4c368b`); whether it falls to the default.
5. `chardata+0xc8 & 0x10` and `+0x164` (blood gates, opposite senses in
   ResolveImpact and ResolveDead), `+0x440` / `+0x448` (low-health
   timer), `this+0xe8` (the engage gate), object flag `0x10000000`,
   `charflags 0x80` / `8` / `0x40000`, attack flag `0x10000` (fatigue
   attacks, `+0xf4` threshold).
6. ResolveImpact reads `lastattack (+0x160)` without a null check: does
   a character that never attacked ever reach that branch?
7. TObjectInstance `+0x28/+0x2c` as accum (field_map medium confidence).
8. The SCharAttackData / SCharAttackImpact layouts used above have one
   extra dword at `+0x20` against the port's (flags at `+0x24`, impact
   stride 0x5c); what it holds is unread here (kata D1).

## 9. Port status / kata

Ported from the asm above and held to the original by A/B katas (retail
run in the emulator, `tools/retail_runtime/slots/combat/melee_attack.py`;
port `src/retailab_melee.cpp`; cases `tools/retail_ab/targets_melee.py`).
Coverage is the share of the function's basic blocks some case ran
(`tools/retail_ab/melee_coverage.py`).

### 9.1 C4 `melee-hit` (7855 cases)

ResolveAttack `0x4c6dd0`, ResolveHit `0x4c62b0`, OnAttacked `0x4cdce0`
(port `SignalAttack`), with Damage a seam recording the hit block by
meaning. Coverage: ResolveHit 216/229 blocks, ResolveAttack 125/133,
OnAttacked 50/52. Not reached: a hit tier no row of TOHITDAMAGE gives,
`malloc` failing, the FULLCOMBATRES line (the case dialog list is empty),
a sound that exists, a character with no imagery, the network.

Divergences (none noticeable): ResolveAttack returns at once when the
doing block has no attack (retail would read through null); the hit block
belongs to Damage (no free after it, below); the text bar line is printed
as `"%s"` (retail passes it as the format); PlayWave by name and volume
only; the network messages aren't sent.

Emulator note: the block-sound switch in ResolveAttack (`0x4c727c`,
`cmp eax, 6; rep movsb; ja`) loses the cmp's flags across the `rep movsb`
under Unicorn; the fixture takes the `ja` as the hardware does (a hook at
`0x4c7281`). Only that site is patched; no general fix.

### 9.2 C2 `melee-damage` (1137 cases)

TCharacter::Damage `0x4c4950` whole (port `TCharacter::Damage`,
`DamageDeath`, `DamageImpact`, `ForceDamageBlock`, `DamageShare`),
TObjectInstance::Damage `0x46e970` (the paralysis break), TPlayer::Killed
`0x518ed0` / Died `0x518f90` (frag counts). Coverage: Damage 272/307,
TObjectInstance::Damage 15/16, Killed 9/18, Died 6/7. Not reached: the
network (all of the rest of Killed / Died), the floating number (below),
`malloc` failing, a given block carrying `data`, two share branches no
death can take (Health() > damage ≥ 1 with Health() ≤ 0).

Corrections to §3.4:

- When no death name exists at all, the **given block stays the death
  block** (retail jumps back with `ab` still the caller's), even one
  carrying a non-death impact; "no death block" only when none was given.
- Slots `0x244` / `0x248` are Killed(victim) / Died(killer): empty for
  characters, TPlayer's count frags (`+0x650` players killed, `+0x654`
  deaths by a player, `+0x658` others killed, `+0x65c` other deaths),
  saved with the player. Pulse `0x4c2114` also sets charflags `0x40000`
  and calls Died(NULL); when health returns it clears the flag and calls
  slot `0x24c`.
- Object flag `0x10000000` is OF_INVULNERABLE; `0x668104` is the
  "alreadydead" console cheat (also read at `0x4c2a20`, `0x4d3590`).
- `+0x220` (snaptime) is set to −1 by ClearChar and written only here;
  nothing reads it.
- The block impact is made with action 15 (the 1998 FLYBACK), which
  ResolveAction has no case for: it plays once and goes back to the root.
- Blocking with an interactive hit keeps the given block **without its
  impact for the snap and turn** (`[esp+0x14]` stays 0 on that path).
- The floating number is TCharAnimator's (`0x4da3b0`, class
  `0x5a7e38`), given −min(Health(), damage), the screen position 135 px
  above the character, and a colour by the health left
  ((Health() − damage)·100 / MaxHealth(): under 25 red, under 50 yellow,
  else white). It renders the number (`itoa`) into a 64×32 bitmap
  (64×64 when `0x669324` is set) and keeps it in slots 1–7 of eight
  (`+0x104`, 0x14 each: bitmap, ticks 36, x, y, rise).
  TCharAnimator::Animate `0x4d7a00` counts them down (`0x4da6f0`: rise
  += 400 a tick, in 1/256 px); the map pass (`0x4141f2`) draws them
  (`0x4da750`) at (x − 32, y − rise/256) less the scroll, alpha 255 until
  the last 24 ticks, then ticks·255/24. **Not ported**: the port has no
  post-character overlay yet (TPlayScreen::AddPostCharText is empty);
  Damage marks the spot.

Divergences (none noticeable): the slot after a full impact list (six)
is an empty impact (retail reads past the list); the given block's attack
is read guarded (retail reads it unguarded); a block Damage doesn't keep
is freed (retail leaks one ForceCommand refuses).

### 9.3 C5 `melee-resolvers` (2100 cases)

ResolveImpact `0x4c74b0`, ResolveBlock `0x4c77a0`, ResolveDead
`0x4c7810`, Block `0x4d2e30`, StopBlock `0x4d30f0`, Dodge `0x4d3150`, and
EffectCombatFlash `0x4c8500` (seamed in the kata: it spawns an effect).
Identities as §1 (the vtable slots and ResolveAction's dispatch, the
strings "impale", "block", "crollb", the BLOCK range). Coverage:
ResolveImpact 49/62 blocks, ResolveBlock 10/10, ResolveDead 24/25, Block
56/63, StopBlock 9/9, Dodge 63/72. Not reached: frees after a SetDesired
that can't refuse (ResolveImpact clears the priority first), `malloc`
failing, a block carrying `data`, the network, Dodge's debug-camera turn
(`0x6671f0`, not ported, as in UpdateMove), one dead branch in Block.

Answers to §8:

- (2) Dodge's last letter of "crollb", by the facing's sector (rows) and
  `dir` (columns 0–7): `0x10–0x30` bflrbfbf, `0x31–0x50` bfbfblrf,
  `0x51–0x70` rlbfbbff, `0x71–0x91` fbbfrbfl, `0x92–0xb2` fbrlfbfb,
  `0xb3–0xd2` fbfbfrlb, `0xd3–0xf2` lrfbffbb, else bffblfbr. `dir` is the
  Command (`0x47d90c`) of retail's eight controls "Combat Dodge Left /
  Right / Up / Down / UpLeft / UpRight / DownLeft / DownRight" (commands
  `0x30`–`0x37`); any other keeps the back roll. The port has one dodge
  control (GAMECMD_DODGE) and passes −1: the back roll. `0x5a497c` is
  −40.58 (−255/2π), the camera's radians to facing units.
- (3) ResolveImpact returns 0; ResolveBlock 2 while the guard holds, 0
  when it ends; ResolveDead 2.
- (4) Action 15 has no case in ResolveAction: the block impact plays once.
- (6) Retail **crashes** there (a read at `[0 + 0x24]`) when the
  character never attacked (`+0x160` is 0 until ResolveAttack sets it).
  The branch is reached only when the impact block plays under the
  impact's own name (ResolveHit names a stun or knockdown with the combat
  prefix, so mostly death and interactive impacts), with wait left and
  the loop's state present. The port: with no last attack the loop waits
  its turn (SetDesired). REVSYNC-DIVERGENCE (a crash).
- (5, part) `chardata+0xc8 & 0x10` is the bit a failing BLEEDER parse
  sets (CF_BADBLEEDER): ResolveImpact bleeds only without it, ResolveDead
  only with it, so in the shipped data a death never bleeds.
- EffectCombatFlash's frame is a state: 2·random(0, n/2), at most n − 2,
  of the effect's n imagery states (imagery slot `0x3c` is NumStates).

Divergences (none noticeable): the null last attack above; Block checks
that the guard's target is a character with an attack (retail reads
through whatever it is); a block SetDesired or ForceCommand refuses is
freed (DropUnheld; retail leaks Dodge's, ResolveDead's).

Fixture note (both sides, for every kata): the dumps name a block the
case began with by its address; a block made after one was freed may get
the same address. The retail side now drops a freed address (the engine
free `0x4830f0`, observed), the port side a deleted block
(TActionBlock::destroyedSeam), so such a block is "new N" on both.
