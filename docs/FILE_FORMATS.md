# File Formats Documentation

## Overview

Revenant uses several custom file formats for storing game assets. The two primary imagery formats are **I2D** (2D imagery) and **I3D** (3D models), which are built on top of a common resource system that supports binary buffer loading with relative offsets.

## Summary

### File Format Architecture

Revenant's file system is built around a **layered architecture** with three main components:

1. **Common Resource System** - Base layer providing binary buffer loading and compression
2. **I2D Format** - 2D imagery with walkmaps for tiles, items, and sprites  
3. **I3D Format** - 3D models with skeletal animation and morphing
4. **Map Sector Format** - World data with object storage and sector-based loading

### Key Design Principles

- **Header/Body Separation** - Metadata in headers, bulk data in bodies
- **Relative Offsets** - Platform-independent pointer system using OFFSET type
- **Progressive Loading** - Headers load immediately, bodies load asynchronously
- **Compression Support** - ZIP compression for large assets
- **Version Compatibility** - Backward compatibility with automatic conversion

### File Types Overview

| Format | Purpose | Key Features |
|--------|---------|--------------|
| **I2D** | 2D imagery (tiles, items) | Walkmaps, animations, multiple states |
| **I3D** | 3D models (characters) | Skeletal animation, morphing, textures |
| **Sector** | World data | Object storage, walkmaps, dynamic loading |

## Core Concepts

### Binary Buffer System

All file formats use a **single binary buffer** approach where:
- Files are loaded entirely into memory as one buffer
- Data structures use relative offsets instead of absolute pointers
- The `OFFSET` type provides platform-independent pointer arithmetic

```cpp
typedef char* OFFSET;  // Relative pointer within loaded buffer
```

### Header/Body Architecture

**Header Section:**
- Contains metadata and structure definitions
- Always uncompressed for fast access
- Includes offsets to body data
- Loaded immediately for object creation

**Body Section:**
- Contains bulk asset data (images, models, animations)
- May be compressed to reduce file size
- Loaded asynchronously in background
- Accessed via offsets from header

### Progressive Loading System

The system supports **progressive loading** with these states:
- `QE_NONE` - Not loaded
- `QE_QUEUED` - Queued for loading
- `QE_LOADING` - Currently loading
- `QE_LOADED` - Fully loaded
- `QE_FAILED` - Load failed

### Offset-Based Data Access

Instead of absolute pointers, the system uses **relative offsets**:

```cpp
// Offset arrays for variable data
typedef OFFSET* OS3DImageryState;       // Array of state offsets
typedef OFFSET* OOOVEC3;                // 3D array [state][frame][vert]
typedef OFFSET* OS3DFace;               // Array of face offsets
```

### Compression System

**Supported Types:**
- `COMP_NONE` - No compression
- `COMP_ZIP` - ZIP implode compression

**Usage:**
- Headers are never compressed
- Bodies may be compressed based on file type
- Automatic decompression during loading

### Version Compatibility

**Version Management:**
- Each format has version numbers in headers
- Backward compatibility through version checking
- Automatic conversion of old formats
- Graceful handling of missing features

### Memory Management

**Buffer Lifecycle:**
1. **Loading** - File read into single buffer
2. **Processing** - Offsets converted to pointers
3. **Usage** - Direct memory access via pointers
4. **Cleanup** - Buffer freed when no longer needed

**Optimization Features:**
- **Page Touching** - Forces VM system to keep pages loaded
- **Virtual Memory Locking** - Locks memory in physical RAM (optional)
- **Reference Counting** - Tracks usage for automatic cleanup

### Coordinate Systems

**Multiple Coordinate Spaces:**
- **World Coordinates** - 3D game world (32-bit precision)
- **Screen Coordinates** - 2D display (scaled from world)
- **Walkmap Coordinates** - Collision grid (16-unit granularity)
- **Sector Coordinates** - World partitioning (1024-unit sectors)

**Coordinate Conversions:**
```cpp
// World to Walkmap
walkmap_x = (world_x >> WALKMAPSHIFT) - regx;
walkmap_y = (world_y >> WALKMAPSHIFT) - regy;

// World to Sector  
sector_x = world_x >> SECTORWSHIFT;
sector_y = world_y >> SECTORHSHIFT;
```

### Object System Integration

**Object Lifecycle:**
1. **Creation** - Object created from imagery data
2. **Placement** - Positioned in world with walkmap transfer
3. **Animation** - State changes trigger visual updates
4. **Interaction** - Collision detection via walkmaps
5. **Persistence** - Serialized to sector files

### Error Handling

**Validation Mechanisms:**
- **Magic Numbers** - Ensures valid file types
- **Version Checks** - Prevents incompatible loads
- **Size Validation** - Prevents buffer overflows
- **Corruption Detection** - Sanity checks for data integrity

**Fallback Strategies:**
- **Missing Resources** - Graceful handling with defaults
- **Load Failures** - Error reporting and recovery
- **Corrupted Data** - Skip invalid objects, continue loading

### Performance Optimizations

**Loading Optimizations:**
- **Progressive Loading** - Headers first, bodies later
- **Async I/O** - Background loading threads
- **Compression** - Reduced file sizes and I/O
- **Caching** - Keep frequently used assets in memory

