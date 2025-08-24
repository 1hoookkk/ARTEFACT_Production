@echo off
REM SessionStart hook: Safe boot verification
REM Always exits 0 - informational only

echo [SessionStart] Safe boot verification starting...

echo.
echo === Environment Summary ===
echo Working Directory: %CD%

REM Check basic tools
where cmake >nul 2>&1 && echo ✓ CMake found || echo ✗ CMake not found
where python >nul 2>&1 && echo ✓ Python found || echo ✗ Python not found
where msbuild >nul 2>&1 && echo ✓ MSBuild found || echo ✗ MSBuild not found

echo.
echo === TDD Guard Status ===

REM Check for tdd_guard Python module
python -c "import tdd_guard" >nul 2>&1 && (
    echo ✓ tdd_guard module available
) || (
    echo ℹ tdd_guard module not found
    echo   To install: pip install tdd-guard
)

REM Check for config
if exist "tdd-guard.yaml" (
    echo ✓ tdd-guard.yaml found
) else (
    echo ℹ tdd-guard.yaml not found - will be created during bootstrap
)

echo.
echo === Test Infrastructure ===

if exist "Source\Tests" (
    echo ✓ Source\Tests directory exists
    dir /b "Source\Tests\*.cpp" 2>nul >nul && echo ✓ Test files found || echo ℹ No .cpp test files found
) else (
    echo ℹ Source\Tests directory not found
)

if exist "build" (
    echo ✓ Build directory exists
    ctest --test-dir build -N 2>nul | findstr /c:"Test #" >nul && (
        echo ✓ CTest discovers tests
    ) || (
        echo ℹ No tests discovered by CTest
    )
) else (
    echo ℹ Build directory not found
)

echo.
echo [SessionStart] Safe boot verification completed
echo ℹ Ready for TDD workflow: Red → Green → Refactor

REM Always exit 0 - never block session start
exit /b 0
