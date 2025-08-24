@echo off
REM Bootstrap-safe TDD Guard for Claude Code hooks
REM Handles the deadlock: allows test creation during bootstrap, enforces Red->Green after

setlocal enabledelayedexpansion

REM Parse hook data from stdin (Claude Code passes JSON)
set "hook_data="
for /f "delims=" %%i in ('type 2^>nul') do set "hook_data=!hook_data!%%i"

REM If no hook data, assume bootstrap mode - allow operation
if "!hook_data!"=="" (
    echo {"reason": "Bootstrap mode - no hook data"}
    exit /b 0
)

REM Check if this is a maker tool operation
echo !hook_data! | findstr /i "write\|edit\|multiedit" >nul
if errorlevel 1 (
    echo {"reason": "Non-maker tool - pass through"}
    exit /b 0
)

REM Check if tdd_guard Python module is available
python -c "import tdd_guard" >nul 2>&1
if errorlevel 1 (
    echo {"reason": "tdd_guard module not found - install with: pip install tdd-guard"}
    exit /b 1
)

REM Check if build directory exists and has discoverable tests
if not exist "build" (
    echo {"reason": "No build directory - run cmake first"}
    exit /b 1
)

REM Quick test discovery - look for SpectralSmoke or SpectralSynthEngineTest
ctest --test-dir build -N 2>nul | findstr /i "SpectralSmoke\|SpectralSynthEngineTest" >nul
if errorlevel 1 (
    echo {"reason": "No discoverable tests - configure tests first"}
    exit /b 1
)

REM If we get here, run actual TDD guard validation
if exist "tdd-guard.yaml" (
    python -m tdd_guard run --config tdd-guard.yaml
    exit /b !errorlevel!
) else (
    echo {"reason": "No tdd-guard.yaml config - pass through"}
    exit /b 0
)
