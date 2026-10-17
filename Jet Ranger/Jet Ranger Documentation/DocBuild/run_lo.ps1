# Runs a LibreOffice-python script against a headless soffice listening on port 2002, then stops that soffice.
#   .\run_lo.ps1 svg2png.py [args...]
param([Parameter(Mandatory = $true)][string]$Script, [Parameter(ValueFromRemainingArguments = $true)][string[]]$Rest)
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$lo = "C:\Program Files\LibreOffice\program"
$soffice = Start-Process "$lo\soffice.exe" -ArgumentList '--headless', '--invisible', '--norestore', '--accept=socket,host=localhost,port=2002;urp;' -PassThru -WindowStyle Hidden
$ok = $false
for ($i = 0; $i -lt 40; $i++) { Start-Sleep 1; if (netstat -ano | Select-String ":2002 .*LISTENING") { $ok = $true; break } }
if (-not $ok) { "soffice did not start listening"; exit 1 }
try {
    & "$lo\python.exe" (Join-Path $here $Script) @Rest
} finally {
    Get-Process soffice* -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
}
