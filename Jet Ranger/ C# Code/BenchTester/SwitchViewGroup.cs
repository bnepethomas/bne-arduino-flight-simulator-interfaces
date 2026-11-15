using System.Net;
using System.Net.Sockets;
using System.Text;

namespace BenchTester
{
    /// <summary>
    /// Live state of the overhead (Upper) switch matrix on the ADF and Upper controller (172.16.1.107). The board (firmware build 15 and later) sends each
    /// matrix change as "UPPER_SW:&lt;index&gt;:&lt;0|1&gt;" and a full snapshot every 5 seconds as "UPPER_SNAP:&lt;72 characters of 0 and 1&gt;" (9 rows of 8 columns, row by row)
    /// to this PC on UDP port 27004. The index is row x 11 + column, the same number the board sends the USB HID board as a joystick button.
    /// A closed switch (to ground) is shown green, an open one grey and one not heard of yet as a dashed outline.
    /// </summary>
    internal sealed class SwitchViewGroup : GroupBox
    {
        public const int ListenPort = 27004;
        private const int Rows = 9, Cols = 8, Stride = 11;

        // The switches that have a name in the master document's mapping table (UpperSwitchMapping.ods), and the two the PCB itself carries on row 8.
        private static readonly Dictionary<int, string> Names = new()
        {
            [0] = "HTR 0", [1] = "HTR 1", [2] = "HTR 2", [3] = "HTR 3", [4] = "HTR 4", [5] = "Air vent", [6] = "Air heat",
            [11] = "Fwd fuel CB", [12] = "Aft fuel CB", [13] = "Gen field CB", [14] = "Gen CB", [15] = "Dir gyro", [16] = "Battery", [17] = "Generator",
            [22] = "Landing lt", [23] = "Instr lts", [24] = "Caution CB", [25] = "Anti-coll", [26] = "Position", [27] = "Pitot heat", [28] = "Avionics",
            [38] = "SAS CB", [39] = "Strobe",
            [88] = "Warn mute", [89] = "TOT test",
        };

        private readonly Label[,] cells = new Label[Rows, Cols];
        private readonly bool?[,] state = new bool?[Rows, Cols];
        private readonly Label lblLast = new() { Text = "No switch change heard yet.", AutoSize = true, Margin = new Padding(3, 6, 3, 0) };
        private readonly Label lblLink = new() { AutoSize = true, Margin = new Padding(3, 2, 3, 0), ForeColor = Color.DimGray };
        private readonly System.Windows.Forms.Timer clock = new() { Interval = 1000 };
        private readonly CancellationTokenSource cts = new();
        private UdpClient? listener;
        private DateTime lastPacket = DateTime.MinValue;
        private string? listenError;

        public SwitchViewGroup()
        {
            Text = "Overhead switches, live (ADF and Upper .107, UDP " + ListenPort + ")";
            Anchor = AnchorStyles.Top | AnchorStyles.Left;
            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            Padding = new Padding(8, 4, 8, 8);

            var flow = new FlowLayoutPanel { Location = new Point(10, 22), AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = false, FlowDirection = FlowDirection.TopDown };
            var grid = new TableLayoutPanel { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = Cols, RowCount = Rows, Margin = new Padding(0) };
            var tip = new ToolTip();
            for (int r = 0; r < Rows; r++)
            {
                for (int c = 0; c < Cols; c++)
                {
                    int index = r * Stride + c;
                    var cell = new Label
                    {
                        AutoSize = false,
                        Size = new Size(66, 34),
                        TextAlign = ContentAlignment.MiddleCenter,
                        BorderStyle = BorderStyle.FixedSingle,
                        Margin = new Padding(1),
                        Font = new Font("Segoe UI", 7.5f),
                    };
                    tip.SetToolTip(cell, "Index " + index + " (row " + r + ", column " + c + "), joystick " + (index / 32 + 1) + " button " + (index % 32 + 1)
                        + (Names.TryGetValue(index, out var n) ? ": " + n : ": not assigned"));
                    cells[r, c] = cell;
                    grid.Controls.Add(cell, c, r);
                    Paint(r, c);
                }
            }
            flow.Controls.Add(grid);
            flow.Controls.Add(lblLast);
            flow.Controls.Add(lblLink);
            Controls.Add(flow);

            clock.Tick += (_, _) => UpdateLink();
            clock.Start();
            StartListening();
            UpdateLink();
        }

