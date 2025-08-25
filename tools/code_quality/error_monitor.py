#!/usr/bin/env python3
"""
SpectralCanvas Pro - Real-time Error Monitor
============================================

Continuously monitors build outputs, runtime logs, and crash dumps
for errors and automatically suggests fixes based on known patterns.

Features:
- Real-time log monitoring
- Crash dump analysis
- Build error pattern matching
- Automatic issue classification
- Integration with agent system

Usage:
    python error_monitor.py --watch-build
    python error_monitor.py --analyze-crash runtime/crash_analysis/latest.dmp
    python error_monitor.py --daemon
"""

import os
import re
import sys
import time
import json
import subprocess
import threading
from pathlib import Path
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass, asdict
from datetime import datetime
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

@dataclass
class ErrorEvent:
    """Represents a detected error event"""
    timestamp: str
    source: str  # 'build', 'runtime', 'crash'
    severity: str  # 'critical', 'error', 'warning', 'info'
    error_type: str
    message: str
    file_path: Optional[str] = None
    line_number: Optional[int] = None
    suggested_fix: Optional[str] = None
    agent_recommendation: Optional[str] = None

class BuildErrorAnalyzer:
    """Analyzes build errors and suggests fixes"""
    
    def __init__(self):
        # Common build error patterns and their fixes
        self.error_patterns = {
            # JUCE/C++ compilation errors
            r"error C2039.*'(\w+)'.*is not a member of.*juce": {
                'type': 'juce_missing_member',
                'fix': 'Update JUCE version or check API documentation for removed/renamed members',
                'agent': 'juce-integration-specialist'
            },
            r"error C2065.*'(\w+)'.*undeclared identifier": {
                'type': 'undeclared_identifier',
                'fix': 'Add missing #include directive or check spelling',
                'agent': 'build-stability-monitor'
            },
            r"error C2664.*cannot convert.*'std::(\w+)'": {
                'type': 'type_conversion',
                'fix': 'Add explicit type conversion or use correct type',
                'agent': 'spectralcanvas-pro-specialist'
            },
            r"error LNK2019.*unresolved external symbol.*'(\w+)'": {
                'type': 'unresolved_symbol',
                'fix': 'Add missing library to CMake target_link_libraries or implement missing function',
                'agent': 'build-stability-monitor'
            },
            r"fatal error C1083.*Cannot open include file.*'(\w+\.h)'": {
                'type': 'missing_header',
                'fix': 'Add missing include directory to CMake or install missing dependency',
                'agent': 'build-stability-monitor'
            },
            
            # RT-Safety violations
            r".*allocation.*audio.*thread": {
                'type': 'rt_safety_allocation',
                'fix': 'Move memory allocation to initialization or use lock-free structures',
                'agent': 'rt-audio-guardian'
            },
            r".*mutex.*processBlock": {
                'type': 'rt_safety_blocking',
                'fix': 'Replace mutex with atomic operations or lock-free queue',
                'agent': 'rt-audio-guardian'
            },
            
            # JUCE specific issues
            r".*MessageManager.*audio thread": {
                'type': 'juce_message_manager',
                'fix': 'Use MessageManager::callAsync or move to message thread',
                'agent': 'juce-integration-specialist'
            },
            r".*Graphics::.*without OpenGL context": {
                'type': 'juce_graphics_context',
                'fix': 'Ensure graphics operations are called from message thread',
                'agent': 'juce-integration-specialist'
            },
            
            # CMake configuration errors
            r"CMake Error.*target.*does not exist": {
                'type': 'cmake_missing_target',
                'fix': 'Check CMakeLists.txt for correct target name or add missing target',
                'agent': 'build-stability-monitor'
            },
            r"CMake Error.*JUCE.*not found": {
                'type': 'cmake_juce_missing',
                'fix': 'Ensure JUCE is properly fetched and configured in CMakeLists.txt',
                'agent': 'juce-integration-specialist'
            }
        }
        
    def analyze_build_output(self, output: str) -> List[ErrorEvent]:
        """Analyze build output and extract error information"""
        events = []
        lines = output.split('\n')
        
        for line_num, line in enumerate(lines):
            for pattern, info in self.error_patterns.items():
                match = re.search(pattern, line, re.IGNORECASE)
                if match:
                    # Extract file path and line number if present
                    file_match = re.search(r'([A-Za-z]:[^:]+\.(cpp|h|hpp)):(\d+)', line)
                    file_path = file_match.group(1) if file_match else None
                    line_number = int(file_match.group(3)) if file_match else None
                    
                    # Determine severity
                    severity = 'critical' if 'fatal' in line.lower() else 'error'
                    if 'warning' in line.lower():
                        severity = 'warning'
                        
                    event = ErrorEvent(
                        timestamp=datetime.now().isoformat(),
                        source='build',
                        severity=severity,
                        error_type=info['type'],
                        message=line.strip(),
                        file_path=file_path,
                        line_number=line_number,
                        suggested_fix=info['fix'],
                        agent_recommendation=info['agent']
                    )
                    events.append(event)
                    
        return events

