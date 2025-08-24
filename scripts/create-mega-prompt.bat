@echo off
REM Mega-Prompt Creation Script for SpectralCanvas Pro
REM Helps create comprehensive context packages for AI collaboration

echo =======================================
echo Mega-Prompt Creator - SpectralCanvas Pro
echo =======================================
echo.

if not exist "CLAUDE.md" (
    echo ERROR: Must run from SpectralCanvas Pro root directory
    pause
    exit /b 1
)

REM Create output directory
if not exist "prompts" mkdir "prompts"

echo Select the type of mega-prompt to create:
echo.
echo 1. Feature Implementation
echo 2. Debugging Session  
echo 3. Architecture Evolution
echo 4. Custom Template
echo.
set /p CHOICE="Enter choice (1-4): "

set TEMPLATE_FILE=""
set OUTPUT_FILE=""
set PROMPT_TYPE=""

if "%CHOICE%"=="1" (
    set TEMPLATE_FILE=".claude\templates\feature-implementation.md"
    set OUTPUT_FILE="prompts\feature-prompt-%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.md"
    set PROMPT_TYPE="Feature Implementation"
)

if "%CHOICE%"=="2" (
    set TEMPLATE_FILE=".claude\templates\debugging-session.md"
    set OUTPUT_FILE="prompts\debug-prompt-%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.md"
    set PROMPT_TYPE="Debugging Session"
)

if "%CHOICE%"=="3" (
    set TEMPLATE_FILE=".claude\templates\architecture-evolution.md"
    set OUTPUT_FILE="prompts\architecture-prompt-%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.md"
    set PROMPT_TYPE="Architecture Evolution"
)

if "%CHOICE%"=="4" (
    echo.
    echo Available templates:
    dir ".claude\templates\*.md" /b 2>nul
    echo.
    set /p CUSTOM_TEMPLATE="Enter template filename: "
    set TEMPLATE_FILE=".claude\templates\%CUSTOM_TEMPLATE%"
    set OUTPUT_FILE="prompts\custom-prompt-%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.md"
    set PROMPT_TYPE="Custom Prompt"
)

if %TEMPLATE_FILE%=="" (
    echo Invalid choice. Exiting.
    pause
    exit /b 1
)

if not exist %TEMPLATE_FILE% (
    echo ERROR: Template file %TEMPLATE_FILE% not found
    echo.
    echo Make sure you have the enhanced integration protocol set up.
    echo Run scripts\validate-ai-context.bat to check your setup.
    pause
    exit /b 1
)

echo.
echo === CREATING %PROMPT_TYPE% ===
echo Template: %TEMPLATE_FILE%
echo Output: %OUTPUT_FILE%
echo.

REM Copy template to output file
copy %TEMPLATE_FILE% %OUTPUT_FILE% >nul

echo Template copied successfully.
echo.

REM Add current project context
echo === ADDING PROJECT CONTEXT ===

REM Create context header
set CONTEXT_FILE="prompts\current-context.tmp"
(
echo # CURRENT PROJECT CONTEXT
echo Generated: %DATE% %TIME%
echo.
echo ## Git Status
) > %CONTEXT_FILE%

git status >> %CONTEXT_FILE%
echo. >> %CONTEXT_FILE%

echo ## Recent Commits >> %CONTEXT_FILE%
git log --oneline -5 >> %CONTEXT_FILE%
echo. >> %CONTEXT_FILE%

echo ## Build Status >> %CONTEXT_FILE%
if exist "build\CMakeCache.txt" (
    echo Build configured: YES >> %CONTEXT_FILE%
) else (
    echo Build configured: NO >> %CONTEXT_FILE%
)
echo. >> %CONTEXT_FILE%

echo ## Available Tests >> %CONTEXT_FILE%
if exist "Tests" (
    dir "Tests\*.cpp" /b >> %CONTEXT_FILE% 2>nul
) else (
    echo No test files found >> %CONTEXT_FILE%
)
echo. >> %CONTEXT_FILE%

REM Append context to prompt file
echo. >> %OUTPUT_FILE%
echo --- >> %OUTPUT_FILE%
echo. >> %OUTPUT_FILE%
type %CONTEXT_FILE% >> %OUTPUT_FILE%

del %CONTEXT_FILE%

echo.
echo === MEGA-PROMPT CREATION COMPLETE ===
echo.
echo File created: %OUTPUT_FILE%
echo.
echo NEXT STEPS:
echo 1. Open %OUTPUT_FILE% in your editor
echo 2. Replace all [REPLACE: ...] placeholders with specific details
echo 3. Customize the specifications for your exact needs
echo 4. Copy the completed prompt to Claude Code
echo.
echo TIPS FOR EFFECTIVE MEGA-PROMPTS:
echo - Be specific about success criteria
echo - Include concrete examples where possible
echo - Specify performance and quality requirements
echo - Define scope boundaries clearly
echo - Include rollback procedures
echo.

REM Offer to open the file
set /p OPEN="Open the file now? (y/n): "
if /i "%OPEN%"=="y" (
    start notepad %OUTPUT_FILE%
)

echo.
echo Remember: The more complete context you provide, the better
echo the AI can make intelligent architectural decisions!
echo.
pause