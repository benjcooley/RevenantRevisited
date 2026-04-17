# Class Mapping Analysis - Revenant Revisited

This document maps the decompiled classes in the `Classes/` folder to their corresponding source files in the `src/` folder, based on file size analysis and content examination.

## Large Classes (Primary Game Systems)

### Core Object System
- **Classes/cls_0x45f7c0_TObjectClass.cpp** (157,413 bytes) → **src/object.cpp** (86,203 bytes)
  - Contains TObjectClass and TObjectInstance implementations
  - Handles object creation, management, and serialization
  - Largest class in the system

### Character System
- **Classes/cls_0x5a5320_TCharacter.cpp** (361,908 bytes) → **src/character.cpp** (136,658 bytes)
  - Main character class implementation
  - Handles character stats, abilities, and behavior

### Screen/UI System
- **Classes/cls_0x5a50e8_TScreen.cpp** (382,360 bytes) → **src/screen.cpp** (22,766 bytes)
  - Screen management and rendering system
  - Note: Size difference suggests multiple screen-related classes combined

### Play Screen
- **Classes/cls_0x5b4f30_TPlayScreen.cpp** (126,351 bytes) → **src/playscreen.cpp** (35,976 bytes)
  - Main gameplay screen implementation

### Graphics System
- **Classes/cls_0x5a35ac_T3DImagery.cpp** (102,627 bytes) → **src/3dimage.cpp** (103,849 bytes)
  - 3D imagery and graphics rendering
  - Very close size match

### Player System
- **Classes/cls_0x5b93c4_TPlayer.cpp** (90,914 bytes) → **src/player.cpp** (19,608 bytes)
  - Player-specific functionality
  - Note: Size difference suggests additional player-related classes

### 3D Scene System
- **Classes/cls_0x411eb0_T3DScene.cpp** (74,523 bytes) → **src/3dscene.cpp** (72,634 bytes)
  - 3D scene management and rendering
  - Very close size match

### 3D Imagery Resource System
- **Classes/cls_0x5a486c_T3DImageryRes.cpp** (73,107 bytes) → **src/3dimage.cpp** (103,849 bytes)
  - 3D imagery resource management
  - Likely part of the larger 3D image system

### Surface System
- **Classes/cls_0x5a68d8_TSurface.cpp** (70,275 bytes) → **src/surface.cpp** (22,908 bytes)
  - Surface rendering and management
  - Note: Size difference suggests multiple surface-related classes

### Object Instance System
- **Classes/cls_0x5b8e94_TObjectInstance_Final.cpp** (232,648 bytes) → **src/object.cpp** (86,203 bytes)
  - **CONFIRMED 100%**: This is the final version of TObjectInstance from the shipped game
  - Struct layout matches source code TObjectInstance up to ~75%, then has additional fields
  - **Inheritance confirmed**: Multiple classes call base methods using `cls_0x5b8e94::virt_meth_0xXXXXXX((cls_0x5b8e94 *)this,param_1);` pattern
  - Represents the "fatter" final version with features added during late development
  - Contains the complete object instance system with all final features
  - **Memory impact**: Despite large size (1352 bytes per instance), impact is modest since only ~9 sectors active at once

### Complex Object System
- **Classes/cls_0x5a9900_TComplexObject.cpp** (6,034 bytes) → **src/complexobj.cpp** (14,161 bytes)
  - **CONFIRMED**: TComplexObject that inherits from TObjectInstance
  - **Inheritance confirmed**: Calls `cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);` (inherits from T3DImageryRes)
  - **Base class calls**: Calls `cls_0x5b8e94::virt_meth_0x4df0f0((cls_0x5b8e94 *)this,param_1);` (inherits from TObjectInstance)
  - **25 virtual functions**: Substantial virtual function table indicating complex behavior
  - **396-byte structure**: Intermediate size between TObjectInstance and TCharacter
  - **Inheritance chain**: TCharacter → TComplexObject → TObjectInstance (confirmed)

## Medium Classes (Secondary Systems)

### Effect System
- **Classes/cls_0x5a7e38.cpp** (58,202 bytes) → **src/effect.cpp** (407,959 bytes)
  - Effect management system
  - Note: Large size difference suggests this is a subset

