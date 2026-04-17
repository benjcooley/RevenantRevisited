#!/usr/bin/env python3
"""
Class Analyzer for Revenant Revisited
Analyzes classes in /src and /Classes folders to find matches and generate signatures
"""

import os
import re
import json
import hashlib
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Set
from dataclasses import dataclass
from collections import defaultdict
import difflib

@dataclass
class ClassSignature:
    """Represents a class signature for matching"""
    name: str
    file_path: str
    size: int
    method_names: Set[str]
    member_vars: Set[str]
    inheritance: List[str]
    virtual_methods: Set[str]
    static_methods: Set[str]
    includes: Set[str]
    hash: str

@dataclass
class MatchResult:
    """Result of class matching"""
    src_class: ClassSignature
    decompiled_class: ClassSignature
    similarity_score: float
    method_matches: List[Tuple[str, str]]
    size_ratio: float
    confidence: str

class ClassAnalyzer:
    def __init__(self, src_dir: str = "src", classes_dir: str = "Classes"):
        self.src_dir = Path(src_dir)
        self.classes_dir = Path(classes_dir)
        self.src_classes: Dict[str, ClassSignature] = {}
        self.decompiled_classes: Dict[str, ClassSignature] = {}
        
    def extract_cpp_methods(self, content: str) -> Set[str]:
        """Extract method names from C++ code"""
        methods = set()
        
        # Match various method patterns
        patterns = [
            r'(\w+)\s+(\w+)\s*\([^)]*\)\s*[;{]',  # Regular methods
            r'virtual\s+(\w+)\s+(\w+)\s*\([^)]*\)',  # Virtual methods
            r'static\s+(\w+)\s+(\w+)\s*\([^)]*\)',  # Static methods
            r'(\w+)\s+(\w+)\s*::\s*(\w+)\s*\([^)]*\)',  # Class methods
            r'bool\s+(\w+)\s*\([^)]*\)',  # Boolean methods
            r'void\s+(\w+)\s*\([^)]*\)',  # Void methods
            r'int\w*\s+(\w+)\s*\([^)]*\)',  # Integer methods
        ]
        
        for pattern in patterns:
            matches = re.findall(pattern, content, re.MULTILINE)
            for match in matches:
                if isinstance(match, tuple):
                    # Extract the method name (usually the last part)
                    method_name = match[-1] if match else ""
                else:
                    method_name = match
                
                if method_name and len(method_name) > 2 and not method_name.startswith('_'):
                    methods.add(method_name)
        
        return methods
    
    def extract_member_variables(self, content: str) -> Set[str]:
        """Extract member variable names from C++ code"""
        members = set()
        
        # Look for member variable patterns in class definitions
        patterns = [
            r'(\w+)\s+(\w+)\s*;',  # Basic member variables
            r'(\w+)\s+(\w+)\s*\[[^\]]*\]\s*;',  # Array members
            r'(\w+)\s*\*\s*(\w+)\s*;',  # Pointer members
            r'(\w+)\s*&\s*(\w+)\s*;',  # Reference members
        ]
        
        for pattern in patterns:
            matches = re.findall(pattern, content, re.MULTILINE)
            for match in matches:
                if isinstance(match, tuple):
                    var_name = match[1] if len(match) > 1 else match[0]
                else:
                    var_name = match
                
                if var_name and len(var_name) > 1 and not var_name.startswith('_'):
                    members.add(var_name)
        
        return members
    
    def extract_inheritance(self, content: str) -> List[str]:
        """Extract inheritance information from C++ code"""
        inheritance = []
        
        # Look for class inheritance patterns
        patterns = [
            r'class\s+(\w+)\s*:\s*(?:public|private|protected)\s+(\w+)',
            r'class\s+(\w+)\s*:\s+(\w+)',
        ]
        
        for pattern in patterns:
            matches = re.findall(pattern, content, re.MULTILINE)
            for match in matches:
                if isinstance(match, tuple):
                    base_class = match[1]
                else:
                    base_class = match
                
                if base_class and base_class not in inheritance:
                    inheritance.append(base_class)
        
        return inheritance
    
    def extract_includes(self, content: str) -> Set[str]:
        """Extract include statements from C++ code"""
        includes = set()
        
        # Match #include patterns
        pattern = r'#include\s*[<"]([^>"]+)[>"]'
        matches = re.findall(pattern, content)
        
        for match in matches:
            includes.add(match)
        
        return includes
    
    def extract_virtual_methods(self, content: str) -> Set[str]:
        """Extract virtual method names"""
        virtual_methods = set()
        
        pattern = r'virtual\s+(?:\w+\s+)?(\w+)\s*\([^)]*\)'
        matches = re.findall(pattern, content, re.MULTILINE)
        
        for match in matches:
            if match and len(match) > 2:
                virtual_methods.add(match)
        
        return virtual_methods
    
    def extract_static_methods(self, content: str) -> Set[str]:
        """Extract static method names"""
        static_methods = set()
        
        pattern = r'static\s+(?:\w+\s+)?(\w+)\s*\([^)]*\)'
        matches = re.findall(pattern, content, re.MULTILINE)
        
        for match in matches:
            if match and len(match) > 2:
                static_methods.add(match)
        
        return static_methods
    
    def generate_signature(self, file_path: Path, content: str) -> ClassSignature:
        """Generate a class signature from file content"""
        file_size = len(content)
        
        # Extract class name from filename
        class_name = file_path.stem
        if class_name.startswith('cls_'):
            class_name = class_name[4:]  # Remove 'cls_' prefix
        
        # Extract various features
        method_names = self.extract_cpp_methods(content)
        member_vars = self.extract_member_variables(content)
        inheritance = self.extract_inheritance(content)
        virtual_methods = self.extract_virtual_methods(content)
        static_methods = self.extract_static_methods(content)
        includes = self.extract_includes(content)
        
        # Generate hash for content
        content_hash = hashlib.md5(content.encode()).hexdigest()
        
        return ClassSignature(
            name=class_name,
            file_path=str(file_path),
            size=file_size,
            method_names=method_names,
            member_vars=member_vars,
            inheritance=inheritance,
            virtual_methods=virtual_methods,
            static_methods=static_methods,
            includes=includes,
            hash=content_hash
        )
    
    def analyze_src_files(self):
        """Analyze all C++ files in the src directory"""
        print("Analyzing src files...")
        
        for file_path in self.src_dir.glob("*.cpp"):
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                signature = self.generate_signature(file_path, content)
                self.src_classes[signature.name] = signature
                
            except Exception as e:
                print(f"Error analyzing {file_path}: {e}")
        
        print(f"Analyzed {len(self.src_classes)} src files")
    
    def analyze_decompiled_files(self):
        """Analyze all decompiled class files"""
        print("Analyzing decompiled files...")
        
        for file_path in self.classes_dir.glob("cls_*.cpp"):
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                signature = self.generate_signature(file_path, content)
                self.decompiled_classes[signature.name] = signature
                
            except Exception as e:
                print(f"Error analyzing {file_path}: {e}")
        
        print(f"Analyzed {len(self.decompiled_classes)} decompiled files")
    
    def calculate_similarity(self, src_sig: ClassSignature, decompiled_sig: ClassSignature) -> float:
        """Calculate similarity score between two class signatures"""
        score = 0.0
        total_weight = 0.0
        
        # Method name similarity (weight: 0.4)
        if src_sig.method_names and decompiled_sig.method_names:
            method_similarity = len(src_sig.method_names & decompiled_sig.method_names) / len(src_sig.method_names | decompiled_sig.method_names)
            score += method_similarity * 0.4
            total_weight += 0.4
        
        # Size similarity (weight: 0.2)
        if src_sig.size > 0 and decompiled_sig.size > 0:
            size_ratio = min(src_sig.size, decompiled_sig.size) / max(src_sig.size, decompiled_sig.size)
            score += size_ratio * 0.2
            total_weight += 0.2
        
        # Virtual method similarity (weight: 0.2)
        if src_sig.virtual_methods and decompiled_sig.virtual_methods:
            virtual_similarity = len(src_sig.virtual_methods & decompiled_sig.virtual_methods) / len(src_sig.virtual_methods | decompiled_sig.virtual_methods)
            score += virtual_similarity * 0.2
            total_weight += 0.2
        
        # Include similarity (weight: 0.1)
        if src_sig.includes and decompiled_sig.includes:
            include_similarity = len(src_sig.includes & decompiled_sig.includes) / len(src_sig.includes | decompiled_sig.includes)
            score += include_similarity * 0.1
            total_weight += 0.1
        
        # Inheritance similarity (weight: 0.1)
        if src_sig.inheritance and decompiled_sig.inheritance:
            inheritance_similarity = len(set(src_sig.inheritance) & set(decompiled_sig.inheritance)) / len(set(src_sig.inheritance) | set(decompiled_sig.inheritance))
            score += inheritance_similarity * 0.1
            total_weight += 0.1
        
        return score / total_weight if total_weight > 0 else 0.0
    
    def find_matches(self, threshold: float = 0.3) -> List[MatchResult]:
        """Find matches between src and decompiled classes"""
        matches = []
        
        for src_name, src_sig in self.src_classes.items():
            best_match = None
            best_score = 0.0
            
            for decompiled_name, decompiled_sig in self.decompiled_classes.items():
                similarity = self.calculate_similarity(src_sig, decompiled_sig)
                
                if similarity > best_score and similarity >= threshold:
                    best_score = similarity
                    best_match = decompiled_sig
            
            if best_match:
                # Find method matches
                method_matches = []
                for src_method in src_sig.method_names:
                    for decompiled_method in best_match.method_names:
                        if difflib.SequenceMatcher(None, src_method, decompiled_method).ratio() > 0.8:
                            method_matches.append((src_method, decompiled_method))
                
                # Calculate size ratio
                size_ratio = min(src_sig.size, best_match.size) / max(src_sig.size, best_match.size)
                
                # Determine confidence level
                if best_score > 0.7:
                    confidence = "HIGH"
                elif best_score > 0.5:
                    confidence = "MEDIUM"
                else:
                    confidence = "LOW"
                
                matches.append(MatchResult(
                    src_class=src_sig,
                    decompiled_class=best_match,
                    similarity_score=best_score,
                    method_matches=method_matches,
                    size_ratio=size_ratio,
                    confidence=confidence
                ))
        
        # Sort by similarity score
        matches.sort(key=lambda x: x.similarity_score, reverse=True)
        return matches
    
    def generate_report(self, matches: List[MatchResult], output_file: str = "class_matches.json"):
        """Generate a detailed report of matches"""
        report = {
            "summary": {
                "total_src_classes": len(self.src_classes),
                "total_decompiled_classes": len(self.decompiled_classes),
                "total_matches": len(matches),
                "high_confidence_matches": len([m for m in matches if m.confidence == "HIGH"]),
                "medium_confidence_matches": len([m for m in matches if m.confidence == "MEDIUM"]),
                "low_confidence_matches": len([m for m in matches if m.confidence == "LOW"])
            },
            "matches": []
        }
        
        for match in matches:
            match_data = {
                "src_class": {
                    "name": match.src_class.name,
                    "file": match.src_class.file_path,
                    "size": match.src_class.size,
                    "method_count": len(match.src_class.method_names),
                    "virtual_method_count": len(match.src_class.virtual_methods)
                },
                "decompiled_class": {
                    "name": match.decompiled_class.name,
                    "file": match.decompiled_class.file_path,
                    "size": match.decompiled_class.size,
                    "method_count": len(match.decompiled_class.method_names),
                    "virtual_method_count": len(match.decompiled_class.virtual_methods)
                },
                "similarity_score": match.similarity_score,
                "size_ratio": match.size_ratio,
                "confidence": match.confidence,
                "method_matches": match.method_matches
            }
            report["matches"].append(match_data)
        
        # Save to JSON file
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        # Print summary
        print(f"\n=== CLASS MATCHING REPORT ===")
        print(f"Total src classes: {report['summary']['total_src_classes']}")
        print(f"Total decompiled classes: {report['summary']['total_decompiled_classes']}")
        print(f"Total matches found: {report['summary']['total_matches']}")
        print(f"High confidence: {report['summary']['high_confidence_matches']}")
        print(f"Medium confidence: {report['summary']['medium_confidence_matches']}")
        print(f"Low confidence: {report['summary']['low_confidence_matches']}")
        
        print(f"\n=== TOP MATCHES ===")
        for i, match in enumerate(matches[:10]):
            print(f"{i+1}. {match.src_class.name} -> {match.decompiled_class.name} (Score: {match.similarity_score:.3f}, Confidence: {match.confidence})")
        
        print(f"\nReport saved to {output_file}")
    
    def run_analysis(self, threshold: float = 0.3):
        """Run the complete analysis"""
        self.analyze_src_files()
        self.analyze_decompiled_files()
        matches = self.find_matches(threshold)
        self.generate_report(matches)

if __name__ == "__main__":
    analyzer = ClassAnalyzer()
    analyzer.run_analysis(threshold=0.3) 