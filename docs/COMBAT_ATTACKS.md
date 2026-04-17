# Combat Attacks: The Heart of Revenant

## Overview

Attacks are the **fundamental building blocks** of Revenant's combat system. Every combat interaction - from a simple sword swing to complex synchronized moves like the Ogrok's pile driver - is built on the same underlying attack framework. Understanding how attacks work is key to understanding how the entire game functions.

## The Attack Philosophy

### Why This System Exists

Revenant's attack system was designed to solve a fundamental problem: **how do you create varied, dynamic combat without hardcoding every possible interaction?**

The answer was to create a **data-driven system** where:
- **Attacks are defined in data files** (RULES.DEF), not code
- **The same engine handles all attacks** - from simple swings to complex combos
- **Designers can create new attacks** without touching code
- **Attacks can respond to context** - distance, timing, opponent state

### Core Design Principles

1. **Animation-Driven**: Every attack is fundamentally an animation with timing data
2. **Context-Aware**: Attacks can check conditions (distance, timing, opponent state)
3. **Priority-Based**: Special attacks override normal attacks when conditions are met
4. **Synchronized**: Multiple characters can play coordinated animations
5. **Extensible**: New attack types can be added through data, not code

## How Attacks Work

### The Attack Lifecycle

Every attack follows the same basic lifecycle:

1. **Selection**: System chooses which attack to use based on priority and conditions
2. **Validation**: Checks if the attack can be performed (range, timing, fatigue, etc.)
3. **Initiation**: Starts the attack animation and sets up timing
4. **Execution**: Plays the animation and handles timing events
5. **Impact**: At the specified frame, resolves hit detection and damage
6. **Completion**: Finishes the animation and returns to combat state

### The Magic of Timing

The key insight is that **timing is everything**. Each attack has critical timing points:

- **blocktime**: When the opponent can block (must be before impact)
- **impacttime**: When the attack actually hits (critical for hit detection)
- **waittime**: When the character can attack again
- **stunwait**: How long to stay in impact state

This timing system allows for:
- **Precise hit detection** at specific animation frames
- **Blocking windows** that match the visual animation
- **Combo opportunities** through stun timing
- **Synchronized interactions** between characters

## Interactive Attacks: The Pile Driver

### What Makes Interactive Attacks Special

Interactive attacks like the Ogrok's pile driver are where Revenant's system really shines. These are **synchronized animations** where two characters play coordinated movements together.

### How the Pile Driver Works

```def
// Ogrok's pile driver attack
ATTACK "piledriver", CA_INTERACTIVE | CA_SPECIAL | CA_NOPUSH, 1, "", "", "", "",
       1, 1, 120, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 30

// The victim's pile driver impact
IMPACT "piledriver_victim", CAI_KNOCKDOWN, "piledriver_loop", 72, 1, 100, 80, 0
```

**Key Elements:**
- **CA_INTERACTIVE**: Tells the system this is a synchronized attack
- **CA_NOPUSH**: Prevents character displacement during the move
- **Synchronized timing**: Both characters start their animations at the same frame
- **Coordinated impacts**: The victim plays a specific response animation

### The Synchronization Process

1. **Attack Initiation**: Ogrok starts "piledriver" animation
2. **Victim Response**: Target immediately starts "piledriver_victim" animation
3. **Synchronized Playback**: Both animations play at the same speed
4. **Coordinated Impact**: At the impact frame, both characters are in the right positions
5. **Loop State**: Victim continues in "piledriver_loop" for extended effect

### Why This Works

The system achieves synchronization through:
- **Frame-perfect timing**: Both animations start at the same game frame
- **Position snapping**: Characters are moved to the correct relative positions
- **Animation coordination**: The animations are designed to work together
- **Impact synchronization**: Hit detection happens at the exact right moment

## Combat in Action: A Narrative Journey

### The Dance of Basic Combat

Imagine you're playing as Locke, the game's protagonist, facing off against a bandit in the ruins of Aven. The combat begins with the familiar rhythm of basic attacks.

**The Opening Exchange:**
Locke steps forward, his sword raised. The bandit lunges with a thrust - a simple `CA_THRUST` attack with `impacttime` of 8 frames. You see the bandit's arm extend, the blade gleaming in the torchlight. At exactly frame 8, the system checks if Locke is within the `hitangle` of 32 (a 45° cone) and within the `mindist`/`maxdist` range of 5-150 units.

