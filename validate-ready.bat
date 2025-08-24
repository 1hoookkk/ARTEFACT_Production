@echo off
setlocal enabledelayedexpansion

:: SpectralCanvas Pro - Pre-Handoff Validation System
:: Comprehensive check before passing to next AI in pipeline

echo ==========================================
echo  SpectralCanvas Pro - Pre-Handoff Validation
echo ==========================================
echo.

set /a ERRORS=0
set /a WARNINGS=0

:: Check 1: Core Project Files
echo 🔍 **Checking core project files...**
if exist "PROJECT_SNAPSHOT.md" (
    echo ✅ PROJECT_SNAPSHOT.md found
) else (
    echo ❌ PROJECT_SNAPSHOT.md missing - CRITICAL
    set /a ERRORS+=1
)

if exist "CLAUDE.md" (
    echo ✅ CLAUDE.md constitution found  
) else (
    echo ❌ CLAUDE.md missing - CRITICAL
    set /a ERRORS+=1
)

if exist "CMakeLists.txt" (
    echo ✅ CMakeLists.txt found
) else (
    echo ❌ CMakeLists.txt missing - CRITICAL
    set /a ERRORS+=1
)

if exist "Source\Core\PluginProcessor.cpp" (
    echo ✅ Core source files present
) else (
    echo ❌ Core source files missing - CRITICAL
    set /a ERRORS+=1
)

echo.

:: Check 2: Git Repository Health
echo 🔍 **Checking git repository...**
git status >nul 2>&1
if %ERRORLEVEL%==0 (
    echo ✅ Git repository operational
    
    :: Check for uncommitted changes
    git diff --quiet
    if %ERRORLEVEL%==0 (
        git diff --cached --quiet
        if %ERRORLEVEL%==0 (
            echo ✅ No uncommitted changes
        ) else (
            echo ⚠️  Staged changes present
            set /a WARNINGS+=1
        )
    ) else (
        echo ⚠️  Uncommitted changes present
        set /a WARNINGS+=1
    )
    
    :: Check current branch
    for /f %%i in ('git branch --show-current') do set CURRENT_BRANCH=%%i
    echo 📍 Current branch: !CURRENT_BRANCH!
    
) else (
    echo ❌ Git repository not found - CRITICAL
    set /a ERRORS+=1
)

echo.

:: Check 3: Build System
echo 🔍 **Checking build system...**
if exist "build\" (
    echo ✅ Build directory exists
) else (
    echo ⚠️  Build directory missing (will be created)
    set /a WARNINGS+=1
)

if exist "scripts\build-tests.bat" (
    echo ✅ Build scripts present
) else (
    echo ❌ Build scripts missing
    set /a ERRORS+=1
)

if exist "scripts\run-tests.bat" (
    echo ✅ Test scripts present
) else (
    echo ❌ Test scripts missing  
    set /a ERRORS+=1
)

echo.

:: Check 4: Test Infrastructure
echo 🔍 **Checking test infrastructure...**
if exist "Source\Tests\" (
    echo ✅ Test directory found
    
    :: Count test files
    set /a TEST_COUNT=0
    for %%i in (Source\Tests\*.cpp) do set /a TEST_COUNT+=1
    echo 📊 Test files found: !TEST_COUNT!
    
    if !TEST_COUNT! GEQ 5 (
        echo ✅ Adequate test coverage
    ) else (
        echo ⚠️  Limited test coverage
        set /a WARNINGS+=1
    )
) else (
    echo ❌ Test directory missing
    set /a ERRORS+=1
)

echo.

:: Check 5: Workflow Infrastructure
echo 🔍 **Checking workflow infrastructure...**
if exist "handoff-export.bat" (
    echo ✅ Handoff export script present
) else (
    echo ⚠️  Handoff export script missing
    set /a WARNINGS+=1
)

if exist "handoff-import.bat" (
    echo ✅ Handoff import script present
) else (
    echo ⚠️  Handoff import script missing
    set /a WARNINGS+=1
)

if exist "workflow\templates\" (
    echo ✅ AI templates directory found
    
    set /a TEMPLATE_COUNT=0
    for %%i in (workflow\templates\*.md) do set /a TEMPLATE_COUNT+=1
    echo 📊 AI templates found: !TEMPLATE_COUNT!
    
    if !TEMPLATE_COUNT! GEQ 4 (
        echo ✅ Complete template set
    ) else (
        echo ⚠️  Incomplete template set
        set /a WARNINGS+=1
    )
) else (
    echo ⚠️  AI templates missing
    set /a WARNINGS+=1
)

