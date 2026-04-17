# Object System Documentation

## Overview

Revenant's object system is a sophisticated class-based architecture that manages all game objects including characters, items, tiles, effects, and interactive elements. The system uses a **factory pattern** with builders, **class-based statistics**, and **hierarchical object management**.

## Summary

### Object System Architecture

Revenant's object system is built around **four core components**:

1. **TObjectClass** - Manages object types and statistics for a class
2. **TObjectBuilder** - Factory pattern for creating object instances
3. **TObjectInstance** - Individual game object instances
4. **SObjectInfo** - Shared metadata for object types

### Key Design Principles

- **Class-Based Organization** - Objects grouped by functionality (items, characters, tiles, etc.)
- **Factory Pattern** - Builders create specific object types
- **Shared Statistics** - Class-level and instance-level statistics
- **Memory Efficiency** - Optimized for thousands of objects
- **Unique Identification** - Each object type has a unique ID

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
    
    // Object type management
    TVirtualArray<SObjectInfo> objinfo;  // All object types in this class
    
    // Statistics system
    TStatisticDefList statdefs;     // Class-level statistics
    TStatisticDefList objstatdefs;  // Instance-level statistics
};
```

**Object Type Registration:**
```cpp
int32_t TObjectClass::AddType(char *name, char *imgfilename, uint32_t uniqueid)
{
    // Find appropriate builder for this object type
    TObjectBuilder* objbuilder = TObjectBuilder::GetBuilder(name);
    
    // Register imagery
    int32_t imageryid = TObjectImagery::RegisterImagery(imgfilename);
    
    // Create object info structure
    SObjectInfo* inf = new SObjectInfo(name, objbuilder, imageryid, uniqueid);
    
    // Add to class and initialize statistics
    int32_t objtype = objinfo.AddPtr(inf);
    
    return objtype;
}
```

### TObjectBuilder - Factory Pattern

**Purpose:** Creates specific object instances using the factory pattern.

**Builder Registration:**
```cpp
#define DEFINE_BUILDER(name, obj)                                               \
class obj##Builder : public TObjectBuilder                                      \
{                                                                               \
  public:                                                                       \
    obj##Builder() : TObjectBuilder(name) {}                                    \
    virtual TObjectInstance* Build(TObjectImagery* img)                         \
        { return new obj(img); }                                                \
    virtual TObjectInstance* Build(SObjectDef* def, TObjectImagery* img)        \
        { return new obj(def, img); }                                           \
};

#define REGISTER_BUILDER(obj) obj##Builder obj##BuilderInstance;
```

**Builder System:**
```cpp
class TObjectBuilder
{
  public:
    TObjectBuilder(const char *name);
    virtual TObjectInstance* Build(TObjectImagery* img) = 0;
    virtual TObjectInstance* Build(SObjectDef* def, TObjectImagery* img) = 0;
    
    static TObjectBuilder* GetBuilder(int32_t objtype);
    static TObjectBuilder* GetBuilder(const char *name);
    
  private:
    static int32_t numobjtypes;
    static TObjectBuilder* builders[MAXOBJECTTYPES];
    char *objtypename;
};
```

### SObjectInfo - Shared Metadata

**Purpose:** Stores shared information for all instances of an object type.

**Structure (Memory Optimized):**
```cpp
struct SObjectInfo
{   // HEY! KEEP ME SMALL! (Used by thousands of objects)
    char *name;                             // Object type name
    TObjectBuilder* objbuilder;             // Builder for this type
    int32_t imageryid;                      // Imagery resource ID
    TObjectImagery* imagery;                // Loaded imagery object
    TStatisticList stats, objstats;         // Class and instance statistics
    uint32_t uniqueid;                      // Unique type identifier
};
```

**Memory Optimization:** This structure is shared across all instances of an object type, so it's kept as small as possible to minimize memory usage.

### TObjectInstance - Individual Objects

**Purpose:** Represents individual game object instances in the world.

**Inheritance Structure:**
```cpp
class TObjectInstance : protected SObjectDef
{
    // Object identification
    SObjectInfo* inf;                // Pointer to shared object info
    TObjectClass* cl;                // Pointer to object class
    char *name;                      // Instance-specific name
    
    // Object state
    S3DPoint pos;                    // 3D position in world
    uint32_t flags;                  // Object flags
    uint16_t state;                  // Current state
    uint8_t group;                   // Group assignment
    