class RuntimeErrorAnalyzer:
    """Analyzes runtime errors and crash patterns"""
    
    def __init__(self):
        self.runtime_patterns = {
            r"Segmentation fault.*core dumped": {
                'type': 'segfault',
                'fix': 'Check for null pointer dereference or buffer overflow',
                'agent': 'rt-audio-guardian'
            },
            r"Assertion failed.*juce_": {
                'type': 'juce_assertion',
                'fix': 'Check JUCE assertion condition and fix underlying issue',
                'agent': 'juce-integration-specialist'
            },
            r"Stack overflow.*recursion": {
                'type': 'stack_overflow',
                'fix': 'Check for infinite recursion or reduce stack usage',
                'agent': 'spectralcanvas-pro-specialist'
            },
            r"Access violation.*0x[0-9a-fA-F]+": {
                'type': 'access_violation',
                'fix': 'Check for null pointer access or use-after-free',
                'agent': 'rt-audio-guardian'
            },
            r"std::bad_alloc": {
                'type': 'memory_exhaustion',
                'fix': 'Reduce memory usage or check for memory leaks',
                'agent': 'rt-audio-guardian'
            }
        }
        
    def analyze_crash_log(self, log_content: str) -> List[ErrorEvent]:
        """Analyze crash logs for patterns"""
        events = []
        lines = log_content.split('\n')
        
        for line_num, line in enumerate(lines):
            for pattern, info in self.runtime_patterns.items():
                if re.search(pattern, line, re.IGNORECASE):
                    event = ErrorEvent(
                        timestamp=datetime.now().isoformat(),
                        source='runtime',
                        severity='critical',
                        error_type=info['type'],
                        message=line.strip(),
                        suggested_fix=info['fix'],
                        agent_recommendation=info['agent']
                    )
                    events.append(event)
                    
        return events

