# Animation System in Revenant

> **Note:** This document covers the **retail 1998 animation system** (data
> formats, state machine, action blocks). The port reuses the retail data
> on disk and the retail decoder, but everything above the decoder has
> been replaced by a channel-based system. See
> [ANIMATION_SYSTEM.md](ANIMATION_SYSTEM.md) for the rewrite.

## Overview

Revenant features a sophisticated **3D skeletal animation system** with advanced character action management, animation blending, and transition systems. The system supports both **keyframe-based animation** and **motion data** for character movement, with a complex action state machine that handles combat, movement, and interactive animations.

## Core Animation Architecture

### Animation System Components

1. **T3DAnimator** - Base 3D animation system
2. **TCharAnimator** - Character-specific animation with equipment and effects
3. **TActionBlock** - Action state management and transitions
4. **Animation Data** - Keyframe and motion data structures
5. **Blending System** - Smooth transitions between animation states

## Animation Data Formats

### SAniKey - Animation Keyframe Structure

```cpp
struct SAniKey
{
    union
    {
        int32_t data[2];
        struct
        {
            unsigned int x : 12;    // Position X (-2048 to 2047)
            unsigned int y : 12;    // Position Y (-2048 to 2047)
            unsigned int z : 12;    // Position Z (-2048 to 2047)
            unsigned int rx : 9;    // Rotation X (-256 to 255)
            unsigned int ry : 9;    // Rotation Y (-256 to 255)
            unsigned int rz : 9;    // Rotation Z (-256 to 255)
            unsigned int dummy : 1; // Padding to 64 bits
        };
    };
};
```

**Key Features:**
- **Compressed Format**: 64-bit structure with bit-packed data
- **Position Range**: ±2048 units with 4.0 scale factor
- **Rotation Range**: ±256 units with 256/π scale factor
- **Macro Access**: Helper macros for safe data access

### SAniKey32 - Enhanced Animation Keyframe

```cpp
struct SAniKey32
{
    union
    {
        struct {
            uint32_t flags : 2;     // Structure type flags
            int32_t x : 10;         // Position X
            int32_t y : 10;         // Position Y  
            int32_t z : 10;         // Position Z
        };
        struct {
            uint32_t dummy : 2;     // Flags dummy
            uint32_t code : 6;      // Animation code
            int32_t value : 24;     // Value data
        };
    };
};
```

**Animation Codes:**
- **ANICODE32_POSX/POSY/POSZ** - Position components
- **ANICODE32_ROTX/ROTY/ROTZ** - Rotation components
- **ANICODE32_SCLX/SCLY/SCLZ** - Scale components

### SMotionData - Character Motion Data

```cpp
struct SMotionData
{
    unsigned int dist : 16;     // Distance forward on X,Y plane
    int vert : 16;              // Vertical movement on Z axis
    unsigned int ang : 8;       // Angle forward on X,Y plane
    unsigned int rotx : 8;      // Rotation X for object
    unsigned int roty : 8;      // Rotation Y for object
    unsigned int rotz : 8;      // Rotation Z for object (facing)
};
```

**Motion Components:**
- **dist**: Horizontal movement distance (0-65535)
- **vert**: Vertical movement for jumping/flying (-32768 to 32767)
- **ang**: Movement angle (0-255, 256 = 2π)
- **rotx/roty/rotz**: Object rotation angles

## Character Action System

### TActionBlock - Action State Management

```cpp
class TActionBlock
{
public:
    ACTION action;              // Action type (MOVE, COMBAT, ANIMATE, etc.)
    char name[RESNAMELEN];      // Animation state name
    int frame;                  // Starting frame number
    int wait;                   // Delay/wait time
    int angle, moveangle;       // Facing and movement angles
    S3DPoint target;            // Movement target location
    PTObjectInstance obj;       // Target object for action
    PSCharAttackData attack;    // Combat attack data
    PSCharAttackImpact impact;  // Combat impact data
    
    // State flags
    bool priority;              // High priority action
    bool interrupt;             // Can interrupt current action
    bool loop;                  // Looping animation
    bool stop;                  // Stop flag for movement
    bool terminating;           // Action is terminating
    bool firsttime;             // First frame of action
};
```

### Action Types

| Action | Purpose | Description |
|--------|---------|-------------|
| **ACTION_ANIMATE** | Basic animation | Simple looping animations |
| **ACTION_MOVE** | Character movement | Walking, running, sneaking |
| **ACTION_COMBAT** | Combat stance | Fighting animations |
| **ACTION_ATTACK** | Attack animations | Weapon attacks and strikes |
| **ACTION_IMPACT** | Hit reactions | Damage and impact animations |
| **ACTION_BLOCK** | Defensive actions | Blocking and parrying |
| **ACTION_DODGE** | Evasion | Dodging and evasion |
| **ACTION_DEAD** | Death animations | Dying and death states |
| **ACTION_INVOKE** | Spell casting | Magic and spell animations |
| **ACTION_BOW** | Ranged combat | Bow and arrow animations |

### Action State Machine

The character action system uses a **three-tier state machine**:

1. **Root State** - Base animation state (walk, run, sneak, combat)
2. **Desired State** - Next action to transition to
3. **Current State** - Currently executing action

**State Transitions:**
- **Immediate**: High priority actions interrupt current
- **Queued**: Actions wait for current to complete
- **Blended**: Smooth transitions between compatible states

