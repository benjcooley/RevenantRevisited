#!/usr/bin/env python3
"""
Debug Missing Fields - Find exactly what we're not parsing
"""

import re

def debug_all_lines():
    """Show EVERY line from 501-583 and identify what we're missing"""
    print("🔍 DEBUGGING EVERY LINE 501-583")
    print("=" * 70)
    
    with open('src/character.h', 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    field_count = 0
    
    for i, line in enumerate(lines):
        line_num = i + 1
        
        # Only show lines 501-583
        if line_num < 501 or line_num > 583:
            continue
            
        original_line = line.rstrip()
        stripped = line.strip()
        
        # Skip obvious non-fields
        if (not stripped or stripped.startswith('//') or stripped.startswith('/*') 
            or stripped.startswith('*') or stripped == '}' or 'Data members' in stripped
            or '--' in stripped):
            print(f"{line_num:3d}: [SKIP] {original_line}")
            continue
        
        # Check if this looks like a field declaration
        has_semicolon = ';' in stripped
        has_type_pattern = re.search(r'\b(bool|int32_t|uint32_t|float|char|\w+)\s+\w+', stripped)
        is_access_modifier = stripped in ['public:', 'private:', 'protected:']
        
        if has_semicolon and has_type_pattern and not is_access_modifier:
            field_count += 1
            # Try to parse it
            patterns = [
                r'(bool|char|int32_t|uint32_t|float|double)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;',
                r'(P\w+|\w+\*)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;', 
                r'([A-Z]\w+)\s+(\w+)\s*(?:\[([^\]]+)\])?\s*;',
                r'(\w+)\s+(\w+)\s*\[([^\]]+)\]\s*;',
            ]
            
            parsed = False
            for pattern in patterns:
                match = re.search(pattern, stripped)
                if match:
                    field_type = match.group(1)
                    field_name = match.group(2)
                    array_size = match.group(3) if len(match.groups()) > 2 else None
                    array_info = f"[{array_size}]" if array_size else ""
                    print(f"{line_num:3d}: [FIELD {field_count:2d}] {field_name:20} {field_type:15} {array_info}")
                    parsed = True
                    break
            
            if not parsed:
                print(f"{line_num:3d}: [MISSED FIELD!] {stripped}")
        else:
            print(f"{line_num:3d}: [OTHER] {original_line}")
    
    print(f"\n📊 Found {field_count} potential field declarations")

def show_specific_lines():
    """Show the specific lines we know should contain major fields"""
    print("\n🎯 CHECKING SPECIFIC MAJOR FIELDS")
    print("=" * 50)
    
    with open('src/character.h', 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    # Check specific lines we know are important
    key_lines = [542, 564]  # TSpellManager, hasseen array
    
    for line_num in key_lines:
        if line_num - 1 < len(lines):
            line = lines[line_num - 1].strip()
            print(f"Line {line_num}: {line}")
            
            # Try to extract field info
            if 'TSpellManager' in line:
                print("   → This should be TSpellManager SpellManager (large embedded object!)")
            elif 'hasseen' in line:
                print("   → This should be SHasSeen hasseen[MAXHASSEEN] (96 byte array!)")

if __name__ == "__main__":
    debug_all_lines()
    show_specific_lines()
