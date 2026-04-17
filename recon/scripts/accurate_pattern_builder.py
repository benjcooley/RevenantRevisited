#!/usr/bin/env python3
"""
Accurate Pattern Builder - Use real TObjectInstance pattern from user
"""

def build_accurate_complete_pattern():
    """Build complete pattern using user's accurate TObjectInstance pattern"""
    print("🔍 BUILDING ACCURATE COMPLETE PATTERN")
    print("=" * 70)
    
    # PART 1: SObjectDef pattern (still need to estimate this)
    print("📋 PART 1: SObjectDef")
    # From object.h lines 609-628: objclass(2), objtype(2), flags(4), state(2), level(2), 
    # pos(12), vel(12), accum(12), rotatex(1), rotatey(1), rotatez(1), group(1)
    sobjectdef_pattern = "wwdwwAAAbbbb"  # 52 bytes total
    print(f"  SObjectDef pattern: {sobjectdef_pattern}")
    print(f"  SObjectDef length: {len(sobjectdef_pattern)}")
    
    # PART 2: TObjectInstance pattern (USER PROVIDED - ACCURATE!)
    print(f"\n📋 PART 2: TObjectInstance (USER PROVIDED)")
    # User's accurate pattern: dddddddddwwdwwdddddwwbbwd(lightdef)(TStatisticList)dddddddddd
    
    # Break down the pattern
    tobjectinstance_base = "dddddddddwwdwwdddddwwbbwd"  # Basic fields
    lightdef_part = "A"  # SLightDef embedded structure  
    tstatlist_part = "A"  # TStatisticList embedded structure
    tobjectinstance_end = "dddddddddd"  # Final fields
    
    tobjectinstance_pattern = tobjectinstance_base + lightdef_part + tstatlist_part + tobjectinstance_end
    print(f"  TObjectInstance pattern: {tobjectinstance_pattern}")
    print(f"  TObjectInstance length: {len(tobjectinstance_pattern)}")
    print(f"  Breakdown:")
    print(f"    Base fields: {tobjectinstance_base} ({len(tobjectinstance_base)})")
    print(f"    SLightDef: {lightdef_part}")
    print(f"    TStatisticList: {tstatlist_part}")
    print(f"    Final fields: {tobjectinstance_end} ({len(tobjectinstance_end)})")
    
    # PART 3: TComplexObject pattern
    print(f"\n📋 PART 3: TComplexObject")
    tcomplexobject_pattern = "ddd"  # doing, desired, root (3 * 4-byte pointers)
    print(f"  TComplexObject pattern: {tcomplexobject_pattern}")
    print(f"  TComplexObject length: {len(tcomplexobject_pattern)}")
    
    # PART 4: TCharacter's own fields
    print(f"\n📋 PART 4: TCharacter Own Fields")
    # From our previous analysis: 43 fields with realistic sizes
    tcharacter_pattern = "bbAdddbbddbdddddddddddAddddddbdAdAdddAdbdAd"
    print(f"  TCharacter pattern: {tcharacter_pattern}")
    print(f"  TCharacter length: {len(tcharacter_pattern)}")
    
    # COMBINE ALL PATTERNS
    print(f"\n" + "=" * 70)
    complete_pattern = sobjectdef_pattern + tobjectinstance_pattern + tcomplexobject_pattern + tcharacter_pattern
    
    print(f"✅ COMPLETE ACCURATE PATTERN:")
    print(f"   SObjectDef:      {sobjectdef_pattern}")
    print(f"   TObjectInstance: {tobjectinstance_pattern}")  
    print(f"   TComplexObject:  {tcomplexobject_pattern}")
    print(f"   TCharacter:      {tcharacter_pattern}")
    print(f"   ──────────────────────────────────────────")
    print(f"   COMPLETE:        {complete_pattern}")
    print(f"   Total length:    {len(complete_pattern)}")
    
    # Compare with user expected pattern
    user_expected = "bbwddddddbbwddbbwddddddddddd" + "ddddddbbwdddddA" + "dddddddbbwddddd"
    print(f"\n🎯 USER EXPECTED PATTERN:")
    print(f"   Pattern: {user_expected}")
    print(f"   Length: {len(user_expected)}")
    
    # Analysis
    min_len = min(len(complete_pattern), len(user_expected))
    matches = sum(1 for i in range(min_len) if complete_pattern[i] == user_expected[i])
    similarity = matches / min_len if min_len > 0 else 0
    
    print(f"\n📊 COMPARISON WITH USER EXPECTED:")
    print(f"   Similarity: {matches}/{min_len} = {similarity:.1%}")
    print(f"   Length difference: {len(complete_pattern)} vs {len(user_expected)}")
    
    if len(complete_pattern) >= len(user_expected) and similarity > 0.6:
        print("✅ EXCELLENT MATCH!")
    elif len(complete_pattern) >= len(user_expected) and similarity > 0.4:
        print("✅ GOOD MATCH!")
    else:
        print("🔧 Need refinement")
        
    # Validate size calculation
    print(f"\n📏 SIZE VALIDATION:")
    pattern_size = calculate_pattern_size(complete_pattern)
    print(f"   Pattern implies: {pattern_size} bytes")
    print(f"   Ghidra shows: 1636 bytes")
    print(f"   Difference: {1636 - pattern_size} bytes")
    
    return complete_pattern

def calculate_pattern_size(pattern: str) -> int:
    """Calculate total size from pattern"""
    size = 0
    for char in pattern:
        if char == 'b': size += 1
        elif char == 'w': size += 2  
        elif char == 'd': size += 4
        elif char == 'q': size += 8
        elif char == 'A': size += 32  # Conservative estimate for large objects
    return size

if __name__ == "__main__":
    pattern = build_accurate_complete_pattern()
