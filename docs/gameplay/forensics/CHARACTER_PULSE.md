# Forensics: the character's tick (Pulse)

**Topic:** what a character does every game tick after its move, in the
shipped game: the action-state tick, the death bookkeeping and the corpse,
the blood of the scripted deaths, the impale holds, the chained attacks
and the player's automatic combat, the AI, the recovery of health,
fatigue and mana, poison, and the invoke / burn / flail timers; then what
the player adds (the play clock, the death screen countdown, the held
light).
**Status:** 2026-10-11. Read from the disassembly; kata C6 of
[../COMBAT_DOJO.md](../COMBAT_DOJO.md) (`melee-pulse`, §5) runs both
functions in the emulator against the port.
**Evidence:** the unchanged retail image through `rdis.py`; every claim
was read in the asm. Strings by address (`0x5df72c` "decapdeath",
`0x5df740` "combat to ripinhalfed", `0x5df760` "ripkilldeath",
`0x5df778` "atkimpale to walk", `0x5df78c` "atkimpale", `0x5df798`
"impimpale", `0x5df7a4` "Rand", `0x5df7ac` "walk to dead", `0x5e00c4`
" to ", `0x5e00cc` "c", `0x5e00d0` "dead", `0x5df7d4..0x5df7ec` "walk").

Conventions as [COMBAT_HIT.md](COMBAT_HIT.md): action numbers are the
port's `ACTION` enum (1 ANIMATE, 3 COMBAT, 0xb INVOKE, 0x13 DEAD, 0x15
BURN, 0x16 FLAIL, 0x19 BOW); block flags retail's (priority `0x10`,
interrupt `0x20`, loop `0x800`).

## 1. Function table

| Retail | Identity | Evidence |
|---|---|---|
| `0x4c1bb0` | TCharacter::Pulse | TCharacter vtable `0x5a7848`; TPlayer::Pulse calls it first |
| `0x518aa0` | TPlayer::Pulse (the 1998 name was Animate; it is the Pulse slot override) | TPlayer vtable `0x5b4f30`; calls `0x4c1bb0` first |
| `0x4db190` | TComplexObject::Pulse | forced + frame ≥ 5 clears forced / transition; UpdateAction slot `0x210` with the move bits `+0xbc` |
| `0x4d57a0` | UpdateFade | only caller |
| `0x4d5f50` / `0x51d800` | slot `0x314`, the clock a character's timers run on: PlayScreen game time `+0x68c` (`0x47e940`) / the player's play clock `+0x370` | asm |
| `0x4cdf10` / `0x518f90` | slot `0x248` Died(killer): empty / the player's death count, nothing for none | asm |
| `0x4cdf20` | slot `0x24c`, told when health comes back: empty (`ret 4`) in both | asm |
| `0x4cdcd0` | slot `0x23c` SignalHostility: empty (`ret 8`) | asm |
| `0x477d30` | slot `0x154`: `commanddone` (`+0x80`) | asm |
| `0x470040` | the inventory count through a linked inventory (slot `0x170`; characters have none): `+0x68` | asm, [INVENTORY.md](../../gameflow/forensics/INVENTORY.md) §1 |
| `0x4d6570` | the death loot: the inventory onto the map round the body | callers Pulse `0x4c21e4`, network `0x5825f7` |
| `0x4d3590` | TCharacter::Burn | sets the burn timer `+0x1bc` = 50 (`0x4d35fb`) |
| `0x540750` | the character's spell list pulse (`+0x170`) | [SPELLS_MISSILES.md](SPELLS_MISSILES.md) |

TCharacter fields (retail offsets):

| Field | Meaning |
|---|---|
| `+0x138` `+0x13c` `+0x140` | last health / fatigue / mana recovery (clock time) |
| `+0x144` | last run-fatigue charge; −1 while not running |
| `+0x148` `+0x14c` `+0x150` `+0x154` `+0x158` | the health / fatigue / mana / run / poison accumulators, in 1/10000 points |
| `+0x15c` | last poison charge; −1 while not poisoned |
| `+0x188` | invoke delay (SetCast) |
| `+0x194` `+0x198` `+0x19c` `+0x1a0` | fade, its step, its limit, its direction (−1 out) |
| `+0x1bc` | burn ticks |
| `+0x228` | ticks dead |
| `+0x110` | charflags: `0x2` incidentals off, `0x8` unkillable, `0x80` no kill (an impale hold), `0x20000` let go by an impale, `0x40000` Died told, `0x100000` runs Wander + AI |
| `+0x280` | boss kind (4 Yhagoro) |
| TPlayer `+0x39c` | death-screen countdown |

