Param(
    [string]$Root = "src",
    [string[]]$Extensions = @(".ts", ".tsx", ".js", ".jsx", ".py"),
    [int]$Window = 5,
    [int]$MinBlockChars = 60,
    [int]$Top = 30,
    [switch]$ShowSnippet
)

if (-not (Test-Path -LiteralPath $Root)) {
    Write-Error "Root path '$Root' not found. Run from repo root or pass -Root."
    exit 1
}

$files = Get-ChildItem -LiteralPath $Root -Recurse -File |
    Where-Object { $Extensions -contains $_.Extension.ToLower() }

if (-not $files) {
    Write-Host "No files with extensions $($Extensions -join ', ') under '$Root'."
    exit 0
}

function Normalize-Line {
    param([string]$line)
    if ($null -eq $line) { return "" }
    # Trim and collapse whitespace to reduce false negatives across formatting
    return ($line.Trim() -replace "\s+", " ")
}

$sha1 = [System.Security.Cryptography.SHA1]::Create()
$dups = @{}

foreach ($f in $files) {
    try {
        $lines = Get-Content -LiteralPath $f.FullName -ErrorAction Stop
    } catch {
        Write-Warning "Skipped (read error): $($f.FullName) => $($_.Exception.Message)"
        continue
    }

    if ($lines.Count -lt $Window) { continue }

    $norm = for ($i = 0; $i -lt $lines.Count; $i++) { Normalize-Line $lines[$i] }

    for ($i = 0; $i -le $norm.Count - $Window; $i++) {
        $block = ($norm[$i..($i + $Window - 1)] -join "`n").Trim()
        if ($block.Length -lt $MinBlockChars) { continue }

        $bytes = [Text.Encoding]::UTF8.GetBytes($block)
        $hash = [System.BitConverter]::ToString($sha1.ComputeHash($bytes)).Replace('-', '')

        if (-not $dups.ContainsKey($hash)) { $dups[$hash] = @() }
        $dups[$hash] += [pscustomobject]@{
            File  = $f.FullName
            Start = $i + 1
        }
    }
}

$results = @()
foreach ($kv in $dups.GetEnumerator()) {
    $occ = $kv.Value
    if ($occ.Count -gt 1) {
        $results += [pscustomobject]@{
            Hash        = $kv.Key
            Occurrences = $occ.Count
            Locations   = ($occ | ForEach-Object { "$(($_.File)):$($_.Start)" }) -join "; "
        }
    }
}

if (-not $results) {
    Write-Host "No duplicate $Window-line blocks with length >= $MinBlockChars found under '$Root'."
    exit 0
}

$topResults = $results | Sort-Object -Property Occurrences -Descending | Select-Object -First $Top
$topResults | Format-Table -AutoSize

if ($ShowSnippet) {
    foreach ($r in $topResults) {
        $firstLoc = $r.Locations.Split("; ")[0]
        $lastColon = $firstLoc.LastIndexOf(':')
        $path = $firstLoc.Substring(0, $lastColon)
        $start = [int]$firstLoc.Substring($lastColon + 1)
        try {
            $content = Get-Content -LiteralPath $path -ErrorAction Stop
            $end = [Math]::Min($content.Count - 1, $start - 1 + $Window - 1)
            $snippet = $content[$start - 1..$end]
            Write-Host "`n=== Snippet for $($r.Hash) ($($r.Occurrences)x) from $path:$start ==="
            Write-Host ($snippet -join "`n")
        } catch {
            Write-Warning "Failed to load snippet from $path: $($_.Exception.Message)"
        }
    }
}

exit 0