echo.

:: Check 6: Build Validation
if %ERRORS%==0 (
    echo 🔨 **Testing build system...**
    call scripts\build-tests.bat >nul 2>&1
    set BUILD_RESULT=%ERRORLEVEL%
    
    if %BUILD_RESULT%==0 (
        echo ✅ Build successful
        
        echo 🧪 **Testing test suite...**
        call scripts\run-tests.bat >nul 2>&1
        set TEST_RESULT=%ERRORLEVEL%
        
        if %TEST_RESULT%==0 (
            echo ✅ All tests passing
        ) else (
            echo ❌ Test failures detected
            set /a ERRORS+=1
        )
    ) else (
        echo ❌ Build failed
        set /a ERRORS+=1
    )
) else (
    echo ⏭️  **Skipping build validation** (critical errors present)
    set BUILD_RESULT=-1
    set TEST_RESULT=-1
)

echo.

:: Check 7: Documentation Currency
echo 🔍 **Checking documentation currency...**
if exist "PROJECT_SNAPSHOT.md" (
    :: Check if PROJECT_SNAPSHOT.md is recent (modified in last 7 days)
    forfiles /m PROJECT_SNAPSHOT.md /d -7 >nul 2>&1
    if %ERRORLEVEL%==0 (
        echo ✅ PROJECT_SNAPSHOT.md recently updated
    ) else (
        echo ⚠️  PROJECT_SNAPSHOT.md may be outdated
        set /a WARNINGS+=1
    )
)

echo.

:: Generate Status Report
echo ==========================================
echo  📋 VALIDATION SUMMARY
echo ==========================================
echo.

if %ERRORS%==0 (
    if %WARNINGS%==0 (
        echo 🎉 **STATUS: READY FOR HANDOFF**
        echo ✅ All systems GREEN - Perfect handoff conditions
        echo.
        echo 🚀 **Recommended Actions:**
        echo    1. Run 'handoff-export.bat' to create package
        echo    2. Copy QUICK CONTEXT to next AI
        echo    3. Reference appropriate template from workflow\templates\
    ) else (
        echo ⚠️  **STATUS: READY WITH WARNINGS**
        echo ✅ Core systems functional but minor issues detected
        echo ⚠️  Warnings: %WARNINGS%
        echo.
        echo 🚀 **Recommended Actions:**
        echo    1. Review warnings above (optional to fix)
        echo    2. Run 'handoff-export.bat' to create package
        echo    3. Proceed with handoff - warnings are non-blocking
    )
) else (
    echo ❌ **STATUS: NOT READY**
    echo 🛑 Critical errors detected: %ERRORS%
    echo ⚠️  Warnings: %WARNINGS%
    echo.
    echo 🛠️ **Required Actions:**
    echo    1. Fix all critical errors listed above
    echo    2. Re-run 'validate-ready.bat' to confirm fixes
    echo    3. Only proceed after achieving GREEN status
)

echo.
echo 📊 **System Health Metrics:**
echo    🔥 Critical Errors: %ERRORS%
echo    ⚠️  Warnings: %WARNINGS%
if %ERRORS%==0 (
    if %BUILD_RESULT%==0 (
        echo    🔨 Build Status: ✅ SUCCESS
    ) else (
        echo    🔨 Build Status: ❌ FAILED
    )
    
    if %TEST_RESULT%==0 (
        echo    🧪 Test Status: ✅ ALL PASSING
    ) else (
        echo    🧪 Test Status: ❌ FAILURES
    )
) else (
    echo    🔨 Build Status: ⏭️  SKIPPED
    echo    🧪 Test Status: ⏭️  SKIPPED
)

echo.
echo 💡 **Next Steps Based on Status:**
if %ERRORS%==0 (
    echo    ✅ Ready to run: handoff-export.bat
    echo    📋 Ready to use: workflow\templates\[ai-role].md
    echo    🎯 Ready for: AI pipeline handoff
) else (
    echo    🔧 Fix critical errors first
    echo    ♻️  Re-run: validate-ready.bat
    echo    🚫 Do not proceed with handoff
)

echo.
pause