# Revenant Class Refactoring Automation

## Overview
Automated system to match decompiled classes in `/Classes` with source classes in `/src`, then safely rename classes, fields, and methods using VS Code's C++ refactoring engine.

## Goals
1. **Class Identification**: Match large game classes between `/src` and `/Classes`
2. **Field Restoration**: Rename `mbr_0xNNN` fields to meaningful names
3. **Method Restoration**: Rename `meth_0xNNN` methods to actual method names
4. **Safe Refactoring**: Use VS Code's context-aware C++ refactoring
5. **Incremental Progress**: Batch processing with cancel/proceed control

## System Architecture

### File Structure
```
/scripts/
  - analyze_layouts.py (main analysis script)
  - generate_tasks.py (creates YAML task files)

/refactoring/
  - config.yaml (batch settings)
  - tasks.yaml (rename operations)
  - report.md (analysis results)

.vscode/
  - tasks.json (VS Code commands)
```

### Analysis Phase
1. **Two-pass parsing of `/src`**:
   - Pass 1: Parse all struct/class definitions into type dictionary
   - Pass 2: Recursively expand embedded structs into flat field lists
2. **Parse `/Classes` layouts**: Extract offset, size, type, name from OOAnalyzer output
3. **Match classes**: Compare layouts, calculate similarity scores
4. **Generate reports**: Markdown reports with confidence scores and mappings

### Refactoring Phase
1. **Generate task files**: YAML files with rename operations
2. **VS Code execution**: Process tasks in batches with user control
3. **Manual commits**: Git commits between batches for rollback safety

## YAML Task Format

### Config File (`/refactoring/config.yaml`)
```yaml
batch_size: 5
pause_between_batches: 2
confirm_between_batches: true
```

### Tasks File (`/refactoring/tasks.yaml`)
```yaml
tasks:
  # Global class renames across entire codebase
  - type: "class_rename"
    old_names: "cls_0x5a5320 cls_0x5b8e94 cls_0x45f7c0"
    new_names: "TCharacter TObjectInstance TObjectClass"

  # Global field renames across entire codebase
  - type: "field_rename"
    old_names: "mbr_0x70 mbr_0x74 mbr_0x78 mbr_0x7c"
    new_names: "health mana fatigue armor"

  # Class-specific method renames
  - type: "method_rename"
    class: "cls_0x5a5320"
    old_names: "meth_0x44e930 meth_0x44ed50 meth_0x44f140"
    new_names: "Damage RestoreHealth CalculateDamage"
```

## VS Code Integration

### Requirements
- VS Code with C/C++ extension
- clangd language server (for context-aware refactoring)
- Python for analysis scripts

### Execution Flow
1. **Analysis**: Run Python script → generates `tasks.yaml` and `report.md`
2. **Review**: Read report, verify matches, adjust batch sizes if needed
3. **Execution**: VS Code command reads `tasks.yaml`, processes in batches
4. **Control**: User can continue, skip batch, or cancel at any point

### Batch Processing
- Process `batch_size` tasks at a time
- Show progress: "Processing batch X/Y: class_rename (3 classes)"
- User prompt: "Continue? (y/n/cancel)"
- Pause between batches to avoid overwhelming VS Code

## Confidence and Safety

### Thresholds
- **98% confidence** required for automatic renaming
- Manual review of all matches before execution
- Git commits between batches for rollback safety

### Focus
- **Large game classes only** (skip collections, stubs, minor classes)
- **Conservative approach**: Better to miss matches than make incorrect renames

## Future Extensions

### Potential Additional Refactoring Types
```yaml
# Parameter renames (if needed)
- type: "parameter_rename"
  class: "cls_0x5a5320"
  method: "meth_0x44e930"
  old_names: "param_1 param_2"
  new_names: "damage damagetype"

# Local variable renames (if needed)
- type: "local_rename"
  class: "cls_0x5a5320"
  method: "meth_0x44e930"
  old_names: "local_1 local_2"
  new_names: "health_before health_after"
```

### Notes
- Parameter and local variable renaming may not be necessary
- Method names should provide sufficient readability
- Focus on high-impact renames (classes, fields, methods)

## Implementation Plan

### Phase 1: Core Analysis
1. Implement two-pass `/src` layout parser
2. Implement `/Classes` layout parser
3. Implement matching algorithm
4. Generate basic reports

### Phase 2: VS Code Integration
1. Test VS Code refactoring capabilities
2. Implement task file reader
3. Implement batch processing
4. Add user control (continue/skip/cancel)

### Phase 3: Refinement
1. Tune batch sizes based on performance
2. Improve confidence scoring
3. Add error handling and recovery
4. Optimize for large codebases

## Key Principles

### Lean and Focused
- No extra features or "nice-to-haves"
- Focus on core functionality only
- Easy to modify and extend

### Safety First
- High confidence thresholds
- Manual review required
- Git-based rollback strategy

### Incremental Progress
- Can run analysis multiple times
- Configurable batch sizes
- User control over execution

### Simplicity
- YAML for human-readable task files
- Markdown for reports
- Standard VS Code integration 