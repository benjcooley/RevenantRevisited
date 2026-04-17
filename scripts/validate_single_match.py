#!/usr/bin/env python3
"""
Single Match Validator - Test one known match at a time
Start simple, verify each step works before scaling up
"""

import sys
import re
from pathlib import Path

def test_tcharacter_parsing():
    """Test parsing of TCharacter specifically - we know this should work"""
    print("🔍 TESTING TCHARACTER PARSING")
    print("=" * 50)
    
    # Read the character.h file
    char_file = Path('src/character.h')
    if not char_file.exists():
        print("❌ character.h not found")
        return False
        
    with open(char_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    print(f"📄 File size: {len(content)} chars")
    
    # Look for the class definition manually
    # We know it starts around line 56 and data members at line 501
    lines = content.split('\n')
    print(f"📄 Total lines: {len(lines)}")
    
    # Find the class start
    class_start = None
    for i, line in enumerate(lines):
        if 'class TCharacter' in line:
            class_start = i
            break
    
    if class_start is None:
        print("❌ Could not find 'class TCharacter'")
        return False
        
    print(f"✅ Found class TCharacter at line {class_start + 1}")
    print(f"   Line content: {lines[class_start].strip()}")
    
    # Find the data members section (around line 501)
    data_start = None
    for i in range(500, min(len(lines), 520)):
        line = lines[i].strip()
        if line and not line.startswith('//') and ('bool ' in line or 'int32_t ' in line):
            data_start = i
            break
    
    if data_start is None:
        print("❌ Could not find data members section around line 501")
        return False
        
    print(f"✅ Found data members starting at line {data_start + 1}")
    print(f"   Line content: {lines[data_start].strip()}")
    
    # Count the data members manually
    field_count = 0
    for i in range(data_start, len(lines)):
        line = lines[i].strip()
        if line == '};':  # End of class
            break
        if line and not line.startswith('//') and not line.startswith('/*') and not line.startswith('*'):
            # Simple field patterns
            if re.search(r'(bool|int32_t|uint32_t|float|char|\w+\*)\s+\w+\s*[;\[]', line):
                field_count += 1
                if field_count <= 10:  # Show first 10
                    print(f"   Field {field_count}: {line}")
    
    print(f"✅ Manually counted {field_count} data members")
    return field_count > 30  # Should be 40+

def test_ghidra_tcharacter():
    """Test parsing a known TCharacter Ghidra file"""
    print("\n🔍 TESTING GHIDRA TCHARACTER PARSING")
    print("=" * 50)
    
    # Test the smaller TCharacter file first
    ghidra_file = Path('Classes/cls_0x5a7b98_TCharacter.cpp')
    if not ghidra_file.exists():
        print("❌ Ghidra TCharacter file not found")
        return False
    
    with open(ghidra_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    print(f"📄 File size: {len(content)} chars")
    
    # Look for the structure definition
    struct_match = re.search(r'Structure\s+(\w+)\s*\{(.*?)\}', content, re.DOTALL)
    if not struct_match:
        print("❌ No structure definition found")
        return False
        
    struct_name = struct_match.group(1)
    struct_body = struct_match.group(2)
    
    print(f"✅ Found structure: {struct_name}")
    
    # Parse fields
    field_pattern = r'^\s*(\d+)\s+\w+\s+(\d+)\s+(\w+)'
    fields = []
    
    for line in struct_body.split('\n'):
        match = re.match(field_pattern, line.strip())
        if match:
            offset = int(match.group(1))
            size = int(match.group(2))
            name = match.group(3)
            fields.append((offset, size, name))
    
    print(f"✅ Found {len(fields)} fields in Ghidra structure")
    
    # Show first few fields
    for i, (offset, size, name) in enumerate(fields[:5]):
        print(f"   Field {i+1}: offset={offset}, size={size}, name={name}")
    
    return len(fields) > 50  # Should be 80+

def compare_fingerprints():
    """Compare the patterns we should get"""
    print("\n🎯 FINGERPRINT COMPARISON")
    print("=" * 50)
    
    src_ok = test_tcharacter_parsing()
    ghidra_ok = test_ghidra_tcharacter()
    
    if src_ok and ghidra_ok:
        print("✅ Both source and Ghidra parsing work")
        print("💡 The fingerprinting system should be able to match these")
        return True
    else:
        print("❌ One or both parsing methods failed")
        print("🔧 Need to fix parsing before fingerprinting can work")
        return False

if __name__ == "__main__":
    success = compare_fingerprints()
    
    if success:
        print("\n🚀 NEXT STEP: Fix fingerprinting system to use this parsing logic")
    else:
        print("\n🔧 NEXT STEP: Fix the parsing issues identified above")
