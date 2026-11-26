using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;

namespace P3DViewLauncher
{
    /// <summary>Finds Prepar3D's view windows and moves them. SimConnect's window position takes no negative
    /// numbers, so it cannot reach screens left of or above the primary one; this is done through Windows.</summary>
    public static class NativeWindows
    {
        const int GWL_STYLE = -16;
        const long WS_CAPTION = 0x00C00000, WS_THICKFRAME = 0x00040000;
        const uint SWP_NOZORDER = 0x0004, SWP_FRAMECHANGED = 0x0020, SWP_SHOWWINDOW = 0x0040;
        const int SW_RESTORE = 9;
        const uint SWP_NOSIZE = 0x0001, SWP_NOMOVE = 0x0002, SWP_NOACTIVATE = 0x0010, KEYEVENTF_KEYUP = 0x0002;
        const byte VK_MENU = 0x12;
        [DllImport("user32.dll")] static extern bool SetForegroundWindow(IntPtr hWnd);
        [DllImport("user32.dll")] static extern void keybd_event(byte key, byte scan, uint flags, UIntPtr extra);

        delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

        [StructLayout(LayoutKind.Sequential)] struct RECT { public int Left, Top, Right, Bottom; }

        [DllImport("user32.dll")] static extern bool EnumWindows(EnumWindowsProc callback, IntPtr lParam);
        [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hWnd);
        [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowText(IntPtr hWnd, StringBuilder text, int max);
        [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);
        [DllImport("user32.dll", SetLastError = true)] static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
        [DllImport("user32.dll", EntryPoint = "GetWindowLongPtrW")] static extern IntPtr GetWindowLongPtr(IntPtr hWnd, int index);
        [DllImport("user32.dll", EntryPoint = "SetWindowLongPtrW", SetLastError = true)] static extern IntPtr SetWindowLongPtr(IntPtr hWnd, int index, IntPtr value);
        [DllImport("user32.dll", SetLastError = true)] static extern bool SetWindowPos(IntPtr hWnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
        [DllImport("user32.dll")] static extern bool ShowWindow(IntPtr hWnd, int command);
        [DllImport("user32.dll")] static extern bool SetProcessDpiAwarenessContext(IntPtr context);

        /// <summary>Makes screen positions real pixels when the screens use different Windows scaling.</summary>
        public static void UseRealPixels()
        {
            try { SetProcessDpiAwarenessContext(new IntPtr(-4)); }   // per monitor aware, version 2
            catch (EntryPointNotFoundException) { }
        }

        /// <summary>The visible, titled windows belonging to Prepar3D.</summary>
        public static List<KeyValuePair<IntPtr, string>> SimulatorWindows()
        {
            var ids = new HashSet<uint>();
            foreach (var p in Process.GetProcessesByName("Prepar3D")) ids.Add((uint)p.Id);
            var found = new List<KeyValuePair<IntPtr, string>>();
            EnumWindows((hWnd, _) =>
            {
                GetWindowThreadProcessId(hWnd, out uint id);
                if (!ids.Contains(id) || !IsWindowVisible(hWnd)) return true;
                var text = new StringBuilder(256);
                GetWindowText(hWnd, text, text.Capacity);
                if (text.Length > 0) found.Add(new KeyValuePair<IntPtr, string>(hWnd, text.ToString()));
                return true;
            }, IntPtr.Zero);
            return found;
        }

        public static IntPtr FindSimulatorWindow(string title)
        {
            foreach (var w in SimulatorWindows())
                if (string.Equals(w.Value, title, StringComparison.OrdinalIgnoreCase)) return w.Key;
            return IntPtr.Zero;
        }

        /// <summary>Removes the window's frame and makes it cover the whole screen. This is done through Windows only:
        /// Prepar3D's own full screen mode is not used. Returns null, or what went wrong.</summary>
        public static string FillScreen(IntPtr window, Rectangle screen)
        {
            ShowWindow(window, SW_RESTORE);
            long style = GetWindowLongPtr(window, GWL_STYLE).ToInt64();
            if (SetWindowLongPtr(window, GWL_STYLE, new IntPtr(style & ~(WS_CAPTION | WS_THICKFRAME))) == IntPtr.Zero)
                return Failed("the window frame could not be removed");
            // Twice: moving between screens with different scaling can resize the window on the first move.
            for (int i = 0; i < 2; i++)
                if (!SetWindowPos(window, IntPtr.Zero, screen.X, screen.Y, screen.Width, screen.Height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW))
                    return Failed("the window could not be moved");
            BringToFront(window);
            return null;
        }

        /// <summary>Raises the window above other programs' windows once, without keeping it on top. Windows ignores a
        /// background program that only asks for "top", so the window is made topmost and at once ordinary again. It is
        /// then activated if Windows allows it (just after a key press), which puts it over the taskbar too.</summary>
        static void BringToFront(IntPtr window)
        {
            const uint flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
            SetWindowPos(window, new IntPtr(-1), 0, 0, 0, 0, flags);   // topmost
            SetWindowPos(window, new IntPtr(-2), 0, 0, 0, 0, flags);   // not topmost
            keybd_event(VK_MENU, 0, 0, UIntPtr.Zero);
            keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, UIntPtr.Zero);
            SetForegroundWindow(window);
            System.Threading.Thread.Sleep(300);
        }

        /// <summary>Moves the window to the middle of the screen. It stays an ordinary window with its frame, not
        /// maximised. The size changes only if one is given (it is limited to the screen). Returns null, or what went
        /// wrong.</summary>
        public static string Place(IntPtr window, Rectangle screen, Size? size)
        {
            ShowWindow(window, SW_RESTORE);
            // Twice: moving between screens with different scaling can resize the window on the first move.
            for (int i = 0; i < 2; i++)
            {
                if (!GetWindowRect(window, out RECT now)) return Failed("the window size could not be read");
                int width = Math.Min(size?.Width ?? now.Right - now.Left, screen.Width);
                int height = Math.Min(size?.Height ?? now.Bottom - now.Top, screen.Height);
                if (!SetWindowPos(window, IntPtr.Zero, screen.X + (screen.Width - width) / 2, screen.Y + (screen.Height - height) / 2,
                                  width, height, SWP_NOZORDER | SWP_SHOWWINDOW))
                    return Failed("the window could not be moved");
            }
            return null;
        }

        static string Failed(string what)
        {
            int error = Marshal.GetLastWin32Error();
            return what + " (Windows error " + error + (error == 5 ? ": run this program as administrator, as Prepar3D is" : "") + ")";
        }
    }
}
