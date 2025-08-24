param(
    [string]$Config = "RelWithDebInfo",
    [string]$Target = "ALL_BUILD",
    [string]$BuildDir = "./build",
    [switch]$Clean = $false,
    [switch]$Tests = $true,
    [string]$SlackWebhook = $env:SPECTRALCANVAS_SLACK_WEBHOOK,
    [switch]$DryRun = $false
)

# SpectralCanvas Pro - Build Wrapper with RT-Safety Notifications
# Integrates with cmake build system and provides comprehensive feedback

$ErrorActionPreference = "Continue"  # Don't stop on warnings

Write-Host "🎵 SpectralCanvas Pro - Build & Notify System" -ForegroundColor Magenta
Write-Host "=============================================" -ForegroundColor Magenta

# Validate build directory
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Error "Not in SpectralCanvas Pro root directory (CMakeLists.txt not found)"
    exit 1
}

# Build phase detection for appropriate notifications
$buildPhases = @()
if ($Clean) { $buildPhases += "Clean" }
$buildPhases += "Configure"
$buildPhases += "Compile"
if ($Tests) { $buildPhases += "Test" }

$totalPhases = $buildPhases.Count
$currentPhase = 0

function Send-BuildNotification {
    param($phase, $status, $details = "", $isRTSafety = $false)
    
    $title = "SpectralCanvas Pro: $phase $status"
    $message = "$details"
    
    # Desktop notification
    & "$PSScriptRoot\run-and-notify.ps1" -Title $title -Command "echo '$message'" -RTSafety:$isRTSafety -LogDir "./logs/build"
    
    # Slack notification (if configured)
    if ($SlackWebhook) {
        $color = switch ($status) {
            "Success" { "good" }
            "Failed" { "danger" }  
            "Started" { "warning" }
            default { "warning" }
        }
        & "$PSScriptRoot\notify-slack.ps1" -WebhookUrl $SlackWebhook -Title $title -Text $message -Color $color -RTSafety:$isRTSafety
    }
}

