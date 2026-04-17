#!/usr/bin/env python3
"""
Class Refactorer for Revenant Revisited
Helps refactor decompiled classes by renaming methods and restoring class definitions
"""

import os
import re
import json
import shutil
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Set
from dataclasses import dataclass
import difflib

@dataclass
class MethodMapping:
    """Represents a method name mapping"""
    decompiled_name: str
    src_name: str
    confidence: float
    method_type: str  # 'virtual', 'static', 'regular'

@dataclass
class RefactoringPlan:
    """Represents a refactoring plan for a class"""
    class_name: str
    decompiled_file: str
    src_file: str
    method_mappings: List[MethodMapping]
    class_rename: Optional[str] = None
    confidence: float = 0.0

class ClassRefactorer:
    def __init__(self, src_dir: str = "src", classes_dir: str = "Classes", backup_dir: str = "Classes_Original"):
        self.src_dir = Path(src_dir)
        self.classes_dir = Path(classes_dir)
        self.backup_dir = Path(backup_dir)
        self.method_patterns = {
            'virtual': r'virtual\s+(?:\w+\s+)?(\w+)\s*\([^)]*\)',
            'static': r'static\s+(?:\w+\s+)?(\w+)\s*\([^)]*\)',
            'regular': r'(?<!virtual\s+)(?<!static\s+)(\w+)\s+(\w+)\s*\([^)]*\)\s*[;{]',
            'decompiled': r'(\w+)\s*\([^)]*\)\s*[;{]',  # Decompiled method pattern
        }
        
    def load_matches(self, matches_file: str = "class_matches.json") -> Dict:
        """Load class matches from JSON file"""
        if not os.path.exists(matches_file):
            print(f"Warning: {matches_file} not found. Run class_analyzer.py first.")
            return {}
        
        with open(matches_file, 'r') as f:
            return json.load(f)
    
    def extract_methods_from_file(self, file_path: Path, method_type: str = "all") -> List[str]:
        """Extract method names from a C++ file"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return []
        
        methods = []
        
        if method_type == "all" or method_type == "decompiled":
            # Extract decompiled method names (like meth_0x123456)
            pattern = r'meth_0x[0-9a-fA-F]+'
            matches = re.findall(pattern, content)
            methods.extend(matches)
        
        if method_type == "all" or method_type in ["virtual", "static", "regular"]:
            pattern = self.method_patterns.get(method_type, r'(\w+)\s*\([^)]*\)')
            matches = re.findall(pattern, content, re.MULTILINE)
            
            for match in matches:
                if isinstance(match, tuple):
                    method_name = match[-1] if match else ""
                else:
                    method_name = match
                
                if method_name and len(method_name) > 2 and not method_name.startswith('_'):
                    methods.append(method_name)
        
        return list(set(methods))  # Remove duplicates
    
    def find_method_matches(self, src_methods: List[str], decompiled_methods: List[str]) -> List[MethodMapping]:
        """Find matches between src and decompiled methods"""
        mappings = []
        
        for src_method in src_methods:
            best_match = None
            best_score = 0.0
            
            for decompiled_method in decompiled_methods:
                # Use sequence matcher for fuzzy matching
                similarity = difflib.SequenceMatcher(None, src_method.lower(), decompiled_method.lower()).ratio()
                
                if similarity > best_score and similarity > 0.6:  # Minimum threshold
                    best_score = similarity
                    best_match = decompiled_method
            
            if best_match:
                # Determine method type based on patterns
                method_type = "regular"
                if re.search(r'virtual\s+' + re.escape(src_method), "virtual " + src_method):
                    method_type = "virtual"
                elif re.search(r'static\s+' + re.escape(src_method), "static " + src_method):
                    method_type = "static"
                
                mappings.append(MethodMapping(
                    decompiled_name=best_match,
                    src_name=src_method,
                    confidence=best_score,
                    method_type=method_type
                ))
        
        return mappings
    
    def create_refactoring_plans(self, matches_data: Dict) -> List[RefactoringPlan]:
        """Create refactoring plans based on class matches"""
        plans = []
        
        for match in matches_data.get("matches", []):
            if match["confidence"] == "LOW":
                continue  # Skip low confidence matches
            
            src_file = match["src_class"]["file"]
            decompiled_file = match["decompiled_class"]["file"]
            
            # Extract methods from both files
            src_methods = self.extract_methods_from_file(Path(src_file), "all")
            decompiled_methods = self.extract_methods_from_file(Path(decompiled_file), "decompiled")
            
            # Find method matches
            method_mappings = self.find_method_matches(src_methods, decompiled_methods)
            
            # Determine class rename
            class_rename = None
            src_class_name = match["src_class"]["name"]
            decompiled_class_name = match["decompiled_class"]["name"]
            
            if src_class_name != decompiled_class_name:
                class_rename = src_class_name
            
            plan = RefactoringPlan(
                class_name=decompiled_class_name,
                decompiled_file=decompiled_file,
                src_file=src_file,
                method_mappings=method_mappings,
                class_rename=class_rename,
                confidence=match["similarity_score"]
            )
            
            plans.append(plan)
        
        return plans
    
    def backup_file(self, file_path: Path) -> Path:
        """Create a backup of a file"""
        backup_path = self.backup_dir / file_path.name
        if not backup_path.exists():
            shutil.copy2(file_path, backup_path)
        return backup_path
    
    def refactor_class_file(self, plan: RefactoringPlan, dry_run: bool = True) -> bool:
        """Refactor a single class file based on the plan"""
        file_path = Path(plan.decompiled_file)
        
        if not file_path.exists():
            print(f"Warning: File {file_path} not found")
            return False
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return False
        
        original_content = content
        
        # Apply method renames
        for mapping in plan.method_mappings:
            if mapping.confidence > 0.7:  # Only apply high confidence mappings
                # Replace method names in various contexts
                patterns = [
                    rf'\b{mapping.decompiled_name}\b',  # Word boundary
                    rf'{mapping.decompiled_name}\s*\(',  # Method call
                    rf'::\s*{mapping.decompiled_name}\s*\(',  # Class method
                ]
                
                for pattern in patterns:
                    content = re.sub(pattern, mapping.src_name, content)
        
        # Apply class rename if specified
        if plan.class_rename:
            # Replace class name in various contexts
            old_class_name = plan.class_name
            new_class_name = plan.class_rename
            
            patterns = [
                rf'\bclass\s+{old_class_name}\b',
                rf'\b{old_class_name}\s*\*',
                rf'\b{old_class_name}\s*&',
                rf'\b{old_class_name}\s*\(',
                rf'::\s*{old_class_name}\b',
            ]
            
            for pattern in patterns:
                content = re.sub(pattern, new_class_name, content)
        
        # Show changes if dry run
        if dry_run:
            print(f"\n=== REFACTORING PLAN FOR {file_path.name} ===")
            print(f"Confidence: {plan.confidence:.3f}")
            print(f"Method mappings: {len(plan.method_mappings)}")
            
            if plan.method_mappings:
                print("Method renames:")
                for mapping in plan.method_mappings[:5]:  # Show first 5
                    print(f"  {mapping.decompiled_name} -> {mapping.src_name} (confidence: {mapping.confidence:.3f})")
            
            if plan.class_rename:
                print(f"Class rename: {plan.class_name} -> {plan.class_rename}")
            
            # Show diff
            diff = list(difflib.unified_diff(
                original_content.splitlines(keepends=True),
                content.splitlines(keepends=True),
                fromfile=f"{file_path.name} (original)",
                tofile=f"{file_path.name} (refactored)",
                lineterm=""
            ))
            
            if diff:
                print("\nChanges:")
                for line in diff[:20]:  # Show first 20 lines of diff
                    print(line.rstrip())
                if len(diff) > 20:
                    print(f"... and {len(diff) - 20} more lines")
            
            return True
        else:
            # Actually apply changes
            try:
                # Create backup
                self.backup_file(file_path)
                
                # Write refactored content
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                
                print(f"Refactored {file_path.name}")
                return True
                
            except Exception as e:
                print(f"Error refactoring {file_path}: {e}")
                return False
    
    def generate_refactoring_report(self, plans: List[RefactoringPlan], output_file: str = "refactoring_report.json"):
        """Generate a report of all refactoring plans"""
        report = {
            "summary": {
                "total_plans": len(plans),
                "high_confidence_plans": len([p for p in plans if p.confidence > 0.7]),
                "medium_confidence_plans": len([p for p in plans if 0.5 <= p.confidence <= 0.7]),
                "total_method_mappings": sum(len(p.method_mappings) for p in plans),
                "classes_to_rename": len([p for p in plans if p.class_rename])
            },
            "plans": []
        }
        
        for plan in plans:
            plan_data = {
                "class_name": plan.class_name,
                "decompiled_file": plan.decompiled_file,
                "src_file": plan.src_file,
                "confidence": plan.confidence,
                "class_rename": plan.class_rename,
                "method_mappings": [
                    {
                        "decompiled_name": m.decompiled_name,
                        "src_name": m.src_name,
                        "confidence": m.confidence,
                        "method_type": m.method_type
                    }
                    for m in plan.method_mappings
                ]
            }
            report["plans"].append(plan_data)
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\n=== REFACTORING REPORT ===")
        print(f"Total plans: {report['summary']['total_plans']}")
        print(f"High confidence: {report['summary']['high_confidence_plans']}")
        print(f"Medium confidence: {report['summary']['medium_confidence_plans']}")
        print(f"Total method mappings: {report['summary']['total_method_mappings']}")
        print(f"Classes to rename: {report['summary']['classes_to_rename']}")
        print(f"Report saved to {output_file}")
    
    def run_refactoring(self, dry_run: bool = True, matches_file: str = "class_matches.json"):
        """Run the complete refactoring process"""
        print("Loading class matches...")
        matches_data = self.load_matches(matches_file)
        
        if not matches_data:
            print("No matches found. Run class_analyzer.py first.")
            return
        
        print("Creating refactoring plans...")
        plans = self.create_refactoring_plans(matches_data)
        
        if not plans:
            print("No refactoring plans created.")
            return
        
        print(f"Created {len(plans)} refactoring plans")
        
        # Generate report
        self.generate_refactoring_report(plans)
        
        # Apply refactoring
        print(f"\nApplying refactoring (dry_run={dry_run})...")
        success_count = 0
        
        for plan in plans:
            if self.refactor_class_file(plan, dry_run):
                success_count += 1
        
        print(f"\nRefactoring complete: {success_count}/{len(plans)} files processed")
        
        if dry_run:
            print("\nThis was a dry run. To apply changes, run with dry_run=False")

if __name__ == "__main__":
    refactorer = ClassRefactorer()
    refactorer.run_refactoring(dry_run=True)  # Start with dry run 