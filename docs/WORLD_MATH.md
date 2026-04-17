# World Mathematics Documentation

## Overview

Revenant uses a sophisticated hybrid 2.5D coordinate system that combines isometric 2D tile-based rendering with full 3D character models. This document describes the mathematical foundations, coordinate transformations, and modern approaches for the game's spatial systems.

## Summary

### Coordinate System Architecture

Revenant's mathematical system is built around **four coordinate spaces**:

1. **World Space** - Global 3D coordinates for game logic
2. **Screen Space** - 2D pixel coordinates for rendering
3. **Walkmap Space** - Height-based collision coordinates
4. **Object Space** - Local coordinates for 3D models

### Key Mathematical Principles

- **Isometric Projection** - 2:1 ratio with 30° tilt for 2.5D appearance
- **Y-Up Coordinate System** - DirectX-style right-handed coordinates
- **Hybrid Rendering** - 2D tiles with 3D objects and Z-buffer integration
- **Height-Based Collision** - Walkmap system for terrain and obstacles

### Mathematical Systems Overview

| System | Purpose | Key Features |
|--------|---------|--------------|
| **Coordinate Conversion** | World ↔ Screen transforms | Isometric projection, Z-depth calculation |
| **Transform System** | Object positioning and rotation | Matrix-based, hierarchical transforms |
| **Collision Detection** | Walkmap and object collision | Height-based, spatial partitioning |
| **Camera System** | View and projection management | Fixed isometric, depth-aware rendering |

## Core Concepts

### Isometric Projection System

Revenant uses a **fixed isometric projection** that creates the characteristic 2.5D appearance:

**Projection Characteristics:**
- **2:1 Ratio** - Screen coordinates use 2:1 aspect ratio for isometric tiles
- **30° Tilt** - Camera tilted 30° from horizontal for depth perception
- **45° Rotation** - View rotated 45° around Z-axis for diamond-shaped tiles
- **Fixed Distance** - Camera positioned at constant distance from scene

**Mathematical Constants:**
```cpp
#define MYSTERYVAL1     (867)    // cos(30°) * 1000
#define MYSTERYVAL2     (1000)   // Scale factor
#define WORLDZOFFSET    0x3900 + 0x7FFF  // Base Z offset
```

### Coordinate Spaces

**World Space (3D)**
- **X-axis**: Left to right (positive)
- **Y-axis**: Forward to back (positive) 
- **Z-axis**: Up is positive (Y-up model)
- **Units**: 64 units per meter (UNITSPERMETER)

**Screen Space (2D)**
- **X-axis**: Left to right in pixels
- **Y-axis**: Top to bottom in pixels
- **Z-depth**: 16-bit depth buffer values
- **Origin**: Top-left corner of screen

**Walkmap Space**
- **Granularity**: 16x16 pixel tiles (WALKMAPGRANULARITY)
- **Height Data**: 8-bit height values per tile
- **Collision**: Height-based obstacle detection
- **Pathfinding**: Grid-based navigation

### Z-Depth System

The Z-depth calculation accounts for both **height and position**:

```cpp
void WorldToScreenZ(const S3DPoint& pos, int32_t &z)
{
    z = (uint16_t)(WORLDZOFFSET - (int32_t)(pos.z / 2) - 
                   (int32_t)((pos.x + pos.y) * MYSTERYVAL1 / MYSTERYVAL2));
}
```

**Components:**
- **WORLDZOFFSET**: Base depth offset (0x3900 + 0x7FFF)
- **pos.z / 2**: Height contribution to depth
- **(pos.x + pos.y) * 0.867**: Position-based depth adjustment

This ensures **proper depth ordering** between 2D tiles and 3D objects.

## Coordinate Transformations

### World to Screen Conversion

**Primary Conversion Function:**
```cpp
void WorldToScreen(const S3DPoint& pos, int32_t &x, int32_t &y)
{
    x = pos.x - pos.y;  // Isometric X projection
    y = ((pos.x + pos.y) / 2) - (int32_t)((pos.z * 867) / 1000);  // Isometric Y with Z offset
}
```

**Full 3D Conversion:**
```cpp
void WorldToScreen(const S3DPoint& pos, S3DPoint& spos)
{
    spos.x = pos.x - pos.y;
    spos.y = ((pos.x + pos.y) / 2) - (int32_t)((pos.z * 867) / 1000);
    spos.z = (uint16_t)(WORLDZOFFSET - (int32_t)(pos.z / 2) - 
                       (int32_t)((pos.x + pos.y) * 867 / 1000));
}
```

**Mathematical Basis:**
- **Isometric X**: `x = world.x - world.y` (45° rotation)
- **Isometric Y**: `y = (world.x + world.y) / 2 - world.z * cos(30°)`
- **Z-depth**: Complex formula for proper depth ordering

### Screen to World Conversion

