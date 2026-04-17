# Combat Rules System (RULES.DEF)

## Overview

The **RULES.DEF** file is Revenant's combat configuration system that allows designers to create complex monster behaviors without programming. This data-driven approach enables rapid iteration and balancing of combat encounters.

## Character Definition Structure

### Basic Character Setup

```def
CHARACTER "MonsterName"
BEGIN
  // Core stats
  HEALTH 100
  FATIGUE 50
  MANA 25
  
  // Combat parameters
  ARMOR 3
  ATTACKFREQ 60, 90
  BLOCK 25, 10, 20
  COMBATRANGE 320
  
  // Attack definitions go here...
END
```

### Essential Combat Parameters

| Parameter | Description | Example Values |
|-----------|-------------|----------------|
| **HEALTH** | Maximum health points | 50-200 |
| **FATIGUE** | Stamina for attacks | 25-100 |
| **MANA** | Spell casting points | 0-50 |
| **ARMOR** | Defense rating (1-25) | 1-10 |
| **ATTACKFREQ** | Attack timing (tenths of seconds) | 20,80 (fast) to 100,150 (slow) |
| **BLOCK** | Block chance, min/max timing | 25,10,20 (25% chance, 10-20 frames) |
| **COMBATRANGE** | Engagement distance (64 = 1 meter) | 320 (5 meters) |

## Complete Script Command Reference

### Character Definition Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **CHARACTER** | "name" | Defines a new character/monster | `CHARACTER "Ogrok"` |
| **BEGIN** | none | Starts character definition block | `BEGIN` |
| **END** | none | Ends character definition block | `END` |

### Core Stat Commands

| Command | Parameters | Description | Range | Example |
|---------|------------|-------------|-------|---------|
| **HEALTH** | value | Maximum health points | 1-1000 | `HEALTH 75` |
| **FATIGUE** | value | Maximum fatigue/stamina | 1-200 | `FATIGUE 35` |
| **MANA** | value | Maximum spell casting points | 0-100 | `MANA 0` |
| **ARMOR** | value | Base armor rating | 1-25 | `ARMOR 3` |

### Combat Behavior Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **ATTACKFREQ** | min, max | Attack timing in tenths of seconds | `ATTACKFREQ 60, 90` |
| **BLOCK** | chance, min, max | Block frequency and timing | `BLOCK 25, 10, 20` |
| **ATTACKMOD** | modifier | Attack hit chance modifier | `ATTACKMOD 0` |
| **DEFENSEMOD** | modifier | Defense hit chance modifier | `DEFENSEMOD 0` |
| **COMBATRANGE** | distance | Engagement distance (64 = 1 meter) | `COMBATRANGE 320` |
| **COMBATWALKSPEED** | speed | Movement speed during combat | `COMBATWALKSPEED 5` |

### Weapon and Damage Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **WEAPONDAMAGE** | damage | Base weapon damage | `WEAPONDAMAGE 15` |
| **WEAPONTYPE** | type | Weapon type for damage calculations | `WEAPONTYPE WT_AXE` |
| **SWIPECOLOR** | r, g, b | Weapon trail color (0-255) | `SWIPECOLOR 255, 0, 0` |

### Perception Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **SIGHT** | min, max, range, angle | Visual perception settings | `SIGHT 30, 100, 640, 64` |
| **HEARING** | min, max, range | Audio perception settings | `HEARING 10, 50, 384` |

### Movement Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **WALKSPEED** | speed | Normal walking speed | `WALKSPEED 8` |
| **SNEAKSPEED** | speed | Sneaking movement speed | `SNEAKSPEED 4` |
| **RUNSPEED** | speed | Running movement speed | `RUNSPEED 12` |

### Bow/Archery Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **ARROWPOS** | x, y, z | Arrow spawn position offset | `ARROWPOS 0, 0, 0` |
| **ARROWSPEED** | speed | Arrow travel speed | `ARROWSPEED 10` |
| **BOWWAIT** | ticks | Delay between arrow shots | `BOWWAIT 30` |
| **BOWAIMSPEED** | angle | Bow aiming speed | `BOWAIMSPEED 5` |

### Group and Enemy Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **GROUPS** | "group1,group2" | Character group affiliations | `GROUPS "monster,undead"` |
| **ENEMIES** | "group1,group2" | Enemy group definitions | `ENEMIES "player,undead"` |

