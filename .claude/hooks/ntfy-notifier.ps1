param(
    [string]$Message = "Claude Code task completed",
    [string]$Title = "SpectralCanvas Pro",
    [string]$Priority = "default",
    [string]$Tags = "computer,claude"
)

# ntfy.sh notification script for Claude Code hooks
# Usage: powershell.exe -ExecutionPolicy Bypass -File .claude/hooks/ntfy-notifier.ps1 "Your message"

$TopicUrl = "https://ntfy.sh/spectralcanvas-dev-$($env:USERNAME)"
$Headers = @{
    "Title" = $Title
    "Priority" = $Priority  
    "Tags" = $Tags
}

try {
    $Response = Invoke-RestMethod -Uri $TopicUrl -Method Post -Body $Message -Headers $Headers -TimeoutSec 10
    Write-Host "✅ Notification sent: $Message"
} catch {
    Write-Warning "❌ Failed to send notification: $_"
    # Log to backup file for debugging
    $LogEntry = "$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss'): FAILED - $Message"
    Add-Content -Path "C:\temp\spectral_debug.log" -Value $LogEntry -ErrorAction SilentlyContinue
}