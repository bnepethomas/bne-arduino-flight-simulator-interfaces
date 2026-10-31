namespace XPlaneBridge
{
    internal static class Program
    {
        /// <summary>The command line, kept for the form: --board-host, --xplane-host, --xplane-port, --datarefs, --profile-dir, --module, --no-send.</summary>
        public static Dictionary<string, string> Args { get; private set; } = new(StringComparer.OrdinalIgnoreCase);

        [STAThread]
        static void Main(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                if (!args[i].StartsWith("--")) continue;
                string key = args[i].Substring(2);
                string value = (i + 1 < args.Length && !args[i + 1].StartsWith("--")) ? args[++i] : "1";
                Args[key] = value;
            }
            ApplicationConfiguration.Initialize();
            Application.Run(new Form1());
        }
    }
}
