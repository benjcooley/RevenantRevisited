# Statistics System in Revenant

## Overview

Revenant uses a comprehensive statistics system with two levels: **Class Statistics** (shared across all instances of an object type) and **Instance Statistics** (unique to individual objects). Statistics are defined using `DEFSTAT` for class-level and `DEFOBJSTAT` for instance-level statistics.

## Statistics Definition Format

```cpp
DEFSTAT(Class, Name, ID, Index, Default, Min, Max)
DEFOBJSTAT(Class, Name, ID, Index, Default, Min, Max)
```

**Parameters:**
- `Class` - Object class name
- `Name` - Statistic name
- `ID` - Four-character unique identifier
- `Index` - Array index position
- `Default` - Default value
- `Min` - Minimum allowed value
- `Max` - Maximum allowed value

## Complete Statistics Table

### Player Character Statistics (OBJCLASS_PLAYER)

#### Core Character Values
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Level** | LEV | 1 | 1 | 100 | Character level |
| **Exp** | EXP | 0 | 0 | 1,000,000 | Experience points |

#### Physical Attributes
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Strn** (Strength) | STRN | 14 | 0 | 100 | Physical power, affects damage |
| **Cons** (Constitution) | CONS | 14 | 0 | 100 | Health and stamina, affects max health/fatigue |
| **Agil** (Agility) | AGIL | 14 | 0 | 100 | Movement and dexterity, affects lockpicking |
| **Rflx** (Reflexes) | RFLX | 14 | 0 | 100 | Reaction speed and accuracy |
| **Mind** (Mind) | MIND | 14 | 0 | 100 | Mental power, affects max mana and magic |
| **Luck** (Luck) | LUCK | 14 | 0 | 100 | Random chance modifier |

#### Combat Skills
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Attack** | ATK | 0 | 0 | 100 | General offensive combat ability |
| **Defense** | DEF | 0 | 0 | 100 | General defensive combat ability |
| **Invoke** | INV | 0 | 0 | 100 | Magic casting ability |

#### Weapon Skills
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Hands** | HAN | 0 | 0 | 100 | Hand-to-hand combat |
| **Knife** | KNF | 0 | 0 | 100 | Daggers and short blades |
| **Sword** | SWR | 0 | 0 | 100 | Swords and long blades |
| **Bludgeons** | BLD | 0 | 0 | 100 | Clubs, maces, hammers |
| **Axes** | AXE | 0 | 0 | 100 | Axes and chopping weapons |
| **Bows** | BOW | 0 | 0 | 100 | Bows and ranged weapons |

#### Utility Skills
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Stealth** | SLT | 0 | 0 | 100 | Sneaking and stealth ability |
| **LockPick** | LPK | 0 | 0 | 100 | Lockpicking ability |

#### Skill Experience (Hidden)
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **AttackExp** | ATKE | 0 | 0 | 1,000,000 | Attack skill experience |
| **DefenseExp** | DEFE | 0 | 0 | 1,000,000 | Defense skill experience |
| **InvokeExp** | INVE | 0 | 0 | 1,000,000 | Invocation skill experience |
| **HandsExp** | HANE | 0 | 0 | 1,000,000 | Hands skill experience |
| **KnifeExp** | KNFE | 0 | 0 | 1,000,000 | Knife skill experience |
| **SwordExp** | SWRE | 0 | 0 | 1,000,000 | Sword skill experience |
| **BludgeonsExp** | BLDE | 0 | 0 | 1,000,000 | Bludgeons skill experience |
| **AxesExp** | AXEE | 0 | 0 | 1,000,000 | Axes skill experience |
| **BowsExp** | BOWE | 0 | 0 | 1,000,000 | Bows skill experience |
| **StealthExp** | SLTE | 0 | 0 | 1,000,000 | Stealth skill experience |
| **LockPickExp** | LPKE | 0 | 0 | 1,000,000 | Lockpicking skill experience |

### Character Statistics (OBJCLASS_CHARACTER)

#### Core Character Stats
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Radius** | RAD | 16 | 16 | 256 | Character collision radius |

#### Character Flags (Instance Stats)
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Aggressive** | AGGR | 0 | 0 | 1 | Character is aggressive |
| **Poisoned** | PSND | 0 | 0 | 1 | Character is poisoned |
| **Sleeping** | SLP | 0 | 0 | 1 | Character is sleeping |

#### Vital Statistics (Instance Stats)
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Health** | HLT | 25 | 0 | 10,000 | Current health points |
| **Fatigue** | FAT | 25 | 0 | 10,000 | Current fatigue points |
| **Mana** | MAN | 25 | 0 | 10,000 | Current mana points |

### Weapon Statistics (OBJCLASS_WEAPON)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **EqSlot** | EQSL | 3 | 0 | 9 | Equipment slot assignment |
| **Type** | TYPE | WT_HAND | WT_HAND | WT_LAST | Weapon type |
| **Damage** | DMG | 0 | 0 | 10,000 | Base damage value |
| **Combining** | COMB | 0 | 0 | 16 | Combination properties |
| **Poison** | PSN | 0 | 0 | 32 | Poison damage |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

### Armor Statistics (OBJCLASS_ARMOR)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **EqSlot** | EQSL | 4 | 0 | 8 | Equipment slot assignment |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |
| **Protection** | PROT | 1 | 1 | 10 | Damage reduction value |
| **Combining** | COMB | 0 | 0 | 16 | Combination properties |
| **ResistPoison** | RPSN | 0 | -100 | 100 | Poison resistance |
| **Stealth** | STLH | 0 | -100 | 100 | Stealth modifier |

