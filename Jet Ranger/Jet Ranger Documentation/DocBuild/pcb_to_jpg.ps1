# Converts the PNG files that export_pcb_images.py leaves in %TEMP%\pcb_export to JPEG (quality 80) in .\pcb, which the documents embed.
#   powershell -ExecutionPolicy Bypass -File pcb_to_jpg.ps1
Add-Type -AssemblyName System.Drawing
$tmp = Join-Path $env:TEMP "pcb_export"
$out = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) "pcb"
New-Item -ItemType Directory -Force $out | Out-Null
$codec = [System.Drawing.Imaging.ImageCodecInfo]::GetImageEncoders() | Where-Object { $_.MimeType -eq "image/jpeg" }
$ep = New-Object System.Drawing.Imaging.EncoderParameters 1
$ep.Param[0] = New-Object System.Drawing.Imaging.EncoderParameter ([System.Drawing.Imaging.Encoder]::Quality), 80L
foreach ($f in (Get-ChildItem $tmp -Filter *.png | Where-Object { $_.Name -match "_(sch|pcb)\.png$" })) {
    $img = [System.Drawing.Image]::FromFile($f.FullName)
    $img.Save((Join-Path $out ($f.BaseName + ".jpg")), $codec, $ep)
    $img.Dispose()
}