### Damage Resistance Commands

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **DAMAGEMODS** | misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison | Damage type modifiers | `DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0` |

### Character Flags

| Command | Parameters | Description | Example |
|---------|------------|-------------|---------|
| **FLAGS** | flag_value | Character special flags | `FLAGS CF_UNDEAD` |

## Attack Definition System

### ATTACK Command Structure

```def
ATTACK "animation_name", flags, button, "response", "block", "miss", "death",
       blocktime, impacttime, waittime, stunwait, mindist, maxdist,
       hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,
       weaponmask, weaponskill, attackpcnt
```

### Attack Parameter Reference

| Parameter | Type | Description | Range | Example |
|-----------|------|-------------|-------|---------|
| **animation_name** | string | 3D animation name (no tags) | - | `"thrust"` |
| **flags** | hex | Attack behavior flags | 0x0000-0xFFFFFFFF | `CA_SPECIAL \| CA_BLOOD` |
| **button** | int | Input button (1-9) | 1-9 | `1` |
| **response** | string | Triggered by opponent state | - | `"stunned"` |
| **block** | string | Custom block animation | - | `"heavyblock"` |
| **miss** | string | Miss animation | - | `"miss"` |
| **death** | string | Death animation | - | `"death"` |
| **blocktime** | int | Frames opponent can block | 1-50 | `10` |
| **impacttime** | int | Frame when attack hits | 1-100 | `14` |
| **waittime** | int | Frames before next attack | 1-200 | `25` |
| **stunwait** | int | Frames in impact state | 0-100 | `24` |
| **mindist** | int | Minimum attack distance | 1-500 | `5` |
| **maxdist** | int | Maximum attack distance | 1-500 | `150` |
| **hitminrange** | int | Minimum hit detection | 0-200 | `5` |
| **hitmaxrange** | int | Maximum hit detection | 0-200 | `80` |
| **hitangle** | int | Hit detection cone | 0-128 | `32` |
| **damagemod** | int | Damage modifier % | -100 to +100 | `10` |
| **fatigue** | int | Stamina cost | 0-50 | `5` |
| **attackskill** | int | Required skill level | 0-10 | `3` |
| **weaponmask** | hex | Allowed weapon types | 0x0000-0xFFFF | `WM_AXE` |
| **weaponskill** | int | Required weapon skill | 0-10 | `2` |
| **attackpcnt** | int | Relative frequency | 1-100 | `80` |

### Attack Flags Reference

| Flag | Value | Description | Usage |
|------|-------|-------------|-------|
| **CA_SPECIAL** | 0x0001 | High-priority special attack | Rare, powerful moves |
| **CA_RESPONSE** | 0x0002 | Triggered by opponent state | Counter-attacks |
| **CA_SPLATTER** | 0x0004 | Causes opponent to splatter | Gore effects |
| **CA_DEATH** | 0x0008 | High damage death attack | Finishing moves |
| **CA_HAND** | 0x0010 | Hand-to-hand attack | Unarmed combat |
| **CA_THRUST** | 0x0020 | Thrust attack | Swords, knives |
| **CA_SLASH** | 0x0040 | Slashing attack | Swords, axes |
| **CA_CHOP** | 0x0080 | Chopping attack | Axes, heavy weapons |
| **CA_SPARKS** | 0x0100 | Show sparks on block | Metal weapons |
| **CA_BLOOD** | 0x0200 | Show blood on impact | Sharp weapons |
| **CA_NOMISS** | 0x1000 | Play full animation | Always complete |
| **CA_SNEAK** | 0x2000 | Only in sneak mode | Stealth attacks |
| **CA_CHAIN** | 0x4000 | Chain attack | Combo sequences |
| **CA_AUTOCOMBO** | 0x8000 | Auto-combo | Automatic chains |
| **CA_ATTACKDOWN** | 0x40000 | Attack downed opponents | Ground attacks |
| **CA_ATTACKSTUN** | 0x80000 | Attack stunned opponents | Opportunity attacks |
| **CA_MOVING** | 0x100000 | Only when moving | Mobile attacks |
| **CA_ONETARGET** | 0x200000 | Single target only | Focused attacks |
| **CA_NOPUSH** | 0x400000 | No character push | Precise positioning |
| **CA_MAGICATTACK** | 0x800000 | Magical attack | Spell casting |
| **CA_PLAYANIM** | 0x1000000 | Animation only | Non-damaging |
| **CA_INTERACTIVE** | 0x2000000 | Interactive attack | Synchronized |
| **CA_SNEAKMODE** | 0x4000000 | Works in sneak mode | Stealth compatible |
| **CA_WALKMODE** | 0x8000000 | Works in walk mode | Movement compatible |
| **CA_BOWMODE** | 0x10000000 | Works in bow mode | Ranged compatible |
| **CA_RUNNING** | 0x20000000 | Running attack | Mobile combat |
| **CA_ACTION** | 0x40000000 | Action, not attack | Non-combat |