If Locke is hit, the `CA_BLOOD` flag triggers crimson droplets to spray from the wound, and the `stunwait` of 15 frames keeps Locke staggered just long enough for the bandit to follow up. But if Locke blocks at the right moment (before the `blocktime` of 6 frames), the blade rings against his sword with a satisfying `CA_SPARKS` effect.

**The Counter-Attack:**
Now it's Locke's turn. The system evaluates his available attacks. His basic swing has `attackpcnt` of 60, meaning it's used 60% of the time when no special conditions are met. The swing has `impacttime` of 10 frames - you can see Locke's sword arc through the air, building momentum before the decisive strike at frame 10.

The `waittime` of 20 frames means Locke must wait 20 frames before he can attack again, creating the natural rhythm of combat. Too aggressive, and you'll be caught in recovery. Too defensive, and you'll lose momentum.

### The Art of Chain Attacks

Now imagine Locke has learned the "Triple Strike" combo from Master Jong. This is where the system's sophistication really shows.

**The Combo Begins:**
Locke initiates "chain1" - a quick thrust with `impacttime` of 8 frames. The `CA_CHAIN` flag tells the system this attack can continue into another. At frame 24 (the `waittime`), instead of returning to idle, the system automatically transitions to "chain2" if the player presses attack again.

**The Momentum Builds:**
"Chain2" is a horizontal slash with `impacttime` of 12 frames. The damage has increased from 5 to 7, and the `CA_CHAIN` flag allows it to continue to "chain3". The timing window is tight - you have 5 frames to press attack again, creating skill expression.

**The Climax:**
"Chain3" is the finisher - a powerful overhead chop with `impacttime` of 20 frames and damage of 10. The `CA_CHOP` flag might trigger special effects, and there's no `CA_CHAIN` flag, so the combo ends here. The `waittime` of 0 means Locke can immediately start another attack if he's fast enough.

The beauty is that each attack in the chain feels distinct - the quick thrust, the flowing slash, the devastating chop - but they flow together seamlessly through the chain system.

### Opportunity Attacks: The Tactical Mind

Combat isn't just about swinging swords. Revenant's system recognizes that timing and situation matter.

**The Stunned Opponent:**
A bandit has been knocked down by a previous attack. Locke's "stunfinish" attack has `CA_ATTACKSTUN` - it only works when the opponent is stunned. The system checks the opponent's state and, finding them stunned, allows this special attack.

The "stunfinish" has `attackpcnt` of 1, meaning it's very rare, but when it triggers, it does massive damage (20 points) with `impacttime` of 30 frames - a slow, deliberate execution move. The `CA_SPECIAL` flag might trigger unique effects or animations.

**The Moving Target:**
Locke spots a bandit running away. His "running thrust" has `CA_MOVING` - it only works when the target is moving. The system detects the bandit's movement and allows this specialized attack, which might have longer range or different timing to account for the moving target.

### Interactive Attacks: The Dramatic Moments

Now imagine the most spectacular moment - Locke facing an Ogrok, one of the game's most fearsome enemies.

**The Setup:**
The Ogrok has been weakened, and Locke has positioned himself perfectly. The Ogrok's "piledriver" attack has `CA_INTERACTIVE` - this isn't just an attack, it's a synchronized dance between two characters.

**The Synchronization:**
When the Ogrok initiates the piledriver, something remarkable happens. Both characters start their animations simultaneously - the Ogrok begins his charging animation, and Locke immediately starts his "piledriver_victim" response. The `CA_NOPUSH` flag prevents the characters from being displaced during the complex move.

**The Impact:**
At frame 72 (the `impacttime`), both characters are perfectly positioned. The Ogrok's massive fists connect with Locke's body, and the `CAI_KNOCKDOWN` flag sends Locke sprawling. But this isn't the end - the system transitions Locke into "piledriver_loop", a continuous animation that shows him being pummeled.

**The Aftermath:**
The `stunwait` of 30 frames keeps Locke helpless while the Ogrok can follow up with additional attacks. The `waittime` of 100 frames means the Ogrok must wait before using this devastating move again, preventing it from being spammed.

### The Subtle Art of Timing

What makes Revenant's combat feel so responsive is the precision of its timing system.

**The Blocking Window:**
Every attack has a `blocktime` that must be before the `impacttime`. This creates realistic blocking - you can't block after you've been hit. The visual feedback matches the mechanical timing perfectly.

**The Recovery Rhythm:**
The `waittime` creates natural combat rhythm. Too short, and combat becomes a button-mashing frenzy. Too long, and it feels sluggish. Revenant's timing creates a dance-like quality where each move has weight and consequence.

