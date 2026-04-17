# Revenant Architecture Documentation

## Overview

Revenant is built using a modular, object-oriented architecture designed for a late-1990s Windows game with DirectX 5. The codebase follows a classic game engine pattern with clear separation between systems, extensive use of inheritance, and a sophisticated resource management system.

## Core Architecture

### Entry Points

**Main Entry Point**: `src/revmain.cpp` - `WinMain()`
- Initializes system components (DirectX, graphics, input)
- Sets up game directories and configuration
- Creates main window and display system
- Starts the game loop via `TScreen::ShowScreen()`

**Game Loop**: `src/screen.cpp` - `TScreen::ShowScreen()`
- Manages screen transitions and state
- Handles input processing (mouse, keyboard, joystick)
- Controls rendering pipeline (background, animation, overlay)
- Manages game timing and frame rate

### Key Core Classes

#### 1. Display & Graphics System

**TDisplay** (`src/display.h`)
- **Purpose**: Manages the 2.5D rendering system with Z-buffer tiles
- **Key Features**:
  - Front/back buffer management for double buffering
  - Z-buffer handling with hardware/software fallbacks
  - Dirty rectangle system for efficient updates
  - Support for Voodoo cards and other hardware variations
- **Rendering Approach**: 
  - 2D tiles with embedded Z-buffer data
  - Layered grid 3D renderer (different tiles in different image buffers)
  - Pixel-accurate Z-intersections for 3D/2D integration

**T3DScene** (`src/3dscene.h`)
- **Purpose**: Handles 3D character and object rendering
- **Features**: 
  - 3D character models with full rigs (non-skinned)
  - Complex animation system with many animations
  - Depth buffer integration with 2D background
  - Real-time lighting and shadows

#### 2. Screen Management System

**TScreen** (`src/screen.h`)
- **Purpose**: Base class for all game screens
- **Features**:
  - Pane management system (up to 32 panes per screen)
  - Exclusive pane handling for modal dialogs
  - Input routing to appropriate panes
  - Frame-based timing and animation

**TPane** (`src/screen.h`)
- **Purpose**: Virtual view panes within screens
- **Features**:
  - Independent clipping rectangles and origins
  - Scrolling support with smooth transitions
  - Background buffer management
  - Input handling (mouse, keyboard, joystick)

**TPlayScreen** (`src/playscreen.h`)
- **Purpose**: Main game screen with combat and exploration
- **Key Features**:
  - Game command system (GAMECOMMAND enum)
  - Combat system integration
  - Post-character animation effects
  - Game time and day/night cycle management

#### 3. Object System

**TObjectInstance** (`src/object.h`)
- **Purpose**: Base class for all game objects
- **Key Features**:
  - 3D positioning and movement system
  - State machine for animations and behaviors
  - Inventory system with slots
  - Statistics and object properties
  - Scripting system integration
  - Lighting and shadow support

**TObjectClass** (`src/object.h`)
- **Purpose**: Defines object types and their properties
- **Object Classes**:
  - `OBJCLASS_PLAYER`, `OBJCLASS_CHARACTER` - Characters and NPCs
  - `OBJCLASS_WEAPON`, `OBJCLASS_ARMOR` - Equipment
  - `OBJCLASS_TALISMAN` - Magic system components
  - `OBJCLASS_TILE` - Map tiles with walkmaps
  - `OBJCLASS_EFFECT` - Visual and spell effects

**TObjectBuilder** (`src/object.h`)
- **Purpose**: Factory pattern for object creation
- **Features**: Dynamic object instantiation based on type definitions

#### 4. Combat System

**Combat Mechanics** (from `src/playscreen.h`)
- **Opportunistic Matching**: First moves that match requirements are selected, then randomly picked
- **Requirements**: Character locations, facing directions, proximity
- **Move Types**:
  - Standard attacks (RPG-style)
  - Custom attacks (keyboard/joypad combos)
  - Combo system (GAMECMD_COMBO1 through GAMECMD_COMBO12)
  - Special moves (dodge, block, sneak attacks)

**Training System**
- Master Jong (named after Jong Beum Yoon, business partner)
- Teaches fighting moves and combos
- Regular training sessions in the game

#### 5. Map & World System

**Sector-Based Loading** (`src/revmain.cpp`)
- **Dynamic Loading**: Sectors loaded as map scrolls
- **Preload System**: Optional sector caching for performance
- **Walkmap System**: Grid of heights and walkable areas per tile

**Tile System**
- **Z-Buffer Tiles**: 2D images with embedded depth data
- **3D Placement**: Tiles can be placed anywhere in 3D space
- **Isometric Grid**: 2D isometric 3/4 view with 3D positioning
- **Sector Files**: Map divided into loadable sector files

#### 6. Resource Management

**TMulti** (`src/multi.h`)
- **Purpose**: Multi-resource file format for game assets
- **Features**: Compression, streaming, caching

**TChunkCache** (`src/chunkcache.h`)
- **Purpose**: Tile and asset caching system
- **Features**: Memory-based caching with configurable size

**TImagery** (`src/imagery.h`)
- **Purpose**: Manages I2D and I3D file formats
- **Features**: Animation states, transitions, still images

#### 7. Input System

**TControlMap** (`src/ctrlmap.h`)
- **Purpose**: Maps keys and joystick inputs to game commands
- **Features**: Configurable bindings, multiple input devices

**DirectInput Integration** (`src/directinput.cpp`)
- **Purpose**: Windows DirectInput for joystick and gamepad support
- **Features**: Device enumeration, input processing

#### 8. Scripting System

