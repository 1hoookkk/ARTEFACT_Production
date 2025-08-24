@echo off
setlocal enabledelayedexpansion

:: SpectralCanvas Pro - PROJECT_SNAPSHOT.md Auto-Update System
:: Keeps project documentation current with automatic status updates

if "%~1"=="" (
    echo ==========================================
    echo  SpectralCanvas Pro - Snapshot Updater
    echo ==========================================
    echo.
    echo 📝 **Usage:** update-snapshot.bat [section] "Update message"
    echo.
    echo 🎯 **Available Sections:**
    echo    current-state    - Update current achievement status
    echo    test-status      - Update test suite results
    echo    next-phase      - Update next development target
    echo    architecture    - Update architecture overview
    echo    roadmap         - Update strategic roadmap
    echo    full-update     - Comprehensive update of all sections
    echo.
    echo 💡 **Examples:**
    echo    update-snapshot.bat current-state "EMUFilter TPT SVF implementation complete"
    echo    update-snapshot.bat test-status "6/6 tests passing including EMUFilter FFT"
    echo    update-snapshot.bat next-phase "Ready for Spectral Brushes development"
    echo    update-snapshot.bat full-update "Major milestone: Core + Filter complete"
    echo.
    pause
    exit /b 1
)

set "SECTION=%~1"
set "UPDATE_MSG=%~2"
set "TIMESTAMP=%DATE% %TIME%"

if "%UPDATE_MSG%"=="" (
    echo ❌ **ERROR: Update message required**
    echo 💡 Use quotes around your message: "Your update here"
    echo.
    pause
    exit /b 1
)

echo ==========================================
echo  📋 Updating PROJECT_SNAPSHOT.md
echo ==========================================
echo.
echo 🎯 **Section:** %SECTION%
echo 📝 **Message:** %UPDATE_MSG%
echo ⏰ **Timestamp:** %TIMESTAMP%
echo.

:: Verify PROJECT_SNAPSHOT.md exists
if not exist "PROJECT_SNAPSHOT.md" (
    echo ❌ **ERROR: PROJECT_SNAPSHOT.md not found**
    echo 💡 This file is critical for project continuity
    echo.
    pause
    exit /b 1
)

:: Create backup
echo 💾 **Creating backup...**
copy "PROJECT_SNAPSHOT.md" "PROJECT_SNAPSHOT.md.backup" >nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ **ERROR: Failed to create backup**
    echo.
    pause
    exit /b 1
)
echo ✅ Backup created: PROJECT_SNAPSHOT.md.backup

:: Get current system status
echo 🧪 **Gathering current system status...**

:: Test status
call scripts\run-tests.bat >temp_test_output.txt 2>&1
set TEST_RESULT=%ERRORLEVEL%

if %TEST_RESULT%==0 (
    set "TEST_STATUS=✅ ALL PASSING"
    
    :: Count tests by looking for specific patterns
    set /a TEST_COUNT=0
    findstr /c:"✓" temp_test_output.txt >nul && set /a TEST_COUNT+=1
    findstr /c:"PASSED" temp_test_output.txt >nul && set /a TEST_COUNT+=1
    
    set "TEST_SUMMARY=All tests passing"
) else (
    set "TEST_STATUS=❌ FAILURES"
    set "TEST_SUMMARY=Test failures detected"
)

:: Git status
git status --short > temp_git_status.txt 2>&1
set GIT_RESULT=%ERRORLEVEL%

:: Current branch
for /f %%i in ('git branch --show-current 2^>nul') do set CURRENT_BRANCH=%%i
if "%CURRENT_BRANCH%"=="" set CURRENT_BRANCH=unknown

:: Recent commits
git log --oneline -3 > temp_recent_commits.txt 2>&1

:: Build status
call scripts\build-tests.bat >nul 2>&1
set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT%==0 (
    set "BUILD_STATUS=🟢 GREEN"
) else (
    set "BUILD_STATUS=🔴 RED"
)

echo ✅ System status gathered

:: Process updates based on section
echo 🔄 **Processing %SECTION% update...**

if /i "%SECTION%"=="current-state" goto UPDATE_CURRENT_STATE
if /i "%SECTION%"=="test-status" goto UPDATE_TEST_STATUS  
if /i "%SECTION%"=="next-phase" goto UPDATE_NEXT_PHASE
if /i "%SECTION%"=="architecture" goto UPDATE_ARCHITECTURE
if /i "%SECTION%"=="roadmap" goto UPDATE_ROADMAP
if /i "%SECTION%"=="full-update" goto UPDATE_FULL
goto UNKNOWN_SECTION