**The Stun System:**
The `stunwait` creates opportunities for follow-up attacks and combos. A well-timed stun can turn the tide of battle, while being stunned yourself creates moments of vulnerability that add tension.

### The Context-Aware Combat

Revenant's system doesn't just execute attacks - it chooses them intelligently based on context.

**Distance Matters:**
A "close thrust" might have `mindist` of 1 and `maxdist` of 50, while a "long sweep" might have `mindist` of 100 and `maxdist` of 200. The system automatically chooses the appropriate attack based on your distance from the target.

**Timing Creates Opportunities:**
The `attackpcnt` system means some attacks are rare but powerful. When that 1% chance "critical strike" triggers, it feels special and memorable.

**State-Dependent Attacks:**
The `CA_ATTACKSTUN`, `CA_ATTACKDOWN`, and `CA_MOVING` flags create attacks that only work in specific situations, making combat feel tactical and responsive.

## Common Attack Patterns

### 1. Basic Attacks (Most Common)

**Purpose**: Bread-and-butter combat moves
**Usage**: 60-80% of all attacks in most characters

```def
// Standard sword swing
ATTACK "swing", CA_SLASH | CA_SPARKS | CA_BLOOD, 2, "", "", "", "",
       6, 10, 19, 0, 0, 0, 0, 80, 32, 0, 5, 0, WM_SWORD, 0, 0
```

**Why they work**: Simple, reliable, good damage-to-fatigue ratio

### 2. Chain Attacks (Very Common)

**Purpose**: Progressive damage sequences
**Usage**: 20-30% of attacks for characters with combos

```def
// Chain attack 1 (start)
ATTACK "chain1", CA_CHAIN | CA_THRUST, 1, "", "", "", "chain2",
       5, 7, 8, 24, 5, 150, 5, 80, 32, 0, 0, 0, 0, 0, 0

// Chain attack 2 (continue)
ATTACK "chain2", CA_CHAIN | CA_SLASH, 1, "", "", "", "chain3",
       7, 10, 12, 24, 5, 150, 5, 80, 32, 5, 0, 0, 0, 0, 0

// Chain attack 3 (finish)
ATTACK "chain3", CA_CHAIN | CA_CHOP, 1, "", "", "", "",
       10, 14, 20, 0, 5, 150, 5, 80, 32, 15, 0, 0, 0, 0, 0
```

**Why they work**: Progressive damage, timing windows create skill expression

### 3. Opportunity Attacks (Common)

**Purpose**: Exploit opponent weaknesses
**Usage**: 10-20% of attacks for tactical characters

```def
// Attack stunned opponents
ATTACK "stunfinish", CA_SPECIAL | CA_ATTACKSTUN, 1, "", "", "", "",
       1, 5, 30, 0, 1, 50, 1, 50, 32, 20, 0, 0, 0, 0, 15
```

**Why they work**: High damage when conditions are met, creates tactical depth

### 4. Interactive Attacks (Rare but Powerful)

**Purpose**: Dramatic, synchronized moves
**Usage**: 5-10% of attacks, usually special moves

```def
// Grapple attack
ATTACK "grapple", CA_INTERACTIVE | CA_NOPUSH, 1, "", "", "", "",
       1, 1, 120, 0, 1, 80, 1, 80, 32, 0, 0, 0, 0, 0, 30
IMPACT "grapple_victim", 0, "", 0, 1, 100, 80, 0
```

**Why they work**: Creates memorable moments, shows system sophistication

## Attack Design Philosophy

### The "Why" Behind Attack Design

**1. Context Matters**
- Attacks should feel different based on situation
- Distance, timing, opponent state all affect attack choice
- This creates tactical depth without complexity

**2. Timing is Everything**
- Precise timing creates skill expression
- Blocking windows, combo timing, impact timing all matter
- This makes combat feel responsive and meaningful

**3. Animation Drives Everything**
- Every attack is fundamentally an animation
- Timing data is tied to animation frames
- This ensures visual and mechanical consistency

**4. Data Over Code**
- Attacks are defined in data files, not hardcoded
- This allows rapid iteration and balancing
- Designers can create new attacks without programmers

### Common Design Patterns

**The "Bread and Butter" Pattern**
- One attack with high frequency (attackpcnt = 100)
- Reliable, good damage-to-fatigue ratio
- Used when no special conditions are met

**The "Combo Builder" Pattern**
- Progressive damage through chain attacks
- Each attack in the chain does more damage
- Timing windows create skill expression

**The "Opportunity Exploiter" Pattern**
- Special attacks that only work in specific conditions
- High damage when conditions are met
- Creates tactical decision-making

