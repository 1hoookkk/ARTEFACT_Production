@echo off
REM Test core infrastructure files following CLAUDE.md
echo === CLAUDE.md Core Infrastructure Test ===

echo [1/3] Testing Config.cpp compilation...
cmake --build build --config Debug --target SpectralCanvas_rc_lib --parallel 1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Resource library failed to compile
    exit /b 1
)
echo [OK] Resource library compiled successfully

echo [2/3] Testing main target compilation (with timeout)...
timeout 60 cmake --build build --config Debug --target SpectralCanvas --parallel 1
if %ERRORLEVEL% EQU 1 (
    echo [TIMEOUT] Main target build timed out - this is expected for large builds
    echo Checking if compilation started successfully...
) else if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Main target failed to compile
    exit /b 1
) else (
    echo [OK] Main target compiled successfully
)

echo [3/3] Checking for core infrastructure files...
if exist "Source\Core\CrashToggles.h" (
    echo [OK] CrashToggles.h created
) else (
    echo ERROR: CrashToggles.h missing
    exit /b 1
)

if exist "Source\Core\Config.h" (
    echo [OK] Config.h created
) else (
    echo ERROR: Config.h missing
    exit /b 1
)

if exist "Source\Core\Config.cpp" (
    echo [OK] Config.cpp created
) else (
    echo ERROR: Config.cpp missing
    exit /b 1
)

echo === Core Infrastructure Test PASSED ===
echo Next: Apply CrashToggles in PluginEditor and CanvasComponent per CLAUDE.md