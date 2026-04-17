# Object Types in Revenant

## Overview

Revenant uses a class-based object system with 25 different object classes, each serving specific purposes in the game. Objects are organized by functionality and share common systems like statistics, inventory, and scripting.

## Complete Object Type Table

| Class ID | Class Name | Purpose | Key Features | Examples | Statistics |
|----------|------------|---------|--------------|----------|------------|
| **OBJCLASS_ITEM** | General Items | Basic inventory items | Generic item functionality | Miscellaneous objects | Value, Weight |
| **OBJCLASS_WEAPON** | Melee Weapons | Close combat weapons | Damage types, weapon skills, attack animations | Swords, axes, maces | EqSlot, Damage, WeaponType, Value |
| **OBJCLASS_ARMOR** | Protective Gear | Character protection | Damage reduction, equipment slots | Chest plates, boots, helmets | EqSlot, ArmorValue, Value |
| **OBJCLASS_TALISMAN** | Magic Items | Spell casting and magic effects | Elemental affinities, spell power | Sun, Moon, Death, Life talismans | Elemental stats, Magic power |
| **OBJCLASS_FOOD** | Consumable Items | Health and mana restoration | Healing, sustenance, poison cure | Meat, bread, apples, potions | Health, Mana, Fatigue, Poison, Cure, Fill |
| **OBJCLASS_CONTAINER** | Storage Objects | Item storage and organization | Locking, key requirements, inventory management | Chests, pouches, bookcases | Openable, Value, Locked, KeyId, PickDifficulty |
| **OBJCLASS_LIGHTSOURCE** | Illumination | Area lighting and visibility | Dynamic lighting, equipment slot | Lanterns, torches | EqSlot, Value |
| **OBJCLASS_TOOL** | Utility Items | Specialized functions | Lockpicking, crafting, interactions | Lockpicks, keys | Value, Pick (lockpicking ability) |
| **OBJCLASS_MONEY** | Currency | Economic transactions | Value storage, trading | Gold coins, gems | Value |
| **OBJCLASS_TILE** | Map Tiles | World construction | Walkmaps, collision detection, visual elements | Floor tiles, walls, decorations | Walkable, Height, Visual properties |
| **OBJCLASS_EXIT** | Level Transitions | Area connections | Level loading, teleportation | Doors, portals, stairs | Destination, Transition type |
| **OBJCLASS_PLAYER** | Player Character | Main protagonist | Full character system, inventory, skills | Main character | All character stats, skills, equipment |
| **OBJCLASS_CHARACTER** | NPCs & Monsters | Non-player entities | AI, combat, dialogue | Townspeople, monsters, enemies | Combat stats, AI behavior, dialogue |
| **OBJCLASS_TRAP** | Hazard Objects | Environmental dangers | Damage, status effects, triggers | Pressure plates, poison darts | Damage, Trigger type, Status effects |
| **OBJCLASS_SHADOW** | Shadow Effects | Visual effects | Dynamic shadows, lighting interaction | Character shadows, object shadows | Shadow properties |
| **OBJCLASS_HELPER** | Utility Objects | Development and debugging | Editor tools, reference objects | Boxes, axes, planes | Development properties |
| **OBJCLASS_KEY** | Locking Devices | Container access | Key-lock matching system | Golden Sun Key, door keys | Value, KeyId |
| **OBJCLASS_AMMO** | Projectile Items | Ranged weapon ammunition | Damage, quantity management | Arrows, lightning bolts | EqSlot, Value, Amount |
| **OBJCLASS_SCROLL** | Knowledge Items | Information and lore | Reading, spell learning | Books, scrolls, tomes | Value |
| **OBJCLASS_RANGEDWEAPON** | Distance Weapons | Ranged combat | Projectile firing, ammunition use | Bows, crossbows | EqSlot, Value |
| **OBJCLASS_EFFECT** | Visual Effects | Special effects | Particles, animations, temporary effects | Blood, spell effects, explosions | Effect properties, duration |

## Detailed Object Class Descriptions

### OBJCLASS_FOOD - Consumable Items

**Purpose:** Restore character health, mana, and fatigue while providing sustenance.

