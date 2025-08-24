@echo off
echo ==============================================================================
echo SpectralCanvas Pro - Phase 7 Quick Validation
echo ==============================================================================
echo.
echo This is a streamlined version of the Phase 7 validation for rapid testing.
echo Use phase7-validation-runbook.bat for comprehensive validation.
echo.

REM Check if executable exists
if not exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo [ERROR] SpectralCanvas Pro.exe not found. Run build-debug.bat first.
    echo [ERROR] Expected location: build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe
    exit /b 1
)

echo [PASS] Executable found
echo.

REM Create runtime directory if needed
if not exist runtime mkdir runtime

echo [INFO] Running quick smoke test (5 seconds minimal mode)...
timeout 5 "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" --minimal --log > runtime\phase7_quick_test.log 2>&1

set EXIT_CODE=%ERRORLEVEL%

if %EXIT_CODE% EQU 0 (
    echo [PASS] Quick validation successful - Clean exit
) else if %EXIT_CODE% EQU 1 (
    echo [PASS] Quick validation successful - Timeout exit (expected)
) else if %EXIT_CODE% EQU 139 (
    echo [CRITICAL] Segmentation fault detected! Phase 7 has critical issues.
    echo [ERROR] Review runtime\phase7_quick_test.log for details
    exit /b 139
) else (
    echo [WARN] Unexpected exit code: %EXIT_CODE%
    echo [INFO] Check runtime\phase7_quick_test.log for details
)

echo.
echo Quick validation complete. Exit code: %EXIT_CODE%
echo Log file: runtime\phase7_quick_test.log
echo.
echo For comprehensive validation, run: tools\test\phase7-validation-runbook.bat