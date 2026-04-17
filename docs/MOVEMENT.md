# Movement, Pathfinding, and Navigation: The Art of Getting Stuck

## Overview

Revenant's movement system is a complex beast that combines grid-based walkmaps, 3D collision detection, character blocking, and pathfinding algorithms. It's sophisticated enough to handle the game's 2.5D isometric world, but has some... interesting quirks that players remember fondly (or not so fondly).

## The Core Movement System

### The Walkmap Foundation

At the heart of movement is the **walkmap system** - a grid-based height map that defines where characters can walk:

```cpp
// Grid-based walkmap with 16-unit resolution
#define GRIDSIZE 16
#define WALKMAPSHIFT 4  // 2^4 = 16
#define GRIDMASK (GRIDSIZE - 1)
```

**How it works:**
- Each sector contains a walkmap grid (32x32 tiles)
- Each grid cell stores the walkable height at that location
- Characters check this height to determine if they can move there
- The system prevents walking into walls, off cliffs, or into holes

### The Blocked() Function: Where Everything Goes Wrong

The infamous `Blocked()` function is where Locke's adventures in getting stuck begin:

```cpp
BOOL TCharacter::Blocked(S3DPoint &pos, S3DPoint &newpos, DWORD bits, int *height, PTCharacter *bychar)
{
    // Get walk map values in current area
    int mindelta, maxdelta;
    if (bits & MOVE_NOTMOVING)
    {
        *height = MapPane.GetWalkHeight(newpos);  // If not moving, just get what's under us
        maxdelta = mindelta = 0;
    }
    else
    {
        // If moving, check Radius() around us to see if changes (deltas) between walk
        // grids go to far up (maxdelta - walls, barriers), or too far down (mindelta -
        // holes, etc.)  Also returns height underneath us.
        MapPane.GetWalkHeightRadius(newpos, Radius(), mindelta, maxdelta, *height);
    }

    // Are we blocked? 
    BOOL blocked = !(bits & MOVE_FALLING) && 
        (abs(pos.z - *height) > MAXZMOVE || 
        abs(maxdelta) > MAXZMOVE || abs(mindelta) > MAXZMOVE ||
        *height == 0 || (!(aniflags & AF_FLY) && (*bychar = CharBlocking(this, newpos))));
         
    return blocked;
}
```

**The blocking conditions:**
1. **Height difference too large** (`abs(pos.z - *height) > MAXZMOVE`)
2. **Terrain too steep** (`abs(maxdelta) > MAXZMOVE` or `abs(mindelta) > MAXZMOVE`)
3. **No walkable surface** (`*height == 0`)
4. **Another character in the way** (`CharBlocking()`)

### The MAXZMOVE Problem

```cpp
#define MAXZMOVE 32
```

This constant defines the maximum height difference a character can traverse. At 32 units, it's quite restrictive - any step higher than 32 units is considered "blocked." This is why Locke gets stuck on:
- **Small rocks** that are just a bit too tall
- **Stairs** with steps larger than 32 units
- **Terrain transitions** that have height differences
- **Door thresholds** and other minor obstacles

## Character Movement: The Dance of Accumulators

### The Move() Function

Character movement uses an accumulator system for smooth motion:

```cpp
uint32_t TCharacter::Move()
{
    if (flags & OF_IMMOBILE || inventnum >= 0 || flags & OF_PARALIZE)
        return MOVE_NOTHING;                

    uint32_t retval = 0;

    // Falling stuff  
    int32_t h = MapPane.GetWalkHeight(pos);
    int32_t d = pos.z - h;
    if (d < -16)
        pos.z = h;
    else if (d > 16)
    {
        vel.z = max(vel.z - GRAVITY, -TERMINAL_VELOCITY);
        retval |= MOVE_FALLING;
    }
    if (d < 1)
        vel.z = 0;

    // Get nextmove
    S3DPoint nextmove;
    GetNextMove(nextmove);

    // Get movement and add to accumulator
    S3DPoint newaccum;
    if (movetopos)              // If movetopos is set, set newaccum to correct deltas
    {
        newaccum.x = (movepos.x - pos.x) * ROLLOVER;
        newaccum.y = (movepos.y - pos.y) * ROLLOVER;
        newaccum.z = (movepos.z - pos.z) * ROLLOVER;
        movepos.x = movepos.y = movepos.z = 0;
        movetopos = false;
    }
    else
    {
        newaccum = accum + nextmove + vel;
    }

    // Apply movement
    S3DPoint newpos = pos;
    rollover(newaccum.x, newpos.x);
    rollover(newaccum.y, newpos.y);
    rollover(newaccum.z, newpos.z);

    // Check if blocked
    if (Blocked(pos, newpos, retval))
        return MOVE_BLOCKED;

    SetPos(newpos);
    accum = newaccum;

    return MOVE_MOVED | retval;
}
```

