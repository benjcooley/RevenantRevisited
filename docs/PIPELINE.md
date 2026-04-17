# Ghidra to Readable Code Pipeline

## Clean 3-Stage Architecture

### Stage 1: Pure Ghidra Output
**Source:** `RevenantGhidra/`
- Original decompiled files with minimal naming
- Files like: `cls_0x401000.cpp`, `cls_0x5a5320.cpp`
- **Never modify this folder** - it's our ground truth

### Stage 2: Structure-Fixed C++
**Process:** `RevenantGhidra/` → `Classes_Converted/`
**Script:** `scripts/convert_structs.py`
- Fix C++ syntax for VS Code compatibility
- Split into .h/.cpp files
- Remove OOAnalyzer namespace
- **Keep original Ghidra names** - no semantic renaming yet

### Stage 3: Readable Code
**Process:** `Classes_Converted/` + `mappings/` → `Classes_Readable/`
**Script:** `scripts/rename_classes.py`
- Apply meaningful names from mapping files
- Cross-file type inference and renaming
- **Output uses source class names** (TCharacter.cpp, etc.)

## File Flow
```
RevenantGhidra/cls_0x5a5320.cpp
    ↓ (Phase 1: Structure Fix)
Classes_Converted/cls_0x5a5320.cpp + cls_0x5a5320.h
    ↓ (Phase 2: Semantic Rename + mappings/TCharacter_cls_0x5a5320_map.yaml)
Classes_Readable/TCharacter.cpp
```

## Benefits
- **Clean inputs:** Pure Ghidra names preserved
- **Iterative:** Can re-run Phase 2 with updated mappings  
- **Safe:** Always can backtrack to RevenantGhidra/
- **Flexible:** Mapping files control final output