**Basic Conversion:**
```cpp
void ScreenToWorld(int32_t x, int32_t y, S3DPoint& pos, int32_t zheight)
{
    if (zheight)
        y += (int32_t)((zheight * 867) / 1000);  // Adjust for height

    pos.x = (x / 2) + y;  // Reverse isometric X
    pos.y = y - (x / 2);  // Reverse isometric Y
    pos.z = zheight;      // Set height
}
```

**Full 3D Conversion:**
```cpp
void ScreenToWorld(S3DPoint& pos, const S3DPoint& spos)
{
    pos.z = (-3464 * spos.y / 1000 - 2 * spos.z) / 4;
    pos.y = -spos.x / 2 + 867 * pos.z / 1000 + spos.y;
    pos.x = spos.x + pos.y;
}
```

### Height Mapping System

**Height Conversion Macros:**
```cpp
#define FIX_Z_VALUE(zmap) ((float)zmap / ((float)1.46))
#define REV_FIX_Z_VALUE(z3d) ((float)z3d * (float)1.46)
```

**Walkmap Integration:**
- **Tile Heights**: 8-bit values stored in walkmap data
- **Height Scaling**: 1.46x factor between walkmap and world coordinates
- **Collision Detection**: Height-based obstacle avoidance
- **Pathfinding**: Height-aware navigation algorithms

## Transform System

### Matrix-Based Transforms

**DirectX-Style Matrices:**
```cpp
static D3DMATRIX proj = {
    D3DVAL(1.0/65536.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0/65536.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0/65536.0/ZSCALE), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};
```

**Camera Transform Sequence:**
1. **Translation**: Move camera to world position
2. **Z-Rotation**: 45° rotation around Z-axis
3. **X-Rotation**: -(90° + CAMERAANGLE) tilt from horizontal
4. **Distance**: Move back by camera distance

### Object Transformations

**3D Object Positioning:**
```cpp
// Scale object to world units
obj->pos.X = -(float)((((float)width / 2.0) - (float)image->GetWorldRegX(state)) * (float)GRIDSIZE);
obj->pos.Y = -(float)((((float)length / 2.0) - (float)image->GetWorldRegY(state)) * (float)GRIDSIZE);
obj->pos.Z = -(float)((((float)height / 2.0) - (float)image->GetWorldRegZ(state)) * (float)GRIDSIZE);
```

**Transform Composition:**
- **Scale**: Object size in world units
- **Rotation**: Object orientation
- **Translation**: World position
- **Matrix Multiplication**: Combine all transformations

## Angle and Distance System

### Precomputed Tables

**Distance and Angle Tables:**
```cpp
extern uint8_t AngleTable[256][256];   // Precomputed angles
extern uint8_t DistTable[256][256];    // Precomputed distances
extern short DistX[256];               // X components for angles
extern short DistY[256];               // Y components for angles
```

**Table Generation:**
```cpp
// Generate distance and angle tables
for (short y = 0; y < 256; y++) {
    for(short x = 0; x < 256; x++) {
        dist = Round(sqrt((double)x * (double)x + (double)y * (double)y));
        if (dist >= 255)
            DistTable[x][y] = (uint8_t)255;
        else
            DistTable[x][y] = dist;
        
        if (x == 0)
            ang = 64;  // Special case for vertical
        else
            ang = (uint8_t)Round((atan2l(y, x) / M_2PI) * 256.0);
        
        AngleTable[x][y] = ang;
    }
}
```

### Vector Operations

**Angle to Vector Conversion:**
```cpp
void ConvertToVector(int32_t angle, int32_t speed, S3DPoint& vect, int32_t zangle)
{
    if (zangle == 0) {
        vect.x = (DistX[angle] * speed) / 256;
        vect.y = (DistY[angle > 128 ? angle - 128 : 128 - angle] * speed) / 256;
        vect.z = 0;
    } else {
        ConvertToVector(angle, (DistX[zangle] * speed) / 256, vect, 0);
        vect.z = (DistY[zangle] * speed) / 256;
    }
}
```

**Facing Calculation:**
```cpp
int32_t ConvertToFacing(const S3DPoint& target)
{
    int32_t absx = absval(target.x);
    int32_t absy = absval(target.y);

    while (absx > 255 || absy > 255) {
        absx >>= 1;
        absy >>= 1;
    }

    int32_t angle = 64 - AngleTable[absx][absy];

    // Adjust for quadrant
    if (target.x < 0 && target.y > 0)
        angle += 128;
    else if (target.x < 0)
        angle = 256 - angle;
    else if (target.y > 0)
        angle = 128 - angle;

    return angle;
}
```

**Distance Calculation:**
```cpp
int32_t Distance(const S3DPoint& pos)
{
    int32_t numshifts = 0;
    int32_t x = abs(pos.x);
    int32_t y = abs(pos.y);

    while (x > 255 || y > 255) {
        x >>= 1;
        y >>= 1;
        numshifts++;
    }

    int32_t d = (int32_t)DistTable[x][y];
    d <<= numshifts;

    return d;
}
```

