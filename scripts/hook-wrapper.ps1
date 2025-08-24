# Hook Wrapper for Claude Code - Windows Visibility & Logging
# Ensures hook output appears in Claude terminal and is logged durably

param(
    [Parameter(Mandatory=$true)]
    [string]$Payload
)

# Ensure log directory exists
$LogDir = "$env:USERPROFILE\.claude\log"
if (!(Test-Path $LogDir)) {
    New-Item -ItemType Directory -Path $LogDir -Force | Out-Null
}

$LogFile = "$LogDir\spectral_debug.log"
$Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

# Helper function for visible logging
function Log-Visible {
    param([string]$Message)
    $Entry = "[$Timestamp] $Message"
    Write-Output $Entry  # Goes to Claude's stdout
    Add-Content -Path $LogFile -Value $Entry
}

# Log hook execution start
Log-Visible "HOOK START: $Payload"

try {
    # Check if payload script exists
    if (!(Test-Path $Payload)) {
        Log-Visible "ERROR: Payload script not found: $Payload"
        exit 1
    }

    # Execute the payload script and capture all streams
    Log-Visible "EXECUTING: PowerShell -NoProfile -ExecutionPolicy Bypass -File `"$Payload`""
    
    $Result = & PowerShell -NoProfile -ExecutionPolicy Bypass -File $Payload *>&1
    $ExitCode = $LASTEXITCODE
    
    # Log all output
    if ($Result) {
        $Result | ForEach-Object {
            Log-Visible "OUTPUT: $_"
        }
    }
    
    # Optional toast notification (if BurntToast available)
    try {
        Import-Module BurntToast -ErrorAction SilentlyContinue
        if (Get-Module BurntToast) {
            New-BurntToastNotification -Text "Claude Hook", "Executed: $(Split-Path $Payload -Leaf)" -ErrorAction SilentlyContinue
        } else {
            Log-Visible "INFO: BurntToast not available, skipping toast notification"
        }
    } catch {
        Log-Visible "INFO: Toast notification skipped - $_"
    }
    
    Log-Visible "HOOK COMPLETED: Exit code $ExitCode"
    
    # Show summary and recent log tail
    Write-Output ""
    Write-Output "=== HOOK EXECUTION SUMMARY ==="
    Write-Output "Payload: $Payload"
    Write-Output "Exit Code: $ExitCode"
    Write-Output "Log File: $LogFile"
    Write-Output ""
    Write-Output "=== RECENT LOG ENTRIES (Last 150 lines) ==="
    
    # Tail the log file (last 150 lines)
    if (Test-Path $LogFile) {
        Get-Content $LogFile | Select-Object -Last 150
    }
    
    exit $ExitCode
    
} catch {
    Log-Visible "ERROR: Hook execution failed - $($_.Exception.Message)"
    Write-Output "Hook wrapper failed: $($_.Exception.Message)"
    exit 1
}