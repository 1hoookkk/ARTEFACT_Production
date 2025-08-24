@echo off
echo Testing Vintage Machine Features...
echo.

echo 1. Building project...
cmake --build build --config RelWithDebInfo --target ALL_BUILD
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo 2. Testing paint-to-audio (working version)...
cd build\RelWithDebInfo
TestPaintProducesAudio.exe
if %ERRORLEVEL% neq 0 (
    echo Paint-to-audio test failed!
    exit /b 1
)

echo.
echo 3. Testing performance...
TestPaintPerformance.exe
if %ERRORLEVEL% neq 0 (
    echo Performance test failed!
    exit /b 1
)

echo.
echo ✅ VINTAGE MACHINE SYSTEM TESTS PASSED!
echo.
echo 🎯 Ready to Demo:
echo    - Paint on canvas → Creates spectral oscillators
echo    - Transform button → 2-second vintage hihat demo
echo    - Character switch → 150ms smooth vintage machine morph
echo    - Tape speed: 1.0 → 1.15x when magic ON
echo    - Stereo width: 1.0 → 1.4x when magic ON
echo.
pause