### Weapon Type Definitions

| Type | Value | Description | Example Weapons |
|------|-------|-------------|-----------------|
| **WT_HAND** | 0 | Natural weapons | Claws, fists, tails |
| **WT_KNIFE** | 1 | Small blades | Daggers, knives |
| **WT_SWORD** | 2 | Swords | Short swords, long swords |
| **WT_BLUDGEON** | 3 | Blunt weapons | Clubs, maces, hammers |
| **WT_AXE** | 4 | Axes | Battle axes, hatchets |
| **WT_STAFF** | 5 | Polearms | Spears, staffs, halberds |
| **WT_BOW** | 6 | Bows | Longbows, shortbows |
| **WT_CROSSBOW** | 7 | Crossbows | Light crossbows, heavy crossbows |

### Weapon Mask Definitions

| Mask | Value | Description | Usage |
|------|-------|-------------|-------|
| **WM_HAND** | 0x0001 | Hand weapons | Unarmed combat |
| **WM_KNIFE** | 0x0002 | Knife weapons | Small blades |
| **WM_SWORD** | 0x0004 | Sword weapons | Swords |
| **WM_BLUDGEON** | 0x0008 | Bludgeon weapons | Clubs, maces |
| **WM_AXE** | 0x0010 | Axe weapons | Axes |
| **WM_STAFF** | 0x0020 | Staff weapons | Polearms |
| **WM_BOW** | 0x0040 | Bow weapons | Bows |
| **WM_CROSSBOW** | 0x0080 | Crossbow weapons | Crossbows |
| **WM_TWOHAND** | 0x000E | Two-handed weapons | Swords, axes, bludgeons |

## Impact System

### IMPACT Command Structure

```def
IMPACT "impact_animation", flags, "loopname", looptime, mindmgpcnt, maxdmgpcnt, snapdist, snaptime
```

### Impact Parameter Reference

| Parameter | Type | Description | Range | Example |
|-----------|------|-------------|-------|---------|
| **impact_animation** | string | Impact animation name | - | `"impact"` |
| **flags** | hex | Impact effect flags | 0x0000-0xFFFF | `CAI_STUN` |
| **loopname** | string | Looping state name | - | `"stunloop"` |
| **looptime** | int | Loop duration (frames) | 0-200 | `72` |
| **mindmgpcnt** | int | Minimum damage % | 0-100 | `1` |
| **maxdmgpcnt** | int | Maximum damage % | 0-100 | `100` |
| **snapdist** | int | Position snap distance | 0-200 | `80` |
| **snaptime** | int | Snap duration (frames) | 0-50 | `5` |

### Impact Flags Reference

| Flag | Value | Description | Usage |
|------|-------|-------------|-------|
| **CAI_STUN** | 0x0001 | Opponent becomes stunned | Disable opponent |
| **CAI_KNOCKDOWN** | 0x0002 | Opponent knocked down | Ground opponent |
| **CAI_DEATH** | 0x0004 | Death impact | Kill opponent |
| **CAI_WHENSTUNNED** | 0x0008 | Only when already stunned | Chain stuns |
| **CAI_WHENDOWN** | 0x0010 | Only when already down | Chain knockdowns |
| **CAI_FLYBACK** | 0x0020 | Opponent flies back | Knockback effect |
| **CAI_BLOOD** | 0x0040 | Blood effect | Gore visualization |

### CHARIMPACT Command

```def
CHARIMPACT "impact_name", flags, "loopname", looptime, mindmgpcnt, maxdmgpcnt, snapdist, snaptime
```

**CHARIMPACT** defines character-specific responses to being hit, allowing for:
- **Custom knockdown animations** - Different fall styles per character
- **Damage-based responses** - Different impacts for light/heavy damage
- **State-specific responses** - Different when already stunned/knocked down

