# Screenshot helper for the User Guide.
#   . .\shot.ps1                       (dot-source it)
#   $p = Start-App "path\to\app.exe"   starts a program and waits for its window
#   Save-Window $hwnd "out.png"        captures one window (title bar and frame included)
#   Find-Window $pid "Title"           finds a top-level or owned window of a process by title
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName UIAutomationClient, UIAutomationTypes
Add-Type @"
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
public class WinShot {
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hWnd, IntPtr hdc, uint flags);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hWnd, out RECT r);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr w, IntPtr l);
    public static void Click(IntPtr h) { PostMessage(h, 0x00F5, IntPtr.Zero, IntPtr.Zero); }   // BM_CLICK, returns at once even if the click opens a modal dialog
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    [DllImport("user32.dll")] public static extern bool MoveWindow(IntPtr h, int x, int y, int w, int hgt, bool repaint);
    [DllImport("dwmapi.dll")] static extern int DwmGetWindowAttribute(IntPtr hwnd, int attr, out RECT r, int size);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    public static string Save(IntPtr h, string path) {
        ShowWindow(h, 9);   // restore if minimised
        RECT r;
        if (DwmGetWindowAttribute(h, 9, out r, Marshal.SizeOf(typeof(RECT))) != 0) GetWindowRect(h, out r);   // 9 = extended frame bounds (no shadow)
        int w = r.Right - r.Left, hh = r.Bottom - r.Top;
        RECT full; GetWindowRect(h, out full);
        int fw = full.Right - full.Left, fh = full.Bottom - full.Top;
        using (Bitmap bmp = new Bitmap(fw, fh)) {
            using (Graphics g = Graphics.FromImage(bmp)) {
                IntPtr dc = g.GetHdc();
                PrintWindow(h, dc, 2);     // PW_RENDERFULLCONTENT
                g.ReleaseHdc(dc);
            }
            // crop to the visible frame (drops the invisible resize border)
            Rectangle crop = new Rectangle(r.Left - full.Left, r.Top - full.Top, w, hh);
            using (Bitmap c = bmp.Clone(crop, bmp.PixelFormat)) { c.Save(path, ImageFormat.Png); }
        }
        return w + "x" + hh;
    }
}
"@ -ReferencedAssemblies System.Drawing

$script:A = [System.Windows.Automation.AutomationElement]
$script:TS = [System.Windows.Automation.TreeScope]

function Start-App($exe, $seconds = 3) {
    $p = Start-Process $exe -PassThru
    Start-Sleep $seconds
    $p.Refresh()
    return $p
}

function Find-Window($procId, $title) {
    $root = $script:A::RootElement
    $c = New-Object System.Windows.Automation.PropertyCondition($script:A::ProcessIdProperty, $procId)
    foreach ($w in $root.FindAll($script:TS::Children, $c)) {
        if (-not $title -or $w.Current.Name -like $title) { return $w }
    }
    # owned dialogs are descendants of the main window
    $main = $root.FindFirst($script:TS::Children, $c)
    if ($main) {
        $d = $main.FindAll($script:TS::Descendants, (New-Object System.Windows.Automation.PropertyCondition($script:A::ControlTypeProperty, [System.Windows.Automation.ControlType]::Window)))
        foreach ($w in $d) { if (-not $title -or $w.Current.Name -like $title) { return $w } }
    }
    return $null
}

function Save-Window($element, $path) {
    $h = [IntPtr]$element.Current.NativeWindowHandle
    [WinShot]::SetForegroundWindow($h) | Out-Null
    Start-Sleep -Milliseconds 400
    return [WinShot]::Save($h, $path)
}

function Resize-Window($element, $x, $y, $w, $h) {
    # makes a window big enough that its scroll bars disappear, so the whole form is captured
    $hwnd = [IntPtr]$element.Current.NativeWindowHandle
    [WinShot]::MoveWindow($hwnd, $x, $y, $w, $h, $true) | Out-Null
    Start-Sleep -Milliseconds 800
}