        private void StartListening()
        {
            try
            {
                listener = new UdpClient(new IPEndPoint(IPAddress.Any, ListenPort));
                _ = Task.Run(() => ReceiveLoopAsync(listener, cts.Token));
            }
            catch (SocketException ex)
            {
                listenError = "UDP " + ListenPort + " is not available (" + ex.SocketErrorCode + "): is another BenchTester running?";
            }
        }

        private async Task ReceiveLoopAsync(UdpClient client, CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    var result = await client.ReceiveAsync(token);
                    string text = Encoding.ASCII.GetString(result.Buffer).Trim();
                    if (!IsHandleCreated || IsDisposed) continue;
                    BeginInvoke(new Action(() => Handle(text)));
                }
                catch (OperationCanceledException) { return; }
                catch (ObjectDisposedException) { return; }
                catch (SocketException) { await Task.Delay(200, CancellationToken.None); }
            }
        }

        private void Handle(string text)
        {
            if (text.StartsWith("UPPER_SW:", StringComparison.Ordinal))
            {
                var parts = text.Substring("UPPER_SW:".Length).Split(':');
                if (parts.Length == 2 && int.TryParse(parts[0], out int index) && (parts[1] == "0" || parts[1] == "1") && TryCell(index, out int r, out int c))
                {
                    lastPacket = DateTime.Now;
                    bool closed = parts[1] == "1";
                    state[r, c] = closed;
                    Paint(r, c);
                    string name = Names.TryGetValue(index, out var n) ? " " + n : "";
                    lblLast.Text = "Last change: " + index + name + " " + (closed ? "closed" : "open") + " at " + DateTime.Now.ToString("HH:mm:ss");
                }
            }
            else if (text.StartsWith("UPPER_SNAP:", StringComparison.Ordinal))
            {
                string bits = text.Substring("UPPER_SNAP:".Length);
                if (bits.Length != Rows * Cols) return;
                lastPacket = DateTime.Now;
                for (int r = 0; r < Rows; r++)
                {
                    for (int c = 0; c < Cols; c++)
                    {
                        char ch = bits[r * Cols + c];
                        if (ch != '0' && ch != '1') continue;
                        state[r, c] = ch == '1';
                        Paint(r, c);
                    }
                }
            }
            UpdateLink();
        }

        private static bool TryCell(int index, out int row, out int col)
        {
            row = index / Stride;
            col = index % Stride;
            return index >= 0 && row < Rows && col < Cols;
        }

        private void Paint(int r, int c)
        {
            int index = r * Stride + c;
            var cell = cells[r, c];
            string name = Names.TryGetValue(index, out var n) ? n : "";
            cell.Text = index + (name.Length > 0 ? Environment.NewLine + name : "");
            switch (state[r, c])
            {
                case true: cell.BackColor = Color.LimeGreen; cell.ForeColor = Color.Black; break;
                case false: cell.BackColor = Color.Gainsboro; cell.ForeColor = Color.Black; break;
                default: cell.BackColor = Color.White; cell.ForeColor = Color.Gray; break;
            }
            cell.Font = new Font("Segoe UI", 7.5f, name.Length > 0 ? FontStyle.Bold : FontStyle.Regular);
        }

        private void UpdateLink()
        {
            if (listenError != null) { lblLink.Text = listenError; lblLink.ForeColor = Color.Firebrick; return; }
            if (lastPacket == DateTime.MinValue)
            {
                lblLink.Text = "Waiting for the board (ADF firmware build 15 or later sends the states every 5 s).";
                lblLink.ForeColor = Color.DimGray;
                return;
            }
            int age = (int)(DateTime.Now - lastPacket).TotalSeconds;
            bool stale = age > 12;
            lblLink.Text = stale ? "No packet for " + age + " s: the ADF board is off, or the PC address is not 172.16.1.10." : "Link OK (last packet " + age + " s ago). Green = closed to ground.";
            lblLink.ForeColor = stale ? Color.Firebrick : Color.DimGray;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                cts.Cancel();
                clock.Dispose();
                listener?.Dispose();
                cts.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