## Camera System

### Fixed Isometric Camera

**Camera Setup:**
```cpp
void SetCameraPos(S3DPoint pos, int32_t zdist)
{
    // Move camera to world position
    D3DMATRIXMove(&view, &cameraPos);
    
    // Apply isometric rotation
    D3DMATRIXRotateZ(&view, 45.0 * TORADIAN);  // 45° Z rotation
    D3DMATRIXRotateX(&view, -(90.0 + CAMERAANGLE) * TORADIAN);  // Tilt down
    
    // Move back by camera distance
    D3DMATRIXMove(&view, &distanceVector);
}
```

**Camera Characteristics:**
- **Fixed Angle**: 45° rotation + 30° tilt
- **Constant Distance**: Camera always same distance from scene
- **No Zoom**: Fixed perspective for consistent gameplay
- **Depth Aware**: Z-buffer integration for proper occlusion

### Viewport Management

**Viewport Setup:**
```cpp
// 3D units per pixel calculation
float unitsperpixel = (float)512.0 / (float)sqrt(256.0 * 256.0 + 256.0 * 256.0);

D3DVIEWPORT viewData;
viewData.dvScaleX = 65536.0 * unitsperpixel;
viewData.dvScaleY = 65536.0 * unitsperpixel;
```

**Z-Buffer Integration:**
- **Background Z-Buffer**: Tiles set initial depth values
- **3D Object Z-Buffer**: Models update depth during rendering
- **Z-Buffer Restoration**: Areas behind objects restored for next frame

## Collision Detection

### Walkmap System

**Walkmap Characteristics:**
- **16x16 Granularity**: WALKMAPGRANULARITY = 16
- **Height Data**: 8-bit height values per tile
- **Collision Detection**: Height-based obstacle avoidance
- **Pathfinding**: Grid-based navigation algorithms

**Walkmap Integration:**
```cpp
// Walkmap coordinate conversion
#define WALKMAPSHIFT 4
#define WALKMAPGRANULARITY (1 << WALKMAPSHIFT)

// Height constants
#define WALKMAPCONST 1
#define ZPOSCONST 1
```

### Spatial Partitioning

**Sector System:**
- **Sector Size**: 1024x1024 world units
- **Sector Grid**: 32x32 sectors maximum
- **Dynamic Loading**: Sectors loaded/unloaded based on player position
- **Object Management**: Objects organized by sector

**Collision Optimization:**
- **Spatial Hashing**: Fast object lookup by position
- **AABB Testing**: Axis-aligned bounding box collision
- **Height Testing**: Z-axis collision detection
- **Line-of-Sight**: Ray casting for visibility

## Modern Mathematical Approaches

### Notable Omissions from Revenant

**Quaternion-Based Rotations**
- **Current System**: Euler angles with potential gimbal lock issues
- **Missing**: Smooth rotation interpolation for camera transitions and complex animations
- **Impact**: Limited camera movement and animation complexity

**Generic Matrix System**
- **Current System**: DirectX-specific D3DMATRIX structures
- **Missing**: Platform-agnostic matrix operations for cross-platform compatibility
- **Impact**: Tight coupling to DirectX, difficult to port to other graphics APIs

**Spatial Data Structures**
- **Current System**: Basic sector-based spatial partitioning
- **Missing**: Hierarchical spatial structures for efficient collision detection
- **Impact**: Performance limitations with large numbers of objects

### Beneficial Additions

**SIMD Vectorization**
- **Current**: Scalar mathematical operations
- **Benefit**: 4x performance improvement for matrix and vector operations
- **Implementation**: SSE/AVX instructions for modern processors

**Interpolation System**
- **Current**: Linear interpolation only
- **Benefit**: Smooth camera transitions and animation blending
- **Implementation**: Spherical interpolation (SLERP) for rotations, easing functions for motion

**Precision Management**
- **Current**: Fixed precision with potential overflow issues
- **Benefit**: Adaptive precision for different calculation contexts
- **Implementation**: Context-aware switching between single/double precision

### Cross-Platform Considerations

**Graphics API Abstraction**
- **Current**: DirectX-specific mathematical operations
- **Benefit**: Works with OpenGL, Vulkan, Metal, or any graphics API
- **Implementation**: Standard 4x4 matrices with platform-specific optimizations

**Memory Optimization**
- **Current**: Basic memory layout
- **Benefit**: Cache-friendly data organization and reduced memory footprint
- **Implementation**: Structure of arrays, memory pooling, SIMD alignment

These targeted improvements would enhance **performance and portability** while maintaining the **authentic mathematical foundation** of the original game.

This mathematical foundation provides the **flexibility and performance** needed for a contemporary game engine while maintaining the **authentic feel** of the original Revenant experience. The system balances **mathematical correctness** with **practical performance** considerations for modern hardware. 