# Forensics: the four bosses (AI_PerMonster)

**Topic:** what the shipped game's four boss monsters do on top of the
ordinary monster AI: Baez (the Ogrok champion of the ring), Solifuge
(the cave spider), Jhaga (the priest) and Yhagoro (the last boss).
**Status:** 2026-10-11. Read from the disassembly; every claim below was
read in the asm (`recon/retail_asm/baseline`, capstone). Kata C3c of
[../COMBAT_DOJO.md](../COMBAT_DOJO.md) (`melee-boss`, §9) runs the original
function in the emulator against the port.
**Evidence:** AI_PerMonster `0x4c9b70`..`0x4cd4ab` (14,652 bytes, one
function); its helpers below. The Ghidra decompile
(`recon/discovered/cls_0x5a7b98_TCharacter_AI_PerMonster_4c9b70.cpp`) was
a guide only: it loses the stack variables Baez's walk keeps across
branches (the spot the player is sent to, §2.4) and Jhaga's quadrant
indices (§4.2).

Conventions as [COMBAT_ATTACK_CHOICE.md](COMBAT_ATTACK_CHOICE.md): `this+0xNN`
is a TCharacter field, `S+0xNN` the boss's state (`this+0x284`), block flags
retail's (priority `0x10`). Action 1 is ANIMATE, 3 COMBAT. "Desire(n)" and
"Force(n)" are the two helpers of §1; "rough distance" is the inline
`dx + dy - min(dx, dy) / 2` (§1).

## 1. Shape, helpers and fields

AI (`0x4c8b60`) calls AI_PerMonster (`0x4c8bd4`) for an aggressive
character before anything else of its own and ignores what it returns
(1 for a boss, 0 for anyone else).

```
if this+0x280 == 0:                                  // once
    kind by GetTypeName() (stricmp): "Baez" 1 (state 0x58 bytes),
        "Solifuge" 2 (0x34), "Jhaga" 3 (0x5c), "Yhagoro" 4 (0xc), else -1
    a boss: S = this+0x284 (malloc'd if null), zeroed
switch this+0x280 (table 0x4cd4ac): 1 Baez 0x4c9ce8, 2 Solifuge 0x4cbb1d,
    3 Jhaga 0x4ccaa4, 4 Yhagoro 0x4cd07d; else return 0
```

A boss whose set-up fails (Baez without his table, Jhaga without four
waypoints) is made -1 for good: it fights as an ordinary monster from then
on.

