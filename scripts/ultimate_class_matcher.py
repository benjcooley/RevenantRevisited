#!/usr/bin/env python3
"""
Ultimate Class Matcher - Final comprehensive class matching system
Extracts ALL real source classes, matches against Ghidra with improved algorithms,
identifies issues, and generates mapping files. Designed for reuse and continuous improvement.
"""

import json
import re
from pathlib import Path
from collections import defaultdict
from typing import Dict, List, Optional, Tuple

class SourceClassExtractor:
    """Improved source class fingerprint extraction"""
    
    def extract_from_header(self, header_file: Path) -> Optional[Dict]:
        """Extract comprehensive fingerprint from C++ header"""
        try:
            with open(header_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            # Clean and normalize content
            content = self.preprocess_content(content)
            
            # Find all class/struct definitions
            classes = self.find_class_definitions(content)
            
            if not classes:
                return None
                
            # Use the largest/most complex class
            best_class = max(classes, key=lambda c: c['field_count'])
            
            return {
                'class_name': best_class['name'],
                'source_file': header_file.name,
                'total_size': best_class['estimated_size'],
                'field_count': best_class['field_count'],
                'field_pattern': best_class['field_pattern'],  # Type pattern fingerprint
                'complexity_score': self.calculate_complexity(best_class),
                'inheritance_depth': best_class['inheritance_depth'],
                'has_virtuals': best_class['has_virtuals'],
                'field_types': best_class['field_types'][:10],  # Sample for debugging
                'fields': best_class['fields']  # NEW: Store actual field data
            }
            
        except Exception as e:
            print(f"⚠️  Error extracting {header_file.name}: {e}")
            return None
            
    def preprocess_content(self, content: str) -> str:
        """Clean C++ content for better parsing"""
        # Remove comments
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        
        # Remove preprocessor directives
        content = re.sub(r'^\s*#.*$', '', content, flags=re.MULTILINE)
        
        # Normalize whitespace
        content = re.sub(r'\s+', ' ', content)
        
        return content
        
    def find_class_definitions(self, content: str) -> List[Dict]:
        """Find all class/struct definitions in content"""
        classes = []
        
        # Pattern 1: Standard class/struct with inheritance
        pattern1 = r'(?:class|struct)\s+(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+(?:,\s*\w+)*))?\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}'
        
        # Pattern 2: Custom _CLASSDEF macro pattern (fixed)
        pattern2 = r'_CLASSDEF\(\s*(\w+)\s*\)[\s\S]*?class\s+\1\s*(?::\s*(?:public|private|protected)?\s*(\w+(?:,\s*\w+)*))?\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}'
        
        # Pattern 3: Template classes
        pattern3 = r'template\s*<[^>]*>\s*(?:class|struct)\s+(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+(?:,\s*\w+)*))?\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\}'
        
        for pattern in [pattern1, pattern2, pattern3]:
            for match in re.finditer(pattern, content, re.DOTALL | re.IGNORECASE):
                class_name = match.group(1)
                inheritance = match.group(2) if len(match.groups()) > 1 and match.group(2) else ""
                class_body = match.group(3) if len(match.groups()) > 2 else match.group(2)
                
                # Skip if no class body found
                if not class_body:
                    continue
                    
                fields = self.extract_fields(class_body)
                
                # Skip if no fields extracted (likely parsing error)
                if len(fields) == 0:
                    continue
                
                classes.append({
                    'name': class_name,
                    'inheritance': inheritance,
                    'inheritance_depth': len(inheritance.split(',')) if inheritance else 0,
                    'has_virtuals': 'virtual' in class_body,
                    'field_count': len(fields),
                    'estimated_size': self.estimate_class_size(fields),
                    'field_pattern': self.create_field_pattern(fields),  # NEW: Pattern fingerprint
                    'field_types': [f[1] if len(f) >= 2 else f[0] for f in fields],  # Type names
                    'fields': fields  # Store full field data with offsets
                })
                
        return classes
        
    def extract_fields(self, class_body: str) -> List[Tuple[str, str, int]]:
        """Extract field declarations from class body with offset estimation"""
        fields = []
        current_offset = 0
        
        # Clean up the class body - remove comments and normalize whitespace
        cleaned_body = re.sub(r'//.*?$', '', class_body, flags=re.MULTILINE)
        cleaned_body = re.sub(r'/\*.*?\*/', '', cleaned_body, flags=re.DOTALL)
        
        # Split by semicolons and analyze each potential field
        declarations = cleaned_body.split(';')
        
        for decl in declarations:
            decl = decl.strip()
            if not decl:
                continue
                
            # Skip access specifiers, methods, constructors, destructors
            if (any(keyword in decl for keyword in 
                   ['public:', 'private:', 'protected:', '(', ')', 'virtual', 'static', 'friend', 
                    'typedef', 'enum', 'class', 'struct', '~', 'return']) or
                decl.endswith(':') or
                '::' in decl and '(' in decl):  # Method calls
                continue
                
            # Try to parse as field declaration
            field = self.parse_field_declaration(decl)
            if field:
                field_name, field_type = field
                field_size = self.estimate_field_size(field_type)
                fields.append((field_name, field_type, current_offset))
                current_offset += field_size
                
        return fields
        
    def parse_field_declaration(self, decl: str) -> Optional[Tuple[str, str]]:
        """Parse a field declaration into (name, type)"""
        decl = decl.strip()
        
        # Handle common field patterns - more comprehensive
        patterns = [
            # Standard types with optional pointers and arrays
            r'((?:unsigned\s+|signed\s+)?(?:char|short|int|long|float|double|bool)(?:\s*\*)*)\s+(\w+)(?:\[[\w\s]*\])*$',
            # Custom types (TObject*, S3DPoint, etc.)
            r'([TS]\w+(?:\s*\*)*)\s+(\w+)(?:\[[\w\s]*\])*$',
            # Basic types with modifiers
            r'((?:byte|word|dword|int32_t|uint32_t|int16_t|uint16_t|int8_t|uint8_t)(?:\s*\*)*)\s+(\w+)(?:\[[\w\s]*\])*$',
            # Pointer types
            r'(\w+\s*\*+)\s+(\w+)$',
            # Simple type name patterns
            r'(\w+)\s+(\w+)(?:\[[\w\s]*\])*$'
        ]
        
        for pattern in patterns:
            match = re.match(pattern, decl)
            if match:
                type_name = match.group(1).strip()
                field_name = match.group(2).strip()
                
                # Skip obvious non-fields
                if field_name in ['return', 'if', 'for', 'while', 'switch', 'case', 'break', 'continue']:
                    continue
                    
                return (field_name, type_name)
                
        return None
        
    def estimate_field_size(self, field_type: str) -> int:
        """Estimate field size in bytes"""
        field_type = field_type.lower().strip()
        
        # Pointer types
        if '*' in field_type:
            return 4  # 32-bit pointers
            
        # Array types - estimate as pointer for now
        if '[' in field_type:
            return 4
            
        # Size map for basic types
        size_map = {
            'char': 1, 'byte': 1, 'bool': 1, 'int8_t': 1, 'uint8_t': 1,
            'short': 2, 'word': 2, 'int16_t': 2, 'uint16_t': 2,
            'int': 4, 'long': 4, 'float': 4, 'dword': 4, 'int32_t': 4, 'uint32_t': 4,
            'double': 8, 'long long': 8
        }
        
        for type_name, size in size_map.items():
            if type_name in field_type:
                return size
                
        # Default size for unknown types (assume pointer or struct)
        return 4
        
    def estimate_class_size(self, fields: List) -> int:
        """Improved size estimation based on field types"""
        size_map = {
            'char': 1, 'byte': 1, 'bool': 1, 'BYTE': 1,
            'short': 2, 'word': 2, 'WORD': 2,
            'int': 4, 'long': 4, 'float': 4, 'dword': 4, 'DWORD': 4,
            'double': 8, 'longlong': 8, '__int64': 8,
        }
        
        total_size = 0
        for field in fields:
            if len(field) >= 2:
                field_name, type_name = field[0], field[1]
            else:
                continue
            # Pointer types
            if '*' in type_name:
                total_size += 4  # 32-bit pointers
            # Array types (estimate)
            elif '[' in field_name:
                base_size = 4  # Default element size
                for base_type, size in size_map.items():
                    if base_type in type_name.lower():
                        base_size = size
                        break
                # Try to extract array size
                array_match = re.search(r'\[(\d+)\]', field_name)
                array_size = int(array_match.group(1)) if array_match else 10  # Default guess
                total_size += base_size * array_size
            # Regular types
            else:
                type_size = 4  # Default
                for base_type, size in size_map.items():
                    if base_type in type_name.lower():
                        type_size = size
                        break
                total_size += type_size
                
        # Add padding and virtual table pointer if needed
        if total_size > 0:
            total_size += (4 - (total_size % 4)) % 4  # Alignment padding
            
        return max(total_size, len(fields) * 4)  # Minimum reasonable size
        
    def create_field_pattern(self, fields: List) -> str:
        """Create field pattern fingerprint - SIZE ONLY (to match Ghidra capabilities)"""
        pattern = ""
        
        for field in fields:
            if len(field) >= 2:
                field_name, type_name = field[0], field[1]
            else:
                continue
            # Calculate actual byte size for this field type
            field_size = self.get_field_byte_size(field_name, type_name)
            
            # Map size to pattern character - IDENTICAL to Ghidra logic
            if field_size == 1:
                pattern += 'b'  # byte (1)
            elif field_size == 2:
                pattern += 'w'  # word (2) 
            elif field_size == 4:
                pattern += 'd'  # dword (4)
            elif field_size == 8:
                pattern += 'q'  # qword (8)
            elif field_size == 3:
                pattern += 'x'  # 3-byte field (uncommon)
            elif field_size > 8 and field_size <= 16:
                pattern += 'D'  # double-dword (9-16 bytes)
            elif field_size > 16 and field_size <= 64:
                pattern += 'S'  # embedded struct (17-64 bytes)
            elif field_size > 64:
                pattern += 'A'  # large array/buffer (65+ bytes)
            else:
                pattern += 'd'  # default to dword for weird sizes
                    
        return pattern
        
    def get_field_byte_size(self, field_name: str, type_name: str) -> int:
        """Calculate actual byte size for a source field"""
        # Handle arrays first
        if '[' in field_name:
            array_match = re.search(r'\[(\d+)\]', field_name)
            array_size = int(array_match.group(1)) if array_match else 10  # Default guess
            
            # Base type size
            if any(t in type_name.lower() for t in ['char', 'byte', 'bool']):
                return array_size * 1
            elif any(t in type_name.lower() for t in ['short', 'word']):
                return array_size * 2
            elif any(t in type_name.lower() for t in ['double', 'longlong', '__int64']):
                return array_size * 8
            else:
                return array_size * 4  # Most types are 4 bytes
        
        # Non-array types - pure size mapping
        if any(t in type_name.lower() for t in ['char', 'byte', 'bool']):
            return 1
        elif any(t in type_name.lower() for t in ['short', 'word']):
            return 2
        elif any(t in type_name.lower() for t in ['double', 'longlong', '__int64']):
            return 8
        else:
            # Everything else is 4 bytes: int, long, float, pointers, objects, structs
            return 4
        
    def calculate_complexity(self, class_info: Dict) -> int:
        """Calculate complexity score for prioritization"""
        score = 0
        score += class_info['field_count'] * 2
        score += class_info['inheritance_depth'] * 5
        score += 10 if class_info['has_virtuals'] else 0
        score += len(set(class_info['field_types'])) * 3  # Type diversity
        return score

class UltimateClassMatcher:
    """Final comprehensive class matching system"""
    
    def __init__(self):
        self.source_fingerprints = {}
        self.ghidra_fingerprints = {}
        self.extractor = SourceClassExtractor()
        self.load_fingerprints()
        
    def load_fingerprints(self):
        """Load improved source and Ghidra fingerprints"""
        print("ULTIMATE CLASS MATCHER - LOADING FINGERPRINTS")
        print("=" * 60)
        
        # Extract ALL real source classes with improved parsing
        source_dir = Path("/Users/benjamincooley/projects/RevenantRevisited/src")
        extracted_count = 0
        failed_count = 0
        
        for header_file in sorted(source_dir.glob("*.h")):
            fingerprint = self.extractor.extract_from_header(header_file)
            if fingerprint and fingerprint['field_count'] > 0:
                class_name = fingerprint['class_name']
                self.source_fingerprints[class_name] = fingerprint
                extracted_count += 1
                if fingerprint['complexity_score'] > 10:  # Lowered threshold to capture more classes
                    pattern = fingerprint.get('field_pattern', 'N/A')
                    print(f"✅ {class_name}: {fingerprint['total_size']} bytes, "
                          f"{fingerprint['field_count']} fields, "
                          f"complexity: {fingerprint['complexity_score']} [{header_file.name}]")
                    print(f"   🧬 Pattern: {pattern}")
            else:
                failed_count += 1
                
        print(f"\n📊 Extracted {extracted_count} classes, {failed_count} failed")
        
        # Load and enhance Ghidra classes with patterns
        with open('class_fingerprints.json') as f:
            data = json.load(f)
            
        raw_ghidra = {name: info for name, info in data['ghidra_classes'].items() 
                      if name.startswith('cls_0x')}
        
        # Enhanced filtering and pattern generation for substantial classes
        self.ghidra_fingerprints = {}
        for name, info in raw_ghidra.items():
            if self.is_substantial_ghidra_class(info):
                # Add field pattern to Ghidra fingerprint
                info['field_pattern'] = self.create_ghidra_field_pattern(info.get('fields', []))
                self.ghidra_fingerprints[name] = info
                
        print(f"📊 Loaded {len(self.ghidra_fingerprints)} substantial Ghidra classes")
        print()
        
    def is_substantial_ghidra_class(self, info: Dict) -> bool:
        """Enhanced filtering for substantial Ghidra classes"""
        size = info['total_size']
        fields = info['field_count']
        
        # Minimum complexity requirements
        if fields < 3 or size < 32:
            return False
            
        # Filter out clearly merged/corrupted classes
        bytes_per_field = size / fields if fields > 0 else 0
        if bytes_per_field > 300:  # Extremely high ratio suggests corruption
            return False
            
        # Filter out trivial repetitive patterns
        if size > 1000 and fields < 10:  # Large but few fields - likely arrays/buffers
            return False
            
        return True
        
    def create_ghidra_field_pattern(self, fields: List) -> str:
        """Create field pattern from Ghidra field data with gap detection"""
        pattern = ""
        last_offset = 0
        
        # Sort fields by offset to detect gaps
        sorted_fields = sorted([f for f in fields if len(f) >= 4], 
                              key=lambda f: f[1])  # Sort by offset
        
        for field_data in sorted_fields:
            field_name, offset, size, type_name = field_data[:4]
            
            # Detect gap between this field and last
            gap_size = offset - last_offset
            if gap_size > 0:
                if gap_size > 32:
                    pattern += 'A'  # Large gap/array (>32 bytes)
                else:
                    # Fill gap with appropriate padding
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
            
            # Add pattern for current field
            if size == 1:
                pattern += 'b'  # byte (1)
            elif size == 2:
                pattern += 'w'  # word (2) 
            elif size == 4:
                pattern += 'd'  # dword (4) - could be int, float, pointer, object ref
            elif size == 8:
                pattern += 'q'  # qword (8) - could be double, long long
            elif size == 3:
                pattern += 'x'  # 3-byte field (uncommon)
            elif size > 8 and size <= 16:
                pattern += 'D'  # double-dword (9-16 bytes)
            elif size > 16 and size <= 32:
                pattern += 'S'  # embedded struct (17-32 bytes)
            elif size > 32:
                pattern += 'A'  # large array/buffer (>32 bytes)
            else:
                pattern += 'd'  # default to dword for weird sizes
                
            last_offset = offset + size
                
        return pattern
        
    def longest_common_subsequence(self, s1: str, s2: str) -> int:
        """Calculate LCS length for pattern similarity"""
        m, n = len(s1), len(s2)
        dp = [[0] * (n + 1) for _ in range(m + 1)]
        
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                if s1[i-1] == s2[j-1]:
                    dp[i][j] = dp[i-1][j-1] + 1
                else:
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1])
        
        return dp[m][n]
    
    def calculate_pattern_similarity(self, pattern1: str, pattern2: str) -> float:
        """Calculate pattern similarity using LCS and edit distance"""
        if not pattern1 or not pattern2:
            return 0.0
            
        # LCS similarity
        lcs_length = self.longest_common_subsequence(pattern1, pattern2)
        max_length = max(len(pattern1), len(pattern2))
        lcs_similarity = (lcs_length / max_length * 100) if max_length > 0 else 0
        
        # Simple edit distance approximation
        min_length = min(len(pattern1), len(pattern2))
        common_prefix = 0
        for i in range(min_length):
            if pattern1[i] == pattern2[i]:
                common_prefix += 1
            else:
                break
                
        prefix_similarity = (common_prefix / max_length * 100) if max_length > 0 else 0
        
        # Combine similarities
        return max(lcs_similarity, prefix_similarity)
        
    def calculate_pattern_diversity(self, pattern: str) -> float:
        """Calculate pattern diversity (entropy-like measure)"""
        if not pattern:
            return 0.0
        
        # Count unique characters
        unique_chars = len(set(pattern))
        total_chars = len(pattern)
        
        # Diversity score: more unique chars = higher diversity
        diversity = unique_chars / total_chars if total_chars > 0 else 0
        
        # Bonus for specific diverse patterns
        if unique_chars >= 4:  # Very diverse
            diversity *= 1.5
        elif unique_chars >= 3:  # Moderately diverse  
            diversity *= 1.2
            
        return min(diversity, 1.0)
    
    def calculate_size_uniqueness(self, size: int) -> float:
        """Calculate how unique this size is in the dataset"""
        # Count how many classes have similar sizes (±8 bytes)
        similar_count = 0
        total_count = len(self.ghidra_fingerprints)
        
        for ghidra_info in self.ghidra_fingerprints.values():
            if abs(ghidra_info['total_size'] - size) <= 8:
                similar_count += 1
        
        # Uniqueness: fewer similar sizes = more unique
        uniqueness = 1.0 - (similar_count / total_count) if total_count > 0 else 0
        return max(uniqueness, 0.1)  # Minimum uniqueness of 0.1

    def calculate_enhanced_confidence(self, source_info: Dict, ghidra_info: Dict) -> Dict:
        """Enhanced confidence with field pattern matching"""
        source_size = source_info['total_size']
        ghidra_size = ghidra_info['total_size']
        source_fields = source_info['field_count']
        ghidra_fields = ghidra_info['field_count']
        source_pattern = source_info.get('field_pattern', '')
        ghidra_pattern = ghidra_info.get('field_pattern', '')
        
        # Size and field similarity
        size_diff = abs(source_size - ghidra_size)
        size_ratio = min(source_size, ghidra_size) / max(source_size, ghidra_size)
        field_diff = abs(source_fields - ghidra_fields)
        field_ratio = min(source_fields, ghidra_fields) / max(source_fields, ghidra_fields) if max(source_fields, ghidra_fields) > 0 else 0
        
        # NEW: Pattern similarity scoring
        pattern_score = self.calculate_pattern_similarity(source_pattern, ghidra_pattern)
        
        # NEW: Containment analysis for detecting merged/split classes  
        containment = self.analyze_containment_patterns(source_pattern, ghidra_pattern)
        
        # Enhanced scoring system with pattern weighting
        confidence_score = 0
        
        # Pattern matching with containment analysis
        containment_bonus = 0
        containment_type = 'none'
        
        # Boost score for containment patterns (merged/split class detection)
        if containment['confidence'] >= 90:
            containment_bonus = 20
            containment_type = 'strong_containment'
        elif containment['confidence'] >= 70:
            containment_bonus = 15  
            containment_type = 'moderate_containment'
        elif containment['confidence'] >= 50:
            containment_bonus = 10
            containment_type = 'weak_containment'
            
        # HARD SIZE CONSTRAINTS - reject massive size differences
        if size_diff > 1000:  # 1KB+ difference is definitely wrong
            confidence_score = max(10, int(size_ratio * 20))  # Cap at very low score
        elif size_diff > 500:  # 500B+ difference is very suspicious
            confidence_score = max(20, int(size_ratio * 30))  # Cap at low score
        elif size_diff > 200:  # 200B+ difference needs good justification
            confidence_score = max(30, int(size_ratio * 50))  # Cap at medium-low score
        elif size_diff > 100:  # 100B+ difference is concerning
            confidence_score = max(40, min(70, int(pattern_score * 0.7)))  # Reduced pattern weight
            
        # Primary pattern discriminator with containment bonus (ONLY for reasonable size diffs)
        elif pattern_score >= 95:  # Near-perfect pattern match
            confidence_score = 95 + (5 if size_diff == 0 else 0) + containment_bonus
        elif pattern_score >= 80:  # Very good pattern match
            confidence_score = 80 + min(10, int((100 - size_diff/10))) + containment_bonus
        elif pattern_score >= 60:  # Good pattern match
            confidence_score = 60 + min(15, int(size_ratio * 15)) + containment_bonus
        elif pattern_score >= 40:  # Moderate pattern match
            confidence_score = 40 + min(20, int(field_ratio * 20)) + containment_bonus
        elif containment['confidence'] >= 70:  # Strong containment even with poor pattern
            confidence_score = 50 + containment_bonus
        else:
            # Fall back to old size/field matching for poor patterns
            if size_diff == 0 and field_diff == 0:
                confidence_score = 70  # Lower than pattern matches
            elif size_diff <= 8 and field_diff <= 2:
                confidence_score = 60
            elif size_ratio >= 0.9 and field_ratio >= 0.8:
                confidence_score = 50
            else:
                confidence_score = max(20, int(size_ratio * 40))
            
        # Complexity bonus
        complexity = source_info['complexity_score']
        if complexity > 50 and confidence_score >= 80:
            confidence_score += 5
        elif complexity > 30 and confidence_score >= 60:
            confidence_score += 3
            
        # Convert to confidence level
        if confidence_score >= 95:
            confidence = "PERFECT"
        elif confidence_score >= 85:
            confidence = "HIGH" 
        elif confidence_score >= 70:
            confidence = "MEDIUM"
        elif confidence_score >= 55:
            confidence = "LOW"
        else:
            confidence = "VERY_LOW"
            
        # Calculate diversity and uniqueness for output
        source_diversity = self.calculate_pattern_diversity(source_pattern)
        ghidra_diversity = self.calculate_pattern_diversity(ghidra_pattern)
        avg_diversity = (source_diversity + ghidra_diversity) / 2
        
        source_uniqueness = self.calculate_size_uniqueness(source_size)
        
        return {
            'score': confidence_score,
            'level': confidence,
            'size_diff': size_diff,
            'field_diff': field_diff,
            'size_ratio': size_ratio,
            'field_ratio': field_ratio,
            'pattern_score': pattern_score,
            'pattern_match': f"{source_pattern[:15]} vs {ghidra_pattern[:15]}",
            'source_pattern': source_pattern,
            'ghidra_pattern': ghidra_pattern,
            'containment': containment,
            'containment_type': containment_type,
            'complexity': complexity,
            'diversity': avg_diversity,
            'uniqueness': source_uniqueness
        }
        
