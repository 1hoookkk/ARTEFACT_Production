# SpectralCanvas Pro - Automated Code Quality & Error Fixing System

## 🎯 Overview

This system provides comprehensive automated code quality assurance for SpectralCanvas Pro, specifically designed to catch and fix errors commonly introduced by AI code generation (like Claude) while maintaining RT-safety and JUCE best practices.

## 🚀 Quick Start

### 1. Setup (One-time)
```cmd
scripts\setup-quality-system.bat
```

### 2. Daily Usage
```cmd
# Quick quality check
quality-check.bat --scan-all

# Auto-fix safe issues
quality-check.bat --fix

# Start error monitoring
monitor-errors.cmd --daemon
```

### 3. For Specific Files
```cmd
# Check a file you just modified
quality-check.bat --file Source\Core\MyFile.cpp

# Monitor for issues while developing
python tools\code_quality\error_monitor.py --daemon
```

## 🔧 Core Tools

### Auto Code Fixer (`tools/code_quality/auto_code_fixer.py`)
**Purpose**: Scans C++ code for quality issues and provides automated fixes

**Key Features**:
- **RT-Safety Detection**: Finds memory allocation, blocking operations in audio thread
- **JUCE Anti-patterns**: Detects improper JUCE API usage  
- **C++ Quality**: Identifies common modern C++ violations
- **Automated Fixing**: Safely fixes issues with user approval

**Common Issues Detected**:
- `malloc()` in audio thread → "Pre-allocate in constructor"
- `std::mutex` in processBlock → "Use atomic operations"
- `#include <windows.h>` before JUCE → "Reorder includes"
- Raw pointers → "Use smart pointers"

### Error Monitor (`tools/code_quality/error_monitor.py`)
**Purpose**: Real-time monitoring of build outputs and runtime crashes

**Key Features**:
- **Build Error Analysis**: Parses compiler output for known patterns
- **Crash Detection**: Analyzes runtime crashes and suggests fixes
- **Agent Routing**: Recommends which specialist agent to consult
- **Pattern Learning**: Improves detection over time

**Error Classifications**:
- **Critical**: Segfaults, RT-safety violations → Immediate action
- **Error**: Build failures, API issues → Fix in current session
- **Warning**: Anti-patterns, style issues → Address in maintenance

### Quality Check Script (`scripts/quality-check.bat`)
**Purpose**: Integrated workflow combining all quality tools

**Workflow Phases**:
1. **Code Analysis**: Scan for issues
2. **Automated Fixes**: Apply safe fixes with approval
3. **Error Monitoring**: Check for patterns in recent activity
4. **Report Generation**: Create comprehensive quality reports

## 🎨 AI Code Integration Workflow

### When Claude (or other AI) Generates Code

1. **Immediate Scan**:
   ```cmd
   quality-check.bat --file Source\Core\NewAICode.cpp
   ```

2. **RT-Safety Check** (for audio code):
   ```cmd
   findstr /i "malloc new delete mutex" Source\Core\NewAICode.cpp
   ```

3. **Integration Test**:
   ```cmd
   quality-check.bat --scan-all --fix
   powershell scripts\build-and-notify.ps1
   ```

### Common AI Code Issues & Fixes

| Issue | AI Often Does | Quality System Fix |
|-------|---------------|-------------------|
| Memory allocation in audio thread | `new float[size]` | → Pre-allocate in constructor |
| Blocking operations | `std::mutex lock` | → Use atomic operations |
| Include order | `#include <windows.h>` first | → Move after JUCE headers |
| Raw pointers | `float* buffer = new float[1024]` | → `std::vector<float> buffer` |
| Exception handling | `try/catch` in processBlock | → Return error codes |

## 🤖 Agent System Integration

The quality system automatically routes issues to specialized agents:

### RT-Safety Issues → `rt-audio-guardian`
- Memory allocation violations
- Blocking operations in audio thread
- Thread safety problems

### JUCE Problems → `juce-integration-specialist`  
- API compatibility issues
- Platform-specific problems
- Plugin format compliance

### Build Issues → `build-stability-monitor`
- CMake configuration errors
- Dependency problems
- Linker issues

### General Quality → `code-quality-specialist`
- Code style violations
- Performance issues
- Maintainability problems

## 📊 Quality Metrics & Reporting

### Automated Reports
- **Daily**: Error pattern summary
- **Weekly**: Quality trend analysis  
- **Per-Session**: Comprehensive scan results

