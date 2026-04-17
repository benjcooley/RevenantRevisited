# Ghidra Class Identifications

## Overview

This document tracks the identification of classes from the Ghidra decompilation of Revenant.exe. As we analyze the binary and cross-reference with the source code, we're systematically identifying which classes correspond to which game systems.

## Identified Classes

### Core Game Systems

| Ghidra Class | Likely Source Class | Size | Evidence |
|--------------|-------------------|------|----------|
| `cls_0x5a5320` | **TCharacter** | 2576 bytes | - 112-byte base class matches TObjectInstance<br>- Massive size matches character complexity<br>- Coordinate calculations and rendering methods<br>- Hundreds of member variables for stats/inventory |
| `cls_0x5a5ed4` | **TObjectInstance** | 112 bytes | - Base class with virtual function table<br>- Embedded in larger classes<br>- Matches source code TObjectInstance size |
| `cls_0x5b8e94` | **Software Renderer** | 1352 bytes | - 220 methods (template-heavy code)<br>- Added later in development<br>- Rendering-related functionality<br>- Coordinate calculations |
| `cls_0x5a50e8` | **TScreen** | 276 bytes | - Base screen class size<br>- Virtual function table<br>- Pane management arrays |
| `cls_0x5a7b98` | **TPlayScreen** | 1636 bytes | - Starts with 276-byte TScreen base class<br>- Game state management<br>- Input handling methods |

### Inheritance Relationships

```
TPlayScreen (cls_0x5a7b98, 1636 bytes)
├── TScreen (cls_0x5a50e8, 276 bytes)
    └── Base screen functionality

TCharacter (cls_0x5a5320, 2576 bytes)  
├── TObjectInstance (cls_0x5a5ed4, 112 bytes)
    └── Base object functionality
```

## Identification Methodology

### Size-Based Analysis
- **Largest classes** = Core game systems (renderer, character, screen)
- **Medium classes** = Major subsystems (UI, audio, networking)
- **Small classes** = Utility classes and helpers

### Inheritance Pattern Matching
- **Base class sizes** are consistent between source and binary
- **Embedded classes** at offset 0 indicate inheritance
- **Virtual function tables** help identify base classes

### Method Count Analysis
- **Template-heavy classes** (like renderers) have hundreds of methods
- **Simple utility classes** have few methods
- **Complex systems** have many methods with varied functionality

### Cross-Reference Strategy
1. **Start with largest classes** - Most important systems
2. **Match base class sizes** - Inheritance patterns
3. **Analyze method patterns** - Functionality clues
4. **Compare with source code** - Structure validation

## File Naming Convention

Identified classes are renamed using the pattern:
```
cls_0x[ADDRESS]_likely_[CLASSNAME].cpp
```

Examples:
- `cls_0x5a5320_likely_TCharacter.cpp`
- `cls_0x5b8e94_likely_SoftwareRenderer.cpp`
- `cls_0x5a50e8_likely_TScreen.cpp`

## Next Steps

### Priority Classes to Identify
1. **TPlayer** - Should inherit from TCharacter
2. **T3DScene** - 3D rendering system
3. **TScriptManager** - Scripting system
4. **TMulti** - Resource management
5. **TSector** - Map/level system

### Analysis Techniques
1. **Look for TCharacter inheritance** - Find classes that start with 2576-byte embedded class
2. **Search for rendering patterns** - Classes with many coordinate calculations
3. **Find script-related classes** - Classes with string handling and state management
4. **Identify resource classes** - Classes with file I/O and memory management

## Notes

- **Class evolution was minimal** - Structures match well between source and binary
- **Template code generates massive method counts** - Software renderer has 220 methods
- **Base classes are stable** - TObjectInstance (112 bytes) and TScreen (276 bytes) are consistent
- **Size is a reliable indicator** - Largest classes correspond to most complex systems

## Updates

- **2024-06-26**: Initial identification of 5 major classes
- **2024-06-26**: Established naming convention and methodology 