`ClearChar` (`0x4c18a0`) starts the four recovery stamps (`+0x138`..`+0x144`)
at the clock (slot `0x314`), the four accumulators `+0x148`..`+0x154` at 0,
the invoke delay and burn ticks at 0, and SetPoisoned(0). `+0x158`,
`+0x15c` and `+0x228` are left as the allocator zeroed them.

Rules (`0x65d7a8`): `+0x74/+0x78/+0x7c` HEALTHDATA / FATIGUEDATA /
MANADATA recovery values, `+0x80/+0x84/+0x88` their rates, `+0x8c/+0x90`
POISONDATA value and rate. Character data: `+0x44c/+0x450` RUNFATIGUE
value and rate (the parser `0x48a73c`), `+0x158` COMBATRANGE min, `+0x164`
BLEEDER, `+0xc8` flags (bit `0x10` a bad BLEEDER).

## 2. TCharacter::Pulse `0x4c1bb0`

```
if PlayScreen+0x5d4 (0x65d0c4): return
spells (+0x170).Pulse()                          // 0x540750
animator (+0x58)->Pulse()                        // slot 0x2c, when there is one
if forcecommanddone: doing->wait = 0; commanddone = 1; forcecommanddone = 0
if combatflashticks > 0: combatflashticks--
TComplexObject::Pulse()                          // UpdateAction(move bits)
if root is COMBAT or BOW and root->obj: root->obj->SignalHostility(this, root->obj)   // empty
if CurrentScreen frames − exittimestamp > 5: clear OF_ONEXIT
if  ... > 24: +0xe4 = 0                          // the exit stood on; nothing reads it

bleeds = !(chardata.flags & 0x10) && chardata.bleeder
doing Is "decapdeath", bleeds, 5 < frame < 65, frame % 5 == 0:     blood(75,  facing − 0x5b, 0x40, 0x5b)
doing Is "combat to ripinhalfed", bleeds, 65 < frame < 73, even:    blood(105, facing − 0x5b, 0x40, 0x5b)
doing Is "ripkilldeath", bleeds, 115 < frame < 126:                 blood(130, ...) twice
named "Rand" (stricmp), doing Is "walk to dead", frame % 5 == 0:   blood(40,  facing − 0x80, 0x20, 0x20)
                                                                  // Rand's alone, bleeder or not
    blood(h, hangle, vangle, hspread): NewObject {class EFFECT, type "blood", level, pos + (0,0,h),
        facing 0}; its SetParams (slot 0x200) (h, hangle & 0xff, vangle, hspread, 5, 1)

if doing Is "atkimpale to walk" or (doing Is "atkimpale" and a player): charflags &= ~0x80
if bosskind == 4 and doing Is "impimpale" and Player: Player.charflags |= 0x20000

UpdateFade()
if Health() <= 0:
    if !(charflags & 0x40000): charflags |= 0x40000; Died(nullptr)
    clear OF_PARALIZE
elif charflags & 0x40000:
    charflags &= ~0x40000; slot 0x24c(0)                         // empty

if Health() <= 0 and !(charflags & 8):                           // the corpse
    a character (not a player, in single player) with inventory: the death loot (0x4d6570)
    if doing->action != DEAD:
        doing loses priority and interrupt
        name = root->name + " to " + (player ? AnimPrefix() : "c") + "dead"
        if HasActionAni(name):
            ab = new AB(name, DEAD); priority | interrupt | loop
            ForceCommand(ab); freed unless executing or held
        name = CombatAnimName("dead"), or "dead" without that animation
        ab = new AB(name, DEAD)
        doing->action == DEAD ? SetDesired(ab) : ForceCommand(ab)
        SetRoot(ab)
    if script: script.Trigger(DEAD); charflags |= 2              // incidentals off
    deadticks++                                                  // +0x228
    if deadticks >= 240 (a network player's corpse stays):
        fade <= 0: OF_KILL                                       // gone
        elif fade direction == 0: Health(); fade 100, step 1, limit 0, direction −1   // 100 ticks out
    return

chain / autocombo block (COMBAT_ATTACK_CHOICE.md §3.11)
player auto-combat (COMBAT_ATTACK_CHOICE.md §3.11)

if charflags & 0x20000:                                          // the impale let go
    unless desired Is "walk" and root Is "walk":
        root loses priority
        unless doing has priority and desired != doing:
            ForceCommand(new AB("walk", ANIMATE), angle = facing)    // not freed if refused
        desired loses priority
        SetDesired(new AB("walk", ANIMATE), angle = facing); freed if refused
    charflags &= ~0x20000

if charflags & 0x100000: Wander(); AI()
elif OF_AI: AI()

now = clock (slot 0x314)
each of +0x138 +0x13c +0x140 +0x144 +0x15c: if > now or |now − it| > 50000: = now

if alreadydead cheat and a player:
    health, mana, fatigue to their maximums (in that order); SetPoisoned(0)
    skip to the burn timer
health: if Health < MaxHealth:
    acc += (now − last) · value · 10000 / rate                  // 32-bit
    a player: acc += HealthPct · 10000 / 1440
    n = how many times 10000 comes off while acc > 10000
    n > 0: SetHealth(min(Health + n, MaxHealth))
  last = now (always)
mana: the same (ManaPct)
fatigue: running = root is COMBAT and Is "combatrun" / "handrun", or BOW and Is "bowrun",
         or Is "run"; and doing != root
    running: if +0x144 != −1 and Fatigue > 0:
                 run acc += (now − +0x144) · RUNFATIGUE value · 10000 / rate
                 n as above; n > 0: SetFatigue(max(0, Fatigue − n))
             +0x144 = now
    else:    +0x144 = −1; recovery as health (FatiguePct)
  +0x13c = now (always)
poison: if Poisoned:
    if +0x15c != −1:
        if Health > 1:
            d = (now − +0x15c) · value · 240000 / (rate · 100)   // 32-bit
            acc += (100 − Resist(POISON)) · d / 100
            n as above; n >= Health: n = Health − 1
            n > 0: Damage(n, POISON, 0, 0, 0)
        else SetPoisoned(0)
    +0x15c = now
  else +0x15c = −1
invoke: doing INVOKE and delay > 0: --delay == 0: root interrupts, SetDesired(root)
burn:   doing BURN and burn ticks > 0: --ticks == 0: the same
flail:  doing FLAIL and commanddone: the same, then BeginFighting(nullptr, COMBAT)
(a network host's monsters send their state every 120 frames)
```