## Character Flags Reference

| Flag | Value | Description | Usage |
|------|-------|-------------|-------|
| **CF_UNDEAD** | 0x0001 | Undead creature | Immune to normal weapons |
| **CF_MAGICAL** | 0x0002 | Magical creature | Immune to non-magical weapons |
| **CF_INFRAVISION** | 0x0004 | See in darkness | Dark vision ability |
| **CF_LIGHTBLIND** | 0x0008 | Blinded by light | Reverse light/dark vision |

## Damage Type Modifiers

### DAMAGEMODS Parameter

```def
DAMAGEMODS misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
```

| Parameter | Description | Example Values | Usage |
|-----------|-------------|----------------|-------|
| **misc** | Generic damage | 0, -10, +20 | General damage |
| **hand** | Hand-to-hand | 0, -5, +15 | Unarmed combat |
| **puncture** | Piercing damage | 0, -20, +10 | Arrows, thrusts |
| **cut** | Cutting damage | 0, -15, +5 | Swords, knives |
| **chop** | Chopping damage | 0, -10, +20 | Axes, heavy weapons |
| **bludgeon** | Blunt damage | 0, -25, +15 | Clubs, maces |
| **magical** | Magical damage | 0, -30, +25 | Spells, enchanted weapons |
| **burn** | Fire damage | 0, -40, +30 | Fire spells, lava |
| **freeze** | Cold damage | 0, -35, +25 | Ice spells, frost |
| **poison** | Poison damage | 0, -50, +20 | Poison, venom |

## Perception System

### SIGHT Parameters

```def
SIGHT sightmin, sightmax, range, angle
```

| Parameter | Description | Range | Example |
|-----------|-------------|-------|---------|
| **sightmin** | Sight value at max range | 1-100 | `30` |
| **sightmax** | Sight value at min range | 1-100 | `100` |
| **range** | Maximum sight range | 64-1000 | `640` (10 meters) |
| **angle** | Field of view | 0-255 | `64` (90° cone) |

### HEARING Parameters

```def
HEARING hearingmin, hearingmax, range
```

| Parameter | Description | Range | Example |
|-----------|-------------|-------|---------|
| **hearingmin** | Hearing at max range | 1-100 | `10` |
| **hearingmax** | Hearing at min range | 1-100 | `50` |
| **range** | Maximum hearing range | 64-1000 | `384` (6 meters) |

## Timing and Distance Reference

### Distance Units

| Unit | Description | Real-world equivalent |
|------|-------------|----------------------|
| **64 units** | 1 meter | 3.28 feet |
| **320 units** | 5 meters | 16.4 feet |
| **640 units** | 10 meters | 32.8 feet |

### Timing Units

| Unit | Description | Real-world equivalent |
|------|-------------|----------------------|
| **1 frame** | 1/24 second | 41.67 milliseconds |
| **10 frames** | ~0.42 seconds | 416.7 milliseconds |
| **24 frames** | 1 second | 1000 milliseconds |
| **60 frames** | 2.5 seconds | 2500 milliseconds |

### Attack Frequency Examples

| ATTACKFREQ | Description | Attack Rate |
|------------|-------------|-------------|
| `20, 80` | Very fast | Every 0.2-0.8 seconds |
| `60, 90` | Medium | Every 0.6-0.9 seconds |
| `100, 150` | Slow | Every 1.0-1.5 seconds |
| `200, 300` | Very slow | Every 2.0-3.0 seconds |

## Combat Design Workflow

### 1. Character Concept

**Define the monster's personality:**
- **Aggressive vs Defensive** - Attack frequency and blocking
- **Fast vs Slow** - Attack timing and movement
- **Powerful vs Weak** - Damage and health values
- **Specialized vs Versatile** - Number and variety of attacks

### 2. Attack Selection

**Choose attack types based on character:**
- **Melee fighters** - Close-range thrusts, slashes, chops
- **Ranged attackers** - Throwing, magical attacks
- **Brutes** - Heavy, slow attacks with knockdown
- **Agile fighters** - Fast, mobile attacks

### 3. Timing Balance

