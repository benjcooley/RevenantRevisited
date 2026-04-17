# Map Rendering System in Revenant

## Overview

Revenant uses a sophisticated **2.5D layered z-buffer rendering system** that combines 2D tile-based world geometry with 3D character models. The system employs a **multi-pass rendering pipeline** with separate unlit and lit buffers, dynamic lighting, and multi-threaded background updates.

**Most notably, Revenant implements a full deferred lighting system (G-buffer rendering) years before this technique became standard in the graphics industry.** This makes it one of the earliest games to use what would later become known as deferred shading.

## Historical Significance

### Deferred Lighting Innovation (1998)

Revenant's rendering system represents a significant technical innovation for its time:

- **Deferred Lighting**: Implements what we now call G-buffer rendering 6-8 years before it became standard
- **Multi-Pass Pipeline**: Geometry pass → Lighting pass → Compositing pass → Final pass
- **Per-Pixel Lighting**: Sophisticated lighting calculations with normal maps and multiple light sources
- **Memory Optimization**: 32-bit lighting buffer with 16-bit final output

**Historical Context:**
- **1998**: Revenant releases with deferred lighting
- **2004-2005**: Deferred shading formally described in academic papers
- **Late 2000s**: G-buffer rendering becomes standard in AAA games

### Modern Parallels

Revenant's approach directly parallels modern deferred rendering:

| Revenant (1998) | Modern Deferred Rendering |
|------------------|---------------------------|
| `unlitmulti` (32-bit) | G-buffer (position, normals, albedo) |
| `DrawStaticLights()` | Light accumulation pass |
| `TransferUnlitToLit()` | Lighting composition |
| `litmulti` (16-bit) | Final color buffer |
| Software rendering | GPU-based rendering |

## Core Architecture

### Rendering Philosophy

Revenant's rendering system is designed around the concept of **layered depth rendering** with **hybrid deferred/forward lighting**:

- **2D Tiles**: World geometry rendered as 2D sprites with z-depth
- **3D Characters**: Full 3D models with skeletal animation
- **Z-Buffer Integration**: Unified depth testing across both 2D and 3D elements
- **Hybrid Lighting**: Deferred lighting for world geometry, forward rendering for characters
- **G-Buffer Approach**: Store geometry data for world, then apply lighting in separate passes

### Hybrid Rendering Architecture

Revenant uses a sophisticated hybrid approach that combines the best of both rendering techniques:

#### 2D World: Deferred Lighting Pipeline
- **Geometry Pass**: Render tiles to unlit buffer (G-buffer creation)
- **Lighting Pass**: Apply lighting calculations to stored geometry data  
- **Compositing Pass**: Transfer lit data to final buffer
- **Benefits**: Multiple light sources, complex lighting, memory efficient

#### 3D Characters: Forward Rendering Pipeline
- **DirectX Fixed Function**: Uses GPU's built-in lighting pipeline
- **Scene3D.AddLight()**: Character light sources integrated with 3D scene
- **Performance**: GPU-accelerated lighting for characters
- **Benefits**: Hardware acceleration, real-time lighting updates

This hybrid approach is remarkably advanced for 1998 and mirrors modern "forward+ rendering" techniques where:
- **Deferred rendering** handles complex world lighting
- **Forward rendering** handles characters and transparent objects

### Deferred Lighting Pipeline

The system follows a classic deferred rendering approach for world geometry:

1. **Geometry Pass**: Render all world objects to unlit buffer (G-buffer creation)
2. **Lighting Pass**: Apply lighting calculations to stored geometry data
3. **Compositing Pass**: Transfer lit data to final buffer
4. **Forward Pass**: Render 3D characters and effects on top

This approach allows for:
- **Multiple light sources** without performance degradation
- **Complex lighting calculations** with normal maps
- **Efficient memory usage** through buffer optimization
- **Scalable lighting** independent of geometry complexity

### Key Components

#### TMosaicSurface - Multi-Buffer System
```cpp
class TMosaicSurface : public TSurface
{
    // Multiple rendering buffers
    PTMosaicSurface unlitmulti;    // 32-bit unlit buffer (geometry + normals)
    PTMosaicSurface litmulti;      // 16-bit lit buffer (final output)
    PTMosaicSurface updatemulti;   // Clone for background updates
    bool zbufferiscloned;          // Z-buffer sharing optimization
};
```