### 2.1 How fast things recover (shipped rules.def)

HEALTHDATA 25, 1, 6000; FATIGUEDATA 3, 10, 75; MANADATA 25, 1, 6000;
POISONDATA 6, 100. The clock runs 100 a second in steps of 4, 4, 4, 4, 4,
5 (frames · 100 / 24), and each step's share is truncated before it is
added, so the rates fall a little short of the nominal:

| | Nominal | Per 6 ticks (25/100 s) | Real |
|---|---|---|---|
| health, mana | 1 a minute | 5 · 6 + 8 = 38 of 10000 | 1 per 65.8 s |
| fatigue | 10 per 0.75 s | 5 · 5333 + 6666 = 33331 | 13.3 a second |
| poison (no resistance) | value · 2400 / rate a hundredth: 1.44 a second | 5 · 576 + 720 = 3600 | 1.44 a second, never below 1 health |
| run fatigue (RUNFATIGUE 1, 100 by default) | 1 a second | 5 · 400 + 500 = 2500 | 1 a second, while the run state runs |
| Locke's run fatigue (RUNFATIGUE 10, 100) | 10 a second | 5 · 4000 + 5000 = 25000 | 10 a second: from full (78) out of breath in under 8 s |

The player adds HealthPct / ManaPct / FatiguePct (stats 21–23) · 10000 /
1440 a tick: that many points a minute.

A step that has gathered exactly 10000 keeps it (`>` not `>=`), and an
accumulator keeps its remainder while the stat is full.

### 2.2 Retail bugs kept

