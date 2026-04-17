#!/usr/bin/env python3
"""
Realistic Size Calculator - Use proper sizes to match 1636 bytes
"""

def calculate_realistic_tcharacter_size():
    """Calculate TCharacter size with realistic embedded object sizes"""
    print("🔍 CALCULATING REALISTIC TCHARACTER SIZES")
    print("=" * 60)
    
    # Fields we found with realistic size estimates
    fields = [
        # Basic fields (lines 501-540)
        ('autocombat', 'bool', 1),
        ('movetopos', 'bool', 1), 
        ('movepos', 'S3DPoint', 12),
        ('chardata', 'PSCharData', 4),
        ('waittype', 'int32_t', 4),
        ('waitticks', 'int32_t', 4),
        ('forcecommanddone', 'bool', 1),
        ('forcenomove', 'bool', 1),
        ('charflags', 'uint32_t', 4),
        ('exittimestamp', 'int32_t', 4),
        ('is_invisible', 'bool', 1),
        ('shovedir', 'int32_t', 4),
        ('nextattack', 'int32_t', 4),
        ('glimpse', 'int32_t', 4),
        ('noise', 'int32_t', 4),
        ('lasthealthrecov', 'int32_t', 4),
        ('lastfatiguerecov', 'int32_t', 4),
        ('lastmanarecov', 'int32_t', 4),
        ('lastpoisondamage', 'int32_t', 4),
        ('lastattack', 'PSCharAttackData', 4),
        ('lastattackticks', 'int32_t', 4),
        ('chainhits', 'int32_t', 4),
        
        # MAJOR EMBEDDED OBJECT - TSpellManager (line 542)
        # This is likely HUGE - could be 800+ bytes for a complex spell system
        ('SpellManager', 'TSpellManager', 800),  # Much larger estimate!
        
        ('invokedelay', 'int32_t', 4),
        ('oldab', 'PTActionBlock', 4),
        ('magic_resistance', 'float', 4),
        ('fade', 'int32_t', 4),
        ('fade_step', 'int32_t', 4),
        ('fade_limit', 'int32_t', 4),
        ('invisible_spell', 'bool', 1),
        ('teleport_level', 'int32_t', 4),
        ('teleport_position', 'S3DPoint', 12),
        ('burning', 'TObjectInstance*', 4),
        
        # MAJOR ARRAY - SHasSeen array (line 564)  
        ('hasseen', 'SHasSeen[MAXHASSEEN]', 96),  # 8 * 12 bytes
        
        ('snapticks', 'int32_t', 4),
        ('combatflashticks', 'int32_t', 4),
        ('lastbowshot', 'int32_t', 4),
        ('target_last_position', 'S3DPoint', 12),
        ('last_position_count', 'int32_t', 4),
        ('target_out_of_sight', 'bool', 1),
        ('last_position_distance', 'int32_t', 4),
        ('last_position_start_point', 'S3DPoint', 12),
        ('target_last_angle', 'int32_t', 4),
    ]
    
    # Calculate sizes and pattern
    total_size = 0
    pattern = ""
    
    print("Key fields with realistic sizes:")
    for i, (name, type_name, size) in enumerate(fields):
        if size >= 32 or 'SpellManager' in name or 'hasseen' in name:  # Show large fields
            print(f"  {name:25} {type_name:20} {size:4d} bytes")
        
        total_size += size
        
        # Generate pattern character
        if size == 1:
            pattern += 'b'
        elif size == 2:
            pattern += 'w' 
        elif size == 4:
            pattern += 'd'
        elif size == 8:
            pattern += 'q'
        elif size >= 32:
            pattern += 'A'  # Large embedded object/array
        else:
            pattern += 'A'
    
    print(f"\n📊 REALISTIC TCHARACTER RESULTS:")
    print(f"   Fields: {len(fields)}")
    print(f"   Size: {total_size} bytes")
    print(f"   Pattern: {pattern}")
    print(f"   Pattern length: {len(pattern)}")
    
    # Total with base classes
    base_size = 276
    total_with_base = base_size + total_size
    ghidra_size = 1636
    
    print(f"\n📈 SIZE COMPARISON:")
    print(f"   Base classes: {base_size} bytes")
    print(f"   TCharacter: {total_size} bytes")
    print(f"   Total expected: {total_with_base} bytes")
    print(f"   Ghidra actual: {ghidra_size} bytes") 
    print(f"   Difference: {ghidra_size - total_with_base} bytes")
    
    # Compare with user expected pattern
    user_expected = "bbwddddddbbwddbbwddddddddddd" + "ddddddbbwdddddA" + "dddddddbbwddddd"
    print(f"\n🎯 USER EXPECTED PATTERN:")
    print(f"   {user_expected}")
    print(f"   Length: {len(user_expected)}")
    
    if abs(total_with_base - ghidra_size) <= 200:
        print("✅ EXCELLENT SIZE MATCH!")
    elif abs(total_with_base - ghidra_size) <= 400:
        print("✅ GOOD SIZE MATCH!")
    else:
        print("❌ Still need to adjust sizes")
        
        # If still too small, suggest TSpellManager is even bigger
        if total_with_base < ghidra_size:
            needed = ghidra_size - total_with_base
            print(f"💡 Suggestion: TSpellManager might be {800 + needed} bytes instead of 800")

if __name__ == "__main__":
    calculate_realistic_tcharacter_size()