### Key Metrics Tracked
- **RT-Safety Score**: % of audio code that's RT-safe
- **Error Rate**: Issues per 1000 lines of code
- **Fix Rate**: % of issues automatically resolved
- **Build Success**: % of successful builds

### Report Locations
- `runtime/reports/` - Quality reports
- `runtime/error_monitor.json` - Error event log
- `runtime/quality_metrics.md` - Trend analysis

## 🔄 Continuous Integration

### Pre-Commit Hooks
- Automatic quality scan on file changes
- RT-safety validation for audio code
- Build error prevention

### Build Integration
- Quality checks run automatically during builds
- Failed quality checks can block builds
- Automated notifications for critical issues

### Development Workflow
```
Edit Code → Quality Check → Auto-Fix → Build → Monitor → Commit
     ↑                                           ↓
     └─────────── Fix Issues ←── Error Analysis ─┘
```

## 🛠️ Configuration

### Main Config File: `.quality_config`
```ini
[scan_settings]
auto_fix_safe_issues=true
rt_safety_strict_mode=true
generate_reports=true

[monitoring]
monitor_build_logs=true
notify_on_critical=true
```

### Tool-Specific Settings
- **Auto Fixer**: Customize detection patterns
- **Error Monitor**: Configure notification thresholds
- **Quality Check**: Set workflow preferences

## 🚨 Emergency Procedures

### Critical RT-Safety Violation
1. **Stop Development**: Don't commit the code
2. **Run Analysis**: `quality-check.bat --file <problem_file>`
3. **Consult Agent**: Contact `rt-audio-guardian`
4. **Verify Fix**: Test with stress testing

### Build System Breakdown
1. **Clean Build**: Delete build directory
2. **Quality Scan**: `quality-check.bat --scan-all`
3. **Agent Help**: Contact `build-stability-monitor`
4. **Document Solution**: Add to knowledge base

### Crash Loop
1. **Capture Dumps**: Enable crash reporting
2. **Analyze Pattern**: `monitor-errors.cmd --analyze-crash <dump>`
3. **Emergency Fix**: Use minimal safe configuration
4. **Root Cause**: Deep analysis with appropriate agent

## 📚 Documentation

### Essential Reading
- `docs/workflows/code-quality-best-practices.md` - Complete workflow guide
- `docs/agents/code-quality-specialist.contract.yaml` - Agent specifications
- `Source/Core/RT_SAFETY_GUIDELINES.md` - RT-safety requirements

### Quick Reference
- **Commands**: All available quality commands
- **Error Patterns**: Common issues and fixes
- **Agent Contacts**: When to use which specialist

## 🎯 Best Practices

### For AI-Assisted Development
1. **Scan Immediately**: Check all AI-generated code
2. **Verify RT-Safety**: Manually review audio thread code
3. **Test Integration**: Ensure new code works with existing system
4. **Document Decisions**: Record any non-obvious choices

### For Manual Development
1. **Frequent Scanning**: Check quality during development
2. **Fix Early**: Address issues as soon as detected
3. **Use Agents**: Consult specialists for complex problems
4. **Monitor Trends**: Watch quality metrics over time

## 💡 Tips & Tricks

### Faster Development
- Use `quality-check.bat --file <file>` for quick checks
- Run error monitoring in background: `monitor-errors.cmd --daemon &`
- Enable auto-fix for safe issues: `--fix` flag

### Better Results
- Review quality reports regularly
- Update detection patterns based on new issues
- Share findings with the team
- Integrate quality checks into your editor

## 🔧 Troubleshooting

### Common Setup Issues
- **Python not found**: Install Python 3.7+ and add to PATH
- **Dependencies missing**: Run `pip install -r tools/code_quality/requirements.txt`
- **Tools not working**: Check file permissions and paths

### Performance Issues
- **Slow scanning**: Use `--file` for individual files
- **High memory usage**: Increase available RAM or scan smaller sections
- **Background monitoring**: Use `--daemon` mode for less overhead

---

## 🎵 Ready to Rock!

Your SpectralCanvas Pro codebase now has comprehensive automated quality assurance. The system will help you maintain high code quality while leveraging AI assistance effectively and safely.

**Next Steps**:
1. Run the setup script: `scripts\setup-quality-system.bat`
2. Read the workflow guide: `docs\workflows\code-quality-best-practices.md`
3. Try a quality check: `quality-check.bat --scan-all`
4. Start monitoring: `monitor-errors.cmd --daemon`

Happy coding with confidence! 🚀