| Retail | Identity | Evidence |
|---|---|---|
| `0x4111a0` | TComplexObject **Try**(name): a block (name, ANIMATE) facing the facing, SetDesired(ab, 0); a refused block is freed | the script `try` (`0x422c8e`); port `TComplexObject::Try` |
| `0x429990` | TComplexObject **Force**(name): 2 at once while doing has priority and isn't desired; else a block (name, ANIMATE) facing the facing, ForceCommand(ab, 0, 0) (a refused block leaks) | the script `force` (`0x422d04`); port `TComplexObject::Force` |
| `0x4d7040` / `0x4d7060` / `0x4d7080` | IsDoing / IsDesired / IsRoot(name) | asm |
| `0x4d7400` / `0x4d7420` | charflags `|=` / `&= ~` | asm |
| `0x4d7020` | Face(a): facing and move angle | asm; port `Face` |
| `0x4d7000` | GetPos(out) | asm |
| `0x4d6ff0` | S3DPoint's empty constructor (returns this) | asm |
| `0x4c9af0` | cdecl MakeEffect(type, pos): NewObject of the effect class (`0x19`, FindObjType on `0x66cc68`) at pos on the pane's level, GetInstance | asm, Jhaga's only caller |
| `0x4d5b90` | TCharacter::CastByName(spell, targets, n, source) | ported |
| `0x4cedb0` | TCharacter::Goto(x, y, item) | ported |
| `0x4cee70` | TCharacter::Stop | ported (kata M6) |
| `0x4538d0` | TMapPane::SetCameraObject(obj, flags) | port `CenterOnObj` |
| `0x47c580` | TPlayScreen::SetControl(on) | port `SetControlOn` |
| `0x4d3fd0` | TCharacter::EndFighting | the port's is still 1998's |
| `0x4d6a30` / `0x4d6b40` | TCharacter tint / untint: the imagery's materials (count `+0x34`) saved to `this+0x298` (6 floats each: diffuse r,g,b at `+4`, emissive r,g,b at `+0x34` of the 0x5c-byte S3DMat) and set to diffuse (0.1, 0.1, 1.0), emissive (0.2, 0.2, 1.0); `this+0x29c` says saved. Untint puts them back and frees the copy | asm; GetMaterial `0x40b770`, SetMaterial `0x40b830` |
| `0x44cf80` | TMapIterator::Init(rect, flags, …): with (0, 0, 0, 0, -1) every object of the pane's 3x3 sector window on its level | asm |
| `0x4c8500` | EffectCombatFlash | ported (kata C5) |
| slot `0x8` / `0xc` | SetPos / MoveTo (`0x4d6e00`: movetopos = 1, movepos) | vtable `0x5a7848` |
| slot `0x18` / `0x40` / `0x48` | SetState / ResetFlags (`0x472e90`) / Damage(amount, type) (`0x46e970`, TPlayer `0x5191e0`) | vtable |
| slot `0x24` / `0x28` | GetAnimator (`+0x58`) / CreateAnimator (`0x46e8b0`) | vtable |
| slot `0x1c0` / `0x1c4` / `0x1d8` | Health / SetHealth / MaxHealth (chardata `+0x1e8`; TPlayer `0x520630`) | vtable |
| slot `0x1f8` / `0x208` / `0x218` | SetRoot / SetDesired / ForceCommand | vtable |
| slot `0x258` / `0x2cc` | Radius / WeaponType | vtable |
| imagery slot `0x90` | GetAniLength(state) | T3DImagery vtable |

The inline "rough distance" (Baez, Solifuge) is not the table Distance
`0x46de60`: `dx = |a.x - b.x|`, `dy = |a.y - b.y|`, `dy - min(dx, dy) / 2 +
dx` (the halving an arithmetic shift).

charflags the bosses set (the setters the port's comments called
unidentified): `0x4` NoTurn, `0x8` Unkillable (Baez always, Solifuge in
the air), `0x10` the /7 damage (Solifuge on the ground), `0x40` (Solifuge
in his `skywalkf`; no reader found), `0x80` NoKill (Baez, Solifuge; and the
player holding weapon type 2 against Yhagoro), `0x100` / `0x200` the halved
physical / magic damage (Yhagoro walking / fighting), `0x400` / `0x800`
(Yhagoro walking / fighting; no reader found), `0x1000` DeathHold (the
player, by Solifuge), `0x2000` / `0x4000` the walk prefix and walk-fighting
(Yhagoro walking), `0x8000` NotTargetable, `0x10000` PLAYANIM roots
(Yhagoro).

Random draws: Jhaga `random(0, 2)` once per teleport cycle (§4.2, stored
and cleared at once: unused); Solifuge two `random(0, 1)` name picks and
two `random(0, Radius·8)` offsets per minion it spawns (§3.4). Baez and
Yhagoro draw nothing of their own.

## 2. Baez (kind 1): the ring and the fake death

### 2.1 State (`S`, 0x58 bytes)