try {
    # Phase 1: Clean (if requested)
    if ($Clean) {
        $currentPhase++
        Write-Host "[$currentPhase/$totalPhases] 🧹 Cleaning build directory..." -ForegroundColor Yellow
        
        if (-not $DryRun) {
            if (Test-Path $BuildDir) {
                Remove-Item $BuildDir -Recurse -Force
                Write-Host "   Build directory cleaned" -ForegroundColor Green
            }
            Send-BuildNotification -phase "Clean" -status "Success" -details "Build directory cleaned"
        }
    }

    # Phase 2: Configure
    $currentPhase++
    Write-Host "[$currentPhase/$totalPhases] ⚙️  Configuring CMake..." -ForegroundColor Yellow
    
    $configureCmd = "cmake -S . -B $BuildDir -DCMAKE_BUILD_TYPE=$Config"
    if ($Tests) {
        $configureCmd += " -DBUILD_TESTS=ON"
    }
    
    Write-Host "   Command: $configureCmd" -ForegroundColor Gray
    
    if (-not $DryRun) {
        $configResult = & "$PSScriptRoot\run-and-notify.ps1" -Title "CMake Configure" -Command $configureCmd -RTSafety
        if ($LASTEXITCODE -ne 0) {
            Send-BuildNotification -phase "Configure" -status "Failed" -details "CMake configuration failed. Check JUCE dependencies and build environment." -isRTSafety $true
            exit $LASTEXITCODE
        }
        Send-BuildNotification -phase "Configure" -status "Success" -details "CMake configuration completed successfully"
    }

    # Phase 3: Compile
    $currentPhase++
    Write-Host "[$currentPhase/$totalPhases] 🔨 Building SpectralCanvas..." -ForegroundColor Yellow
    
    $buildCmd = "cmake --build $BuildDir --config $Config --target $Target"
    Write-Host "   Command: $buildCmd" -ForegroundColor Gray
    Write-Host "   Target: $Target ($Config)" -ForegroundColor Gray
    
    if (-not $DryRun) {
        $buildStart = Get-Date
        Send-BuildNotification -phase "Build" -status "Started" -details "Compiling SpectralCanvas Pro ($Config configuration)" -isRTSafety $true
        
        $buildResult = & "$PSScriptRoot\run-and-notify.ps1" -Title "SpectralCanvas Build" -Command $buildCmd -RTSafety
        $buildDuration = (Get-Date) - $buildStart
        
        if ($LASTEXITCODE -ne 0) {
            Send-BuildNotification -phase "Build" -status "Failed" -details "Build failed after $($buildDuration.TotalMinutes.ToString('F1')) minutes. Check RT-safety violations and JUCE integration." -isRTSafety $true
            Write-Host ""
            Write-Host "💡 SpectralCanvas Pro Build Troubleshooting:" -ForegroundColor Cyan
            Write-Host "   • Audio thread safety: No locks/allocs in processBlock()" -ForegroundColor Cyan
            Write-Host "   • Parameter bindings: Check APVTS consistency" -ForegroundColor Cyan  
            Write-Host "   • Emergency systems: Verify test tone infrastructure" -ForegroundColor Cyan
            Write-Host "   • Run diagnostic: tools/test/phase7-quick-validation.bat" -ForegroundColor Cyan
            exit $LASTEXITCODE
        }
        
        Send-BuildNotification -phase "Build" -status "Success" -details "Build completed in $($buildDuration.TotalMinutes.ToString('F1')) minutes. Ready for audio processing!" -isRTSafety $true
        
        # Check for generated binaries
        $vst3Path = "$BuildDir/SpectralCanvas_artefacts/$Config/VST3/SpectralCanvas Pro.vst3"
        $standalonePath = "$BuildDir/SpectralCanvas_artefacts/$Config/Standalone/SpectralCanvas Pro.exe"
        
        if (Test-Path $vst3Path) {
            Write-Host "   ✅ VST3 Plugin: $vst3Path" -ForegroundColor Green
        }
        if (Test-Path $standalonePath) {
            Write-Host "   ✅ Standalone App: $standalonePath" -ForegroundColor Green
        }
    }

    # Phase 4: Tests (if enabled)
    if ($Tests) {
        $currentPhase++
        Write-Host "[$currentPhase/$totalPhases] 🧪 Running RT-Safety Tests..." -ForegroundColor Yellow
        
        $testCmd = "ctest --test-dir $BuildDir -C $Config --output-on-failure"
        Write-Host "   Command: $testCmd" -ForegroundColor Gray
        
        if (-not $DryRun) {
            $testStart = Get-Date
            Send-BuildNotification -phase "Tests" -status "Started" -details "Running RT-safety and integration tests" -isRTSafety $true
            
            $testResult = & "$PSScriptRoot\run-and-notify.ps1" -Title "SpectralCanvas Tests" -Command $testCmd -RTSafety
            $testDuration = (Get-Date) - $testStart
            
            if ($LASTEXITCODE -ne 0) {
                Send-BuildNotification -phase "Tests" -status "Failed" -details "Tests failed after $($testDuration.TotalSeconds.ToString('F1')) seconds. RT-safety or integration issues detected." -isRTSafety $true
                Write-Host ""
                Write-Host "🔍 Test Failure Analysis:" -ForegroundColor Red
                Write-Host "   • RT-Safety: Check for audio thread violations" -ForegroundColor Red
                Write-Host "   • Paint→Audio: Verify SPSC queue integrity" -ForegroundColor Red
                Write-Host "   • Parameters: Check APVTS atomic access" -ForegroundColor Red
                Write-Host "   • Emergency: Verify test tone injection works" -ForegroundColor Red
                exit $LASTEXITCODE
            }
            
            Send-BuildNotification -phase "Tests" -status "Success" -details "All tests passed in $($testDuration.TotalSeconds.ToString('F1')) seconds. RT-safety validated!" -isRTSafety $true
        }
    }

    # Final success notification
    Write-Host ""
    Write-Host "🎉 SpectralCanvas Pro Build Complete!" -ForegroundColor Green
    Write-Host "   Ready for spectral synthesis and canvas-based sound design" -ForegroundColor Green
    
    if (-not $DryRun) {
        Send-BuildNotification -phase "Pipeline" -status "Success" -details "Full build pipeline completed successfully. All systems operational!" -isRTSafety $true
    }

} catch {
    Write-Error "Build system error: $_"
    Send-BuildNotification -phase "System" -status "Failed" -details "Build system encountered an unexpected error: $_" -isRTSafety $true
    exit 1
}

exit 0