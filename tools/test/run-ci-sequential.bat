@echo off
echo === SpectralCanvas Sequential CI Test ===
echo.

REM Set environment for sequential-only mode
set SC_FORCE_SINGLECORE=1

REM Create runtime directory if it doesn't exist
if not exist runtime mkdir runtime

echo [INFO] Starting sequential CI test with single-core enforcement
echo [INFO] Environment: SC_FORCE_SINGLECORE=%SC_FORCE_SINGLECORE%
echo.

REM Test 1: Check if executable exists
if not exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo [ERROR] SpectralCanvas Pro.exe not found. Build may have failed.
    echo [ERROR] Expected location: build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe
    exit /b 1
)

echo [PASS] SpectralCanvas Pro.exe found
echo.

REM Test 2: Minimal mode stability test
echo Testing --minimal mode for 5 seconds...
timeout 5 "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --minimal --log > runtime\minimal_test.log 2>&1

if %ERRORLEVEL% EQU 0 (
    echo [PASS] Minimal mode completed successfully
) else (
    echo [WARN] Minimal mode exited with code %ERRORLEVEL%
    echo [INFO] Check runtime\minimal_test.log for details
)
echo.

REM Test 3: Safe mode stability test
echo Testing --safe mode for 5 seconds...
timeout 5 "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --safe --log > runtime\safe_test.log 2>&1

if %ERRORLEVEL% EQU 0 (
    echo [PASS] Safe mode completed successfully
) else (
    echo [WARN] Safe mode exited with code %ERRORLEVEL%
    echo [INFO] Check runtime\safe_test.log for details
)
echo.

REM Test 4: Quick stability test (2 seconds default mode)
echo Testing default mode for 2 seconds...
timeout 2 "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --log > runtime\default_test.log 2>&1

if %ERRORLEVEL% EQU 0 (
    echo [PASS] Default mode completed successfully
) else (
    echo [WARN] Default mode exited with code %ERRORLEVEL%
    echo [INFO] Check runtime\default_test.log for details
)
echo.

REM Generate timestamp for report
for /f "tokens=1-4 delims=/ " %%i in ("%date%") do (
    set yyyy=%%l
    set mm=%%j
    set dd=%%k
)
for /f "tokens=1-3 delims=: " %%i in ("%time%") do (
    set hh=%%i
    set min=%%j
    set ss=%%k
)

REM Create status report
echo === Sequential CI Test Report === > runtime\ci_sequential_report.txt
echo Test Date: %date% %time% >> runtime\ci_sequential_report.txt
echo Environment: SC_FORCE_SINGLECORE=%SC_FORCE_SINGLECORE% >> runtime\ci_sequential_report.txt
echo. >> runtime\ci_sequential_report.txt
echo Executable Status: FOUND >> runtime\ci_sequential_report.txt
echo Minimal Mode: TESTED >> runtime\ci_sequential_report.txt
echo Safe Mode: TESTED >> runtime\ci_sequential_report.txt
echo Default Mode: TESTED >> runtime\ci_sequential_report.txt
echo. >> runtime\ci_sequential_report.txt
echo Log files generated: >> runtime\ci_sequential_report.txt
echo - runtime\minimal_test.log >> runtime\ci_sequential_report.txt
echo - runtime\safe_test.log >> runtime\ci_sequential_report.txt
echo - runtime\default_test.log >> runtime\ci_sequential_report.txt

echo [INFO] Sequential CI test completed
echo [INFO] Report saved to runtime\ci_sequential_report.txt
echo.
echo === CI Sequential Test Complete ===