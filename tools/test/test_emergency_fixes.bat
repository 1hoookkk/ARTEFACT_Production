@echo off
echo ========================================
echo SpectralCanvas Emergency Fix Test Build
echo ========================================
echo.
echo This will test the emergency crash fixes:
echo - Quality Guardian disabled
echo - AtomicOscillator static initialization fixed
echo - Defensive null checks added
echo.

echo Cleaning previous builds...
if exist build rmdir /s /q build
mkdir build
cd build

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
    echo [SUCCESS] SpectralCanvasPro executable built successfully!
    echo Location: build\SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe
) else if exist "bin\Release\SpectralCanvasPro.exe" (
    echo [SUCCESS] SpectralCanvasPro executable built successfully!
    echo Location: build\bin\Release\SpectralCanvasPro.exe
) else if exist "ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe" (
    echo [SUCCESS] ARTEFACT Standalone executable built successfully!
    echo Location: build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe
) else (
    echo [ERROR] No executable found! Build may have failed.
)

echo.
echo ========================================
echo Testing the executable...
echo.

if exist "SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe" (
    echo Launching SpectralCanvasPro...
    start "SpectralCanvas Pro - Emergency Fix Test" /wait "SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe"
    echo.
    echo Exit code: %ERRORLEVEL%
    if %ERRORLEVEL% neq 0 (
        echo [WARNING] Application exited with error code: %ERRORLEVEL%
    ) else (
        echo [SUCCESS] Application exited normally!
    )
) else (
    echo [SKIP] Cannot test - executable not found
)

cd ..
echo.
echo ========================================
echo Emergency fix test complete!
echo.
echo If the application still crashes:
echo 1. Check Windows Event Viewer for crash details
echo 2. Run with debugger attached
echo 3. Check spectral_debug.log for error messages
echo ========================================
pause