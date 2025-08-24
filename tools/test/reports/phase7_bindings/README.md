# Phase 7 Validation Reports

This directory contains test reports and logs from Phase 7 validation runs.

## Directory Structure

Each validation run creates a timestamped subdirectory with the following files:

### Required Log Files
- `minimal_smoke.log` - 10-second minimal mode stability test
- `param_jog.log` - Parameter change and attachment validation
- `editor_open_close.log` - Combined editor lifecycle test results
- `paint_perf_snapshot.txt` - Basic performance metrics
- `attachments_status.txt` - Parameter attachment verification status

### Report Files
- `PHASE7_VALIDATION_REPORT.txt` - Comprehensive validation summary
- Individual cycle logs: `editor_open_close_1.log`, `editor_open_close_2.log`, `editor_open_close_3.log`

## Exit Code Reference

- **Exit 0**: Clean shutdown (ideal)
- **Exit 1**: Timeout exit (expected for timed tests)  
- **Exit 139**: Segmentation fault (CRITICAL - indicates stability failure)
- **Other codes**: Unexpected behavior requiring investigation

## Usage

1. Run `tools\test\verify-phase7-setup.bat` to verify test infrastructure
2. Run `tools\build\build-debug.bat` to build the debug executable
3. Run `tools\test\phase7-validation-runbook.bat` for comprehensive validation
4. Or run `tools\test\phase7-quick-validation.bat` for rapid smoke testing

## Report Analysis

### Critical Failure Indicators
- Any Exit 139 (segmentation fault)
- Repeated crashes across multiple test phases
- Complete failure to launch in minimal mode

### Warning Indicators  
- Unexpected exit codes (not 0 or 1)
- Single editor cycle failures
- Incomplete log files

### Success Indicators
- All tests complete with Exit 0 or Exit 1
- Clean editor lifecycle logs
- Parameter attachment verification successful
- No segmentation faults detected

## Latest Report Access

The most recent validation report is always copied to:
`runtime\phase7_validation_latest.txt`

This provides quick access to the latest validation status without navigating timestamped directories.