**Runtime Optimizations:**
- **Sector-Based Loading** - Load only visible world areas
- **Object Sets** - Efficient object categorization
- **Walkmap Caching** - Height data for collision detection
- **Reference Counting** - Automatic memory management

### Platform Independence

**Cross-Platform Features:**
- **Relative Offsets** - No absolute pointer dependencies
- **Endian-Neutral** - Consistent byte ordering
- **Alignment Handling** - Proper structure alignment
- **Size Agnostic** - Works with different integer sizes

**Modern Porting Considerations:**
- **Memory Mapping** - Use mmap() for large files
- **Async I/O** - Modern async file operations
- **Streaming** - Progressive loading improvements
- **Compression** - Modern compression algorithms

This conceptual foundation provides the context needed to understand the detailed file format specifications that follow. The system's design emphasizes performance, flexibility, and cross-platform compatibility while maintaining backward compatibility with the original game's data.

## Common Resource System

### File Structure

All resource files follow a common structure:

```
[FileResHdr] - Resource file header
[Header Section] - Optional header data (hdrsize bytes)
[Bitmap Table] - Array of offsets to bitmaps (if topbm > 0)
[Data Section] - Binary asset data (datasize bytes)
```

### FileResHdr Structure

```cpp
struct FileResHdr
{
    uint32_t resmagic;    // Magic number: 'CGSR'
    uint16_t topbm;       // Number of bitmaps in file
    uint8_t  comptype;    // Compression type (COMP_NONE, COMP_ZIP)
    uint8_t  version;     // File format version
    uint32_t datasize;    // Size of data section
    uint32_t objsize;     // Total object size
    uint32_t hdrsize;     // Size of header section
};
```

## Map Sector Format

### Overview

The map sector format stores individual sections of the game world. The world is divided into a grid of sectors, each containing objects, walkmaps, and other game data. Sectors are dynamically loaded as the player moves through the world.

### File Naming Convention

Sector files follow the naming pattern:
```
{level}_{sectorx}_{sectory}.DAT
```

**Examples:**
- `0_1_2.DAT` - Level 0, Sector (1,2)
- `1_5_3.DAT` - Level 1, Sector (5,3)

### Sector Dimensions

```cpp
#define SECTORWIDTH     1024    // World coordinates
#define SECTORHEIGHT    1024    // World coordinates
#define SECTORWSHIFT    10      // Shift for sector width (1024 = 2^10)
#define SECTORHSHIFT    10      // Shift for sector height (1024 = 2^10)

#define MAXSECTORX      32      // Maximum sectors in X direction
#define MAXSECTORY      32      // Maximum sectors in Y direction
#define MAXMAPWIDTH     (MAXSECTORX * SECTORWIDTH)   // 32,768 world units
#define MAXMAPHEIGHT    (MAXSECTORY * SECTORHEIGHT)  // 32,768 world units
```

### File Structure

```
[Sector Header] - Magic number and version
[Object Count] - Number of objects in sector
[Object Data] - Array of serialized objects
```

### Sector Header

```cpp
// Magic number: 'MAP ' (space-padded)
uint32_t SectrorMapFCC = (('M' << 0) | ('A' << 8) | ('P' << 16) | (' ' << 24));

// Current version (MAP_VERSION = 9)
int32_t version;

// Number of objects in this sector
int32_t numobjects;
```

### Object Storage

Each object in the sector is stored as a serialized block with the following structure:

```cpp
// Object block header
short objversion;        // Object version (if version >= 8)
short objclass;          // Object class ID
short objtype;           // Object type ID (version < 1) or -1
uint32_t uniqueid;       // Unique object ID (version >= 1)
short blocksize;         // Size of object data (version >= 4)

// Object data (serialized TObjectInstance)
[Object Data]            // Variable length object data

// Inventory data
[Inventory Data]         // Object's inventory contents
```

### Object Loading Process

1. **Version Detection**: Check if file has header or legacy format
2. **Object Count**: Read number of objects to load
3. **Object Loading**: For each object:
   - Read object header (class, type, unique ID)
   - Create object instance based on class
   - Deserialize object data
   - Load inventory data
   - Add to sector's object array

### Walkmap System

Each sector contains a walkmap for collision detection:

```cpp
#define WALKMAPSHIFT        4
#define WALKMAPGRANULARITY  (1 << WALKMAPSHIFT)  // 16 units
#define WALKMAPSIZE         ((SECTORWIDTH >> WALKMAPSHIFT) * (SECTORHEIGHT >> WALKMAPSHIFT))

// Walkmap data (64x64 grid for 1024x1024 sector)
uint16_t walkmap[WALKMAPSIZE];  // Height values at each walkmap position
```

**Walkmap Coordinates:**
- **World to Walkmap**: `(x >> WALKMAPSHIFT, y >> WALKMAPSHIFT)`
- **Walkmap to World**: `(x << WALKMAPSHIFT, y << WALKMAPSHIFT)`
- **Granularity**: 16 world units per walkmap cell

### Object Sets

Sectors organize objects into sets for efficient processing:

```cpp
#define NUMOBJSETS      6

// Object sets for different processing categories
TObjSetArray objsets[NUMOBJSETS-1];  // Arrays of object indices

// Set types:
// OBJSET_MOVING     - Objects that move
// OBJSET_CHARACTER  - Character objects
// OBJSET_LIGHTS     - Light sources
// OBJSET_PULSE      - Pulsing objects
// OBJSET_ANIMATE    - Animated objects
// OBJSET_NOTIFY     - Objects that need notification
```