| Off | Port (`SBaezState`) | Meaning |
|---|---|---|
| `+0x00` | `stunned` | in one of his stun pauses |
| `+0x04` | `stunticks` | ticks left of it |
| `+0x08` | `stunlevel` | the health band that last stunned him: 80, 60, 40, 20 |
| `+0x0c` | `stage` | the fall: 0 fighting, 1 the walk to the table, 2 the throw, 3 dead |
| `+0x10` | `baezspot` | where he walks to: the table + (0, 0x15e, 0) |
| `+0x1c` | `playerspot` | where the player is walked second: the table + (0, 0xa0, 0) |
| `+0x28` | `table` | the table's map index |
| `+0x2c` | `frame` | ticks into the throw |
| `+0x30` | `throwlength` | frames of his `combat to cdead` |
| `+0x34` | `watchersfound` | the crowd looked up |
| `+0x38` | `playerhealth` | the player's health last tick |
| `+0x3c` | `health` | his own last tick |
| `+0x40` | `falling` | the fall into `fakedead` still to play |
| `+0x44` | `firstspot` | where the player is walked first: the table + (0x20, 0x1b0, 0x58) |
| `+0x50` | `playerarrived` | the player reached `firstspot` |
| `+0x54` | `waitticks` | ticks the walking rests (48 while the player walks, 1000 once Baez is near his spot), counted down |

The crowd: eleven objects named `mark rex vince redo paul matt austin
hersh carl riz jason2` (table `0x5df54c`) kept in the global `0x66caac`
(11 pointers, never cleared; one Baez in the game).

### 2.2 Every tick

```
charflags |= 0x88                                      // unkillable, no killing blow
if !S.watchersfound:
    walk the map (0x44cf80 (0,0,0,0,-1)): an object whose own name
        (+0x38, stricmp) is one of the eleven goes in its slot
    S.watchersfound = 1; S.health = Health(); S.playerhealth = Player ? Player.Health() : 0
h = Health(); ph = Player ? Player.Health() : 0
cheer = ph < S.playerhealth                            // the player was hurt
boo   = ph > S.playerhealth || h < S.health            // he healed, or Baez was hurt
mid = (Player.pos + pos) / 2  per axis, toward zero (the player at 0,0,0 when none)
for each watcher w:
    if !w.GetAnimator(): w.CreateAnimator()
    w.Face(ConvertToFacing(w.pos, mid))
    root watcha / watchb / watchc: cheer → w.Try("cheer"+a|b|c), else boo → w.Try("boo"+a|b|c)
S.health = Health(); if Player: S.playerhealth = Player.Health()
if S.stage: SetHealth(1); Try("walk")                  // pinned at 1 health, wanting to walk
if Health() > 1: STUNS (2.3)  → return 1
FALL (2.4)                                             → return 1
```

### 2.3 Stuns (health above 1)

```
if Health() <= 1: return                               // read again
pct = Health()·100 / MaxHealth()
for (level, below) in (80, 0) (60, 80) (40, 60) (20, 40):   // each needs the one before
    if pct <= level && S.stunlevel == below:
        S.stunlevel = level; S.stunticks = 72; S.stunned = 1; Try("combat to stunned")
if S.stunned:
    charflags |= 4; SetFighting(0)
    if IsDoing("combat"): Try("combat to stunned")
    elif IsDoing("combat to stunned"): Try("stunned")
    else: S.stunticks--
    if S.stunticks < 1: S.stunned = 0; charflags &= ~4; S.stunticks = 0; Try("stunned to combat")
elif IsDoing("stunned"): Try("stunned to combat")
```

### 2.4 The fall (health 1 or less)

