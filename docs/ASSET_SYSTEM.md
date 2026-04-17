# Asset System Documentation

## Overview

The Revenant asset system is built around a sophisticated binary buffer loading approach with relative offsets. This system allows efficient loading, caching, and management of game assets including graphics, audio, animations, and other resources.

## Summary

### Asset System Architecture

Revenant's asset system is built around a **layered architecture** with four main components:

1. **Binary Buffer System** - Base layer providing single-buffer file loading
2. **Resource Management** - Resource file format with header/body separation
3. **Progressive Loading** - Asynchronous loading with background threads
4. **Caching System** - Memory-based caching with LRU eviction

### Key Design Principles

- **Single Buffer Loading** - Files loaded entirely into memory as one buffer
- **Relative Offsets** - Platform-independent pointer system using offsets
- **Header/Body Separation** - Metadata in headers, bulk data in bodies
- **Progressive Loading** - Headers load immediately, bodies load asynchronously
- **Compression Support** - ZIP compression for large assets
- **Memory Optimization** - Page touching and virtual memory locking

### Asset Types Overview

| Asset Type | Format | Purpose | Key Features |
|------------|--------|---------|--------------|
| **Graphics** | I2D/I3D | 2D/3D imagery | Progressive loading, states, animations |
| **Audio** | WAV/Resource | Sound effects | DirectSound integration, CD audio |
| **Maps** | Sector | World data | Dynamic loading, walkmaps, objects |
| **Multi** | Resource | Collections | Offset arrays, multiple asset types |

## Core Concepts

### Binary Buffer Philosophy

All asset files use a **single binary buffer** approach where:
- Files are loaded entirely into memory as one contiguous buffer
- Data structures use relative offsets instead of absolute pointers
- The entire file becomes a self-contained memory space
- No additional file I/O during runtime access

```cpp
// Single buffer loading
ptr = (char *)malloc(frh.objsize);  // Allocate for entire file
fread(ptr, frh.datasize, 1, fl);    // Load all data at once
```

### Relative Offset System

Instead of absolute pointers, the system uses **relative offsets**:

```cpp
// Offset-based access
void *Object(int32_t i) { return offsets[i]; }
PTBitmap Bitmap(int32_t i) { return (PTBitmap)(void *)offsets[i]; }
PTAnimation Animation(int32_t i) { return (PTAnimation)(void *)offsets[i]; }
```

**Benefits:**
- **Platform Independence** - No absolute pointer dependencies
- **Memory Efficiency** - Single allocation per file
- **Fast Access** - Direct memory access via offsets
- **Flexibility** - Multiple asset types in single files

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
- `QE_LOADED` - Successfully loaded
- `QE_FAILED` - Load failed

**Background Thread System:**
```cpp
static void BeginLoaderThread();
static void EndLoaderThread();
static uint_fast32_t LoaderThread(void *args);
static void PauseLoader();
static void ResumeLoader();
```

### Resource File Format

**FileResHdr Structure:**
```cpp
struct FileResHdr
{
    uint32_t resmagic;    // Magic number: 'CGSR'
    uint16_t topbm;       // Number of bitmaps in file
    uint8_t  comptype;    // Compression type
    uint8_t  version;     // File format version
    uint32_t datasize;    // Size of data section
    uint32_t objsize;     // Total object size
    uint32_t hdrsize;     // Size of header section
};
```

**File Structure:**
```
[FileResHdr] - Resource file header
[Header Section] - Optional header data (hdrsize bytes)
[Bitmap Table] - Array of offsets to bitmaps (if topbm > 0)
[Data Section] - Binary asset data (datasize bytes)
```

### Memory Management Strategy

**Buffer Lifecycle:**
1. **Loading** - File read into single buffer
2. **Processing** - Offsets converted to pointers
3. **Usage** - Direct memory access via offsets
4. **Cleanup** - Buffer freed when no longer needed

**Optimization Features:**
- **Page Touching** - Forces VM system to keep pages loaded
- **Virtual Memory Locking** - Locks memory in physical RAM (optional)
- **Reference Counting** - Tracks usage for automatic cleanup

