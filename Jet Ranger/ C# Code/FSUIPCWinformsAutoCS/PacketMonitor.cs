using System.Collections.Concurrent;
using System.Text;

namespace FSUIPCTest
{
    // The packet monitor: every payload the bridge sends to the boards (and the GPS interface), one line each with the time, the target board and where it went,
    // with a filter by target board. The packets are queued as they are sent and shown ten times a second, so a busy bridge does not slow down.
    public sealed class PacketMonitorForm : Form
    {
        const int Keep = 6000;               // the packets kept for re-filtering
        const int ShowLast = 2500;           // the lines shown after a filter change

        readonly BoardTargets targets;
        readonly ConcurrentQueue<SentPacket> queue = new();
        readonly List<SentPacket> history = new();
        readonly ComboBox cboBoard = new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 190 };
        readonly CheckBox chkPause = new() { Text = "Pause", AutoSize = true };
        readonly Button btnClear = new() { Text = "Clear", AutoSize = true };
        readonly Label lblCount = new() { AutoSize = true, Margin = new Padding(12, 6, 3, 0) };
        readonly TextBox txt = new() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Dock = DockStyle.Fill, Font = new Font("Consolas", 9F), BackColor = Color.White };
        readonly System.Windows.Forms.Timer timer = new() { Interval = 100 };
        int total, shown;

        public PacketMonitorForm(BoardTargets targets, string initialFilter)
        {
            this.targets = targets;
            Text = "Packets sent to the boards";
            ClientSize = new Size(1000, 480);
            MinimumSize = new Size(560, 240);

            var bar = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 6, 8, 4) };
            bar.Controls.AddRange(new Control[] { new Label { Text = "Target board", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, cboBoard, chkPause, btnClear, lblCount });
            Controls.Add(txt);
            Controls.Add(bar);

            cboBoard.Items.Add("All boards");
            foreach (var n in BoardTargets.Names) cboBoard.Items.Add(n);
            cboBoard.SelectedIndex = Math.Max(0, IndexOf(initialFilter));
            cboBoard.SelectedIndexChanged += (_, _) => Rebuild();
            btnClear.Click += (_, _) => { history.Clear(); txt.Clear(); total = shown = 0; ShowCount(); };
            timer.Tick += (_, _) => Drain();
            targets.PacketSent += OnSent;
            FormClosed += (_, _) => { timer.Stop(); targets.PacketSent -= OnSent; };
            timer.Start();
            ShowCount();
        }

        // The combo box entry for a filter given on the command line: "radio", "stepper", "dual", "adf", "gps" or a last number such as "105".
        static int IndexOf(string filter)
        {
            if (string.IsNullOrWhiteSpace(filter)) return 0;
            for (int i = 0; i < BoardTargets.Names.Length; i++)
                if (BoardTargets.Names[i].Contains(filter.Trim(), StringComparison.OrdinalIgnoreCase)) return i + 1;
            return 0;
        }

        void OnSent(SentPacket p) => queue.Enqueue(p);

        bool Matches(SentPacket p) => cboBoard.SelectedIndex <= 0 || p.Board == (string)cboBoard.SelectedItem!;

        static string Line(SentPacket p) => p.Time.ToString("HH:mm:ss.fff") + "  " + p.Board.PadRight(22) + p.Destination.PadRight(17) + p.Payload + "\r\n";

        void Drain()
        {
            var sb = new StringBuilder();
            while (queue.TryDequeue(out var p))
            {
                if (chkPause.Checked) continue;                  // paused: the packets are dropped, not queued up
                total++;
                history.Add(p);
                if (Matches(p)) { sb.Append(Line(p)); shown++; }
            }
            if (history.Count > Keep) history.RemoveRange(0, history.Count - Keep);
            if (sb.Length == 0) return;
            txt.AppendText(sb.ToString());
            if (txt.TextLength > 600000)                         // keep the box small: drop the oldest third
            {
                string text = txt.Text;
                int cut = text.IndexOf('\n', text.Length / 3);
                txt.Text = cut >= 0 ? text.Substring(cut + 1) : text;
                txt.SelectionStart = txt.TextLength;
                txt.ScrollToCaret();
            }
            ShowCount();
        }

        // After a filter change: show the last packets that match, from the history.
        void Rebuild()
        {
            var sb = new StringBuilder();
            int n = 0;
            foreach (var p in history.Where(Matches).Reverse().Take(ShowLast).Reverse()) { sb.Append(Line(p)); n++; }
            txt.Text = sb.ToString();
            txt.SelectionStart = txt.TextLength;
            txt.ScrollToCaret();
            shown = n;
            ShowCount();
        }

        void ShowCount() => lblCount.Text = shown + " shown, " + total + " sent since this window opened" + (chkPause.Checked ? " (paused)" : "");
    }
}
