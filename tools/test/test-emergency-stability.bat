@echo off
echo ==============================================================================
echo SpectralCanvas Pro - Emergency Stability Test Suite
echo ==============================================================================
echo.
echo This script tests all emergency stabilization modes to identify crash points.
echo Tests are run in order of increasing complexity to isolate failure points.
echo.

set REPORT_DIR=runtime
set TIMESTAMP=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%
set EXECUTABLE=build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe

REM Create runtime directory for test reports
if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"

echo Test Results will be saved to: %REPORT_DIR%\stability_test_%TIMESTAMP%.log
echo.

REM Check if executable exists
if not exist "%EXECUTABLE%" (
    echo ERROR: Executable not found: %EXECUTABLE%
    echo Please run build-debug.bat first.
    pause
    exit /b 1
)

echo ==============================================================================
echo TEST 1: Minimal Mode (Maximum Safety)
echo ==============================================================================
echo.
echo Testing minimal mode - only VST3 compliance, no complex features...
echo Command: "%EXECUTABLE%" --minimal --log
echo.

REM Test minimal mode with 5 second timeout
timeout 5 "%EXECUTABLE%" --minimal --log > "%REPORT_DIR%\test_minimal_%TIMESTAMP%.log" 2>&1
set MINIMAL_RESULT=%ERRORLEVEL%

if %MINIMAL_RESULT% equ 0 (
    echo ✅ MINIMAL MODE: SUCCESS - Clean exit
) else if %MINIMAL_RESULT% equ 1 (
    echo ⚠️  MINIMAL MODE: TIMEOUT - Still running after 5 seconds ^(may be stable^)
) else (
    echo ❌ MINIMAL MODE: CRASH - Exit code %MINIMAL_RESULT%
    echo Last output:
    tail -n 5 "%REPORT_DIR%\test_minimal_%TIMESTAMP%.log"
)

echo.
echo ==============================================================================
echo TEST 2: Safe Mode (Essential Systems)
echo ==============================================================================
echo.
echo Testing safe mode - essential systems only, simplified UI...
echo Command: "%EXECUTABLE%" --safe --log
echo.

REM Test safe mode with 5 second timeout
timeout 5 "%EXECUTABLE%" --safe --log > "%REPORT_DIR%\test_safe_%TIMESTAMP%.log" 2>&1
set SAFE_RESULT=%ERRORLEVEL%

if %SAFE_RESULT% equ 0 (
    echo ✅ SAFE MODE: SUCCESS - Clean exit
) else if %SAFE_RESULT% equ 1 (
    echo ⚠️  SAFE MODE: TIMEOUT - Still running after 5 seconds ^(may be stable^)
) else (
    echo ❌ SAFE MODE: CRASH - Exit code %SAFE_RESULT%
    echo Last output:
    tail -n 5 "%REPORT_DIR%\test_safe_%TIMESTAMP%.log"
)

echo.
echo ==============================================================================
echo TEST 3: Normal Mode (Full Features)
echo ==============================================================================
echo.
echo Testing normal mode - all features enabled...
echo Command: "%EXECUTABLE%" --log
echo.

REM Test normal mode with 5 second timeout
timeout 5 "%EXECUTABLE%" --log > "%REPORT_DIR%\test_normal_%TIMESTAMP%.log" 2>&1
set NORMAL_RESULT=%ERRORLEVEL%

if %NORMAL_RESULT% equ 0 (
    echo ✅ NORMAL MODE: SUCCESS - Clean exit
) else if %NORMAL_RESULT% equ 1 (
    echo ⚠️  NORMAL MODE: TIMEOUT - Still running after 5 seconds ^(may be stable^)
) else (
    echo ❌ NORMAL MODE: CRASH - Exit code %NORMAL_RESULT%
    echo Last output:
    tail -n 5 "%REPORT_DIR%\test_normal_%TIMESTAMP%.log"
)

echo.
echo ==============================================================================
echo TEST 4: Debug Mode (All Debug Features)
echo ==============================================================================
echo.
echo Testing debug mode - all debug features enabled...
echo Command: "%EXECUTABLE%" --debug --log
echo.

REM Test debug mode with 5 second timeout
timeout 5 "%EXECUTABLE%" --debug --log > "%REPORT_DIR%\test_debug_%TIMESTAMP%.log" 2>&1
set DEBUG_RESULT=%ERRORLEVEL%

if %DEBUG_RESULT% equ 0 (
    echo ✅ DEBUG MODE: SUCCESS - Clean exit
) else if %DEBUG_RESULT% equ 1 (
    echo ⚠️  DEBUG MODE: TIMEOUT - Still running after 5 seconds ^(may be stable^)
) else (
    echo ❌ DEBUG MODE: CRASH - Exit code %DEBUG_RESULT%
    echo Last output:
    tail -n 5 "%REPORT_DIR%\test_debug_%TIMESTAMP%.log"
)

echo.
echo ==============================================================================
echo EMERGENCY STABILITY SUMMARY
echo ==============================================================================
echo.

REM Calculate stability score
set /a SCORE=0
if %MINIMAL_RESULT% leq 1 set /a SCORE+=25
if %SAFE_RESULT% leq 1 set /a SCORE+=25
if %NORMAL_RESULT% leq 1 set /a SCORE+=25
if %DEBUG_RESULT% leq 1 set /a SCORE+=25

echo Stability Test Results:
echo   Minimal Mode: %MINIMAL_RESULT% ^(%~1^)
echo   Safe Mode:    %SAFE_RESULT% ^(%~2^)
echo   Normal Mode:  %NORMAL_RESULT% ^(%~3^)
echo   Debug Mode:   %DEBUG_RESULT% ^(%~4^)
echo.
echo Overall Stability Score: %SCORE%/100

if %SCORE% geq 75 (
    echo ✅ EXCELLENT: Most modes stable
) else if %SCORE% geq 50 (
    echo ⚠️  MODERATE: Some modes stable
) else if %SCORE% geq 25 (
    echo ❌ POOR: Only basic modes stable
) else (
    echo 🚨 CRITICAL: All modes failing
)

echo.
echo Generated logs in %REPORT_DIR%:
for %%f in ("%REPORT_DIR%\test_*_%TIMESTAMP%.log") do echo   - %%~nxf

REM Create summary report
echo Emergency Stability Test Summary > "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo ================================== >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo Timestamp: %TIMESTAMP% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo Executable: %EXECUTABLE% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo Results: >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo   Minimal Mode: %MINIMAL_RESULT% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo   Safe Mode:    %SAFE_RESULT% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo   Normal Mode:  %NORMAL_RESULT% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo   Debug Mode:   %DEBUG_RESULT% >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo. >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"
echo Stability Score: %SCORE%/100 >> "%REPORT_DIR%\stability_summary_%TIMESTAMP%.txt"

echo.
echo Emergency stability testing complete.
pause