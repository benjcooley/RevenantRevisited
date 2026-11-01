# Object System Documentation

## Overview

Revenant's object system manages every game object: characters, items,
tiles, effects and the interactive pieces. Since the 2026 object model
work (docs/OBJECT_MODEL_PLAN.md, phases 1-5) it is one reflected model: a
map object is a node that owns its object instance and its components,
its type is an archetype in the class's type table, and its state is a
plain record that every file format is a serialization of.

## Summary

### Object System Architecture

Revenant's object system is built around five things:

1. **TNode** - the thing in the map: name, uid, transform, children, components
2. **TObjectInstance** - the node's first component, the root of the class hierarchy
3. **TObjectClass** - a class of types with its statistics (CLASS.DEF)
4. **TObjectDef** - the archetype, one per type, in the class's type table
5. **SObjectState** - the record of one object's state, reflected, derived per class

### Key Design Principles

- **Class-Based Organization** - Objects grouped by functionality (items, characters, tiles, etc.)
- **Archetypes** - one `TObjectDef` per type makes, applies and captures instances
- **One Record** - `SObjectState` is the state; DEF text, the retail stream and the packed format only serialize it
- **Components** - everything optional or separable on an object is a `TComponent` on its node
- **Shared Statistics** - Class-level and instance-level statistics
- **Unique Identification** - Each object type has a unique ID; each node a 64-bit uid

### Object Types Overview

| Class | Purpose | Examples |
|-------|---------|----------|
| OBJCLASS_ITEM | Inventory items | Weapons, armor, tools |
| OBJCLASS_CHARACTER | NPCs and creatures | Monsters, townspeople |
| OBJCLASS_PLAYER | Player character | Main character |
| OBJCLASS_TILE | Map tiles | Floor, walls, decorations |
| OBJCLASS_EFFECT | Visual effects | Spells, particles |
| OBJCLASS_EXIT | Level transitions | Doors, portals |

## Core Architecture

### TObjectClass - Class Management

**Purpose:** Manages all object types within a specific class (e.g., all items, all characters).

**Key Responsibilities:**
- **Object Type Management** - Register and manage object types
- **Statistics System** - Define and manage class-level and instance-level statistics
- **Object Creation** - Create new object instances
- **File I/O** - Load and save class definitions

**Class Structure:**
```cpp
class TObjectClass
{
    // Class identification
    char *name;                     // Class name (e.g., "ITEM", "CHARACTER")
    int32_t id;                     // Class ID number
    uint16_t objflags;              // Class-wide flags
    TObjectClass* basedon;          // Inheritance (base class)
    
    // Object type management: the archetypes, by type index
    std::vector<std::unique_ptr<TObjectDef>> objinfo;
    
    // Statistics system
    TStatisticDefList statdefs;     // Class-level statistics
    TStatisticDefList objstatdefs;  // Instance-level statistics
};
```

**Object Type Registration:** `TObjectClass::AddType(name, imgfilename,
uniqueid)` asks the def registry (`TObjectDef::Make`) for the type name
first, then the class name, so a type with code of its own ("ArrowWallS",
"VialRack") gets its def and every other type of the class gets the
class's; registers the imagery; fills the def's name, imagery id, stats
and unique id; and appends it to the type table. The index is the
`objtype` every record names.

### TObjectDef - The Archetype