### Caching System

**Chunk Cache:**
- Memory-based caching for tile and asset chunks
- Configurable size based on available physical memory
- LRU eviction policy
- Compression support

**Cache Configuration:**
```cpp
// Automatic sizing based on available memory
int32_t physmegs = TotalPhys() / (1024 * 1024);
if (physmegs < 16)
    ChunkCacheSize = 2;
else if (physmegs <= 24)
    ChunkCacheSize = 3;
else if (physmegs <= 32)
    ChunkCacheSize = 4;
else
    ChunkCacheSize = min(physmegs - 32, 16);
```

### Compression System

**Supported Types:**
- `COMP_NONE` - No compression
- `COMP_ZIP` - ZIP implode compression

**Usage:**
- Headers are never compressed
- Bodies may be compressed based on file type
- Automatic decompression during loading

### Asset Loading Methods

**Method 1: Direct Resource Loading**
```cpp
void *LoadResource(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
```

**Method 2: Header-Only Loading**
```cpp
void *LoadResourceHeader(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
```

**Method 3: Body-Only Loading**
```cpp
void *LoadResourceBinary(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
```

### Multi-Asset Collections

**TMulti Class:**
- Manages collections of related assets
- Single buffer with offset table
- Supports animations, bitmaps, fonts, wave data

**Access Methods:**
```cpp
void *Object(int32_t i) { return offsets[i]; }
void *Object(char *name);
PTAnimation Animation(int32_t i);
PTBitmap Bitmap(int32_t i);
TFont* Font(int32_t i);
```

### Error Handling

**Validation Mechanisms:**
- **Magic Number** - Ensures valid resource files ('CGSR')
- **Version Check** - Prevents loading incompatible files
- **Size Validation** - Prevents buffer overflows
- **Corruption Detection** - Sanity checks for data integrity

**Fallback Strategies:**
- **Missing Resources** - Graceful handling with defaults
- **Load Failures** - Error reporting and recovery
- **Memory Failures** - Graceful handling of out-of-memory conditions

### Performance Optimizations

**Loading Optimizations:**
- **Progressive Loading** - Headers first, bodies later
- **Background Threads** - Asynchronous loading
- **Compression** - Reduced file sizes and I/O
- **Caching** - Keep frequently used assets in memory

**Runtime Optimizations:**
- **Color Depth Conversion** - Automatic format optimization
- **Memory Access Patterns** - Sequential loading, random access
- **Page Alignment** - Optimized memory operations

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

### Configuration Management

**Path System:**
```cpp
TString ClassDefPath;    // Class definitions
TString ExileRCPath;     // Resource compiler path
TString ResourcePath;    // Main resource directory
TString BaseMapPath;     // Base map files
TString CurMapPath;      // Current map files
```

**Performance Settings:**
- **Chunk Cache Size** - Configurable based on available memory
- **Preload Settings** - Optional sector preloading
- **Loading Threads** - Background loading configuration

This conceptual foundation provides the context needed to understand the detailed asset system specifications that follow. The system's design emphasizes performance, flexibility, and cross-platform compatibility while maintaining efficient memory usage and loading times.

## Core Architecture

### Binary Buffer Loading with Relative Offsets

The asset system loads files as complete binary buffers into memory, then uses relative offsets to access different parts of the data. This approach provides:

- **Fast Access**: Direct memory access without file I/O overhead
- **Efficient Memory Usage**: Single buffer per resource file
- **Flexible Data Layout**: Multiple asset types in single files
- **Offset-Based Navigation**: Relative pointers within loaded buffers

## Key Components

### 1. Resource Loading System

**TMulti** (`src/multi.h`)
- **Purpose**: Multiple resource array class for managing collections of assets
- **Structure**: Big buffer of resources accessed by offset array
- **Usage**: `TMulti::LoadMulti(filename)` loads entire resource files

**LoadResource Functions** (`src/resource.h`)
```cpp
void *LoadResource(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
void *LoadResourceHeader(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
void *LoadResourceBinary(const char *name, int32_t id = -1, uint32_t *ressize = nullptr);
```