### Sector Loading Process

```cpp
bool TSector::Load(bool lock)
{
    // 1. Try current map directory first
    strcpy(mappath, CurMapPath);
    strcat(mappath, CURMAPDIR "\\");
    strcat(mappath, filename);
    
    // 2. Fall back to base map directory
    if (!fp) {
        strcpy(mappath, BaseMapPath);
        strcat(mappath, BASEMAPDIR "\\");
        strcat(mappath, filename);
    }
    
    // 3. Read entire file into buffer
    int32_t bufsize = flen(fp);
    uint8_t *buf = (uint8_t *)malloc(bufsize);
    fread(buf, bufsize, 1, fp);
    
    // 4. Parse with TInputStream
    TInputStream is(buf, bufsize);
    
    // 5. Check for header format
    int32_t numobjects;
    is >> numobjects;
    
    if ((uint32_t)numobjects == SectrorMapFCC) {
        is >> version;    // Read version
        is >> numobjects; // Read actual object count
    }
    
    // 6. Load objects
    for (int32_t c = 0; c < numobjects; c++) {
        TObjectInstance* inst = TObjectInstance::LoadObject(is, version, true);
        if (inst) {
            inst->ForceSector(this);
            inst->ForceLevel(level);
        }
        objects.Set(inst, c);
    }
}
```

### Sector Saving Process

```cpp
void TSector::Save()
{
    int32_t version = MAP_VERSION;
    
    // Don't save empty sectors
    if (objects.NumItems() == 0) {
        unlink(filename);
        return;
    }
    
    TOutputStream os(STARTSIZE, GROWSIZE);
    
    // Write header
    os << SectrorMapFCC;  // 'MAP '
    os << version;        // Current version
    os << objects.NumItems();
    
    // Write objects
    for (TObjectIterator i(&objects); i; i++) {
        TObjectInstance::SaveObject(i.Item(), os, true);
    }
    
    // Write to file
    FILE *fp = fopen(mappath, "wb");
    fwrite(os.Buffer(), os.DataSize(), 1, fp);
    fclose(fp);
}
```

### Version Compatibility

**Version History:**
- **Version 0**: No unique IDs, direct object type storage
- **Version 1**: Added unique IDs for objects
- **Version 3**: Removed map indexes (now use unique IDs)
- **Version 4**: Added block size for object skipping
- **Version 8**: Added object versioning
- **Version 9**: Current version

**Backward Compatibility:**
- Version checking during loading
- Automatic conversion of old formats
- Graceful handling of missing object classes

### Preload System

Sectors can be preloaded for performance:

```cpp
// Preload sectors in specified area
static bool LoadPreloadSectors(int32_t level, int32_t numrects, SRect *rects);

// Find preloaded sector
static TSector* FindPreloadSector(int32_t level, int32_t sectorx, int32_t sectory);

// Clear preloaded sectors
static void ClearPreloadSectors(int32_t level = -1, int32_t numrects = 0, SRect *rects = nullptr);
```

### Sector Coordinate System

**World to Sector Conversion:**
```cpp
int32_t sectorx = worldx >> SECTORWSHIFT;  // Divide by 1024
int32_t sectory = worldy >> SECTORHSHIFT;  // Divide by 1024
```

**Sector to World Conversion:**
```cpp
int32_t worldx = sectorx << SECTORWSHIFT;  // Multiply by 1024
int32_t worldy = sectory << SECTORHSHIFT;  // Multiply by 1024
```

**Sector Bounds:**
```cpp
// Sector world coordinates
int32_t left = sectorx << SECTORWSHIFT;
int32_t top = sectory << SECTORHSHIFT;
int32_t right = left + SECTORWIDTH - 1;
int32_t bottom = top + SECTORHEIGHT - 1;
```

### Memory Management

**Sector Lifecycle:**
1. **Loading**: File read into memory buffer
2. **Parsing**: Objects deserialized and created
3. **Usage**: Objects processed during gameplay
4. **Saving**: Objects serialized back to file
5. **Cleanup**: Memory freed when sector unloaded

**Optimization Features:**
- **Preloading**: Keep frequently accessed sectors in memory
- **Object Sets**: Efficient object categorization
- **Walkmap Caching**: Height data for collision detection
- **Reference Counting**: Track sector usage

### Error Handling

**File Validation:**
- Magic number check ('MAP ')
- Version compatibility
- Object count validation
- Corrupted object handling

**Fallback Mechanisms:**
- Missing sector files create empty sectors
- Corrupted objects are skipped
- Invalid object classes fall back to tile class
- Version mismatch triggers conversion

## I2D Format (2D Imagery)

### Overview

The I2D format stores 2D animated imagery for objects like items, walls, and sprites. It uses a header/body separation system for efficient loading.

### File Structure

```
[I2D Header] - SImageryHeader + SImageryStateHeader array
[I2D Body] - SAnimImageryBody with actual image data
```

### I2D Header (SImageryHeader)

```cpp
struct SImageryHeader
{
    int32_t                 imageryid;              // Id number for imagery handler
    int32_t                 numstates;              // Number of states
    SImageryStateHeader     states[1];              // Imagery for state (variable array)
};
```

