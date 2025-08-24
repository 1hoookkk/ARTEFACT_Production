@echo off
setlocal enabledelayedexpansion

:: SpectralCanvas Pro - Git Checkpoint System
:: Creates safe restore points with automatic state documentation

if "%~1"=="" (
    echo ❌ **ERROR: Checkpoint message required**
    echo.
    echo 💡 **Usage:** checkpoint.bat "Brief description of changes"
    echo.
    echo 📝 **Examples:**
    echo    checkpoint.bat "EMUFilter TPT SVF implementation complete"
    echo    checkpoint.bat "Added spectral brush parameter system"
    echo    checkpoint.bat "Fixed RT-safety issue in paint queue"
    echo.
    pause
    exit /b 1
)

set "CHECKPOINT_MSG=%~1"
set "TIMESTAMP=%DATE:~-4%-%DATE:~4,2%-%DATE:~7,2%_%TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%"
set "TIMESTAMP=%TIMESTAMP: =0%"
set "TAG_NAME=checkpoint-%TIMESTAMP%"

echo ==========================================
echo  SpectralCanvas Pro - Creating Checkpoint
echo ==========================================
echo.
echo 📝 **Message:** %CHECKPOINT_MSG%
echo ⏰ **Timestamp:** %TIMESTAMP%
echo 🏷️  **Tag:** %TAG_NAME%
echo.

:: Verify git repository
git status >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ❌ **ERROR: Not a git repository**
    echo 💡 Initialize git first with: git init
    echo.
    pause
    exit /b 1
)

:: Run pre-checkpoint validation
echo 🧪 **Running pre-checkpoint validation...**
echo.

:: Build test
echo 🔨 **Building project...**
call scripts\build-tests.bat >nul 2>&1
set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT%==0 (
    echo ✅ Build successful
) else (
    echo ❌ Build failed
    echo ⚠️  **WARNING:** Creating checkpoint with build issues
    echo 🔍 Run 'scripts\build-tests.bat' to see errors
)

:: Test validation
echo 🧪 **Running tests...**
call scripts\run-tests.bat >nul 2>&1
set TEST_RESULT=%ERRORLEVEL%

if %TEST_RESULT%==0 (
    echo ✅ All tests passing
) else (
    echo ❌ Test failures detected
    echo ⚠️  **WARNING:** Creating checkpoint with test issues
    echo 🔍 Run 'scripts\run-tests.bat' to see failures
)

echo.

:: Create checkpoint metadata
echo 📋 **Creating checkpoint metadata...**
set "CHECKPOINT_FILE=CHECKPOINT_%TIMESTAMP%.md"

(
echo # 🎯 CHECKPOINT: %CHECKPOINT_MSG%
echo **Created:** %DATE% %TIME%
echo **Tag:** %TAG_NAME%
echo **Branch:** 
git branch --show-current
echo.
echo ## 🧪 System Status
echo **Build Status:** 
if %BUILD_RESULT%==0 (
    echo ✅ SUCCESS - Clean compilation
) else (
    echo ❌ FAILED - Build errors detected
)
echo.
echo **Test Status:**
if %TEST_RESULT%==0 (
    echo ✅ SUCCESS - All tests passing
) else (
    echo ❌ FAILED - Test failures detected
)
echo.
echo ## 📁 Repository State
echo ```
git status --short
echo ```
echo.
echo ## 🔄 Recent Activity
echo ```
git log --oneline -3
echo ```
echo.
echo ## 📊 File Summary
echo **Modified Files:**
git diff --name-only HEAD~1..HEAD 2>nul
echo.
echo **Total Files in Repo:**
git ls-files ^| find /c /v ""
echo.
echo ## 🚀 Recovery Instructions
echo ```bash
echo # To restore this checkpoint:
echo git checkout %TAG_NAME%
echo.
echo # To see what changed since this checkpoint:  
echo git diff %TAG_NAME%..HEAD
echo.
echo # To create branch from this checkpoint:
echo git checkout -b feature-branch %TAG_NAME%
echo ```
echo.
echo ---
echo 🎨 *"Safe checkpoint created. Continue building the dream."*
echo **SpectralCanvas Pro Team**
) > "%CHECKPOINT_FILE%"

:: Stage all changes
echo 📦 **Staging all changes...**
git add .
if %ERRORLEVEL% NEQ 0 (
    echo ❌ **ERROR: Failed to stage files**
    echo.
    pause
    exit /b 1
)

:: Create commit
echo 💾 **Creating commit...**
git commit -m "CHECKPOINT: %CHECKPOINT_MSG%

🎯 Checkpoint created at %TIMESTAMP%
📋 Build Status: %BUILD_RESULT% | Test Status: %TEST_RESULT%
🏷️ Tag: %TAG_NAME%

🤖 Generated with [Claude Code](https://claude.ai/code)

Co-Authored-By: Claude <noreply@anthropic.com>"

if %ERRORLEVEL% NEQ 0 (
    echo ❌ **ERROR: Failed to create commit**
    echo 💡 Check git status and try again
    echo.
    pause
    exit /b 1
)

:: Create tag
echo 🏷️  **Creating tag...**
git tag -a "%TAG_NAME%" -m "CHECKPOINT: %CHECKPOINT_MSG%

Created: %TIMESTAMP%
Build: %BUILD_RESULT% | Tests: %TEST_RESULT%"

if %ERRORLEVEL% NEQ 0 (
    echo ⚠️  **WARNING: Failed to create tag**
    echo 💡 Tag creation failed but commit succeeded
) else (
    echo ✅ Tag created successfully
)

echo.
echo ==========================================
echo  ✅ CHECKPOINT CREATED SUCCESSFULLY
echo ==========================================
echo.

if %BUILD_RESULT%==0 (
    if %TEST_RESULT%==0 (
        echo 🎉 **STATUS: PERFECT** - All systems GREEN
        echo 🎯 Safe to continue development or handoff to next AI
    ) else (
        echo ⚠️  **STATUS: BUILD_OK** - Tests need attention
        echo 🔍 Fix test issues before major changes
    )
) else (
    echo ⚠️  **STATUS: ISSUES** - Build problems detected
    echo 🛠️ Address build issues before proceeding
)

echo.
echo 📋 **Checkpoint Details:**
echo    💾 Commit: Latest commit with all changes
echo    🏷️  Tag: %TAG_NAME%
echo    📄 Metadata: %CHECKPOINT_FILE%
echo.
echo 🚀 **Next Steps:**
echo    1. Continue development OR
echo    2. Run 'handoff-export.bat' to package for next AI OR  
echo    3. Use 'git checkout %TAG_NAME%' to restore this state
echo.
echo 💡 **Pro Tips:**
echo    - Use 'git tag' to see all checkpoints
echo    - Use 'git log --oneline' to see commit history
echo    - Checkpoint metadata saved to %CHECKPOINT_FILE%
echo.
pause