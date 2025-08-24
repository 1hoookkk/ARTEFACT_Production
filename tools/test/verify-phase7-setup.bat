@echo off
echo ==============================================================================
echo Phase 7 Validation Setup Verification
echo ==============================================================================
echo.

echo [INFO] Verifying Phase 7 test infrastructure...

REM Check required directories
echo [CHECK] Testing directory structure...
if not exist tools\test\reports\phase7_bindings (
    echo [ERROR] Phase 7 reports directory missing
    echo [FIX] Creating tools\test\reports\phase7_bindings...
    mkdir tools\test\reports\phase7_bindings
)
echo [PASS] Directory structure verified

REM Check for required scripts
echo.
echo [CHECK] Testing script availability...

if not exist tools\test\phase7-validation-runbook.bat (
    echo [ERROR] phase7-validation-runbook.bat missing
    exit /b 1
)
echo [PASS] Main runbook script found

if not exist tools\test\phase7-quick-validation.bat (
    echo [ERROR] phase7-quick-validation.bat missing  
    exit /b 1
)
echo [PASS] Quick validation script found

REM Check build tools
echo.
echo [CHECK] Build environment...
cmake --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake not available
    exit /b 1
)
echo [PASS] CMake available

where msbuild >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [WARN] MSBuild not in PATH - run from VS Developer Command Prompt
) else (
    echo [PASS] MSBuild available
)

REM Check if build exists
echo.
echo [CHECK] Build status...
if exist "build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe" (
    echo [PASS] Debug executable already exists
    echo [INFO] Ready for Phase 7 validation
) else (
    echo [WARN] Debug executable not found
    echo [INFO] Run tools\build\build-debug.bat before validation
)

echo.
echo ==============================================================================
echo Setup verification complete
echo ==============================================================================
echo.
echo Available Phase 7 validation commands:
echo   tools\test\phase7-quick-validation.bat      (5-second smoke test)
echo   tools\test\phase7-validation-runbook.bat    (comprehensive validation)
echo.
echo Prerequisites:
echo   1. Run tools\build\build-debug.bat to build executable
echo   2. Run from Visual Studio Developer Command Prompt for MSBuild access
echo   3. Ensure 15+ seconds available for full validation runbook