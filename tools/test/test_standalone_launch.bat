@echo off
echo Testing SpectralCanvas Pro Standalone Launch...
echo.

set EXE_PATH=build\SpectralCanvas_artefacts\RelWithDebInfo\Standalone\SpectralCanvas Pro.exe

if exist "%EXE_PATH%" (
    echo Found executable at: %EXE_PATH%
    echo.
    echo Starting application...
    start "" "%EXE_PATH%"
    timeout /t 3 /nobreak >nul
    
    REM Check if process is running
    tasklist /FI "IMAGENAME eq SpectralCanvas Pro.exe" 2>NUL | find /I /N "SpectralCanvas Pro.exe">NUL
    if "%ERRORLEVEL%"=="0" (
        echo SUCCESS: SpectralCanvas Pro is running!
        echo.
        echo The application launched successfully without segmentation fault.
        echo You can now close this window and interact with the application.
    ) else (
        echo WARNING: Process not found in task list.
        echo The application may have crashed or exited immediately.
    )
) else (
    echo ERROR: Executable not found at expected location!
    echo Expected: %EXE_PATH%
)

echo.
pause