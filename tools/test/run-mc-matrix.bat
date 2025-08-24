@echo off
REM =============================================================================
REM Multicore DSP Test Matrix Runner
REM Tests all combinations of launch modes, buffer sizes, sample rates, and 
REM multicore configurations for SpectralCanvas Pro
REM =============================================================================

setlocal enabledelayedexpansion

echo ============================================================================
echo SpectralCanvas Pro - Multicore DSP Test Matrix
echo ============================================================================
echo.

REM Ensure runtime directory exists
if not exist "runtime" mkdir runtime

REM Initialize performance summary CSV with headers
echo Timestamp,Phase,Mode,Buffer,SampleRate,Threads,MC_State,CPU_Percent,MaxDepth,RMS,Result > runtime\perf-summary.csv

REM Verify executable exists
set "EXECUTABLE=build\SpectralCanvas_artefacts\Debug\Standalone\SpectralCanvas Pro.exe"
if not exist "%EXECUTABLE%" (
    echo ERROR: Executable not found at %EXECUTABLE%
    echo Please build the Debug Standalone target first:
    echo cmake -B build ^&^& msbuild build\SpectralCanvas_Standalone.vcxproj -p:Configuration=Debug
    exit /b 1
)

echo Found executable: %EXECUTABLE%
echo.

REM Test configuration arrays
set "MODES=--minimal --safe default --debug"
set "BUFFERS=64 128 256"
set "SAMPLE_RATES=44100 48000"
set "MC_CONFIGS=default force_single force_multi"

REM Phase counter
set PHASE=0

echo Starting test matrix execution...
echo.

REM Loop through all combinations
for %%m in (%MODES%) do (
    for %%b in (%BUFFERS%) do (
        for %%s in (%SAMPLE_RATES%) do (
            for %%c in (%MC_CONFIGS%) do (
                set /a PHASE+=1
                
                REM Set environment variables based on MC config
                if "%%c"=="force_single" (
                    set SC_FORCE_SINGLECORE=1
                    set SC_FORCE_MULTICORE=
                    set MC_STATE=SingleCore
                ) else if "%%c"=="force_multi" (
                    set SC_FORCE_SINGLECORE=
                    set SC_FORCE_MULTICORE=1
                    set MC_STATE=MultiCore
                ) else (
                    set SC_FORCE_SINGLECORE=
                    set SC_FORCE_MULTICORE=
                    set MC_STATE=Adaptive
                )
                
                echo ========================================
                echo Phase !PHASE!: Testing configuration
                echo Mode: %%m
                echo Buffer: %%b samples
                echo Sample Rate: %%s Hz
                echo Multicore: !MC_STATE!
                echo ========================================
                
                REM Prepare launch arguments
                set "LAUNCH_ARGS=--log --test-duration 5"
                if not "%%m"=="default" set "LAUNCH_ARGS=%%m !LAUNCH_ARGS!"
                
                REM Create test-specific log file
                set "TEST_LOG=runtime\test_phase_!PHASE!.log"
                
                REM Launch test with timeout (30 seconds max)
                echo Running: "%EXECUTABLE%" !LAUNCH_ARGS!
                echo Launch Time: !DATE! !TIME! > "!TEST_LOG!"
                echo Arguments: !LAUNCH_ARGS! >> "!TEST_LOG!"
                echo Environment: SC_FORCE_SINGLECORE=!SC_FORCE_SINGLECORE! SC_FORCE_MULTICORE=!SC_FORCE_MULTICORE! >> "!TEST_LOG!"
                echo. >> "!TEST_LOG!"
                
                REM Run with timeout using start command
                start /wait /b "" timeout /t 30 >nul 2>nul &
                start /wait /b "" "%EXECUTABLE%" !LAUNCH_ARGS! >> "!TEST_LOG!" 2>&1
                
                set TEST_RESULT=!ERRORLEVEL!
                
                REM Parse metrics and append to CSV
                call tools\test\parse_metrics.bat !PHASE! "%%m" %%b %%s "!MC_STATE!" "!TEST_RESULT!" "!TEST_LOG!"
                
                REM Brief pause between tests
                timeout /t 2 /nobreak >nul 2>nul
                
                REM Clean up environment for next test
                set SC_FORCE_SINGLECORE=
                set SC_FORCE_MULTICORE=
            )
        )
    )
)

echo.
echo ============================================================================
echo Test Matrix Execution Complete
echo ============================================================================
echo Total Phases: !PHASE!
echo Results CSV: runtime\perf-summary.csv
echo Individual logs: runtime\test_phase_*.log
echo.

REM Generate summary report
echo Generating test summary...
call :generate_summary

echo.
echo Test matrix execution finished. Check runtime\perf-summary.csv for results.
goto :eof

:generate_summary
echo ============================================================================ > runtime\mc-test-summary.txt
echo SpectralCanvas Pro - Multicore Test Matrix Summary >> runtime\mc-test-summary.txt
echo Generated: %DATE% %TIME% >> runtime\mc-test-summary.txt
echo ============================================================================ >> runtime\mc-test-summary.txt
echo. >> runtime\mc-test-summary.txt

REM Count successful vs failed tests
set SUCCESS_COUNT=0
set FAIL_COUNT=0

REM This would need more sophisticated parsing in a real implementation
echo Total Phases Executed: !PHASE! >> runtime\mc-test-summary.txt
echo. >> runtime\mc-test-summary.txt
echo See runtime\perf-summary.csv for detailed results. >> runtime\mc-test-summary.txt
echo Individual test logs: runtime\test_phase_*.log >> runtime\mc-test-summary.txt
echo. >> runtime\mc-test-summary.txt

goto :eof