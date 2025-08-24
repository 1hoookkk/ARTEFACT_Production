# ============================
# SpectralCanvas Asset Process (v2)
# ============================

$ErrorActionPreference = 'Stop'

# ---- Paths
$root        = "C:\ARTEFACT_Production\Source"
$srcFrames   = Join-Path $root "assets_src\frames"
$srcPanels   = Join-Path $root "assets_src\panels"
$srcSprites  = Join-Path $root "assets_src\sprites"
$dstRoot     = Join-Path $root "assets_rt"
$dstFrames   = Join-Path $dstRoot "frames"
$dstPanels   = Join-Path $dstRoot "panels"
$dstSprites  = Join-Path $dstRoot "sprites"

# ---- Check ImageMagick
if (-not (Get-Command magick -ErrorAction SilentlyContinue)) {
  Write-Host "[X] ImageMagick ('magick') not found on PATH." -ForegroundColor Red
  Write-Host "    Install from: https://imagemagick.org/script/download.php#windows"
  Write-Host "    During install, tick: 'Add application directory to your system PATH'." -ForegroundColor Yellow
  exit 1
}

# ---- Ensure output dirs
$null = New-Item -ItemType Directory -Force -Path $dstRoot,$dstFrames,$dstPanels,$dstSprites

function Copy-IfExists {
  param($srcDir,$dstDir)
  if (Test-Path $srcDir) {
    Write-Host "[=] Copying from $srcDir -> $dstDir"
    Get-ChildItem "$srcDir\*.png" -ErrorAction SilentlyContinue | ForEach-Object {
      Copy-Item $_.FullName -Destination (Join-Path $dstDir $_.Name) -Force
    }
    $count = (Get-ChildItem "$dstDir\*.png" -ErrorAction SilentlyContinue).Count
    Write-Host "    Copied $count file(s)."
  } else {
    Write-Host "[!] Skipping (missing): $srcDir" -ForegroundColor DarkYellow
  }
}

# 1) Copy opaque assets (panels & frames) unchanged
Copy-IfExists $srcPanels $dstPanels
Copy-IfExists $srcFrames $dstFrames

# 2) Process sprites (transparent PNGs)
if (-not (Test-Path $srcSprites)) {
  Write-Host "[!] Skipping sprites (missing): $srcSprites" -ForegroundColor DarkYellow
} else {
  $spriteFiles = Get-ChildItem "$srcSprites\*.png" -ErrorAction SilentlyContinue
  if ($spriteFiles.Count -eq 0) {
    Write-Host "[!] No sprite PNGs found in $srcSprites" -ForegroundColor DarkYellow
  } else {
    Write-Host "[=] Cleaning sprite backgrounds -> $dstSprites"
    foreach ($f in $spriteFiles) {
      $out      = Join-Path $dstSprites $f.Name
      $maskFile = Join-Path $dstSprites "__alpha_mask_tmp.png"

      # a) remove white-ish bg to transparency
      & magick "$($f.FullName)" -alpha on -fuzz 4% -transparent white -strip "$out"
      if ($LASTEXITCODE -ne 0) {
        Write-Host "    [!] Background clean failed: $($f.Name)" -ForegroundColor Yellow
        continue
      }

      # b) harden alpha WITHOUT parentheses trick:
      #    make a mask from current alpha, erode 1px, then apply as new alpha
      & magick "$out" -alpha extract -morphology erode diamond:1 "$maskFile"
      if ($LASTEXITCODE -ne 0) {
        Write-Host "    [!] Alpha mask build failed: $($f.Name)" -ForegroundColor Yellow
        continue
      }
      & magick "$out" "$maskFile" -compose CopyOpacity -composite "$out"
      if ($LASTEXITCODE -ne 0) {
        Write-Host "    [!] Alpha apply failed: $($f.Name)" -ForegroundColor Yellow
        continue
      }
      if (Test-Path $maskFile) { Remove-Item $maskFile -Force }

      Write-Host "    [+] Processed: $($f.Name)"
    }
  }
}

Write-Host "`n✅ Base processing complete. Outputs in: $dstRoot" -ForegroundColor Green

# ---- OPTIONAL: Standardize canvas sizes (uncomment to enable) ----
# Write-Host "`n[>] Optional canvas standardization..." -ForegroundColor Cyan
# Get-ChildItem "$dstSprites\*knob*large*.png" -ErrorAction SilentlyContinue | ForEach-Object {
#   & magick "$($_.FullName)" -gravity center -background none -extent 2048x2048 "$($_.FullName)"
#   Write-Host "    [•] Knob large extent: $($_.Name)"
# }
# Get-ChildItem "$dstSprites\*knob*small*.png" -ErrorAction SilentlyContinue | ForEach-Object {
#   & magick "$($_.FullName)" -gravity center -background none -extent 1536x1536 "$($_.FullName)"
#   Write-Host "    [•] Knob small extent: $($_.Name)"
# }
# Get-ChildItem "$dstSprites\*button*up*.png","$dstSprites\*button*down*.png" -ErrorAction SilentlyContinue | ForEach-Object {
#   & magick "$($_.FullName)" -gravity center -background none -extent 2048x2048 "$($_.FullName)"
#   Write-Host "    [•] Button extent: $($_.Name)"
# }
# Get-ChildItem "$dstSprites\*led*.png" -ErrorAction SilentlyContinue | ForEach-Object {
#   & magick "$($_.FullName)" -gravity center -background none -extent 1024x1024 "$($_.FullName)"
#   Write-Host "    [•] LED extent: $($_.Name)"
# }
# Get-ChildItem "$dstSprites\*lcd*.png" -ErrorAction SilentlyContinue | ForEach-Object {
#   & magick "$($_.FullName)" -gravity center -background none -extent 2048x1024 "$($_.FullName)"
#   Write-Host "    [•] LCD extent: $($_.Name)"
# }
