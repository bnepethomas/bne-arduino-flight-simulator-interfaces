# Trims the white border (plus a small margin) off a PNG and saves it as a JPEG (quality 85). Used by export_install_views.py.
#   powershell -ExecutionPolicy Bypass -File trim_to_jpg.ps1 -Png in.png -Jpg out.jpg
param([Parameter(Mandatory = $true)][string]$Png, [Parameter(Mandatory = $true)][string]$Jpg)
Add-Type -AssemblyName System.Drawing
Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
public static class Trim {
    public static Rectangle Bounds(Bitmap b, int margin) {
        var d = b.LockBits(new Rectangle(0, 0, b.Width, b.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        int minx = b.Width, miny = b.Height, maxx = -1, maxy = -1;
        var px = new byte[d.Stride * b.Height];
        System.Runtime.InteropServices.Marshal.Copy(d.Scan0, px, 0, px.Length);
        for (int y = 0; y < b.Height; y++) {
            for (int x = 0; x < b.Width; x++) {
                int o = y * d.Stride + x * 4;
                if (px[o] < 200 || px[o + 1] < 200 || px[o + 2] < 200) {
                    if (x < minx) minx = x;
                    if (x > maxx) maxx = x;
                    if (y < miny) miny = y;
                    if (y > maxy) maxy = y;
                }
            }
        }
        b.UnlockBits(d);
        if (maxx < 0) return new Rectangle(0, 0, b.Width, b.Height);
        minx = Math.Max(0, minx - margin); miny = Math.Max(0, miny - margin);
        maxx = Math.Min(b.Width - 1, maxx + margin); maxy = Math.Min(b.Height - 1, maxy + margin);
        return new Rectangle(minx, miny, maxx - minx + 1, maxy - miny + 1);
    }
}
'@
$img = New-Object System.Drawing.Bitmap $Png
$rect = [Trim]::Bounds($img, 24)
$crop = $img.Clone($rect, $img.PixelFormat)
$codec = [System.Drawing.Imaging.ImageCodecInfo]::GetImageEncoders() | Where-Object { $_.MimeType -eq "image/jpeg" }
$ep = New-Object System.Drawing.Imaging.EncoderParameters 1
$ep.Param[0] = New-Object System.Drawing.Imaging.EncoderParameter ([System.Drawing.Imaging.Encoder]::Quality), 85L
$crop.Save($Jpg, $codec, $ep)
$crop.Dispose(); $img.Dispose()