:UPDATE_CURRENT_STATE
echo 📝 Updating current achievement status...
:: This would use a more sophisticated text replacement tool in production
:: For now, we'll append a status update
(
echo.
echo ### 🔄 Status Update - %TIMESTAMP%
echo **Achievement:** %UPDATE_MSG%
echo **Build Status:** %BUILD_STATUS%
echo **Test Status:** %TEST_STATUS%
echo **Branch:** %CURRENT_BRANCH%
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UPDATE_TEST_STATUS
echo 📝 Updating test suite status...
(
echo.
echo ### 🧪 Test Status Update - %TIMESTAMP%
echo **Status:** %UPDATE_MSG%
echo **Result:** %TEST_STATUS%
echo **Details:** %TEST_SUMMARY%
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UPDATE_NEXT_PHASE
echo 📝 Updating next development target...
(
echo.
echo ### 🚀 Next Phase Update - %TIMESTAMP%
echo **Target:** %UPDATE_MSG%
echo **Ready Since:** %TIMESTAMP%
echo **Prerequisites:** %BUILD_STATUS% build, %TEST_STATUS% tests
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UPDATE_ARCHITECTURE
echo 📝 Updating architecture overview...
(
echo.
echo ### 🏗️ Architecture Update - %TIMESTAMP%
echo **Change:** %UPDATE_MSG%
echo **Impact:** [To be documented]
echo **Verification:** %TEST_STATUS%
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UPDATE_ROADMAP
echo 📝 Updating strategic roadmap...
(
echo.
echo ### 🎯 Roadmap Update - %TIMESTAMP%
echo **Milestone:** %UPDATE_MSG%
echo **Achieved:** %TIMESTAMP%
echo **Next Focus:** [To be determined]
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UPDATE_FULL
echo 📝 Performing comprehensive update...
(
echo.
echo ==========================================
echo ## 🔄 COMPREHENSIVE STATUS UPDATE
echo **Updated:** %TIMESTAMP%
echo ==========================================
echo.
echo ### 📊 System Status
echo **Build:** %BUILD_STATUS%
echo **Tests:** %TEST_STATUS%
echo **Branch:** %CURRENT_BRANCH%
echo **Last Update:** %UPDATE_MSG%
echo.
echo ### 📁 Repository State
echo ```
type temp_git_status.txt
echo ```
echo.
echo ### 🔄 Recent Activity
echo ```
type temp_recent_commits.txt
echo ```
echo.
echo ### 🎯 Current Focus
echo %UPDATE_MSG%
echo.
echo **Next Steps:** Ready for continued development or AI handoff
echo.
) >> PROJECT_SNAPSHOT.md
goto UPDATE_SUCCESS

:UNKNOWN_SECTION
echo ❌ **ERROR: Unknown section '%SECTION%'**
echo 💡 Use one of: current-state, test-status, next-phase, architecture, roadmap, full-update
goto CLEANUP_AND_EXIT

:UPDATE_SUCCESS
echo ✅ **PROJECT_SNAPSHOT.md updated successfully**
echo.

:: Optional: Commit the update
echo 💾 **Auto-commit update? (y/n)**
set /p COMMIT_CHOICE=
if /i "%COMMIT_CHOICE%"=="y" (
    git add PROJECT_SNAPSHOT.md
    git commit -m "docs: update PROJECT_SNAPSHOT.md - %SECTION%

%UPDATE_MSG%

Updated: %TIMESTAMP%
Build: %BUILD_STATUS% | Tests: %TEST_STATUS%

🤖 Generated with [Claude Code](https://claude.ai/code)

Co-Authored-By: Claude <noreply@anthropic.com>"
    
    if %ERRORLEVEL%==0 (
        echo ✅ Changes committed to git
    ) else (
        echo ⚠️  Commit failed - changes saved but not committed
    )
) else (
    echo 📝 Changes saved but not committed
)

echo.
echo 🎉 **Snapshot update complete**
echo 📋 **Updated section:** %SECTION%
echo 🔍 **View changes:** git diff HEAD~1 PROJECT_SNAPSHOT.md
echo 📄 **Backup available:** PROJECT_SNAPSHOT.md.backup
echo.

:CLEANUP_AND_EXIT
:: Clean up temp files
del temp_test_output.txt temp_git_status.txt temp_recent_commits.txt 2>nul

echo 💡 **Pro Tips:**
echo    - Use 'full-update' for major milestones
echo    - Commit snapshot updates for version tracking  
echo    - Backup is always created automatically
echo.
pause