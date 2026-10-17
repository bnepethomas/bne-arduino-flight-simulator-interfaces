# Renders pages of a PDF to PNG files using the Windows built-in PDF API (for visual checking only).
#   .\pdf2png.ps1 -Pdf out\sample.pdf -Pages 1,2,3 [-Width 1100] [-OutDir out\pages]
param([Parameter(Mandatory = $true)][string]$Pdf, [int[]]$Pages = @(1), [int]$Width = 1100, [string]$OutDir)
Add-Type -AssemblyName System.Runtime.WindowsRuntime
$null = [Windows.Data.Pdf.PdfDocument, Windows.Data.Pdf, ContentType = WindowsRuntime]
$null = [Windows.Storage.StorageFile, Windows.Storage, ContentType = WindowsRuntime]
$null = [Windows.Storage.Streams.InMemoryRandomAccessStream, Windows.Storage.Streams, ContentType = WindowsRuntime]
$null = [Windows.Data.Pdf.PdfPageRenderOptions, Windows.Data.Pdf, ContentType = WindowsRuntime]

$asTaskOp = ([System.WindowsRuntimeSystemExtensions].GetMethods() | Where-Object {
        $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]
$asTaskAct = ([System.WindowsRuntimeSystemExtensions].GetMethods() | Where-Object {
        $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncAction' })[0]
function Await($op, [Type]$type) {
    $t = $asTaskOp.MakeGenericMethod($type).Invoke($null, @($op))
    $t.Wait(-1) | Out-Null
    $t.Result
}
$pdfPath = (Resolve-Path $Pdf).Path
if (-not $OutDir) { $OutDir = Join-Path (Split-Path $pdfPath) "pages" }
New-Item -ItemType Directory -Force $OutDir | Out-Null
$file = Await ([Windows.Storage.StorageFile]::GetFileFromPathAsync($pdfPath)) ([Windows.Storage.StorageFile])
$doc = Await ([Windows.Data.Pdf.PdfDocument]::LoadFromFileAsync($file)) ([Windows.Data.Pdf.PdfDocument])
"pages in PDF: " + $doc.PageCount
foreach ($n in $Pages) {
    if ($n -lt 1 -or $n -gt $doc.PageCount) { continue }
    $page = $doc.GetPage($n - 1)
    $opts = New-Object Windows.Data.Pdf.PdfPageRenderOptions
    $opts.DestinationWidth = $Width
    $stream = New-Object Windows.Storage.Streams.InMemoryRandomAccessStream
    $act = $page.RenderToStreamAsync($stream, $opts)
    $tk = $asTaskAct.Invoke($null, @($act)); $tk.Wait(-1) | Out-Null
    $out = Join-Path $OutDir ("page{0:D3}.png" -f $n)
    $fs = [System.IO.File]::Create($out)
    $rs = [System.IO.WindowsRuntimeStreamExtensions]::AsStreamForRead($stream)
    $rs.CopyTo($fs)
    $fs.Close(); $rs.Close(); $stream.Dispose(); $page.Dispose()
    $out
}
