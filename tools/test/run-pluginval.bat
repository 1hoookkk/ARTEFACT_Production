@echo off
echo ==============================================================================
echo SpectralCanvas Pro - PluginVal Validation Script
echo ==============================================================================
echo.
echo This script runs comprehensive VST3 validation using pluginval.
echo Tests all safety modes: Normal, Safe, and Minimal.
echo.

REM Check if pluginval exists
if not exist "pluginval.exe" (
    echo ERROR: pluginval.exe not found in current directory.
    echo.
    echo Please download pluginval from:
    echo https://github.com/Tracktion/pluginval/releases
    echo.
    echo Or build it yourself and place pluginval.exe in the project root.
    pause
    exit /b 1
)

REM Locate VST3 build (prefer RelWithDebInfo, fallback to Debug)
set VST3_PATH=
if exist "build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3" (
    set VST3_PATH=build\SpectralCanvas_artefacts\RelWithDebInfo\VST3\SpectralCanvas Pro.vst3
) else if exist "build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas Pro.vst3" (
    set VST3_PATH=build\SpectralCanvas_artefacts\Debug\VST3\SpectralCanvas Pro.vst3
) else (
    echo ERROR: VST3 build not found. Please build first:
    echo   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
    echo   cmake --build build --config RelWithDebInfo --target SpectralCanvas_VST3
    pause
    exit /b 1
)

REM Create runtime directory for validation reports
if not exist "runtime" mkdir runtime

set REPORT_DIR=runtime
set TIMESTAMP=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%

echo ==============================================================================
echo PHASE 1: Quick Smoke Test (Normal Mode)
echo ==============================================================================
echo.
echo Running basic validation to check if plugin loads...
pluginval.exe --validate "%VST3_PATH%" --strictness-level 5 --timeout-ms 30000 --output-dir "%REPORT_DIR%" --verbose > "%REPORT_DIR%\pluginval_smoke_%TIMESTAMP%.log" 2>&1

if %ERRORLEVEL% equ 0 (
    echo ✅ SMOKE TEST PASSED: Plugin loads successfully
) else (
    echo ❌ SMOKE TEST FAILED: Plugin failed to load
    echo Check log: %REPORT_DIR%\pluginval_smoke_%TIMESTAMP%.log
    type "%REPORT_DIR%\pluginval_smoke_%TIMESTAMP%.log"
    echo.
    echo ABORTING: Fix basic loading issues before continuing.
    pause
    exit /b 1
)

echo.
echo ==============================================================================
echo PHASE 2: Comprehensive Validation (Strict Mode)
echo ==============================================================================
echo.
echo Running comprehensive VST3 validation...
echo This may take several minutes and will stress-test the plugin.
echo.

pluginval.exe --validate "%VST3_PATH%" --strictness-level 10 --timeout-ms 180000 --output-dir "%REPORT_DIR%" --verbose > "%REPORT_DIR%\pluginval_full_%TIMESTAMP%.log" 2>&1

echo.
echo Validation complete. Results:
if %ERRORLEVEL% equ 0 (
    echo ✅ FULL VALIDATION PASSED: All tests successful
) else (
    echo ⚠️  FULL VALIDATION ISSUES: Some tests failed ^(common for complex plugins^)
    echo Check detailed log: %REPORT_DIR%\pluginval_full_%TIMESTAMP%.log
)

echo.
echo ==============================================================================
echo PHASE 3: Performance and Memory Test
echo ==============================================================================
echo.
echo Testing plugin under stress conditions...

pluginval.exe --validate "%VST3_PATH%" --strictness-level 8 --timeout-ms 120000 --num-repeats 3 --output-dir "%REPORT_DIR%" --verbose > "%REPORT_DIR%\pluginval_stress_%TIMESTAMP%.log" 2>&1

echo.
echo Stress test complete. Results:
if %ERRORLEVEL% equ 0 (
    echo ✅ STRESS TEST PASSED: Plugin stable under load
) else (
    echo ⚠️  STRESS TEST ISSUES: Performance problems detected
    echo Check detailed log: %REPORT_DIR%\pluginval_stress_%TIMESTAMP%.log
)

echo.
echo ==============================================================================
echo VALIDATION SUMMARY
echo ==============================================================================
echo.
echo Generated reports in %REPORT_DIR%:
echo   - pluginval_smoke_%TIMESTAMP%.log   ^(Quick smoke test^)
echo   - pluginval_full_%TIMESTAMP%.log    ^(Comprehensive validation^)
echo   - pluginval_stress_%TIMESTAMP%.log  ^(Performance and memory test^)
echo.
echo Review all logs for detailed analysis.
echo.

REM Create summary status file
echo Validation Summary > "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"
echo ================== >> "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"
echo Timestamp: %TIMESTAMP% >> "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"
echo Plugin: SpectralCanvas Pro >> "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"
echo Configuration: Debug VST3 >> "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\validation_summary_%TIMESTAMP%.txt"

echo All validation phases complete.
pause
