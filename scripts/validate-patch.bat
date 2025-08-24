@echo off
REM Patch Validation Script - Integration Protocol v2
REM Validates that Maker agent output follows checksum gate requirements

echo ==========================================
echo Patch Validation - Integration Protocol v2
echo ==========================================
echo.

if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    pause
    exit /b 1
)

echo This script validates Maker agent output for:
echo - Checksum verification echoed correctly
echo - Proper diff format structure  
echo - Required build/test commands present
echo - No unauthorized commentary
echo.

REM Input validation
if "%1"=="" (
    echo Usage: validate-patch.bat ^<patch_file^> [expected_checksum]
    echo.
    echo Example:
    echo   validate-patch.bat maker_output.txt a1b2c3d4...
    echo.
    echo Or create patch file interactively:
    set /p PATCH_FILE="Enter path to Maker output file: "
    if not exist "%PATCH_FILE%" (
        echo ERROR: File not found: %PATCH_FILE%
        pause
        exit /b 1
    )
) else (
    set PATCH_FILE=%1
)

if "%2"=="" (
    set /p EXPECTED_CHECKSUM="Enter expected SHA256 checksum: "
) else (
    set EXPECTED_CHECKSUM=%2
)

echo.
echo === VALIDATING PATCH FILE ===
echo File: %PATCH_FILE%
echo Expected Checksum: %EXPECTED_CHECKSUM%
echo.

set VALIDATION_PASSED=1

REM Check 1: Checksum verification line
echo [1/6] Checking checksum verification...
findstr /c:"CHECKSUM VERIFICATION:" "%PATCH_FILE%" >nul
if errorlevel 1 (
    echo   [FAIL] Missing "CHECKSUM VERIFICATION:" line
    set VALIDATION_PASSED=0
) else (
    REM Extract the checksum from the file
    for /f "tokens=3" %%c in ('findstr /c:"CHECKSUM VERIFICATION:" "%PATCH_FILE%"') do set FOUND_CHECKSUM=%%c
    
    if "%FOUND_CHECKSUM%"=="%EXPECTED_CHECKSUM%" (
        echo   [PASS] Checksum verification matches expected value
    ) else (
        echo   [FAIL] Checksum mismatch
        echo         Expected: %EXPECTED_CHECKSUM%
        echo         Found:    %FOUND_CHECKSUM%
        set VALIDATION_PASSED=0
    )
)

REM Check 2: Diff section structure
echo [2/6] Checking diff section structure...
findstr /c:"==DIFF==" "%PATCH_FILE%" >nul
if errorlevel 1 (
    echo   [FAIL] Missing ==DIFF== section
    set VALIDATION_PASSED=0
) else (
    echo   [PASS] ==DIFF== section found
)

REM Check 3: Build section
echo [3/6] Checking build section...
findstr /c:"==BUILD==" "%PATCH_FILE%" >nul
if errorlevel 1 (
    echo   [FAIL] Missing ==BUILD== section
    set VALIDATION_PASSED=0
) else (
    REM Check for required cmake commands
    findstr /c:"cmake" "%PATCH_FILE%" | findstr /c:"-B build" >nul
    if errorlevel 1 (
        echo   [WARN] Build section missing standard cmake configuration
    ) else (
        echo   [PASS] Build section with cmake commands found
    )
)

REM Check 4: Test section
echo [4/6] Checking test section...
findstr /c:"==TEST==" "%PATCH_FILE%" >nul
if errorlevel 1 (
    echo   [FAIL] Missing ==TEST== section
    set VALIDATION_PASSED=0
) else (
    REM Check for ctest command
    findstr /c:"ctest" "%PATCH_FILE%" >nul
    if errorlevel 1 (
        echo   [WARN] Test section missing ctest command
    ) else (
        echo   [PASS] Test section with ctest command found
    )
)

REM Check 5: End marker
echo [5/6] Checking end marker...
findstr /c:"==END==" "%PATCH_FILE%" >nul
if errorlevel 1 (
    echo   [FAIL] Missing ==END== marker
    set VALIDATION_PASSED=0
) else (
    echo   [PASS] ==END== marker found
)

REM Check 6: Unauthorized content
echo [6/6] Checking for unauthorized content...
set COMMENTARY_FOUND=0

REM Look for common commentary patterns outside of diff blocks
findstr /r /c:"^[^+-@].*[Ss]uccessfully" "%PATCH_FILE%" | findstr /v "CHECKSUM VERIFICATION:" >nul
if not errorlevel 1 set COMMENTARY_FOUND=1

findstr /r /c:"^[^+-@].*[Pp]erfect" "%PATCH_FILE%" >nul  
if not errorlevel 1 set COMMENTARY_FOUND=1

findstr /r /c:"^[^+-@].*[Cc]ompleted" "%PATCH_FILE%" >nul
if not errorlevel 1 set COMMENTARY_FOUND=1

if %COMMENTARY_FOUND%==1 (
    echo   [FAIL] Unauthorized commentary detected
    echo         Maker agents should output ONLY schema format
    set VALIDATION_PASSED=0
) else (
    echo   [PASS] No unauthorized commentary detected
)

echo.
echo === VALIDATION SUMMARY ===

if %VALIDATION_PASSED%==1 (
    echo [SUCCESS] Patch validation PASSED
    echo.
    echo This Maker output follows Integration Protocol v2 requirements:
    echo ✓ Checksum verification matches expected value
    echo ✓ Proper schema structure (DIFF/BUILD/TEST/END)
    echo ✓ No unauthorized commentary
    echo.
    echo READY FOR APPLICATION:
    echo 1. Apply the unified diff manually
    echo 2. Run the build commands from ==BUILD== section
    echo 3. Run the test commands from ==TEST== section  
    echo 4. Verify exit code 0 and expected test results
) else (
    echo [FAILURE] Patch validation FAILED
    echo.
    echo This Maker output violates Integration Protocol v2:
    echo - Fix the issues identified above
    echo - Regenerate the patch with correct format
    echo - Do not apply until validation passes
    echo.
    echo COMMON FIXES:
    echo - Ensure Maker echoes SHA256 exactly from TL;DR
    echo - Remove any explanatory text outside schema
    echo - Include all required sections (DIFF/BUILD/TEST/END)
)

echo.
echo === ADDITIONAL CHECKS ===

REM Check for RT-safety violations in diff (basic)
findstr /c:"new " "%PATCH_FILE%" | findstr /c:"processBlock" >nul
if not errorlevel 1 (
    echo [WARNING] Possible memory allocation in processBlock detected
    echo           Run rt-linter for comprehensive RT-safety analysis
)

REM Check for missing denormal protection  
findstr /c:"processBlock" "%PATCH_FILE%" >nul
if not errorlevel 1 (
    findstr /c:"ScopedNoDenormals" "%PATCH_FILE%" >nul
    if errorlevel 1 (
        echo [WARNING] processBlock changes without denormal protection
        echo           Consider adding juce::ScopedNoDenormals
    )
)

echo.
pause