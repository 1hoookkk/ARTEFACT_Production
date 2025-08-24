@echo off
setlocal EnableDelayedExpansion

echo.
echo ================================================================
echo SpectralCanvas Pro - Automated Binary Search Crash Hunt
echo ================================================================
echo.
echo This script automates the component isolation testing process
echo to systematically identify crash sources through binary search.
echo.

set BUILD_DIR=%~dp0build
set SOURCE_DIR=%~dp0Source\GUI
set CONFIG_FILE=%SOURCE_DIR%\ConfigRooms.h
set BACKUP_FILE=%SOURCE_DIR%\ConfigRooms_original.h
set TEMP_CONFIG=temp_room_config.h
set LOG_FILE=%BUILD_DIR%\CRASH_HUNT_LOG.md

REM Create log file
echo # SpectralCanvas Pro - Binary Search Crash Hunt Results > "%LOG_FILE%"
echo Date: %DATE% %TIME% >> "%LOG_FILE%"
echo Build Directory: %BUILD_DIR% >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

REM Backup original ConfigRooms.h
if not exist "%BACKUP_FILE%" (
    echo Backing up original ConfigRooms.h...
    copy "%CONFIG_FILE%" "%BACKUP_FILE%"
)

echo Select test configuration:
echo.
echo 1) Phase 1 - Minimal Build (LogoComponent + testButton only)
echo 2) Phase 2 - High Risk Components (Inspector panels only)
echo 3) Phase 3 - Canvas Component Isolation
echo 4) Phase 4A - Binary Search First Half
echo 5) Phase 4B - Binary Search Second Half
echo 6) Restore Original Configuration
echo 0) Exit
echo.

set /p choice=Enter your choice (0-6): 

if "%choice%"=="0" goto :end
if "%choice%"=="6" goto :restore

REM Set configuration based on choice
set config_file=""
set test_name=""

if "%choice%"=="1" (
    set config_file=build\test_room_config_minimal.h
    set test_name=Phase 1 - Minimal Build
)
if "%choice%"=="2" (
    set config_file=build\test_room_config_high_risk_only.h  
    set test_name=Phase 2 - High Risk Components
)
if "%choice%"=="3" (
    set config_file=build\test_room_config_canvas_only.h
    set test_name=Phase 3 - Canvas Component Isolation
)
if "%choice%"=="4" (
    set config_file=build\test_room_config_binary_a.h
    set test_name=Phase 4A - Binary Search First Half
)
if "%choice%"=="5" (
    set config_file=build\test_room_config_binary_b.h
    set test_name=Phase 4B - Binary Search Second Half
)

if %config_file%=="" (
    echo Invalid choice. Exiting.
    goto :end
)

echo.
echo ================================================================
echo Configuring: %test_name%
echo Configuration: %config_file%
echo ================================================================

REM Log test start
echo ## %test_name% >> "%LOG_FILE%"
echo Configuration: %config_file% >> "%LOG_FILE%"
echo Start Time: %TIME% >> "%LOG_FILE%"

REM Create temporary config with include
echo #include "%config_file%" > "%TEMP_CONFIG%"
echo. >> "%TEMP_CONFIG%"

REM Combine with original ConfigRooms.h  
type "%TEMP_CONFIG%" > "%CONFIG_FILE%.tmp"
type "%BACKUP_FILE%" >> "%CONFIG_FILE%.tmp"
move "%CONFIG_FILE%.tmp" "%CONFIG_FILE%"

echo Configuration applied successfully.
echo.

echo ================================================================
echo Building SpectralCanvas VST3...
echo ================================================================

REM Build the project
cmake --build . --config Release --target SpectralCanvas_artefacts --verbose

if errorlevel 1 (
    echo.
    echo ================================================================
    echo BUILD FAILED
    echo ================================================================
    echo Build failed >> "%LOG_FILE%"
    goto :cleanup
)

echo.
echo ================================================================  
echo BUILD SUCCESSFUL
echo ================================================================
echo.
echo VST3 Location: %BUILD_DIR%\SpectralCanvas_artefacts\Release\VST3\SpectralCanvas Pro.vst3
echo.
echo NEXT STEPS:
echo 1. Copy the VST3 to your DAW's plugin folder
echo 2. Load the plugin in your DAW
echo 3. Start audio playback  
echo 4. IMMEDIATELY open the plugin editor (within 1 second)
echo 5. Test for crashes with "open editor whilst processing" scenario
echo 6. Repeat test 3-5 times for consistency
echo.

echo Build successful >> "%LOG_FILE%"
echo VST3 ready for manual testing >> "%LOG_FILE%"

set /p test_result=Did the crash test PASS or CRASH? (P/C): 

if /i "%test_result%"=="P" (
    echo Test Result: PASS - No crashes detected >> "%LOG_FILE%"
) else if /i "%test_result%"=="C" (
    echo Test Result: CRASH - Component isolation successful >> "%LOG_FILE%"
) else (
    echo Test Result: Unknown - Manual verification required >> "%LOG_FILE%"
)

set /p notes=Enter any additional notes: 
echo Notes: %notes% >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo.
echo Results logged to: %LOG_FILE%

:cleanup
del "%TEMP_CONFIG%" 2>nul

echo.
set /p restore_choice=Restore original configuration? (Y/N): 
if /i "%restore_choice%"=="Y" goto :restore

goto :end

:restore
echo.
echo Restoring original ConfigRooms.h...
copy "%BACKUP_FILE%" "%CONFIG_FILE%"
echo Original configuration restored.
goto :end

:end
echo.
echo Binary search crash hunt completed.
echo.
echo Summary of available test configurations:
echo - test_room_config_minimal.h (Phase 1)
echo - test_room_config_high_risk_only.h (Phase 2)  
echo - test_room_config_canvas_only.h (Phase 3)
echo - test_room_config_binary_a.h (Phase 4A)
echo - test_room_config_binary_b.h (Phase 4B)
echo.
echo Log file: %LOG_FILE%
echo Component mapping: %BUILD_DIR%\ROOM_COMPONENT_MAPPING.md
echo Full instructions: %BUILD_DIR%\BINARY_SEARCH_TEST_INSTRUCTIONS.md
echo.
pause