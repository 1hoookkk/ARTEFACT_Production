# SpectralCanvas Pro - Code Quality & Error Fixing Workflow

## Overview

This document outlines the comprehensive workflow for maintaining code quality, detecting errors early, and implementing automated fixes in SpectralCanvas Pro. The system is designed to catch issues introduced by AI code generation (like Claude) and provide immediate feedback and fixes.

## 🎯 Core Principles

### 1. **RT-Safety First**
- Audio thread code must be lock-free and allocation-free
- No blocking operations in `processBlock()` or related audio callbacks
- Use atomic operations for thread-safe parameter updates

### 2. **JUCE Best Practices**
- Proper include order (JUCE headers before system headers)
- Avoid Windows macro pollution (`NOMINMAX`, `WIN32_LEAN_AND_MEAN`)
- Use JUCE idioms for threading and graphics

### 3. **Automated Quality Assurance**
- Every code change is scanned for common issues
- Build errors are automatically analyzed and categorized
- Runtime crashes trigger immediate analysis and suggestions

## 🔧 Tools & Automation

### Primary Tools

1. **Auto Code Fixer** (`tools/code_quality/auto_code_fixer.py`)
   - Scans C++ code for RT-safety violations
   - Detects JUCE anti-patterns
   - Identifies common C++ issues
   - Auto-fixes safe issues with user approval

2. **Error Monitor** (`tools/code_quality/error_monitor.py`)
   - Real-time monitoring of build outputs
   - Crash dump analysis
   - Pattern matching for known error types
   - Agent-specific recommendations

3. **Build Integration** (`scripts/build-and-notify.ps1`)
   - Automated build with quality checks
   - Notification system for build status
   - Integration with quality tools

## 📋 Daily Workflow

### For Development Sessions

1. **Pre-Development Scan**
   ```bash
   # Scan entire codebase for existing issues
   python tools/code_quality/auto_code_fixer.py --scan-all --report runtime/pre_session_report.md
   
   # Review and fix critical issues
   python tools/code_quality/auto_code_fixer.py --fix --dry-run
   ```

2. **During Development**
   ```bash
   # Start error monitoring daemon
   python tools/code_quality/error_monitor.py --daemon &
   
   # For each file you modify:
   python tools/code_quality/auto_code_fixer.py --file Source/Core/MyFile.cpp
   ```

3. **Pre-Commit Checks**
   ```bash
   # Full quality scan
   python tools/code_quality/auto_code_fixer.py --scan-all --fix
   
   # Build with monitoring
   powershell scripts/build-and-notify.ps1 -Config Debug -Tests
   ```

### For Code Reviews

1. **Review AI-Generated Code**
   ```bash
   # Scan specific files changed by AI
   python tools/code_quality/auto_code_fixer.py --file Source/Core/NewAICode.cpp
   
   # Check for RT-safety issues
   grep -r "malloc\|new\|delete\|mutex" Source/Core/NewAICode.cpp
   ```

2. **Integration Testing**
   ```bash
   # Run comprehensive tests
   tools/test/phase7-quick-validation.bat
   
   # Monitor for new crashes
   python tools/code_quality/error_monitor.py --summary
   ```

## 🚨 Error Classification & Response

### Critical Errors (Immediate Action Required)

**RT-Safety Violations**
- Memory allocation in audio thread
- Blocking operations in `processBlock()`
- Mutex usage in audio callbacks

*Response*: Stop development, fix immediately, verify with stress testing

**Build Failures**
- Missing dependencies
- Header pollution issues
- Linker errors

*Response*: Use automated fixes where possible, escalate to build-stability-monitor agent

### Warning Issues (Address in Current Session)

**JUCE Anti-patterns**
- Incorrect include order
- Message manager usage in audio thread
- Graphics operations without proper context

*Response*: Apply automated fixes, verify behavior unchanged

**C++ Quality Issues**
- Raw pointer usage where smart pointers are better
- Missing const correctness
- Inefficient string operations