## Animation Blending and Transitions

### Interpolation System

```cpp
#define INTERFRAMES 8

void InterpolatePoints(D3DVECTOR &v1, D3DVECTOR &v2, D3DVALUE &i)
{
    if (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z)
    {
        v1.x = v1.x * i + v2.x * ((D3DVALUE)1.0 - i);
        v1.y = v1.y * i + v2.y * ((D3DVALUE)1.0 - i);
        v1.z = v1.z * i + v2.z * ((D3DVALUE)1.0 - i);
    }
}
```

**Interpolation Features:**
- **8-Frame Interpolation**: Smooth transitions between keyframes
- **Linear Interpolation**: Position and rotation blending
- **Rotation Normalization**: Handles angle wrapping correctly

### Transition System

**Transition Types:**
1. **Immediate Transitions** - No blending, instant state change
2. **Blended Transitions** - Smooth interpolation between states
3. **Queued Transitions** - Actions wait in queue for execution

**Transition Rules:**
- **Priority Actions** - Combat and critical actions interrupt others
- **Compatible States** - Similar actions can blend smoothly
- **State Dependencies** - Some actions require specific base states

## Character Animation Features

### TCharAnimator - Advanced Character Animation

```cpp
class TCharAnimator : public T3DAnimator
{
protected:
    // Equipment rendering
    void ProcessEquipment(int32_t task);
    void HideCharParts();
    void RenderEquipment();
    
    // Visual effects
    void RenderShadow();
    void RenderCombatFlashes();
    void RenderBloodyChunks();
    
    // Transparency system
    void UpdateTransparency();
    void SetMaterialTransparency(PT3DImagery img);
    
    // Weapon effects
    TWeaponSwipe weaponswipe;
    void SetupWeaponSwipe();
};
```

### Equipment Integration

**Equipment Rendering Pipeline:**
1. **Hide Character Parts** - Conceal body parts covered by equipment
2. **Render Equipment** - Draw equipment with character's transform
3. **Transparency Handling** - Manage equipment transparency effects
4. **Weapon Effects** - Special effects for weapon animations

### Visual Effects System

**Character Effects:**
- **Shadows** - Dynamic character shadows
- **Combat Flashes** - Weapon impact and combat effects
- **Blood Effects** - Damage and injury visual feedback
- **Poison Effects** - Status effect visual indicators
- **Transparency** - Stealth and visibility effects

## Animation File Format Integration

### I3D File Animation Support

**Animation Flags:**
- **I3D_HASHIERARCHY** - Skeletal hierarchy support
- **I3D_FACINGMOTION** - Character facing motion data
- **I3D_ROOTMOTION** - Root motion and SAniKey motion data
- **I3D_ANIKEY32** - New compressed animation key format

**Animation Data Storage:**
- **Per-Object Animation** - Each mesh object has independent animation
- **State-Based Animation** - Multiple animation states per object
- **Motion Data** - Character movement and positioning data
- **Hierarchy Support** - Parent-child bone relationships

## Performance Optimizations

### Animation Optimizations

1. **Keyframe Compression** - Bit-packed animation data
2. **Interpolation Caching** - Pre-calculated interpolation values
3. **State Caching** - Cached animation state transitions
4. **LOD System** - Level-of-detail for distant characters

### Memory Management

**Animation Memory:**
- **Dynamic Allocation** - Animation data allocated per object
- **State Cleanup** - Automatic cleanup of completed states
- **Resource Sharing** - Shared animation data between instances

## Integration with Game Systems

### Combat System Integration

**Combat Animation Features:**
- **Attack Synchronization** - Coordinated attack and impact animations
- **Block Timing** - Precise blocking animation timing
- **Combat Stances** - Different animation sets for combat modes
- **Weapon Integration** - Weapon-specific animation support

### Movement System Integration

**Movement Animation Features:**
- **Directional Animation** - 8-directional movement animations
- **Speed-Based Animation** - Walk, run, sneak speed variations
- **Terrain Adaptation** - Animation adjustments for different surfaces
- **Transition Smoothing** - Smooth movement state transitions

### Scripting Integration

**Animation Scripting:**
- **Action Commands** - Script-driven animation triggers
- **State Management** - Script control of animation states
- **Timing Control** - Script-based animation timing
- **Event Integration** - Animation event callbacks

## Modern Port Considerations

### Animation System Modernization

**Potential Improvements:**
1. **GPU Animation** - Move animation calculations to GPU
2. **Advanced Blending** - Add additive and multiplicative blending
3. **Animation Trees** - Implement animation state trees
4. **Procedural Animation** - Add procedural animation support
5. **IK System** - Inverse kinematics for realistic movement

**Preservation Priorities:**
1. **Maintain Timing** - Preserve original animation timing
2. **State Machine Logic** - Keep action state machine intact
3. **Data Format Compatibility** - Ensure animation data compatibility
4. **Visual Fidelity** - Maintain original animation quality

## Summary

Revenant's animation system represents a sophisticated approach to character animation for its time, featuring:

- **Advanced State Management** - Complex action state machine
- **Smooth Transitions** - Interpolated animation blending
- **Equipment Integration** - Seamless equipment rendering
- **Combat Synchronization** - Coordinated combat animations
- **Performance Optimization** - Efficient animation data storage

The system provides a solid foundation for modern ports while maintaining the original game's animation quality and responsiveness. 