**TScriptManager** (`src/script.h`)
- **Purpose**: Text-based scripting system for game logic
- **Features**:
  - Game command execution
  - In-game editor access
  - Text panel interface
  - Object behavior scripting

#### 9. Audio System

**TSoundPlayer** (`src/sound.h`)
- **Purpose**: Music and sound effects management
- **Features**:
  - DirectSound integration
  - CD audio support
  - Speech wave file playback
  - 3D positional audio

## Design Patterns

### 1. Factory Pattern
- **TObjectBuilder**: Creates objects based on type definitions
- **Screen Factory**: Creates different screen types (play, menu, etc.)

### 2. Observer Pattern
- **Notification System**: Objects can notify others of state changes
- **Event Handling**: Input events propagate through the system

### 3. State Machine Pattern
- **Object States**: Objects have multiple states with transitions
- **Animation States**: Complex animation state management
- **Game States**: Screen transitions and game flow

### 4. Resource Pool Pattern
- **Surface Pooling**: Reuses graphics surfaces
- **Object Pooling**: Reuses game objects
- **Buffer Pooling**: Reuses memory buffers

## File Organization

### Source Structure
```
src/
├── revmain.cpp          # Main entry point and system initialization
├── screen.h/cpp         # Screen and pane management
├── display.h/cpp        # Graphics and rendering system
├── object.h/cpp         # Object system and game entities
├── playscreen.h/cpp     # Main game screen
├── mappane.h/cpp        # Map rendering and interaction
├── animation.h/cpp      # Animation system
├── imagery.h/cpp        # Asset loading (I2D/I3D formats)
├── multi.h/cpp          # Multi-resource file format
├── script.h/cpp         # Scripting system
├── sound.h/cpp          # Audio system
├── directinput.cpp      # Input handling
└── [other systems...]
```

### Revenant Directory
```
Revenant/
├── [Original source files]
├── exile2.map          # Linker map showing DLL dependencies
├── Exile.map           # Additional linker information
└── [Backup and variant files]
```

## Configuration & Options

### Build-Time Options (`src/revmain.cpp`)
- **Graphics**: `UseDirect3D2`, `UseSoftware3D`, `UseBlue`
- **Performance**: `CacheExBufs`, `PreloadSectors`, `ChunkCacheSize`
- **Debugging**: `ShowZBuffer`, `ShowNormalBuffer`, `Debug`

### Runtime Options (INI Configuration)
- **Paths**: ClassDefPath, ExileRCPath, ResourcePath, BaseMapPath, CurMapPath
- **Lighting**: MaxLights, Ambient3D, LightRange3D
- **Gameplay**: DoubleTapTicks, ViolenceLevel, GameSpeed

### Hardware Detection
- **MMX Support**: CPU feature detection for optimizations
- **Graphics Cards**: Voodoo-specific optimizations
- **Z-Buffer**: Hardware vs. software fallback detection

## Memory Management

### Custom Allocators
- **xmalloc/xrealloc/xfree**: Custom memory management with debugging
- **Critical Sections**: Thread-safe memory operations
- **Memory Status**: Real-time memory usage monitoring

### Resource Management
- **Surface Pooling**: Reuses DirectDraw surfaces
- **Object Caching**: Caches frequently used objects
- **Sector Preloading**: Configurable sector caching

## Rendering Pipeline

### 2.5D Rendering System
1. **Background Rendering**: Z-buffer tiles drawn to background buffers
2. **3D Object Rendering**: Characters and objects rendered with depth
3. **Z-Intersection**: 3D/2D integration using depth buffer
4. **Layering**: Multiple image buffers composited back-to-front
5. **Lighting**: Real-time lighting applied to 3D objects

### Performance Optimizations
- **Dirty Rectangle System**: Only updates changed screen areas
- **Frame Skipping**: Configurable frame rate control
- **Hardware Acceleration**: Direct3D and MMX optimizations
- **Memory Management**: Efficient surface and buffer reuse

## Game Systems

### Combat System
- **Opportunistic Matching**: AI-driven move selection
- **Combo System**: Complex fighting game-style combinations
- **Training**: Progressive skill development with Master Jong
- **Weapon Types**: Melee, ranged, and magical weapons

### Magic System
- **Talismans**: Collectible items for spell crafting
- **Spell Creation**: Arrange talismans to create new spells
- **Effects**: Visual and gameplay effects system

### World System
- **Sector Loading**: Dynamic map loading as player moves
- **Pathfinding**: Simple but effective monster pathfinding
- **Spawn System**: Monster generators with path connections
- **Day/Night Cycle**: Dynamic lighting and time system

### UI System
- **Text-Based UI**: Simple widget system using text files
- **In-Game Editor**: Hotkey-accessible level editor
- **Multi-Pane Interface**: Modular UI components
- **Dialog System**: Character interaction and storytelling

## Technical Innovations

### 1998 Era Innovations
- **2.5D Rendering**: Hybrid 2D/3D approach before it was common
- **Z-Buffer Tiles**: Pre-computed depth data for performance
- **MMX Optimizations**: Early CPU instruction set usage
- **Hardware Abstraction**: Support for multiple graphics cards
- **Dynamic Loading**: Sector-based streaming before it was standard

### Architecture Strengths
- **Modular Design**: Clear separation of concerns
- **Extensible**: Easy to add new object types and behaviors
- **Performance**: Optimized for 1998 hardware constraints
- **Cross-Platform Ready**: Well-abstracted platform dependencies

This architecture represents a sophisticated game engine for its time, with many concepts that would become standard in later game development. The modular design and clear separation of systems make it an excellent candidate for modernization and cross-platform porting. 