**Coordinate attack timing:**
- **Fast attackers** - ATTACKFREQ 20,80 (0.2-0.8 seconds)
- **Medium attackers** - ATTACKFREQ 60,90 (0.6-0.9 seconds)
- **Slow attackers** - ATTACKFREQ 100,150 (1.0-1.5 seconds)

### 4. Frequency Weighting

**Use attackpcnt to create attack patterns:**
- **100** - Most common attack (bread and butter)
- **80** - Common attack
- **40** - Uncommon attack
- **20** - Rare special attack
- **10** - Very rare special

### 5. Range and Positioning

**Define tactical behavior:**
- **mindist/maxdist** - When to use this attack
- **hitminrange/hitmaxrange** - Actual hit detection
- **hitangle** - Directional accuracy

## Advanced Combat Techniques

### Chain Attacks

```def
// Chain attack 3 (final)
ATTACK "chain3", CA_CHAIN | CA_SLASH, 1, "", "", "", "chain2",
       9, 11, 30, 0, 5, 150, 5, 80, 32, 0, 0, 0, 0, 0, 0

// Chain attack 2 (middle)
ATTACK "chain2", CA_CHAIN | CA_CHOP, 1, "", "", "", "chain1",
       9, 11, 12, 24, 5, 150, 5, 80, 32, 0, 0, 0, 0, 0, 0

// Chain attack 1 (start)
ATTACK "chain1", CA_MOVING | CA_CHAIN | CA_THRUST, 1, "", "", "", "",
       5, 7, 8, 24, 5, 150, 5, 80, 32, 0, 0, 0, 0, 0, 0
```

**Chain Design:**
- **chainname** links attacks together
- **chainexptime** sets timing window
- **CA_CHAIN** enables automatic progression

### Special Attacks

```def
// Opportunity attack for stunned opponents
ATTACK "finisher", CA_SPECIAL | CA_ATTACKSTUN | CA_DEATH, 1, "", "", "", "",
       1, 8, 164, 0, 1, 50, 1, 50, 32, 10, 5, 0, 0, 0, 20
```

**Special Attack Design:**
- **CA_SPECIAL** - High priority
- **CA_ATTACKSTUN** - Only when opponent stunned
- **Low attackpcnt** - Rare but powerful

### Interactive Attacks

```def
ATTACK "grapple", CA_INTERACTIVE | CA_NOPUSH, 1, "", "", "", "",
       1, 1, 120, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 30
IMPACT "grapple_impact", 0, "", 0, 1, 100, 80, 0
```

**Interactive Design:**
- **CA_INTERACTIVE** - Synchronized attack/impact
- **CA_NOPUSH** - No character displacement
- **Synchronized timing** - Attack and impact coordinate

## Monster Archetype Examples

### The Brute (Ogrok)

**Design Philosophy:** Slow, powerful, intimidating
```def
CHARACTER "Ogrok"
BEGIN
  HEALTH 75
  FATIGUE 35
  ARMOR 3
  ATTACKFREQ 60, 90        // Slow, deliberate
  BLOCK 25, 10, 20         // Moderate defense
  WEAPONDAMAGE 15          // High damage
  
  // Rare but devastating specials
  ATTACK "axethrow", CA_SPECIAL | CA_BLOOD, 1, "", "", "", "",
         1, 1, 125, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 10
  
  // Standard heavy attacks
  ATTACK "jumpchop", CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         14, 14, 37, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 100
END
```

### The Swarm (Arakna)

**Design Philosophy:** Fast, numerous, overwhelming
```def
CHARACTER "Arakna"
BEGIN
  HEALTH 45
  FATIGUE 35
  ARMOR 2
  ATTACKFREQ 20, 80        // Very fast
  COMBATWALKSPEED 5         // Attacks while moving
  WEAPONDAMAGE 8           // Lower damage
  
  // Fast, mobile attacks
  ATTACK "combatbite", CA_SLASH | CA_BLOOD, 1, "", "", "", "",
         12, 16, 30, 0, 20, 52, 10, 52, 30, 0, 0, 0, 0, 0, 100
END
```

### The Tank (Issathi)

**Design Philosophy:** High health, defensive, persistent
```def
CHARACTER "Issathi"
BEGIN
  HEALTH 100               // Very high health
  FATIGUE 100              // High stamina
  ARMOR 5                  // Good armor
  BLOCK 15, 24, 36        // Longer block timing
  ATTACKFREQ 60, 90        // Standard timing
  
  // Single powerful attack
  ATTACK "bitefling", CA_SPECIAL | CA_BLOOD | CA_NOPUSH, 1, "", "", "", "",
         1, 1, 65, 0, 1, 80, 0, 80, 16, 0, 0, 0, 0, 0, 40
END
```