### Food Statistics (OBJCLASS_FOOD)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |
| **Health** | HLTH | 0 | 0 | 1,000,000 | Health points restored |
| **Mana** | MANA | 0 | 0 | 1,000,000 | Mana points restored |
| **Fatigue** | FATG | 0 | 0 | 1,000,000 | Fatigue points restored |
| **Poison** | PSN | 0 | 0 | 1,000,000 | Poison damage/healing |
| **Cure** | CURE | 0 | 0 | 1,000,000 | Poison cure effect |
| **Fill** | FILL | 0 | 0 | 1,000,000 | Satiety value |

### Container Statistics (OBJCLASS_CONTAINER)

#### Class Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Openable** | OPEN | 0 | 0 | 1 | Whether container can be opened/closed |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

#### Instance Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Locked** | LOCK | 0 | 0 | 0 | Current lock status |
| **KeyId** | KEY | 0 | 0 | 0 | Required key identifier |
| **PickDifficulty** | PICK | 0 | 0 | 0 | Lockpicking difficulty |

### Tool Statistics (OBJCLASS_TOOL)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |
| **Pick** | PICK | 0 | 0 | 100 | Lockpicking ability |

### Key Statistics (OBJCLASS_KEY)

#### Class Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

#### Instance Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **KeyId** | KEY | 0 | 0 | 1,000,000 | Unique key identifier |

### Money Statistics (OBJCLASS_MONEY)

#### Class Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Value** | VAL | 1 | 1 | 10,000 | Economic value |

#### Instance Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Amount** | AMT | 1 | 1 | 10,000 | Quantity of money |

### Ammo Statistics (OBJCLASS_AMMO)

#### Class Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **EqSlot** | EQSL | 8 | 0 | 10 | Equipment slot assignment |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |
| **Type** | TYPE | 1 | 0 | 4 | Ammunition type |

#### Instance Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Amount** | AMT | 0 | 0 | 1,000 | Quantity of ammunition |

### Ranged Weapon Statistics (OBJCLASS_RANGEDWEAPON)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **EqSlot** | EQSL | 6 | 0 | 8 | Equipment slot assignment |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

### Light Source Statistics (OBJCLASS_LIGHTSOURCE)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **EqSlot** | EQSL | 4 | 0 | 6 | Equipment slot assignment |
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

### Scroll Statistics (OBJCLASS_SCROLL)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Value** | VAL | 0 | 0 | 1,000,000 | Economic value |

### Talisman Statistics (OBJCLASS_TALISMAN)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Code** | CODE | 0 | 0 | 255 | Character code for spells |

### Tile Statistics (OBJCLASS_TILE)

| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Code** | CODE | -1 | -1 | -1 | Tile code |
| **Extra** | EXTR | -1 | -1 | -1 | Extra tile data |
| **Supertile** | SUPR | 0 | 0 | 1 | Supertile flag |
| **Width** | WDTH | 0 | 0 | 16 | Tile width |
| **Height** | HGHT | 0 | 0 | 16 | Tile height |

### Exit Statistics (OBJCLASS_EXIT)

#### Class Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Openable** | OPEN | 0 | 0 | 1 | Whether exit can be opened |
| **Facing** | FACE | 0 | 0 | 255 | Exit facing direction |
| **UseCenter** | USE | 0 | 0 | 2 | Use center flag |
| **StopMoving** | STMV | 0 | 1 | 1 | Stop movement flag |
| **Delay** | DLY | 0 | 0 | 1,000 | Transition delay |

#### Instance Statistics
| Statistic | ID | Default | Min | Max | Description |
|-----------|----|---------|-----|-----|-------------|
| **Locked** | LOCK | 0 | 0 | 1 | Exit lock status |
| **KeyId** | KEY | 0 | 0 | 100,000 | Required key identifier |
| **PickDifficulty** | PICK | 0 | 0 | 100,000 | Lockpicking difficulty |

## Statistics System Features

### Modifier System
Player statistics use a modifier system that converts raw values to gameplay modifiers:

```cpp
// Example modifier ranges for player stats
0-2:   -4 modifier
3-6:   -3 modifier  
7-9:   -2 modifier
10-11: -1 modifier
12-15:  0 modifier
16-17: +1 modifier
18-20: +2 modifier
21-24: +3 modifier
25-29: +4 modifier
30+:   +5 modifier
```

### Calculated Statistics
Some statistics are calculated from other statistics:

- **MaxHealth** = `Rules.healthperlevel * Level * (100 + classmod + constitution%) / 100`
- **MaxFatigue** = `Rules.fatigueperlevel * Level * (100 + classmod + constitution%) / 100`
- **MaxMana** = `Rules.manaperlevel * Level * (100 + classmod + mind%) / 100`

### Skill Experience System
Skills have both a current level and accumulated experience:
- **Skill Level**: Current skill value (0-100)
- **Skill Experience**: Hidden experience points (0-1,000,000)
- Experience is used to calculate skill progression and leveling

### Equipment Integration
Statistics integrate with the equipment system:
- **EqSlot**: Determines which equipment slot an item occupies
- **Protection**: Armor provides damage reduction
- **Damage**: Weapons provide base damage values
- **Combining**: Items can be combined for enhanced effects

### Locking System
Containers and exits use a key-lock matching system:
- **KeyId**: Unique identifier for keys and locks
- **Locked**: Current lock status
- **PickDifficulty**: Difficulty for lockpicking attempts

The statistics system provides a flexible foundation for character development, item properties, and gameplay mechanics throughout Revenant. 