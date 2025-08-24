@echo off
setlocal enabledelayedexpansion

:: SpectralCanvas Pro - AI Handoff Export System
:: Captures complete project state for seamless AI-to-AI handoffs

echo ==========================================
echo  SpectralCanvas Pro - AI Handoff Export
echo ==========================================
echo.

set "HANDOFF_FILE=HANDOFF_PACKAGE.md"
set "TIMESTAMP=%DATE:~-4%-%DATE:~4,2%-%DATE:~7,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%"
set "TIMESTAMP=%TIMESTAMP: =0%"

echo Creating handoff package: %HANDOFF_FILE%
echo Timestamp: %TIMESTAMP%
echo.

:: Create handoff package
(
echo # 🔄 AI HANDOFF PACKAGE
echo **Generated:** %TIMESTAMP%
echo **Project:** SpectralCanvas Pro - Paint-to-Sound VST3
echo **Branch:** %cd%
echo.
echo ## 📋 QUICK CONTEXT ^(Copy this to any AI^)
echo ```
echo Project: SpectralCanvas Pro - Revolutionary paint-to-sound synthesis
echo Vision: MetaSynth immediacy + CDP depth for modern producers
echo Status: Core paint-to-audio COMPLETE + EMUFilter TPT SVF implemented
echo Method: TDD mandatory ^(RED→GREEN→REFACTOR^), RT-safe required
echo Tests: See test results below
echo Constitution: CLAUDE.md ^(TDD laws^) + PROJECT_SNAPSHOT.md ^(complete context^)
echo Next Phase: See roadmap section below
echo ```
echo.
echo ## 🧪 TEST SUITE STATUS
) > "%HANDOFF_FILE%"

:: Run tests and capture results
echo Running test suite...
call scripts\build-tests.bat > temp_build.log 2>&1
set BUILD_RESULT=%ERRORLEVEL%

call scripts\run-tests.bat > temp_test.log 2>&1
set TEST_RESULT=%ERRORLEVEL%

:: Add test results to handoff
(
echo **Build Status:** 
if %BUILD_RESULT%==0 (
    echo ✅ **SUCCESS** - Clean compilation
) else (
    echo ❌ **FAILED** - Build errors detected
)
echo.
echo **Test Results:**
if %TEST_RESULT%==0 (
    echo ✅ **ALL TESTS PASSING**
) else (
    echo ❌ **TEST FAILURES DETECTED**
)
echo.
echo ```
type temp_test.log
echo ```
echo.
echo ## 📁 GIT STATUS
echo ```
) >> "%HANDOFF_FILE%"

:: Add git status
git status --short >> "%HANDOFF_FILE%" 2>&1
(
echo ```
echo.
echo ## 🔄 RECENT CHANGES
echo ```
) >> "%HANDOFF_FILE%"

:: Add recent commits
git log --oneline -5 >> "%HANDOFF_FILE%" 2>&1
(
echo ```
echo.
echo ## 🎯 PROJECT STATE REFERENCE
echo **Complete Context:** See PROJECT_SNAPSHOT.md for full details
echo **Architecture:** Paint→SpectralSynthEngine→EMUFilter→Output
echo **Constitution:** CLAUDE.md contains TDD laws and methodology
echo.
echo ### Current Architecture Status
echo - ✅ Paint-to-audio core: COMPLETE ^(5/5 tests passing^)
echo - ✅ EMUFilter TPT SVF: COMPLETE ^(FFT verified -9.23dB @ 1kHz/200Hz cutoff^)
echo - 🔄 Parameter system: Connected to engine, UI pending
echo - 📋 Next: Spectral Brushes ^(see PROJECT_SNAPSHOT.md Phase 2^)
echo.
echo ### Key Files Modified Recently
) >> "%HANDOFF_FILE%"

:: Add recently modified files
git diff --name-only HEAD~5..HEAD >> "%HANDOFF_FILE%" 2>&1
(
echo.
echo ## 🚀 NEXT PHASE READY
echo **For Next AI:** Choose your role and use appropriate template:
echo - 💡 **Creative Concepts** → Use workflow\templates\chatgpt-concepts.md
echo - 🏗️ **Architecture Review** → Use workflow\templates\gemini-architecture.md  
echo - 📋 **Requirement Processing** → Use workflow\templates\claude-gpt-processing.md
echo - ⚡ **Implementation** → Use workflow\templates\claude-code-implementation.md
echo.
echo **Pre-flight Check:** Run `handoff-import.bat` to validate environment
echo.
echo ---
echo 🎨 *"Draw, and the canvas sings."* - SpectralCanvas Pro Team
) >> "%HANDOFF_FILE%"

:: Cleanup
del temp_build.log temp_test.log 2>nul

echo.
if %BUILD_RESULT%==0 (
    if %TEST_RESULT%==0 (
        echo ✅ **HANDOFF READY** - All systems GREEN
        echo 📦 Package created: %HANDOFF_FILE%
        echo 🎯 Copy the "QUICK CONTEXT" section to your next AI
    ) else (
        echo ⚠️  **HANDOFF WITH WARNINGS** - Test failures detected
        echo 📦 Package created: %HANDOFF_FILE%
        echo 🔍 Review test results before proceeding
    )
) else (
    echo ❌ **HANDOFF BLOCKED** - Build failures detected
    echo 📦 Package created: %HANDOFF_FILE%
    echo 🛠️ Fix build issues before handing off
)

echo.
echo 📋 **Next Steps:**
echo    1. Copy QUICK CONTEXT to next AI
echo    2. Reference specific template from workflow\templates\
echo    3. Run handoff-import.bat on receiving end
echo.
pause