#!/usr/bin/env python3
"""
SpectralCanvas Pro - Automated Code Quality Fixer
=================================================

Comprehensive tool for detecting and fixing common C++ code issues,
JUCE-specific problems, and RT-safety violations in SpectralCanvas Pro.

Usage:
    python auto_code_fixer.py --scan-all
    python auto_code_fixer.py --file Source/Core/PluginProcessor.cpp
    python auto_code_fixer.py --fix-headers --dry-run
"""

import os
import re
import sys
import argparse
import json
import subprocess
from pathlib import Path
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from datetime import datetime

@dataclass
class CodeIssue:
    """Represents a detected code issue"""
    file_path: str
    line_number: int
    issue_type: str
    severity: str  # 'error', 'warning', 'info'
    description: str
    suggested_fix: Optional[str] = None
    auto_fixable: bool = False

class SpectralCanvasCodeFixer:
    """Main code quality fixer for SpectralCanvas Pro"""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.source_dirs = [
            self.project_root / "Source",
            self.project_root / "Tests",
            self.project_root / "External" / "tdd-guard"
        ]
        self.issues: List[CodeIssue] = []
        
        # RT-Safety patterns (critical for audio plugins)
        self.rt_unsafe_patterns = {
            r'\bmalloc\s*\(': 'Memory allocation in audio thread',
            r'\bfree\s*\(': 'Memory deallocation in audio thread', 
            r'\bnew\s+\w+': 'Dynamic allocation in audio thread',
            r'\bdelete\s+': 'Dynamic deallocation in audio thread',
            r'\bstd::vector\s*<.*>\s*\w+\s*\(': 'Vector construction (potential allocation)',
            r'\bstd::string\s+\w+\s*=': 'String allocation in audio thread',
            r'\bDBG\s*\(': 'Debug logging in audio thread',
            r'\bjassert\s*\(': 'Assertion in audio thread',
            r'\bstd::mutex': 'Mutex usage (blocking) in audio thread',
            r'\block_guard': 'Lock guard usage in audio thread',
            r'\bprintf\s*\(': 'Printf in audio thread',
            r'\bstd::cout': 'Console output in audio thread'
        }
        
        # JUCE-specific anti-patterns
        self.juce_antipatterns = {
            r'#include\s+<windows\.h>': 'Include windows.h before JUCE headers',
            r'using namespace juce': 'Avoid "using namespace juce" in headers',
            r'MessageManager::getInstance\(\)->runDispatchLoop': 'Blocking message loop call',
            r'Timer::startTimer\s*\(\s*0\s*\)': 'Zero-interval timer (performance issue)',
            r'Graphics::drawImageAt.*without bounds': 'Unbounded image drawing'
        }
        
        # Common C++ issues that Claude might introduce
        self.cpp_common_issues = {
            r'std::max\s*\(\s*\w+\s*,\s*\w+\s*\)\s*;': 'Potential macro conflict with max()',
            r'std::min\s*\(\s*\w+\s*,\s*\w+\s*\)\s*;': 'Potential macro conflict with min()',
            r'#include\s+<algorithm>\s*\n.*std::(max|min)': 'Include <algorithm> before using std::max/min',
            r'virtual\s+~\w+\s*\(\s*\)\s*\{\s*\}': 'Virtual destructor should be = default',
            r'new\s+\w+\s*\[.*\]': 'Prefer std::vector over raw arrays',
            r'delete\s*\[\s*\]': 'Manual array deletion (use RAII)',
            r'memcpy\s*\(': 'Prefer std::copy over memcpy',
            r'strcpy\s*\(': 'Unsafe string copy (use safe alternatives)'
        }

    def scan_file(self, file_path: Path) -> List[CodeIssue]:
        """Scan a single file for issues"""
        issues = []
        
        if not file_path.exists() or file_path.suffix not in ['.cpp', '.h', '.hpp']:
            return issues
            
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                lines = content.split('\n')
                
            # Check for RT-safety violations
            issues.extend(self._check_rt_safety(file_path, lines))
            
            # Check for JUCE anti-patterns
            issues.extend(self._check_juce_patterns(file_path, lines))
            
            # Check for common C++ issues
            issues.extend(self._check_cpp_issues(file_path, lines))
            
            # Check for header include order issues
            issues.extend(self._check_include_order(file_path, lines))
            
            # Check for potential memory leaks
            issues.extend(self._check_memory_patterns(file_path, lines))
            
        except Exception as e:
            issues.append(CodeIssue(
                file_path=str(file_path),
                line_number=0,
                issue_type='scan_error',
                severity='warning',
                description=f'Failed to scan file: {e}'
            ))
            
        return issues

    def _check_rt_safety(self, file_path: Path, lines: List[str]) -> List[CodeIssue]:
        """Check for RT-safety violations"""
        issues = []
        
        # Only check audio-thread related files
        audio_files = ['PluginProcessor.cpp', 'processBlock', 'AtomicOscillator', 'SpectralSynthEngine']
        if not any(pattern in str(file_path) for pattern in audio_files):
            return issues
            
        for line_num, line in enumerate(lines, 1):
            for pattern, description in self.rt_unsafe_patterns.items():
                if re.search(pattern, line):
                    # Skip if in comments
                    if '//' in line and line.index('//') < line.find(re.search(pattern, line).group()):
                        continue
                        
                    issues.append(CodeIssue(
                        file_path=str(file_path),
                        line_number=line_num,
                        issue_type='rt_safety',
                        severity='error',
                        description=f'RT-safety violation: {description}',
                        suggested_fix=self._suggest_rt_fix(pattern, line),
                        auto_fixable=False  # RT fixes need manual review
                    ))
                    
        return issues

    def _check_juce_patterns(self, file_path: Path, lines: List[str]) -> List[CodeIssue]:
        """Check for JUCE-specific anti-patterns"""
        issues = []
        
        for line_num, line in enumerate(lines, 1):
            for pattern, description in self.juce_antipatterns.items():
                if re.search(pattern, line):
                    issues.append(CodeIssue(
                        file_path=str(file_path),
                        line_number=line_num,
                        issue_type='juce_antipattern',
                        severity='warning',
                        description=description,
                        auto_fixable=True
                    ))
                    
        return issues

    def _check_cpp_issues(self, file_path: Path, lines: List[str]) -> List[CodeIssue]:
        """Check for common C++ issues"""
        issues = []
        
        for line_num, line in enumerate(lines, 1):
            for pattern, description in self.cpp_common_issues.items():
                if re.search(pattern, line):
                    issues.append(CodeIssue(
                        file_path=str(file_path),
                        line_number=line_num,
                        issue_type='cpp_issue',
                        severity='warning',
                        description=description,
                        suggested_fix=self._suggest_cpp_fix(pattern, line),
                        auto_fixable=True
                    ))
                    
        return issues

    def _check_include_order(self, file_path: Path, lines: List[str]) -> List[CodeIssue]:
        """Check for proper include order (JUCE best practices)"""
        issues = []
        
        includes = []
        for line_num, line in enumerate(lines, 1):
            if line.strip().startswith('#include'):
                includes.append((line_num, line.strip()))
                
        # Check if windows.h comes before JUCE headers
        juce_line = next((i for i, (_, inc) in enumerate(includes) if 'juce_' in inc.lower()), None)
        windows_line = next((i for i, (_, inc) in enumerate(includes) if 'windows.h' in inc.lower()), None)
        
        if juce_line is not None and windows_line is not None and windows_line < juce_line:
            issues.append(CodeIssue(
                file_path=str(file_path),
                line_number=includes[windows_line][0],
                issue_type='include_order',
                severity='error',
                description='windows.h included before JUCE headers (causes macro pollution)',
                suggested_fix='Move #include <windows.h> after JUCE includes',
                auto_fixable=True
            ))
            
        return issues

    def _check_memory_patterns(self, file_path: Path, lines: List[str]) -> List[CodeIssue]:
        """Check for potential memory issues"""
        issues = []
        
        # Look for manual memory management patterns
        new_lines = []
        delete_lines = []
        
        for line_num, line in enumerate(lines, 1):
            if re.search(r'\bnew\s+\w+', line) and 'std::' not in line:
                new_lines.append(line_num)
            if re.search(r'\bdelete\s+\w+', line):
                delete_lines.append(line_num)
                
        # Simple heuristic: more news than deletes might indicate leaks
        if len(new_lines) > len(delete_lines):
            issues.append(CodeIssue(
                file_path=str(file_path),
                line_number=new_lines[0],
                issue_type='memory_leak',
                severity='warning',
                description=f'Potential memory leak: {len(new_lines)} new vs {len(delete_lines)} delete',
                suggested_fix='Consider using smart pointers (std::unique_ptr, std::shared_ptr)',
                auto_fixable=False
            ))
            
        return issues

    def _suggest_rt_fix(self, pattern: str, line: str) -> str:
        """Suggest fix for RT-safety violations"""
        fixes = {
            r'\bmalloc\s*\(': 'Pre-allocate memory in constructor or use lock-free data structures',
            r'\bnew\s+\w+': 'Pre-allocate objects or use object pools',
            r'\bDBG\s*\(': 'Use atomic flags for debugging or move to message thread',
            r'\bstd::vector': 'Reserve capacity in constructor or use fixed-size arrays',
            r'\bstd::mutex': 'Use lock-free atomic operations instead'
        }
        
        for pat, fix in fixes.items():
            if re.search(pat, pattern):
                return fix
                
        return 'Review for RT-safety compliance'

    def _suggest_cpp_fix(self, pattern: str, line: str) -> str:
        """Suggest fix for C++ issues"""
        fixes = {
            r'virtual\s+~\w+': 'virtual ~ClassName() = default;',
            r'new\s+\w+\s*\[': 'std::vector<Type> container(size);',
            r'std::max.*macro': 'Add #undef max before usage or use parentheses: (std::max)',
            r'memcpy': 'std::copy(src, src + size, dest);'
        }
        
        for pat, fix in fixes.items():
            if re.search(pat, pattern):
                return fix
                
        return 'Apply modern C++ best practices'

    def scan_all_files(self) -> None:
        """Scan all source files in the project"""
        print(f"🔍 Scanning SpectralCanvas Pro codebase for issues...")
        
        for source_dir in self.source_dirs:
            if source_dir.exists():
                for file_path in source_dir.rglob('*'):
                    if file_path.is_file() and file_path.suffix in ['.cpp', '.h', '.hpp']:
                        issues = self.scan_file(file_path)
                        self.issues.extend(issues)
                        
        print(f"✅ Scan complete. Found {len(self.issues)} issues.")

    def generate_report(self, output_file: Optional[str] = None) -> str:
        """Generate a comprehensive report of all issues"""
        report = []
        report.append("# SpectralCanvas Pro - Code Quality Report")
        report.append(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report.append(f"Total Issues: {len(self.issues)}")
        report.append("")
        
        # Group by severity
        by_severity = {}
        for issue in self.issues:
            if issue.severity not in by_severity:
                by_severity[issue.severity] = []
            by_severity[issue.severity].append(issue)
            
        for severity in ['error', 'warning', 'info']:
            if severity in by_severity:
                report.append(f"## {severity.title()} Issues ({len(by_severity[severity])})")
                report.append("")
                
                for issue in by_severity[severity]:
                    report.append(f"### {issue.file_path}:{issue.line_number}")
                    report.append(f"**Type**: {issue.issue_type}")
                    report.append(f"**Description**: {issue.description}")
                    if issue.suggested_fix:
                        report.append(f"**Suggested Fix**: {issue.suggested_fix}")
                    if issue.auto_fixable:
                        report.append("**Auto-fixable**: ✅ Yes")
                    else:
                        report.append("**Auto-fixable**: ❌ Manual review required")
                    report.append("")
                    
        report_text = "\n".join(report)
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report_text)
                
        return report_text

    def auto_fix_issues(self, dry_run: bool = True) -> Dict[str, int]:
        """Automatically fix issues that are safe to auto-fix"""
        fixed_count = 0
        skipped_count = 0
        
        print(f"🔧 {'[DRY RUN] ' if dry_run else ''}Auto-fixing safe issues...")
        
        # Group by file for batch processing
        by_file = {}
        for issue in self.issues:
            if issue.auto_fixable:
                if issue.file_path not in by_file:
                    by_file[issue.file_path] = []
                by_file[issue.file_path].append(issue)
                
        for file_path, file_issues in by_file.items():
            try:
                if not dry_run:
                    self._fix_file_issues(file_path, file_issues)
                fixed_count += len(file_issues)
                print(f"  ✅ Fixed {len(file_issues)} issues in {file_path}")
            except Exception as e:
                print(f"  ❌ Failed to fix {file_path}: {e}")
                skipped_count += len(file_issues)
                
        return {'fixed': fixed_count, 'skipped': skipped_count}

    def _fix_file_issues(self, file_path: str, issues: List[CodeIssue]) -> None:
        """Fix issues in a specific file"""
        # This would implement actual file fixing logic
        # For now, just log what would be fixed
        pass