### 2. Resource File Format

**FileResHdr Structure** (`src/resourcehdr.h`)
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

**File Structure**:
1. **FileResHdr**: File header with metadata
2. **Header Section**: Optional header data (hdrsize bytes)
3. **Bitmap Table**: Array of uint32_t offsets to bitmaps (if topbm > 0)
4. **Data Section**: Binary asset data (datasize bytes)

### 3. Imagery System

**TObjectImagery** (`src/imagery.h`)
- **Purpose**: Manages I2D and I3D file formats
- **Features**: Progressive loading, caching, state management
- **Structure**: Header + body separation for efficient loading

**Imagery Loading States**:
- `QE_NONE`: Not loaded
- `QE_QUEUED`: Queued for loading
- `QE_LOADING`: Currently loading
- `QE_LOADED`: Successfully loaded
- `QE_FAILED`: Load failed

### 4. Chunk Cache System

**TChunkCache** (`src/chunkcache.h`)
- **Purpose**: Memory-based caching for tile and asset chunks
- **Features**: Configurable size, LRU eviction, compression support
- **Memory Management**: Automatic allocation based on available physical memory

**Cache Configuration**:
```cpp
// From revmain.cpp - automatic sizing
int32_t physmegs = TotalPhys() / (1024 * 1024);
if (physmegs < 16)
    ChunkCacheSize = 2;
else if (physmegs <= 24)
    ChunkCacheSize = 3;
else if (physmegs <= 32)
    ChunkCacheSize = 4;
else
    ChunkCacheSize = min(physmegs - 32, 16);
```

## Loading Process

### 1. Resource File Loading

**Step 1: File Validation**
```cpp
// Check magic number and version
if (frh.resmagic != RESMAGIC)  // 'CGSR'
    FatalError(errnotcgs, filename);
if (frh.version < RESVERSION || frh.version > RESVERSION)
    FatalError(erroldver, filename);
```

**Step 2: Memory Allocation**
```cpp
// Allocate buffer for entire resource
ptr = (char *)malloc(frh.objsize);
```

**Step 3: Data Loading**
```cpp
// Load header section (if any)
if (frh.hdrsize > 0)
    fseek(fl, frh.hdrsize + sizeof(FileResHdr), 0);

// Load bitmap offset table (if any)
if (frh.topbm) {
    bitmaptable = (uint32_t *)malloc(sizeof(uint32_t) * frh.topbm);
    fread(bitmaptable, sizeof(uint32_t), frh.topbm, fl);
}

// Load main data section
fread(ptr, frh.datasize, 1, fl);
```

**Step 4: Runtime Processing**
```cpp
// Convert bitmaps to 16-bit if necessary
if (frh.topbm) {
    for (int32_t i = 0; i < frh.topbm; i++) {
        PTBitmap bm = (PTBitmap)(ptr + bitmaptable[i]);
        if (bm->flags & BM_15BIT)
            Convert15to16(bm);
        if (bm->flags & BM_8BIT)
            ConvertPal15to16(bm);
    }
}
```

### 2. Progressive Loading

**Background Thread System**:
- **Loader Thread**: Asynchronous loading of imagery bodies
- **Event-Driven**: Uses Windows events for thread synchronization
- **Queue Management**: Loads queued resources in background

**Thread Management**:
```cpp
static void BeginLoaderThread();
static void EndLoaderThread();
static uint_fast32_t LoaderThread(void *args);
static void PauseLoader();
static void ResumeLoader();
```

### 3. Offset-Based Access

**TMulti Object Access**:
```cpp
void *Object(int32_t i) { return offsets[i]; }
void *Object(char *name);

PTAnimation Animation(int32_t i) { return (PTAnimation)(void *)offsets[i]; }
PTBitmap Bitmap(int32_t i) { return (PTBitmap)(void *)offsets[i]; }
TFont* Font(int32_t i) { return (TFont*)(void *)offsets[i]; }
```

## Memory Management

### 1. Buffer Lifecycle