**Key Features:**
- **Health Restoration:** Restores character health points
- **Mana Restoration:** Restores character mana points  
- **Fatigue Recovery:** Reduces character fatigue
- **Poison Cure:** Can cure poison status effects
- **Single Use:** Most food items are consumed when used
- **Fill Effect:** Provides sustenance to prevent hunger

**Statistics:**
- `Health` - Health points restored
- `Mana` - Mana points restored
- `Fatigue` - Fatigue points restored
- `Poison` - Poison damage/healing
- `Cure` - Poison cure effect
- `Fill` - Satiety value
- `Value` - Economic value

**Examples:** Meat, Cheese, Apple, Bread, Grapes, Bottle, Fish, Mug, Watermelon, Celery

**Usage:** When used, food items restore the character's health and mana, with the item being consumed (state changes to 1).

### OBJCLASS_CONTAINER - Storage Objects

**Purpose:** Store and organize other objects, with optional locking mechanisms.

**Key Features:**
- **Item Storage:** Can hold multiple objects in inventory
- **Locking System:** Can be locked with specific keys
- **Lockpicking:** Can be picked with appropriate tools
- **Open/Close States:** Containers can be opened and closed
- **Inventory Management:** Integrates with character inventory system

**Statistics:**
- `Openable` - Whether container can be opened/closed
- `Value` - Economic value
- `Locked` - Current lock status (object stat)
- `KeyId` - Required key identifier (object stat)
- `PickDifficulty` - Lockpicking difficulty (object stat)

**Examples:** Chest, Pouch, Spell Pouch, Flask, Vial, Bookcase

**Usage:** Players can store items in containers, transfer items between containers and inventory, and use keys or lockpicks to access locked containers.

### OBJCLASS_WEAPON - Melee Weapons

**Purpose:** Primary combat tools for close-range fighting.

**Key Features:**
- **Damage Types:** Different weapon types (slash, pierce, blunt)
- **Weapon Skills:** Require specific skill levels to use effectively
- **Attack Animations:** Unique attack sequences and combos
- **Equipment Slots:** Occupy specific equipment positions
- **Combat Integration:** Work with character attack system

**Statistics:**
- `EqSlot` - Equipment slot assignment
- `Damage` - Base damage value
- `WeaponType` - Type of weapon (sword, axe, etc.)
- `Value` - Economic value

**Examples:** Various swords, axes, maces, daggers

**Usage:** Equipped in character's weapon slots and used in combat through the attack system.

### OBJCLASS_ARMOR - Protective Gear

**Purpose:** Reduce damage taken and provide character protection.

**Key Features:**
- **Damage Reduction:** Reduces incoming damage
- **Equipment Slots:** Specific body locations
- **Visual Representation:** Changes character appearance
- **Class Restrictions:** Some armor limited to specific character classes

**Statistics:**
- `EqSlot` - Equipment slot assignment
- `ArmorValue` - Damage reduction value
- `Value` - Economic value

**Examples:** Leather armor, chainmail, plate armor, boots, helmets

**Usage:** Equipped in character's armor slots to provide protection in combat.

### OBJCLASS_TALISMAN - Magic Items

**Purpose:** Enable spell casting and provide magical effects.

**Key Features:**
- **Elemental Affinities:** Associated with specific elements
- **Spell Power:** Enhance magical abilities
- **Magic System Integration:** Work with spell casting system
- **Character Stats:** Modify character attributes

**Statistics:**
- Elemental stats (Sun, Moon, Death, Life, etc.)
- Magic power modifiers
- Character attribute bonuses

**Examples:** Sun Talisman, Moon Talisman, Death Talisman, Life Talisman

**Usage:** Equipped to enable spell casting and provide magical bonuses.

### OBJCLASS_TILE - Map Tiles

**Purpose:** Construct the game world and provide collision detection.

**Key Features:**
- **Walkmaps:** Define walkable areas and obstacles
- **Visual Elements:** Provide world appearance
- **Collision Detection:** Block character movement
- **Height Information:** Define elevation changes
- **Sector Organization:** Grouped into map sectors

