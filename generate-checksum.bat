@echo off
REM SHA256 Checksum Generator for Integration Protocol v2
REM Creates checksums for TL;DR verification

echo ==========================================
echo SHA256 Checksum Generator - Protocol v2
echo ==========================================
echo.

if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    pause
    exit /b 1
)

REM Check if PowerShell is available for SHA256 calculation
powershell -Command "Get-Command Get-FileHash" >nul 2>&1
if errorlevel 1 (
    echo ERROR: PowerShell with Get-FileHash not available
    echo This script requires PowerShell 4.0 or later for SHA256 calculation
    pause
    exit /b 1
)

echo This script generates SHA256 checksums for TL;DR verification.
echo.
echo The checksum is calculated over:
echo 1. FILES (one per line)
echo 2. ASSERTIONS (one per line) 
echo 3. DoD items (one per line)
echo.

REM Create temporary file for checksum input
set TEMP_FILE=%TEMP%\spectral_checksum_input.txt

echo Enter FILES (one per line, press Enter on empty line to finish):
echo Example: Source/Core/PluginProcessor.cpp
echo.

(
    set "line_count=0"
    :input_files
    set /p "file_line=FILE> "
    if "%file_line%"=="" goto input_assertions
    echo %file_line%>> "%TEMP_FILE%"
    set /a line_count+=1
    goto input_files
)

:input_assertions
echo.
echo Enter ASSERTIONS (one per line, press Enter on empty line to finish):
echo Example: Parameter appears in APVTS with range 1.0-100.0
echo.

(
    :input_assertions_loop
    set /p "assertion_line=ASSERTION> "
    if "%assertion_line%"=="" goto input_dod
    echo %assertion_line%>> "%TEMP_FILE%"
    goto input_assertions_loop
)

:input_dod
echo.
echo Enter DoD items (one per line, press Enter on empty line to finish):
echo Example: Build succeeds without warnings
echo.

(
    :input_dod_loop
    set /p "dod_line=DoD> "
    if "%dod_line%"=="" goto calculate_hash
    echo %dod_line%>> "%TEMP_FILE%"
    goto input_dod_loop
)

:calculate_hash
echo.
echo === CALCULATING SHA256 ===

REM Calculate SHA256 using PowerShell
for /f "tokens=1" %%h in ('powershell -Command "(Get-FileHash -Path '%TEMP_FILE%' -Algorithm SHA256).Hash.ToLower()"') do set CHECKSUM=%%h

echo.
echo === CHECKSUM RESULT ===
echo.
echo SHA256: %CHECKSUM%
echo.

echo === TL;DR TEMPLATE (Copy-paste ready) ===
echo.
echo ## TL;DR Checksum
echo TASK: ^<one-sentence outcome^>
echo FILES (locked^):
type "%TEMP_FILE%" | findstr /v "ASSERTION DoD" | findstr /r "^"
echo ENTITIES: ^<symbols/functions/ParamIDs^>
echo CONSTRAINTS: RT-audio safety + scope limits
echo ASSERTIONS (test must prove^):
type "%TEMP_FILE%" | findstr /c:"ASSERTION" 2>nul
echo DoD:
type "%TEMP_FILE%" | findstr /c:"DoD" 2>nul
echo SHA256: %CHECKSUM%
echo.

echo === VERIFICATION ===
echo The Reader agent should produce the same SHA256 when processing:
echo 1. The exact FILES list above
echo 2. The exact ASSERTIONS list above  
echo 3. The exact DoD list above
echo.

REM Save result to file
set RESULT_FILE=checksum_%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.txt
(
    echo TL;DR Checksum Generated: %DATE% %TIME%
    echo.
    echo SHA256: %CHECKSUM%
    echo.
    echo Input Content:
    type "%TEMP_FILE%"
) > "%RESULT_FILE%"

echo Result saved to: %RESULT_FILE%
echo.

REM Cleanup
del "%TEMP_FILE%" 2>nul

echo === USAGE INSTRUCTIONS ===
echo 1. Copy the SHA256 value: %CHECKSUM%
echo 2. Paste into your TL;DR Checksum section
echo 3. Provide TL;DR to Maker agent
echo 4. Maker must echo this exact SHA256 before generating diffs
echo 5. If checksums don't match, operation will abort
echo.
pause