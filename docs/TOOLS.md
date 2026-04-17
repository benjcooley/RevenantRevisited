# Development Tools and File Formats

## Overview

This document covers the development tools present in the codebase and the file formats that need to be supported for reading game data. For a preservation port, the focus is on **reading existing data files** rather than rebuilding the original data pipeline.

## Available Resources

### RevenantBin - Game Binary Distribution
**Location:** `RevenantBin/`

Contains the complete game distribution with:
- **Revenant.exe** - The main game executable (2.0MB)
- **Resources/** - Game data files including:
  - `rules.def` - Complete game rules and mechanics
  - `spell.def` - Magic system definitions  
  - `options.def` - Game configuration options
  - `statpane.def` - Statistics panel definitions
  - `master.s` - Master script file
- **Save/** - Save game examples:
  - `Single/New Game1/game.sav` - Binary save game file
  - `Single/New Game1/ss.bmp` - Save game screenshot
- **Imagery/** - Graphics and asset files
- **Music/** - Audio files
- **Support files** - DLLs, configuration files, documentation

### RevenantGhidra - Decompiled Binary Analysis
**Location:** `RevenantGhidra/`

Contains Ghidra decompilation output with:
- **Class structures** - Inferred C++ class layouts from binary analysis
- **Method implementations** - Decompiled function code
- **Virtual function tables** - Object-oriented structure analysis
- **Memory layouts** - Detailed struct member offsets and types

**Key Classes Identified:**
- `cls_0x5a5320` - Large class (2576 bytes) with extensive functionality
- Multiple vtable classes showing inheritance hierarchies
- Various utility and system classes

## Existing Development Tools

### Code Utilities

**CountLines Tool** (`src/countlines.cpp`)
- Simple utility for counting lines in `.def` files
- Used for tracking development progress
- Compiles to standalone executable

**Blit Shader Tool** (`src/blit.cpp`)
- Modern OpenGL shader implementation for texture blitting
- Demonstrates modern rendering approach for the port

### Build System

**Object Builder System**
- Factory pattern for creating game objects
- Defined in `src/object.h` and `src/object.cpp`
- Handles creation of various game entity types

**Imagery Builder System**
- Factory system for creating imagery objects
- Manages 2D graphics and UI elements
- Defined in `src/imagery.h` and `src/imagery.cpp`

**3D Animator Builder System**
- Factory system for 3D animation controllers
- Handles character and object animations
- Defined in `src/3danim.h` and `src/3danim.cpp`

## File Formats to Support

### Game Data Files

**Definition Files (.def)**
- `RULES.DEF` - Game rules and mechanics (available in RevenantBin)
- `SPELL.DEF` - Magic system definitions (available in RevenantBin)
- `AREA.DEF` - Area and level definitions
- `CHAR.DEF` - Character class definitions
- `ITEM.DEF` - Item definitions and properties
- `MONSTER.DEF` - Monster definitions and stats
- `OPTIONS.DEF` - Game configuration options (available in RevenantBin)
- `STATPANE.DEF` - Statistics panel definitions (available in RevenantBin)

**Format:** Text-based configuration files with structured data

### Graphics Files

**Imagery Files (.I2D)**
- 2D graphics and UI elements
- Compressed image data
- Used by `TMulti` resource system

**3D Model Files (.I3D)**
- 3D character and object models
- Animation data
- Texture coordinates and mesh data

**Texture Files (.TN)**
- Texture data for 3D models
- Compressed texture formats

### Save Game Files

**Save Files (.SAV)**
- Complete game state serialization
- Player data, inventory, automap
- World state and object positions
- Script state and variables
- **Example:** `RevenantBin/Save/Single/New Game1/game.sav`

### Resource Files

**Multi Resource Files (.DAT)**
- Packed resource containers
- Graphics, sounds, and other assets
- Managed by `TMulti` system

**Script Files (.S)**
- `master.s` - Master script file (available in RevenantBin)
- Game logic and behavior scripts

## Missing Tools (Not Required for Port)

**ExileRC.exe** - Resource Compiler
- **Status:** Source code not available
- **Purpose:** Originally converted source assets to game formats
- **Impact:** Not needed for preservation port
- **Alternative:** Read existing compiled data files directly

**Data Packers**
- **Status:** Not present in codebase
- **Purpose:** Originally packed loose files into resource containers
- **Impact:** Not needed - use existing packed files

## Modern Port Requirements

### File Reading Capabilities

**Priority 1: Game Data Parsers**
- Parse `.def` files for game configuration (use RevenantBin examples)
- Load save game format for state restoration (use RevenantBin examples)
- Read resource file formats for assets

**Priority 2: Graphics Support**
- Read `.I2D` files for 2D graphics
- Support `.I3D` format for 3D models
- Handle `.TN` texture files

**Priority 3: Resource Management**
- Implement `TMulti` resource loading
- Support `.DAT` resource containers
- Handle asset streaming and caching

### Development Workflow

**Data Analysis**
- Use RevenantBin files as reference implementations
- Cross-reference with RevenantGhidra decompiled structures
- Document format specifications with real examples
- Create test cases with known data from RevenantBin

**Implementation**
- Build file format readers in modern C++
- Use RevenantGhidra class structures as reference
- Implement error handling and validation
- Add logging and debugging support

**Testing**
- Verify data integrity during loading
- Test with all existing game files from RevenantBin
- Validate save game compatibility using provided examples

### Binary Analysis Integration

**Ghidra Decompilation**
- Use inferred class structures to understand data formats
- Cross-reference with source code where available
- Identify missing functionality through binary analysis
- Validate assumptions about file formats

**Reference Implementation**
- Use RevenantBin as the "ground truth" for file formats
- Compare parsed data with expected results
- Validate save game loading with known good saves
- Test resource loading with actual game assets

## Conclusion

The addition of **RevenantBin** and **RevenantGhidra** provides invaluable resources for the preservation port:

1. **Real Data Examples** - Actual game files to test against
2. **Binary Structure Analysis** - Decompiled class layouts and methods
3. **Reference Implementation** - Working game to compare against
4. **Complete Asset Set** - All graphics, sounds, and data files

This combination of source code, binary analysis, and actual game files provides a comprehensive foundation for implementing modern file format readers without needing the original development tools. 