### State Header (SImageryStateHeader)

```cpp
struct SImageryStateHeader
{
    char        animname[MAXANIMNAME];  // Animation name (32 chars)
    OFFSET      walkmap;    // Offset to walkmap data
    uint32_t    flags;      // Imagery state flags
    short       aniflags;   // Animation state flags
    short       frames;     // Number of frames
    short       width;      // Graphics maximum width
    short       height;     // Graphics maximum height
    short       regx;       // Registration point x for graphics
    short       regy;       // Registration point y for graphics
    short       regz;       // Registration point z for graphics
    short       animregx;   // Registration point x of animation
    short       animregy;   // Registration point y of animation
    short       animregz;   // Registration point z of animation
    short       wregx;      // World registration x
    short       wregy;      // World registration y
    short       wregz;      // World registration z
    short       wwidth;     // Object's world width
    short       wlength;    // Object's world length
    short       wheight;    // Object's world height
    short       invaniflags;// Animation flags for inventory animation
    short       invframes;  // Number of frames of inventory animation
};
```

### I2D Body (SAnimImageryBody)

```cpp
struct SAnimImageryBody : public SImageryBody
{
    SAnimImageryState states[1];        // Imagery for state (variable array)
};
```

### State Data (SAnimImageryState)

```cpp
struct SAnimImageryState
{
    OTBitmap    still;                  // Still image (offset to bitmap)
    OTAnimation anim;                   // Animation associated with state
    OTBitmap    invitem;                // Inventory image
    OTAnimation invanim;                // Inventory image animation
    uint32_t    flags;                  // Imagery state flags
};
```

### I2D Flags

```cpp
#define ANIIM_LIT       0x1         // Draw anim imagery lit
#define ANIIM_UNLIT     0x2         // Draw anim imagery unlit
#define ANIIM_SELECTED  0x4         // Draw imagery when selected
#define ANIIM_CUTOFF    0x8         // Don't draw unless top is cut off
```

### I2D Usage

- **Still Images**: Static bitmaps for non-animated objects
- **Animations**: Frame sequences for animated objects
- **Inventory Icons**: Smaller versions for UI display
- **Z-Buffer Support**: Some images include depth information
- **Multiple States**: Objects can have different visual states

## Tile Walkmap Storage

### Overview

Tile walkmaps are stored directly in the **I2D imagery headers** as part of the `SImageryStateHeader` structure. Each state of a tile object can have its own walkmap data that defines the collision and height information for that specific visual state.

### Walkmap Storage Location

Walkmaps are stored in the **header section** of I2D files, not in the body section. This allows for efficient access to collision data without needing to load the full imagery body.

### SImageryStateHeader Walkmap Field

```cpp
struct SImageryStateHeader
{
    char        animname[MAXANIMNAME];  // Animation name (32 chars)
    OFFSET      walkmap;    // Offset to walkmap data
    uint32_t    flags;      // Imagery state flags
    // ... other fields ...
    short       wwidth;     // Object's world width for walkmap
    short       wlength;    // Object's world length for walkmap
    short       wheight;    // Object's world height for walkmap
    // ... other fields ...
};
```

### Walkmap Data Structure

The walkmap data is stored as a **2D array of height values**:

```cpp
// Walkmap data layout
uint8_t walkmap[wwidth * wlength];  // Height values in walkmap coordinates
```

**Data Format:**
- **Type**: `uint8_t` (8-bit height values)
- **Layout**: Row-major order (y * width + x)
- **Size**: `wwidth * wlength` bytes per state
- **Values**: Height in walkmap units (typically 0-255)

### Walkmap Access

Walkmaps are accessed through the `GetWalkMap()` function:

```cpp
virtual const uint8_t *GetWalkMap(int32_t state) const
{
    return ((uint32_t)state < (uint32_t)entry->header->numstates) ?
        ((uint8_t *)entry->header->states[state].walkmap.ptr()) : nullptr;
}
```

### Walkmap Coordinate System

**World to Walkmap Conversion:**
```cpp
// Convert world coordinates to walkmap coordinates
int32_t walkmap_x = (world_x >> WALKMAPSHIFT) - wregx;
int32_t walkmap_y = (world_y >> WALKMAPSHIFT) - wregy;
```

**Walkmap to World Conversion:**
```cpp
// Convert walkmap coordinates to world coordinates
int32_t world_x = (walkmap_x + wregx) << WALKMAPSHIFT;
int32_t world_y = (walkmap_y + wregy) << WALKMAPSHIFT;
```

### Walkmap Registration Points

Each state defines registration points for positioning the walkmap:

```cpp
short wregx;      // World registration X for walkmap
short wregy;      // World registration Y for walkmap
short wregz;      // World registration Z for walkmap
```

### Walkmap Bounding Box

The walkmap dimensions are defined by:

```cpp
short wwidth;     // Walkmap width in walkmap units
short wlength;    // Walkmap length in walkmap units
short wheight;    // Walkmap height in world units
```

### Walkmap Memory Layout

The walkmap data is stored contiguously in the header buffer:

```cpp
// Header memory layout
SImageryHeader header;
SImageryStateHeader states[numstates];
uint8_t walkmap_data[total_walkmap_size];  // Contiguous walkmap data
```