- The products are 32-bit. A stamp more than 21474 behind (fatigue,
  value 10) or 1491 (poison, value 6) overflows: a character whose sector
  comes back after a long absence can get a negative fatigue accumulator
  (its fatigue stalls for minutes) or a nonsense poison charge. Stamps
  further than 50000 off are pulled to now first. The port does the same
  arithmetic.
- The impale release's ForceCommand block is not freed when refused (a
  leak, the port frees it).

## 3. TPlayer::Pulse `0x518aa0`

```
TCharacter::Pulse()
play clock: state bit 0 and not bit 1: +0x374++; +0x370 = +0x374 · 100 / 24
stat effects: +0x354 != −1: pulled to now − 1 when more than 50000 off; now > it: RefreshStats
clear OF_VIRGIN                                                  // touched
if not the editor:
    charflags & 0x1000: countdown = max(countdown, 1)            // setter unidentified
    Health <= 0: countdown == 0: the death screen (0x48ea40); countdown--   (keeps going negative)
    else countdown = 192
held light (slot 0x250, with an animator): the light at the left hand, flickering:
    random(−5, 5) on x and y, the item's intensity ... (0x518bde..0x518d5a)
else if light intensity (+0x8a) > 0: light on, intensity max(0, +0x8a − 25)
else light off
(network: every 120 frames the player's state is sent)
```

RNG: only the held light draws (COMBAT_ATTACK_CHOICE.md §3.12).

## 4. What the 1998 code did differently

- Recovery: whole percentages of the maximum every `rate` hundredths
  (`MaxHealth · value / 100` per period), no accumulators, no player
  percentages, no run fatigue. Retail recovers points (value per rate),
  carries the fractions, adds HealthPct / ManaPct / FatiguePct for the
  player, and charges fatigue while running.
- Poison: a percentage of the maximum per period down to 1; retail
  charges points through Damage (so the hit shows), reduced by the
  poison resistance, and cures itself at 1 health.
- Death: the 1998 code forced a "blood" DEAD block and stayed at that;
  retail plays "<root> to <prefix>dead" then the dead loop, roots it,
  tells Died once (`0x40000`), turns incidentals off, scatters a
  monster's inventory, fades the corpse out after 10 seconds (240 ticks,
  then 100 ticks of fade) and removes it.
- Blood: 1998 sprayed for "impdecap"; retail for the four death
  animations above.
- New in retail: the impale holds (`0x80`, `0x20000`, Yhagoro), the
  burn timer (50 ticks), the flail's return to combat, the AI gate on
  OF_AI / `0x100000`, the clock pull (50000), the player's OF_VIRGIN
  clear and death-screen hold (`0x1000`).
- Auto-combat: see COMBAT_ATTACK_CHOICE.md §6.8.

## 5. Port status / kata

### 5.1 The port (`character.cpp` TCharacter::Pulse and its steps, `player.cpp` TPlayer::Pulse)

Ported as §2 and §3 read, step for step: Pulse calls PulseCorpse,
PulseAutoCombat, PulseImpaleRelease, PulseRecovery (with the invoke
timer, which the cheat skips) and PulseActionTimers. The recovery stamps,
accumulators, run stamp, corpse ticks and burn ticks are TCharacter
fields with retail's offsets in their comments; ClearChar starts them as
`0x4c18a0` does; Burn sets the 50 burn ticks (and refuses the player
under the cheat). The clock is the virtual `Clock()` (slot `0x314`); the
player's percentages come from the virtual `RecoveryBonus`. The 32-bit
products are kept (`Wrap32`, §2.2).

Not ported, and why:

- The death loot (`0x4d6570`): it scatters a dead monster's inventory
  onto the map (gold, the rest along random angles until the walk map
  takes it; the "gold" sound). It is an inventory routine with its own
  map placement; the port's corpses keep their inventory to be looted, as
  the 1998 build's did. Its call is the only part of the corpse's tick
  missing.
- Wander (slot `0x234`, for `charflags & 0x100000`): COMBAT_ATTACK_CHOICE.md
  §3.15.
- The player's held light (`0x518bcc..0x518d95`): the torch at the left
  hand with its flicker (two to four random draws a tick while a torch is
  held), and a lit player's glow. No light code in the port drives it
  yet; until it does, a torch-holding player's RNG tape differs from
  retail's.
- PlayScreen `+0x5d4` (`0x65d0c4`), which skips the whole tick: not
  identified.
