# Combat System in Revenant

## Overview

Revenant features a sophisticated **real-time combat system** with advanced attack scripting, dynamic AI behavior, and complex combat mechanics. The system supports both **player-controlled combat** and **AI-driven combat** with extensive scripting capabilities for customizing combat behavior.

**For designers and artists working with the RULES.DEF combat configuration system, see [COMBAT_RULES.md](COMBAT_RULES.md) for detailed documentation on creating and balancing monster combat behaviors.**

## Core Combat Architecture

### Combat System Components

1. **Attack Data System** - SCharAttackData and SCharAttackImpact structures
2. **Combat AI** - Dynamic AI behavior and decision making
3. **Combat Scripting** - Script triggers and custom combat behavior
4. **Combat Mechanics** - Damage calculation, blocking, and impact systems
5. **Combat States** - Action state management for combat

## Attack Data System

### SCharAttackData - Attack Definition Structure

```cpp
struct SCharAttackData
{
    char attackname[MAXANIMNAME];   // Name of attack animation
    int32_t flags;                      // Attack flags (CA_* constants)
    int32_t button;                     // Attack button ID for input
    int32_t attackpcnt;                 // AI: Percentage chance to use this attack
    int32_t mindist, maxdist;           // Min/max distance for attack
    
    union
    {
        struct    // Ordinary attack information
        {
            char responsename[MAXANIMNAME]; // Response combo/counter animation
            char blockname[MAXANIMNAME];    // Block animation opponent can use
            char missname[MAXANIMNAME];     // Miss animation
            char chainname[MAXANIMNAME];    // Previous attack for chain combos
            int32_t blocktime;                  // Frames opponent can block/counter
            int32_t impacttime;                 // Frames to wait before impact
            int32_t chainexptime;               // Time limit for chain attacks
            int32_t nextwait;                   // Frames to wait before next attack
            int32_t hitminrange, hitmaxrange;   // Hit range
            int32_t hitangle;                   // Hit angle cone
            int32_t damagemod;                  // Damage modifier (10=+10%, -30=-30%)
            int32_t fatigue;                    // Fatigue cost
            int32_t attackskill;                // Min offensive skill required
            int32_t weaponmask;                 // Weapon type mask
            int32_t weaponskill;                // Min weapon skill required
            int32_t numimpacts;                 // Number of impact variations
            SCharAttackImpact impacts[MAXATTACKIMPACTS];  // Impact data array
        };
        struct     // Magical attack information
        {
            char spellname[RESNAMELEN];     // Spell to cast
            S3DPoint spellsource;           // Spell source offset
        };
    };
};
```

### SCharAttackImpact - Impact Effect Structure

```cpp
struct SCharAttackImpact
{
    char impactname[MAXANIMNAME];   // Impact animation name
    int32_t flags;                      // Impact flags (CAI_* constants)
    char loopname[MAXANIMNAME];     // Loop animation for stun/knockdown
    int32_t looptime;                   // Time to play loop animation
    int32_t damagemin, damagemax;       // Damage range for this impact
    int32_t snapdist, snaptime;         // Snap/push distance and time
};
```

## Attack Flags System

### Combat Attack Flags (CA_*)

