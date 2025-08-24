@echo off
REM AI Handoff Export Script for SpectralCanvas Pro
REM Creates a complete context package for handoff to other AIs

echo ========================================
echo AI Handoff Export - SpectralCanvas Pro
echo ========================================
echo.

REM Check if we're in the right directory
if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    echo Current directory: %CD%
    pause
    exit /b 1
)

REM Create handoff directory with timestamp
set TIMESTAMP=%DATE:~-4%-%DATE:~4,2%-%DATE:~7,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%
set TIMESTAMP=%TIMESTAMP: =0%
set HANDOFF_DIR=handoff_%TIMESTAMP%

echo Creating handoff package: %HANDOFF_DIR%
mkdir "%HANDOFF_DIR%"

echo.
echo === COLLECTING PROJECT STATE ===

REM Copy essential files
echo Copying CLAUDE.md...
copy "CLAUDE.md" "%HANDOFF_DIR%\"

echo Copying integration protocol...
if exist ".claude\ENHANCED_INTEGRATION_PROTOCOL.md" (
    copy ".claude\ENHANCED_INTEGRATION_PROTOCOL.md" "%HANDOFF_DIR%\"
)

echo Copying agent configurations...
mkdir "%HANDOFF_DIR%\.claude"
if exist ".claude\agents" (
    xcopy ".claude\agents" "%HANDOFF_DIR%\.claude\agents" /s /e /i
)

if exist ".claude\templates" (
    xcopy ".claude\templates" "%HANDOFF_DIR%\.claude\templates" /s /e /i
)

echo Copying settings...
if exist ".claude\settings.local.json" (
    copy ".claude\settings.local.json" "%HANDOFF_DIR%\.claude\"
)

echo.
echo === CAPTURING PROJECT STATUS ===

REM Git status
echo Capturing git status...
git status > "%HANDOFF_DIR%\git-status.txt"
git log --oneline -10 > "%HANDOFF_DIR%\recent-commits.txt"
git diff --name-status HEAD~1 > "%HANDOFF_DIR%\recent-changes.txt"

REM Build status
echo Checking build status...
if exist "build" (
    echo Build directory exists > "%HANDOFF_DIR%\build-status.txt"
    if exist "build\CMakeCache.txt" (
        echo CMake configured >> "%HANDOFF_DIR%\build-status.txt"
    ) else (
        echo CMake not configured >> "%HANDOFF_DIR%\build-status.txt"
    )
) else (
    echo No build directory found > "%HANDOFF_DIR%\build-status.txt"
)

REM Test status
echo Checking test status...
if exist "Tests" (
    dir "Tests\*.cpp" /b > "%HANDOFF_DIR%\available-tests.txt" 2>nul
) else (
    echo No Tests directory found > "%HANDOFF_DIR%\available-tests.txt"
)

echo.
echo === CREATING CONTEXT SUMMARY ===

REM Create quick context file
echo Creating QUICK_CONTEXT.md...
(
echo # Quick Context - SpectralCanvas Pro Handoff
echo Generated: %DATE% %TIME%
echo.
echo ## Project Status
echo - Repository: SpectralCanvas Pro audio plugin
echo - Framework: JUCE-based VST3/Standalone
echo - Current Branch: 
git branch --show-current
echo - Last Commit: 
git log --oneline -1
echo.
echo ## Architecture Overview
echo - Core DSP: RT-safe spectral synthesis engine
echo - Canvas UI: Visual paint-to-audio interface  
echo - Parameter System: APVTS-based automation
echo - Threading: Lock-free audio thread communication
echo.
echo ## Recent Activity
echo See recent-commits.txt and recent-changes.txt for details
echo.
echo ## Available Templates
echo - .claude/templates/feature-implementation.md
echo - .claude/templates/debugging-session.md
echo.
echo ## AI Collaboration  
echo - Enhanced Integration Protocol active
echo - analyzer-agent: Deep technical analysis
echo - builder-agent: Informed implementation
echo - validator-agent: Quality assurance
echo.
echo ## Next Steps
echo [TO BE FILLED BY USER]
echo - Current objective:
echo - Priority level:
echo - Specific requirements:
echo.
echo ## Constraints
echo - RT-Safety: No allocations in processBlock()
echo - Performance: ^<5%% regression without justification
echo - Quality: ^>90%% test coverage for new code
echo - Standards: Follow existing JUCE patterns
) > "%HANDOFF_DIR%\QUICK_CONTEXT.md"

echo.
echo === CREATING ARCHITECTURE SNAPSHOT ===

REM Create architecture overview
echo Creating architecture snapshot...
(
echo # Architecture Snapshot
echo Generated: %DATE% %TIME%
echo.
echo ## Core Files
echo ### Audio Processing
echo - Source/Core/PluginProcessor.h/cpp: Main plugin interface
echo - Source/Core/SpectralSynthEngine.h/cpp: Core synthesis
echo - Source/Core/EMUFilter.h/cpp: Audio filtering
echo.
echo ### UI Components  
echo - Source/GUI/PluginEditor.h/cpp: Main editor
echo - Source/CanvasComponent.h/cpp: Interactive canvas
echo.
echo ### Threading
echo - Lock-free PaintQueue: UI to audio communication
echo - Atomic parameter snapshots: RT-safe parameter access
echo.
echo ## Build System
echo - CMakeLists.txt: Main build configuration
echo - JUCE framework integration
echo - VST3 + Standalone targets
echo.
echo ## Testing
if exist "Tests" (
    echo ### Available Tests
    dir "Tests\*.cpp" /b 2>nul
) else (
    echo No test files found
)
echo.
echo ## Documentation
echo - CLAUDE.md: Main workflow document
echo - .claude/: AI collaboration configuration
echo - Enhanced Integration Protocol available
) > "%HANDOFF_DIR%\ARCHITECTURE_SNAPSHOT.md"

echo.
echo === HANDOFF PACKAGE COMPLETE ===
echo.
echo Package location: %HANDOFF_DIR%
echo.
echo Package contents:
dir "%HANDOFF_DIR%" /w
echo.
echo === USAGE INSTRUCTIONS ===
echo.
echo 1. Share the entire %HANDOFF_DIR% folder with the next AI
echo 2. Instruct them to read QUICK_CONTEXT.md first
echo 3. Provide specific objectives in the "Next Steps" section
echo 4. Reference appropriate mega-prompt templates from .claude/templates/
echo.
echo For the receiving AI:
echo - Start with QUICK_CONTEXT.md for immediate understanding
echo - Use ARCHITECTURE_SNAPSHOT.md for technical details
echo - Follow Enhanced Integration Protocol for best results
echo.
pause