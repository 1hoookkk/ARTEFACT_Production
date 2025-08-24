@echo off
REM PostToolUse hook: QC snapshots and test logging
REM Always exits 0 - non-fatal logging only

echo [PostToolUse] QC snapshots starting... >> runtime\HOOKS_POST.txt
echo ================================== >> runtime\HOOKS_POST.txt
echo Timestamp: %DATE% %TIME% >> runtime\HOOKS_POST.txt

REM Check if build directory exists and run quick test
if exist "build" (
    echo Running SpectralSmoke test: >> runtime\HOOKS_POST.txt
    ctest --test-dir build -C Debug -R SpectralSmoke --output-on-failure >> runtime\HOOKS_POST.txt 2>&1
    
    if errorlevel 1 (
        echo [WARNING] SpectralSmoke test failed >> runtime\HOOKS_POST.txt
    ) else (
        echo [INFO] SpectralSmoke test passed >> runtime\HOOKS_POST.txt
    )
) else (
    echo [INFO] No build directory - skipping test run >> runtime\HOOKS_POST.txt
)

echo ================================== >> runtime\HOOKS_POST.txt
echo. >> runtime\HOOKS_POST.txt

REM Always exit 0 - non-fatal
exit /b 0