```
charflags |= 4; me = pos
if S.stage == 0:
    S.firstspot = 0
    the first object named "ringwall" (map walk): ResetFlags(flags | 0x400000)   // OF_NOWALK: the ring opens
    every watcher: Try("walk")
    S.stage = 1
    the first object of type "ForBreakTable" (map walk); none:
        this+0x280 = -1; charflags &= ~0xc; return
    if S.firstspot == 0: S.firstspot = table + (0x20, 0x1b0, 0x58)
    S.playerspot = table + (0, 0xa0, 0); S.baezspot = table + (0, 0x15e, 0); S.table = its map index
    doing.priority = 0; Force("combat to fakedead"); Try("fakedead")
    SetCameraObject(this, 4); S.falling = 1
if S.stage == 1:
    if Player: Player.EndFighting()
    SetControl(0)
    if !GetAnimator(): CreateAnimator()
    Face(ConvertToFacing(me, S.baezspot))
    spot = S.playerarrived ? S.playerspot : S.firstspot
    pp = Player.pos
    if !S.playerarrived && rough(pp, spot) < 32: S.playerarrived = 1; spot = (0, 0, 0)
    if rough(pp, spot) < 32:                                      // the player is there
        if Player: Player.MoveTo(spot); Player.Stop(0); Player.Try("walk")
                   Player.Face((ConvertToFacing(pp, me) - 0x80) & 0xff)   // his back to Baez
        d = rough(me, S.baezspot)
        if d < 128: S.waitticks = 1000
        if d < 32:                                                // Baez is there too
            Face(ConvertToFacing(me, table pos (0,0,0 if gone)))
            MoveTo(S.baezspot); Try("walkf"); Stop(0)
            if Player: Player.Face(ConvertToFacing(pp, me))
            Force("combat to cdead"); Try("cdead")
            if IsDoing("combat to cdead"):
                S.stage = 2; S.frame = 0
                S.throwlength = imagery.GetAniLength(FindState("combat to cdead"))
        else:                                                     // Baez walks on
            a = ConvertToFacing(me, S.baezspot); Goto(S.baezspot.x, .y, 0); Face(a)
            unless IsDoing("combat to fakedead"):
                if IsDoing("fakedead"): Try("fakedead to walk")
                if IsDoing("fakedead to walk"): Try("walk")
                if IsDoing("combat") && S.waitticks == 0: Force("combat to fakedead"); Try("fakedead")
                if S.waitticks == 0: Try("combatf") elif IsDoing("walk"): Try("walkf")
    else:                                                         // the player walks to the spot
        S.waitticks = 48
        if S.falling:
            if IsDoing("combat to fakedead"): Try("fakedead")
            else: Force("fakedead")
                  unless IsDoing("combat to fakedead") or IsDoing("fakedead"):
                      Force("combat to fakedead"); Try("fakedead")
            if IsDoing("fakedead"): S.falling = 0
        elif !IsDoing("fakedead"):
            (Baez walks on, as above, but the "combat" rule wants S.waitticks != 0)
        if Player: Player.Try("walk"); Player.Goto(spot.x, spot.y, 0)
    if S.waitticks: S.waitticks--
if S.stage == 2:                                                  // the throw
    if S.frame == 4 && Player: Player.Force("sunsetflipper"); both charflags |= 0x8000
    if S.frame == S.throwlength·53/150: the table (map index S.table).SetState(1); EffectCombatFlash()
    if S.frame >= S.throwlength: S.stage = 3; both charflags &= ~0x8000
    S.frame++
if S.stage == 3:
    Force("cdead"); charflags &= ~8; SetHealth(0); SetCameraObject(Player, 4)
```

(The Force sites test the doing block's priority first; where Baez clears
that priority the line before, the test always passes.) The "Baez walks
on" block is the same code twice, in the two branches, with its
"combat" rule's test of `S.waitticks` the other way round (`== 0` once
the player is there, `!= 0` while he walks). While the player walks,
`waitticks` was just set to 48, so a Baez standing in "combat" always
falls again and a walking one wants `walkf`.

On the tick the player reaches `firstspot`, `spot` is cleared before the
second test, so that tick goes down the walking branch and sends the
player toward (0, 0) (`Player.Goto(0, 0, 0)`); the next tick sends him to
`playerspot`. One tick of a wrong walk target, kept.

The scene: Baez falls and plays dead; the crowd walks off and the ring
opens; the player loses control, walks to the spot before the table, then
to the one beside it, and turns his back; Baez gets up and walks to his
spot behind the player; there the player throws him (`sunsetflipper`) into
the table, which breaks with a flash; Baez dies (`cdead`) and the camera
goes back to the player. Control comes back from the area's script.

