# SpectralCanvas Pro - Windows Notification System

This document provides comprehensive guidance for using the custom Windows hooks and notification system integrated with SpectralCanvas Pro development workflow.

## 📋 Table of Contents

- [Overview](#overview)
- [Quick Start](#quick-start)
- [Script Reference](#script-reference)
- [Usage Examples](#usage-examples)
- [Configuration](#configuration)
- [Integration with Claude Code](#integration-with-claude-code)
- [Troubleshooting](#troubleshooting)

## 🎯 Overview

The notification system provides:
- **Desktop notifications** with BurntToast → SnoreToast → console fallback
- **Comprehensive logging** with timestamped files in `./logs/`
- **RT-safety awareness** for audio thread development
- **Slack/Teams integration** for remote development and CI
- **TDD guard** with continuous file watching
- **Claude Code integration** via allowlisted wrapper commands

### Core Philosophy
- **Secure by design**: Only expose wrapper commands, not raw PowerShell
- **RT-safety first**: Special handling for audio thread operations
- **Fail gracefully**: Multiple fallback levels for notifications
- **Evidence-based**: All operations logged with timestamps

## 🚀 Quick Start

### 1. Basic Build with Notifications
```powershell
# Full build pipeline with notifications
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build-and-notify.ps1

# Release build with Slack notifications (if webhook configured)
$env:SPECTRALCANVAS_SLACK_WEBHOOK = "https://hooks.slack.com/your/webhook/url"
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build-and-notify.ps1 -Config Release
```

### 2. Test Execution with RT-Safety Focus
```powershell
# Run all tests with notifications
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1

# Focus on RT-safety tests only
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1 -RTSafetyOnly

# Specific test with verbose output
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1 -SpecificTest "TestPaintProducesAudio" -Verbose
```

### 3. TDD Guard with File Watching
```powershell
# Start TDD guard with RT-safety focus
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/tdd-guard-notify.ps1

# Custom watch settings
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/tdd-guard-notify.ps1 -WatchIntervalSeconds 5 -TestPattern "Paint|Audio"
```

## 📚 Script Reference

### `notify.ps1` - Core Notification Engine
Smart desktop notification with multiple fallback levels.

**Parameters:**
- `-Title`: Notification title (default: "SpectralCanvas Pro")
- `-Message`: Notification message (default: "Task completed")
- `-Icon`: Path to icon file (optional)
- `-TimeoutSeconds`: Display timeout (default: 5)
- `-SnoreToastPath`: Path to SnoreToast.exe fallback (default: "../tools/SnoreToast.exe")

**Fallback Chain:**
1. **BurntToast** (Windows 10/11 native) - Preferred
2. **SnoreToast** (bundled executable) - Fallback
3. **Console output** - Always works

```powershell
# Basic notification
scripts/notify.ps1 -Title "Build Complete" -Message "SpectralCanvas Pro built successfully"

# With custom icon and timeout
scripts/notify.ps1 -Title "RT-Safety Alert" -Message "Audio thread violation detected" -Icon "assets/warning.png" -TimeoutSeconds 10
```

### `run-and-notify.ps1` - Universal Task Wrapper
Executes commands with comprehensive logging and notifications.

**Parameters:**
- `-Command`: Command to execute (required)
- `-Title`: Task title for logging and notifications
- `-LogDir`: Log directory (default: "./logs")
- `-WorkingDir`: Working directory (default: ".")
- `-RTSafety`: Flag for RT-safety context
- `-DryRun`: Preview mode without execution

**Features:**
- Captures stdout/stderr with timestamps
- RT-safety violation detection
- Comprehensive error reporting
- Desktop and Slack notifications
- SpectralCanvas-specific troubleshooting hints

```powershell
# Generic task execution
scripts/run-and-notify.ps1 -Title "Custom Build" -Command "cmake --build build --target SpectralCanvas"

# RT-safety focused task
scripts/run-and-notify.ps1 -Title "Audio Engine Test" -Command "ctest -R processBlock" -RTSafety

# Dry run preview
scripts/run-and-notify.ps1 -Title "Test Preview" -Command "complex-command-here" -DryRun
```

### `build-and-notify.ps1` - SpectralCanvas Build Wrapper
Specialized build system with multi-phase notifications.

**Parameters:**
- `-Config`: Build configuration (default: "RelWithDebInfo")
- `-Target`: Build target (default: "ALL_BUILD")
- `-BuildDir`: Build directory (default: "./build")
- `-Clean`: Clean before build
- `-Tests`: Include test execution (default: true)
- `-SlackWebhook`: Slack webhook URL (or use env var)
- `-DryRun`: Preview mode

**Build Phases:**
1. **Clean** (if requested)
2. **Configure** (CMake configuration)
3. **Compile** (Build execution)
4. **Test** (RT-safety validation)

```powershell
# Standard development build
scripts/build-and-notify.ps1

# Release build with clean
scripts/build-and-notify.ps1 -Config Release -Clean

# Debug build without tests
scripts/build-and-notify.ps1 -Config Debug -Tests:$false

# Preview build plan
scripts/build-and-notify.ps1 -DryRun
```

### `test-and-notify.ps1` - RT-Safety Test Runner
Specialized test execution with RT-safety focus and continuous monitoring.

**Parameters:**
- `-Config`: Build configuration (default: "RelWithDebInfo")
- `-BuildDir`: Build directory (default: "./build")
- `-TestPattern`: Regex pattern for test selection
- `-SpecificTest`: Run single named test
- `-RTSafetyOnly`: Focus on RT-safety tests only
- `-ContinuousMode`: Continuous test execution
- `-Verbose`: Detailed test output
- `-SlackWebhook`: Slack webhook URL
- `-DryRun`: Preview mode

**RT-Safety Patterns:**
Automatically detects: RT, Paint, Audio, processBlock, SpectralSynth, ThreadSafety, SPSC

```powershell
# All tests with notifications
scripts/test-and-notify.ps1

# RT-safety focus only
scripts/test-and-notify.ps1 -RTSafetyOnly

# Custom pattern
scripts/test-and-notify.ps1 -TestPattern "Paint|Canvas|Spectral"

# Continuous monitoring
scripts/test-and-notify.ps1 -ContinuousMode

# Single test debug
scripts/test-and-notify.ps1 -SpecificTest "TestPaintProducesAudio" -Verbose
```

### `tdd-guard-notify.ps1` - Continuous TDD with File Watching
Advanced TDD guard with file system monitoring and RT-safety focus.

**Parameters:**
- `-Config`: Build configuration (default: "RelWithDebInfo")
- `-BuildDir`: Build directory (default: "./build")
- `-WatchIntervalSeconds`: File check interval (default: 10)
- `-TestPattern`: Test pattern for TDD cycles (default: "RT|Paint|Audio")
- `-WatchPath`: Path to monitor (default: "./Source")
- `-SlackWebhook`: Slack webhook URL
- `-Verbose`: Detailed output
- `-DryRun`: Preview mode

**Features:**
- Monitors C++ source files (*.cpp, *.h, *.hpp)
- Automatic test execution on file changes
- Consecutive failure tracking
- RT-safety focused test selection
- File change notifications

```powershell
# Basic TDD guard
scripts/tdd-guard-notify.ps1

# Fast iteration with custom pattern
scripts/tdd-guard-notify.ps1 -WatchIntervalSeconds 5 -TestPattern "Paint|Canvas"

# Monitor specific directory
scripts/tdd-guard-notify.ps1 -WatchPath "./Source/Core" -TestPattern "RT"
```

### `notify-slack.ps1` - Webhook Integration
Secure webhook notifications for Slack, Teams, and Discord.

**Parameters:**
- `-WebhookUrl`: Webhook URL (required)
- `-Text`: Message text (required)
- `-Title`: Message title (default: "SpectralCanvas Pro")
- `-Color`: Message color - good/warning/danger or hex (default: "good")
- `-Channel`: Slack channel override
- `-Username`: Bot username (default: "SpectralCanvas-Bot")
- `-IconEmoji`: Bot emoji (default: ":musical_note:")
- `-RTSafety`: RT-safety context flag

**Service Support:**
- **Slack**: Full rich formatting with attachments
- **Microsoft Teams**: MessageCard format with facts
- **Discord/Generic**: Simple content format

```powershell
# Basic Slack notification
scripts/notify-slack.ps1 -WebhookUrl "https://hooks.slack.com/..." -Text "Build completed successfully"

# Teams notification with RT context
scripts/notify-slack.ps1 -WebhookUrl "https://outlook.office.com/..." -Text "RT-safety tests passed" -Color "good" -RTSafety

# Custom formatting
scripts/notify-slack.ps1 -WebhookUrl $webhook -Text "Critical failure" -Color "danger" -Channel "#dev-alerts"
```

## 💡 Usage Examples

### Development Workflow Integration

```powershell
# Morning startup: Clean build with full validation
scripts/build-and-notify.ps1 -Clean -Config RelWithDebInfo

# Feature development: TDD guard for paint engine work
scripts/tdd-guard-notify.ps1 -TestPattern "Paint|Canvas" -WatchPath "./Source/Core"

# Pre-commit validation: Full RT-safety test suite
scripts/test-and-notify.ps1 -RTSafetyOnly -Verbose

# Release preparation: Build all configurations
scripts/build-and-notify.ps1 -Config Release -Clean
scripts/test-and-notify.ps1 -Config Release
```

### CI/Remote Development

```powershell
# Set webhook for team notifications
$env:SPECTRALCANVAS_SLACK_WEBHOOK = "https://hooks.slack.com/services/YOUR/WEBHOOK/URL"

# Automated build with team notifications
scripts/build-and-notify.ps1 -Config Release

# Long-running test monitoring
scripts/test-and-notify.ps1 -ContinuousMode -RTSafetyOnly
```

### Debugging and Diagnostics

```powershell
# Preview build plan without execution
scripts/build-and-notify.ps1 -DryRun

# Detailed test diagnostics
scripts/test-and-notify.ps1 -SpecificTest "TestPaintProducesAudio" -Verbose

# Manual notification testing
scripts/notify.ps1 -Title "Test Alert" -Message "Notification system working"

# Check log files
Get-ChildItem ./logs -Filter "*.log" | Sort-Object LastWriteTime -Descending | Select-Object -First 5
```

## ⚙️ Configuration

### Environment Variables

```powershell
# Slack webhook for team notifications
$env:SPECTRALCANVAS_SLACK_WEBHOOK = "https://hooks.slack.com/services/T00000000/B00000000/XXXXXXXXXXXXXXXXXXXXXXXX"

# Persistent environment variable (Windows)
[Environment]::SetEnvironmentVariable("SPECTRALCANVAS_SLACK_WEBHOOK", "your-webhook-url", "User")
```

### Desktop Notification Setup

1. **BurntToast (Recommended)**
   ```powershell
   Install-Module -Name BurntToast -Scope CurrentUser
   ```

2. **SnoreToast (Fallback)**
   - Download from: https://github.com/KDE/snoretoast/releases
   - Place `SnoreToast.exe` in `./tools/` directory
   - Or specify custom path with `-SnoreToastPath`

3. **Console Fallback**
   - Always available, no setup required
   - Provides timestamped console output

### Log Configuration

```powershell
# Default log location
./logs/

# Log file naming pattern
{TaskTitle}_{YYYYMMDD_HHMMSS}.log

# Example log files
./logs/SpectralCanvas_Build_20250823_143022.log
./logs/SpectralCanvas_Tests_20250823_143045.log
./logs/TDD_Guard_20250823_143100.log
```

## 🔗 Integration with Claude Code

The notification system is pre-configured in `CLAUDE.md` with auto-accept permissions for wrapper commands.

### Allowlisted Commands
```powershell
# These commands can be executed by Claude Code without user approval:

pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build-and-notify.ps1
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1  
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/run-and-notify.ps1
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/notify.ps1
```

### Example Claude Code Integration
```markdown
## Claude Code Task Examples

**Build SpectralCanvas:**
```
Claude, please build SpectralCanvas Pro with notifications enabled.
```
→ Claude executes: `scripts/build-and-notify.ps1`

**Run RT-safety tests:**
```  
Claude, run the RT-safety focused tests and notify me of results.
```
→ Claude executes: `scripts/test-and-notify.ps1 -RTSafetyOnly`

**Start TDD guard:**
```
Claude, start TDD guard for paint engine development.
```
→ Claude executes: `scripts/tdd-guard-notify.ps1 -TestPattern "Paint|Canvas"`
```

### Security Model
- **Wrapper commands only**: Claude can only execute pre-approved wrapper scripts
- **No raw PowerShell**: Direct PowerShell execution requires user approval
- **Deterministic logging**: All operations logged with timestamps
- **Idempotent operations**: Safe to re-run, predictable outcomes

## 🔧 Troubleshooting

### Common Issues

#### Desktop Notifications Not Appearing
```powershell
# Check BurntToast installation
Get-Module -Name BurntToast -ListAvailable

# Test manual notification
scripts/notify.ps1 -Title "Test" -Message "Manual test notification"

# Check Windows notification settings
# Settings > System > Notifications & actions
```

#### SnoreToast Fallback Issues
```powershell
# Download and place SnoreToast.exe
# Check path: ./tools/SnoreToast.exe
Test-Path "./tools/SnoreToast.exe"

# Test with custom path
scripts/notify.ps1 -Title "Test" -Message "Testing SnoreToast" -SnoreToastPath "C:\path\to\SnoreToast.exe"
```

#### Slack Webhook Not Working
```powershell
# Verify webhook URL format
echo $env:SPECTRALCANVAS_SLACK_WEBHOOK
# Should be: https://hooks.slack.com/services/...

# Test webhook manually
scripts/notify-slack.ps1 -WebhookUrl $env:SPECTRALCANVAS_SLACK_WEBHOOK -Text "Test message"

# Check Slack app permissions and webhook configuration
```

#### Log Files Not Created
```powershell
# Check logs directory
Test-Path "./logs"

# Create manually if missing
New-Item -ItemType Directory -Path "./logs" -Force

# Check permissions
Get-Acl "./logs"
```

#### RT-Safety Detection Issues
```powershell
# Test RT-safety flag detection
scripts/run-and-notify.ps1 -Title "RT Test" -Command "echo processBlock" -RTSafety

# Check for RT-safety keywords in output
scripts/test-and-notify.ps1 -SpecificTest "TestPaintProducesAudio" -Verbose
```

### Performance Considerations

#### TDD Guard File Watching
```powershell
# Adjust watch interval for performance
scripts/tdd-guard-notify.ps1 -WatchIntervalSeconds 30  # Slower, less CPU

# Limit watch scope
scripts/tdd-guard-notify.ps1 -WatchPath "./Source/Core"  # Smaller scope

# Use specific test patterns
scripts/tdd-guard-notify.ps1 -TestPattern "Paint"  # Fewer tests
```

#### Log File Management
```powershell
# Clean old log files (older than 7 days)
Get-ChildItem "./logs" -Filter "*.log" | Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-7) } | Remove-Item

# Compress old logs
Get-ChildItem "./logs" -Filter "*.log" | Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-1) } | Compress-Archive -DestinationPath "./logs/archived_$(Get-Date -Format 'yyyyMMdd').zip"
```

### Debug Mode

Enable verbose output for troubleshooting:
```powershell
# All scripts support -Verbose flag
scripts/build-and-notify.ps1 -Verbose
scripts/test-and-notify.ps1 -Verbose  
scripts/tdd-guard-notify.ps1 -Verbose

# Use -DryRun to preview without execution
scripts/build-and-notify.ps1 -DryRun
scripts/run-and-notify.ps1 -Title "Debug" -Command "test-command" -DryRun
```

## 📈 Advanced Usage

### Custom Notification Channels

Create specialized notification flows:
```powershell
# Critical alerts to specific Slack channel
scripts/notify-slack.ps1 -WebhookUrl $webhook -Text "Build failed!" -Color "danger" -Channel "#critical-alerts"

# Success notifications to team channel  
scripts/notify-slack.ps1 -WebhookUrl $webhook -Text "Release ready!" -Color "good" -Channel "#releases"

# RT-safety alerts with context
scripts/notify-slack.ps1 -WebhookUrl $webhook -Text "Audio thread violation detected" -Color "warning" -RTSafety
```

### Integration with External Tools

```powershell
# Git hooks integration
# In .git/hooks/pre-commit:
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/test-and-notify.ps1 -RTSafetyOnly

# Task scheduler integration
schtasks /create /tn "SpectralCanvas Nightly Build" /tr "pwsh -File scripts/build-and-notify.ps1 -Config Release" /sc daily /st 02:00

# GitHub Actions integration
# In .github/workflows/build.yml:
# - name: Build with notifications
#   run: pwsh -File scripts/build-and-notify.ps1 -Config Release
```

---

## 📞 Support

For issues with the notification system:

1. **Check logs**: `./logs/` directory contains detailed execution logs
2. **Test components**: Use individual scripts with `-DryRun` and `-Verbose`
3. **Verify environment**: Check PowerShell execution policy and module installations
4. **Review configuration**: Ensure webhook URLs and paths are correct

The notification system is designed to fail gracefully - if advanced features don't work, basic console output will always be available.