**Size Calculation:**
```cpp
int32_t total_size = sizeof(SImageryHeader);
for (int32_t i = 0; i < numstates; i++) {
    if (i > 0) total_size += sizeof(SImageryStateHeader);
    total_size += states[i].wwidth * states[i].wlength;
}
total_size = (total_size + 3) & 0xFFFFFFFCL;  // Round to 4-byte boundary
```

### Walkmap Validation

The system includes validation to ensure walkmap consistency:

```cpp
// Check if state has valid walkmap
if (states[i].wwidth < 1 || states[i].wlength < 1) {
    if (states[i].walkmap.ptr() != nullptr) {
        // Error: State has walkmap but no bounding box
    }
} else {
    if (states[i].walkmap.ptr() == nullptr) {
        // Error: State has bounding box but no walkmap
    }
}
```

### Walkmap Usage in Game

**Collision Detection:**
```cpp
// Get walkmap for object state
uint8_t *walk = imagery->GetWalkMap(oi->GetState());

// Get object dimensions
int32_t width, length, height;
imagery->GetWorldBoundBox(oi->GetState(), width, length, height);

// Get registration points
int32_t regx = imagery->GetWorldRegX(oi->GetState());
int32_t regy = imagery->GetWorldRegY(oi->GetState());
```

**Walkmap Transfer to Sectors:**
```cpp
// Transfer walkmap data to sector walkmap
sect->WalkmapHandler(WALK_TRANSFER, walk, pos.z, x, y, width, length, width);
```

### Walkmap Rotation Support

The system supports walkmap rotation based on object facing:

```cpp
if (oi->GetFace() != 0) {
    // Apply rotations to walkmap based on facing
    appliedwalk = (uint8_t *)malloc(width * length);
    
    // Rotate walkmap data based on facing angle
    // 0-127: 90 degree rotation
    // 128-191: 180 degree rotation  
    // 192-255: 270 degree rotation
}
```

### Walkmap Granularity

**Walkmap Resolution:**
```cpp
#define WALKMAPSHIFT        4
#define WALKMAPGRANULARITY  (1 << WALKMAPSHIFT)  // 16 world units per walkmap cell
```

**Coordinate Conversion:**
- **World Units**: 1 unit = 1 world coordinate
- **Walkmap Units**: 1 unit = 16 world coordinates (WALKMAPGRANULARITY)
- **Sector Units**: 1 unit = 1024 world coordinates (SECTORWIDTH)

### Walkmap Integration with Sectors

Tile walkmaps are integrated into the sector walkmap system:

1. **Object Placement**: When objects are placed, their walkmaps are transferred to sector walkmaps
2. **Dynamic Updates**: Walkmaps are updated when objects move or change state
3. **Collision Detection**: Sector walkmaps are used for pathfinding and collision detection
4. **Height Information**: Walkmap values provide height data for 3D positioning

### Walkmap File Format Summary

**Storage Location**: I2D header section (not body)
**Data Type**: 8-bit height values
**Layout**: 2D array in row-major order
**Size**: Variable based on object dimensions
**Access**: Direct pointer access via OFFSET system
**Validation**: Automatic consistency checking
**Rotation**: Runtime rotation support
**Integration**: Transferred to sector walkmaps for gameplay

This walkmap system provides efficient collision detection and height information for tile-based objects while maintaining the performance benefits of the header/body separation architecture.

## Audio File Formats

### Overview

Revenant supports multiple audio formats through a **dual system** approach:
1. **WAV Files** - Standard Windows WAV format for sound effects and dialogue
2. **Resource-Based Audio** - Custom WaveData format stored in resource files
3. **CD Audio** - Redbook audio for background music

### WAV File Support

#### Standard WAV Format

Revenant uses the **standard Windows WAV format** with full RIFF chunk support:

```cpp
// WAV file structure
RIFF Header ('RIFF')
├── WAVE Chunk ('WAVE')
    ├── Format Chunk ('fmt ')
    │   └── WAVEFORMATEX structure
    └── Data Chunk ('data')
        └── Audio sample data
```

#### WAV Loading Process

```cpp
bool LoadWave(char *filename, LPWAVEFORMATEX &format, uint32_t &size, LPBYTE &data)
{
    // 1. Open file with MMIO
    HMMIO hmmio = mmioOpen(filename, nullptr, MMIO_READ | MMIO_ALLOCBUF);
    
    // 2. Find WAVE chunk
    MMCKINFO mmckchunkinfo;
    mmckchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    mmioDescend(hmmio, &mmckchunkinfo, nullptr, MMIO_FINDRIFF);
    
    // 3. Read format chunk
    MMCKINFO subchunk;
    subchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    mmioDescend(hmmio, &subchunk, &mmckchunkinfo, MMIO_FINDCHUNK);
    mmioRead(hmmio, (HPSTR)format, subchunk.cksize);
    
    // 4. Read data chunk
    subchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmioDescend(hmmio, &subchunk, &mmckchunkinfo, MMIO_FINDCHUNK);
    data = new uint8_t[subchunk.cksize];
    mmioRead(hmmio, (char *)data, subchunk.cksize);
}
```

#### WAV Format Support