Undefined in retail: with no player in stage 1, `pp` is whatever the stack
held (the last watcher's position, or garbage); the port takes (0, 0, 0)
as Baez's mid point does.

## 3. Solifuge (kind 2): the stun, the sky walk and the brood

### 3.1 State (`S`, 0x34 bytes)

| Off | Port (`SSolifugeState`) | Meaning |
|---|---|---|
| `+0x00` | `skytimer` | ticks to the next sky walk |
| `+0x04` | `skyphase` | 0 on the ground, 1 going up, 2 in the air |
| `+0x08` | `skyticks` | ticks left in the air |
| `+0x0c` | `band` | health band: 0 above 66%, 1 above 33%, 2 the rest |
| `+0x10` | `stunticks` | ticks left stunned |
| `+0x14` | `stunned` | stunned on the ground |
| `+0x18` | `tinted` | his materials are blued (paralysed) |
| `+0x1c` | `brood[3]` | the map indices of the three spiders he called |
| `+0x28` | `landing` | the last 16 ticks of the sky walk |
| `+0x2c` | `pissedoff` | a band was lost: the roar to play |
| `+0x30` | `slam` | just landed: the next stance slams the ground |

### 3.2 Every tick, on the ground

```
charflags |= 0x80
if Player:
    if Player.Health() < 1 && !IsDoing("combat"): Player.charflags |= 0x1000; return
    Player.charflags &= ~0x1000; pp = Player.pos
me = pos; objflags = flags
pct = Health()·100 / MaxHealth()
if 66 < pct <= 100: S.band = 0
elif 33 < pct <= 66: if S.band == 0: S.pissedoff = 1;  S.band = 1
else: if S.band == 1: S.pissedoff = 1;  S.band = 2
if Health() < 1: each brood spider still there (GetInstance): SetHealth(0)
if objflags & 0x800000 (paralysed):
    charflags &= ~0x10; if S.band > 0: S.skytimer = 240
    S.stunticks = 72; S.stunned = 1
    if !S.tinted: Tint(); S.tinted = 1
    return
if S.tinted: Untint(); S.tinted = 0
if !S.stunned:
    if !S.slam: charflags &= ~0x8004
    charflags |= 0x10                                    // takes a seventh of the damage
    if S.pissedoff: if IsDoing("pissedoff"): S.pissedoff = 0  else Try("pissedoff")
    charflags 0x8000 = IsDoing("pissedoff") && !S.slam
    if S.slam && (IsDoing("combat") || IsDoing("combatf")):    // the landing slam
        S.slam = 0; S.stunned = 1; charflags = charflags & ~0x8008 | 4
        if rough(me, pp) < 3·Radius() && Player:
            Player.Try(Player.CombatAnimName("impk"))
            Player.Damage(chardata.weapondamage·2, 0)
else:
    charflags = charflags & ~0x8010 | 4
    unless IsDoing("stunned") or IsDoing("skywalk to combat") or IsDoing("skywalk"):
        doing.priority = 0; Force("stunned")
    if --S.stunticks < 1: S.stunned = 0; S.stunticks = 72; doing.priority = 0; Force("combat")
```

### 3.3 The sky walk

```
if S.skytimer && !S.skyphase:
    S.skytimer--
    if IsDoing("skywalk") || IsDoing("skywalkf"): Try("skywalk to combat")
    if !S.skytimer && !S.skyphase: S.skyphase = 1; if S.band > 1: S.skytimer = 480
if S.skyphase == 1:
    doing.priority = 0; Force("combat to skywalk"); Try("skywalk")
    S.skyphase = 2; S.skyticks = S.band > 1 ? 240 : 120; S.landing = 0
if S.skyphase == 2:
    charflags |= 0x800c; charflags 0x40 = IsDoing("skywalkf")
    if --S.skyticks == 192 && S.band > 1: THE BROOD (3.4)
    if S.skyticks == 168 && S.band > 1: each brood spider doing "sleeping": its Try("sleeping to combat")
    if S.skyticks < 1:                                         // down
        Force("skywalk to combat"); Try("combat"); if IsDesired("combat"): SetRoot(desired)
        S.skyphase = 0; S.slam = 1; S.stunned = 0; S.stunticks = 48; S.skytimer = 240
        charflags &= ~0x40
    else:
        Face(ConvertToFacing(me, pp))
        if S.skyticks == 16: S.landing = 1
        if S.landing: Try("skywalk"); charflags &= ~0x40  else Try("skywalkf")
        if IsDoing("combat to skywalk"): charflags &= ~0x40
return 1
```

The first sky walk waits for a band to be lost (the paralysis or the
landing set `skytimer`); in the lowest band they come every 480 ticks
and last 240.

Undefined in retail: with no player, the sky walk faces a point the stack
held (`pp` is only written when there is a player); the port faces (0, 0,
0). The slam asks for a player before it hurts one.

### 3.4 The brood (192 ticks before landing, lowest band)

For each of the three slots whose spider is gone (GetInstance null) or
dead: slot 0 takes `random(0, 1) ? "Arakna" : "Recluse"`, slots 1 and 2
`random(0, 1) ? "Araknid" : "Artikulid"`; a character of that type
(FindObjType on `0x66ca68`) at Solifuge's position on the pane's level,
x then y each moved by `random(0, Radius·8) - Radius·4`; NewObject(def,
-1), the slot takes its index (0 when the type is unknown); the new
spider `Try("sleeping")`. Radius is asked four times a spider.

## 4. Jhaga (kind 3): the priest's teleports

### 4.1 State (`S`, 0x5c bytes)

| Off | Port (`SJhagaState`) | Meaning |
|---|---|---|
| `+0x00` | `mode` | 0 not set up, 1 resting, 2 teleporting |
| `+0x04`, `+0x18`, `+0x2c` | `points[5]` | x, y, z of five points (retail keeps three arrays): [0] the waypoint with x and y below the centre, [1] x below / y above, [2] x above / y below, [3] both above, [4] the centre |
| `+0x40` | `resttimer` | ticks to the next teleport cycle (240) |
| `+0x44` | `draw` | `random(0, 2)` at each cycle, zeroed at once |
| `+0x48` | `hop` | which jump of the cycle |
| `+0x4c` | `dest` | where this jump lands |
| `+0x58` | `frame` | ticks into this jump |

### 4.2 Every tick

```
if !GetAnimator(): CreateAnimator()
if S.mode == 0:
    walk the map: the first four objects of type "waypoint"
    fewer than four: this+0x280 = -1; charflags &= ~4; return
    centre = their sum / 4 per axis (toward zero)
    points[0..3] = the waypoint in each quadrant round the centre (strict < / >), points[4] = centre
    S.mode = 1; S.resttimer = 240
if S.mode == 1 && --S.resttimer == 0:
    S.resttimer = 240; S.mode = 2; S.draw = random(0, 2); charflags |= 4; S.hop = 0; S.frame = 0
if S.mode == 2:
    S.draw = 0
    hop 0..4 → dest = points[0], points[3], points[1], points[2], points[4]; hop 5 → S.mode = 1, S.frame = -1
    frame 0:  MakeEffect("jtele", pos); Try("cinv2")
    frame 6:  Try("cinv2 to combat"); SetPos(dest); MakeEffect("jteled", dest)
    frame 24: Try("cinv2")
    frame 30: hop 4: CastByName("Priest Fire Wind")
              else: Face(ConvertToFacing(pos, Player ? Player.pos : points[4])); CastByName("Priest Fireball")
    frame 36: Try("cinv2 to combat")
    frame 72: S.hop++; S.frame = 0; if IsDoing("cinv2"): Try("cinv2 to combat")
    else S.frame++
return 1
```

A cycle: five jumps 72 ticks apart (the corners crosswise, then the
centre), a fireball at the player from each corner and the fire wind from
the centre, then 240 ticks of rest. `charflags & 4` stays set once the
first cycle starts.

Undefined in retail: a quadrant with no waypoint (one on the centre's x or
y) leaves its index unset and reads an uninitialised stack slot; the port
takes waypoint 0 for it.

## 5. Yhagoro (kind 4): walking and fighting

### 5.1 State (`S`, 0xc bytes)

| Off | Port (`SYhagoroState`) | Meaning |
|---|---|---|
| `+0x00` | `mode` | 0 walking, 1 fighting, -1 going back to walking |
| `+0x04` | `towalk` | the health band says walk |
| `+0x08` | `tocombat` | the health band says fight |

### 5.2 Every tick

```
if Player && Player.WeaponType() == 2: Player.charflags |= 0x80
charflags |= 0x10000
if !GetAnimator(): CreateAnimator()
pct = Health()·100 / MaxHealth()
above 75 or in (25, 50]: if S.mode == 1: S.towalk = 1          // walk
in (50, 75] or 25 and below: if S.mode == 0: S.tocombat = 1    // fight
if S.towalk && IsDoing("combat"):
    Force("combat to walk"); Try("walk")
    SetRoot(copy of the root named "walk", action COMBAT); S.mode = -1
    charflags |= 4; charflags &= ~0x8000
if S.tocombat && IsDoing("walk"):
    doing.priority = 0; Force("walk to combat"); Try("combat"); S.tocombat = 0; S.mode = 1
charflags 0x8000 = IsDoing("walk to combat") || IsDoing("combat to walk")
if S.mode == 1 && !S.tocombat && !IsDoing("walk to combat"):
    charflags: -0x100 +0x800 -0x2000 -0x4000 -0x4 +0x200 -0x400
    if !IsRoot("combat"): Try("combat") else charflags &= ~0x8000
if S.mode == 0 && !S.towalk && !IsDoing("combat to walk"):
    charflags: -0x200 +0x400 -0x4 -0x2000 -0x4000 +0x2000 +0x4000 +0x100 -0x800
    if !IsRoot("walk"):
        Force("walk"); SetRoot(copy of the root named "walk", action COMBAT)
        desired.priority = 0 (if any); Try("walk")
    else charflags &= ~0x8000
if S.mode == -1 && IsDoing("walk") && IsDesired("walk") && IsRoot("walk"):
    S.towalk = 0; S.mode = 0; charflags &= ~0x8004
return 1
```

Walking he halves physical damage and fights in his walk root (the
impale lives there: CHARACTER_PULSE.md §2 `PulseImpaleRelease`); fighting
he halves magic. The copied root keeps every field of the old one but
the name (its `data` pointer too) and is an ACTION_COMBAT block.

## 6. Port

`src/bossai.cpp` (TCharacter's boss methods) and `src/bossai.h` (the four
states, held by `TCharacter::bossstate`); AIPerMonster itself stays in
`character.cpp`. TComplexObject `Try` / `Force` became retail's
`0x4111a0` / `0x429990` (Try frees a refused block; Force has the priority
test) -- the script commands `try` / `force` call the same functions.

Divergences (none a player can see):

- The crowd is kept in Baez's state as safe references instead of the
  global array of raw pointers (a watcher unloaded with its sector is
  gone, not dangling).
- Force frees a block ForceCommand refused (retail leaks it).
- The undefined reads above (no player in Baez's stage 1 or for
  Solifuge's facing in the air, an empty quadrant for Jhaga) take defined
  values; Baez's throw tests for a player before it lets go of his
  NotTargetable (retail doesn't).
- Yhagoro's copied root keeps no `data` (retail's shares the old root's,
  which the old root's release frees).
- The state isn't saved (neither is retail's: a loaded boss sets up
  again).

Also changed for them: CastByName (`0x4d5b90`) gates before the spell
lookup, which DoAttack's MAGICATTACK (`0x4d2286`) makes without the gates
(`CastVariant`); `TCharacter::castByNameSeam` answers the whole call (kata
S1), `castSeam` the lookup and cast (C3, C3c).

## 7. Kata C3c (`melee-boss`)

Retail: `tools/retail_runtime/slots/combat/melee_attack.py`, call
`per-monster` (AI_PerMonster run as original, `ticks` times, a `Tick`
record after each); port: `src/retailab_melee.cpp` (SBossScope); cases:
`tools/retail_ab/targets_boss.py`.

```sh
python3 tools/retail_ab/retail_ab.py melee-boss [--case baez.|solifuge.|jhaga.|yhagoro.]
```

Runs as original on both sides: the boss code and everything it calls
that is ported -- SetDesired, ForceCommand, SetRoot, Try / Force, the
TActionBlock constructors and Is, Goto with Go, Stop (the player's lets go
of the controls: PlayMouseClick recorded), MoveTo, SetFighting,
CombatAnimName, CastByName's gates, SetPos (recorded), SetCameraObject
(the camera dumped), the ring wall's ResetFlags, Damage (the melee seam,
recorded with its arguments), EffectCombatFlash (recorded), the stat
reads and writes (recorded). Seams, recorded on both sides: the pane's
object walk (`0x44cf80`, the case's `paneobjects`), NewObject (the case's
`made` characters handed out by id), GetInstance by id, the spell lookup
and cast, the table's SetState, CreateAnimator, the tint, SetControl, and
EndFighting (retail's `0x4d3fd0` isn't ported: the port's is 1998's).

The state of each boss is set and dumped by the port's field names on
both sides (BOSS_FIELDS in the fixture); Baez's crowd is the retail
global's.

Coverage (basic blocks, `melee_coverage.py`; every block not reached
explained):

| Function | Reached | Not reachable |
|---|---|---|
| AI_PerMonster's dispatch `0x4c9b70` | 28/28 | |
| Baez `0x4c9ce8` | 609/727 | malloc failing 39; a fresh block's data freed 53 (none has any); a priority test or a refused want right after the priority was cleared 5; `this` null 2; the walking branch's walk-on doing "fakedead" (excluded by its own test) 10 and its "combatf" (the rest was just set to 48) 8; stage 1 without a player (stack garbage) 1 |
| Solifuge `0x4cbb1d` | 327/371 | malloc failing 15; data freed 22; priority cleared 7 |
| Jhaga `0x4ccaa4` | 99/104 | an empty quadrant (an uninitialised index) 4; malloc failing 1 |
| Yhagoro `0x4cd07d` | 98/100 | malloc failing 2 |

Cases (1925): Yhagoro 745, Jhaga 514, Solifuge 277, Baez 388, a monster
that isn't a boss 1; eleven of them a tick of the whole AI round the boss
(`bossworld`). Each machine's states and moves, the health bands at their
edges, the timers at their turns, the draws by tape, the doing block
holding priority (every want refused, every force waiting), no player, no
animator, the set-ups that fail, and runs of up to 620 ticks (Jhaga's
whole cycle, Solifuge's sky walk with its brood, Baez's throw).

Mutations of the port (each alone, the boss's cases run):

| Boss | Mutation | Caught |
|---|---|---|
| Yhagoro | the walk band's edge 75 inclusive | yes |
| Yhagoro | a knife instead of a sword sets the player's NoKill | yes |
| Yhagoro | back to walking without the desired walk | yes |
| Yhagoro | the change to walking keeps NotTargetable | no: an equivalent mutant (retail sets that bit again three lines on) |
| Jhaga | the second jump to the wrong corner | yes |
| Jhaga | the cast a tick late | yes |
| Jhaga | two quadrants swapped | yes |
| Jhaga | no turn to the player before the fireball | yes |
| Jhaga | the rest a tick longer | yes |
| Solifuge | the band's edge 66 to 67 | yes |
| Solifuge | the slam's reach two radii | yes |
| Solifuge | the brood scattered over three radii | yes |
| Solifuge | the brood called a tick late | yes |
| Solifuge | no roar for the lowest band | yes |
| Baez | the stun band 60 to 61 | yes |
| Baez | the crowd cheers an unchanged player | yes |
| Baez | the first spot 16 short | yes |
| Baez | the table breaks at 53/151 | yes |
| Baez | the walk rests 999 | yes |
| Baez | the crowd faces Baez, not the fight | yes |