## Balancing Guidelines

### Attack Frequency Balancing

**Relative Weight System:**
- **Primary attacks** (100): 60-80% of total attacks
- **Secondary attacks** (40-80): 20-30% of total attacks
- **Special attacks** (10-30): 5-15% of total attacks
- **Ultimate attacks** (1-10): 1-5% of total attacks

### Timing Coordination

**Critical Timing Rules:**
- **blocktime < impacttime** - Blocks must happen before hits
- **waittime > impacttime** - Can't interrupt attacks
- **stunwait** - Creates combo opportunities

### Range and Positioning

**Tactical Design:**
- **mindist/maxdist** - Attack selection based on distance
- **hitminrange/hitmaxrange** - Actual hit detection
- **hitangle** - Directional accuracy (32 = 45° cone)

## Common Design Patterns

### 1. Combo Builders

**Progressive damage chains:**
```def
// Light attack (builds combo)
ATTACK "light", CA_CHAIN, 1, "", "", "", "medium", 5, 7, 8, 24, 5, 150, 5, 80, 32, 0, 0, 0, 0, 0, 100

// Medium attack (continues combo)
ATTACK "medium", CA_CHAIN, 1, "", "", "", "heavy", 7, 10, 12, 24, 5, 150, 5, 80, 32, 5, 0, 0, 0, 0, 80

// Heavy attack (finishes combo)
ATTACK "heavy", CA_CHAIN, 1, "", "", "", "", 10, 14, 20, 0, 5, 150, 5, 80, 32, 15, 0, 0, 0, 0, 60
```

### 2. Opportunity Attacks

**Exploiting opponent states:**
```def
// Attack stunned opponents
ATTACK "stunfinish", CA_SPECIAL | CA_ATTACKSTUN, 1, "", "", "", "",
       1, 5, 30, 0, 1, 50, 1, 50, 32, 20, 0, 0, 0, 0, 15

// Attack knocked down opponents
ATTACK "groundfinish", CA_SPECIAL | CA_ATTACKDOWN, 1, "", "", "", "",
       1, 8, 40, 0, 1, 50, 1, 50, 32, 25, 0, 0, 0, 0, 10
```

### 3. Defensive Patterns

**Balancing offense and defense:**
```def
// High block chance, short timing
BLOCK 50, 5, 10           // 50% chance, 5-10 frame window

// Low block chance, long timing
BLOCK 15, 20, 40          // 15% chance, 20-40 frame window
```

## Testing and Iteration

### Combat Testing Checklist

1. **Attack Timing** - Do attacks feel responsive?
2. **Frequency Balance** - Are special attacks rare enough?
3. **Range Behavior** - Does monster use appropriate attacks at different distances?
4. **Defense Balance** - Is blocking frequency appropriate?
5. **Damage Balance** - Are damage values appropriate for health levels?

### Iteration Process

1. **Playtest** - Fight the monster multiple times
2. **Adjust timing** - Modify ATTACKFREQ and attack timing
3. **Tune frequency** - Adjust attackpcnt values
4. **Balance damage** - Modify WEAPONDAMAGE and damagemod
5. **Refine defense** - Adjust BLOCK and ARMOR values

## Integration with Animation

### Animation Naming Conventions

**Standard naming:**
- **Attack animations** - "thrust", "swing", "chop"
- **Impact animations** - "impact", "heavyimpact"
- **Block animations** - "block", "heavyblock"
- **Death animations** - "death", "heavydeath"

**Animation tags (in 3DS Max, not RULES.DEF):**
- **"_s"** - Synchronized frame (for impacts/blocks)
- **"_l"** - Looping state (for stuns/knockdowns)
- **"_o"** - One-shot state

### Animation Coordination

**Critical timing coordination:**
- **impacttime** must match animation frame where weapon hits
- **blocktime** must be before impacttime
- **waittime** must allow animation to complete
- **stunwait** must match looping animation length

This system allows designers to create complex, varied monster behaviors through data configuration rather than programming, enabling rapid iteration and balancing of combat encounters. 