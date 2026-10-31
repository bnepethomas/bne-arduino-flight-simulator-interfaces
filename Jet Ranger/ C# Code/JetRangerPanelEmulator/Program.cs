namespace JetRangerPanelEmulator
{
    internal static class Program
    {
        // Options:  --board radio|servo|stepper|dual|adf|all   open those windows at once (no launcher)
        //           --net <a.b.c>                              the first three numbers of the emulators' addresses (default 127.0.0, so the Radio is 127.0.0.101)
        //           --watchdog <seconds>                       the no-data time before a board blanks (30 like the real boards)
        //           --dump <folder>                            write each board's state to <folder>\<board>.json four times a second (for tests)
        public static Dictionary<string, string> Args = new(StringComparer.OrdinalIgnoreCase);

        [STAThread]
        static void Main(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                if (!args[i].StartsWith("--")) continue;
                Args[args[i].Substring(2)] = (i + 1 < args.Length && !args[i + 1].StartsWith("--")) ? args[++i] : "1";
            }
            ApplicationConfiguration.Initialize();
            BoardWindow.DumpDirectory = Args.GetValueOrDefault("dump");
            if (double.TryParse(Args.GetValueOrDefault("watchdog"), System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out double wd) && wd > 0) BoardWindow.WatchdogSeconds = wd;
            string net = Args.GetValueOrDefault("net") ?? "127.0.0";
            if (Args.TryGetValue("board", out var which))
            {
                var windows = LauncherForm.Open(which, net);
                if (windows.Count == 0) { MessageBox.Show("Unknown board '" + which + "'. Use radio, servo, stepper, dual, adf or all."); return; }
                Application.Run(new MultiFormContext(windows));
            }
            else Application.Run(new LauncherForm(net));
        }
    }

    // Keeps the program running until every emulator window is closed.
    sealed class MultiFormContext : ApplicationContext
    {
        int open;
        public MultiFormContext(IEnumerable<BoardWindow> windows)
        {
            foreach (var w in windows)
            {
                open++;
                w.FormClosed += (_, _) => { if (--open == 0) ExitThread(); };
                w.Show();
            }
        }
    }
}
