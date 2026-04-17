#!/usr/bin/env python3
"""
Phase 2: Semantic Renaming Script
Takes Classes_Converted + mappings/ → Classes_Readable/

Applies meaningful names from mapping files using 3-phase approach:
1. Global class name replacements
2. File-scoped field/method renaming  
3. Parameter-scoped cross-class renaming
"""

import os
import re
import yaml
from pathlib import Path
from typing import Dict, List, Set, Optional
from dataclasses import dataclass

@dataclass
class ClassMapping:
    """Represents a single class mapping from YAML"""
    source_class: str
    ghidra_class: str
    confidence: str
    fields: Dict[str, str]
    methods: Dict[str, str]
    parameters: Dict[str, Dict[str, str]]  # method -> {param -> class_type}

class ClassRenamer:
    def __init__(self, input_dir: str = "Classes_Converted", 
                 mappings_dir: str = "mappings",
                 output_dir: str = "Classes_Readable"):
        self.input_dir = Path(input_dir)
        self.mappings_dir = Path(mappings_dir)
        self.output_dir = Path(output_dir)
        self.mappings: Dict[str, ClassMapping] = {}  # ghidra_class -> mapping
        self.class_name_map: Dict[str, str] = {}     # ghidra_class -> source_class
        
    def load_mappings(self):
        """Load all YAML mapping files"""
        print("Loading mapping files...")
        
        for mapping_file in self.mappings_dir.glob("*_map.yaml"):
            try:
                with open(mapping_file, 'r') as f:
                    data = yaml.safe_load(f)
                
                mapping = ClassMapping(
                    source_class=data['source_class'],
                    ghidra_class=data['ghidra_class'],
                    confidence=data.get('confidence', 'MEDIUM'),
                    fields=data.get('fields', {}),
                    methods=data.get('methods', {}),
                    parameters=data.get('parameters', {})
                )
                
                # Debug field loading
                fields = data.get('fields', {})
                if fields and isinstance(fields, dict):
                    print(f"    ✅ Loaded {len(fields)} field mappings")
                elif fields:
                    print(f"    ⚠️  Fields loaded but wrong type: {type(fields)}")
                else:
                    print(f"    ❌ No field mappings found")
                
                self.mappings[mapping.ghidra_class] = mapping
                self.class_name_map[mapping.ghidra_class] = mapping.source_class
                print(f"  Loaded: {mapping_file.name} ({mapping.confidence})")
                
            except Exception as e:
                print(f"  Error loading {mapping_file}: {e}")
        
        print(f"Total mappings loaded: {len(self.mappings)}")
    
    def apply_global_class_renames(self, content: str) -> str:
        """Phase 1: Replace all Ghidra class names with source names"""
        for ghidra_class, source_class in self.class_name_map.items():
            # Replace class names but be careful with boundaries
            content = re.sub(rf'\b{re.escape(ghidra_class)}\b', source_class, content)
        return content
    
    def apply_file_scoped_renames(self, content: str, mapping: ClassMapping) -> str:
        """Phase 2: Context-aware field and method renames within this class scope ONLY"""
        
        # Context-aware field renaming - only rename fields belonging to THIS class
        if mapping.fields:
            print(f"    🔄 Applying {len(mapping.fields)} field renames...")
            content = self.apply_context_aware_field_renames(content, mapping.fields)
        else:
            print(f"    ❌ No field mappings to apply")
        
        # Rename methods (class-scoped)
        if mapping.methods:
            for ghidra_method, source_method in mapping.methods.items():
                content = re.sub(rf'\b{re.escape(ghidra_method)}\b', source_method, content)
        
        return content
    
    def apply_context_aware_field_renames(self, content: str, fields: Dict[str, str]) -> str:
        """Apply field renames only in the correct class context to avoid renaming other class fields"""
        
        for ghidra_field, source_field in fields.items():
            
            # 1. SAFE: this-> access (always belongs to current class)
            content = re.sub(
                rf'\bthis->{re.escape(ghidra_field)}\b',
                f'this->{source_field}',
                content
            )
            
            # 2. SAFE: Direct member access within class methods (no -> or . prefix)
            # Only rename bare field names that aren't preceded by -> or .
            content = re.sub(
                rf'(?<![a-zA-Z0-9_\-\.>])\b{re.escape(ghidra_field)}\b(?=\s*[=\[\+\-\*\/\%\!\&\|\^\<\>\;\,\)\s])',
                source_field,
                content
            )
            
            # 3. SKIP: obj->field and var.field access to avoid renaming other classes
            # These require type analysis to determine if obj/var is the same class type
            
        return content
    
    def process_file(self, ghidra_class: str, mapping: ClassMapping) -> bool:
        """Process both .h and .cpp files for a class"""
        success = True
        
        # Process .h file (struct definition)
        success &= self.process_header_file(ghidra_class, mapping)
        
        # Process .cpp file (method implementations)  
        success &= self.process_source_file(ghidra_class, mapping)
        
        return success
    
    def process_header_file(self, ghidra_class: str, mapping: ClassMapping) -> bool:
        """Process the .h file containing struct definition"""
        input_file = self.input_dir / f"{ghidra_class}.h"
        if not input_file.exists():
            print(f"  Warning: {input_file} not found")
            return False
        
        try:
            with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Apply renaming to header
            content = self.apply_global_class_renames(content)
            content = self.apply_file_scoped_renames(content, mapping)
            
            # Write to output with source class name
            output_file = self.output_dir / f"{mapping.source_class}.h"
            self.output_dir.mkdir(parents=True, exist_ok=True)
            
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(content)
            
            print(f"  Generated: {output_file.name}")
            return True
            
        except Exception as e:
            print(f"  Error processing {ghidra_class}.h: {e}")
            return False
    
    def process_source_file(self, ghidra_class: str, mapping: ClassMapping) -> bool:
        """Process the .cpp file containing method implementations"""
        input_file = self.input_dir / f"{ghidra_class}.cpp"
        if not input_file.exists():
            # Try with original naming pattern from Classes/
            pattern_files = list(self.input_dir.glob(f"*{ghidra_class}*.cpp"))
            if pattern_files:
                input_file = pattern_files[0]
        if not input_file.exists():
            print(f"  Warning: {input_file} not found")
            return False
        
        try:
            with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Replace classes_all.h with specific header
            content = re.sub(
                r'#include\s+"classes_all\.h"',
                f'#include "{mapping.source_class}.h"',
                content
            )
            
            # Apply 3-phase renaming
            content = self.apply_global_class_renames(content)
            content = self.apply_file_scoped_renames(content, mapping)
            # TODO: Phase 3 - parameter-scoped renaming
            
            # Write to output with source class name
            output_file = self.output_dir / f"{mapping.source_class}.cpp"
            self.output_dir.mkdir(parents=True, exist_ok=True)
            
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(content)
            
            print(f"  Generated: {output_file.name}")
            return True
            
        except Exception as e:
            print(f"  Error processing {ghidra_class}.cpp: {e}")
            return False
    
    def copy_essential_headers(self):
        """Copy essential header files like classes_all.h"""
        essential_files = ["classes_all.h", "classes_forward.h", "common_types.h"]
        
        for filename in essential_files:
            src_file = self.input_dir / filename
            if src_file.exists():
                dst_file = self.output_dir / filename
                try:
                    with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    with open(dst_file, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"  Copied: {filename}")
                except Exception as e:
                    print(f"  Error copying {filename}: {e}")
    
    def update_classes_all_header(self):
        """Create organized classes_all.h with converted and unconverted sections"""
        classes_all_file = self.output_dir / "classes_all.h"
        
        try:
            # Get list of all Ghidra classes from original
            original_classes_all = self.input_dir / "classes_all.h"
            all_ghidra_classes = []
            
            if original_classes_all.exists():
                with open(original_classes_all, 'r', encoding='utf-8', errors='ignore') as f:
                    for line in f:
                        if line.strip().startswith('#include "cls_0x') and line.strip().endswith('.h"'):
                            # Extract class name from #include "cls_0x....h"
                            class_name = line.strip().split('"')[1][:-2]  # Remove .h
                            all_ghidra_classes.append(class_name)
            
            # Get all mapped Ghidra class base names (without suffixes)
            mapped_ghidra_bases = set(mapping.ghidra_class for mapping in self.mappings.values())
            
            # Separate converted vs unconverted classes (remove duplicates)
            converted_classes = sorted(set(mapping.source_class for mapping in self.mappings.values()))
            unconverted_classes = []
            
            for cls in all_ghidra_classes:
                # Extract base name (before any suffix like _TCharacter)
                if '_' in cls:
                    # Split by underscore and take first 3 parts: cls_0x12345
                    parts = cls.split('_')
                    if len(parts) >= 3:
                        base_name = '_'.join(parts[:3])  # cls_0x12345
                    else:
                        base_name = cls
                else:
                    base_name = cls
                
                # Only include if the base name is not mapped
                if base_name not in mapped_ghidra_bases:
                    unconverted_classes.append(cls)
            
            unconverted_classes = sorted(unconverted_classes)
            
            # Generate organized classes_all.h
            content = """#pragma once
#include "common_types.h"

// ============================================
// CONVERTED CLASSES (Readable Source Names)
// ============================================
"""
            
            # Add converted classes (local includes) - already sorted and unique
            for source_class in converted_classes:
                content += f'#include "{source_class}.h"\n'
            
            content += """
// ============================================  
// UNCONVERTED GHIDRA CLASSES (Original Names)
// ============================================
"""
            
            # Add unconverted classes (relative paths to Classes_Converted) - already sorted
            for ghidra_class in unconverted_classes:
                content += f'#include "../Classes_Converted/{ghidra_class}.h"\n'
            
            # Write the organized header
            with open(classes_all_file, 'w', encoding='utf-8') as f:
                f.write(content)
            
            print(f"  Updated: classes_all.h")
            print(f"    - {len(converted_classes)} converted classes")
            print(f"    - {len(unconverted_classes)} unconverted classes")
            
        except Exception as e:
            print(f"  Error updating classes_all.h: {e}")
    
    def run_renaming(self):
        """Main renaming process"""
        print("=== Phase 2: Semantic Renaming ===")
        
        self.load_mappings()
        if not self.mappings:
            print("No mappings found. Create mapping files first.")
            return
        
        # Create output directory and copy essential files
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.copy_essential_headers()
        
        print(f"\nProcessing {len(self.mappings)} mapped classes...")
        success_count = 0
        
        for ghidra_class, mapping in self.mappings.items():
            print(f"\nProcessing {ghidra_class} -> {mapping.source_class}...")
            if self.process_file(ghidra_class, mapping):
                success_count += 1
        
        # Update classes_all.h with renamed includes
        self.update_classes_all_header()
        
        print(f"\n=== Complete: {success_count}/{len(self.mappings)} files processed ===")

if __name__ == "__main__":
    renamer = ClassRenamer()
    renamer.run_renaming()
