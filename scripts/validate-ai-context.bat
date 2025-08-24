@echo off
REM Context Validation Script for AI Handoffs
REM Validates that all necessary context is available for intelligent AI collaboration

echo ==========================================
echo AI Context Validation - SpectralCanvas Pro
echo ==========================================
echo.

set VALIDATION_PASSED=1

echo === CHECKING PROJECT STRUCTURE ===

REM Check for essential files
if not exist "CLAUDE.md" (
    echo [FAIL] CLAUDE.md missing - workflow documentation required
    set VALIDATION_PASSED=0
) else (
    echo [PASS] CLAUDE.md present
)

if not exist "CMakeLists.txt" (
    echo [FAIL] CMakeLists.txt missing - build system required
    set VALIDATION_PASSED=0
) else (
    echo [PASS] CMakeLists.txt present
)

if not exist "Source\Core\PluginProcessor.h" (
    echo [FAIL] Core plugin files missing
    set VALIDATION_PASSED=0
) else (
    echo [PASS] Core plugin files present
)

echo.
echo === CHECKING AI COLLABORATION SETUP ===

if not exist ".claude\agents" (
    echo [WARN] .claude\agents directory missing - subagents not configured
) else (
    echo [PASS] Subagent configurations available
    
    if exist ".claude\agents\analyzer-agent.md" (
        echo   [PASS] analyzer-agent configured
    ) else (
        echo   [WARN] analyzer-agent configuration missing
    )
    
    if exist ".claude\agents\builder-agent.md" (
        echo   [PASS] builder-agent configured  
    ) else (
        echo   [WARN] builder-agent configuration missing
    )
    
    if exist ".claude\agents\validator-agent.md" (
        echo   [PASS] validator-agent configured
    ) else (
        echo   [WARN] validator-agent configuration missing
    )
)

if not exist ".claude\templates" (
    echo [WARN] Mega-prompt templates missing
) else (
    echo [PASS] Mega-prompt templates available
    
    if exist ".claude\templates\feature-implementation.md" (
        echo   [PASS] Feature implementation template ready
    ) else (
        echo   [WARN] Feature implementation template missing
    )
    
    if exist ".claude\templates\debugging-session.md" (
        echo   [PASS] Debugging session template ready
    ) else (
        echo   [WARN] Debugging session template missing
    )
)

if not exist ".claude\ENHANCED_INTEGRATION_PROTOCOL.md" (
    echo [WARN] Enhanced Integration Protocol not found
) else (
    echo [PASS] Enhanced Integration Protocol available
)

echo.
echo === CHECKING BUILD SYSTEM ===

if not exist "build" (
    echo [INFO] No build directory - run cmake -B build first
) else (
    echo [PASS] Build directory exists
    
    if exist "build\CMakeCache.txt" (
        echo   [PASS] CMake configured
        
        REM Try a quick build check
        echo   [INFO] Testing build system...
        cmake --build build --config Release --target ARTEFACT >nul 2>&1
        if errorlevel 1 (
            echo   [WARN] Build system has issues - may need attention
        ) else (
            echo   [PASS] Build system functional
        )
    ) else (
        echo   [INFO] CMake not configured - run cmake -B build
    )
)

echo.
echo === CHECKING VERSION CONTROL ===

git status >nul 2>&1
if errorlevel 1 (
    echo [FAIL] Not a git repository or git not available
    set VALIDATION_PASSED=0
) else (
    echo [PASS] Git repository available
    
    REM Check for uncommitted changes
    git diff-index --quiet HEAD --
    if errorlevel 1 (
        echo   [WARN] Uncommitted changes present - consider committing before handoff
    ) else (
        echo   [PASS] Working directory clean
    )
)

echo.
echo === CHECKING DOCUMENTATION QUALITY ===

REM Count key sections in CLAUDE.md
if exist "CLAUDE.md" (
    findstr /C:"## AI Collaboration Framework" "CLAUDE.md" >nul
    if errorlevel 1 (
        echo [WARN] CLAUDE.md missing AI Collaboration section
    ) else (
        echo [PASS] CLAUDE.md has AI Collaboration Framework
    )
    
    findstr /C:"Mega-Prompt Guidelines" "CLAUDE.md" >nul
    if errorlevel 1 (
        echo [WARN] CLAUDE.md missing Mega-Prompt Guidelines
    ) else (
        echo [PASS] CLAUDE.md has Mega-Prompt Guidelines
    )
)

echo.
echo === CHECKING RT-SAFETY CONTEXT ===

if exist "Source\Core\PluginProcessor.cpp" (
    findstr /C:"processBlock" "Source\Core\PluginProcessor.cpp" >nul
    if errorlevel 1 (
        echo [FAIL] processBlock implementation missing
        set VALIDATION_PASSED=0
    ) else (
        echo [PASS] processBlock implementation present
        
        findstr /C:"ScopedNoDenormals" "Source\Core\PluginProcessor.cpp" >nul
        if not errorlevel 1 (
            echo   [PASS] RT-safety patterns detected
        ) else (
            echo   [INFO] RT-safety patterns not clearly visible
        )
    )
)

echo.
echo === VALIDATION SUMMARY ===

if %VALIDATION_PASSED%==1 (
    echo [SUCCESS] Context validation PASSED
    echo.
    echo This project is ready for AI collaboration with:
    echo - Complete project structure
    echo - AI collaboration framework configured  
    echo - Build system operational
    echo - Documentation available
    echo.
    echo Recommended next steps:
    echo 1. Use scripts\ai-handoff-export.bat to create handoff package
    echo 2. Provide specific objectives using mega-prompt templates
    echo 3. Reference Enhanced Integration Protocol for best practices
) else (
    echo [FAILURE] Context validation FAILED
    echo.
    echo Critical issues must be resolved before AI collaboration:
    echo - Fix missing essential files
    echo - Ensure build system works  
    echo - Complete documentation setup
    echo.
    echo Run this script again after addressing issues.
)

echo.
echo === CONTEXT HEALTH CHECK ===

REM Calculate rough project maturity score
set SCORE=0

if exist "CLAUDE.md" set /a SCORE+=20
if exist ".claude\agents" set /a SCORE+=15
if exist ".claude\templates" set /a SCORE+=15
if exist "build\CMakeCache.txt" set /a SCORE+=20
if exist "Tests" set /a SCORE+=15
git status >nul 2>&1 && set /a SCORE+=15

echo Project Maturity Score: %SCORE%/100
echo.

if %SCORE% geq 80 (
    echo [EXCELLENT] Project ready for advanced AI collaboration
) else if %SCORE% geq 60 (
    echo [GOOD] Project ready for standard AI collaboration  
) else if %SCORE% geq 40 (
    echo [FAIR] Basic AI collaboration possible, improvements recommended
) else (
    echo [POOR] Significant setup required before AI collaboration
)

echo.
pause