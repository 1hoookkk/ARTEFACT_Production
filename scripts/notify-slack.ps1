param(
    [string]$WebhookUrl,
    [string]$Text,
    [string]$Title = "SpectralCanvas Pro",
    [string]$Color = "good",  # good (green), warning (orange), danger (red), or hex
    [string]$Channel,
    [string]$Username = "SpectralCanvas-Bot",
    [string]$IconEmoji = ":musical_note:",
    [switch]$RTSafety = $false
)

# SpectralCanvas Pro - Slack/Teams Webhook Notification
# Secure webhook posting with RT-safety context

if (-not $WebhookUrl) { 
    Write-Error "WebhookUrl is required. Set via environment variable or parameter."
    Write-Host "Usage: -WebhookUrl 'https://hooks.slack.com/...' -Text 'Build completed'"
    exit 2 
}

if (-not $Text) {
    Write-Error "Text message is required."
    exit 2
}

try {
    # Detect webhook type
    $isSlack = $WebhookUrl -match "hooks\.slack\.com"
    $isTeams = $WebhookUrl -match "outlook\.office\.com"
    
    # RT-Safety context formatting
    $rtContext = if ($RTSafety) { " (RT-Safe Build)" } else { "" }
    $formattedTitle = "$Title$rtContext"
    
    # Color mapping for different services
    $colorMap = @{
        "good" = "#36a64f"      # Green
        "warning" = "#ff9900"   # Orange  
        "danger" = "#ff0000"    # Red
    }
    $hexColor = if ($colorMap.ContainsKey($Color)) { $colorMap[$Color] } else { $Color }

    if ($isSlack) {
        # Slack-specific payload
        $payload = @{
            text = $formattedTitle
            attachments = @(
                @{
                    color = $Color
                    fields = @(
                        @{
                            title = "Message"
                            value = $Text
                            short = $false
                        }
                        @{
                            title = "Timestamp"
                            value = (Get-Date -Format "yyyy-MM-dd HH:mm:ss")
                            short = $true
                        }
                        @{
                            title = "Machine"
                            value = $env:COMPUTERNAME
                            short = $true
                        }
                    )
                }
            )
        }
        
        # Add optional Slack-specific fields
        if ($Channel) { $payload.channel = $Channel }
        if ($Username) { $payload.username = $Username }
        if ($IconEmoji) { $payload.icon_emoji = $IconEmoji }
        
    } elseif ($isTeams) {
        # Microsoft Teams-specific payload
        $payload = @{
            "@type" = "MessageCard"
            "@context" = "https://schema.org/extensions"
            summary = $formattedTitle
            themeColor = $hexColor
            sections = @(
                @{
                    activityTitle = $formattedTitle
                    activityText = $Text
                    facts = @(
                        @{
                            name = "Timestamp"
                            value = (Get-Date -Format "yyyy-MM-dd HH:mm:ss")
                        }
                        @{
                            name = "Machine"
                            value = $env:COMPUTERNAME
                        }
                        @{
                            name = "RT-Safety"
                            value = if ($RTSafety) { "Yes" } else { "No" }
                        }
                    )
                }
            )
        }
        
    } else {
        # Generic webhook payload (works with Discord, others)
        $payload = @{
            content = "$formattedTitle`n$Text`n`nTimestamp: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`nMachine: $env:COMPUTERNAME"
        }
    }

    # Convert to JSON
    $jsonPayload = $payload | ConvertTo-Json -Depth 10 -Compress

    # Send webhook with timeout and error handling
    $response = Invoke-RestMethod -Uri $WebhookUrl -Method Post -ContentType 'application/json' -Body $jsonPayload -TimeoutSec 15

    Write-Host "[notify-slack] Message sent successfully" -ForegroundColor Green
    Write-Verbose "Response: $response"
    
} catch {
    Write-Warning "Webhook notification failed: $_"
    Write-Verbose "Payload was: $jsonPayload"
    
    # Don't fail the entire operation if webhook fails
    # This is especially important for CI/build scenarios
    Write-Host "[notify-slack] Continuing despite webhook failure..." -ForegroundColor Yellow
}

# Always exit successfully - webhook failures shouldn't break builds
exit 0