**Supported Formats:**
- **PCM** - Uncompressed audio (primary format)
- **Compressed** - ACM decompression support
- **Mono/Stereo** - Both channel configurations
- **Variable Bit Depth** - 8-bit, 16-bit, etc.

**Format Conversion:**
```cpp
// Convert to PCM if needed
if (sound->format.wFormatTag != WAVE_FORMAT_PCM) {
    sound->format.wFormatTag = WAVE_FORMAT_PCM;
    sound->format.wBitsPerSample = 16;
    sound->format.nBlockAlign = sound->format.wBitsPerSample * sound->format.nChannels / 8;
    sound->format.nAvgBytesPerSec = sound->format.nSamplesPerSec * sound->format.nBlockAlign;
    
    // Use ACM for decompression
    acmStreamOpen(&acmstream, nullptr, format, &(sound->format),
        nullptr, nullptr, nullptr, ACM_STREAMOPENF_NONREALTIME);
}
```

### Resource-Based Audio System

#### WaveData Format

Audio can be stored in **custom resource files** using the WaveData format:

```cpp
struct TWaveData
{
    WAVEFORMATEX format;            // Standard WAV format structure
    uint32_t size;                  // Number of bytes of data
    int32_t volume;                 // Volume adjustment
    int32_t loopstart;              // Begin loop location
    int32_t loopend;                // End loop location
    uint8_t data[1];                // Actual sound data (variable size)
};
```

#### Resource Loading

```cpp
// Load from resource file
PTSound TSound::Load(int32_t resid)
{
    char filename[MAXPATHLEN];
    strcpy(filename, "\\sound\\old\\wave");
    
    PTWaveData wave = (PTWaveData)LoadResource(filename, resid);
    if (!wave)
        return nullptr;
    
    bool looping = (wave->loopend - wave->loopstart) > 0;
    PTSound sound = Load(&(wave->format), wave->size, wave->data, looping);
    
    delete wave;
    return sound;
}
```

#### Resource File Structure

Resource-based audio follows the **common resource system**:

```
[FileResHdr] - Resource file header
[Header Section] - Optional header data
[WaveData Array] - Array of TWaveData structures
```

### Audio Directory Structure

#### File-Based Audio

```
sound/
├── effects/           # Sound effects (DIRRESID_EFFECTDIR)
│   ├── sword.wav
│   ├── magic.wav
│   └── footsteps.wav
└── {language}/        # Dialogue (DIRRESID_DIALOGDIR)
    ├── english/
    ├── french/
    └── german/
```

#### Resource-Based Audio

```
sound/old/wave.DAT     # WaveData resource file
```

### Audio Loading Methods

#### Method 1: Direct WAV Files

```cpp
// Load from WAV file
PTSound TSound::Load(char *name, int32_t dirresid)
{
    char filename[MAXPATHLEN];
    makepath(ResourcePath, filename, MAXPATHLEN - 1);
    strcat(filename, "sound\\");
    
    if (dirresid == DIRRESID_EFFECTDIR)
        strcat(filename, "effects");
    else if (dirresid == DIRRESID_DIALOGDIR)
        strcat(filename, Language);
    
    strcat(filename, "\\");
    strcat(filename, name);
    strcat(filename, ".wav");
    
    LPWAVEFORMATEX format;
    uint32_t size;
    LPBYTE data;
    
    if (!LoadWave(filename, format, size, data))
        return nullptr;
    
    PTSound sound = Load(format, size, data, false);
    
    delete format;
    delete data;
    return sound;
}
```

#### Method 2: Resource Files

```cpp
// Load from resource ID
PTSound sound = TSound::Load(resource_id);
```

#### Method 3: Direct Buffer

```cpp
// Load from memory buffer
PTSound sound = TSound::Load(format, size, data, looping);
```

### Audio Playback System

#### DirectSound Integration

```cpp
// Create DirectSound buffer
DSBUFFERDESC desc;
memset(&desc, 0, sizeof(DSBUFFERDESC));
desc.dwSize = sizeof(DSBUFFERDESC);
desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
desc.dwBufferBytes = sound->size;
desc.lpwfxFormat = &sound->format;

HRESULT res = SoundPlayer.DirectSound->CreateSoundBuffer(&desc, &(sound->SoundBuffer), nullptr);
```

#### Audio Controls

```cpp
// Playback controls
void TSound::Play(int volume = 0, int freq = 0);
void TSound::Stop();
bool TSound::IsPlaying();
bool TSound::IsLooping();

// Volume and frequency control
SoundBuffer->SetVolume(volume);
SoundBuffer->SetFrequency(freq);
SoundBuffer->SetCurrentPosition(0);
SoundBuffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0);
```

### CD Audio System

#### Redbook Audio Support

Revenant includes **CD audio support** for background music:

```cpp
// CD functions
void CDOpen();
void CDClose();
void CDPlayTrack(int32_t track);
uint32_t CDTrackLength(int32_t track);
void CDPlayRandomTrack();
void CDStop();
bool CDPlaying();
void CDSetVolume(uint16_t volume);
```

#### CD Implementation

```cpp
void CDOpen()
{
    char err[120];
    DWORD res = mciSendString("open cdaudio shareable", nullptr, 0, 0L);
    mciGetErrorString(res, err, 120);
    isopen = true;
}

void CDPlayTrack(int32_t track)
{
    char cmd[80];
    sprintf(cmd, "play cdaudio from %d", track);
    mciSendString(cmd, nullptr, 0, 0L);
}
```