# Removed duplicate function - see above for implementation
        max_len = max(len(pattern1), len(pattern2))
        lcs_score = (lcs_length / max_len) * 100 if max_len > 0 else 0
        
        # Edit distance for minor differences
        edit_dist = self.edit_distance(pattern1, pattern2)
        edit_score = max(0, 100 - (edit_dist * 10))
        
        # Combined score (LCS weighted higher for structural matching)
        combined_score = (lcs_score * 0.7) + (edit_score * 0.3)
        
        return int(min(100, combined_score))
        
    def analyze_containment_patterns(self, pattern1: str, pattern2: str) -> Dict:
        """Analyze if one pattern is contained/embedded in another with struct expansion"""
        if not pattern1 or not pattern2:
            return {'type': 'none', 'confidence': 0}
            
        # Direct containment
        if pattern1 in pattern2:
            return {
                'type': 'full_containment', 
                'direction': 'pattern1_in_pattern2',
                'confidence': 100,
                'description': f"'{pattern1}' fully contained in '{pattern2}'"
            }
        elif pattern2 in pattern1:
            return {
                'type': 'full_containment',
                'direction': 'pattern2_in_pattern1', 
                'confidence': 100,
                'description': f"'{pattern2}' fully contained in '{pattern1}'"
            }
            
        # Struct expansion analysis (Ghidra flattening embedded structs)
        struct_expansion = self.analyze_struct_expansion(pattern1, pattern2)
        if struct_expansion['confidence'] >= 70:
            return struct_expansion
            
        # LCS-based partial containment for partial matches
        lcs_length = self.longest_common_subsequence(pattern1, pattern2)
        max_len = max(len(pattern1), len(pattern2))
        
        if lcs_length >= max_len * 0.8:  # 80% overlap
            return {
                'type': 'partial_containment',
                'confidence': int((lcs_length / max_len) * 100),
                'description': f"High overlap: {lcs_length}/{max_len} chars match"
            }
        elif lcs_length >= max_len * 0.6:  # 60% overlap
            return {
                'type': 'weak_containment',
                'confidence': int((lcs_length / max_len) * 80),
                'description': f"Moderate overlap: {lcs_length}/{max_len} chars match"
            }
            
        return {'type': 'none', 'confidence': 0}
        
    def analyze_struct_expansion(self, source_pattern: str, ghidra_pattern: str) -> Dict:
        """Detect if Ghidra flattened embedded structs OR inheritance hierarchies"""
        
        # 1. INHERITANCE PATTERN MATCHING (common for derived classes)
        inheritance_match = self.analyze_inheritance_pattern(source_pattern, ghidra_pattern)
        if inheritance_match['confidence'] >= 70:
            return inheritance_match
            
        # 2. STRUCT EMBEDDING (POINT, RECT, etc.)
        if 'S' not in source_pattern and 'A' not in source_pattern and 'D' not in source_pattern:
            return {'type': 'no_struct_expansion', 'confidence': 0}
            
        # Expand source structs to match Ghidra's flattened view
        expanded_source = self.expand_struct_pattern(source_pattern)
        
        # Compare expanded source with Ghidra pattern
        if expanded_source == ghidra_pattern:
            return {
                'type': 'perfect_struct_expansion',
                'confidence': 95,
                'description': f"Source '{source_pattern}' → '{expanded_source}' = Ghidra '{ghidra_pattern}'",
                'expanded_pattern': expanded_source
            }
            
        # Partial expansion match using LCS
        similarity = self.calculate_pattern_similarity(expanded_source, ghidra_pattern)
        if similarity >= 85:
            return {
                'type': 'strong_struct_expansion',
                'confidence': similarity,
                'description': f"Source '{source_pattern}' → '{expanded_source}' ({similarity}% match)",
                'expanded_pattern': expanded_source
            }
        elif similarity >= 70:
            return {
                'type': 'weak_struct_expansion',
                'confidence': similarity,
                'description': f"Possible expansion ({similarity}% match)",
                'expanded_pattern': expanded_source
            }
            
        return {'type': 'no_struct_expansion', 'confidence': 0}
        
    def expand_struct_pattern(self, pattern: str) -> str:
        """Expand struct/array markers into likely constituent fields"""
        expanded = ""
        
        for char in pattern:
            if char == 'S':
                # Common embedded structs: POINT(dd), RECT(dddd), SIZE(dd)
                expanded += 'dd'  # Conservative 2-field expansion (POINT/SIZE)
            elif char == 'A':
                # Large array/buffer - expand to multiple fields
                expanded += 'dddd'  # Conservative 4-field expansion
            elif char == 'D':
                # Double-dword (9-16 bytes) - likely 2-4 fields
                expanded += 'ddd'  # 3-field expansion (12 bytes)
            elif char == 'x':
                # 3-byte field - might be packed struct or RGB
                expanded += 'bbd'  # byte + byte + dword
            else:
                # Regular fields pass through unchanged
                expanded += char
                
        return expanded
        
    def analyze_inheritance_pattern(self, source_pattern: str, ghidra_pattern: str) -> Dict:
        """Detect inheritance flattening where Ghidra includes base class fields"""
        
        # Normalize both patterns to handle semantic type loss in Ghidra
        normalized_source = self.normalize_pattern_to_size_only(source_pattern)
        normalized_ghidra = ghidra_pattern  # Already size-only
        
        # 1. PREFIX MATCHING: Source pattern as prefix of Ghidra (inheritance)
        if normalized_ghidra.startswith(normalized_source):
            extra_fields = len(normalized_ghidra) - len(normalized_source)
            confidence = max(70, 95 - (extra_fields * 2))  # Penalty for extra fields
            return {
                'type': 'inheritance_prefix_match',
                'confidence': confidence,
                'description': f"Source pattern '{source_pattern}' → '{normalized_source}' is prefix of Ghidra '{ghidra_pattern}' (+{extra_fields} inherited fields)"
            }
            
        # 2. SUFFIX MATCHING: Source pattern as suffix (less common)
        if normalized_ghidra.endswith(normalized_source):
            extra_fields = len(normalized_ghidra) - len(normalized_source)
            confidence = max(65, 90 - (extra_fields * 2))
            return {
                'type': 'inheritance_suffix_match', 
                'confidence': confidence,
                'description': f"Source pattern '{source_pattern}' → '{normalized_source}' is suffix of Ghidra '{ghidra_pattern}' (+{extra_fields} base fields)"
            }
            
        # 3. SEMANTIC TYPE NORMALIZATION: Check if patterns match after normalizing all 4-byte types
        if normalized_source == normalized_ghidra:
            return {
                'type': 'semantic_type_normalization',
                'confidence': 90,
                'description': f"Patterns match after normalizing semantic types: '{source_pattern}' → '{normalized_source}' = '{ghidra_pattern}'"
            }
            
        # 4. PARTIAL INHERITANCE: High LCS similarity suggests inheritance with some changes
        lcs_length = self.longest_common_subsequence(normalized_source, normalized_ghidra)
        if len(normalized_source) > 0:
            similarity = (lcs_length / len(normalized_source)) * 100
            if similarity >= 75:
                return {
                    'type': 'partial_inheritance_match',
                    'confidence': int(similarity),
                    'description': f"High similarity ({similarity:.0f}%) suggests inheritance with modifications"
                }
        
        return {'type': 'no_inheritance_pattern', 'confidence': 0}
        
    def normalize_pattern_to_size_only(self, pattern: str) -> str:
        """Normalize semantic types to size-only for fair comparison with Ghidra"""
        normalized = ""
        for char in pattern:
            if char in ['w', 'b', 'x']:  # Keep different sizes
                normalized += char
            elif char in ['p', 'o', 's']:  # Pointers, objects, structs → dwords in Ghidra
                normalized += 'd'
            else:
                normalized += char
        return normalized
        
    def generate_field_mappings(self, source_name: str, ghidra_class: str) -> Dict:
        """Generate field mappings by aligning source and Ghidra field layouts"""
        
        if source_name not in self.source_fingerprints or ghidra_class not in self.ghidra_fingerprints:
            return {'yaml_content': '  # Could not extract field mappings - missing fingerprint data'}
            
        source_info = self.source_fingerprints[source_name]
        ghidra_info = self.ghidra_fingerprints[ghidra_class]
        
        # Extract actual field data
        source_fields = source_info.get('fields', [])
        ghidra_fields = ghidra_info.get('fields', [])
        
        if not source_fields or not ghidra_fields:
            return {'yaml_content': '  # Field mapping requires field layout data - extracting from definitions...'}
            
        # Align fields by offset and size similarity
        mappings = []
        mapped_ghidra = set()
        
        yaml_lines = []
        yaml_lines.append("  # Automatic field mappings based on size/offset analysis")
        
        for src_name, src_type, src_offset in source_fields[:20]:  # Limit to first 20 fields
            best_match = None
            best_score = 0
            
            for ghidra_field in ghidra_fields:
                ghidra_name, ghidra_offset, ghidra_size, ghidra_type = ghidra_field
                
                if ghidra_name in mapped_ghidra:
                    continue
                    
                # Calculate similarity score
                offset_score = max(0, 100 - abs(src_offset - ghidra_offset) * 2)  # Penalty for offset difference
                size_score = 100 if self.estimate_field_size(src_type) == ghidra_size else 0
                total_score = offset_score + size_score
                
                if total_score > best_score:
                    best_score = total_score
                    best_match = ghidra_field
                    
            if best_match and best_score > 50:  # Reasonable confidence threshold
                ghidra_name = best_match[0]
                mapped_ghidra.add(ghidra_name)
                yaml_lines.append(f'  "{ghidra_name}": "{src_name}"  # {src_type} -> {best_match[3]} (score: {best_score})')
                
        if not yaml_lines or len(yaml_lines) <= 1:
            return {'yaml_content': '  # No confident field mappings found - manual mapping required'}
            
        return {'yaml_content': '\n'.join(yaml_lines)}
        
    def estimate_field_size(self, field_type: str) -> int:
        """Estimate field size in bytes (duplicate of SourceClassExtractor method)"""
        field_type = field_type.lower().strip()
        
        # Pointer types
        if '*' in field_type:
            return 4  # 32-bit pointers
            
        # Array types - estimate as pointer for now
        if '[' in field_type:
            return 4
            
        # Size map for basic types
        size_map = {
            'char': 1, 'byte': 1, 'bool': 1, 'int8_t': 1, 'uint8_t': 1,
            'short': 2, 'word': 2, 'int16_t': 2, 'uint16_t': 2,
            'int': 4, 'long': 4, 'float': 4, 'dword': 4, 'int32_t': 4, 'uint32_t': 4,
            'double': 8, 'long long': 8
        }
        
        for type_name, size in size_map.items():
            if type_name in field_type:
                return size
                
        # Default size for unknown types (assume pointer or struct)
        return 4
        
    def run_analysis(self):
        """Run the complete analysis and display results with REAL matching"""
        print("\nULTIMATE CLASS MATCHER - ANALYSIS RESULTS")
        print("=" * 60)
        
        matches = []
        used_ghidra_classes = set()
        
        for source_name, source_info in self.source_fingerprints.items():
            best_match = None
            best_confidence = 0
            
            # Try every Ghidra class and find the best match
            for ghidra_name, ghidra_info in self.ghidra_fingerprints.items():
                if ghidra_name in used_ghidra_classes:
                    continue  # Ensure 1:1 mapping
                    
                # Calculate REAL confidence using existing logic
                confidence_info = self.calculate_enhanced_confidence(source_info, ghidra_info)
                
                if confidence_info['score'] > best_confidence:
                    best_confidence = confidence_info['score']
                    best_match = {
                        'source_name': source_name,
                        'ghidra_class': ghidra_name,
                        'ghidra_size': ghidra_info['total_size'],
                        'ghidra_fields': ghidra_info['field_count'],
                        'confidence': confidence_info
                    }
            
            # Only accept high-quality matches
            if best_match and best_confidence >= 70:  # Minimum threshold
                matches.append(best_match)
                used_ghidra_classes.add(best_match['ghidra_class'])
                
                # Get source and Ghidra info for pattern display
                source_info = self.source_fingerprints[source_name]
                ghidra_info = self.ghidra_fingerprints[best_match['ghidra_class']]
                
                level = best_match['confidence']['level']
                score = best_match['confidence']['score']
                size_diff = abs(source_info['total_size'] - ghidra_info['total_size'])
                
                # Classify match type using BOTH size and pattern similarity
                pattern_sim = best_match['confidence'].get('pattern_score', 0)
                
                if size_diff <= 20 and pattern_sim >= 95:
                    match_type = "Type A (EXACT)"
                elif size_diff <= 20 and pattern_sim >= 80:
                    match_type = "Type A- (CLOSE)"
                elif size_diff <= 200 and pattern_sim >= 70:
                    match_type = "Type B (INHERITANCE)"
                elif pattern_sim >= 90:  # High pattern match despite size diff
                    match_type = "Type B+ (MERGED/INHERITANCE)"
                else:
                    match_type = "Type C (UNCLEAR)"
                
                conf = best_match['confidence']
                diversity = conf.get('diversity', 0)
                uniqueness = conf.get('uniqueness', 0)
                
                print(f"✅ {source_name} → {best_match['ghidra_class']} ({level}: {score}) - {match_type}")
                print(f"   📏 Size: {source_info['total_size']}b vs {ghidra_info['total_size']}b (±{size_diff}b)")
                print(f"   📊 Fields: {source_info['field_count']} vs {ghidra_info['field_count']}")
                print(f"   🧬 Source:  {source_info.get('field_pattern', 'N/A')}")
                print(f"   🧬 Ghidra:  {ghidra_info.get('field_pattern', 'N/A')}")
                print(f"   🎨 Diversity: {diversity:.2f}, Uniqueness: {uniqueness:.2f}")
                
                # Pattern analysis
                src_pattern = source_info.get('field_pattern', '')
                ghidra_pattern = ghidra_info.get('field_pattern', '')
                if src_pattern in ghidra_pattern:
                    pos = ghidra_pattern.find(src_pattern)
                    if pos == 0:
                        print(f"   💡 Pattern: Source appears to be BASE class (starts at position 0)")
                    else:
                        print(f"   💡 Pattern: Source appears to be DERIVED class (starts at position {pos})")
                elif ghidra_pattern in src_pattern:
                    print(f"   💡 Pattern: Ghidra appears to be subset of source")
                else:
                    # Calculate similarity
                    lcs = self.longest_common_subsequence(src_pattern, ghidra_pattern)
                    similarity = (lcs / max(len(src_pattern), len(ghidra_pattern)) * 100) if max(len(src_pattern), len(ghidra_pattern)) > 0 else 0
                    print(f"   💡 Pattern: {similarity:.1f}% similarity ({lcs} common chars)")
                
                print()
            else:
                print(f"❌ {source_name}: No good match (best: {best_confidence})")
        
        print(f"\n📊 TOTAL ANALYZED: {len(self.source_fingerprints)} source classes")
        print(f"🎯 Generated {len(matches)} high-quality matches")
        
        return matches

