#!/usr/bin/env python3
"""
Quick validation script to examine top class matches
Shows side-by-side comparison of source vs decompiled classes
"""

import json
from pathlib import Path

def load_matches():
    """Load both match result files"""
    with open('class_matches.json') as f:
        simple_matches = json.load(f)
    
    with open('revenant_matches.json') as f:
        structural_matches = json.load(f)
    
    return simple_matches, structural_matches

def show_file_comparison(src_file, ghidra_file, max_lines=50):
    """Show side-by-side comparison of two files"""
    print(f"\n{'='*80}")
    print(f"SOURCE: {src_file}")
    print(f"GHIDRA: {ghidra_file}")
    print(f"{'='*80}")
    
    try:
        with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
            src_lines = f.readlines()[:max_lines]
    except:
        src_lines = ["[FILE NOT FOUND]"]
    
    try:
        with open(ghidra_file, 'r', encoding='utf-8', errors='ignore') as f:
            ghidra_lines = f.readlines()[:max_lines]
    except:
        ghidra_lines = ["[FILE NOT FOUND]"]
    
    # Show first 20 lines of each
    print("\nSOURCE CODE:")
    print("-" * 40)
    for i, line in enumerate(src_lines[:20]):
        print(f"{i+1:2d}: {line.rstrip()}")
    
    print("\nGHIDRA DECOMPILED:")
    print("-" * 40)  
    for i, line in enumerate(ghidra_lines[:20]):
        print(f"{i+1:2d}: {line.rstrip()}")

def validate_top_matches():
    """Validate the top matches from both methods"""
    simple_matches, structural_matches = load_matches()
    
    print("REVENANT CLASS MATCHING - VALIDATION REPORT")
    print("=" * 60)
    
    print("\n🎯 TOP STRUCTURAL MATCHES (Most Reliable)")
    print("-" * 50)
    
    # Show top 5 structural matches
    for i, match in enumerate(structural_matches['matches'][:5]):
        src_class = match['src_class']
        ghidra_class = match['decompiled_class']  
        confidence = match['confidence']
        
        print(f"\n{i+1}. {src_class} → cls_{ghidra_class} (Score: {confidence:.3f})")
        print(f"   Layout: {match['layout_similarity']:.1%}, "
              f"Functional: {match['functional_similarity']:.1%}")
        
        if match['field_mappings']:
            print(f"   Field mappings: {len(match['field_mappings'])} found")
            for src_field, ghidra_field in match['field_mappings'][:3]:
                print(f"     • {src_field} → {ghidra_field}")
    
    print("\n⚡ PERFECT SIZE MATCHES (High Confidence)")
    print("-" * 50)
    
    # Show perfect size matches from simple method
    perfect_matches = []
    for match in simple_matches['matches']:
        if match['size_ratio'] == 1.0:
            perfect_matches.append(match)
    
    for i, match in enumerate(perfect_matches[:5]):
        src = match['src_class']
        ghidra = match['decompiled_class'] 
        size = src['size']
        
        print(f"{i+1}. {src['name']} → cls_{ghidra['name']} ({size} bytes each)")
    
    print(f"\n📊 SUMMARY:")
    print(f"  • Structural matches: {len(structural_matches['matches'])} (conservative)")
    print(f"  • Simple matches: {len(simple_matches['matches'])} (comprehensive)")
    print(f"  • Perfect size matches: {len(perfect_matches)} (very likely correct)")
    
    return structural_matches['matches'][:3]  # Return top 3 for detailed analysis

def examine_match(src_class, ghidra_class):
    """Examine a specific match in detail"""
    src_file = f"src/{src_class}.cpp"
    ghidra_file = f"Classes/cls_{ghidra_class}.cpp"
    
    show_file_comparison(src_file, ghidra_file)

if __name__ == "__main__":
    top_matches = validate_top_matches()
    
    print(f"\n🔍 DETAILED EXAMINATION")
    print("-" * 50)
    
    # Examine top match in detail
    if top_matches:
        match = top_matches[0]
        print(f"\nExamining top match: {match['src_class']} → {match['decompiled_class']}")
        examine_match(match['src_class'], match['decompiled_class'])
        
        print(f"\n💡 NEXT STEPS:")
        print(f"  1. Review the code comparison above")
        print(f"  2. Look for missing functionality in the Ghidra version") 
        print(f"  3. Identify algorithmic differences")
        print(f"  4. Run: python validate_matches.py to see more matches")
        
        print(f"\n🎮 READY FOR CODE COMPARISON PHASE!")
        print(f"Your pipeline is complete and matches are ready for analysis.")
