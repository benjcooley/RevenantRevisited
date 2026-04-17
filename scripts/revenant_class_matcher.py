#!/usr/bin/env python3
"""
Revenant Class Matcher
Specialized tool for matching decompiled classes with source classes using:
1. Class layout and size matching
2. String constant matching
3. Functional similarity analysis
"""

import os
import re
import json
import struct
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Set, NamedTuple
from dataclasses import dataclass
from collections import defaultdict
import difflib

@dataclass
class ClassLayout:
    """Represents the memory layout of a class"""
    fields: List[Tuple[str, int, int]]  # (name, offset, size)
    total_size: int
    alignment: int

@dataclass
class StringConstant:
    """Represents a string constant found in code"""
    name: str
    value: str
    location: str  # file and line info

@dataclass
class ClassMatch:
    """Represents a match between src and decompiled classes"""
    src_class: str
    decompiled_class: str
    confidence: float
    match_type: str  # 'layout', 'strings', 'functional'
    layout_similarity: float = 0.0
    string_similarity: float = 0.0
    functional_similarity: float = 0.0
    field_mappings: List[Tuple[str, str]] = None  # (src_field, decompiled_field)
    string_mappings: List[Tuple[str, str]] = None  # (src_string, decompiled_string)

class RevenantClassMatcher:
    def __init__(self, src_dir: str = "src", classes_dir: str = "Classes"):
        self.src_dir = Path(src_dir)
        self.classes_dir = Path(classes_dir)
        self.src_classes: Dict[str, Dict] = {}
        self.decompiled_classes: Dict[str, Dict] = {}
        
    def extract_class_layout_from_src(self, content: str) -> ClassLayout:
        """Extract class layout from source code"""
        fields = []
        total_size = 0
        
        # Look for struct/class member definitions
        patterns = [
            r'(\w+)\s+(\w+)\s*;',  # Basic members
            r'(\w+)\s+(\w+)\s*\[[^\]]*\]\s*;',  # Arrays
            r'(\w+)\s*\*\s*(\w+)\s*;',  # Pointers
            r'(\w+)\s*&\s*(\w+)\s*;',  # References
        ]
        
        for pattern in patterns:
            matches = re.findall(pattern, content, re.MULTILINE)
            for match in matches:
                if isinstance(match, tuple):
                    type_name, field_name = match
                else:
                    type_name, field_name = match, ""
                
                # Estimate field size based on type
                size = self.estimate_field_size(type_name)
                if size > 0:
                    fields.append((field_name, total_size, size))
                    total_size += size
        
        return ClassLayout(fields=fields, total_size=total_size, alignment=4)
    
    def extract_class_layout_from_decompiled(self, content: str) -> ClassLayout:
        """Extract class layout from decompiled code"""
        fields = []
        total_size = 0
        
        # Look for OOAnalyzer structure definitions
        pattern = r'(\d+)\s+(\w+)\s+(\d+)\s+(\w+)\s+"([^"]*)"'
        matches = re.findall(pattern, content)
        
        for match in matches:
            offset = int(match[0])
            type_name = match[1]
            size = int(match[2])
            field_name = match[3]
            comment = match[4]
            
            fields.append((field_name, offset, size))
            total_size = max(total_size, offset + size)
        
        return ClassLayout(fields=fields, total_size=total_size, alignment=4)
    
    def estimate_field_size(self, type_name: str) -> int:
        """Estimate the size of a field based on its type"""
        type_sizes = {
            'char': 1, 'byte': 1, 'bool': 1,
            'short': 2, 'int16_t': 2, 'uint16_t': 2,
            'int': 4, 'int32_t': 4, 'uint32_t': 4, 'long': 4, 'dword': 4,
            'float': 4, 'double': 8,
            'int64_t': 8, 'uint64_t': 8, 'long long': 8,
            'void*': 4, 'char*': 4, 'int*': 4, 'float*': 4,
        }
        
        # Handle pointer types
        if '*' in type_name:
            return 4
        
        # Handle array types
        if '[' in type_name:
            base_type = type_name.split('[')[0].strip()
            return type_sizes.get(base_type, 4)
        
        return type_sizes.get(type_name, 4)
    
    def calculate_layout_similarity(self, src_layout: ClassLayout, decompiled_layout: ClassLayout) -> float:
        """Calculate similarity between two class layouts"""
        if not src_layout.fields or not decompiled_layout.fields:
            return 0.0
        
        # Compare field sizes and positions
        src_field_sizes = [field[2] for field in src_layout.fields]
        decompiled_field_sizes = [field[2] for field in decompiled_layout.fields]
        
        # Use sequence matcher for field size similarity
        size_similarity = difflib.SequenceMatcher(None, src_field_sizes, decompiled_field_sizes).ratio()
        
        # Compare total sizes
        size_ratio = min(src_layout.total_size, decompiled_layout.total_size) / max(src_layout.total_size, decompiled_layout.total_size)
        
        # Weighted combination
        return (size_similarity * 0.7) + (size_ratio * 0.3)
    
    def extract_string_constants_from_src(self, content: str) -> List[StringConstant]:
        """Extract string constants from source code"""
        strings = []
        
        # Look for string literals
        patterns = [
            r'"([^"]*)"',  # Double quoted strings
            r"'([^']*)'",  # Single quoted strings
        ]
        
        lines = content.split('\n')
        for line_num, line in enumerate(lines, 1):
            for pattern in patterns:
                matches = re.findall(pattern, line)
                for match in matches:
                    if len(match) > 3:  # Only significant strings
                        strings.append(StringConstant(
                            name=f"string_{line_num}",
                            value=match,
                            location=f"line_{line_num}"
                        ))
        
        return strings
    
    def extract_string_constants_from_decompiled(self, content: str) -> List[StringConstant]:
        """Extract string constants from decompiled code"""
        strings = []
        
        # Look for .DATA segment string definitions
        pattern = r's_([^_]+)_([^_]+)_([^_]+)\s+db\s+"([^"]*)"'
        matches = re.findall(pattern, content)
        
        for match in matches:
            name = f"s_{match[0]}_{match[1]}_{match[2]}"
            value = match[3]
            strings.append(StringConstant(
                name=name,
                value=value,
                location="DATA_segment"
            ))
        
        # Also look for string literals in function calls
        pattern2 = r'FUN_([0-9a-fA-F]+)\s*\(\s*"([^"]*)"'
        matches2 = re.findall(pattern2, content)
        
        for match in matches2:
            name = f"FUN_{match[0]}"
            value = match[1]
            strings.append(StringConstant(
                name=name,
                value=value,
                location="function_call"
            ))
        
        return strings
    
    def calculate_string_similarity(self, src_strings: List[StringConstant], decompiled_strings: List[StringConstant]) -> float:
        """Calculate similarity based on string constants"""
        if not src_strings or not decompiled_strings:
            return 0.0
        
        src_values = set(s.value for s in src_strings)
        decompiled_values = set(s.value for s in decompiled_strings)
        
        # Calculate Jaccard similarity
        intersection = len(src_values & decompiled_values)
        union = len(src_values | decompiled_values)
        
        return intersection / union if union > 0 else 0.0
    
    def extract_functional_signatures(self, content: str) -> Dict[str, int]:
        """Extract functional signatures (method patterns, control flow, etc.)"""
        signatures = {}
        
        # Count different types of operations
        patterns = {
            'function_calls': r'FUN_[0-9a-fA-F]+',
            'memory_access': r'\[[^\]]*\]',
            'pointer_deref': r'\*[a-zA-Z_][a-zA-Z0-9_]*',
            'conditional_jumps': r'if\s*\(',
            'loops': r'(for|while)\s*\(',
            'switch_statements': r'switch\s*\(',
            'virtual_calls': r'virt_meth_[0-9a-fA-F]+',
        }
        
        for name, pattern in patterns.items():
            matches = re.findall(pattern, content, re.MULTILINE)
            signatures[name] = len(matches)
        
        return signatures
    
    def calculate_functional_similarity(self, src_signatures: Dict[str, int], decompiled_signatures: Dict[str, int]) -> float:
        """Calculate functional similarity between two files"""
        if not src_signatures or not decompiled_signatures:
            return 0.0
        
        # Normalize signatures
        src_total = sum(src_signatures.values()) or 1
        decompiled_total = sum(decompiled_signatures.values()) or 1
        
        src_normalized = {k: v/src_total for k, v in src_signatures.items()}
        decompiled_normalized = {k: v/decompiled_total for k, v in decompiled_signatures.items()}
        
        # Calculate cosine similarity
        numerator = sum(src_normalized.get(k, 0) * decompiled_normalized.get(k, 0) for k in set(src_normalized) | set(decompiled_normalized))
        
        src_magnitude = sum(v*v for v in src_normalized.values()) ** 0.5
        decompiled_magnitude = sum(v*v for v in decompiled_normalized.values()) ** 0.5
        
        denominator = src_magnitude * decompiled_magnitude
        
        return numerator / denominator if denominator > 0 else 0.0
    
    def analyze_src_files(self):
        """Analyze all source files"""
        print("Analyzing source files...")
        
        for file_path in self.src_dir.glob("*.cpp"):
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                class_name = file_path.stem
                
                # Extract class information
                layout = self.extract_class_layout_from_src(content)
                strings = self.extract_string_constants_from_src(content)
                signatures = self.extract_functional_signatures(content)
                
                self.src_classes[class_name] = {
                    'file_path': str(file_path),
                    'content': content,
                    'layout': layout,
                    'strings': strings,
                    'signatures': signatures,
                    'size': len(content)
                }
                
            except Exception as e:
                print(f"Error analyzing {file_path}: {e}")
        
        print(f"Analyzed {len(self.src_classes)} source files")
    
    def analyze_decompiled_files(self):
        """Analyze all decompiled files"""
        print("Analyzing decompiled files...")
        
        for file_path in self.classes_dir.glob("cls_*.cpp"):
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                class_name = file_path.stem
                if class_name.startswith('cls_'):
                    class_name = class_name[4:]  # Remove 'cls_' prefix
                
                # Extract class information
                layout = self.extract_class_layout_from_decompiled(content)
                strings = self.extract_string_constants_from_decompiled(content)
                signatures = self.extract_functional_signatures(content)
                
                self.decompiled_classes[class_name] = {
                    'file_path': str(file_path),
                    'content': content,
                    'layout': layout,
                    'strings': strings,
                    'signatures': signatures,
                    'size': len(content)
                }
                
            except Exception as e:
                print(f"Error analyzing {file_path}: {e}")
        
        print(f"Analyzed {len(self.decompiled_classes)} decompiled files")
    
    def find_matches(self, layout_threshold: float = 0.7, string_threshold: float = 0.3, functional_threshold: float = 0.4) -> List[ClassMatch]:
        """Find matches using the three-tier approach"""
        matches = []
        
        for src_name, src_data in self.src_classes.items():
            best_match = None
            best_score = 0.0
            best_match_data = None
            
            for decompiled_name, decompiled_data in self.decompiled_classes.items():
                # Tier 1: Layout similarity
                layout_sim = self.calculate_layout_similarity(src_data['layout'], decompiled_data['layout'])
                
                # Tier 2: String similarity
                string_sim = self.calculate_string_similarity(src_data['strings'], decompiled_data['strings'])
                
                # Tier 3: Functional similarity
                functional_sim = self.calculate_functional_similarity(src_data['signatures'], decompiled_data['signatures'])
                
                # Combined score with weights
                combined_score = (layout_sim * 0.5) + (string_sim * 0.3) + (functional_sim * 0.2)
                
                # Determine match type
                match_type = "functional"
                if layout_sim > layout_threshold:
                    match_type = "layout"
                elif string_sim > string_threshold:
                    match_type = "strings"
                
                if combined_score > best_score:
                    best_score = combined_score
                    best_match = decompiled_name
                    best_match_data = {
                        'layout_sim': layout_sim,
                        'string_sim': string_sim,
                        'functional_sim': functional_sim,
                        'match_type': match_type
                    }
            
            if best_match and best_score > 0.6:  # Overall threshold
                # Find field mappings
                field_mappings = self.find_field_mappings(src_data['layout'], self.decompiled_classes[best_match]['layout'])
                
                # Find string mappings
                string_mappings = self.find_string_mappings(src_data['strings'], self.decompiled_classes[best_match]['strings'])
                
                match = ClassMatch(
                    src_class=src_name,
                    decompiled_class=best_match,
                    confidence=best_score,
                    match_type=best_match_data['match_type'],
                    layout_similarity=best_match_data['layout_sim'],
                    string_similarity=best_match_data['string_sim'],
                    functional_similarity=best_match_data['functional_sim'],
                    field_mappings=field_mappings,
                    string_mappings=string_mappings
                )
                
                matches.append(match)
        
        # Sort by confidence
        matches.sort(key=lambda x: x.confidence, reverse=True)
        return matches
    
    def find_field_mappings(self, src_layout: ClassLayout, decompiled_layout: ClassLayout) -> List[Tuple[str, str]]:
        """Find mappings between fields based on position and size"""
        mappings = []
        
        for src_field in src_layout.fields:
            src_name, src_offset, src_size = src_field
            
            for decompiled_field in decompiled_layout.fields:
                decompiled_name, decompiled_offset, decompiled_size = decompiled_field
                
                # Match based on offset and size
                if src_offset == decompiled_offset and src_size == decompiled_size:
                    mappings.append((src_name, decompiled_name))
                    break
        
        return mappings
    
    def find_string_mappings(self, src_strings: List[StringConstant], decompiled_strings: List[StringConstant]) -> List[Tuple[str, str]]:
        """Find mappings between string constants"""
        mappings = []
        
        for src_string in src_strings:
            for decompiled_string in decompiled_strings:
                if src_string.value == decompiled_string.value:
                    mappings.append((src_string.name, decompiled_string.name))
                    break
        
        return mappings
    
    def generate_report(self, matches: List[ClassMatch], output_file: str = "revenant_matches.json"):
        """Generate a detailed report"""
        report = {
            "summary": {
                "total_src_classes": len(self.src_classes),
                "total_decompiled_classes": len(self.decompiled_classes),
                "total_matches": len(matches),
                "layout_matches": len([m for m in matches if m.match_type == "layout"]),
                "string_matches": len([m for m in matches if m.match_type == "strings"]),
                "functional_matches": len([m for m in matches if m.match_type == "functional"]),
                "high_confidence": len([m for m in matches if m.confidence > 0.8]),
                "medium_confidence": len([m for m in matches if 0.6 <= m.confidence <= 0.8]),
                "low_confidence": len([m for m in matches if m.confidence < 0.6])
            },
            "matches": []
        }
        
        for match in matches:
            match_data = {
                "src_class": match.src_class,
                "decompiled_class": match.decompiled_class,
                "confidence": match.confidence,
                "match_type": match.match_type,
                "layout_similarity": match.layout_similarity,
                "string_similarity": match.string_similarity,
                "functional_similarity": match.functional_similarity,
                "field_mappings": match.field_mappings,
                "string_mappings": match.string_mappings
            }
            report["matches"].append(match_data)
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        # Print summary
        print(f"\n=== REVENANT CLASS MATCHING REPORT ===")
        print(f"Total src classes: {report['summary']['total_src_classes']}")
        print(f"Total decompiled classes: {report['summary']['total_decompiled_classes']}")
        print(f"Total matches found: {report['summary']['total_matches']}")
        print(f"Layout matches: {report['summary']['layout_matches']}")
        print(f"String matches: {report['summary']['string_matches']}")
        print(f"Functional matches: {report['summary']['functional_matches']}")
        print(f"High confidence: {report['summary']['high_confidence']}")
        print(f"Medium confidence: {report['summary']['medium_confidence']}")
        print(f"Low confidence: {report['summary']['low_confidence']}")
        
        print(f"\n=== TOP MATCHES ===")
        for i, match in enumerate(matches[:10]):
            print(f"{i+1}. {match.src_class} -> {match.decompiled_class}")
            print(f"   Confidence: {match.confidence:.3f}, Type: {match.match_type}")
            print(f"   Layout: {match.layout_similarity:.3f}, Strings: {match.string_similarity:.3f}, Func: {match.functional_similarity:.3f}")
        
        print(f"\nReport saved to {output_file}")
    
    def run_analysis(self):
        """Run the complete analysis"""
        self.analyze_src_files()
        self.analyze_decompiled_files()
        matches = self.find_matches()
        self.generate_report(matches)

if __name__ == "__main__":
    matcher = RevenantClassMatcher()
    matcher.run_analysis() 