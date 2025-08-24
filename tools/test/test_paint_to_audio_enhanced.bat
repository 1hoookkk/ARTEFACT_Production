@echo off
echo ============================================
echo ENHANCED PAINT-TO-AUDIO MAPPER TEST
echo ============================================
echo.
echo Testing enhanced paint-to-audio pipeline with:
echo   ✅ Enhanced pressure sensitivity curves
echo   ✅ Velocity-based musical expression
echo   ✅ Sophisticated color-to-audio mapping
echo   ✅ MetaSynth-style coordinate system
echo   ✅ Real-time oscillator allocation
echo.

REM Open Task Manager to Performance tab for monitoring
echo Opening Task Manager for performance monitoring...
start taskmgr /7 /performance

REM Wait for Task Manager to load
timeout /t 2 /nobreak > nul

REM Launch SpectralCanvas Pro
echo.
echo Launching SpectralCanvas Pro - Enhanced Build...
cd /d "C:\ARTEFACT_Production\build\SpectralCanvasPro_artefacts\Release"
start "SpectralCanvas Pro - Enhanced Paint-to-Audio" "SPECTRAL CANVAS PRO.exe"

echo.
echo ============================================
echo ENHANCED PAINT-TO-AUDIO TEST CHECKLIST:
echo ============================================
echo.
echo 1. 🎨 BASIC PAINT RESPONSE:
echo    □ Paint on canvas - should hear immediate audio
echo    □ Mouse movement creates velocity modulation
echo    □ Different Y positions = different frequencies
echo.
echo 2. 🎵 PRESSURE SENSITIVITY:
echo    □ Light touches = quiet oscillators
echo    □ Heavy pressure = loud, rich oscillators
echo    □ Pressure curve feels natural (not linear)
echo.
echo 3. 🌈 COLOR-TO-AUDIO MAPPING:
echo    □ Red paint = Left pan
echo    □ Green paint = Right pan
echo    □ Blue paint = Center pan
echo    □ Bright colors = Open filter
echo    □ Saturated colors = More resonance
echo.
echo 4. ⚡ VELOCITY RESPONSE:
echo    □ Fast mouse movements = higher modulation
echo    □ Slow movements = stable tones
echo    □ Velocity affects envelope attack
echo.
echo 5. 🎛️ PERFORMANCE METRICS:
echo    □ CPU usage stays under 30%%
echo    □ No audio dropouts or clicks
echo    □ Sub-10ms latency (immediate response)
echo    □ Memory usage remains stable
echo.
echo 6. 🎹 METASYNTH-STYLE WORKFLOW:
echo    □ X-axis = Time progression
echo    □ Y-axis = Frequency (20Hz-20kHz logarithmic)
echo    □ Paint strokes create oscillators in real-time
echo    □ Multiple colors blend naturally
echo.
echo ============================================
echo ADVANCED TESTING SCENARIOS:
echo ============================================
echo.
echo A. PRESSURE DYNAMICS TEST:
echo    1. Paint very lightly - should be barely audible
echo    2. Gradually increase pressure - volume should grow smoothly
echo    3. Quick pressure changes - should respond immediately
echo.
echo B. COLOR MIXING TEST:
echo    1. Paint red stroke on left side
echo    2. Paint green stroke on right side  
echo    3. Paint blue stroke in center
echo    4. Mix colors while painting - should blend audio parameters
echo.
echo C. VELOCITY EXPRESSION TEST:
echo    1. Paint slowly - should create stable tones
echo    2. Paint quickly - should add movement/modulation
echo    3. Vary speed during stroke - should respond dynamically
echo.
echo D. FREQUENCY RANGE TEST:
echo    1. Paint at top of canvas - high frequencies (10kHz+)
echo    2. Paint at bottom - low frequencies (100Hz-)
echo    3. Paint across full height - frequency sweep
echo.
echo ============================================
echo TROUBLESHOOTING:
echo ============================================
echo.
echo If no audio:
echo   1. Check if audio is activated (first paint stroke should activate)
echo   2. Verify system audio is working
echo   3. Check CPU usage isn't too high
echo.
echo If performance issues:
echo   1. Close other applications
echo   2. Check Task Manager CPU/Memory
echo   3. Try simpler paint strokes first
echo.
echo ============================================
echo.
echo Press any key when testing is complete...
pause > nul

echo.
echo ============================================
echo TEST RESULTS SUMMARY:
echo ============================================
echo.
echo Please rate each area (1-5, 5=perfect):
echo.
set /p basic_response="Basic paint-to-audio response: "
set /p pressure_sens="Pressure sensitivity: "
set /p color_mapping="Color-to-audio mapping: "
set /p velocity_resp="Velocity response: "
set /p performance="Performance (CPU/latency): "
set /p overall="Overall experience: "

echo.
echo ============================================
echo FINAL ASSESSMENT:
echo ============================================
echo Basic Response: %basic_response%/5
echo Pressure Sensitivity: %pressure_sens%/5  
echo Color Mapping: %color_mapping%/5
echo Velocity Response: %velocity_resp%/5
echo Performance: %performance%/5
echo Overall: %overall%/5
echo.

if %overall% GEQ 4 (
    echo ✅ PAINT-TO-AUDIO MAPPER: EXCELLENT - Ready for production!
) else if %overall% GEQ 3 (
    echo ⚠️  PAINT-TO-AUDIO MAPPER: GOOD - Minor improvements needed
) else (
    echo 🚨 PAINT-TO-AUDIO MAPPER: NEEDS WORK - Major issues found
)

echo.
echo Test completed. Results logged for development team.
echo ============================================
pause