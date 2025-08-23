---
name: config-verify
description: Verify Claude Code configuration is valid
---

# Config Verification

## JSON Validation
```bash
pwsh -NoProfile -Command "try { Get-Content '.claude/settings.local.json' -Raw | ConvertFrom-Json | Out-Null; Write-Output '✅ settings.local.json: Valid JSON' } catch { Write-Output '❌ settings.local.json: Invalid JSON' }"
pwsh -NoProfile -Command "try { Get-Content '.claude/settings.json' -Raw | ConvertFrom-Json | Out-Null; Write-Output '✅ settings.json: Valid JSON' } catch { Write-Output '❌ settings.json: Invalid JSON' }"
```

## Frontmatter Check
```bash
pwsh -NoProfile -Command "Get-ChildItem '.claude/agents' -Recurse -File -Include *.md | ForEach-Object { $content = Get-Content $_.FullName -Raw; if ($content -match '^---[\s\S]*?name:[\s\S]*?---') { Write-Output \"✅ $($_.Name): Has valid frontmatter with name\" } else { Write-Output \"❌ $($_.Name): Missing frontmatter or name field\" } }"
```

## Hook Directory Check
```bash
pwsh -NoProfile -Command "if (Test-Path \"$env:USERPROFILE\.claude\hooks\") { Write-Output '✅ Hooks directory exists' } else { Write-Output '❌ Hooks directory missing' }"
pwsh -NoProfile -Command "if (Test-Path \"$env:USERPROFILE\.claude\logs\") { Write-Output '✅ Logs directory exists' } else { Write-Output '❌ Logs directory missing' }"
```

## Test Hooks (Dry Run)
```bash
pwsh -NoProfile -ExecutionPolicy Bypass -File "$env:USERPROFILE\.claude\hooks\smart-lint.ps1"
pwsh -NoProfile -ExecutionPolicy Bypass -File "$env:USERPROFILE\.claude\hooks\smart-test.ps1"
pwsh -NoProfile -ExecutionPolicy Bypass -File "$env:USERPROFILE\.claude\hooks\ntfy-notifier.ps1"
```

## Claude Diagnostics
```bash
claude --diagnostics
```