**The accumulator system:**
- Movement is accumulated in fractional units
- `rollover()` converts accumulators to actual position changes
- This allows for smooth movement even with integer positions
- But it can also cause precision issues and getting stuck

## Pathfinding: The "Nudge" System

### The Reality: No Real Pathfinding

Revenant did **not** have proper pathfinding algorithms like A*, Dijkstra, or other sophisticated pathfinding systems. What it had was much more basic - a simple "nudge" system that tried to push characters around obstacles. This was typical for games of this era, long before the sophisticated AI pathfinding systems that would come later.

### The Infamous Nudge Algorithm

Revenant's "pathfinding" was really just obstacle avoidance through nudging:

```cpp
int TCharacter::GetNudgeAngle(int angle)
{
    int ch = MapPane.GetWalkHeight(pos);
    int nudge = 0;
    S3DPoint c;
    c.z = 0;

    // hum...need to make this keep some sort of static nudge
    // value in order to avoid the quivering affect.
    // possibly need to reset that when the target location
    // is changed, as well.
    // also - in the case of a coridor, need to make sure that
    // no matter what way they are facing it always sends them
    // straight down it.

    for (c.y = pos.y - 32; c.y <= (pos.y + 32); c.y += 16)
        for (c.x = pos.x - 32; c.x <= (pos.x + 32); c.x += 16)
        {
            if (c.y == pos.y && c.x == pos.x)
                continue;

            if (absval(MapPane.GetWalkHeight(c) - ch) < 30)
                continue;

            int dist = (sqr(64) - SQRDIST(c, pos)) / 100;
            dist = min(50, max(1, dist));

            int ang = ConvertToFacing(pos, c) - GetFace();
            int weight = (dist * (64 - absval(ang))) / 100;

            if (weight < 1)
                continue;

            if (ang > 0)
                nudge -= weight;
            else
                nudge += weight;
        }

    return angle;
}
```

**How the nudge works:**
1. **Scans surrounding area** (32 units in each direction)
2. **Finds height differences** that might be obstacles
3. **Calculates weight** based on distance and angle
4. **Applies nudge** to steer around obstacles

**The problems:**
- **Quivering effect** - characters shake when near obstacles
- **Static nudge values** - doesn't reset when target changes
- **Corridor issues** - doesn't always send characters straight down corridors
- **Oversensitive** - reacts to minor height differences
- **No path planning** - just reactive obstacle avoidance
- **Can't find alternate routes** - if direct path is blocked, character gets stuck

### The Era Context

This was **1998** - a time when:
- **A* pathfinding** was known in academic circles but not widely used in games
- **Behavior trees** were still in research phase
- **Walkmeshes** and navigation meshes were years away
- **Real-time strategy games** were just starting to use proper pathfinding
- **Most games** used simple obstacle avoidance or grid-based movement

Revenant's approach was typical for the era - a reactive system that tried to work around immediate obstacles rather than planning paths. It was only years later, working on games like **The Force Unleashed** at LucasArts, that more sophisticated systems like behavior trees, proper walkmeshes, and advanced pathfinding algorithms would become standard.

### What "Pathfinding" Really Meant

In Revenant, "pathfinding" consisted of:
- **Direct line movement** to target
- **Reactive nudging** when hitting obstacles
- **Character pushing** when blocked by other characters
- **Fallback teleporting** when completely stuck

There was no:
- **Path planning** or route calculation
- **Waypoint systems** or intermediate targets
- **Cost-based pathfinding** or terrain analysis
- **Dynamic pathfinding** or real-time route updates

## The "Getting Stuck" Chronicles

### Why Locke Gets Stuck on Everything

**1. The MAXZMOVE Restriction**
```cpp
#define MAXZMOVE 32
```
- Any step higher than 32 units = blocked
- This includes small rocks, stairs, door thresholds
- Very restrictive for a 3D world

**2. The Grid-Based Precision Problem**
- Movement is grid-aligned (16-unit resolution)
- Characters can get "trapped" between grid cells
- Accumulator precision issues cause getting stuck

**3. The Nudge System Overreaction**
- Characters constantly try to avoid obstacles
- Can cause them to get stuck in "nudge loops"
- The quivering effect makes movement feel unstable

**4. Character Blocking**
```cpp
PTCharacter TCharacter::CharBlocking(TCharacter *me, S3DPoint &pos, int radius)
{
    // Check if other characters are blocking movement
    // This can cause characters to get stuck behind each other
}
```

**5. The Walkmap Generation Issues**
```cpp
void TMapPane::CalculateWalkmap()
{
    // Auto-generates walkmap by raycasting from above
    // Can miss small obstacles or create incorrect heights
    // This leads to invisible barriers
}
```

### Common Stuck Scenarios

**The "Small Rock Trap":**
- Locke approaches a small rock (height > 32 units)
- `Blocked()` returns TRUE due to height difference
- Locke stops, can't move forward
- Nudge system tries to steer around, but rock is too close

