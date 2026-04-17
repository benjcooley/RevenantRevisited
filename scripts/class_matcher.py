#!/usr/bin/env python3

"""
CLASS MATCHER - Stage 2/3 of Revenant Class Identification Pipeline

Takes fingerprints from Stage 1 and identifies matching classes between Ghidra and Source.
"""

import re
from pathlib import Path
from typing import List, Tuple, Dict, Optional
from dataclasses import dataclass

@dataclass
class ClassFingerprint:
    name: str
    pattern: str
    size: int
    fields: int
    pattern_length: int

@dataclass
class ClassMatch:
    ghidra_class: str
    source_class: str
    confidence: float
    pattern_similarity: float
    size_similarity: float
    field_similarity: float
    notes: str

class ClassMatcher:
    def __init__(self):
        self.ghidra_fingerprints: List[ClassFingerprint] = []
        self.source_fingerprints: List[ClassFingerprint] = []
        self.matches: List[ClassMatch] = []
        
    def load_fingerprints(self):
        """Load fingerprints from Stage 1 output files"""
        print("📥 Loading fingerprints from Stage 1...")
        
        # Load Ghidra fingerprints
        ghidra_file = Path("../ghidra_fingerprints.txt")
        if ghidra_file.exists():
            self.ghidra_fingerprints = self._parse_fingerprint_file(ghidra_file, "ghidra")
            print(f"  ✅ Loaded {len(self.ghidra_fingerprints)} Ghidra fingerprints")
        else:
            print(f"  ❌ {ghidra_file} not found - run class_fingerprinter.py first")
            return False
            
        # Load Source fingerprints  
        source_file = Path("../source_fingerprints.txt")
        if source_file.exists():
            self.source_fingerprints = self._parse_fingerprint_file(source_file, "source")
            print(f"  ✅ Loaded {len(self.source_fingerprints)} Source fingerprints")
        else:
            print(f"  ❌ {source_file} not found - run class_fingerprinter.py first")
            return False
            
        return True
        
    def _parse_fingerprint_file(self, file_path: Path, file_type: str) -> List[ClassFingerprint]:
        """Parse fingerprint file and return list of fingerprints"""
        fingerprints = []
        
        with open(file_path, 'r') as f:
            lines = f.readlines()
            
        for line in lines:
            line = line.strip()
            if not line or '|' not in line:
                continue
            if any(word in line for word in ['Total:', 'Features:', 'Signatures:', 'Sorted', '=']):
                continue
                
            # Parse line: "class_name | size | fields | pattern_len | pattern"
            parts = line.split('|')
            if len(parts) >= 5:
                name = parts[0].strip()
                size_str = parts[1].strip()
                fields_str = parts[2].strip()
                pattern_len_str = parts[3].strip()
                pattern = parts[4].strip()
                
                # Extract numeric values
                size_match = re.search(r'(\d+)b', size_str)
                fields_match = re.search(r'(\d+)f', fields_str)
                pattern_len_match = re.search(r'(\d+)p', pattern_len_str)
                
                if size_match and fields_match and pattern_len_match:
                    # Remove "..." if present for actual pattern
                    clean_pattern = pattern.replace('...', '')
                    
                    fingerprints.append(ClassFingerprint(
                        name=name,
                        pattern=clean_pattern,
                        size=int(size_match.group(1)),
                        fields=int(fields_match.group(1)),
                        pattern_length=int(pattern_len_match.group(1))
                    ))
                    
        return fingerprints
    
    def calculate_pattern_similarity(self, pattern1: str, pattern2: str) -> float:
        """Calculate similarity between two patterns using LCS"""
        if not pattern1 or not pattern2:
            return 0.0
            
        # Longest Common Subsequence
        m, n = len(pattern1), len(pattern2)
        dp = [[0] * (n + 1) for _ in range(m + 1)]
        
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                if pattern1[i-1] == pattern2[j-1]:
                    dp[i][j] = dp[i-1][j-1] + 1
                else:
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1])
        
        lcs_length = dp[m][n]
        max_length = max(m, n)
        return lcs_length / max_length if max_length > 0 else 0.0
    
    def calculate_size_similarity(self, size1: int, size2: int) -> float:
        """Calculate size similarity (closer sizes = higher similarity)"""
        if size1 == 0 or size2 == 0:
            return 0.0
            
        larger = max(size1, size2)
        smaller = min(size1, size2)
        
        # Perfect match = 1.0, half size = 0.5, etc.
        return smaller / larger
    
    def calculate_field_similarity(self, fields1: int, fields2: int) -> float:
        """Calculate field count similarity"""
        if fields1 == 0 or fields2 == 0:
            return 0.0
            
        larger = max(fields1, fields2)
        smaller = min(fields1, fields2)
        
        return smaller / larger
    
    def find_matches(self):
        """Find matches between Ghidra and Source classes"""
        print(f"🔍 Finding matches between {len(self.ghidra_fingerprints)} Ghidra and {len(self.source_fingerprints)} Source classes...")
        
        matches = []
        
        for ghidra_fp in self.ghidra_fingerprints:
            best_match = None
            best_confidence = 0.0
            
            for source_fp in self.source_fingerprints:
                # Calculate similarities
                pattern_sim = self.calculate_pattern_similarity(ghidra_fp.pattern, source_fp.pattern)
                size_sim = self.calculate_size_similarity(ghidra_fp.size, source_fp.size)
                field_sim = self.calculate_field_similarity(ghidra_fp.fields, source_fp.fields)
                
                # Weighted confidence score
                confidence = (
                    pattern_sim * 0.6 +    # Pattern is most important
                    size_sim * 0.3 +       # Size is secondary  
                    field_sim * 0.1        # Field count is least important
                )
                
                if confidence > best_confidence and confidence >= 0.4:  # Minimum threshold
                    best_confidence = confidence
                    best_match = ClassMatch(
                        ghidra_class=ghidra_fp.name,
                        source_class=source_fp.name,
                        confidence=confidence,
                        pattern_similarity=pattern_sim,
                        size_similarity=size_sim,
                        field_similarity=field_sim,
                        notes=f"Pattern: {pattern_sim:.2f}, Size: {size_sim:.2f}, Fields: {field_sim:.2f}"
                    )
            
            if best_match:
                matches.append(best_match)
        
        # Sort by confidence (highest first)
        matches.sort(key=lambda m: m.confidence, reverse=True)
        self.matches = matches
        
        print(f"✅ Found {len(matches)} potential matches")
        return matches
    
    def save_matches(self):
        """Save matches to file for Stage 3"""
        matches_file = Path("../class_matches.txt")
        
        with open(matches_file, 'w') as f:
            f.write("REVENANT CLASS MATCHES - STAGE 2/3 OUTPUT\n")
            f.write("=" * 50 + "\n\n")
            f.write(f"Total matches found: {len(self.matches)}\n")
            f.write("Confidence threshold: 0.40 (40%)\n")
            f.write("Sorting: By confidence (highest first)\n\n")
            f.write(f"{'Ghidra Class':<25} | {'Source Class':<25} | {'Conf':<5} | {'Notes'}\n")
            f.write("-" * 100 + "\n")
            
            for match in self.matches:
                f.write(f"{match.ghidra_class:<25} | {match.source_class:<25} | {match.confidence:.2f}  | {match.notes}\n")
        
        print(f"💾 Saved matches to {matches_file}")
        
        # Show summary
        high_conf = len([m for m in self.matches if m.confidence >= 0.8])
        med_conf = len([m for m in self.matches if 0.6 <= m.confidence < 0.8])
        low_conf = len([m for m in self.matches if 0.4 <= m.confidence < 0.6])
        
        print(f"\n📊 Match Quality Summary:")
        print(f"  🎯 High confidence (≥80%): {high_conf}")
        print(f"  ⚡ Medium confidence (60-79%): {med_conf}")
        print(f"  ⚠️  Low confidence (40-59%): {low_conf}")
        
        return matches_file
    
    def run_matching_pipeline(self):
        """Run the complete matching pipeline"""
        print("🚀 REVENANT CLASS MATCHING - STAGE 2/3")
        print("=" * 50)
        
        if not self.load_fingerprints():
            return False
            
        matches = self.find_matches()
        matches_file = self.save_matches()
        
        print(f"\n🎯 Pipeline Stage 2/3: Class Matching")
        print(f"  ✅ Loaded fingerprints from Stage 1")
        print(f"  ✅ Found {len(matches)} class matches")
        print(f"  ✅ Saved matches to {matches_file.name}")
        print(f"\n➡️  Next: Run name_applier.py for Stage 3/3")
        
        return True

if __name__ == "__main__":
    matcher = ClassMatcher()
    matcher.run_matching_pipeline()