### Multiplayer System
- **Classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp** (75,139 bytes) → **src/multi.cpp** (815 bytes)
  - Multiplayer functionality
  - Note: Size difference suggests this is a major system

### Network/DirectX System
- **Classes/cls_0x5756d0_likely_TDirectX_or_TNetwork.cpp** (80,639 bytes) → **src/directdraw.cpp** (27,873 bytes)
  - DirectX or network functionality
  - Note: Size difference suggests additional networking code

### Network System (Second)
- **Classes/cls_0x57d9d0_likely_TDirectX_or_TNetwork.cpp** (102,627 bytes) → **src/directdraw.cpp** (27,873 bytes)
  - Additional network/DirectX functionality

## Small Classes (Utility/Helper Classes)

### Text Bar
- **Classes/cls_0x5a4358_TTextBar.cpp** (15,615 bytes) → **src/textbar.cpp** (2,996 bytes)
  - Text bar UI component

### Command System
- **Classes/cls_0x5a7e98.cpp** (19,678 bytes) → **src/command.cpp** (102,817 bytes)
  - Command processing system
  - Note: Size difference suggests this is a subset

### Character Animation
- **Classes/cls_0x5a7dd8.cpp** (8,852 bytes) → **src/charanimator.cpp** (34,165 bytes)
  - Character animation system
  - Note: Size difference suggests this is a subset

### Effect Component
- **Classes/cls_0x5a7e98__vftable_5a7e98.cpp** (301 bytes) → **src/effectcomp.cpp** (47,463 bytes)
  - Effect component virtual function table
  - Very small, likely just VFT

## Virtual Function Tables (VFTs)

Many small files (200-300 bytes) appear to be virtual function tables:
- **Classes/cls_0x5a47f0__vftable_5a47f0.cpp** (291 bytes)
- **Classes/cls_0x5a495c__vftable_5a495c.cpp** (291 bytes)
- **Classes/cls_0x5a5ff0__vftable_5a5ff0.cpp** (291 bytes)
- **Classes/cls_0x5a359c__vftable_5a359c.cpp** (301 bytes)
- **Classes/cls_0x5a4868__vftable_5a4868.cpp** (301 bytes)
- **Classes/cls_0x5a50e4__vftable_5a50e4.cpp** (301 bytes)
- **Classes/cls_0x5a52d8__vftable_5a52d8.cpp** (301 bytes)
- **Classes/cls_0x5a7e98__vftable_5a7e98.cpp** (301 bytes)
- **Classes/cls_0x5b5354__vftable_5b5354.cpp** (301 bytes)
- **Classes/cls_0x5b9f54__vftable_5b9f54.cpp** (301 bytes)
- **Classes/cls_0x5da790__vftable_5da790.cpp** (301 bytes)

## Very Small Classes (Likely Simple Wrappers)

Files under 1KB are likely simple wrapper classes or basic data structures:
- **Classes/cls_0x444e45.cpp** (164 bytes)
- **Classes/cls_0x5da790.cpp** (164 bytes)
- **Classes/cls_0x5e54a4.cpp** (164 bytes)
- **Classes/cls_0x56fef0.cpp** (222 bytes)

## Notes on Size Discrepancies

1. **Decompiled vs Source**: The decompiled classes often appear larger than their source counterparts due to:
   - Additional generated code from decompilation
   - Inline function expansion
   - Compiler optimizations in the original
   - Multiple related classes combined in one file

2. **Missing Classes**: Some large src files don't have obvious matches, suggesting:
   - Classes may be split across multiple decompiled files
   - Some classes may not have been successfully decompiled
   - Some functionality may be in header files or inline

3. **VFT Files**: The many small VFT files suggest a complex inheritance hierarchy with many virtual functions.

## Next Steps

1. **Content Analysis**: Examine the actual function names and structures in the decompiled files
2. **Cross-Reference**: Look for function name patterns that match between decompiled and source
3. **Header Analysis**: Check header files for class definitions that might match
4. **Function Mapping**: Map individual functions between decompiled and source files

## File Size Ranges

- **Large Classes**: 50KB+ (Major game systems)
- **Medium Classes**: 10KB-50KB (Secondary systems)
- **Small Classes**: 1KB-10KB (Utility classes)
- **VFT Files**: 200-500 bytes (Virtual function tables)
- **Tiny Classes**: <1KB (Simple wrappers/data structures) 