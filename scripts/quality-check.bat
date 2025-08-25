@echo off
REM SpectralCanvas Pro - Integrated Quality Check Script
REM Runs comprehensive code quality checks and error monitoring

setlocal enabledelayedexpansion

echo.
echo 🎵 SpectralCanvas Pro - Quality Check System
echo ===============================================

REM Check if we're in the right directory
if not exist "CMakeLists.txt" (
    echo ❌ Error: Not in SpectralCanvas Pro root directory
    echo    Please run this script from the project root where CMakeLists.txt is located
    exit /b 1
)

REM Check Python availability
python --version >nul 2>&1
if !errorlevel! neq 0 (
    echo ❌ Error: Python not found in PATH
    echo    Please install Python 3.7+ and ensure it's in your PATH
    exit /b 1
)

REM Create runtime directory if it doesn't exist
if not exist "runtime" mkdir runtime

REM Parse command line arguments
set SCAN_ALL=0
set FIX_ISSUES=0
set MONITOR_DAEMON=0
set TARGET_FILE=
set GENERATE_REPORT=1

:parse_args
if "%~1"=="" goto end_parse
if "%~1"=="--scan-all" set SCAN_ALL=1
if "%~1"=="--fix" set FIX_ISSUES=1
if "%~1"=="--monitor" set MONITOR_DAEMON=1
if "%~1"=="--file" (
    shift
    set TARGET_FILE=%~1
)
if "%~1"=="--no-report" set GENERATE_REPORT=0
shift
goto parse_args
:end_parse

echo.
echo 🔍 Phase 1: Code Quality Analysis
echo ----------------------------------

if "%TARGET_FILE%" neq "" (
    echo 📄 Scanning specific file: %TARGET_FILE%
    python tools/code_quality/auto_code_fixer.py --file "%TARGET_FILE%"
    if !errorlevel! neq 0 (
        echo ❌ Critical issues found in %TARGET_FILE%
        exit /b 1
    )
) else if %SCAN_ALL%==1 (
    echo 📂 Scanning entire codebase...
    python tools/code_quality/auto_code_fixer.py --scan-all
    if !errorlevel! neq 0 (
        echo ❌ Critical issues found in codebase
        echo    Review the report and fix critical errors before proceeding
        exit /b 1
    )
) else (
    echo 📂 Scanning modified files...
    
    REM Get list of modified files (Git)
    git status --porcelain 2>nul | findstr /E "\.cpp \.h \.hpp" > temp_modified_files.txt
    if exist temp_modified_files.txt (
        for /f "tokens=2*" %%i in (temp_modified_files.txt) do (
            echo    Checking: %%j
            python tools/code_quality/auto_code_fixer.py --file "%%j"
        )
        del temp_modified_files.txt
    ) else (
        echo    No modified C++ files detected, scanning critical components...
        python tools/code_quality/auto_code_fixer.py --file "Source/Core/PluginProcessor.cpp"
        python tools/code_quality/auto_code_fixer.py --file "Source/Core/SpectralSynthEngine.cpp"
    )
)

echo ✅ Code quality analysis complete

echo.
echo 🔧 Phase 2: Automated Fixes
echo ----------------------------

if %FIX_ISSUES%==1 (
    echo 🛠️  Applying automated fixes...
    python tools/code_quality/auto_code_fixer.py --scan-all --fix --dry-run
    
    echo.
    set /p APPLY_FIXES="Apply these fixes? (y/N): "
    if /i "!APPLY_FIXES!"=="y" (
        python tools/code_quality/auto_code_fixer.py --scan-all --fix
        echo ✅ Automated fixes applied
    ) else (
        echo ⏭️  Fixes skipped (dry-run only)
    )
) else (
    echo ⏭️  Automated fixes disabled (use --fix to enable)
)

echo.
echo 📊 Phase 3: Error Monitoring
echo -----------------------------

REM Check for existing error logs
if exist "runtime\error_monitor.json" (
    echo 📈 Generating error summary from recent activity...
    python tools/code_quality/error_monitor.py --summary
) else (
    echo 📋 No previous error history found
)

if %MONITOR_DAEMON%==1 (
    echo 👀 Starting error monitoring daemon...
    echo    Press Ctrl+C to stop monitoring
    python tools/code_quality/error_monitor.py --daemon
) else (
    echo 🔍 Analyzing existing logs for patterns...
    python tools/code_quality/error_monitor.py --summary
)

echo.
echo 📄 Phase 4: Report Generation
echo ------------------------------

if %GENERATE_REPORT%==1 (
    set TIMESTAMP=%date:~10,4%%date:~4,2%%date:~7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
    set TIMESTAMP=!TIMESTAMP: =0!
    set REPORT_FILE=runtime\quality_report_!TIMESTAMP!.md
    
    echo 📝 Generating comprehensive quality report...
    python tools/code_quality/auto_code_fixer.py --scan-all --report "!REPORT_FILE!"
    
    echo ✅ Quality report saved to: !REPORT_FILE!
    
    REM Open report if available
    if exist "!REPORT_FILE!" (
        echo 👀 Opening report...
        start "" "!REPORT_FILE!"
    )
) else (
    echo ⏭️  Report generation disabled
)

echo.
echo 🎉 Quality Check Complete
echo =========================

REM Final summary
echo.
echo 📋 Summary:
if %SCAN_ALL%==1 (
    echo    ✅ Full codebase scan completed
) else (
    echo    ✅ Modified files scan completed
)

if %FIX_ISSUES%==1 (
    echo    ✅ Automated fixes processed
)

if %MONITOR_DAEMON%==1 (
    echo    ✅ Error monitoring daemon started
) else (
    echo    ✅ Error analysis completed
)

if %GENERATE_REPORT%==1 (
    echo    ✅ Quality report generated
)

echo.
echo 💡 Next Steps:
echo    • Review any critical issues in the report
echo    • Run build tests: powershell scripts\build-and-notify.ps1
echo    • For RT-safety issues, consult rt-audio-guardian agent
echo    • For build problems, consult build-stability-monitor agent

echo.
echo 🔧 Available Commands:
echo    quality-check.bat --scan-all --fix      # Full scan with fixes
echo    quality-check.bat --monitor             # Start error monitoring
echo    quality-check.bat --file Source\Core\MyFile.cpp  # Scan specific file

echo.
pause