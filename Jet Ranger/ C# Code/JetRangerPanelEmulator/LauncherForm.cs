namespace JetRangerPanelEmulator
{
    // The small window that opens the emulators: one button per controller and one for all of them.
    public sealed class LauncherForm : Form
    {
        readonly string net;
        readonly Dictionary<string, BoardWindow> open = new();
        readonly Label info = new() { AutoSize = true, Margin = new Padding(8, 10, 8, 4) };

        static readonly (string Key, string Text, int Number)[] Boards =
        {
            ("radio", "Radio controller", 101), ("servo", "Servo controller", 102), ("stepper", "Stepper controller", 105),
            ("dual", "Dual Stepper controller", 106), ("adf", "ADF and Upper controller", 107),
        };

        public static List<BoardWindow> Open(string which, string net)
        {
            var list = new List<BoardWindow>();
            foreach (var (key, _, _) in Boards)
                if (which.Equals("all", StringComparison.OrdinalIgnoreCase) || which.Equals(key, StringComparison.OrdinalIgnoreCase)) list.Add(Create(key, net));
            // spread them out so they do not sit on top of each other
            int x = 20, y = 20;
            foreach (var w in list) { w.Location = new Point(x, y); x += 40; y += 40; }
            return list;
        }

        static BoardWindow Create(string key, string net) => key switch
        {
            "radio" => new RadioWindow(net),
            "servo" => new ServoWindow(net),
            "stepper" => new StepperWindow(net),
            "dual" => new DualStepperWindow(net),
            _ => new AdfWindow(net),
        };

        public LauncherForm(string net)
        {
            this.net = net;
            Text = "Jet Ranger panel emulators";
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            var panel = new FlowLayoutPanel { FlowDirection = FlowDirection.TopDown, AutoSize = true, Padding = new Padding(12) };
            panel.Controls.Add(new Label
            {
                AutoSize = true, MaximumSize = new Size(380, 0),
                Text = "Each emulator listens on its own loopback address and the board's port, so a bridge started with --emulators (or with its Send to the emulators box ticked) drives them like the real boards.",
            });
            foreach (var (key, text, number) in Boards)
            {
                var b = new Button { Text = text + "  (" + net + "." + number + ")", Width = 380, Height = 34, Tag = key };
                b.Click += (_, _) => OpenOne(key, b);
                panel.Controls.Add(b);
            }
            var all = new Button { Text = "Open all five", Width = 380, Height = 38 };
            all.Click += (_, _) => { foreach (var (key, _, _) in Boards) OpenOne(key, null); };
            panel.Controls.Add(all);
            panel.Controls.Add(info);
            Controls.Add(panel);
            FormClosed += (_, _) => { foreach (var w in open.Values.ToList()) w.Close(); };
        }

        void OpenOne(string key, Button? b)
        {
            if (open.TryGetValue(key, out var existing) && !existing.IsDisposed) { existing.Activate(); return; }
            var w = Create(key, net);
            w.Location = new Point(Left + 60 + open.Count * 30, Top + 60 + open.Count * 30);
            w.FormClosed += (_, _) => open.Remove(key);
            open[key] = w;
            w.Show(this);
        }
    }
}
