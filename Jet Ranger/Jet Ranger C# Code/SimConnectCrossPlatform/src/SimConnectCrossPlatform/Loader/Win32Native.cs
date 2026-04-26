// src/SimConnectCrossPlatform/Loader/Win32Native.cs

using System;
using System.Runtime.InteropServices;

namespace SimConnectCrossPlatform.Loader
{
    /// <summary>
    /// Win32 P/Invoke declarations for dynamic DLL loading
    /// </summary>
    internal static class Win32Native
    {
        [DllImport("kernel32.dll", SetLastError = true,
            CharSet = CharSet.Ansi)]
        public static extern IntPtr LoadLibrary(
            [MarshalAs(UnmanagedType.LPStr)] string lpFileName);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool FreeLibrary(IntPtr hModule);

        [DllImport("kernel32.dll", SetLastError = true,
            CharSet = CharSet.Ansi, ExactSpelling = true)]
        public static extern IntPtr GetProcAddress(
            IntPtr hModule,
            [MarshalAs(UnmanagedType.LPStr)] string procName);

        [DllImport("kernel32.dll")]
        public static extern uint GetLastError();
    }
}