### Audio Management

#### Sound Reference System

```cpp
struct SSoundRef
{
    char *name;             // Name of sound
    char *dir;              // Directory where sound is stored
    int32_t resid;          // Resource id (negative for WAV files)
    int32_t usecount;       // How many in use
    uint32_t flags;         // Sound flags
    PTSound sound;          // Sound data, may be more than one
};
```

#### Sound Categories

```cpp
#define DIRRESID_EFFECTDIR -1    // Sound effects directory
#define DIRRESID_DIALOGDIR -2    // Dialogue directory
#define SOUND_DYING     (1 << 0) // Auto-deallocate when stopped
```

#### Sound Discovery

```cpp
bool TSoundPlayer::SearchSoundDir(char *soundpath, char *subdir, int32_t dirresid)
{
    char fname[MAXPATHLEN];
    strcpy(fname, soundpath);
    strcat(fname, subdir);
    strcat(fname, "\\*.wav");
    
    struct _finddata_t data;
    int32_t found, handle;
    found = handle = _findfirst(fname, &data);
    
    while (found != -1) {
        // Add sound to reference list
        SSoundRef* ref = new SSoundRef;
        ref->name = strdup(data.name);
        ref->resid = dirresid;
        ref->usecount = 0;
        ref->sound = nullptr;
        
        soundlist.Add(ref);
        found = _findnext(handle, &data);
    }
}
```

### Audio Features

#### Loop Support

**Automatic Looping:**
```cpp
// Check for loop points
bool looping = (wave->loopend - wave->loopend) > 0;

// Play with looping
SoundBuffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0);
```

#### Volume Control

**Dynamic Volume:**
```cpp
// Set volume (0 = max, -10000 = silence)
SoundBuffer->SetVolume(volume);

// Volume adjustment in WaveData
int32_t volume;  // Volume adjustment value
```

#### Frequency Control

**Pitch Shifting:**
```cpp
// Set playback frequency
SoundBuffer->SetFrequency(freq);
```

### Audio Performance

#### Memory Management

**Buffer Lifecycle:**
1. **Loading** - WAV file or resource loaded into memory
2. **Processing** - Format conversion if needed
3. **DirectSound** - Buffer created in DirectSound
4. **Playback** - Audio played from DirectSound buffer
5. **Cleanup** - Buffer released when sound stops

#### Optimization Features

- **Static Buffers** - Pre-loaded audio for immediate playback
- **Volume Control** - Hardware-accelerated volume changes
- **Pan Control** - Stereo positioning support
- **Frequency Control** - Real-time pitch adjustment
- **Looping** - Efficient loop playback without re-buffering

### Audio File Format Summary

**Supported Formats:**
- **WAV Files** - Standard Windows WAV with RIFF chunks
- **Resource Audio** - Custom WaveData format in resource files
- **CD Audio** - Redbook audio for background music

**Audio Features:**
- **Multi-format Support** - PCM and compressed formats
- **ACM Decompression** - Automatic format conversion
- **Loop Support** - Configurable loop points
- **Volume Control** - Dynamic volume adjustment
- **Frequency Control** - Real-time pitch shifting
- **Pan Control** - Stereo positioning
- **3D Audio** - Positional audio support (commented out)

**Directory Structure:**
- **Effects** - Sound effects in `sound/effects/`
- **Dialogue** - Language-specific dialogue in `sound/{language}/`
- **Resources** - WaveData resources in `sound/old/wave.DAT`

This audio system provides comprehensive support for both file-based and resource-based audio, with full DirectSound integration and CD audio support for a complete audio experience.

## I3D Format (3D Models)

### Overview

The I3D format stores 3D models with skeletal animation data. It supports morphing animations, multiple objects, textures, and complex animation systems.

### File Structure

```
[I3D Header] - SImageryHeader + SImageryStateHeader array
[I3D Body] - S3DImageryBody with 3D model data
```

### I3D Body (S3DImageryBody)

```cpp
struct S3DImageryBody : public SImageryBody
{
    uint32_t flags;                     // Feature flags
    uint32_t version;                   // Version of imagery (VERSION3DIMAGEBODY = 3)
    
    OS3DImageryState statedata;         // Array of S3DImageryState structures
    
    int32_t numverts;                   // Number of vertices
    OOOVEC3 verts;                      // 3D vertex data [state][frame][vert]
    
    int32_t numfaces;                   // Number of faces
    OS3DFace faces;                     // Face data stored by texture
    
    int32_t nummaterials;               
    OMATERIAL materials;                // Offset to D3DMATERIAL array
    
    int32_t numtextures;
    OS3DImageryTexture textures;        // Offset to texture array
    
    int32_t numobjects;
    OS3DImageryObject objects;          // Offset to object array
    
    int32_t numtags;
    OS3DImageryTag tags;                // Offset to animation tags
};
```

### I3D Flags