**Buffer Purposes:**
- **unlitmulti**: 32-bit buffer for geometry, normals, and lighting calculations
- **litmulti**: 16-bit final output buffer with applied lighting
- **updatemulti**: Thread-safe clone for background rendering

#### Surface Flags and Capabilities
```cpp
#define MOSAICSURF_8BIT          (1<<1)   // 8-bit color depth
#define MOSAICSURF_16BIT         (1<<2)   // 16-bit color depth  
#define MOSAICSURF_24BIT         (1<<3)   // 24-bit color depth
#define MOSAICSURF_32BIT         (1<<4)   // 32-bit color depth
#define MOSAICSURF_BMSURFACE     (1<<5)   // Bitmap surface
#define MOSAICSURF_SYSTEMMEM     (1<<6)   // System memory allocation
#define MOSAICSURF_VIDEOMEM      (1<<7)   // Video memory allocation
#define MOSAICSURF_ZBUFFER       (1<<9)   // Z-buffer support
#define MOSAICSURF_ZSYSTEMMEM    (1<<10)  // Z-buffer in system memory
#define MOSAICSURF_NORMALS       (1<<13)  // Normal buffer support
#define MOSAICSURF_CLONEZBUFFER  (1<<18)  // Clone z-buffer from source
```

## Rendering Pipeline

### Multi-Pass Rendering Process

The rendering pipeline follows a specific sequence optimized for the 2.5D architecture:

#### 1. DrawUnlitObjects() - Geometry Pass
```cpp
void TMapPane::DrawUnlitObjects(SRect &r)
{
    // Clear buffers if needed
    if (ClearBeforeDraw || level != 0)
        unlitmulti->Box(r.x(), r.y(), r.w(), r.h(), 0, 0xFFFF, 0,
            DM_WRAPCLIP | DM_ZBUFFER | DM_NORMALS);
    
    // Draw all objects in unlit pass
    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        i.Item()->DrawUnlit(unlitmulti);
    }
}
```

**Purpose:**
- Render all geometry to 32-bit unlit buffer
- Establish z-depth for all objects
- Generate normal maps for lighting calculations
- Handle transparency and alpha blending

#### 2. DrawStaticLights() - Lighting Pass
```cpp
void TMapPane::DrawStaticLights(SRect &r)
{
    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        i.Item()->DrawLight(unlitmulti, IsDirty());
    }
}
```

**Purpose:**
- Apply dynamic lighting to unlit geometry
- Calculate light falloff and shadows
- Generate lighting maps for transfer to lit buffer
- Handle multiple light sources and colors

#### 3. TransferUnlitToLit() - Buffer Transfer
```cpp
void TMapPane::TransferUnlitToLit(SRect &r)
{
    // Transfer graphic/lighting data to lit buffer
    TransferAndLight32to16(litmulti, unlitmulti, r);
    
    // Transfer zbuffer data to lit buffer
    if (!zbufferiscloned)
        litmulti->Blit(r.x(), r.y(), unlitmulti, r.x(), r.y(), r.w(), r.h(), 
            DM_NODRAW | DM_ZBUFFER | DM_WRAPCLIP);
}
```

**Purpose:**
- Convert 32-bit lighting data to 16-bit final colors
- Preserve z-buffer information for 3D objects
- Optimize memory usage for final output

#### 4. DrawLitObjects() - Final Pass
```cpp
void TMapPane::DrawLitObjects(SRect &r)
{
    // Draw walkmap in editor mode
    if (Editor && StatusBar.EditWalkmap())
        DrawWalkMap(r);
    
    // Draw all lit objects (3D characters, effects)
    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        if (i.Item()->GetFlags() & OF_EDITOR && !Editor)
            continue;
        i.Item()->DrawLit(litmulti);
    }
}
```

**Purpose:**
- Render 3D characters and effects
- Apply final lighting and shadows
- Handle editor-specific overlays
- Generate final screen output

