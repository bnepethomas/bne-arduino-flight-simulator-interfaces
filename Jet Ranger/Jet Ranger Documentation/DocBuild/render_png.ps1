# Renders every SVG in .\diagrams to a PNG in .\diagrams\png using LibreOffice (for visual checking only).
param([string[]]$Names)
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$svgDir = Join-Path $here "diagrams"
$png = Join-Path $svgDir "png"
New-Item -ItemType Directory -Force $png | Out-Null
$files = if ($Names) { $Names | ForEach-Object { Join-Path $svgDir ($_ + ".svg") } } else { Get-ChildItem $svgDir -Filter *.svg | ForEach-Object { $_.FullName } }
& "C:\Program Files\LibreOffice\program\soffice.exe" --headless --norestore --convert-to png --outdir $png @files | Out-Null
Get-ChildItem $png -Filter *.png | Select-Object Name, Length
