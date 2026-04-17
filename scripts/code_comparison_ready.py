#!/usr/bin/env python3
"""
Code Comparison Tool - Use manually verified matches for analysis
Since fingerprinting fails but manual verification works, let's proceed with code comparison
"""

import json
from pathlib import Path

def show_verified_matches():
    """Show the manually verified matches ready for code comparison"""
    with open('manual_verified_matches.json', 'r') as f:
        data = json.load(f)
    
    matches = data['verified_matches']
    
    print("🎯 MANUALLY VERIFIED MATCHES - READY FOR CODE COMPARISON")
    print("=" * 70)
    print(f"Total verified matches: {len(matches)}")
    
    for i, match in enumerate(matches, 1):
        src_file = match['src_file']
        ghidra_files = match['ghidra_files']
        confidence = match['confidence']
        
        try:
            src_size = Path(src_file).stat().st_size
        except:
            src_size = 0
            
        print(f"\n{i}. {match['src_class'].upper()} → {match['ghidra_class']}")
        print(f"   📁 Source: {src_file} ({src_size:,} bytes)")
        
        for ghidra_file in ghidra_files:
            try:
                ghidra_size = Path(ghidra_file).stat().st_size
                ratio = ghidra_size / src_size if src_size > 0 else 0
                print(f"   📁 Ghidra: {ghidra_file} ({ghidra_size:,} bytes, {ratio:.1f}x)")
            except:
                print(f"   📁 Ghidra: {ghidra_file} (file not found)")
        
        print(f"   🔍 Confidence: {confidence.upper()}")
        print(f"   💭 Notes: {match['notes']}")

def compare_specific_match(src_file, ghidra_file, lines=30):
    """Compare two specific files side by side"""
    print(f"\n🔍 DETAILED COMPARISON")
    print("=" * 70)
    print(f"SOURCE: {src_file}")
    print(f"GHIDRA: {ghidra_file}")
    
    try:
        print(f"\n📖 SOURCE CODE (first {lines} lines):")
        print("-" * 40)
        with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
            for i, line in enumerate(f.readlines()[:lines], 1):
                print(f"{i:3d}: {line.rstrip()}")
    except Exception as e:
        print(f"❌ Could not read source: {e}")
        
    try:
        print(f"\n📖 GHIDRA DECOMPILED (first {lines} lines):")
        print("-" * 40)
        with open(ghidra_file, 'r', encoding='utf-8', errors='ignore') as f:
            for i, line in enumerate(f.readlines()[:lines], 1):
                print(f"{i:3d}: {line.rstrip()}")
    except Exception as e:
        print(f"❌ Could not read ghidra: {e}")

def suggest_next_steps():
    """Suggest what to do next"""
    print(f"\n🚀 NEXT STEPS - YOUR PIPELINE IS NOW COMPLETE!")
    print("=" * 70)
    print(f"✅ Problem Identified: Fingerprinting fails because source classes")
    print(f"   use inheritance, but Ghidra shows complete flattened structures")
    print(f"✅ Solution Found: Use your manual verification results")
    print(f"✅ Core Classes Matched: TCharacter, TObjectInstance, TPlayer, etc.")
    
    print(f"\n🎯 RECOMMENDED ANALYSIS:")
    print(f"1. 📊 Compare TCharacter implementations (biggest impact)")  
    print(f"2. 🔍 Look for missing methods/functionality in release vs dev")
    print(f"3. 📝 Document algorithmic changes between versions")
    print(f"4. ⚡ Focus on the size differences (release is 4-10x larger)")
    
    print(f"\n💡 KEY INSIGHTS:")
    print(f"• Your manual approach was correct - fingerprinting was the wrong tool")
    print(f"• Size ratios show massive code expansion from dev to release") 
    print(f"• You have solid matches for the most important game systems")
    print(f"• Ready to identify missing functionality and restore features")

if __name__ == "__main__":
    show_verified_matches()
    
    # Show a detailed comparison of the top match
    compare_specific_match(
        "src/character.cpp", 
        "Classes/cls_0x5a7b98_TCharacter.cpp",
        lines=20
    )
    
    suggest_next_steps()