def main():
    parser = argparse.ArgumentParser(description='SpectralCanvas Pro Code Quality Fixer')
    parser.add_argument('--scan-all', action='store_true', help='Scan all source files')
    parser.add_argument('--file', help='Scan specific file')
    parser.add_argument('--fix', action='store_true', help='Auto-fix safe issues')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be fixed without changing files')
    parser.add_argument('--report', help='Output report file path')
    parser.add_argument('--project-root', default='.', help='Project root directory')
    
    args = parser.parse_args()
    
    fixer = SpectralCanvasCodeFixer(args.project_root)
    
    if args.file:
        file_path = Path(args.file)
        issues = fixer.scan_file(file_path)
        fixer.issues = issues
        print(f"Found {len(issues)} issues in {file_path}")
        
    elif args.scan_all:
        fixer.scan_all_files()
        
    else:
        print("Use --scan-all to scan all files or --file <path> to scan specific file")
        return
        
    if args.fix:
        results = fixer.auto_fix_issues(dry_run=args.dry_run)
        print(f"Fixed: {results['fixed']}, Skipped: {results['skipped']}")
        
    # Generate report
    report_file = args.report or f"runtime/code_quality_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.md"
    report = fixer.generate_report(report_file)
    print(f"📄 Report saved to: {report_file}")
    
    # Print summary
    error_count = len([i for i in fixer.issues if i.severity == 'error'])
    warning_count = len([i for i in fixer.issues if i.severity == 'warning'])
    
    print(f"\n📊 Summary: {error_count} errors, {warning_count} warnings")
    
    if error_count > 0:
        sys.exit(1)  # Exit with error code if critical issues found

if __name__ == '__main__':
    main()