### Background Drawing Flags

The rendering system uses flags to control which passes are executed:

```cpp
enum BGDRAW_FLAGS
{
    BGDRAW_UNLIT = 0,      // Draw unlit geometry
    BGDRAW_LIGHTS = 1,     // Apply lighting
    BGDRAW_AMBIENT = 2,    // Transfer to lit buffer
    BGDRAW_LIT = 3,        // Draw lit objects
    BGDRAW_REDRAW = 4      // Force complete redraw
};
```

## Object Rendering Integration

### DrawUnlit() - Object Geometry Rendering
```cpp
void TAnimImagery::DrawUnlit(TObjectInstance* oi, TSurface* surface)
{
    int32_t state = oi->GetState();
    PTBitmap bm = oi->GetStillImage();
    
    if (!bm) return;
    
    S3DPoint screenpos;
    oi->GetScreenPos(screenpos);
    
    uint32_t flags = DM_TRANSPARENT;
    
    // Handle different bitmap types
    if (bm->flags & BM_ALPHA)
        flags |= DM_ALPHA;
    else if (bm->flags & BM_ZBUFFER)
        flags |= DM_ZBUFFER; 
    else
        flags |= DM_ZSTATIC;
    
    // Add normal mapping for lighting
    if (bm->flags & BM_NORMALS && !NoNormals)
        flags |= DM_NORMALS;
    
    // Apply object transformations
    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;
    
    // Render to unlit buffer
    if (GetImageFlags(state) & ANIIM_UNLIT && bm)
        surface->ZPut(screenpos.x - GetRegX(state),
                      screenpos.y - GetRegY(state),
                      screenpos.z - GetRegZ(state), bm, flags);
}
```

### DrawLit() - Object Final Rendering
```cpp
void TAnimImagery::DrawLit(TObjectInstance* oi, TSurface* surface)
{
    int32_t state = oi->GetState();
    PTBitmap bm = oi->GetStillImage();
    
    if (!bm) return;
    
    uint32_t flags = DM_TRANSPARENT;
    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;
    
    // Render lit objects (3D characters, effects)
    if (GetImageFlags(state) & ANIIM_LIT && bm)
    {
        S3DPoint screenpos;
        oi->GetScreenPos(screenpos);
        
        if (bm->flags & BM_ALPHA)
            flags |= DM_ALPHA;
        
        if (bm->flags & BM_ZBUFFER)
        {
            int dim = 0;
            flags |= DM_ZBUFFER;
            surface->ZPutDim(screenpos.x - GetRegX(state),
                             screenpos.y - GetRegY(state),
                             screenpos.z - GetRegZ(state), bm, flags, dim);
        }
        else
            surface->Put(screenpos.x - GetRegX(state), 
                        screenpos.y - GetRegY(state), bm, flags);
    }
}
```

## Lighting System

### Dynamic Lighting Architecture

Revenant's lighting system operates in multiple layers:

#### 1. Ambient Lighting
```cpp
void DrawAmbientLight(TSurface* surface, SRect& r)
{
    // Apply global ambient lighting
    // Sets base lighting level for all objects
}
```

#### 2. Static Light Sources
```cpp
void DrawStaticLightNoNormals(S3DPoint& pos, RSColor color, 
                              int32_t intensity, TSurface* surface, int32_t id)
{
    // Calculate light falloff
    int32_t intratio = (254 << 16) / intensity + 256;
    
    // Apply lighting to 32-bit buffer
    // Uses lookup tables for performance
}
```

#### 3. Light Dropoff Calculation
```cpp
void DrawStaticLightBlock(int32_t ptrpos, uint16_t* zbufptr, uint32_t* ubptr,
                         int32_t z, int32_t intensity, int32_t nextline,
                         int32_t xpos, int32_t xend, int32_t ypos, int32_t yend)
{
    // Per-pixel lighting calculation
    // Respects z-buffer for proper depth testing
    // Applies color and intensity falloff
}
```

### Lighting Integration with 3D Scene

