@echo off
REM Simple Checksum Generator - Works without PowerShell
REM For Integration Protocol v2 TL;DR verification

echo ==========================================
echo Simple Checksum Generator - Protocol v2
echo ==========================================
echo.

if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    pause
    exit /b 1
)

echo This script helps create TL;DR checksums without PowerShell.
echo.
echo You'll input the FILES, ASSERTIONS, and DoD items.
echo The Reader agent will calculate the actual SHA256.
echo.

REM Create temporary file for the input
set TEMP_FILE=tldr_input_%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.txt

echo Enter FILES (one per line, press Enter on empty line to finish):
echo Example: Source/Core/PluginProcessor.cpp
echo.

REM Collect FILES
set FILE_COUNT=0
:input_files
set /p "file_line=FILE> "
if "%file_line%"=="" goto input_assertions
echo %file_line%>> "%TEMP_FILE%"
set /a FILE_COUNT+=1
goto input_files

:input_assertions
echo.
echo Enter ASSERTIONS (one per line, press Enter on empty line to finish):
echo Example: Parameter appears in APVTS with range 1.0-100.0
echo.

REM Collect ASSERTIONS  
set ASSERTION_COUNT=0
:input_assertions_loop
set /p "assertion_line=ASSERTION> "
if "%assertion_line%"=="" goto input_dod
echo %assertion_line%>> "%TEMP_FILE%"
set /a ASSERTION_COUNT+=1
goto input_assertions_loop

:input_dod
echo.
echo Enter DoD items (one per line, press Enter on empty line to finish):
echo Example: Build succeeds without warnings
echo.

REM Collect DoD
set DOD_COUNT=0
:input_dod_loop
set /p "dod_line=DoD> "
if "%dod_line%"=="" goto generate_template
echo %dod_line%>> "%TEMP_FILE%"
set /a DOD_COUNT+=1
goto input_dod_loop

:generate_template
echo.
echo === INPUT COLLECTED ===
echo Files: %FILE_COUNT%
echo Assertions: %ASSERTION_COUNT%
echo DoD items: %DOD_COUNT%
echo.

echo Content saved to: %TEMP_FILE%
echo.
type "%TEMP_FILE%"
echo.

echo === TL;DR TEMPLATE (Copy-paste ready) ===
echo.
echo ## TL;DR Checksum
echo TASK: ^<one-sentence outcome^>
echo FILES (locked^):
type "%TEMP_FILE%" | findstr /v "^$"
echo ENTITIES: ^<symbols/functions/ParamIDs^>
echo CONSTRAINTS: RT-audio safety + scope limits
echo ASSERTIONS (test must prove^):
echo ^<your assertions from above^>
echo DoD:
echo ^<your DoD items from above^>
echo SHA256: ^<READER_AGENT_WILL_CALCULATE_THIS^>
echo.

echo === NEXT STEPS ===
echo 1. Copy the content from %TEMP_FILE%
echo 2. Give this exact content to READER agent with instruction:
echo    "Calculate SHA256 checksum over this content in exact order"
echo 3. Reader will return the SHA256 hash
echo 4. Use that hash in your TL;DR Checksum
echo 5. Provide TL;DR to Maker agent
echo.

echo === FOR READER AGENT ===
echo Send this to reader-agent:
echo.
echo "Please calculate SHA256 checksum over the following content"
echo "Content (in exact order):"
echo.
type "%TEMP_FILE%"
echo.
echo "Return only: SHA256: ^<64-character-hex^>"
echo.

echo Content file: %TEMP_FILE%
echo Keep this file until you get the SHA256 from Reader agent.
echo.
pause