**Purpose:** Class information: one per CLASS.DEF type, owned by its
`TObjectClass`, reached from an instance through `Archetype()`. It
carries the type-level data retail kept in `SObjectInfo` (name, imagery,
the type's stats, the retail unique id), the type's default state, and
does the work in code (src/game/object/objectdef.h):

```cpp
class TObjectDef
{
    std::string      name;
    int32_t          imageryid;
    TObjectImagery*  imagery;
    TStatisticList   stats, objstats;
    uint32_t         uniqueid;

    virtual const SObjectState& DefaultState() const = 0;   // the type's default record
    virtual TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const = 0;
    virtual TObjectInstance* Create(TObjectImagery* img) const = 0;
    virtual void Apply(const SObjectState& state, TObjectInstance& inst) const;
    virtual void Capture(const TObjectInstance& inst, SObjectState& state) const;

    static void Register(const char* name, FMake make);     // SRegistrar at file scope
    static std::unique_ptr<TObjectDef> Make(const char* name);
};
```

`Create` makes an instance of the right C++ class (what the builders
did). `Apply` puts a record's values onto a live instance and its
components; `Capture` takes them back out. They are hand-written per
class, not generated: a class with state of its own derives
(`TCharacterDef : TStateObjectDef<SCharacterState>`,
`TPlayerDef : TCharacterDef`, `TExitDef`, `TScrollDef`, `TWeaponDef`,
`TDripDef`, `TMonsterGenDef`) and overrides the pair, calling the base's
first; a class without derives nothing and registers with
`REGISTER_OBJECT_DEF(TArmor)` or, under a type name,
`DEFINE_OBJECT_DEF("VIAL RACK", TVialRack)`. A subclass with only a
constructor of its own registers `MakeObjectDef<TPressPlate, TExitDef>`.

### TObjectInstance - Individual Objects

**Purpose:** Represents individual game object instances in the world.

**Structure** (since 2026-10-18, docs/OBJECT_MODEL_PLAN.md §3b): a map object is
a `TNode` (final: name, soft-ref id, TTransform, children, components, map
index, sector, level) whose first component is the `TObjectInstance`, the root
of the object class hierarchy. The node owns the object; `node->Object()` and
`instance->Node()` are cached pointers. The instance forwards the node-facing
API, so a `TObjectInstance*` still answers transform, map index, sector and
components. Delete an object with `DeleteObject(inst)`, own one with
`TObjectPtr<T>`.
```cpp
class TObjectInstance : public TComponent
{
    // Identity in the class system
    short objclass, objtype;         // Class / type indices
    SObjectInfo* inf;                // Pointer to shared object info
    TObjectClass* cl;                // Pointer to object class
    char *name;                      // Instance-specific name

    // State retail kept in the object header (reflected: props 15-19)
    uint32_t flags;                  // Object flags
    uint16_t state;                  // Current state
    uint16_t level;                  // Map level
    S3DPoint vel, accum;             // Movement integrator
    uint8_t group;                   // Group assignment

    // The record it was spawned or loaded with (Reset re-applies it)
    std::unique_ptr<SObjectState> initial_state_;

    // Components live in the node's slots: stats_, script_, inventory_
    // are kept for life; the light and the visual through TSafeComponentRef
};
```

The transform is the only spatial store. Code written against retail's
integer world reads and writes it through the free functions in
`intworld.h`: `IntPos(oi)`, `SetIntPos(oi, p)`, `IntFacing(oi)`,
`SetIntFacing(oi, f)`, `IntRotateX/Y`. `SetPos` remains the map move with
its sector, walkmap, shadow and light side effects.

## Object State and Serialization

### The rule

**An object's state is its record.** `SObjectState` (objectdef.h) is the
reflected record of one object: what a spawn site fills, what a file
stores, what an instance keeps as the state it was loaded or spawned in.
Every file format is a serialization of it and nothing else:

- the retail stream (sector `.DAT` files, `game.sav`): `TLegacyDeserialize`
  reads bytes into a record and hands it to `Archetype()->Apply`;
  `TLegacySerialize` has `Capture` fill a record and writes its bytes
  (src/game/object/legacystream.{h,cpp}); the fixed-flag and version
  rules live in the codec, not on the objects;
- DEF text and the packed binary: the reflection system's codecs
  (src/engine/refl/defio.cpp, packed.cpp) over the same records;
- the editor: shows the record, edits it, applies it.

No object class has `Load`, `Save` or `ObjVersion`; nothing is streamed
from a live object. The instance keeps its record: `InitialState()` is
what it was spawned or loaded with, `Reset()` applies it again,
`CaptureState()` fills a fresh one.

### SObjectState - The Record

```cpp
struct SObjectState            // ids 1-99 are the base's
{
    REFL_IDS(1, 99)
    short    objclass, objtype;       // 1, 2
    uint32_t flags;  uint16_t state, level;
    S3DPoint pos, vel, accum;  uint8_t rotatex, rotatey, rotatez (facing);
    uint8_t  group;
    TUid     uid;                     // 13: the node's identity
    std::string name;                 // 14: the instance's own, if any
    int32_t  inventnum, invindex;     // 15, 16: the slot it sits in
    int32_t  shadow, mapindex;        // 17, 18
    int32_t  frame, framerate;        // 19, 20: the playhead
    std::vector<SObjectStatState> stats;   // 21: object stats by unique id
    SLightState light;                // 22: the light, when OF_LIGHT

    virtual std::unique_ptr<SObjectState> Clone() const;
};
```

A derived record takes its own hundred so the base can grow
(src/game/object/objectstates.h): `SComplexObjectState` 100s,
`SCharacterState` 200s (recovery clocks, poison, the teleport point),
`SExitState` 300s, `SScrollState` 400s, `SWeaponState` 500s,
`SDripState` 600s, `SMonsterGenState` 700s, `SPlayerState` 800s (the
quick spells, the talisman codes, the HUD modes, the level-up stats, the
multiplayer fields, the module, the play clock, the explored automap).
Each holds exactly the field set its retail stream carries, so the
sector and save formats barely changed; what changed is that every
field has an id.

### Object Creation

```cpp
SObjectState def;
def.objclass = OBJCLASS_ITEM;
def.objtype  = 5;                       // the record's default is -1: no type
def.pos      = S3DPoint(100, 200, 0);
TObjectInstance* inst = cl->NewObject(&def);
```

`TObjectClass::NewObject` loads the type's imagery and calls its def's
`Create`. The constructor adopts a node, sets `inf` to the archetype,
clones the record as `initial_state_` and has the archetype `Apply` it;
the class constructors (`TCharacter::ClearChar`, `TPlayer::ClearPlayer`)
add the components every object of the class has.

### Components

Everything optional or separable on an object is a `TComponent` on its
node, with annotated properties the inspector shows. Those the record
carries are applied and captured by the def; the rest (marked *between
ticks*) retail never saved and the port doesn't either.

| Component | On | Reached by | Holds |
|---|---|---|---|
| `TLightComponent` | any, when OF_LIGHT | `Light()` / `EnsureLight()` | the light (record: `light`) |
| `T2DVisualComponent` / `T3DVisualComponent` | any | `Visual()` | nothing persisted: the imagery's draw |
| `TStatsComponent` | any with object stats | `Stats()` | the object stats (record: `stats`) |
| `TScriptComponent` | any scripted | `GetScript()` | the bound script |
| `TInventoryComponent` | containers | `Contents()` | the slots (record: the nested items) |
| `TVitalsComponent` | characters | `Vitals()` | recovery clocks (record), accumulators (between ticks), poison, the teleport point |
| `TAIComponent` | characters | `Brain()` | attack timers and requests, target memory, waypoints, wander, retreat, boss state, has-seen, auto-combat, glimpse and noise (between ticks) |
| `TCombatComponent` | characters | `Combat()` | the last attack, chain, button, bow, combat flash, snap time, invoke delay, magic resistance (between ticks) |
| `TFadeComponent` | characters | `Fader()` | the fade and the invisibility spell (between ticks) |
| `TEquipmentComponent` | players | `Equipment()` | the equip slots (rebuilt from the inventory on load) |
| `TSpellbookComponent` | players | `Spellbook()` | talisman codes, quick spells (record) |
| `TStatEffectsComponent` | players | `Effects()` | stat effects in force, their expiry, the modified stats |
| `TAutomapComponent` | players | `Automap()` | the explored automap (record) |

What stays on the object: the playhead (`frame`, `framerate`) and
`state`, since an object without an animator still has them; the action
blocks and `commanddone` (the complex object's); velocity and the
accumulator (a motion component is deferred); the spell manager; the
burn, the voice, the corpse clock. An exit has no component: its state
is one flags word in `SExitState`, and its destination is the exit list's
entry by name.

### The Gate

`--test=state-roundtrip --level=L` (tools/state_gate.sh over every
level) puts every object of a level and the new game's player through
`Capture` -> `Apply` -> `Capture` and asserts the records match.
`--test=sector-dump` and `--test=player-dump` prove the retail bytes
against a baseline binary. Run them before pushing a change to any
record, def or component.

## Object Flags System

### Core Object Flags

**Movement and Physics:**
```cpp
#define OF_IMMOBILE      (1<<0)     // Not affected by gravity
#define OF_MOVING        (1<<3)     // Moving object (characters, missiles)
#define OF_AI            (1<<5)     // Has AI system
#define OF_DISABLED      (1<<6)     // AI is disabled
```

**Visibility and Rendering:**
```cpp
#define OF_INVISIBLE     (1<<7)     // Not visible during play
#define OF_EDITOR        (1<<8)     // Editor-only object
#define OF_DRAWFLIP      (1<<9)     // Horizontal flip
#define OF_LIGHT         (1<<2)     // Generates light
```

**Animation and Behavior:**
```cpp
#define OF_ANIMATING     (1<<4)     // Has animator
#define OF_EDITORLOCK    (1<<1)     // Locked in editor
```

### Movement System

**Movement Return Values:**
```cpp
#define MOVE_NOTHING    0           // No movement
#define MOVE_MOVED      (1 << 0)    // Position changed
#define MOVE_BLOCKED    (1 << 1)    // Hit obstacle
#define MOVE_FALLING    (1 << 2)    // Falling due to gravity
#define MOVE_NOTMOVING  (1 << 3)    // No current velocity
```

## Statistics System

### Two-Level Statistics

**Class-Level Statistics:** Shared across all instances of an object type
**Instance-Level Statistics:** Unique to each object instance

**Statistics Definition:**
```cpp
struct SStatisticDef
{
    char name[MAXNAMELEN];          // Statistic name
    int32_t def, min, max;          // Default, minimum, maximum values
    uint32_t uniqueid;              // Unique identifier
};
```

**Statistics Management:**
```cpp
// Add class-level statistic
int32_t TObjectClass::AddStat(SStatisticDef &newstatdef, int32_t newid)
{
    int32_t statid = statdefs.AddStat(newstatdef, newid);
    
    // Initialize for all existing object types
    for (int32_t c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].stats.New();
        objinfo[c].stats.Set(statid, newstatdef.def);
    }
    
    return statid;
}
```

## Object Lifecycle

### Creation and Initialization

**1. Object Definition Creation**
**2. Class Object Creation**
**3. Instance Construction**
**4. System Initialization (Script, Animator, etc.)**
**5. Map Integration**

### Destruction and Cleanup

**Destructor Process:** the node owns the object (`DeleteObject(inst)`,
never `delete inst`). The object's destructor deletes its inventory items
first (they are its contents, held by the inventory component), then
detaches its components, frees the imagery, removes itself from the map
and the parent inventory, and cancels any centering on it.

## Inventory System

### TInventoryIterator - Inventory Traversal

**Purpose:** Provides safe iteration through object inventories.

**Usage:**
```cpp
class TInventoryIterator
{
  public:
    TInventoryIterator(TObjectInstance* own);
    TObjectInstance* Item() const;
    int32_t InvIndex() const;
    TObjectInstance* NextItem();
    
    // Operator overloads for easy iteration
    bool operator ++ (int32_t);
    operator TObjectInstance* () const;
    TObjectInstance* operator -> () const;
    RTObjectInstance operator * () const;
    
  protected:
    TObjectInstance* owner;     // Inventory parent
    int32_t invindex;           // Current index
    TObjectInstance* item;      // Current item
};
```

**Example Usage:**
```cpp
// Iterate through all items in inventory
for (TInventoryIterator i(this); i; i++)
{
    TObjectInstance* item = i.Item();
    // Process item...
}
```

## Object Classes

### Predefined Object Classes

**Core Game Classes:**
```cpp
enum
{
    OBJCLASS_ITEM,           // General items
    OBJCLASS_WEAPON,         // Weapons
    OBJCLASS_ARMOR,          // Armor
    OBJCLASS_TALISMAN,       // Magical items
    OBJCLASS_FOOD,           // Food items
    OBJCLASS_CONTAINER,      // Containers
    OBJCLASS_LIGHTSOURCE,    // Light sources
    OBJCLASS_TOOL,           // Tools
    OBJCLASS_MONEY,          // Currency
    OBJCLASS_TILE,           // Map tiles
    OBJCLASS_EXIT,           // Level exits
    OBJCLASS_PLAYER,         // Player character
    OBJCLASS_CHARACTER,      // NPCs and creatures
    OBJCLASS_TRAP,           // Traps
    OBJCLASS_SHADOW,         // Shadows
    OBJCLASS_HELPER,         // Helper objects
    OBJCLASS_KEY,            // Keys
    OBJCLASS_AMMO,           // Ammunition
    OBJCLASS_SCROLL,         // Scrolls
    OBJCLASS_RANGEDWEAPON,   // Ranged weapons
    OBJCLASS_EFFECT,         // Visual effects
};
```

## Memory Management

### Optimization Strategies

**1. Shared Metadata:** one TObjectDef per type, shared by all its instances
**2. Small Instance Data:** TObjectInstance contains only instance-specific data; what a class doesn't need is a component it doesn't have
**3. Lazy Loading:** Imagery loaded only when needed

**Memory Usage Notes:**
- **TObjectDef** is per type, not per instance (the type table is the registry)
- **TObjectInstance** contains only instance-specific data
- **Statistics** stored efficiently with default values
- **Imagery** shared across instances of same type

## Integration with Other Systems

### Scripting System
- Objects can have associated scripts
- Scripts initialized during object creation
- Scripts can modify object behavior and statistics

### Animation System
- Objects can have animators for visual effects
- Animators handle state changes and frame updates
- Integrated with imagery system

### Lighting System
- Objects can generate light (OF_LIGHT flag)
- The light is a TLightComponent, there exactly when the flag is set; the record's `light` field is its serialization
- Integrated with 3D scene system

### Map System
- Objects placed in sectors for spatial organization
- Map system manages object visibility and interaction
- Objects automatically removed from map on destruction

The object system provides a robust foundation for all game entities, with efficient memory usage, flexible class organization, and comprehensive integration with other game systems. 