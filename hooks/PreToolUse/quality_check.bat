@echo off
REM SpectralCanvas Pro - Pre-Tool Use Quality Check Hook
REM Automatically runs before any major tool operations

setlocal enabledelayedexpansion

echo.
echo 🔍 Pre-Tool Quality Check
echo =========================

REM Quick check for critical issues
echo 🚨 Checking for critical RT-safety violations...

REM Check if there are any obvious RT-safety issues in recent files
findstr /s /i /c:"malloc" /c:"new " /c:"delete " /c:"mutex" /c:"lock_guard" Source\Core\*.cpp > temp_rt_issues.txt 2>nul
if exist temp_rt_issues.txt (
    for /f %%i in ('type temp_rt_issues.txt ^| find /c /v ""') do set RT_ISSUE_COUNT=%%i
    if !RT_ISSUE_COUNT! gtr 0 (
        echo ⚠️  Found !RT_ISSUE_COUNT! potential RT-safety issues in core files
        echo    Running detailed scan...
        
        python tools/code_quality/auto_code_fixer.py --file Source\Core\PluginProcessor.cpp 2>nul
        if !errorlevel! neq 0 (
            echo ❌ Critical RT-safety violations detected
            echo    Please fix before proceeding with tool operations
            echo    Use: python tools/code_quality/auto_code_fixer.py --scan-all
            del temp_rt_issues.txt 2>nul
            exit /b 1
        )
    )
    del temp_rt_issues.txt 2>nul
) else (
    echo ✅ No obvious RT-safety issues detected
)

REM Check for recent build errors
if exist "runtime\error_monitor.json" (
    echo 🔍 Checking for recent critical errors...
    python tools/code_quality/error_monitor.py --summary | findstr /i "critical error" > temp_critical.txt 2>nul
    if exist temp_critical.txt (
        for /f %%i in ('type temp_critical.txt ^| find /c /v ""') do set CRITICAL_COUNT=%%i
        if !CRITICAL_COUNT! gtr 0 (
            echo ⚠️  Found recent critical errors
            echo    Consider reviewing error summary before proceeding
        )
        del temp_critical.txt 2>nul
    )
)

echo ✅ Pre-tool quality check complete
echo.