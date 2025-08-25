Param(
  [Parameter(Mandatory=$true)][string]$Message,
  [string]$Title = "Claude Code",
  [string]$Tags = "white_check_mark",
  [ValidateSet("1","2","3","4","5")][string]$Priority = "3",
  [string]$Click = "",
  [string]$File = "",
  [string]$TopicEnv = $env:NTFY_TOPIC,
  [string]$TokenEnv = $env:NTFY_TOKEN
)

# Fallbacks: use env first; if missing, try script-scoped defaults (AVOID HARD-CODING)
$Topic = if ($TopicEnv) { $TopicEnv } else { "upje22bhvoax1gvj3pdxvu27n96kwy" }
$Token = if ($TokenEnv) { $TokenEnv } else { "awxujv8ad14oxy9dbghje2567cgko8" }

if (-not $Topic) { Write-Error "NTFY topic missing. Set env NTFY_TOPIC or pass TopicEnv."; exit 2 }
if (-not $Token) { Write-Error "NTFY token missing. Set env NTFY_TOKEN or pass TokenEnv."; exit 3 }

$Uri = "https://ntfy.sh/$Topic"

$Headers = @{
  "Authorization" = "Bearer $Token"
  "Title"         = $Title
  "Priority"      = $Priority
  "Tags"          = $Tags
}
if ($Click) { $Headers["Click"] = $Click }

try {
  if ($File -and (Test-Path $File)) {
    # Attach a file with the notification
    $Body = Get-Content -Path $File -Raw
    Invoke-RestMethod -Uri $Uri -Method POST -Headers $Headers -InFile $File -ContentType "application/octet-stream" | Out-Null
  } else {
    Invoke-RestMethod -Uri $Uri -Method POST -Headers $Headers -Body $Message | Out-Null
  }
  # Optional local log (no secrets)
  $log = Join-Path -Path (Split-Path $PSCommandPath -Parent) -ChildPath "..\notifications.log"
  $stamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
  "$stamp`t$Title`t$Priority`t$Message" | Out-File -FilePath $log -Append -Encoding UTF8
} catch {
  Write-Error "ntfy-notifier: $($_.Exception.Message)"
  exit 1
}
