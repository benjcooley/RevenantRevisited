#!/usr/bin/env python3
"""
Field Mapper - Intelligent field mapping from source to Ghidra
Extracts actual field names from source files and maps them to Ghidra fields by offset analysis
"""

import json
import re
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import yaml

class FieldMapper:
    """Map source field names to Ghidra field offsets"""
    
    def __init__(self):
        # Load Ghidra fingerprints
        with open('class_fingerprints.json', 'r') as f:
            self.ghidra_data = json.load(f)['ghidra_classes']
    
    def extract_source_fields(self, source_file: str, class_name: str) -> List[Tuple[str, str, int]]:
        """Extract field names, types, and estimated offsets from source class"""
        
        src_path = Path(f"src/{source_file}")
        if not src_path.exists():
            return []
            
        with open(src_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Find the specific class definition
        class_pattern = rf'(?:_CLASSDEF\({class_name}\)[\s\S]*?)?class\s+{class_name}(?:\s*:\s*[^{{]*)?{{([^{{}}]*(?:{{[^{{}}]*}}[^{{}}]*)*)}}'
        match = re.search(class_pattern, content, re.DOTALL | re.IGNORECASE)
        
        if not match:
            print(f"⚠️  Could not find class {class_name} in {source_file}")
            return []
            
        class_body = match.group(1)
        return self.parse_class_fields(class_body)
    
    def parse_class_fields(self, class_body: str) -> List[Tuple[str, str, int]]:
        """Parse field declarations from class body, return (name, type, estimated_offset)"""
        fields = []
        current_offset = 0
        
        # Split by semicolons and analyze each declaration
        declarations = class_body.split(';')
        
        for decl in declarations:
            decl = decl.strip()
            if not decl or self.is_non_field(decl):
                continue
                
            field_info = self.parse_field_declaration(decl)
            if field_info:
                field_name, field_type = field_info
                field_size = self.estimate_field_size(field_type, field_name)
                
                fields.append((field_name, field_type, current_offset))
                current_offset += field_size
                
        return fields
    
    def is_non_field(self, decl: str) -> bool:
        """Check if declaration is not a field (method, access specifier, etc.)"""
        skip_patterns = [
            r'public\s*:', r'private\s*:', r'protected\s*:',
            r'\([^)]*\)',  # Method calls
            r'virtual\s+', r'static\s+', r'friend\s+',
            r'typedef\s+', r'enum\s+',
            r'^\s*$'  # Empty lines
        ]
        
        for pattern in skip_patterns:
            if re.search(pattern, decl, re.IGNORECASE):
                return True
        return False
    
    def parse_field_declaration(self, decl: str) -> Optional[Tuple[str, str]]:
        """Parse a field declaration into (name, type)"""
        # Handle various field patterns
        patterns = [
            r'(\w+(?:\s*\*)*)\s+(\w+)(?:\[[\d\s]*\])*$',  # type name[array]
            r'((?:unsigned|signed)?\s*(?:char|short|int|long|float|double)(?:\s*\*)*)\s+(\w+)$',
            r'(\w+::\w+|\w+)\s+(\w+)$',  # namespaced types
            r'(\w+(?:\s*\*)*)\s+(\w+)\s*(?://.*)?$'  # with comments
        ]
        
        for pattern in patterns:
            match = re.match(pattern, decl.strip())
            if match:
                type_name = match.group(1).strip()
                field_name = match.group(2).strip()
                
                # Skip constructor/destructor patterns
                if field_name.startswith('~') or '(' in field_name:
                    continue
                    
                return (field_name, type_name)
                
        return None
    
    def estimate_field_size(self, field_type: str, field_name: str) -> int:
        """Estimate field size in bytes"""
        # Handle arrays
        if '[' in field_name:
            array_match = re.search(r'\[(\d+)\]', field_name)
            array_size = int(array_match.group(1)) if array_match else 10
            base_size = self.get_base_type_size(field_type)
            return base_size * array_size
            
        return self.get_base_type_size(field_type)
    
    def get_base_type_size(self, type_name: str) -> int:
        """Get base type size in bytes"""
        type_map = {
            'char': 1, 'byte': 1, 'bool': 1, 'uint8_t': 1, 'int8_t': 1,
            'short': 2, 'word': 2, 'uint16_t': 2, 'int16_t': 2,
            'int': 4, 'long': 4, 'float': 4, 'uint32_t': 4, 'int32_t': 4, 'dword': 4,
            'double': 8, 'uint64_t': 8, 'int64_t': 8, 'qword': 8,
        }
        
        # Normalize type name
        clean_type = type_name.lower().replace('unsigned', '').replace('signed', '').strip()
        clean_type = re.sub(r'\s*\*+\s*', '', clean_type)  # Remove pointers -> 4 bytes
        
        if '*' in type_name or clean_type.startswith('p'):
            return 4  # Pointer
        
        return type_map.get(clean_type, 4)  # Default to 4 bytes
    
    def map_fields_to_ghidra(self, source_fields: List[Tuple[str, str, int]], 
                            ghidra_class: str) -> Dict[str, str]:
        """Map source fields to Ghidra fields by offset analysis"""
        
        if ghidra_class not in self.ghidra_data:
            return {}
            
        ghidra_info = self.ghidra_data[ghidra_class]
        ghidra_fields = ghidra_info.get('fields', [])
        
        if not ghidra_fields:
            return {}
        
        mappings = {}
        
        for src_name, src_type, src_offset in source_fields:
            # Find best matching Ghidra field by offset proximity
            best_match = None
            best_score = float('inf')
            
            for ghidra_field in ghidra_fields:
                if len(ghidra_field) < 4:
                    continue
                    
                ghidra_name, ghidra_offset, ghidra_size, ghidra_type = ghidra_field[:4]
                
                # Calculate match score (offset proximity + size compatibility)
                offset_diff = abs(src_offset - ghidra_offset)
                size_diff = abs(self.estimate_field_size(src_type, src_name) - ghidra_size)
                
                score = offset_diff + (size_diff * 2)  # Weight size differences more
                
                if score < best_score and score < 20:  # Reasonable threshold
                    best_score = score
                    best_match = ghidra_name
            
            if best_match:
                mappings[best_match] = src_name
                
        return mappings
    
    def generate_field_mapping(self, source_class: str, ghidra_class: str, source_file: str) -> Dict[str, str]:
        """Generate complete field mapping for a class"""
        
        # Extract source fields
        source_fields = self.extract_source_fields(source_file, source_class)
        
        if not source_fields:
            return {}
        
        print(f"📊 {source_class} ({source_file}):")
        print(f"   Source fields: {len(source_fields)}")
        for name, type_name, offset in source_fields[:5]:  # Show first 5
            print(f"     {name}: {type_name} @ 0x{offset:x}")
        if len(source_fields) > 5:
            print(f"     ... and {len(source_fields)-5} more")
        
        # Map to Ghidra fields
        mappings = self.map_fields_to_ghidra(source_fields, ghidra_class)
        
        print(f"   Generated {len(mappings)} field mappings")
        
        return mappings
    
    def update_mapping_file(self, mapping_file: str):
        """Update a mapping file with actual field mappings"""
        
        mapping_path = Path(f"mappings/{mapping_file}")
        if not mapping_path.exists():
            print(f"⚠️  Mapping file not found: {mapping_file}")
            return
        
        # Load existing mapping
        with open(mapping_path, 'r') as f:
            content = f.read()
        
        # Extract class info from filename or content
        match = re.search(r'(\w+)_cls_0x([a-f0-9]+)_map\.yaml', mapping_file)
        if not match:
            print(f"⚠️  Could not parse mapping filename: {mapping_file}")
            return
            
        source_class = match.group(1)
        ghidra_class = f"cls_0x{match.group(2)}"
        
        # Extract source file from YAML content
        source_file_match = re.search(r'Source file:\s*(\S+)', content)
        if not source_file_match:
            print(f"⚠️  Could not find source file in {mapping_file}")
            return
            
        source_file = source_file_match.group(1)
        
        # Generate field mappings
        field_mappings = self.generate_field_mapping(source_class, ghidra_class, source_file)
        
        if not field_mappings:
            print(f"⚠️  No field mappings generated for {source_class}")
            return
        
        # Generate YAML field section
        yaml_fields = "fields:\n"
        for ghidra_field, source_field in field_mappings.items():
            yaml_fields += f'  {ghidra_field}: "{source_field}"  # Auto-mapped by offset analysis\n'
        
        # Replace the placeholder fields section
        updated_content = re.sub(
            r'fields:\s*\n\s*#[^\n]*\n',
            yaml_fields,
            content
        )
        
        # Write updated mapping
        with open(mapping_path, 'w') as f:
            f.write(updated_content)
        
        print(f"✅ Updated {mapping_file} with {len(field_mappings)} field mappings")

def main():
    """Update all mapping files with intelligent field mappings"""
    mapper = FieldMapper()
    
    print("🔧 INTELLIGENT FIELD MAPPING")
    print("=" * 50)
    
    mapping_dir = Path("mappings")
    mapping_files = list(mapping_dir.glob("*.yaml"))
    
    # Skip README
    mapping_files = [f for f in mapping_files if f.name != "README.md"]
    
    print(f"Found {len(mapping_files)} mapping files to update")
    print()
    
    for i, mapping_file in enumerate(mapping_files[:10]):  # Process first 10
        print(f"[{i+1}/{len(mapping_files)}] Processing {mapping_file.name}")
        mapper.update_mapping_file(mapping_file.name)
        print()
    
    print("✅ Field mapping complete!")
    print("🎯 Next: Run conversion pipeline to generate readable code")

if __name__ == "__main__":
    main()
