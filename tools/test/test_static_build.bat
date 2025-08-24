@echo off
echo ========================================
echo Testing SpectralCanvas Static Build
echo ========================================
echo.

echo Testing static build...
echo.

REM First check if the file exists
if exist "build_static\SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe" (
    echo [OK] Static executable found
    echo.
    
    echo Checking file size...
    for %%F in ("build_static\SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe") do echo File size: %%~zF bytes
    echo.
    
    echo Attempting to launch...
    start "SpectralCanvas Pro Static" /wait "build_static\SpectralCanvasPro_artefacts\Release\SPECTRAL CANVAS PRO.exe"
    
    if %ERRORLEVEL% neq 0 (
        echo.
        echo [ERROR] Application exited with error code: %ERRORLEVEL%
    ) else (
        echo.
        echo [OK] Application exited normally
    )
) else (
    echo [ERROR] Static executable not found!
)

echo.
echo ========================================
echo Testing regular build for comparison...
echo ========================================
echo.

if exist "build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe" (
    echo [OK] Regular executable found
    echo.
    
    echo Checking file size...
    for %%F in ("build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe") do echo File size: %%~zF bytes
    echo.
    
    echo Attempting to launch...
    start "SpectralCanvas Regular" /wait "build\ARTEFACT_artefacts\Release\Standalone\SpectralCanvas.exe"
    
    if %ERRORLEVEL% neq 0 (
        echo.
        echo [ERROR] Application exited with error code: %ERRORLEVEL%
    ) else (
        echo.
        echo [OK] Application exited normally
    )
) else (
    echo [ERROR] Regular executable not found!
)

echo.
echo ========================================
echo Test complete. Check for any error messages above.
echo ========================================
pause