class ErrorMonitor:
    """Main error monitoring system"""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.build_analyzer = BuildErrorAnalyzer()
        self.runtime_analyzer = RuntimeErrorAnalyzer()
        self.error_log_file = self.project_root / "runtime" / "error_monitor.json"
        self.observers: List[Observer] = []
        self.running = False
        
        # Ensure runtime directory exists
        self.error_log_file.parent.mkdir(exist_ok=True)
        
    def log_error_event(self, event: ErrorEvent) -> None:
        """Log an error event to the persistent log"""
        events = []
        
        # Load existing events
        if self.error_log_file.exists():
            try:
                with open(self.error_log_file, 'r') as f:
                    events = json.load(f)
            except json.JSONDecodeError:
                events = []
                
        # Add new event
        events.append(asdict(event))
        
        # Keep only last 1000 events
        events = events[-1000:]
        
        # Save back to file
        with open(self.error_log_file, 'w') as f:
            json.dump(events, f, indent=2)
            
        # Print to console
        print(f"🚨 {event.severity.upper()}: {event.error_type}")
        print(f"   {event.message}")
        if event.suggested_fix:
            print(f"   💡 Fix: {event.suggested_fix}")
        if event.agent_recommendation:
            print(f"   🤖 Agent: {event.agent_recommendation}")
        print()

    def monitor_build_logs(self, build_dir: Path) -> None:
        """Monitor build directory for new log files"""
        class BuildLogHandler(FileSystemEventHandler):
            def __init__(self, monitor):
                self.monitor = monitor
                
            def on_created(self, event):
                if event.is_file and event.src_path.endswith('.log'):
                    self.monitor._analyze_build_log(Path(event.src_path))
                    
            def on_modified(self, event):
                if event.is_file and event.src_path.endswith('.log'):
                    self.monitor._analyze_build_log(Path(event.src_path))
                    
        if build_dir.exists():
            handler = BuildLogHandler(self)
            observer = Observer()
            observer.schedule(handler, str(build_dir), recursive=True)
            observer.start()
            self.observers.append(observer)
            print(f"👀 Monitoring build logs in {build_dir}")

    def monitor_runtime_logs(self, runtime_dir: Path) -> None:
        """Monitor runtime directory for crash dumps and logs"""
        class RuntimeLogHandler(FileSystemEventHandler):
            def __init__(self, monitor):
                self.monitor = monitor
                
            def on_created(self, event):
                if event.is_file:
                    path = Path(event.src_path)
                    if path.suffix in ['.log', '.dmp', '.txt']:
                        self.monitor._analyze_runtime_log(path)
                        
        if runtime_dir.exists():
            handler = RuntimeLogHandler(self)
            observer = Observer()
            observer.schedule(handler, str(runtime_dir), recursive=True)
            observer.start()
            self.observers.append(observer)
            print(f"👀 Monitoring runtime logs in {runtime_dir}")

    def _analyze_build_log(self, log_path: Path) -> None:
        """Analyze a build log file"""
        try:
            with open(log_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            events = self.build_analyzer.analyze_build_output(content)
            for event in events:
                self.log_error_event(event)
                
        except Exception as e:
            print(f"Error analyzing build log {log_path}: {e}")

    def _analyze_runtime_log(self, log_path: Path) -> None:
        """Analyze a runtime log file"""
        try:
            with open(log_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            events = self.runtime_analyzer.analyze_crash_log(content)
            for event in events:
                self.log_error_event(event)
                
        except Exception as e:
            print(f"Error analyzing runtime log {log_path}: {e}")

    def watch_build(self) -> None:
        """Start monitoring build processes"""
        build_dirs = [
            self.project_root / "build",
            self.project_root / "build-vs",
            self.project_root / "build-mingw"
        ]
        
        for build_dir in build_dirs:
            self.monitor_build_logs(build_dir)
            
        # Also monitor runtime crashes
        runtime_dir = self.project_root / "runtime"
        self.monitor_runtime_logs(runtime_dir)
        
        self.running = True
        print("🔍 Error monitoring started. Press Ctrl+C to stop.")
        
        try:
            while self.running:
                time.sleep(1)
        except KeyboardInterrupt:
            print("\n🛑 Stopping error monitor...")
            self.stop()

    def stop(self) -> None:
        """Stop all monitoring"""
        self.running = False
        for observer in self.observers:
            observer.stop()
            observer.join()
        self.observers.clear()

    def analyze_existing_logs(self) -> List[ErrorEvent]:
        """Analyze all existing log files"""
        all_events = []
        
        # Analyze build logs
        build_dirs = [self.project_root / "build", self.project_root / "logs"]
        for build_dir in build_dirs:
            if build_dir.exists():
                for log_file in build_dir.rglob("*.log"):
                    try:
                        with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
                            content = f.read()
                        events = self.build_analyzer.analyze_build_output(content)
                        all_events.extend(events)
                    except Exception as e:
                        print(f"Error reading {log_file}: {e}")
                        
        # Analyze runtime logs
        runtime_dir = self.project_root / "runtime"
        if runtime_dir.exists():
            for log_file in runtime_dir.rglob("*.log"):
                try:
                    with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    events = self.runtime_analyzer.analyze_crash_log(content)
                    all_events.extend(events)
                except Exception as e:
                    print(f"Error reading {log_file}: {e}")
                    
        return all_events

    def generate_summary_report(self) -> str:
        """Generate a summary report of recent errors"""
        events = []
        
        # Load from log file
        if self.error_log_file.exists():
            try:
                with open(self.error_log_file, 'r') as f:
                    event_dicts = json.load(f)
                    events = [ErrorEvent(**e) for e in event_dicts]
            except Exception as e:
                print(f"Error loading event log: {e}")
                
        # Group by error type
        by_type = {}
        for event in events:
            if event.error_type not in by_type:
                by_type[event.error_type] = []
            by_type[event.error_type].append(event)
            
        # Generate report
        report = []
        report.append("# SpectralCanvas Pro - Error Monitor Summary")
        report.append(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report.append(f"Total Events: {len(events)}")
        report.append("")
        
        # Summary by severity
        by_severity = {}
        for event in events:
            by_severity[event.severity] = by_severity.get(event.severity, 0) + 1
            
        report.append("## Summary by Severity")
        for severity, count in sorted(by_severity.items()):
            report.append(f"- {severity.title()}: {count}")
        report.append("")
        
        # Top error types
        report.append("## Most Common Error Types")
        type_counts = [(error_type, len(events)) for error_type, events in by_type.items()]
        type_counts.sort(key=lambda x: x[1], reverse=True)
        
        for error_type, count in type_counts[:10]:
            report.append(f"- {error_type}: {count} occurrences")
            # Show sample fix
            sample_event = by_type[error_type][0]
            if sample_event.suggested_fix:
                report.append(f"  💡 Fix: {sample_event.suggested_fix}")
            if sample_event.agent_recommendation:
                report.append(f"  🤖 Agent: {sample_event.agent_recommendation}")
            report.append("")
            
        return "\n".join(report)

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='SpectralCanvas Pro Error Monitor')
    parser.add_argument('--watch-build', action='store_true', help='Monitor build processes')
    parser.add_argument('--analyze-crash', help='Analyze specific crash dump file')
    parser.add_argument('--daemon', action='store_true', help='Run as background daemon')
    parser.add_argument('--summary', action='store_true', help='Generate summary report')
    parser.add_argument('--project-root', default='.', help='Project root directory')
    
    args = parser.parse_args()
    
    monitor = ErrorMonitor(args.project_root)
    
    if args.analyze_crash:
        crash_file = Path(args.analyze_crash)
        if crash_file.exists():
            with open(crash_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            events = monitor.runtime_analyzer.analyze_crash_log(content)
            for event in events:
                monitor.log_error_event(event)
        else:
            print(f"Crash file not found: {crash_file}")
            
    elif args.summary:
        report = monitor.generate_summary_report()
        print(report)
        
        # Save report
        report_file = Path(args.project_root) / "runtime" / f"error_summary_{datetime.now().strftime('%Y%m%d_%H%M%S')}.md"
        with open(report_file, 'w') as f:
            f.write(report)
        print(f"\n📄 Report saved to: {report_file}")
        
    elif args.watch_build or args.daemon:
        monitor.watch_build()
        
    else:
        # Analyze existing logs
        events = monitor.analyze_existing_logs()
        for event in events:
            monitor.log_error_event(event)
        print(f"Analyzed {len(events)} error events")

if __name__ == '__main__':
    main()