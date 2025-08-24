@echo off
echo ========================================
echo SpectralCanvas Foundation Fix Test Build
echo ========================================
echo.
echo This will test the foundation fixes:
echo - Emergency audio bypass (direct sine wave)
echo - Oscillator search limits (32 max instead of 1024)
echo - UI thread separation (no synthesis from paint)
echo.

echo Forcing complete clean rebuild...
if exist build rmdir /s /q build
if exist build_static rmdir /s /q build_static
if exist build_foundation rmdir /s /q build_foundation

echo Creating fresh build directory...
mkdir build_foundation
cd build_foundation

echo.
echo Configuring CMake...
cmake .. -G "Visual Studio 17 2022" -A x64

echo.
echo Building Release configuration...
cmake --build . --config Release --target SpectralCanvasPro

echo.
echo ========================================
echo Build complete!
echo.

if exist "SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe" (
    echo [SUCCESS] Foundation fixes executable built successfully!
    echo Location: build_foundation\SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe
    echo.
    echo ========================================
    echo Testing foundation fixes...
    echo.
    
    echo Launching SpectralCanvasPro with foundation fixes...
    start "SpectralCanvas Pro - Foundation Test" "SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe"
    
    echo.
    echo PLEASE TEST:
    echo 1. Click TEST AUDIO button - should hear 440Hz sine wave
    echo 2. Try painting on canvas - should NOT freeze UI
    echo 3. All buttons should work without blocking
    echo.
    
) else if exist "bin\Release\SpectralCanvasPro.exe" (
    echo [SUCCESS] Foundation fixes executable built successfully!
    echo Location: build_foundation\bin\Release\SpectralCanvasPro.exe
) else (
    echo [ERROR] No executable found! Build may have failed.
    echo Check for compilation errors above.
)

cd ..
echo.
echo ========================================
echo Foundation fix test complete!
echo.
echo Expected improvements:
echo - TEST AUDIO button works (440Hz sine wave)
echo - Paint strokes create visual feedback instantly
echo - UI remains responsive during all operations
echo - No freezing when painting
echo ========================================
pause