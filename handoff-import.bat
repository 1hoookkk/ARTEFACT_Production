@echo off
setlocal enabledelayedexpansion

:: SpectralCanvas Pro - AI Handoff Import System
:: Validates environment and displays context for receiving AI

echo ==========================================
echo  SpectralCanvas Pro - AI Handoff Import
echo ==========================================
echo.

set "HANDOFF_FILE=HANDOFF_PACKAGE.md"

:: Check if handoff package exists
if not exist "%HANDOFF_FILE%" (
    echo ❌ **ERROR: No handoff package found**
    echo 💡 Run 'handoff-export.bat' first to create package
    echo.
    pause
    exit /b 1
)

echo 📦 **Handoff package found:** %HANDOFF_FILE%
echo ⏰ **Package created:** 
for %%i in (%HANDOFF_FILE%) do echo    %%~ti
echo.

echo ==========================================
echo  🧪 ENVIRONMENT VALIDATION
echo ==========================================
echo.

:: Check git status
echo 🔍 **Checking git repository...**
git status >nul 2>&1
if %ERRORLEVEL%==0 (
    echo ✅ Git repository detected
) else (
    echo ❌ Git repository not found
    set /a ERRORS+=1
)

:: Check CMake build system
echo 🔍 **Checking build system...**
if exist "CMakeLists.txt" (
    echo ✅ CMakeLists.txt found
) else (
    echo ❌ CMakeLists.txt missing
    set /a ERRORS+=1
)

if exist "build\" (
    echo ✅ Build directory exists
) else (
    echo ⚠️  Build directory missing (will be created)
)

:: Check core project files
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

if exist "Source\Core\PluginProcessor.cpp" (
    echo ✅ Core source files present
) else (
    echo ❌ Core source files missing
    set /a ERRORS+=1
)

if exist "Source\Tests\" (
    echo ✅ Test suite directory found
) else (
    echo ❌ Test suite missing
    set /a ERRORS+=1
)

echo.

:: Validate environment by running tests
echo ==========================================
echo  ⚡ RUNNING VALIDATION TESTS
echo ==========================================
echo.

if %ERRORS% GTR 0 (
    echo ❌ **VALIDATION FAILED** - %ERRORS% critical errors detected
    echo 🛠️ Fix missing files before proceeding
    echo.
    goto :DISPLAY_CONTEXT
)

echo 🔨 **Building project...**
call scripts\build-tests.bat >nul 2>&1
set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT%==0 (
    echo ✅ Build successful
    
    echo 🧪 **Running test suite...**
    call scripts\run-tests.bat >nul 2>&1
    set TEST_RESULT=%ERRORLEVEL%
    
    if %TEST_RESULT%==0 (
        echo ✅ All tests passing
        echo.
        echo 🎉 **ENVIRONMENT VALIDATED** - Ready for AI handoff!
    ) else (
        echo ❌ Test failures detected
        echo ⚠️  Environment has issues but may still be usable
    )
) else (
    echo ❌ Build failed
    echo ⚠️  Environment has build issues
)

:DISPLAY_CONTEXT
echo.
echo ==========================================
echo  📋 HANDOFF CONTEXT
echo ==========================================
echo.

:: Display the handoff package content
type "%HANDOFF_FILE%"

echo.
echo ==========================================
echo  🚀 READY FOR NEXT PHASE
echo ==========================================
echo.

if %ERRORS% GTR 0 (
    echo ❌ **STATUS: BLOCKED** - Fix errors before proceeding
    echo 🛠️ Check missing files and dependencies
) else (
    if %BUILD_RESULT%==0 (
        if %TEST_RESULT%==0 (
            echo ✅ **STATUS: READY** - All systems GREEN for handoff
            echo 🎯 Environment validated and tests passing
        ) else (
            echo ⚠️  **STATUS: CAUTION** - Build OK but test issues
            echo 🔍 Review test results before proceeding
        )
    ) else (
        echo ⚠️  **STATUS: CAUTION** - Build issues detected
        echo 🔧 May need build system attention
    )
)

echo.
echo 📚 **Available Templates:**
if exist "workflow\templates\" (
    echo    - workflow\templates\chatgpt-concepts.md
    echo    - workflow\templates\gemini-architecture.md
    echo    - workflow\templates\claude-gpt-processing.md
    echo    - workflow\templates\claude-code-implementation.md
) else (
    echo    ⚠️  Templates not yet created (coming in next script)
)

echo.
echo 💡 **Quick Start for AIs:**
echo    1. Read the QUICK CONTEXT section above
echo    2. Reference PROJECT_SNAPSHOT.md for complete details
echo    3. Follow CLAUDE.md constitution for TDD methodology
echo    4. Choose appropriate template for your role
echo.
pause