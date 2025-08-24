param(
    [string]$LogDir = "./logs",
    [string]$Title = "SpectralCanvas Task",
    [string]$Command,
    [string]$WorkingDir = ".",
    [string]$SnoreToastPath = "../tools/SnoreToast.exe",
    [switch]$RTSafety = $false,
    [switch]$DryRun = $false
)

# SpectralCanvas Pro - Command Wrapper with Notifications & Logging
# Captures output, logs results, sends notifications on completion

if (-not $Command) { 
    Write-Error "Command is required. Usage: -Command 'cmake --build build'"
    exit 2 
}

# Ensure log directory exists
if (-not (Test-Path $LogDir)) { 
    New-Item -ItemType Directory -Path $LogDir -Force | Out-Null 
}

# Generate timestamped log filename
$timestamp = Get-Date -Format 'yyyyMMdd_HHmmss'
$safeTitle = $Title -replace '[^a-zA-Z0-9_-]', '_'
$logFile = Join-Path $LogDir "$safeTitle`_$timestamp.log"

# RT-Safety detection
$isRTSafetyTask = $RTSafety -or ($Command -match "(RT|Paint|Audio|processBlock|SpectralSynthEngine)")

if ($DryRun) {
    Write-Host "[DRY-RUN] Would execute: $Command" -ForegroundColor Yellow
    Write-Host "[DRY-RUN] Working directory: $(Resolve-Path $WorkingDir)" -ForegroundColor Yellow
    Write-Host "[DRY-RUN] Log file: $logFile" -ForegroundColor Yellow
    Write-Host "[DRY-RUN] RT-Safety context: $isRTSafetyTask" -ForegroundColor Yellow
    exit 0
}

Write-Host "[run-and-notify] Starting: $Title" -ForegroundColor Cyan
Write-Host "[run-and-notify] Command: $Command" -ForegroundColor Gray
Write-Host "[run-and-notify] Log: $logFile" -ForegroundColor Gray

# Change to working directory if specified
$originalDir = Get-Location
if ($WorkingDir -ne ".") {
    Set-Location $WorkingDir
}

try {
    # Create process start info for proper output capture
    $startInfo = New-Object System.Diagnostics.ProcessStartInfo
    $startInfo.FileName = "powershell.exe"
    $startInfo.Arguments = "-Command `"$Command`""
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.WorkingDirectory = (Get-Location).Path

    # Start process and capture output
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $startInfo
    $process.Start() | Out-Null

    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()
    $exitCode = $process.ExitCode

    # Create comprehensive log entry
    $logContent = @"
SpectralCanvas Pro - Task Execution Log
=====================================
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Title: $Title
Command: $Command
Working Directory: $(Get-Location)
Exit Code: $exitCode
RT-Safety Context: $isRTSafetyTask
Duration: $((Get-Date) - $timestamp)

STDOUT:
$stdout

STDERR:
$stderr

ANALYSIS:
$( if ($exitCode -eq 0) { "✅ SUCCESS" } else { "❌ FAILED" } )
$( if ($isRTSafetyTask) { "⚡ RT-Safety task detected - review for locks/allocs/logging" } )
$( if ($stdout -match "(error|failed|exception)" -or $stderr -match "(error|failed|exception)") { "⚠️  Error patterns detected in output" } )

"@

    # Write log file
    $logContent | Out-File -FilePath $logFile -Encoding UTF8

    # Determine notification message and type
    if ($exitCode -eq 0) {
        $notifyTitle = "✅ $Title: Success"
        $notifyMessage = "Exit code 0 - Log: $logFile"
        if ($isRTSafetyTask) {
            $notifyMessage += " (RT-Safe)"
        }
        Write-Host "[run-and-notify] SUCCESS: $Title completed" -ForegroundColor Green
    } else {
        $notifyTitle = "❌ $Title: FAILED"
        $notifyMessage = "Exit code $exitCode - Log: $logFile"
        if ($isRTSafetyTask) {
            $notifyMessage += " (RT-Critical)"
        }
        Write-Host "[run-and-notify] FAILED: $Title failed with exit code $exitCode" -ForegroundColor Red
    }

    # Send notification
    & "$PSScriptRoot\notify.ps1" -Title $notifyTitle -Message $notifyMessage -SnoreToastPath $SnoreToastPath

    # Special handling for build failures in SpectralCanvas
    if ($exitCode -ne 0 -and $Command -match "(cmake|build)") {
        Write-Host "" -ForegroundColor Yellow
        Write-Host "🔧 Build failed. Common SpectralCanvas issues:" -ForegroundColor Yellow
        Write-Host "   • Check RT-safety violations (locks in audio thread)" -ForegroundColor Yellow
        Write-Host "   • Verify JUCE module dependencies" -ForegroundColor Yellow
        Write-Host "   • Review parameter binding consistency" -ForegroundColor Yellow
        Write-Host "   • Run: tools/test/phase7-quick-validation.bat" -ForegroundColor Yellow
    }

    # Return to original directory
    Set-Location $originalDir
    
    # Exit with the same code as the wrapped command
    exit $exitCode

} catch {
    # Handle process execution errors
    $errorMessage = "Process execution failed: $_"
    Write-Error $errorMessage
    
    # Log the error
    @"
SpectralCanvas Pro - Task Execution Error
=======================================
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Title: $Title
Command: $Command
Error: $errorMessage
"@ | Out-File -FilePath $logFile -Encoding UTF8

    # Send error notification
    & "$PSScriptRoot\notify.ps1" -Title "💥 $Title: ERROR" -Message "Process failed - Log: $logFile" -SnoreToastPath $SnoreToastPath
    
    Set-Location $originalDir
    exit 1
}