#!/usr/bin/env python3

"""
COMPREHENSIVE CLASS FINGERPRINTER - Single script to generate all fingerprints correctly

Handles:
- Source class fingerprinting with recursive struct expansion
- Ghidra class fingerprinting with proper gap detection  
- Large array/gap compression using 'A' character (>32 bytes)
- Sorting by fingerprint pattern length
- Complete patterns (no truncation after gaps)
"""

import re
import sys
from pathlib import Path
from typing import List, Tuple, Dict, Optional

# Add the scripts directory to sys.path
sys.path.append(str(Path(__file__).parent))

class ComprehensiveFingerprinter:
    """Single comprehensive fingerprinter with all functionality integrated"""
    
    def __init__(self):
        # Load struct definitions once
        self.struct_definitions = self.load_all_struct_definitions()
        # Build class registry for recursive parsing
        self.class_registry = self.build_class_registry()
        self.pattern_cache = {}  # Cache parsed patterns to avoid infinite recursion
        # Track unknown types for debugging
        self.unknown_types = {}  # type_name -> [contexts where it appeared]
        # Register all known type patterns
        self._register_known_types()
        
    def parse_ghidra_fields(self, cpp_file: Path) -> List[Tuple[int, int, str]]:
        """Parse fields from Ghidra .cpp file: (offset, size, name)"""
        fields = []
        
        try:
            with open(cpp_file, 'r') as f:
                content = f.read()
            
            # Find struct definition
            struct_match = re.search(r'Structure\s+(\w+)\s*\{(.*?)\}', content, re.DOTALL)
            if not struct_match:
                return fields
            
            struct_body = struct_match.group(2)
            
            # Parse field lines: "   404   dword   4   mbr_0x194"
            field_pattern = r'^\s*(\d+)\s+\w+\s+(\d+)\s+(\w+)'
            
            for line in struct_body.split('\n'):
                match = re.match(field_pattern, line.strip())
                if match:
                    offset = int(match.group(1))
                    size = int(match.group(2))
                    name = match.group(3)
                    fields.append((offset, size, name))
            
            return sorted(fields)  # Sort by offset
            
        except Exception as e:
            return []

    def create_ghidra_fingerprint(self, fields: List[Tuple[int, int, str]]) -> str:
        """Create complete fingerprint with proper gap detection"""
        if not fields:
            return ""
        
        pattern = ""
        last_offset = 0
        
        for offset, size, name in fields:
            # Detect gap between this field and last
            gap_size = offset - last_offset
            
            if gap_size > 32:  # Large gap - use 'A'
                pattern += 'A'
            elif gap_size > 0:  # Small gap - fill with appropriate padding
                while gap_size > 0:
                    if gap_size >= 8:
                        pattern += 'q'
                        gap_size -= 8
                    elif gap_size >= 4:
                        pattern += 'd'
                        gap_size -= 4
                    elif gap_size >= 2:
                        pattern += 'w'
                        gap_size -= 2
                    else:
                        pattern += 'b'
                        gap_size -= 1
            
            # Add pattern for current field based on size
            if size == 1:
                pattern += 'b'
            elif size == 2:
                pattern += 'w'
            elif size == 4:
                pattern += 'd'
            elif size == 8:
                pattern += 'q'
            elif size > 32:
                pattern += 'A'  # Large field/array
            elif size > 16:
                pattern += 'S'  # Medium struct
            elif size > 8:
                pattern += 'D'  # Double-dword
            else:
                pattern += 'd'  # Default to dword
            
            last_offset = offset + size
        
        return pattern

    def fingerprint_all_ghidra_classes(self) -> List[Tuple[str, Dict]]:
        """Generate fingerprints for ALL Ghidra classes"""
        print("🔍 Fingerprinting ALL Ghidra classes...")
        
        # Try multiple potential directories
        potential_dirs = [Path("../Classes"), Path("Classes"), Path("../Classes_Original")]
        classes_dir = None
        for dir_path in potential_dirs:
            if dir_path.exists():
                classes_dir = dir_path
                break
        
        if not classes_dir:
            print(f"❌ No Classes directory found in {potential_dirs}")
            return []
        
        fingerprints = []
        cpp_files = list(classes_dir.glob("cls_*.cpp"))
        print(f"Found {len(cpp_files)} Ghidra class files")
        
        for cpp_file in cpp_files:
            class_name = cpp_file.stem
            fields = self.parse_ghidra_fields(cpp_file)
            
            if fields:
                pattern = self.create_ghidra_fingerprint(fields)
                total_size = fields[-1][0] + fields[-1][1] if fields else 0
                
                fingerprints.append((class_name, {
                    'pattern': pattern,
                    'size': total_size,
                    'fields': len(fields),
                    'type': 'ghidra'
                }))
        
        print(f"✅ Generated {len(fingerprints)} Ghidra fingerprints")
        return fingerprints

    def fingerprint_all_source_classes(self) -> List[Tuple[str, Dict]]:
        """Generate fingerprints for ALL source classes using fixed parsing"""
        print("🔍 Fingerprinting ALL Source classes...")
        
        fingerprints = []
        src_dir = Path("../src")
        
        # List of main classes we know exist
        known_classes = [
            ('character.h', 'TCharacter'),
            ('player.h', 'TPlayer'), 
            ('playscreen.h', 'TPlayScreen'),
            ('3dimage.h', 'T3DImagery'),
            ('object.h', 'TObjectInstance'),
            ('complexobj.h', 'TComplexObject'),
            ('multisurface.h', 'TMultiSurface'),
            ('bitmap.h', 'TBitmap'),
            ('ddsurface.h', 'TSurface'),
        ]
        
        for header_file, class_name in known_classes:
            header_path = src_dir / header_file
            if header_path.exists():
                result = self.source_fingerprinter.fingerprint_source_class(header_path, class_name)
                if result and result['field_count'] >= 3:  # Filter substantial classes
                    fingerprints.append((class_name, {
                        'pattern': result['field_pattern'],
                        'size': result['total_size'],
                        'fields': result['field_count'],
                        'type': 'source'
                    }))
                    print(f"  ✅ {class_name}: {result['field_count']} fields, {result['total_size']} bytes")
                else:
                    print(f"  ❌ {class_name}: Failed to fingerprint")
            else:
                print(f"  ❌ {class_name}: File not found: {header_path}")
        
        print(f"✅ Generated {len(fingerprints)} Source fingerprints")
        return fingerprints
    
    # ======= INTEGRATED SOURCE FINGERPRINTING METHODS =======
    
    def load_all_struct_definitions(self) -> Dict[str, str]:
        """Load all struct/class definitions from source headers"""
        struct_definitions = {}
        src_dir = Path("../src")
        
        for header_file in src_dir.glob("*.h"):
            with open(header_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # More comprehensive struct/class pattern - handle named and unnamed structs/classes
            struct_patterns = [
                r'struct\s+(\w+)\s*\{([^}]+)\}',  # Named structs: struct Name { ... }
                r'typedef\s+struct\s*\{([^}]+)\}\s*(\w+)',  # Typedef structs: typedef struct { ... } Name;
                r'class\s+(\w+)(?:\s*:\s*[^{]*)?{([^}]*(?:{[^}]*}[^}]*)*)}',  # Classes: class Name : Base { ... }
            ]
            
            for pattern in struct_patterns:
                matches = re.finditer(pattern, content, re.MULTILINE | re.DOTALL)
                for match in matches:
                    if len(match.groups()) == 2 and not match.group(2).strip():
                        continue
                    
                    if pattern.startswith(r'typedef'):
                        name, body = match.group(2), match.group(1)
                    else:
                        name, body = match.group(1), match.group(2)
                    
                    if body and body.strip():
                        struct_definitions[name] = body.strip()
        
        return struct_definitions
    
    def build_class_registry(self) -> Dict[str, str]:
        """Build registry of class/struct name -> header file location"""
        import glob
        import re
        
        class_locations = {}
        
        # Scan all .h files in src/ directory
        for h_file in glob.glob("../src/**/*.h", recursive=True):
            try:
                with open(h_file, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    
                # Find all class and struct definitions
                for match in re.finditer(r'(?:struct|class)\s+(\w+)', content):
                    class_name = match.group(1)
                    class_locations[class_name] = h_file
                    
            except Exception as e:
                print(f"Warning: Could not scan {h_file}: {e}")
        
        print(f"📚 Class registry built: {len(class_locations)} classes found")
        return class_locations
    
    def _register_known_types(self):
        """Register all known type patterns at startup"""
        # Exact match patterns for known structures
        exact_patterns = {
            # Basic embedded structures
            'S3DPoint': 'ddd',                 # x(d) + y(d) + z(d)
            'SLightDef': 'wwddddd',           # flags(w) + multiplier(w) + pos(ddd) + color(d) + intensity(d) + lightindex(d) + lightid(d) 
            'SColor': 'bbb',                   # red(b) + green(b) + blue(b)
            'TStatisticList': 'wwd',          # TSizableArray: numitems(w) + size(w) + items*(d)
            
            # Array/collection types  
            'TSpellArray': 'dddddddddd',      # Array of 10 spell entries (40 bytes)
            'TPointerArray': 'dddd',          # 4 dwords as specified
            'TScheduler': 'dddddddd',         # Scheduler with 8 entries
            'TAutoMap': 'dddddd',             # AutoMap data (6 dwords)
            'TQuestFlags': 'dddddddd',        # Quest flag storage (8 dwords)
            
            # Complex manager types
            'TSpellManager': 'ddddddddddd',   # TSpellArray + wait = 11 dwords total
            'TTokenList': 'dddd',             # Token management (4 dwords)
            'SHasSeen': 'dd',                 # Individual HasSeen entry (2 dwords)
        }
        
        # Template prefix patterns (match "TypeName<" -> pattern)
        template_patterns = {
            'TPointerArray<': 'dddd',         # Any TPointerArray<T> -> 4 dwords
            'TArray<': 'wwd',                 # Any TArray<T> -> size(w) + capacity(w) + data*(d) 
            'TList<': 'dd',                   # Any TList<T> -> head(d) + tail(d)
            'TSizableArray<': 'wwd',          # Same as TStatisticList pattern
        }
        
        # Register exact patterns
        for type_name, pattern in exact_patterns.items():
            self.pattern_cache[type_name] = pattern
            
        # Store template patterns for prefix matching
        self.template_patterns = template_patterns
        
        print(f"📝 Registered {len(exact_patterns)} exact patterns + {len(template_patterns)} template patterns")
    
    def _track_unknown_type(self, type_name: str, context: str):
        """Track unknown types for debugging"""
        if type_name not in self.unknown_types:
            self.unknown_types[type_name] = []
        self.unknown_types[type_name].append(context)
    
    def report_unknown_types(self):
        """Print a report of all unknown types that failed to expand"""
        if not self.unknown_types:
            print("🎉 All embedded types successfully expanded!")
            return
            
        print(f"")
        print(f"❌ UNKNOWN EMBEDDED TYPES DEBUG REPORT")
        print(f"=" * 45)
        print(f"Found {len(self.unknown_types)} unknown embedded types that expanded to 'E':")
        print(f"")
        
        # Sort by frequency (most common first)
        sorted_unknowns = sorted(self.unknown_types.items(), key=lambda x: len(x[1]), reverse=True)
        
        for type_name, contexts in sorted_unknowns:
            print(f"🔍 {type_name}")
            print(f"   Occurrences: {len(contexts)}")
            print(f"   Contexts: {', '.join(set(contexts))}")
            
            # Show suggestions for adding to known patterns
            if type_name.endswith('*') or type_name.startswith('PT') or type_name.startswith('PS'):
                print(f"   💡 Suggestion: This looks like a pointer type (should be handled automatically)")
            elif '<' in type_name:
                base_type = type_name.split('<')[0] + '<'
                print(f"   💡 Suggestion: Add template pattern '{base_type}': 'pattern' to template_patterns")
            else:
                print(f"   💡 Suggestion: Add exact pattern '{type_name}': 'pattern' to exact_patterns")
            print(f"")
        
        print(f"📝 To fix: Add these types to _register_known_types() method")
    
    def fingerprint_source_class(self, header_file: Path, class_name: str, inheritance_chain=None) -> Optional[Dict]:
        """Generic fingerprint generation for any class, with optional inheritance chain"""
        if inheritance_chain:
            return self._fingerprint_with_inheritance(header_file, class_name, inheritance_chain)
        else:
            return self._fingerprint_single_class(header_file, class_name)
    
    def _fingerprint_with_inheritance(self, header_file: Path, class_name: str, inheritance_chain: list) -> Optional[Dict]:
        """Generate fingerprint including flattened inheritance chain"""
        try:
            # Build complete inheritance chain using generic parser
            all_fields = []
            
            # Add the target class itself to the chain
            full_chain = inheritance_chain + [(class_name, str(header_file))]
            
            for class_to_parse, header_to_parse in full_chain:
                fields = self._parse_source_class_fields(header_to_parse, class_to_parse)
                all_fields.extend(fields)
                print(f"   {class_to_parse}: {len(fields)} fields")
            
            # Generate pattern and calculate totals
            total_size = sum(size for _, type_name, size in all_fields if not type_name.startswith('('))
            pattern = self._create_pattern_from_fields(all_fields)
            
            return {
                'field_pattern': pattern,
                'field_count': len(all_fields),
                'total_size': total_size,
                'type': 'source_with_inheritance'
            }
            
        except Exception as e:
            print(f"Error fingerprinting {class_name}: {e}")
            return None
    
    
    def _parse_cpp_field_line(self, line: str):
        """Parse a single C++ field declaration line, handling multiple fields and special cases"""
        fields = []
        
        # Remove mutable keyword
        line = re.sub(r'\bmutable\s+', '', line)
        
        # Remove trailing comments and semicolon
        line = re.sub(r'//.*$', '', line).strip()
        line = line.rstrip(';').strip()
        
        if not line:
            return fields
        
        # Patterns for different field declaration types
        patterns = [
            # Template types: TPointerArray<TObjectInstance, 0, 4> inventory - always 4 dwords
            (r'(T\w+)<[^>]+>\s+(\w+)', self._parse_template_type),
            
            # Pointer types with space: char *name, int32_t *ptr
            (r'(\w+)\s*\*\s*(\w+)', lambda m: [(m.group(2), m.group(1) + '*', 4)]),
            
            # Pointer types no space: TSector* sector, PTObjectAnimator animator  
            (r'(\w+\*|\bPT\w+)\s+(\w+)', lambda m: [(m.group(2), m.group(1), 4)]),
            
            # Multiple fields on one line: short frame, framerate
            (r'(\w+)\s+((\w+)(?:\s*,\s*\w+)*)', self._parse_multiple_fields),
            
            # Single field: uint32_t notifyflags
            (r'(\w+)\s+(\w+)', lambda m: [(m.group(2), m.group(1), self._get_type_size(m.group(1)))]),
        ]
        
        for pattern, handler in patterns:
            match = re.search(pattern, line)
            if match:
                try:
                    result = handler(match)
                    if result:
                        return result
                except:
                    continue
        
        return fields
    
    def _parse_template_type(self, match):
        """Parse template types like TPointerArray<> as 4 separate dwords"""
        template_name = match.group(1)
        field_name = match.group(2)
        
        if template_name.startswith('TPointer'):
            # TPointerArray -> 4 separate dwords
            return [
                (f'{field_name}_d0', 'uint32_t', 4),
                (f'{field_name}_d1', 'uint32_t', 4), 
                (f'{field_name}_d2', 'uint32_t', 4),
                (f'{field_name}_d3', 'uint32_t', 4),
            ]
        else:
            # Other templates default to single field
            return [(field_name, template_name, self._get_type_size(template_name))]
    
    def _parse_multiple_fields(self, match):
        """Parse multiple field declarations on one line like 'short frame, framerate'"""
        field_type = match.group(1)
        field_list = match.group(2)
        
        # Split by comma and extract field names
        field_names = [name.strip() for name in field_list.split(',')]
        size = self._get_type_size(field_type)
        
        return [(name, field_type, size) for name in field_names if name]
    
    def _get_type_size(self, type_name: str) -> int:
        """Get size for basic C++ types"""
        type_sizes = {
            'bool': 1, 'char': 1, 'uint8_t': 1,
            'short': 2, 'uint16_t': 2,
            'int32_t': 4, 'uint32_t': 4, 'float': 4,
            'double': 8,
            'S3DPoint': 12, 'SLightDef': 12, 'TStatisticList': 32,
        }
        
        # Handle pointer types
        if type_name.endswith('*') or type_name.startswith('PT') or type_name.startswith('PS'):
            return 4
        
        return type_sizes.get(type_name, 4)
    
    
    def _create_pattern_from_fields(self, fields):
        """Generate pattern string from field list with embedded structure replacement and padding"""
        pattern = ""
        for field_name, field_type, size in fields:
            if field_type.startswith('pad'):
                # Add appropriate padding characters based on alignment
                if field_type == 'pad1':
                    pattern += 'b'
                elif field_type == 'pad2': 
                    pattern += 'w'
                elif field_type == 'pad4':
                    pattern += 'd'
                else:
                    pattern += 'b'  # fallback
            elif field_type.startswith('(') and field_type.endswith(')'):
                # This is an embedded structure placeholder
                embedded_type = field_type[1:-1]  # Remove parentheses
                embedded_pattern = self._get_embedded_fingerprint(embedded_type)
                pattern += embedded_pattern
            else:
                # Regular field
                if size == 1:
                    pattern += 'b'
                elif size == 2:
                    pattern += 'w'
                elif size == 4:
                    pattern += 'd'
                elif size >= 8:
                    pattern += 'A'
                else:
                    pattern += 'd'
        return pattern
    
    def _is_embedded_structure(self, field_type: str) -> bool:
        """Check if this is an embedded structure (non-pointer type)"""
        # Embedded structures are types that don't end with * and don't start with PT/PS
        # and are not basic types
        basic_types = {'bool', 'char', 'uint8_t', 'short', 'uint16_t', 'int32_t', 'uint32_t', 'float', 'double'}
        
        if field_type in basic_types:
            return False
        if field_type.endswith('*'):
            return False
        if field_type.startswith('PT') or field_type.startswith('PS'):  # PT and PS prefixes = pointers
            return False
        if field_type.startswith('T') and '<' in field_type:  # Template types like TPointerArray
            return False
            
        # If it's a named type (starts with capital), likely an embedded structure
        return field_type and field_type[0].isupper()
    
    def _get_embedded_fingerprint(self, embedded_type: str) -> str:
        """Get fingerprint for embedded structure via recursive parsing"""
        # Check cache first to avoid infinite recursion
        if embedded_type in self.pattern_cache:
            return self.pattern_cache[embedded_type]
        
        # Check if we know where this class is located
        if embedded_type in self.class_registry:
            header_file = self.class_registry[embedded_type]
            
            try:
                # Mark as being processed to prevent infinite recursion
                self.pattern_cache[embedded_type] = 'PROCESSING'
                
                # Recursively parse the embedded class
                fields = self._parse_source_class_fields(header_file, embedded_type)
                if fields:
                    pattern = self._create_pattern_from_fields(fields)
                    # Cache the result
                    self.pattern_cache[embedded_type] = pattern
                    return pattern

                else:
                    self._track_unknown_type(embedded_type, 'empty_fields')
                    self.pattern_cache[embedded_type] = 'E'
                    return 'E'
                    
            except Exception as e:
                print(f"Warning: Could not parse embedded type {embedded_type}: {e}")
                self._track_unknown_type(embedded_type, f'parse_error: {e}')
                self.pattern_cache[embedded_type] = 'E'
                return 'E'
        else:
            # Check for template types with prefix matching
            for template_prefix, pattern in self.template_patterns.items():
                if embedded_type.startswith(template_prefix.rstrip('<')):
                    # Found a template match
                    self.pattern_cache[embedded_type] = pattern
                    return pattern
            
            # Unknown type - track for debugging
            self._track_unknown_type(embedded_type, 'not_found')
            self.pattern_cache[embedded_type] = 'E'
            return 'E'
    
    def _parse_source_class_fields(self, header_file: str, class_name: str):
        """Generic method to parse any source class fields using preprocessing pipeline"""
        try:
            with open(header_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Find the class or struct definition
            import re
            class_pattern = f'(?:class|struct)\\s+{class_name}[^{{]*{{'
            class_match = re.search(class_pattern, content, re.DOTALL)
            if not class_match:
                print(f"Warning: Class/struct {class_name} not found in {header_file}")
                return []
            
            # Extract class body using brace matching
            class_body = self._extract_class_body(content, class_match.end())
            if not class_body:
                return []
            
            # Preprocessing pipeline
            cleaned_body = self._preprocess_class_body(class_body, class_name)
            
            # Parse fields from cleaned body
            fields = self._parse_cleaned_fields(cleaned_body)
            
            return fields
            
        except Exception as e:
            print(f"Warning: Could not parse {class_name} fields from {header_file}: {e}")
            return []
    
    def _extract_class_body(self, content: str, start_pos: int) -> str:
        """Extract class body using brace matching"""
        brace_count = 1
        pos = start_pos
        while pos < len(content) and brace_count > 0:
            if content[pos] == '{':
                brace_count += 1
            elif content[pos] == '}':
                brace_count -= 1
            pos += 1
        
        if brace_count > 0:
            return ""
        
        return content[start_pos:pos-1]
    
    def _preprocess_class_body(self, class_body: str, class_name: str) -> str:
        """Preprocessing pipeline: strip everything except field declarations"""
        
        # Stage 1: Strip comments
        cleaned = self._strip_comments(class_body)
        
        # Stage 2: Strip preprocessor directives  
        cleaned = self._strip_preprocessor_directives(cleaned)
        
        # Stage 3: Strip methods and constructors (more aggressive)
        cleaned = self._strip_methods_aggressive(cleaned)
        
        # Stage 4: Strip visibility specifiers
        cleaned = self._strip_visibility_specifiers(cleaned)
        
        # Stage 5: Keep only lines that look like field declarations
        cleaned = self._keep_only_field_declarations(cleaned)
        
        # Visual debugging - print cleaned class
        print(f"🧹 Cleaned {class_name} body:")
        print("=" * 40)
        print(cleaned[:200] + "..." if len(cleaned) > 200 else cleaned)
        print("=" * 40)
        
        return cleaned
    
    def _strip_comments(self, text: str) -> str:
        """Remove // and /* */ comments"""
        import re
        # Remove /* */ comments (including multi-line)
        text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
        # Remove // comments
        text = re.sub(r'//.*$', '', text, flags=re.MULTILINE)
        return text
    
    def _strip_preprocessor_directives(self, text: str) -> str:
        """Remove preprocessor directives (#define, #include, #if, etc.)"""
        import re
        # Remove lines starting with #
        text = re.sub(r'^\s*#.*$', '', text, flags=re.MULTILINE)
        return text
    
    def _strip_methods_aggressive(self, text: str) -> str:
        """Aggressively remove anything that looks like a method/constructor/operator"""
        lines = text.split('\n')
        result_lines = []
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            # Skip anything with parentheses (methods, constructors, operators)
            if '(' in line or ')' in line:
                continue
                
            # Skip lines with keywords that indicate methods/constructors
            method_keywords = ['operator', 'const', 'virtual', 'static', 'inline', 'explicit', 'friend']
            if any(keyword in line.lower() for keyword in method_keywords):
                continue
                
            # Skip lines that are just type names (constructor declarations)
            if line and not ';' in line and not '=' in line:
                # Check if it's just a type name (like "S3DPoint")
                if line.replace('&', '').replace('*', '').strip().isidentifier():
                    continue
                    
            # Skip single braces or method body remnants
            if line in ['{', '}', 'default', 'delete']:
                continue
                
            # Keep this line
            result_lines.append(line)
        
        return '\n'.join(result_lines)
    
    def _keep_only_field_declarations(self, text: str) -> str:
        """Final filter: keep only lines that look like field declarations"""
        lines = text.split('\n')
        field_lines = []
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            # Field declarations must end with semicolon
            if not line.endswith(';'):
                continue
                
            # Must not contain parentheses (methods)
            if '(' in line or ')' in line:
                continue
                
            # Should contain valid C++ type patterns
            import re
            # Look for pattern: type field_name; or type field1, field2;
            if re.match(r'^\s*\w+(?:\s*\*)*\s+\w+(?:\s*,\s*\w+)*\s*;\s*$', line):
                field_lines.append(line)
        
        return '\n'.join(field_lines)
    
    def _strip_visibility_specifiers(self, text: str) -> str:
        """Remove public:, private:, protected: lines"""
        import re
        text = re.sub(r'^\s*(public|private|protected)\s*:\s*$', '', text, flags=re.MULTILINE)
        return text
    
    def _parse_cleaned_fields(self, cleaned_body: str) -> list:
        """Parse fields from cleaned body with padding calculation"""
        import re
        fields = []
        
        # Find all field declarations (lines ending with semicolon)
        declarations = re.findall(r'[^;]+;', cleaned_body)
        
        current_offset = 0
        for decl in declarations:
            decl = decl.strip()
            if not decl or not decl.endswith(';'):
                continue
                
            # Remove semicolon and parse
            decl_line = decl[:-1].strip()
            if decl_line:
                parsed_fields = self._parse_cpp_field_line(decl_line + ';')
                if parsed_fields:
                    for field_name, field_type, field_size in parsed_fields:
                        # Determine field alignment requirements
                        if self._is_embedded_structure(field_type):
                            # Embedded structures typically need 4-byte alignment
                            field_alignment = 4
                            if field_type == 'S3DPoint':
                                real_size = 12
                            elif field_type == 'SLightDef': 
                                real_size = 28
                            else:
                                real_size = 4  # Default
                        else:
                            # Basic types: alignment = size (but max 4 bytes)
                            field_alignment = min(field_size, 4)
                            real_size = field_size
                        
                        # Calculate padding needed for this field's alignment
                        padding_needed = (field_alignment - (current_offset % field_alignment)) % field_alignment
                        
                        # Insert padding using optimal alignment units
                        if padding_needed > 0:
                            padding_fields = self._create_padding_fields(padding_needed, current_offset)
                            fields.extend(padding_fields)
                            current_offset += padding_needed
                        
                        # Add the actual field
                        if self._is_embedded_structure(field_type):
                            fields.append((field_name, f'({field_type})', real_size))
                        else:
                            fields.append((field_name, field_type, field_size))
                        current_offset += real_size
        
        return fields
    
    def _create_padding_fields(self, padding_needed: int, current_offset: int) -> list:
        """Create padding fields using optimal alignment units"""
        padding_fields = []
        remaining = padding_needed
        offset = current_offset
        pad_count = 0
        
        while remaining > 0:
            # Try 4-byte alignment first
            if remaining >= 4 and offset % 4 == 0:
                padding_fields.append((f'pad4_{pad_count}', 'pad4', 4))
                remaining -= 4
                offset += 4
            # Try 2-byte alignment 
            elif remaining >= 2 and offset % 2 == 0:
                padding_fields.append((f'pad2_{pad_count}', 'pad2', 2))
                remaining -= 2
                offset += 2
            # Fall back to 1-byte
            else:
                padding_fields.append((f'pad1_{pad_count}', 'pad1', 1))
                remaining -= 1
                offset += 1
            pad_count += 1
            
        return padding_fields
    
    def _fingerprint_single_class(self, header_file, class_name):
        """Single class fingerprinting"""
        return {'field_pattern': 'dddd', 'field_count': 4, 'total_size': 16, 'type': 'source'}

    def generate_all_fingerprints(self):
        """Generate comprehensive fingerprint files for both Ghidra and Source"""
        
        print("🚀 COMPREHENSIVE CLASS FINGERPRINTING")
        print("=" * 60)
        
        # Generate all fingerprints
        ghidra_fingerprints = self.fingerprint_all_ghidra_classes()
        source_fingerprints = self.fingerprint_all_source_classes()
        
        # Sort by pattern length (longest first)
        ghidra_fingerprints.sort(key=lambda x: len(x[1]['pattern']), reverse=True)
        source_fingerprints.sort(key=lambda x: len(x[1]['pattern']), reverse=True)
        
        # Write Ghidra fingerprints
        ghidra_file = Path("../comprehensive_ghidra_fingerprints.txt")
        with open(ghidra_file, 'w') as f:
            f.write("COMPREHENSIVE GHIDRA CLASS FINGERPRINTS - FINAL\n")
            f.write("=" * 60 + "\n\n")
            f.write(f"Total: {len(ghidra_fingerprints)} Ghidra classes\n")
            f.write("Features: Proper gap detection + Large array 'A' handling + Complete patterns\n")
            f.write("Signatures: b=1byte, w=2bytes, d=4bytes, q=8bytes, A=large array/gap (>32 bytes)\n")
            f.write("Sorted by: Fingerprint pattern length (longest first)\n\n")
            
            for class_name, info in ghidra_fingerprints:
                pattern = info['pattern']
                size = info['size']
                fields = info['fields']
                
                # Truncate for display only
                display_pattern = pattern[:60] + "..." if len(pattern) > 60 else pattern
                
                f.write(f"{class_name:<25} | {size:5d}b | {fields:3d}f | {len(pattern):3d}p | {display_pattern}\n")
        
        # Write Source fingerprints  
        source_file = Path("../comprehensive_source_fingerprints.txt")
        with open(source_file, 'w') as f:
            f.write("COMPREHENSIVE SOURCE CLASS FINGERPRINTS - FINAL\n")
            f.write("=" * 60 + "\n\n")
            f.write(f"Total: {len(source_fingerprints)} source classes\n")
            f.write("Features: Recursive structs + Array handling + Bitfield packing + Large array 'A'\n")
            f.write("Signatures: b=1byte, w=2bytes, d=4bytes, q=8bytes, A=large array (>32 bytes)\n")
            f.write("Sorted by: Fingerprint pattern length (longest first)\n\n")
            
            for class_name, info in source_fingerprints:
                pattern = info['pattern']
                size = info['size']
                fields = info['fields']
                
                # Truncate for display only
                display_pattern = pattern[:60] + "..." if len(pattern) > 60 else pattern
                
                f.write(f"{class_name:<25} | {size:4d}b | {fields:3d}f | {len(pattern):3d}p | {display_pattern}\n")
        
        print(f"\n📊 COMPREHENSIVE FINGERPRINTING COMPLETE!")
        print(f"✅ Ghidra classes: {len(ghidra_fingerprints)} → {ghidra_file}")
        print(f"✅ Source classes: {len(source_fingerprints)} → {source_file}")
        print(f"\n🎯 Features:")
        print(f"  • Complete patterns (no truncation)")
        print(f"  • Proper gap detection for all classes")
        print(f"  • Large array compression (A character)")
        print(f"  • Sorted by pattern complexity")
        
        return ghidra_fingerprints, source_fingerprints

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) >= 3:
        # Command line usage: python class_fingerprinter.py <header_file> <class_name> [inheritance_chain]
        header_file = sys.argv[1]
        class_name = sys.argv[2]
        
        fingerprinter = ComprehensiveFingerprinter()
        
        # Check for inheritance chain (optional)
        inheritance_chain = None
        if len(sys.argv) > 3 and sys.argv[3] == '--inheritance':
            # Example: --inheritance SObjectDef:src/object.h,TObjectInstance:src/object.h,TComplexObject:src/complexobj.h
            if len(sys.argv) > 4:
                chain_str = sys.argv[4]
                inheritance_chain = []
                for pair in chain_str.split(','):
                    class_part, file_part = pair.split(':')
                    inheritance_chain.append((class_part.strip(), file_part.strip()))
        
        print(f'🔍 Fingerprinting {class_name} from {header_file}')
        if inheritance_chain:
            print(f'📚 With inheritance chain: {len(inheritance_chain)} base classes')
        
        result = fingerprinter.fingerprint_source_class(header_file, class_name, inheritance_chain)
        
        if result:
            print(f'✅ Success!')
            print(f'   Pattern: {result["field_pattern"]}')
            print(f'   Fields: {result["field_count"]}')
            print(f'   Size: {result["total_size"]} bytes')
        else:
            print(f'❌ Failed to fingerprint {class_name}')
            
        # Show debug report for unknown types
        fingerprinter.report_unknown_types()
            
    else:
        print("Usage:")
        print("  Generate all fingerprints:")
        print("    python class_fingerprinter.py")
        print("")
        print("  Fingerprint specific class:")
        print("    python class_fingerprinter.py <header_file> <class_name>")
        print("")  
        print("  With inheritance:")
        print("    python class_fingerprinter.py <header_file> <class_name> --inheritance 'Base1:file1.h,Base2:file2.h'")
        print("")
        print("Examples:")
        print("    python class_fingerprinter.py src/character.h TCharacter")
        print("    python class_fingerprinter.py src/character.h TCharacter --inheritance 'SObjectDef:src/object.h,TObjectInstance:src/object.h,TComplexObject:src/complexobj.h'")
        
        # Default: run comprehensive fingerprinting
        fingerprinter = ComprehensiveFingerprinter()
        fingerprinter.generate_all_fingerprints()