**Statistics:**
- `Walkable` - Whether characters can walk on tile
- `Height` - Elevation value
- Visual properties

**Examples:** Floor tiles, wall tiles, decorative elements

**Usage:** Placed in map sectors to create the game world geometry and navigation.

### OBJCLASS_CHARACTER - NPCs & Monsters

**Purpose:** Populate the world with intelligent entities.

**Key Features:**
- **AI System:** Autonomous behavior and decision making
- **Combat Abilities:** Attack patterns and combat AI
- **Dialogue System:** Conversation capabilities
- **Pathfinding:** Navigation through the world
- **Statistics:** Health, mana, fatigue, skills

**Statistics:**
- All character statistics (Health, Mana, Fatigue, etc.)
- Combat statistics (Attack, Defense, etc.)
- AI behavior parameters

**Examples:** Townspeople, guards, monsters, enemies

**Usage:** AI-controlled entities that interact with the player and world.

### OBJCLASS_PLAYER - Player Character

**Purpose:** Represent the main character controlled by the player.

**Key Features:**
- **Full Character System:** Complete statistics and abilities
- **Inventory Management:** Item storage and equipment
- **Skill System:** Character development and progression
- **Combat System:** Player-controlled fighting
- **Movement Control:** Direct player input handling

**Statistics:**
- All character statistics
- Skill levels
- Equipment and inventory

**Usage:** The main character that the player controls throughout the game.

### OBJCLASS_CONTAINER - Storage Objects (Detailed)

**Container States:**
- `CLOSED` (0) - Container is closed and locked
- `OPEN` (1) - Container is open and accessible

**Container Types:**
- **Regular Containers:** Basic storage (chests, pouches)
- **Liquid Containers:** Hold liquids (flasks, vials)
- **Special Containers:** Unique functionality (spell pouches)

**Locking System:**
- Containers can be locked with specific keys
- Lockpicks can be used to bypass locks
- Lockpicking success depends on tool quality and character skills

**Inventory Integration:**
- Items can be transferred between containers and character inventory
- Containers have limited storage capacity
- Items maintain their properties when stored

### OBJCLASS_EFFECT - Visual Effects

**Purpose:** Create temporary visual and gameplay effects.

**Key Features:**
- **Particle Systems:** Visual particle effects
- **Temporary Duration:** Effects that fade over time
- **Animation Integration:** Work with animation system
- **Environmental Effects:** Area of effect abilities

**Statistics:**
- Effect properties
- Duration values
- Visual parameters

**Examples:** Blood splatter, spell effects, explosions, environmental effects

**Usage:** Created by spells, combat, or environmental interactions to provide visual feedback.

## Object System Integration

### Statistics System
All objects use a unified statistics system with:
- **Class Statistics:** Shared across all instances of an object type
- **Instance Statistics:** Unique to individual object instances
- **Dynamic Modification:** Statistics can be changed during gameplay

### Inventory System
Objects can be stored in:
- **Character Inventory:** Player's personal storage
- **Container Inventories:** Storage objects like chests
- **World Placement:** Objects placed directly in the game world

### Scripting Integration
Objects can have associated scripts that:
- **Modify Behavior:** Change how objects function
- **Handle Events:** Respond to player interactions
- **Control Logic:** Implement complex object behaviors

### Animation System
Objects can have animations for:
- **Visual Effects:** Movement and appearance changes
- **State Changes:** Different visual states
- **Interaction Feedback:** Response to player actions

## Object Creation and Management

### Factory Pattern
Objects are created using a factory pattern:
- **TObjectBuilder:** Creates specific object instances
- **Class Registration:** Each object class registers its builder
- **Dynamic Creation:** Objects created at runtime

### Memory Management
- **Shared Metadata:** Object types share common information
- **Instance Data:** Individual objects store only unique data
- **Efficient Storage:** Optimized for thousands of objects

### File I/O
- **Save/Load System:** Objects can be saved and loaded
- **Version Compatibility:** Handles different file versions
- **Data Persistence:** Maintains object state across sessions

The object system provides a flexible and extensible foundation for all game entities, with each class serving specific purposes while sharing common infrastructure for statistics, inventory, scripting, and animation. 