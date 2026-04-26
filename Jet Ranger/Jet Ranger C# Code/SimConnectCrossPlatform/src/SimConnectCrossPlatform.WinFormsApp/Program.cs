// File: src/SimConnectCrossPlatform.WinFormsApp/Program.cs

using System;
using System.Windows.Forms;

namespace SimConnectCrossPlatform.WinFormsApp
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.Run(new MainForm());
        }
    }
}