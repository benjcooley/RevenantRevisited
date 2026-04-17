#!/usr/bin/env python3
"""
Convert Ghidra Class Definitions to C++ Structs
Converts commented OOAnalyzer class definitions to actual C++ structs for VS Code refactoring
"""

import os
import re
import json
from pathlib import Path
from typing import Dict, List, Tuple, Set
from dataclasses import dataclass
import shutil

@dataclass
class FieldInfo:
    """Represents a field in a class definition"""
    offset: int
    type_name: str
    size: int
    field_name: str
    comment: str

@dataclass
class ClassInfo:
    """Represents a class definition"""
    name: str
    fields: List[FieldInfo]
    total_size: int
    alignment: int
    referenced_classes: Set[str]

class StructConverter:
    def __init__(self, classes_dir: str = "RevenantGhidra", output_dir: str = "Classes_Converted"):
        self.classes_dir = Path(classes_dir)
        self.output_dir = Path(output_dir)
        self.all_classes: Dict[str, ClassInfo] = {}
        self.type_mapping = {
            'byte': 'uint8_t',
            'char': 'char',
            'uchar': 'unsigned char',
            'short': 'int16_t',
            'ushort': 'uint16_t',
            'int': 'int32_t',
            'uint': 'uint32_t',
            'long': 'int32_t',
            'ulong': 'uint32_t',
            'float': 'float',
            'double': 'double',
            'bool': 'bool',
            'void': 'void',
            'pointer': 'void*',
            'undefined1': 'undefined1',
            'undefined2': 'undefined2',
            'undefined4': 'undefined4',
            'undefined8': 'undefined8',
            'undefined': 'undefined',
        }
        
        # Create output directory
        self.output_dir.mkdir(exist_ok=True)
        
    def parse_ghidra_struct(self, content: str) -> ClassInfo:
        """Parse a Ghidra OOAnalyzer struct definition"""
        # Extract class name
        name_match = re.search(r'Structure\s+(\w+)\s*{', content)
        if not name_match:
            return None
        
        class_name = name_match.group(1)
        fields = []
        referenced_classes = set()
        
        # Parse field definitions
        # Pattern: offset type size field_name "comment"
        field_pattern = r'(\d+)\s+(\w+)\s+(\d+)\s+(\w+)\s+"([^"]*)"'
        matches = re.findall(field_pattern, content)
        
        for match in matches:
            offset = int(match[0])
            type_name = match[1]
            size = int(match[2])
            field_name = match[3]
            comment = match[4]
            
            # Track referenced classes
            if type_name.startswith('cls_'):
                referenced_classes.add(type_name)
            
            fields.append(FieldInfo(
                offset=offset,
                type_name=type_name,
                size=size,
                field_name=field_name,
                comment=comment
            ))
        
        # Extract total size and alignment
        size_match = re.search(r'Length:\s*(\d+)', content)
        total_size = int(size_match.group(1)) if size_match else 0
        
        align_match = re.search(r'Alignment:\s*(\d+)', content)
        alignment = int(align_match.group(1)) if align_match else 1
        
        return ClassInfo(
            name=class_name,
            fields=fields,
            total_size=total_size,
            alignment=alignment,
            referenced_classes=referenced_classes
        )
    
    def convert_type_to_cpp(self, type_name: str) -> str:
        """Convert Ghidra type names to C++ types using common definitions"""
        if type_name.startswith('cls_'):
            return f'struct {type_name}'
        
        # Use the original Ghidra type names since we have typedefs for them
        if type_name in self.type_mapping:
            return type_name  # Use original name (byte, word, dword, etc.)
        return type_name
    
    def get_type_alignment(self, cpp_type: str) -> int:
        """Get the natural alignment of a C++ type"""
        if cpp_type.startswith('struct '):
            return 4  # Assume struct alignment of 4 bytes
        elif cpp_type in ['uint64_t', 'int64_t', 'double']:
            return 8
        elif cpp_type in ['uint32_t', 'int32_t', 'float']:
            return 4
        elif cpp_type in ['uint16_t', 'int16_t']:
            return 2
        elif cpp_type in ['uint8_t', 'int8_t', 'char']:
            return 1
        else:
            return 4  # Default alignment
    
    def calculate_natural_offset(self, current_offset: int, cpp_type: str) -> int:
        """Calculate where a field would naturally be placed with C++ alignment rules"""
        alignment = self.get_type_alignment(cpp_type)
        # Round up to the next alignment boundary
        return (current_offset + alignment - 1) & ~(alignment - 1)
    
    def generate_cpp_struct(self, class_info: ClassInfo) -> str:
        """Generate C++ struct code from ClassInfo with smart padding"""
        lines = []
        
        # Add includes
        lines.append('#include "common_types.h"')
        lines.append('')
        
        # Add forward declarations for referenced classes
        if class_info.referenced_classes:
            lines.append('// Forward declarations')
            for ref_class in sorted(class_info.referenced_classes):
                lines.append(f'struct {ref_class};')
            lines.append('')
        
        # Add the converted C++ struct with smart padding
        lines.append(f'// Converted C++ struct for VS Code refactoring')
        lines.append(f'// Layout-preserving struct with minimal explicit padding')
        lines.append(f'// Note: OOAnalyzer namespace removed for cleaner integration')
        lines.append(f'struct {class_info.name} {{')
        
        # Sort fields by offset to ensure proper order
        sorted_fields = sorted(class_info.fields, key=lambda f: f.offset)
        current_offset = 0
        
        for field in sorted_fields:
            cpp_type = self.convert_type_to_cpp(field.type_name)
            natural_offset = self.calculate_natural_offset(current_offset, cpp_type)
            
            # Check if we need explicit padding
            if field.offset > natural_offset:
                # The field is placed beyond natural alignment - need explicit padding
                padding_size = field.offset - current_offset
                if padding_size == 1:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}; // 1 byte explicit padding')
                elif padding_size == 2:
                    lines.append(f'    uint16_t _padding_{current_offset:02x}; // 2 bytes explicit padding')
                elif padding_size == 3:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}[3]; // 3 bytes explicit padding')
                else:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}[{padding_size}]; // {padding_size} bytes explicit padding')
                current_offset = field.offset
            elif field.offset > current_offset:
                # The field is placed at natural alignment but not immediately after previous field
                padding_size = field.offset - current_offset
                if padding_size == 1:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}; // 1 byte padding')
                elif padding_size == 2:
                    lines.append(f'    uint16_t _padding_{current_offset:02x}; // 2 bytes padding')
                elif padding_size == 3:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}[3]; // 3 bytes padding')
                else:
                    lines.append(f'    uint8_t _padding_{current_offset:02x}[{padding_size}]; // {padding_size} bytes padding')
                current_offset = field.offset
            
            # Add the actual field
            comment = f'// offset {field.offset:02x} - {field.size} bytes - {field.comment}'
            lines.append(f'    {cpp_type} {field.field_name};{comment}')
            current_offset = field.offset + field.size
        
        # Add final padding if needed
        if current_offset < class_info.total_size:
            final_padding = class_info.total_size - current_offset
            if final_padding == 1:
                lines.append(f'    uint8_t _padding_{current_offset:02x}; // 1 byte final padding')
            elif final_padding == 2:
                lines.append(f'    uint16_t _padding_{current_offset:02x}; // 2 bytes final padding')
            elif final_padding == 3:
                lines.append(f'    uint8_t _padding_{current_offset:02x}[3]; // 3 bytes final padding')
            else:
                lines.append(f'    uint8_t _padding_{current_offset:02x}[{final_padding}]; // {final_padding} bytes final padding')
        
        lines.append('};')
        lines.append('')
        
        return '\n'.join(lines)
    
    def remove_ooanalyzer_namespace(self, content: str, class_name: str) -> str:
        """Remove OOAnalyzer:: namespace from function definitions"""
        # Pattern to match function definitions with OOAnalyzer:: namespace
        # This matches patterns like: OOAnalyzer::cls_0x45f7c0::meth_0x410160
        pattern = rf'OOAnalyzer::({class_name}::meth_\w+)'
        
        # Replace with just the method name
        content = re.sub(pattern, r'\1', content)
        
        # Also handle function declarations that might have the namespace
        pattern2 = rf'(\w+)\s+__thiscall\s+OOAnalyzer::({class_name}::meth_\w+)'
        content = re.sub(pattern2, r'\1 __thiscall \2', content)
        
        return content
    
    def convert_file(self, file_path: Path) -> bool:
        """Convert a single file from Ghidra format to C++ structs"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return False
        
        # Find the Ghidra struct definition
        struct_match = re.search(r'/\*.*?OOAnalyzer.*?Structure.*?\*/', content, re.DOTALL)
        if not struct_match:
            print(f"No Ghidra struct found in {file_path}")
            return False
        
        # Parse the struct
        class_info = self.parse_ghidra_struct(struct_match.group(0))
        if not class_info:
            print(f"Failed to parse struct in {file_path}")
            return False
        
        # Store class info for later use
        self.all_classes[class_info.name] = class_info
        
        # Generate C++ struct code
        cpp_struct = self.generate_cpp_struct(class_info)
        
        # Find where to insert the C++ struct (after the Ghidra comment)
        struct_end = struct_match.end()
        
        # Split the file content
        before_struct = content[:struct_end]
        after_struct = content[struct_end:]
        
        # Remove OOAnalyzer namespace from the rest of the content
        after_struct = self.remove_ooanalyzer_namespace(after_struct, class_info.name)
        
        # Insert the C++ struct
        new_content = before_struct + '\n' + cpp_struct + after_struct
        
        # Write the converted file to output directory
        output_file = self.output_dir / file_path.name
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(new_content)
            print(f"Converted {file_path.name} -> {output_file}")
            return True
        except Exception as e:
            print(f"Error writing {output_file}: {e}")
            return False
    
    def create_master_header(self, output_file: str = None):
        """Create a master header with all forward declarations"""
        if not self.all_classes:
            return
        
        if output_file is None:
            output_file = self.output_dir / "classes_forward.h"
        
        lines = []
        lines.append('#ifndef CLASSES_FORWARD_H')
        lines.append('#define CLASSES_FORWARD_H')
        lines.append('')
        lines.append('// Forward declarations for all classes')
        lines.append('// Generated automatically from Ghidra analysis')
        lines.append('')
        
        for class_name in sorted(self.all_classes.keys()):
            lines.append(f'struct {class_name};')
        
        lines.append('')
        lines.append('#endif // CLASSES_FORWARD_H')
        
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write('\n'.join(lines))
            print(f"Created master header: {output_file}")
        except Exception as e:
            print(f"Error creating master header: {e}")
    
    def is_virtual_method(self, method_name: str) -> bool:
        """Check if a method is virtual by looking for virt_meth_ prefix."""
        return method_name.startswith('virt_meth_')
    
    def fix_virtual_method_calls(self, content: str) -> str:
        """Fix virtual method call syntax from explicit class::method(this) to this->method()."""
        # Pattern: class_name::virt_meth_xxxxx((class_name *)this, ...)
        # Replace with: this->virt_meth_xxxxx(...)
        # This handles both single-line and multi-line calls
        pattern = r'(\w+)::(virt_meth_\w+)\s*\(\s*\([^)]*\)\s*this\s*,?\s*([^)]*)\)'
        
        def replace_virtual_call(match):
            class_name = match.group(1)
            method_name = match.group(2)
            other_params = match.group(3).strip()
            
            # If there are other parameters, include them
            if other_params:
                return f'this->{method_name}({other_params})'
            else:
                return f'this->{method_name}()'
        
        return re.sub(pattern, replace_virtual_call, content, flags=re.MULTILINE | re.DOTALL)
    
    def extract_method_declarations(self, cpp_content: str, class_name: str) -> List[str]:
        """Extract method declarations for the class from the cpp file."""
        declarations = []
        seen_method_names = set()  # Track seen method names to avoid duplicates
        
        # Pattern 1: Match method definitions with __thiscall (original Ghidra format)
        method_pattern1 = re.compile(
            rf'([\w:<>]+[\s\*&]+)?__thiscall\s+(?:OOAnalyzer::)?{class_name}::(\w+)\s*\(([^)]*)\)',
            re.MULTILINE)
        # Pattern 2: Match method definitions without __thiscall (already converted format)
        method_pattern2 = re.compile(
            rf'([\w:<>]+[\s\*&]+)?{class_name}::(\w+)\s*\(([^)]*)\)',
            re.MULTILINE)
        
        def clean_virtual_params(params, class_name):
            # Remove all whitespace for easier matching
            params_stripped = params.strip()
            # Pattern for: ClassName *this, ... (possibly with spaces)
            pattern = rf'^\s*{re.escape(class_name)}\s*\*\s*this\s*,\s*'
            params_cleaned = re.sub(pattern, '', params_stripped)
            # Pattern for: only ClassName *this
            pattern_only = rf'^\s*{re.escape(class_name)}\s*\*\s*this\s*$'
            if re.match(pattern_only, params_cleaned):
                return ''
            return params_cleaned
        
        # Process both patterns
        for pattern in [method_pattern1, method_pattern2]:
            for match in pattern.finditer(cpp_content):
                ret_type = (match.group(1) or '').strip() or 'void'
                meth_name = match.group(2)
                params = match.group(3).strip()
                
                # Remove explicit this parameter and default values for all methods
                params = re.sub(r'=[^,)]*', '', params)
                
                # For virtual methods, robustly remove the first parameter (the *this pointer)
                if self.is_virtual_method(meth_name):
                    params = clean_virtual_params(params, class_name)
                else:
                    # Split by comma, normalize whitespace, remove any variant of 'ClassName *this' anywhere in the list
                    param_list = []
                    for p in params.split(','):
                        norm = re.sub(r'\s+', ' ', p.strip())
                        if not re.match(rf'^\s*{re.escape(class_name)}\s*\*\s*this\s*$', norm, re.IGNORECASE):
                            if norm:
                                param_list.append(norm)
                    params = ', '.join(param_list)
                
                # Check if this is a constructor (same name as class)
                if meth_name == class_name:
                    # Constructor - no return type
                    decl = f"{meth_name}({params});"
                else:
                    # Regular method - check if virtual
                    virtual_keyword = "virtual " if self.is_virtual_method(meth_name) else ""
                    decl = f"{virtual_keyword}{ret_type} {meth_name}({params});"
                
                # Deduplicate declarations based on method name and parameters
                method_signature = f"{meth_name}({params})"
                if method_signature not in seen_method_names:
                    declarations.append(decl)
                    seen_method_names.add(method_signature)
        
        return declarations

    def write_class_header(self, class_info: ClassInfo, method_decls: List[str]):
        """Write the .h file for a class."""
        h_path = self.output_dir / f"{class_info.name}.h"
        lines = []
        lines.append('#pragma once')
        lines.append('#include "classes_all.h"')
        lines.append('')
        lines.append(f'struct {class_info.name} {{')
        # Fields
        sorted_fields = sorted(class_info.fields, key=lambda f: f.offset)
        current_offset = 0
        for field in sorted_fields:
            cpp_type = self.convert_type_to_cpp(field.type_name)
            comment = f'// offset {field.offset:02x} - {field.size} bytes - {field.comment}'
            lines.append(f'    {cpp_type} {field.field_name};{comment}')
            current_offset = field.offset + field.size
        # Method declarations
        if method_decls:
            lines.append('')
            for decl in method_decls:
                lines.append(f'    {decl}')
        lines.append('};')
        lines.append('')
        
        with open(h_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(lines))

    def update_cpp_file(self, file_path: Path, class_name: str):
        """Remove struct and Ghidra comment, add #include \"classes_all.h\" at the top."""
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        # Remove Ghidra struct comment and struct definition
        content = re.sub(r'/\*.*?OOAnalyzer.*?Structure.*?\*/', '', content, flags=re.DOTALL)
        content = re.sub(r'struct\s+' + re.escape(class_name) + r'\s*\{[^}]*\};', '', content, flags=re.DOTALL)
        # Remove all OOAnalyzer:: namespace references
        content = re.sub(r'OOAnalyzer::', '', content)
        
        # Handle constructors specially - remove return type for constructors
        # Pattern: return_type __thiscall class_name::class_name(...)
        content = re.sub(
            rf'([\w:<>]+[\s\*&]+)?__thiscall\s+{class_name}::{class_name}\s*\(',
            rf'{class_name}::',
            content
        )
        
        # Fix already-converted constructors that still have *this parameter
        # Pattern: class_name::class_name *this)
        content = re.sub(
            rf'{re.escape(class_name)}::{re.escape(class_name)}\s*\*\s*this\s*\)',
            rf'{class_name}::{class_name}()',
            content
        )
        
        # Fix virtual method calls: class_name::virt_meth_xxx((class_name *)this) -> this->virt_meth_xxx()
        content = self.fix_virtual_method_calls(content)
        
        # Clean up method definitions - remove explicit this parameter and default values
        # Pattern: return_type __thiscall class_name::method(class_name *this, ...)
        method_pattern = re.compile(
            rf'([\w:<>]+[\s\*&]+)?__thiscall\s+{class_name}::(\w+)\s*\(([^)]*)\)',
            re.MULTILINE
        )
        
        def clean_method_definition(match):
            ret_type = (match.group(1) or '').strip() or 'void'
            meth_name = match.group(2)
            params = match.group(3).strip()
            
            # Remove explicit this parameter and default values
            params = re.sub(r'=[^,)]*', '', params)
            # Split by comma, normalize whitespace, remove any variant of 'ClassName *this'
            param_list = []
            for p in params.split(','):
                norm = re.sub(r'\s+', ' ', p.strip())
                # More robust pattern to match any variant of ClassName *this
                if not re.match(rf'^\s*{re.escape(class_name)}\s*\*\s*this\s*$', norm, re.IGNORECASE):
                    if norm:
                        param_list.append(norm)
            params = ', '.join(param_list)
            
            # For constructors, no return type
            if meth_name == class_name:
                return f'{class_name}::{meth_name}({params})'
            else:
                return f'{ret_type} {class_name}::{meth_name}({params})'
        
        content = method_pattern.sub(clean_method_definition, content)
        
        # Add include at the top
        content = f'#include "classes_all.h"\n\n{content}'
        
        # Write to the output directory instead of overwriting the original file
        output_cpp_path = self.output_dir / file_path.name
        with open(output_cpp_path, 'w', encoding='utf-8') as f:
            f.write(content)

    def create_classes_all_header(self):
        """Create the master include file classes_all.h."""
        all_headers = sorted([f for f in self.output_dir.glob('cls_*.h')])
        lines = ['#pragma once', '#include "common_types.h"', '']
        for h in all_headers:
            lines.append(f'#include "{h.name}"')
        lines.append('')
        with open(self.output_dir / 'classes_all.h', 'w', encoding='utf-8') as f:
            f.write('\n'.join(lines))

    def convert_all_files(self):
        print("Converting Ghidra class definitions to C++ headers and sources...")
        class_files = list(self.classes_dir.glob("cls_*.cpp"))
        print(f"Found {len(class_files)} class files")
        # First pass: parse all classes
        print("First pass: parsing all classes...")
        for file_path in class_files:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            struct_match = re.search(r'/\*.*?OOAnalyzer.*?Structure.*?\*/', content, re.DOTALL)
            if not struct_match:
                continue
            class_info = self.parse_ghidra_struct(struct_match.group(0))
            if class_info:
                self.all_classes[class_info.name] = class_info
        # Second pass: generate headers and update cpp files
        print("Second pass: generating headers and updating sources...")
        for file_path in class_files:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            struct_match = re.search(r'/\*.*?OOAnalyzer.*?Structure.*?\*/', content, re.DOTALL)
            if not struct_match:
                continue
            class_info = self.parse_ghidra_struct(struct_match.group(0))
            if not class_info:
                continue
            
            method_decls = self.extract_method_declarations(content, class_info.name)
            self.write_class_header(class_info, method_decls)
            self.update_cpp_file(file_path, class_info.name)
        self.create_classes_all_header()
        print("Header/source split and master include generation complete.")
    
    def generate_summary_report(self, output_file: str = None):
        """Generate a summary report of the conversion"""
        if output_file is None:
            output_file = self.output_dir / "conversion_report.md"
        
        # Ensure refactoring directory exists for the report
        os.makedirs("refactoring", exist_ok=True)
        
        lines = []
        lines.append('# Ghidra to C++ Struct Conversion Report')
        lines.append('')
        lines.append(f'## Summary')
        lines.append(f'- **Total classes converted**: {len(self.all_classes)}')
        lines.append(f'- **Total files processed**: {len(list(self.classes_dir.glob("cls_*.cpp")))}')
        lines.append('')
        
        lines.append('## Class Details')
        for class_name, class_info in sorted(self.all_classes.items()):
            lines.append(f'### {class_name}')
            lines.append(f'- **Total size**: {class_info.total_size} bytes')
            lines.append(f'- **Alignment**: {class_info.alignment}')
            lines.append(f'- **Field count**: {len(class_info.fields)}')
            lines.append(f'- **Referenced classes**: {len(class_info.referenced_classes)}')
            if class_info.referenced_classes:
                lines.append(f'  - {", ".join(sorted(class_info.referenced_classes))}')
            lines.append('')
        
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write('\n'.join(lines))
            print(f"Generated conversion report: {output_file}")
        except Exception as e:
            print(f"Error generating report: {e}")
        
        # Also create a copy in the refactoring directory for easy access
        refactoring_report = Path("refactoring") / "conversion_report.md"
        try:
            with open(refactoring_report, 'w', encoding='utf-8') as f:
                f.write('\n'.join(lines))
            print(f"Also saved report to: {refactoring_report}")
        except Exception as e:
            print(f"Error saving report to refactoring directory: {e}")

def main():
    """Main function"""
    converter = StructConverter()
    converter.convert_all_files()

if __name__ == "__main__":
    main() 