if __name__ == "__main__":
    import sys
    
    matcher = UltimateClassMatcher()
    
    if len(sys.argv) > 1 and sys.argv[1] == "--generate-mappings":
        matches = matcher.run_analysis()
        generated_count = 0
        
        for match in matches:
            conf = match['confidence']
            if conf['level'] in ['PERFECT', 'HIGH', 'MEDIUM'] and conf['score'] >= 80:
                source_name = match['source_name']
                ghidra_class = match['ghidra_class']
                
                # Generate YAML mapping file
                mapping_file = Path(f"mappings/{source_name}_{ghidra_class}_map.yaml")
                
                # Generate field mappings
                field_mappings = matcher.generate_field_mappings(source_name, ghidra_class)
                
                source_info = matcher.source_fingerprints[source_name]
                yaml_content = f"""# {source_name} - {conf['level']} CONFIDENCE MATCH
source_class: "{source_name}"
ghidra_class: "{ghidra_class}"
confidence: "{conf['level']}"

# Match details:
# - Score: {conf['score']}/100
# - Source: {source_info['total_size']} bytes, {source_info['field_count']} fields 
# - Ghidra: {match['ghidra_size']} bytes, {match['ghidra_fields']} fields
# - Size difference: ±{conf['size_diff']} bytes
# - Field difference: ±{conf['field_diff']} fields
# - Complexity score: {conf['complexity']}
# - Source file: {source_info['source_file']}

fields:
{field_mappings['yaml_content']}
  
methods:
  # TODO: Extract method mappings using signature analysis
  
parameters:
  # TODO: Add parameter type mappings for cross-class references
"""
                
                mapping_file.write_text(yaml_content)
                generated_count += 1
                
        print(f"\n🎉 Generated {generated_count} mapping files ready for pipeline")
        
    else:
        matches = matcher.run_analysis()
        
    print("\nULTIMATE CLASS MATCHER - COMPLETE")
    print("=" * 60)
    print(f"✅ Analyzed {len(matcher.source_fingerprints)} complex source classes")
    print(f"🎯 Generated {len([m for m in matches if m['confidence']['score'] >= 80])} high-confidence mapping files")
    print("🔧 Ready for Phase 2 pipeline with improved matches")
    print("\nNEXT STEPS:")
    print("1. Run: python3 scripts/run_conversion.py") 
    print("2. Review generated readable code")
    print("3. Investigate problem classes for missing matches")
    print("4. Iterate and improve extraction algorithms")