**The "Dramatic Moment" Pattern**
- Interactive attacks for special situations
- Synchronized animations between characters
- Creates memorable combat moments

## What's Commonly Used vs. Rare

### Commonly Used (80% of attacks)

**Basic Attack Types:**
- **CA_SLASH**: Standard cutting attacks
- **CA_THRUST**: Piercing attacks
- **CA_CHOP**: Heavy chopping attacks
- **CA_BLOOD**: Blood effects on hit

**Basic Timing:**
- **impacttime**: 7-15 frames (most common)
- **waittime**: 15-25 frames (most common)
- **attackpcnt**: 40-100 (most common)

**Basic Ranges:**
- **mindist/maxdist**: 1-150 units (most common)
- **hitangle**: 32 (45° cone, most common)

### Rarely Used (20% of attacks)

**Special Flags:**
- **CA_INTERACTIVE**: Synchronized attacks
- **CA_ATTACKSTUN**: Only when opponent stunned
- **CA_ATTACKDOWN**: Only when opponent down
- **CA_MOVING**: Only when moving

**Special Timing:**
- **impacttime**: 1 frame (instant hits)
- **waittime**: 100+ frames (very slow recovery)
- **attackpcnt**: 1-10 (very rare attacks)

**Special Ranges:**
- **hitangle**: 128+ (very wide cones)
- **mindist/maxdist**: 200+ units (very long range)

## Creating Effective Attacks

### The Design Process

**1. Start with the Animation**
- What does the attack look like?
- What frame does the weapon hit?
- How long should the recovery be?

**2. Define the Context**
- When should this attack be used?
- What conditions make it appropriate?
- How often should it occur?

**3. Balance the Numbers**
- Damage vs. fatigue cost
- Frequency vs. power
- Range vs. safety

**4. Test and Iterate**
- Does it feel responsive?
- Is the timing right?
- Does it create interesting decisions?

### Common Pitfalls

**Timing Issues:**
- **blocktime > impacttime**: Blocks happen after hits
- **waittime < impacttime**: Can interrupt attacks
- **impacttime > animation length**: Attack never hits

**Balance Issues:**
- **Too high attackpcnt**: Attack used too often
- **Too low attackpcnt**: Attack never used
- **Wrong damage/fatigue ratio**: Attack too weak or too costly

**Range Issues:**
- **mindist > maxdist**: Invalid range
- **hitangle too wide**: Unrealistic hit detection
- **Range too small**: Attack too situational

## The Future of Attacks

### What Made This System Special

Revenant's attack system was ahead of its time in several ways:

**1. Data-Driven Design**
- Attacks defined in configuration, not code
- Rapid iteration and balancing
- Designer-friendly workflow

**2. Context-Aware Combat**
- Attacks respond to situation
- Creates tactical depth
- Feels dynamic and responsive

**3. Synchronized Interactions**
- Multi-character coordinated animations
- Creates dramatic moments
- Shows technical sophistication

**4. Extensible Framework**
- New attack types through data
- No code changes needed
- Future-proof design

### The Practical Reality

Despite its sophistication, the system had significant practical limitations:

**The Comma-Delimited Nightmare:**
The RULES.DEF file used bare-bones comma-delimited format with no comments or structure. Each attack was defined like:
```def
ATTACK "swing", CA_SLASH | CA_SPARKS | CA_BLOOD, 2, "", "", "", "",
       6, 10, 19, 0, 0, 0, 0, 80, 32, 0, 5, 0, WM_SWORD, 0, 0
```

This made the file:
- **Hard to read** - no visual structure or comments
- **Error-prone** - one misplaced comma broke everything
- **Difficult to maintain** - no validation or documentation
- **Artist-unfriendly** - required technical knowledge to edit

**The Artist Workflow Reality:**
The actual workflow was more practical than the file format suggested:
- **Interactive testing** - artists would export animations and immediately test them in-game
- **Real-time iteration** - changes could be reloaded without restarting
- **Visual feedback** - artists could see their work immediately in context
- **Rapid prototyping** - new attacks could be created and tested quickly

**The Complexity Problem:**
The system's sophistication came with costs:
- **Steep learning curve** - understanding all the flags and parameters
- **Debugging difficulty** - hard to trace why an attack wasn't working
- **Balance complexity** - many interconnected parameters to tune
- **Documentation gaps** - the comma-delimited format discouraged documentation

Revenant's attack system represents a sophisticated approach to combat design that was innovative for its era. Its emphasis on data-driven design, context-aware behavior, and synchronized interactions created a combat system that was both technically impressive and deeply engaging. 