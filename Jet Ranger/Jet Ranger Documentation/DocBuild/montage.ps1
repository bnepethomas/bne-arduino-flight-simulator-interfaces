# Makes a contact sheet of rendered diagram PNGs for quick visual checking.
#   .\montage.ps1 -Names a,b,c -Cols 1 -Width 900 -Out sheet.png
param([string[]]$Names, [int]$Cols = 1, [int]$Width = 900, [string]$Out = "sheet.png")
Add-Type -AssemblyName System.Drawing
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$png = Join-Path $here "diagrams\png"
$imgs = @(); foreach ($n in $Names) { $imgs += [System.Drawing.Image]::FromFile((Join-Path $png ($n + ".png"))) }
$cell = [int]($Width / $Cols)
$rowsH = @()
for ($r = 0; $r -lt [math]::Ceiling($imgs.Count / $Cols); $r++) {
    $mx = 0
    for ($c = 0; $c -lt $Cols; $c++) { $i = $r * $Cols + $c; if ($i -lt $imgs.Count) { $h = [int]($imgs[$i].Height * ($cell - 8) / $imgs[$i].Width); if ($h -gt $mx) { $mx = $h } } }
    $rowsH += ($mx + 8)
}
$bmp = New-Object System.Drawing.Bitmap $Width, ([int]($rowsH | Measure-Object -Sum).Sum)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.Clear([System.Drawing.Color]::White)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$y = 0
for ($r = 0; $r -lt $rowsH.Count; $r++) {
    for ($c = 0; $c -lt $Cols; $c++) {
        $i = $r * $Cols + $c
        if ($i -lt $imgs.Count) {
            $h = [int]($imgs[$i].Height * ($cell - 8) / $imgs[$i].Width)
            $g.DrawImage($imgs[$i], $c * $cell + 4, $y + 4, $cell - 8, $h)
            $g.DrawRectangle([System.Drawing.Pens]::LightGray, $c * $cell + 4, $y + 4, $cell - 8, $h)
        }
    }
    $y += $rowsH[$r]
}
$dest = Join-Path (Join-Path $here "diagrams") $Out
$bmp.Save($dest, [System.Drawing.Imaging.ImageFormat]::Png)
$dest
