# Ghidra to Source Class Mappings

## File Format: `<SourceClass>_<GhidraClass>_map.yaml`

### YAML Structure:
```yaml
# Required metadata
source_class: "TCharacter"        # Final output class name
ghidra_class: "cls_0x5a5320"      # Original Ghidra class name
confidence: "HIGH"                # HIGH/MEDIUM/LOW

# Field mappings (within this class scope)
fields:
  mbr_0x70: "health"
  mbr_0x74: "mana"
  mbr_0x78: "fatigue"

# Method mappings (within this class scope) 
methods:
  virt_meth_0x44d5c0: "CalculateDamage"
  meth_0x45aec0: "Initialize"

# Optional: Known parameter types for cross-class renaming
parameters:
  # method_name: {param_name: class_type}
  CalculateDamage:
    pTarget: "TCharacter"
    pWeapon: "TItem"
```

## Usage:
1. **Phase 2 script** loads all mapping files from this directory
2. **Global symbol table** built from all mappings
3. **Cross-file renaming** applied using parameter type information
4. **Output files** named using `source_class` value

## Safety Rules:
- **Fields/Methods**: Only renamed within their defining class file
- **Parameters**: Type-scoped renaming within method scope
- **Global Classes**: Class names replaced everywhere