Add-Type @"
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
public class ScrollShot {
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hWnd, IntPtr hdc, uint flags);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hWnd, out RECT2 r);
    [DllImport("user32.dll")] static extern bool GetClientRect(IntPtr hWnd, out RECT2 r);
    [DllImport("user32.dll")] static extern bool ClientToScreen(IntPtr hWnd, ref POINT p);
    [DllImport("user32.dll")] static extern IntPtr SendMessage(IntPtr hWnd, uint msg, IntPtr w, IntPtr l);
    [DllImport("user32.dll")] static extern int GetSystemMetrics(int i);
    [StructLayout(LayoutKind.Sequential)] struct POINT { public int X, Y; }
    [StructLayout(LayoutKind.Sequential)] public struct RECT2 { public int Left, Top, Right, Bottom; }

    // the client area of the window (below the title bar, inside the frame) without the vertical scroll bar
    static Bitmap Client(IntPtr h, out int titleHeight) {
        RECT2 full; GetWindowRect(h, out full);
        RECT2 cr; GetClientRect(h, out cr);
        POINT o = new POINT { X = 0, Y = 0 }; ClientToScreen(h, ref o);
        int fw = full.Right - full.Left, fh = full.Bottom - full.Top;
        using (Bitmap bmp = new Bitmap(fw, fh)) {
            using (Graphics g = Graphics.FromImage(bmp)) { IntPtr dc = g.GetHdc(); PrintWindow(h, dc, 2); g.ReleaseHdc(dc); }
            int cx = o.X - full.Left, cy = o.Y - full.Top;
            titleHeight = cy;
            int w = cr.Right - cr.Left - GetSystemMetrics(2), hh = cr.Bottom - cr.Top;   // 2 = SM_CXVSCROLL
            return bmp.Clone(new Rectangle(cx, cy, w, hh), PixelFormat.Format32bppArgb);
        }
    }

    static bool RowsEqual(Bitmap a, int ay, Bitmap b, int by, int count) {
        Rectangle ra = new Rectangle(0, ay, a.Width, count), rb = new Rectangle(0, by, b.Width, count);
        BitmapData da = a.LockBits(ra, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        BitmapData db = b.LockBits(rb, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        try {
            int bytes = da.Stride * count;
            byte[] x = new byte[bytes], y = new byte[bytes];
            Marshal.Copy(da.Scan0, x, 0, bytes); Marshal.Copy(db.Scan0, y, 0, bytes);
            for (int i = 0; i < bytes; i++) if (x[i] != y[i]) return false;
            return true;
        } finally { a.UnlockBits(da); b.UnlockBits(db); }
    }

    // Scrolls a form that scrolls itself (WM_VSCROLL, page down) and stitches the pages into one tall image.
    public static string Save(IntPtr h, string path, int maxPages) {
        int title;
        Bitmap stitched = Client(h, out title);
        for (int page = 1; page < maxPages; page++) {
            SendMessage(h, 0x115, (IntPtr)3, IntPtr.Zero);        // WM_VSCROLL, SB_PAGEDOWN
            System.Threading.Thread.Sleep(500);
            int t2; Bitmap next = Client(h, out t2);
            int overlap = 0;
            int max = Math.Min(stitched.Height, next.Height);
            for (int k = max; k >= 20; k--) {
                if (RowsEqual(stitched, stitched.Height - k, next, 0, k)) { overlap = k; break; }
            }
            if (overlap >= next.Height) { next.Dispose(); break; }             // nothing new: reached the end
            if (overlap == 0) overlap = 0;                                       // no overlap found: append as is
            Bitmap grown = new Bitmap(stitched.Width, stitched.Height + next.Height - overlap);
            using (Graphics g = Graphics.FromImage(grown)) {
                g.DrawImage(stitched, 0, 0);
                g.DrawImage(next, 0, stitched.Height - overlap);
            }
            stitched.Dispose(); next.Dispose(); stitched = grown;
        }
        SendMessage(h, 0x115, (IntPtr)6, IntPtr.Zero);                           // SB_TOP: put it back
        stitched.Save(path, ImageFormat.Png);
        string r = stitched.Width + "x" + stitched.Height;
        stitched.Dispose();
        return r;
    }
}
"@ -ReferencedAssemblies System.Drawing

function Save-ScrollingWindow($element, $path, $pages = 8) {
    $h = [IntPtr]$element.Current.NativeWindowHandle
    [WinShot]::SetForegroundWindow($h) | Out-Null
    Start-Sleep -Milliseconds 400
    return [ScrollShot]::Save($h, $path, $pages)
}