- The script trigger is the 1998 call; the kata gives the corpse an empty
  script (no trigger matches on either side), which checks what follows
  it, not the trigger itself.

Divergences (REVSYNC-DIVERGENCE in the code): the player's auto-combat
refuses another player outright (retail asks the game type, which only a
network game sets for player killing); the impale release frees a walk
block ForceCommand refused (retail leaks it).

### 5.2 Kata C6 `melee-pulse` (1364 cases, all match)

`tools/retail_ab/targets_melee.py` `pulse_cases`; retail
`slots/combat/melee_attack.py` (`pulse`), port `retailab_melee.cpp`. Each
case runs `ticks` ticks of the original Pulse (TCharacter or TPlayer) and
the port's, with the vitals recorded after each tick (`Tick`), every
stat written, the blocks, and the tick's own fields compared at the end.
The tick's UpdateAction and AI are recorded and skipped (both are katas
of their own; `seam_update` false runs UpdateAction whole in two corpse
cases); stat reads are not recorded (`quiet_reads`).

Groups: recovery by vital, stamp age (0 … 50001, the 21475 / 30000 /
42950 wraps, the future), accumulator (the 10000 edge, carries,
negatives), full and over-full; the player's percentages (signed); long
runs (240 and 1440 ticks from low) for the feel; running by root and
action, stepping or not, the first tick, at 0, by RUNFATIGUE; poison by
health, age (to the overflow), resistance, the cure, a minute of it with
Damage seamed and real; the stamps pulled, past 50000 on the clock; the
cheat; Died and the paralysis; the corpse by root, prefix, the
transition / dead names present, the doing block, and the fade and
removal; blood for the four animations frame by frame, facing, bleeder,
Rand; the impale holds and release; the invoke / burn / flail timers;
auto-combat (cadence, roots, found or not, the switch, the option,
distance and angle edges, control, a dead target, the memory); the
chain; the AI gate; the exit mark; the forced command, the flash, the
forced bit; the player's clock, stat-effect expiry (and pull), OF_VIRGIN,
the death countdown with its hold, the editor, 200 ticks dead.

Retail coverage of Pulse (`melee_coverage.py`): 413 of 466 blocks before
the pull cases were added; the rest are the death loot and network
paths, the script trigger, allocation failures, Wander, and arithmetic
edges no data reaches. TPlayer::Pulse: the light paths (not ported) and
the network ones.

### 5.3 Mutations

Each broke one line of the port, rebuilt, and ran the kata (cases that
went wrong of 1356, 1364 for the last two):

| | Mutation | Caught |
|---|---|---|
| M1 | an accumulator gives a point at exactly 10000 (`>=`) | 43 |
| M2 | the recovery product in 64 bits (no wrap) | 41 |
| M3 | a stamp pulled at exactly 50000 | 43 |
| M4 | poison may take the last point | 16 |
| M5 | running costs fatigue while the root itself plays | 43 |
| M6 | the corpse fades from 241 ticks | 2 |
| M7 | decapitation blood at frame 65 too | 2 |
| M8 | auto-combat's cone takes 0x30 | 2 |
| M9 | the burn ends a tick late | 2 |
| M10 | no death-screen hold (`0x1000`) | 86 |
| M11 | the player's percentages per 1400 | 9 |
| M12 | the impale release keeps the desired block's priority | 1 |
| M13 | Died told every tick | 0: equivalent (Died(nullptr) does nothing in either class) |
| M14 | the corpse keeps its incidentals | 2 |
| M15 | the player stays "virgin" | 1 |

M12 and M14 got through the first run; the release cases with priority
on the desired block and the script cases were added for them.

### 5.4 In the arena

`locke_vs_araknid` (Demo 1's fight, both sides on the retail AI, 1440
ticks) stays deterministic. Against the same tree without this port the
Araknid dies at tick 650 instead of 380 (46 attacks instead of 30). Over
the fight Locke's fatigue now sits lower (13–32 from tick 120 to 600, his
"cfatigue" exhaustion plays most of the time) and, once it is over,
comes back at 13 a second (19 → 78 in about 5 s) where the 1998 rule
gave about 8; his health stays at 40 for the whole minute (1 point in
65 s; under the 1998 rule he had 43 by tick 60).