    // Systems
    TObjectAnimator* animator;       // Animation system
    TScript* script;                 // Scripting system
    TInventory inventory;            // Inventory system
    SLightDef lightdef;              // Lighting system
};
```

## Object Definition System

### SObjectDef - Object Creation Template

**Purpose:** Defines the initial state when creating new object instances.

**Structure:**
```cpp
class SObjectDef
{
  public:
    short       objclass;   // Class of item (-1 if no object)
    short       objtype;    // Type of item in class (-1 if no object)
    uint32_t    flags;      // Initial object flags
    
    uint16_t    state;      // Initial state
    uint16_t    level;      // Object level
    
    S3DPoint    pos;        // Initial position
    S3DPoint    vel;        // Initial velocity
    S3DPoint    accum;      // Movement accumulator
    
    uint8_t     rotatex;    // X-axis rotation (0-255)
    uint8_t     rotatey;    // Y-axis rotation (0-255)
    union
    {
        uint8_t rotatez;    // Z-axis rotation (0-255)
        uint8_t facing;     // Facing direction (0-255)
    };
    
    uint8_t     group;      // Group assignment
};
```

### Object Creation Process

**1. Object Definition:**
```cpp
SObjectDef def;
def.objclass = OBJCLASS_ITEM;
def.objtype = 5;  // Specific item type
def.pos = S3DPoint(100, 200, 0);
def.flags = OF_IMMOBILE;
```

**2. Class Object Creation:**
```cpp
TObjectInstance* TObjectClass::NewObject(SObjectDef* objectdef)
{
    // Validate object type
    if ((uint32_t)objectdef->objtype >= (uint32_t)objinfo.NumItems())
        return nullptr;
    
    // Load imagery
    objinfo[objectdef->objtype].imagery = 
        TObjectImagery::LoadImagery(objinfo[objectdef->objtype].imageryid);
    
    // Create instance using builder
    TObjectInstance* inst = objinfo[objectdef->objtype].objbuilder->Build(
        objectdef, objinfo[objectdef->objtype].imagery);
    
    return inst;
}
```

**3. Instance Construction:**
```cpp
TObjectInstance::TObjectInstance(SObjectDef* def, TObjectImagery* img)
{
    // Copy definition data
    memcpy(&objclass, def, sizeof(SObjectDef));
    
    // Set imagery and initialize systems
    imagery = img;
    cl = TObjectClass::GetClass(objclass);
    inf = cl->GetObjType(objtype);
    
    // Initialize statistics
    if (cl->NumObjStats() > 0)
    {
        stats.SetNumItems(cl->NumObjStats());
        for (int32_t c = 0; c < cl->NumObjStats(); c++)
            ResetObjStat(c);
    }
    
    // Initialize script (except for tiles)
    if (objclass != OBJCLASS_TILE)
        InitScript(ScriptManager.ObjectScript(this));
}
```

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

**Destructor Process:**
```cpp
TObjectInstance::~TObjectInstance()
{
    // Clean up inventory
    if (Inventory.GetContainer() == this)
        Inventory.SetContainer(nullptr);
    
    // Delete animator
    if (animator)
        delete animator;
    
    // Free imagery
    TObjectImagery::FreeImagery(imagery);
    
    // Clean up lighting
    if (lightdef.lightindex != -1)
        Scene3D.DeleteLight(lightdef.lightindex);
    
    // Recursively delete inventory contents
    for (TInventoryIterator i(this); i; i++)
    {
        i.Item()->RemoveFromInventory();
        delete i.Item();
    }
    
    // Remove from parent inventory
    RemoveFromInventory();
    
    // Remove from map
    if (GetSector() != nullptr)
        MapPane.RemoveObject(this);
    
    // Clean up name
    if (name && name != inf->name)
        free(name);
    
    // Cancel centering
    if (MapPane.GetCenterOnObj() == this)
        MapPane.CenterOnObj(nullptr, false);
    
    // Delete script
    if (script)
        delete script;
}
```

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

**1. Shared Metadata:** SObjectInfo structures are shared across all instances
**2. Small Instance Data:** TObjectInstance contains only instance-specific data
**3. Efficient Arrays:** TVirtualArray for sparse object type storage
**4. Lazy Loading:** Imagery loaded only when needed

**Memory Usage Notes:**
- **SObjectInfo** kept small (used by thousands of objects)
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
- Light definitions stored in SLightDef structure
- Integrated with 3D scene system

### Map System
- Objects placed in sectors for spatial organization
- Map system manages object visibility and interaction
- Objects automatically removed from map on destruction

The object system provides a robust foundation for all game entities, with efficient memory usage, flexible class organization, and comprehensive integration with other game systems. 