```cpp
void TObjectInstance::OnScreen()
{
    // Add 3D lighting for objects with light sources
    if (flags & OF_LIGHT)
    {
        if (lightdef.lightindex == -1)
        {
            S3DPoint lpos = pos;
            lpos += lightdef.pos;
            lightdef.lightindex = Scene3D.AddLight(lpos, lightdef.color, 
                                                  lightdef.intensity, lightdef.multiplier);
        }
    }
}
```

## Multi-Threaded Rendering

### Background Update System

Revenant uses a sophisticated multi-threaded rendering system:

#### Update Thread Management
```cpp
void TMapPane::BeginUpdateThread()
{
    // Start background rendering thread
    // Handles tile updates and object rendering
    // Uses mutex protection for thread safety
}

void TMapPane::EndUpdateThread()
{
    // Safely terminate update thread
    // Clean up resources and wait for completion
}
```

#### Thread-Safe Object Access
```cpp
void TMapPane::DrawUnlitObjects(SRect &r)
{
    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        WaitSingleErr(ObjectMutex);        // Lock object access
        i.Item()->DrawUnlit(unlitmulti);
        ReleaseMutex(ObjectMutex);         // Release lock
        
        if (UpdateCancelled) return;       // Check for cancellation
    }
}
```

### Update Rectangle Management

The system uses intelligent update rectangles to minimize rendering:

```cpp
void TMapPane::GetUpdateObjs(SRect &r)
{
    UpdateObjs.Clear();
    
    // Get objects that need updating in this rectangle
    for (TMapIterator i(&r, CHECK_RECT|CHECK_LIGHT|CHECK_MOVING|CHECK_INVIS|CHECK_NOINVENT); i; i++)
    {
        UpdateObjs.Add(i);
        
        if (UpdateCancelled) return;
    }
}
```

## Z-Buffer System

### Unified Depth Testing

Revenant's z-buffer system handles both 2D tiles and 3D objects:

#### Z-Buffer Types
- **BM_ZBUFFER**: Per-pixel depth information
- **BM_ZSTATIC**: Static depth value for flat objects
- **BM_ALPHA**: Alpha-blended objects (no z-buffer)

#### Z-Buffer Operations
```cpp
// Z-buffer testing for 2D objects
surface->ZPut(x, y, z, bitmap, flags);

// Z-buffer testing for 3D objects  
surface->ZPutDim(x, y, z, bitmap, flags, dim);

// Z-buffer restoration for 3D scene
Scene3D.RestoreZBuffer(r);
```

### Z-Buffer Optimization

```cpp
void T3DScene::RestoreZBuffer(SRect &r)
{
    if (NoScrollZBuffer)
        MapPane.DrawRestoreRect(r.x(), r.y(), r.w(), r.h(),
            DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);
    
    // Copy z-buffer from background to screen
    if (Display->UsingClearZBuffer())
    {
        Display->GetRealZBuffer()->Blit(sr.x(), sr.y(), 
            Display->GetZBuffer(), sr.x(), sr.y(), sr.w(), sr.h());
    }
}
```

## Performance Optimizations

### Mosaic Surface Tiling

The TMosaicSurface system uses tiled rendering for memory efficiency:

```cpp
// Create tiled surface for efficient memory usage
litmulti = new TMosaicSurface(
    SCROLLBUFWIDTH / MOSAICTILEX,    // Number of tiles X
    SCROLLBUFHEIGHT / MOSAICTILEY,   // Number of tiles Y
    MOSAICTILEX,                     // Tile width
    MOSAICTILEY,                     // Tile height
    MOSAICSURF_SYSTEMMEM | MOSAICSURF_ZBUFFER | MOSAICSURF_ZSYSTEMMEM);
```

### Chunk-Based Rendering

The system uses chunk-based rendering for large images:

```cpp
// Process image in chunks for memory efficiency
for (int32_t outerloop = ULy; outerloop < LRy; outerloop++)
{
    for (int32_t innerloop = ULx; innerloop < LRx; innerloop++)
    {
        // Calculate chunk boundaries
        dpval2.sx = 0;
        dpval2.swidth = CHUNKWIDTH;
        dpval2.sy = 0;
        dpval2.sheight = CHUNKHEIGHT;
        
        // Load chunk from cache
        dbval2.source = ChunkCache.AddChunk(hdr->block[outerloop * width + innerloop].ptr(), 1);
        
        // Render chunk with z-buffer support
        if (dpval2.drawmode & DM_ZBUFFER)
        {
            dbval2.szbuffer = (uint16_t *)ChunkCache.AddChunkZ(zhdr->block[outerloop * width + innerloop].ptr(), 2);
        }
    }
}
```