**The "Corner Stuck":**
- Locke tries to turn a corner
- Grid alignment causes him to get caught on corner geometry
- Accumulator precision issues prevent smooth movement
- Character gets trapped between grid cells

**The "Character Traffic Jam":**
- Multiple characters in same area
- `CharBlocking()` prevents movement
- Characters get stuck behind each other
- No pathfinding to find alternate routes

**The "Invisible Wall":**
- Walkmap generation missed an obstacle
- Visual geometry doesn't match walkmap
- Locke appears to walk into empty space
- `Blocked()` returns TRUE due to height = 0

## The Mouse Movement System

### Click-to-Move Implementation

```cpp
void TMapPane::UpdateMouseMovement(int x, int y)
{
    if (!Player)
        return;

    S3DPoint curpos, target;
    Player->GetPos(curpos);

    ScreenToWorld(x + posx, y + posy, target, curpos.z + 50);
    int angle = ConvertToFacing(curpos, target);

    Player->SetFace(angle);
    Player->SetMoveToPos(target);
}
```

**How it works:**
1. **Screen to world conversion** - converts mouse click to 3D position
2. **Angle calculation** - determines which direction to face
3. **SetMoveToPos** - tells character to move to target
4. **Pathfinding** - character tries to find path to target

**The problems:**
- **No path validation** - doesn't check if path is actually possible
- **Direct line movement** - tries to walk in straight line to target
- **No obstacle avoidance** - relies on nudge system for obstacles
- **Can set impossible targets** - target might be unreachable

## The Sector System and Movement

### Sector Transfers

```cpp
int TMapPane::TransferObject(PTObjectInstance inst, int sx, int sy, int newsx, int newsy)
{
    LOCKSECTORS;  // Prevent update system from accessing sectors
    sectors[sx][sy]->RemoveObject(i.SectorIndex());
    sectors[newsx][newsy]->AddObject(inst);
    UNLOCKSECTORS;  // Allow update system to access sectors again
    return inst->GetMapIndex();
}
```

**Sector-based movement:**
- World divided into sectors (32x32 tiles each)
- Characters transfer between sectors as they move
- Sector boundaries can cause movement issues
- Object lists are managed per-sector

## The Fallback Systems

### When Pathfinding Fails

**The "Force Move" System:**
```cpp
if (movetopos)              // If movetopos is set, set newaccum to correct deltas
{
    newaccum.x = (movepos.x - pos.x) * ROLLOVER;
    newaccum.y = (movepos.y - pos.y) * ROLLOVER;
    newaccum.z = (movepos.z - pos.z) * ROLLOVER;
    movepos.x = movepos.y = movepos.z = 0;
    movetopos = false;
}
```

**The "Teleport" Fallback:**
- If character gets completely stuck
- System can force teleport to target
- Used as last resort for movement failures

## Performance Considerations

### Movement Optimization

**Sector-based culling:**
- Only check objects in current sector
- Reduces collision detection overhead
- But can miss objects at sector boundaries

**Walkmap caching:**
- Walkmap heights are cached per sector
- Reduces repeated height calculations
- But can cause issues with dynamic obstacles

**Character blocking optimization:**
- Only check nearby characters for blocking
- Uses radius-based culling
- But can miss blocking characters at edge cases

## The Legacy of Getting Stuck

### What Made It Memorable

**The Player Experience:**
- Locke getting stuck became a defining characteristic
- Players learned to work around the limitations
- Created memorable moments of frustration and humor
- Part of the game's "charm" (in retrospect)

**The Technical Lessons:**
- Grid-based movement has precision limitations
- Height-based collision detection is very restrictive
- Nudge systems can create more problems than they solve
- Mouse-based movement needs path validation

**The Design Impact:**
- Level designers had to work around movement limitations
- Some areas were designed to minimize getting stuck
- Players developed strategies to avoid problematic areas
- The system influenced level design decisions

## Modern Perspectives

### What Would Be Different Today

**Better Collision Detection:**
- Continuous collision detection instead of discrete
- More sophisticated height field algorithms
- Better handling of small obstacles

**Improved Pathfinding:**
- A* or similar algorithms instead of nudge system
- Path validation before movement
- Better obstacle avoidance

**Smoother Movement:**
- Sub-pixel movement instead of grid-based
- Better accumulator precision
- Reduced getting stuck scenarios

**Enhanced User Experience:**
- Visual feedback for blocked movement
- Alternative path suggestions
- Better error handling for impossible targets

Revenant's movement system was sophisticated for its time, combining grid-based walkmaps with 3D collision detection and character blocking. While it had its quirks (like Locke's tendency to get stuck on everything), it successfully handled the game's complex 2.5D world. The system's limitations became part of the game's character, creating memorable moments and teaching valuable lessons about movement system design. 