**Loading Phase**:
1. File opened and header read
2. Memory allocated for entire resource
3. Data loaded into buffer
4. Offset table processed
5. Runtime conversions applied

**Usage Phase**:
- Direct memory access via offsets
- No additional file I/O
- Virtual memory paging for large resources

**Cleanup Phase**:
- Buffer freed when no longer needed
- Reference counting for shared resources

### 2. Memory Optimization

**Page Touching**:
```cpp
// Touch resource to force VM system to keep pages loaded
char *m = ptr;
for (int32_t c = 0; c < (int32_t)frh.datasize; c += 2048, m += 2048) {
    int32_t dummy = *m;
}
```

**Virtual Memory Locking**:
```cpp
// Lock memory in physical RAM (commented out in current code)
// VirtualLock(chunkbuffer, CHUNKWIDTH * CHUNKHEIGHT * numchunks);
```

## File Formats

### 1. Resource Files (.DAT)

**Structure**:
- Binary format with custom header
- Support for compression (ZIP implode)
- Multiple asset types per file
- Version control for compatibility

**Naming Convention**:
- `filename.DAT` - Single resource file
- `filename.000` through `filename.999` - Split resource files

### 2. Imagery Files (I2D/I3D)

**I2D Format**: 2D imagery with animation states
**I3D Format**: 3D models with skeletal data

**Structure**:
- Header section with metadata
- Body section with actual asset data
- Progressive loading support

### 3. Multi Files

**Purpose**: Collections of related assets
**Structure**: Single buffer with offset table
**Types**: Animations, bitmaps, fonts, wave data

## Performance Features

### 1. Caching Strategies

**Chunk Cache**:
- Configurable size based on available memory
- LRU eviction policy
- Separate caches for 8-bit and 16-bit data

**Imagery Cache**:
- Header/body separation for efficient loading
- Use count tracking for memory management
- Background loading for non-critical assets

### 2. Compression Support

**Types**:
- `COMP_NONE`: No compression
- `COMP_ZIP`: ZIP implode compression

**Decompression**:
```cpp
int32_t TChunkCache::ChunkDecompress(void *source, void *dest, uint32_t clear);
int32_t TChunkCache::ChunkDecompressZ(void *source, void *dest, uint32_t clear);
```

### 3. Runtime Optimizations

**Color Depth Conversion**:
- Automatic 15-bit to 16-bit conversion
- Palette-based 8-bit to 16-bit conversion
- Runtime format optimization

**Memory Access Patterns**:
- Sequential access for loading
- Random access for asset retrieval
- Page-aligned memory operations

## Error Handling

### 1. File Validation

**Magic Number Check**: Ensures valid resource files
**Version Compatibility**: Prevents loading incompatible files
**Size Validation**: Prevents buffer overflows
**Corruption Detection**: Sanity checks for bitmap data

### 2. Memory Management

**Allocation Failure**: Graceful handling of out-of-memory conditions
**Load Failure**: Fallback mechanisms for missing resources
**Thread Safety**: Critical sections for shared resource access

## Configuration

### 1. Path Management

**Resource Paths** (from `revmain.cpp`):
```cpp
TString ClassDefPath;    // Class definitions
TString ExileRCPath;     // Resource compiler path
TString ResourcePath;    // Main resource directory
TString BaseMapPath;     // Base map files
TString CurMapPath;      // Current map files
```

### 2. Performance Settings

**Chunk Cache Size**: Configurable based on available memory
**Preload Settings**: Optional sector preloading
**Loading Threads**: Background loading configuration

## Porting Considerations

### 1. Platform Abstraction

**File I/O**: Replace Windows-specific file operations
**Memory Management**: Adapt to platform memory constraints
**Threading**: Replace Windows threading with platform equivalents

### 2. Modern Optimizations

**Memory Mapping**: Use mmap() for large files
**Async I/O**: Modern async file operations
**Compression**: Modern compression algorithms
**Streaming**: Progressive loading improvements

This asset system represents a sophisticated approach to resource management for its time, with many concepts that remain relevant in modern game development. The binary buffer approach with relative offsets provides excellent performance while maintaining flexibility for different asset types. 