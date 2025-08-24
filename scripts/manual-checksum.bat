@echo off
REM Manual Checksum Helper - No PowerShell Required
REM Creates properly formatted input for Reader agent checksum calculation

echo ==========================================
echo Manual Checksum Helper - Protocol v2
echo ==========================================
echo.

echo This script creates the exact input format for Reader agent
echo to calculate SHA256 checksums for TL;DR verification.
echo.

REM Get current timestamp for filename
set TIMESTAMP=%DATE:~-4%%DATE:~4,2%%DATE:~7,2%_%TIME:~0,2%%TIME:~3,2%
set TIMESTAMP=%TIMESTAMP: =0%

set OUTPUT_FILE=checksum_input_%TIMESTAMP%.txt

echo Creating input file: %OUTPUT_FILE%
echo.

echo === EXAMPLE FOR HARMONIC BRUSH FEATURE ===
echo.
echo Here's what the input should look like for our next feature:
echo.

REM Create example content
(
echo Source/Core/SpectralSynthEngine.cpp
echo Source/Core/SpectralSynthEngine.h
echo Tests/TestHarmonicBrush.cpp
echo Parameter harmonicMode appears in APVTS as boolean
echo Canvas Y-coordinate maps to fundamental frequency
echo Harmonic overtones generated with correct ratios
echo Build succeeds without warnings
echo TestHarmonicBrush passes with correct frequency ratios
echo RT-linter shows zero audio thread violations
echo Exit code 0 from ctest
) > "%OUTPUT_FILE%"

echo Content written to %OUTPUT_FILE%:
echo.
type "%OUTPUT_FILE%"
echo.

echo === INSTRUCTIONS ===
echo 1. Edit %OUTPUT_FILE% with your actual FILES/ASSERTIONS/DoD
echo 2. Copy the exact content to Reader agent with this prompt:
echo.
echo "Calculate SHA256 checksum over this content (concatenated as single string):"
echo [paste content from %OUTPUT_FILE%]
echo "Return format: SHA256: [64-character-hex]"
echo.
echo 3. Use the returned SHA256 in your TL;DR Checksum
echo 4. Maker agent must echo this exact SHA256 before generating diffs
echo.

echo === EDIT THE INPUT FILE ===
set /p EDIT_NOW="Open %OUTPUT_FILE% for editing? (y/n): "
if /i "%EDIT_NOW%"=="y" (
    start notepad "%OUTPUT_FILE%"
    echo.
    echo Edit the file with your specific:
    echo - FILES: Source files that will be modified
    echo - ASSERTIONS: What tests must prove
    echo - DoD: Definition of Done criteria
    echo.
    echo Then copy the content to Reader agent for SHA256 calculation.
)

echo.
pause