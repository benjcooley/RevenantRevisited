#!/usr/bin/env python3
"""
Driver Script: Complete Ghidra to Readable Code Pipeline
Runs both Phase 1 (structure fix) and Phase 2 (semantic renaming)
"""

import os
import sys
import subprocess
from pathlib import Path

def run_phase1():
    """Phase 1: Fix Ghidra C++ structure"""
    print("=== Phase 1: Structure Conversion ===")
    
    script_path = Path("scripts/convert_structs.py")
    if not script_path.exists():
        print(f"Error: {script_path} not found")
        return False
    
    try:
        result = subprocess.run([sys.executable, str(script_path)], 
                              capture_output=True, text=True, cwd=".")
        if result.returncode == 0:
            print("Phase 1 completed successfully")
            return True
        else:
            print(f"Phase 1 failed: {result.stderr}")
            return False
    except Exception as e:
        print(f"Error running Phase 1: {e}")
        return False

def run_phase2():
    """Phase 2: Semantic renaming"""
    print("\n=== Phase 2: Semantic Renaming ===")
    
    script_path = Path("scripts/rename_classes.py")
    if not script_path.exists():
        print(f"Error: {script_path} not found")
        return False
    
    try:
        result = subprocess.run([sys.executable, str(script_path)], 
                              capture_output=True, text=True, cwd=".")
        if result.returncode == 0:
            print("Phase 2 completed successfully")
            return True
        else:
            print(f"Phase 2 failed: {result.stderr}")
            return False
    except Exception as e:
        print(f"Error running Phase 2: {e}")
        return False

def main():
    """Run complete conversion pipeline"""
    print("Revenant Ghidra to Readable Code Conversion")
    print("=" * 50)
    
    # Check input directories
    if not Path("RevenantGhidra").exists():
        print("Error: RevenantGhidra/ directory not found")
        return 1
    
    if not Path("mappings").exists():
        print("Error: mappings/ directory not found")
        return 1
    
    mapping_count = len(list(Path("mappings").glob("*_map.yaml")))
    print(f"Found {mapping_count} mapping files")
    
    if mapping_count == 0:
        print("Warning: No mapping files found - Phase 2 will be skipped")
    
    # Run Phase 1
    if not run_phase1():
        print("Pipeline failed at Phase 1")
        return 1
    
    # Run Phase 2 (if mappings exist)
    if mapping_count > 0:
        if not run_phase2():
            print("Pipeline failed at Phase 2")
            return 1
    else:
        print("\nSkipping Phase 2 - no mapping files")
    
    print("\n=== Pipeline Complete ===")
    print(f"Results:")
    print(f"  Classes_Converted/: {len(list(Path('Classes_Converted').glob('*.cpp')))} files")
    if Path("Classes_Readable").exists():
        print(f"  Classes_Readable/:  {len(list(Path('Classes_Readable').glob('*.cpp')))} files")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