### Update Rectangle Optimization

The system only renders areas that have changed:

```cpp
void TMapPane::DrawUpdateRect(RSRect r, int32_t bgdraw)
{
    // Only render the specified rectangle
    // Use background drawing flags to control rendering passes
    // Check for thread cancellation between passes
}
```

## Integration with 3D Scene

### 3D Scene Rendering

Revenant integrates 2D tile rendering with 3D character rendering:

```cpp
bool T3DScene::DrawScene()
{
    if (!Show3D) return true;
    
    // Set up 3D rendering state
    Device->SetRenderState(D3DRENDERSTATE_ZENABLE, ZEnable);
    Device->SetRenderState(D3DRENDERSTATE_ZFUNC, ZFunc);
    
    // Render 3D objects with proper z-buffer integration
    for (int32_t c = 0; c < AnimatorArray.NumItems(); c++)
    {
        if (AnimatorArray[c] != nullptr)
            AnimatorArray[c]->Draw();
    }
}
```

### Camera and Viewport Management

```cpp
void T3DScene::SetCameraPos(hmm_vec3 pos, int32_t zdist)
{
    CameraPos = pos;
    
    // Set up isometric camera view
    hmm_mat4 view;
    D3DMATRIXClear(&view);
    
    // Apply camera transformations
    D3DMATRIXMove(&view, &v1); 
    D3DMATRIXRotateZ(&view, 45.0 * TORADIANf);
    D3DMATRIXRotateX(&view, -(90.0 + CAMERAANGLEf * TORADIAN));
    
    Device->SetMatrix(hView, &view);
}
```

## Editor Integration

### Walkmap Visualization

The editor includes special rendering for walkmap editing:

```cpp
void TMapPane::DrawWalkMap(SRect &r)
{
    // Draw walkmap grid in editor mode
    for (int sx = 0; sx < SECTORWINDOWX; sx++)
    {
        for (int sy = 0; sy < SECTORWINDOWY; sy++)
        {
            if (sectors[sx][sy])
            {
                // Calculate walkmap height
                pos.z = sectors[sx][sy]->ReturnWalkmap(pos.x, pos.y);
                
                // Color-code by height
                SColor color;
                color.red = max(30, 255 - 4*absval(pos.z - 20));
                color.green = max(30, 255 - 4*absval(pos.z - 100));
                color.blue = max(30, 255 - 4*absval(pos.z - 180));
                
                // Draw walkmap visualization
                litmulti->ZPut(screenpos.x, screenpos.y, screenpos.z, 
                              EditorData->Bitmap("grid"), 
                              DM_TRANSPARENT | DM_WRAPCLIP | DM_ZBUFFER | DM_USEREG | DM_BACKGROUND);
            }
        }
    }
}
```

## Rendering System Features

### Key Capabilities

1. **2.5D Rendering**: Seamless integration of 2D tiles and 3D objects
2. **Multi-Pass Lighting**: Sophisticated lighting with ambient and dynamic sources
3. **Z-Buffer Integration**: Unified depth testing across all rendering types
4. **Multi-Threaded Updates**: Background rendering for smooth performance
5. **Memory Optimization**: Tiled surfaces and chunk-based rendering
6. **Editor Support**: Special rendering modes for level editing

### Performance Characteristics

- **Memory Usage**: Optimized through tiled surfaces and shared buffers
- **Rendering Speed**: Multi-threaded background updates maintain 24 FPS
- **Z-Buffer Efficiency**: Selective z-buffer operations based on object type
- **Update Optimization**: Only renders changed areas

The map rendering system represents a sophisticated approach to 2.5D rendering that was advanced for its time, providing the foundation for Revenant's distinctive visual style and smooth gameplay experience. 