| Flag | Value | Description |
|------|-------|-------------|
| **CA_SPECIAL** | 0x0001 | Special attack that overrides normal attacks |
| **CA_RESPONSE** | 0x0002 | Response combo enabled by opponent state |
| **CA_SPLATTER** | 0x0004 | Causes opponent to splatter if successful |
| **CA_DEATH** | 0x0008 | Used when damage is high enough to kill |
| **CA_HAND** | 0x0010 | Hand-to-hand attack type |
| **CA_THRUST** | 0x0020 | Thrust attack (can't do with axe) |
| **CA_SLASH** | 0x0040 | Slashing attack |
| **CA_CHOP** | 0x0080 | Chopping attack |
| **CA_SPARKS** | 0x0100 | Show sparks on block |
| **CA_BLOOD** | 0x0200 | Show blood on impact |
| **CA_SNAPIMPACT** | 0x0400 | Snap the impact animation |
| **CA_SNAPBLOCK** | 0x0800 | Snap the block animation |
| **CA_NOMISS** | 0x1000 | Play entire attack even if misses |
| **CA_SNEAK** | 0x2000 | Only available in sneak mode |
| **CA_CHAIN** | 0x4000 | Automatic chain attack when button pressed |
| **CA_AUTOCOMBO** | 0x8000 | Auto-combo after waiting |
| **CA_ATTACKDOWN** | 0x40000 | Attack for when opponent is on ground |
| **CA_ATTACKSTUN** | 0x80000 | Attack for when opponent is stunned |
| **CA_MOVING** | 0x100000 | Only available when moving |
| **CA_ONETARGET** | 0x200000 | Applied to only one character |
| **CA_NOPUSH** | 0x400000 | No character push effect |
| **CA_MAGICATTACK** | 0x800000 | Magical attack (casts spell) |
| **CA_PLAYANIM** | 0x1000000 | Just plays animation |
| **CA_INTERACTIVE** | 0x2000000 | Interactive attack (syncs impacts) |
| **CA_SNEAKMODE** | 0x4000000 | Works in sneak mode |
| **CA_WALKMODE** | 0x8000000 | Works in walk mode |
| **CA_BOWMODE** | 0x10000000 | Works in bow mode |
| **CA_RUNNING** | 0x20000000 | Running attack |
| **CA_ACTION** | 0x40000000 | Action, not attack |

### Combat Impact Flags (CAI_*)

| Flag | Value | Description |
|------|-------|-------------|
| **CAI_STUN** | 0x0001 | Results in looping stun animation |
| **CAI_KNOCKDOWN** | 0x0002 | Results in looping knockdown |
| **CAI_DEATH** | 0x0004 | Death impact (non-default) |
| **CAI_WHENSTUNNED** | 0x0008 | Only used when character is stunned |
| **CAI_WHENDOWN** | 0x0010 | Only used when character is knocked down |
| **CAI_FLYBACK** | 0x0020 | Causes character to fly back |
| **CAI_BLOOD** | 0x0040 | This impact spouts blood |

## Damage Types

```cpp
#define DT_NONE     -1      // No damage
#define DT_MISC     0       // Generic damage
#define DT_HAND     1       // Hand-to-hand combat
#define DT_PUNCTURE 2       // Arrows, knives, sword thrusts
#define DT_CUT      3       // Cutting/slashing damage
#define DT_CHOP     4       // Chopping damage
#define DT_BLUDGEON 5       // Bludgeoning damage
#define DT_MAGICAL  6       // Magical damage
#define DT_BURN     7       // Burn damage
#define DT_FREEZE   8       // Freeze damage
#define DT_POISON   9       // Poison damage
```

## Combat AI System

### AI Decision Making

```cpp
void TCharacter::AI()
{
    if (flags & OF_DISABLED || IsDead() || NoAI)
        return;

    PTCharacter target = Fighting();
    
    // Find target if none exists
    if (!target && Aggressive())
    {
        target = FindClosestEnemy();
        if (target && Distance(target) < chardata->combatrangemin)
            BeginCombat(target);
    }

    if (target && doing)
    {
        if (doing->action == ACTION_COMBAT)
        {
            if (nextattack > 0)
                nextattack--;

            if (nextattack == 0)
            {
                if (random(1,100) <= chardata->blockfreq)
                    Block();
                else
                    RandomAttack(random(1,100));
                nextattack--;
            }
            else if (nextattack <= 0)
            {
                nextattack = random(chardata->minattackfreq * FRAMERATE / 100, 
                                   chardata->maxattackfreq * FRAMERATE / 100);
            }
        }
    }
}
```

### AI Combat Behavior

**Target Selection:**
- **FindClosestEnemy()** - Finds closest visible enemy
- **Combat Range** - Uses `chardata->combatrangemin` for engagement
- **Line of Sight** - Checks visibility before targeting

**Attack Decision:**
- **Block Frequency** - `chardata->blockfreq` percentage chance to block
- **Attack Frequency** - Random timing between `minattackfreq` and `maxattackfreq`
- **Attack Selection** - Uses `RandomAttack()` with percentage-based selection

**Combat Movement:**
- **Combat Range Management** - Maintains optimal attack distance
- **Target Tracking** - Follows moving targets
- **Positioning** - Strategic positioning for attacks

## Combat Scripting System

### Script Triggers

**Combat Triggers:**
- **TRIGGER_COMBAT** - Triggered when character enters combat mode
- **TRIGGER_DEAD** - Triggered when character dies
- **TRIGGER_ATTACK** - Triggered when character attacks
- **TRIGGER_IMPACT** - Triggered when character is hit

### Script Integration

```cpp
bool TCharacter::BeginCombat(PTCharacter target)
{
    // ... combat initialization ...
    
    if (GetScript())
        GetScript()->Trigger(TRIGGER_COMBAT); // Trigger combat script
    
    return true;
}
```

### Combat Commands

**Available Script Commands:**
- **Combat** - Enter/exit combat mode
- **Attack** - Execute specific attack
- **Block** - Enter blocking stance
- **Stop** - Stop current action
- **Jump** - Jump to script label

## Combat Mechanics

### Attack Resolution

```cpp
int32_t TCharacter::ResolveAttack(PTActionBlock ab, int32_t bits)
{
    PTCharacter targ = (PTCharacter)ab->obj;
    PSCharAttackData attack = ab->attack;
    PSCharAttackImpact impact = ab->impact;

    // Signal attack to target
    if (ab->firsttime && targ)
        targ->SignalAttack(this, ab->obj);

    // Handle character push (unless CA_NOPUSH)
    if (attack && !(attack->flags & CA_NOPUSH) && 
        !(attack->flags & CA_INTERACTIVE) && targ)
    {
        // Push target character
    }

    // Impact at specific frame
    if (attack && frame == attack->impacttime)
    {
        bool hit = false;
        
        // Main target
        if (targ)
            hit = ResolveHit(targ, attack, impact, ab->damage);

        // Multiple targets (unless CA_ONETARGET)
        if (!(attack->flags & CA_ONETARGET))
        {
            // Find additional targets in range
            // Apply damage to all targets
        }

        // Handle miss
        if (!hit && !(attack->flags & CA_NOMISS))
        {
            // Play miss animation
        }
    }

    return COM_EXECUTING;
}
```

### Hit Detection

**Hit Validation:**
- **Range Check** - `hitminrange` to `hitmaxrange`
- **Angle Check** - `hitangle` cone validation
- **Line of Sight** - Obstacle detection
- **Target State** - Valid target conditions

**Damage Calculation:**
- **Base Damage** - Weapon damage + character stats
- **Damage Modifier** - Attack-specific modifier
- **Damage Type** - Type-specific multipliers
- **Armor Reduction** - Target armor calculations

### Blocking System

**Block Mechanics:**
- **Block Timing** - `blocktime` frames to block
- **Block Animation** - `blockname` animation
- **Block Sounds** - Character-specific block sounds
- **Block Effects** - Sparks and visual feedback

**Block Validation:**
- **Timing Check** - Within block window
- **Animation Check** - Valid block animation exists
- **State Check** - Character can block

## Combat State Management

### Combat States

**Primary Combat States:**
- **ACTION_COMBAT** - Standard combat stance
- **ACTION_ATTACK** - Executing attack
- **ACTION_IMPACT** - Taking damage
- **ACTION_BLOCK** - Blocking stance
- **ACTION_STUN** - Stunned state
- **ACTION_KNOCKDOWN** - Knocked down
- **ACTION_DEAD** - Death state

### State Transitions

**Combat State Flow:**
1. **Enter Combat** - `BeginCombat()` → `ACTION_COMBAT`
2. **Execute Attack** - `ACTION_COMBAT` → `ACTION_ATTACK`
3. **Impact Resolution** - `ACTION_ATTACK` → `ACTION_IMPACT`
4. **Return to Combat** - `ACTION_IMPACT` → `ACTION_COMBAT`

## Advanced Combat Features

### Chain Attacks

**Chain Attack System:**
- **CA_CHAIN** - Links attacks together
- **chainname** - Previous attack requirement
- **chainexptime** - Time limit for chain
- **Automatic Execution** - Triggers on button press

### Combo System

**Auto-Combo System:**
- **CA_AUTOCOMBO** - Automatic combo execution
- **Timing Control** - `nextwait` between attacks
- **Progressive Damage** - Increasing damage in combos

### Interactive Combat

**Interactive Features:**
- **CA_INTERACTIVE** - Synchronized impacts
- **No Character Block** - Bypasses character collision
- **No Push** - Prevents character displacement
- **Impact Synchronization** - Coordinated hit effects

### Magical Combat

**Spell Integration:**
- **CA_MAGICATTACK** - Triggers spell casting
- **spellname** - Spell to cast
- **spellsource** - Spell origin point
- **Automatic Casting** - Integrated with attack system

## Performance Optimizations

### Combat Optimizations

1. **Attack Validation** - Pre-validates attacks before execution
2. **Range Culling** - Only processes nearby targets
3. **State Caching** - Caches valid attack states
4. **Animation Optimization** - Efficient animation transitions

### Memory Management

**Combat Memory:**
- **Dynamic Attack Arrays** - Per-character attack data
- **Impact Pooling** - Shared impact data
- **State Cleanup** - Automatic state management

## Integration with Game Systems

### Animation Integration

**Combat Animation Features:**
- **Attack Animations** - Synchronized with combat timing
- **Impact Animations** - Damage-specific animations
- **Block Animations** - Defensive animations
- **Transition Animations** - Smooth state changes

### Sound Integration

**Combat Audio:**
- **Attack Sounds** - Weapon-specific sounds
- **Impact Sounds** - Hit and damage sounds
- **Block Sounds** - Defensive sound effects
- **Miss Sounds** - Failed attack sounds

### Visual Effects

**Combat Effects:**
- **Blood Effects** - Damage visualization
- **Spark Effects** - Block and weapon effects
- **Impact Effects** - Hit feedback
- **Particle Systems** - Advanced visual effects

## Modern Port Considerations

### Combat System Modernization

**Potential Improvements:**
1. **Advanced AI** - Machine learning-based combat behavior
2. **Physics Integration** - Realistic combat physics
3. **Advanced Animation** - Procedural combat animations
4. **Network Combat** - Multiplayer combat synchronization

**Preservation Priorities:**
1. **Combat Timing** - Maintain original combat rhythm
2. **AI Behavior** - Preserve character personality
3. **Script Compatibility** - Ensure script functionality
4. **Visual Fidelity** - Maintain combat impact

## Summary

Revenant's combat system represents a sophisticated approach to real-time combat for its time, featuring:

- **Advanced Attack Scripting** - Complex attack definitions with flags
- **Dynamic AI Behavior** - Intelligent combat decision making
- **Comprehensive Scripting** - Extensive combat script integration
- **Sophisticated Mechanics** - Damage, blocking, and impact systems
- **Performance Optimization** - Efficient combat processing

The system provides a solid foundation for modern ports while maintaining the original game's combat depth and responsiveness.

## Real Monster Combat Examples

### Ogrok (Axe-wielding Monster)

```def
CHARACTER "Ogrok"
BEGIN
  HEALTH 75
  FATIGUE 35
  ARMOR 3
  BLOCK 25,10,20           // 25% chance to block, 10-20 frame timing
  ATTACKFREQ 60, 90        // Attack every 0.6-0.9 seconds
  WEAPONDAMAGE 15
  WEAPONTYPE WT_AXE
  
  // Axe throw (rare special attack)
  ATTACK "atkaxethrow", CA_SPECIAL | CA_BLOOD, 1, "", "", "", "",
         1, 1, 125, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 10
  IMPACT "impaxethrow", 0, "", 0, 1, 100, 43, 0
  
  // Head throw (uncommon special)
  ATTACK "headthrow", CA_SPECIAL | CA_BLOOD, 1, "", "", "", "",
         1, 1, 120, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 20
  
  // Standard thrust (common)
  ATTACK "thrust", CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         10, 10, 10, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 40
  
  // Standard swing (very common)
  ATTACK "swing", CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         9, 9, 25, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 80
  
  // Jumping chop (most common)
  ATTACK "jumpchop", CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         14, 14, 37, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 100
END
```

**Behavior Analysis:**
- **Attack Frequency**: 10% axe throw, 20% head throw, 40% thrust, 80% swing, 100% jump chop
- **Relative weights**: Jump chop is most common, axe throw is rarest
- **Timing**: 0.6-0.9 second attack intervals
- **Defense**: 25% block chance with 10-20 frame timing

### Arakna (Spider Monster)

```def
CHARACTER "Arakna"
BEGIN
  HEALTH 45
  FATIGUE 35
  ARMOR 2
  ATTACKFREQ 20, 80        // Much faster attacks (0.2-0.8 seconds)
  WEAPONDAMAGE 8
  WEAPONTYPE WT_AXE
  COMBATWALKSPEED 5         // Moves while attacking
  
  // Custom impact responses
  CHARIMPACT "cimpacthighheavy", CAI_FLYBACK | CAI_KNOCKDOWN, "backloop", 72, 7, 999, 0, 0
  CHARIMPACT "heavydeath", CAI_DEATH, "death", 0, 8, 999, 0, 0
  
  // High biting attack
  ATTACK "combatbite", CA_SLASH | CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         12, 16, 30, 0, 20, 52, 10, 52, 30, 0, 0, 0, 0, 0, 100
  
  // Low biting attack
  ATTACK "attackbitelow", CA_SLASH | CA_SPARKS | CA_BLOOD, 1, "", "", "", "",
         6, 8, 20, 0, 48, 75, 36, 75, 32, 0, 0, 0, 0, 0, 100
END
```

**Behavior Analysis:**
- **Fast attacks**: 0.2-0.8 second intervals (much faster than Ogrok)
- **Two attack types**: High and low biting with different ranges
- **Movement**: Attacks while moving (COMBATWALKSPEED)
- **Custom impacts**: Special knockdown and death responses
- **Range variation**: High bite (20-52 units), low bite (48-75 units)

### Issathi (Lizard Monster)

```def
CHARACTER "Issathi"
BEGIN
  HEALTH 100               // Higher health than others
  FATIGUE 100              // Higher fatigue (more stamina)
  ARMOR 5                  // Better armor
  ATTACKFREQ 60, 90        // Standard attack timing
  BLOCK 15,24,36          // Lower block chance but longer timing
  WEAPONDAMAGE 10
  WEAPONTYPE WT_HAND       // Natural weapons
  
  // Special fling attack
  ATTACK "bitefling", CA_SPECIAL | CA_BLOOD | CA_NOPUSH, 1, "", "", "", "",
         1, 1, 65, 0, 1, 80, 0, 80, 16, 0, 0, 0, 0, 0, 40
  IMPACT "cimpbitefling", 0, "", 0, 1, 100, 80, 0
END
```

**Behavior Analysis:**
- **Tank-like**: High health, fatigue, and armor
- **Defensive**: Lower block chance but longer block timing
- **Specialized**: Single powerful attack with no pushback
- **Natural weapons**: Uses hand damage type (different from weapons)

## Combat Design Patterns

### Attack Frequency Balancing

The `attackpcnt` values create relative weights:
- **100**: Most common attack
- **40-80**: Common attacks  
- **10-30**: Uncommon attacks
- **1-10**: Rare special attacks

### Timing Coordination

- **blocktime < impacttime**: Ensures blocks happen before hits
- **waittime > impacttime**: Prevents interrupting attacks
- **stunwait**: Creates combo opportunities

### Range and Positioning

- **mindist/maxdist**: Attack selection based on distance
- **hitminrange/hitmaxrange**: Actual hit detection range
- **hitangle**: Directional hit detection
- **snapdist**: Position adjustment for impacts

This data-driven approach allows for complex, varied monster behaviors without requiring custom AI code for each monster type. 