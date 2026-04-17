#!/usr/bin/env python3
"""
Verify Ghidra Inheritance Flattening - Confirm base classes = 276 bytes
"""

import sys
from pathlib import Path
sys.path.append('scripts')
from recursive_fingerprinter import RecursiveFingerprinter

def calculate_base_classes_size():
    """Calculate exact size of flattened base classes to match Ghidra's 276 bytes"""
    print("🔍 CALCULATING BASE CLASS SIZES TO MATCH GHIDRA")
    print("=" * 60)
    
    # SObjectDef - from object.h lines 609-628
    print("📋 SObjectDef fields:")
    sobjectdef_size = 0
    sobjectdef_fields = [
        ('objclass', 2),    ('objtype', 2),     ('flags', 4),
        ('state', 2),       ('level', 2),       ('pos', 12),
        ('vel', 12),        ('accum', 12),      ('rotatex', 1),
        ('rotatey', 1),     ('rotatez', 1),     ('group', 1),
    ]
    for name, size in sobjectdef_fields:
        sobjectdef_size += size
        print(f"  {name:12} {size:2d} bytes")
    print(f"  Total SObjectDef: {sobjectdef_size} bytes")
    
    # TObjectInstance additional fields - from object.h lines 1177-1194
    print("\n📋 TObjectInstance additional fields:")
    tobjectinstance_size = 0
    tobjectinstance_fields = [
        ('name', 4),            ('notifyflags', 4),     ('mapindex', 4),
        ('sector', 4),          ('cl', 4),              ('inf', 4),
        ('shadow', 4),          ('imagery', 4),         ('animator', 4),
        ('frame', 2),           ('framerate', 2),       ('prevstate', 2),
        ('prevframe', 2),       ('owner', 4),
        # Additional fields that might be missing:
        ('lightdef', 12),       # SLightDef structure
        ('script', 4),          # Script pointer
        ('moveangle', 4),       ('movevert', 4),        ('movedist', 4),
        ('movebits', 4),        ('stats', 32),          # Statistics block
        ('oldpos', 12),         ('screenx', 4),         ('screeny', 4),
        ('screenz', 4),         ('invindex', 4),        ('inventnum', 4),
    ]
    for name, size in tobjectinstance_fields:
        tobjectinstance_size += size
        print(f"  {name:12} {size:2d} bytes")
    print(f"  Total TObjectInstance: {tobjectinstance_size} bytes")
    
    # TComplexObject additional fields - from complexobj.h lines 214-216
    print("\n📋 TComplexObject additional fields:")
    tcomplexobject_size = 0
    tcomplexobject_fields = [
        ('doing', 4),       ('desired', 4),     ('root', 4),
    ]
    for name, size in tcomplexobject_fields:
        tcomplexobject_size += size
        print(f"  {name:12} {size:2d} bytes")
    print(f"  Total TComplexObject: {tcomplexobject_size} bytes")
    
    total_base_size = sobjectdef_size + tobjectinstance_size + tcomplexobject_size
    print(f"\n" + "=" * 60)
    print(f"📊 TOTAL BASE CLASSES: {total_base_size} bytes")
    print(f"🎯 GHIDRA SHOWS: 276 bytes")
    print(f"📈 DIFFERENCE: {276 - total_base_size} bytes")
    
    if total_base_size == 276:
        print("✅ PERFECT MATCH!")
    elif abs(total_base_size - 276) <= 8:
        print("✅ CLOSE MATCH (within padding)")
    else:
        print("❌ SIGNIFICANT DIFFERENCE - need to investigate")
    
    return total_base_size

def check_character_own_fields():
    """Check TCharacter's own fields starting from offset 276"""
    print("\n🔍 CHECKING TCHARACTER OWN FIELDS (from offset 276)")
    print("=" * 60)
    
    rf = RecursiveFingerprinter()
    result = rf.fingerprint_source_class(Path('src/character.h'), 'TCharacter')
    
    if result:
        print(f"✅ TCharacter own fields: {result['field_count']} fields, {result['total_size']} bytes")
        print(f"   Pattern: {result['field_pattern']}")
        
        # Total should be 276 (base) + TCharacter own = ~1636 bytes
        expected_total = 276 + result['total_size']
        print(f"📊 Expected total: 276 + {result['total_size']} = {expected_total} bytes")
        print(f"🎯 Ghidra shows: 1636 bytes")
        print(f"📈 Difference: {1636 - expected_total} bytes")
        
        if abs(expected_total - 1636) <= 50:
            print("✅ REASONABLE MATCH!")
        else:
            print("❌ SIGNIFICANT DIFFERENCE")
    else:
        print("❌ Failed to fingerprint TCharacter")

if __name__ == "__main__":
    base_size = calculate_base_classes_size()
    check_character_own_fields()
