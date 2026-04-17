#!/usr/bin/env python3
"""
Complete TCharacter Parser - Parse ALL fields from line 501-583
"""

import re
from pathlib import Path

def parse_complete_tcharacter():
    """Parse TCharacter completely - every single field from 501-583"""
    print("🔍 PARSING COMPLETE TCHARACTER - EVERY FIELD")
    print("=" * 70)
    
    with open('src/character.h', 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    fields = []
    in_data_section = False
    
    for i, line in enumerate(lines):
        line_num = i + 1
        original_line = line
        line = line.strip()
        
        # Start parsing at data members section
        if line_num < 501:
            continue
        
        # End of class
        if line == '};':
            print(f"📍 End of class at line {line_num}")
            break
            
        # Skip comments and empty lines
        if not line or line.startswith('//') or line.startswith('/*') or line.startswith('*'):
            continue
        
        # Skip section comments
        if 'Data members' in line or '--' in line:
            print(f"📋 Section: {line}")
            continue
            
        # Enhanced field parsing patterns
        field_patterns = [
            # Basic types
            r'(bool|char|int32_t|uint32_t|float|double)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;',
            # Pointer types  
            r'(P\w+|\w+\*)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;',
            # Custom types (like S3DPoint, TSpellManager)
            r'([A-Z]\w+)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;',
            # Array declarations
            r'(\w+)\s+(\w+)\s*\[([^\]]+)\]\s*;',
        ]
        
        field_found = False
        for pattern in field_patterns:
            match = re.search(pattern, line)
            if match:
                field_type = match.group(1)
                field_name = match.group(2)
                array_size = match.group(3) if len(match.groups()) > 2 else None
                
                # Calculate field size
                size = estimate_field_size(field_type, array_size, line)
                fields.append((field_name, field_type, size, line_num, line))
                
                # Show progress
                if len(fields) <= 20 or len(fields) % 5 == 0:  
                    array_info = f"[{array_size}]" if array_size else ""
                    print(f"  Field {len(fields):2d}: {field_name:20} {field_type:15} {size:3d}b {array_info} [line {line_num}]")
                
                field_found = True
                break
        
        # If no pattern matched but line looks like a field, warn
        if not field_found and ';' in line and not line.startswith('//'):
            if 'public:' not in line and 'private:' not in line and 'protected:' not in line:
                print(f"  ⚠️  MISSED: {line[:50]}... [line {line_num}]")
    
    # Calculate pattern  
    pattern = ""
    total_size = 0
    for field_name, field_type, size, line_num, line in fields:
        if size == 1:
            pattern += 'b'
        elif size == 2:
            pattern += 'w'
        elif size == 4:
            pattern += 'd' 
        elif size == 8:
            pattern += 'q'
        elif size >= 32:
            pattern += 'A'  # Large embedded object or array
        else:
            pattern += 'A'  # Default for unknown
        total_size += size
    
    print(f"\n" + "=" * 70)
    print(f"✅ COMPLETE TCHARACTER PARSING RESULTS:")
    print(f"   Total fields: {len(fields)}")
    print(f"   Total size: {total_size} bytes")
    print(f"   Pattern: {pattern}")
    print(f"   Pattern length: {len(pattern)}")
    
    # Compare with user's expected pattern
    user_expected = "bbwddddddbbwddbbwddddddddddd" + "ddddddbbwdddddA" + "dddddddbbwddddd"
    print(f"\n🎯 USER EXPECTED:")
    print(f"   Pattern: {user_expected}")
    print(f"   Length: {len(user_expected)}")
    
    # Compare patterns
    min_len = min(len(pattern), len(user_expected))
    matches = sum(1 for i in range(min_len) if pattern[i] == user_expected[i])
    similarity = matches / min_len if min_len > 0 else 0
    
    print(f"\n📊 COMPARISON:")
    print(f"   Similarity: {matches}/{min_len} = {similarity:.1%}")
    
    # Total with base classes
    total_with_base = 276 + total_size
    ghidra_total = 1636
    print(f"\n📈 TOTAL SIZE CHECK:")
    print(f"   Base classes: 276 bytes")
    print(f"   TCharacter: {total_size} bytes") 
    print(f"   Expected total: {total_with_base} bytes")
    print(f"   Ghidra shows: {ghidra_total} bytes")
    print(f"   Difference: {ghidra_total - total_with_base} bytes")
    
    if abs(total_with_base - ghidra_total) <= 100:
        print("✅ EXCELLENT MATCH!")
    elif abs(total_with_base - ghidra_total) <= 300:
        print("✅ GOOD MATCH (within padding)")
    else:
        print("❌ Still significant difference")
    
    return pattern, fields

def estimate_field_size(field_type: str, array_size: str, full_line: str) -> int:
    """Enhanced field size estimation"""
    
    # Basic type sizes
    type_sizes = {
        'bool': 1,
        'char': 1,
        'int32_t': 4,
        'uint32_t': 4,
        'float': 4,
        'double': 8,
    }
    
    # Handle arrays
    if array_size:
        if 'MAXHASSEEN' in array_size:
            # SHasSeen hasseen[MAXHASSEEN] - 8 * 12 bytes each
            return 96
        elif array_size.isdigit():
            base_size = type_sizes.get(field_type, 4)
            return base_size * int(array_size)
        else:
            return 64  # Large unknown array
    
    # Custom type sizes
    custom_sizes = {
        'S3DPoint': 12,           # 3 * int32_t
        'SHasSeen': 12,           # 3 fields
        'TSpellManager': 200,     # Large embedded object - estimate
        'PSCharData': 4,          # Pointer
        'PSCharAttackData': 4,    # Pointer
        'PTActionBlock': 4,       # Pointer
        'TObjectInstance': 4,     # Pointer (not embedded)
    }
    
    # Handle pointers (P prefix or *)
    if field_type.startswith('P') or '*' in field_type:
        return 4
    
    return custom_sizes.get(field_type, type_sizes.get(field_type, 4))

if __name__ == "__main__":
    pattern, fields = parse_complete_tcharacter()