*Response*: Schedule for refactoring, document technical debt

### Info Issues (Address in Maintenance)

**Code Style**
- Naming convention inconsistencies
- Missing documentation
- Overly complex functions

*Response*: Batch fix during maintenance windows

## 🤖 Agent Integration

### When to Activate Specific Agents

**rt-audio-guardian**
- Any RT-safety violations detected
- Audio glitches or dropouts reported
- Performance issues in audio thread

**juce-integration-specialist**
- JUCE API usage questions
- Platform-specific build issues
- Plugin format compatibility problems

**build-stability-monitor**
- CMake configuration errors
- Dependency resolution issues
- Cross-platform build failures

**spectralcanvas-pro-specialist**
- Domain-specific architecture questions
- Canvas-to-audio pipeline issues
- Component integration problems

### Agent Handoff Workflow

1. **Error Detection**: Automated tools detect issue
2. **Classification**: Error monitor categorizes issue type
3. **Agent Recommendation**: System suggests appropriate specialist
4. **Context Preparation**: Gather relevant files and error details
5. **Agent Activation**: Use specific agent contract for resolution

## 📊 Monitoring & Metrics

### Quality Metrics Tracked

- **Error Rate**: Errors per 1000 lines of code
- **RT-Safety Score**: Percentage of audio code that's RT-safe
- **Build Success Rate**: Percentage of successful builds
- **Crash Frequency**: Crashes per hour of runtime
- **Fix Rate**: Percentage of auto-fixable issues resolved

### Regular Reports

**Daily**: Error monitor summary
**Weekly**: Code quality trends
**Monthly**: Technical debt assessment

## 🔄 Continuous Improvement

### Learning from Errors

1. **Pattern Analysis**: Identify recurring error types
2. **Tool Enhancement**: Update detection patterns
3. **Documentation**: Update best practices based on findings
4. **Training**: Share learnings with development team

### Automation Enhancement

1. **New Pattern Detection**: Add patterns for new error types
2. **Fix Algorithm Improvement**: Enhance auto-fix capabilities
3. **Integration**: Better integration with existing tools
4. **Performance**: Optimize scan and fix performance

## 🎓 Best Practice Guidelines

### For AI-Generated Code

1. **Always Scan**: Run quality tools on all AI-generated code
2. **RT-Safety Check**: Manually verify audio thread safety
3. **Integration Test**: Ensure new code works with existing system
4. **Documentation**: Document any non-obvious design decisions

### For Manual Code Changes

1. **Pre-Commit Scan**: Check quality before committing
2. **Incremental Testing**: Test small changes frequently
3. **Agent Consultation**: Use specialist agents for complex issues
4. **Code Review**: Have quality-conscious peer review

### For Emergency Fixes

1. **Stabilization First**: Use safest possible approach
2. **Monitor Impact**: Watch for regression issues
3. **Document Decision**: Record why emergency approach was chosen
4. **Schedule Cleanup**: Plan proper fix for later

## 📞 Quick Reference

### Common Commands

```bash
# Quick file scan
python tools/code_quality/auto_code_fixer.py --file <path>

# Auto-fix safe issues
python tools/code_quality/auto_code_fixer.py --scan-all --fix --dry-run

# Start monitoring
python tools/code_quality/error_monitor.py --daemon

# Emergency crash analysis
python tools/code_quality/error_monitor.py --analyze-crash <crash_file>

# Generate quality report
python tools/code_quality/auto_code_fixer.py --scan-all --report <output_file>
```

### Emergency Contacts

- **RT-Safety Issues**: Use rt-audio-guardian agent
- **Build Failures**: Use build-stability-monitor agent  
- **JUCE Problems**: Use juce-integration-specialist agent
- **General Issues**: Use spectralcanvas-pro-specialist agent

---

*This workflow ensures that SpectralCanvas Pro maintains high code quality while leveraging AI assistance effectively and safely.*