```cpp
#define I3D_ISMORPH      1  // Multiple vertex images per state (morph animation)
#define I3D_HASICONS     2  // Has inventory icons
#define I3D_HASHIERARCHY 4  // Has object hierarchy
#define I3D_FACINGMOTION 8  // Has facing motion data (hip facing deltas)
#define I3D_3DIMAGEBODY2 16 // Uses 3D image body 2 header format
#define I3D_HASPLAYSOUND 32 // Has playsound structure
#define I3D_ROOTMOTION   64 // Uses root motion and SAniKey motion data
#define I3D_ANIKEY32     128// New compressed animation key format
```

### 3D State Data (S3DImageryState)

```cpp
struct S3DImageryState
{
    int32_t     invsize;                // Size of inventory icon data
    OTBitmap    invitem;                // Inventory image data
    OTAnimation invanim;                // Inventory animation data
    OSAniKey32  motion;                 // Offset to motion data for this state
};
```

### 3D Object Data (S3DImageryObject)

```cpp
struct S3DImageryObject
{
    char name[RESNAMELEN];              // Object name
    WORD material;                      // Material index
    WORD vertpos;                       // Position of object verts in vert array
    WORD vertnum;                       // Number of object verts in vert array
    OS3DImageryObjectTexture textures;  // Offset to texture array for this object
    OS3DImageryObjectState states;      // Offset to state array for this object
};
```

### 3D Face Data (S3DFace)

```cpp
struct S3DFace
{
    uint16_t v1, v2, v3;                // Vertex indices
    uint16_t texture;                   // Texture index
    uint16_t material;                  // Material index
    uint16_t flags;                     // Face flags
};
```

### Animation Tags (S3DImageryTag)

```cpp
struct S3DImageryTag
{
    int32_t state;                      // State to trigger tag
    int32_t frame;                      // Frame number to trigger tag
    OFFSET name;                        // Name of tag
    OFFSET str;                         // String for tag
};
```

## Offset System

### OFFSET Type

The `OFFSET` type is used throughout the file formats to provide relative pointers within the loaded binary buffer:

```cpp
typedef char* OFFSET;  // Relative pointer within loaded buffer
```

### Offset Arrays

For variable-sized data, offset arrays are used:

```cpp
typedef OFFSET* OS3DImageryState;       // Array of state offsets
typedef OFFSET* OOOVEC3;                // 3D array [state][frame][vert]
typedef OFFSET* OS3DFace;               // Array of face offsets
typedef OFFSET* OMATERIAL;              // Array of material offsets
```

## Loading Process

### 1. Header Loading

```cpp
// Load header section (always uncompressed)
SImageryHeader* header = (SImageryHeader*)LoadResourceHeader(filename);
```

### 2. Body Loading

```cpp
// Load body section (may be compressed)
SImageryBody* body = LoadResourceBinary(filename);
```

### 3. Progressive Loading

The system supports progressive loading where:
- Headers are loaded immediately for metadata
- Bodies are loaded asynchronously in background thread
- Loading status is tracked (QE_NONE, QE_QUEUED, QE_LOADING, QE_LOADED, QE_FAILED)

## Compression

### Supported Types

```cpp
#define COMP_NONE    0  // No compression
#define COMP_ZIP     1  // ZIP implode compression
```

### Decompression

```cpp
// Automatic decompression during loading
if (frh.comptype == COMP_ZIP) {
    ChunkDecompressZ(source, dest, clear);
}
```

## Version Compatibility

### I3D Version History

- **Version 1**: Original format with limited arrays
- **Version 2**: Added begstate/endstate to state structure
- **Version 3**: Removed begstate/endstate, changed playsounds to animation tags

### Backward Compatibility

The system maintains backward compatibility through:
- Version checking in headers
- Fallback to old format structures
- Automatic conversion of old data

## Memory Management

### Buffer Lifecycle

1. **Loading**: File loaded as single binary buffer
2. **Processing**: Offsets converted to pointers
3. **Usage**: Direct memory access via pointers
4. **Cleanup**: Buffer freed when no longer needed

### Optimization Features

- **Page Touching**: Forces VM system to keep pages loaded
- **Virtual Memory Locking**: Locks memory in physical RAM (optional)
- **Reference Counting**: Tracks usage for automatic cleanup

## File Naming Convention

### Resource Files

- `filename.DAT` - Single resource file
- `filename.000` through `filename.999` - Split resource files

### Imagery Files

- `*.I2D` - 2D imagery files
- `*.I3D` - 3D model files

### Sector Files

- `{level}_{sectorx}_{sectory}.DAT` - Map sector files

## Error Handling

### Validation

- **Magic Number**: Ensures valid resource files ('CGSR')
- **Version Check**: Prevents loading incompatible files
- **Size Validation**: Prevents buffer overflows
- **Corruption Detection**: Sanity checks for data integrity

### Fallback Mechanisms

- **Missing Resources**: Graceful handling of missing files
- **Load Failures**: Fallback to default imagery
- **Corrupted Data**: Error reporting and recovery

## Porting Considerations

### Platform Independence

The binary buffer approach with relative offsets is inherently platform-independent, making it suitable for cross-platform ports.

### Modern Optimizations

- **Memory Mapping**: Use mmap() for large files
- **Async I/O**: Modern async file operations
- **Compression**: Modern compression algorithms
- **Streaming**: Progressive loading improvements

This file format system represents a sophisticated approach to asset management that balances performance, flexibility, and memory efficiency. The header/body separation and progressive loading features were ahead of their time and remain relevant in modern game development. 