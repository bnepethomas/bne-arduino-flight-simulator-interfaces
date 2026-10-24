using System.Net.Sockets;
using System.Text;

namespace SwingTester
{
    internal enum BoardId
    {
        Stepper,      // JET_RANGER_STEPPER_CONTROLLER, single board
        DualStepper,  // JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER (also the OLEDs)
        Servo,        // JET_RANGER_SERVO_CONTROLLER (pitch/bank)
        Radio,        // JET_RANGER_RADIO_CONTROLLER (reset only)
        Upper,        // JET_RANGER_UPPER_CONTROLLER (reset only)
        Hid           // UDP_TO_HID (reset only; listens on 4210, not 13136)
    }

    /// <summary>One gauge's startup swing: which board, which UDP control command.</summary>
    internal sealed class Gauge
    {
        public string Name { get; }
        public BoardId Board { get; }
        public string Command { get; }
        public decimal DefaultSeconds { get; }

        public Gauge(string name, BoardId board, string command, decimal defaultSeconds)
        {
            Name = name;
            Board = board;
            Command = command;
            DefaultSeconds = defaultSeconds;
        }
    }

    public sealed class MainForm : Form
    {
        private const int Port = 13136;

        // Sequence order as requested. Each board runs its swing blocking, so
        // the app waits the per-row "seconds" before sending the next command.
        // The seconds are measured swing times plus ~1s margin (see PROGRAM_SUMMARY.md) - the boards don't
        // report completion over this link - and are editable per row.
        private static readonly Gauge[] Gauges =
        {
            new("Engine Oil Pressure",    BoardId.DualStepper, "SWINGOILP",           6),
            new("Engine Oil Temp",        BoardId.DualStepper, "SWINGOILT",           6),
            new("Transmission Pressure",  BoardId.DualStepper, "SWINGXMSNP",          6),
            new("Transmission Temp",      BoardId.DualStepper, "SWINGXMSNT",          6),
            new("Fuel Available",         BoardId.Stepper,     "SWINGFUEL",          11),
            new("Electrical Load",        BoardId.DualStepper, "SWINGELECTRICALLOAD", 7),
            new("Fuel Load",              BoardId.DualStepper, "SWINGFUELLOAD",       7),
            new("Torque",                 BoardId.Stepper,     "SWINGTQ",            11),
            new("Turbine Out (EGT/ITT)",  BoardId.Stepper,     "SWINGITT",           11),
            new("Gas Producer",           BoardId.Stepper,     "SWINGN1",            11),
            new("Pitch",                  BoardId.Servo,       "SWINGPITCH",          3),
            new("Roll",                   BoardId.Servo,       "SWINGBANK",           3),
            new("Turbine RPM",            BoardId.DualStepper, "SWINGRPME",           7),
            new("Rotor RPM",              BoardId.DualStepper, "SWINGRPMR",           7),
            new("VSI",                    BoardId.Stepper,     "SWINGVSI",           11),
            new("Airspeed",               BoardId.Stepper,     "SWINGIAS",           11),
            new("Radar Alt",              BoardId.Stepper,     "SWINGAGL",           11),
            new("Altimeter",              BoardId.DualStepper, "SWINGALT",            6),
        };

        // Warning lamps on the Servo board, driven by "D,<code>:1/0" data
        // packets. This is the order Swing ALL turns them OFF at the end.
        private static readonly (string Name, string Code)[] Lamps =
        {
            ("SC Fail",                "SCF"),
            ("Generator Fail",         "GENF"),
            ("Aft Fuel Filter",        "FFLTR"),
            ("Baggage Door",           "BD"),
            ("Engine Chip",            "EC"),
            ("Battery Hot",            "BHOT"),
            ("Transmission Oil Press", "TOPW"),
            ("Engine Out",             "EOUT"),
            ("Low Fuel",               "LOWF"),
            ("Fuel Pump",              "FPMP"),
            ("Tail Rotor Chip",        "TRC"),
            ("Transmission Chip",      "TC"),
            ("Battery Temp",           "BTMP"),
            ("Transmission Oil Temp",  "TOTW"),
            ("Rotor Low RPM",          "RLOW"),
        };

        // The 19 panel LEDs driven by the MAX7219 on the Radio controller (172.16.1.101): "D,LED_xxx:1/0" data packets. Auto Pilot panel
        // first, then Audio panel (see the Radio Auto Pilot PCB: digits 0-1 = Auto Pilot panel, digits 2-3 = Audio panel).
        private static readonly (string Panel, string Name, string Token)[] PanelLeds =
        {
            ("Auto Pilot", "CRS",        "LED_CRS"),
            ("Auto Pilot", "HDG",        "LED_HDG"),
            ("Auto Pilot", "IAS",        "LED_IAS"),
            ("Auto Pilot", "ALT",        "LED_ALT"),
            ("Auto Pilot", "APPR",       "LED_APPR"),
            ("Auto Pilot", "FD",         "LED_FD"),
            ("Auto Pilot", "BC",         "LED_BC"),
            ("Auto Pilot", "AP ENGAGE",  "LED_APENG"),
            ("Audio",      "COM1",       "LED_COM1"),
            ("Audio",      "BOTH",       "LED_BOTH"),
            ("Audio",      "NAV2",       "LED_NAV2"),
            ("Audio",      "DME",        "LED_DME"),
            ("Audio",      "OUTER",      "LED_OUTER"),
            ("Audio",      "INNER",      "LED_INNER"),
            ("Audio",      "COM2",       "LED_COM2"),
            ("Audio",      "NAV1",       "LED_NAV1"),
            ("Audio",      "MKR",        "LED_MKR"),
            ("Audio",      "ADF",        "LED_ADF"),
            ("Audio",      "MIDDLE",     "LED_MID"),
        };

        private readonly CheckBox[] ledChecks = new CheckBox[PanelLeds.Length];
        private bool suppressLedEvents;
        private readonly Button btnLedsOn = new() { Text = "All ON", AutoSize = true };
        private readonly Button btnLedsOff = new() { Text = "All OFF", AutoSize = true };
        private readonly Button btnLedChase = new() { Text = "Chase", AutoSize = true };
        private readonly CheckBox chkLedHardwareTest = new() { Text = "Board test (C,LEDTEST)", Appearance = Appearance.Button, AutoSize = true };
        private CancellationTokenSource? ledChaseCts;

        private readonly UdpClient udp = new UdpClient();
        private readonly TextBox txtStepperIp = new() { Text = "172.16.1.105", Width = 110 };
        private readonly TextBox txtDualIp = new() { Text = "172.16.1.106", Width = 110 };
        private readonly TextBox txtServoIp = new() { Text = "172.16.1.102", Width = 110 };
        private readonly TextBox txtRadioIp = new() { Text = "172.16.1.101", Width = 110 };
        private readonly TextBox txtUpperIp = new() { Text = "172.16.1.103", Width = 110 };
        private readonly TextBox txtHidIp = new() { Text = "172.16.1.11", Width = 110 };
        private readonly Button btnReset = new() { Text = "Reset boards...", AutoSize = true };
        private readonly Button btnSwingAll = new() { Text = "Swing ALL (in order)", AutoSize = true };
        private readonly Button btnStop = new() { Text = "Stop", AutoSize = true, Enabled = false };
        private readonly Button btnOledOn = new() { Text = "OLEDs ON", AutoSize = true };
        private readonly Button btnOledOff = new() { Text = "OLEDs OFF", AutoSize = true };
        private readonly TextBox txtTemp = new() { Text = "21C", Width = 60, MaxLength = 8 };
        private readonly Button btnTempSend = new() { Text = "Send TEMP", AutoSize = true };
        private readonly CheckBox chkOled = new() { Text = "Light OLEDs during Swing ALL", AutoSize = true, Checked = true };
        private readonly Button btnLampsOn = new() { Text = "Lamps ON (in order)", AutoSize = true };
        private readonly Button btnLampsOff = new() { Text = "Lamps OFF (in order)", AutoSize = true };
        private readonly CheckBox chkLamps = new() { Text = "Light warning lamps during Swing ALL", AutoSize = true, Checked = true };
        private readonly NumericUpDown numLampGapMs = new() { Minimum = 0, Maximum = 5000, Increment = 100, Value = 400, Width = 70 };
        private readonly NumericUpDown numLampOnGapMs = new() { Minimum = 0, Maximum = 10000, Increment = 100, Value = 1000, Width = 70 };
        // Backlight brightness (C,B:0-15) to every board except the USB HID board, and backlight power (D,BPWR) for testing the main bus rule.
        private static readonly BoardId[] BrightnessBoards = { BoardId.Radio, BoardId.Servo, BoardId.Upper, BoardId.Stepper, BoardId.DualStepper };
        private static readonly BoardId[] BacklightPowerBoards = { BoardId.Radio, BoardId.Servo, BoardId.Stepper, BoardId.DualStepper };
        private readonly TrackBar trkBrightness = new() { Minimum = 0, Maximum = 15, Value = 8, TickFrequency = 1, Width = 260, AutoSize = false, Height = 32 };
        private readonly Label lblBrightness = new() { Text = "8", AutoSize = true, Margin = new Padding(3, 6, 3, 0) };
        private readonly Button btnBrightnessSend = new() { Text = "Send to all boards", AutoSize = true };
        private readonly Button btnBacklightOn = new() { Text = "Backlight power ON", AutoSize = true };
        private readonly Button btnBacklightOff = new() { Text = "Backlight power OFF", AutoSize = true };
        private readonly Label lblStatus = new() { Text = "Ready.", AutoSize = true };
        private readonly CheckBox[] lampChecks = new CheckBox[Lamps.Length];
        private bool suppressLampEvents;
        private readonly Label[] rowLabels = new Label[Gauges.Length];
        private readonly NumericUpDown[] rowSeconds = new NumericUpDown[Gauges.Length];
        private readonly Button[] rowButtons = new Button[Gauges.Length];

        // Each board resets its gauges (and, on the Dual Stepper, blanks the OLEDs
        // and, on the Servo board, switches the lamps off) after 30s with no UDP
        // packet. A full run sends nothing to some boards for 100s+, so while a
        // run is active a harmless "C,KEEPALIVE:1" control packet (ignored by all
        // three sketches, but it counts as received data) goes to every board.
        private readonly System.Windows.Forms.Timer keepAliveTimer = new() { Interval = 10000 };

        private CancellationTokenSource? cts;
        private bool running;

        public MainForm()
        {
            Text = "Jet Ranger Swing Tester";
            StartPosition = FormStartPosition.CenterScreen;
            AutoScaleMode = AutoScaleMode.Dpi;
            Font = new Font("Segoe UI", 9f);
            int workHeight = Screen.PrimaryScreen?.WorkingArea.Height ?? 900;
            ClientSize = new Size(1130, Math.Min(900, workHeight - 80));
            MinimumSize = new Size(640, 400);

            var top = new FlowLayoutPanel
            {
                Dock = DockStyle.Top,
                AutoSize = true,
                Padding = new Padding(8, 8, 8, 0),
                WrapContents = true
            };
            top.Controls.Add(new Label { Text = "Stepper (.105)", AutoSize = true, Margin = new Padding(3, 6, 3, 0) });
            top.Controls.Add(txtStepperIp);
            top.Controls.Add(new Label { Text = "Dual Stepper (.106)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            top.Controls.Add(txtDualIp);
            top.Controls.Add(new Label { Text = "Servo (.102)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            top.Controls.Add(txtServoIp);
            top.Controls.Add(new Label { Text = "Radio (.101)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            top.Controls.Add(txtRadioIp);
            top.Controls.Add(new Label { Text = "Upper (.103)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            top.Controls.Add(txtUpperIp);
            top.Controls.Add(new Label { Text = "HID (.11)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            top.Controls.Add(txtHidIp);

            var actions = new FlowLayoutPanel
            {
                Dock = DockStyle.Top,
                AutoSize = true,
                Padding = new Padding(8, 4, 8, 0),
                WrapContents = true
            };
            actions.Controls.Add(btnSwingAll);
            actions.Controls.Add(btnStop);
            actions.Controls.Add(btnOledOn);
            actions.Controls.Add(btnOledOff);
            actions.Controls.Add(btnReset);
            actions.Controls.Add(chkOled);
            chkOled.Margin = new Padding(12, 6, 3, 0);
            actions.Controls.Add(new Label { Text = "Temp OLED:", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            actions.Controls.Add(txtTemp);
            actions.Controls.Add(btnTempSend);

            var lampActions = new FlowLayoutPanel
            {
                Dock = DockStyle.Top,
                AutoSize = true,
                Padding = new Padding(8, 0, 8, 0),
                WrapContents = true
            };
            lampActions.Controls.Add(btnLampsOn);
            lampActions.Controls.Add(btnLampsOff);
            lampActions.Controls.Add(new Label { Text = "on gap (ms)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            lampActions.Controls.Add(numLampOnGapMs);
            lampActions.Controls.Add(new Label { Text = "off gap (ms)", AutoSize = true, Margin = new Padding(12, 6, 3, 0) });
            lampActions.Controls.Add(numLampGapMs);
            lampActions.Controls.Add(chkLamps);
            chkLamps.Margin = new Padding(12, 6, 3, 0);

            var brightPanel = new FlowLayoutPanel
            {
                Dock = DockStyle.Top,
                AutoSize = true,
                Padding = new Padding(8, 0, 8, 0),
                WrapContents = true
            };
            brightPanel.Controls.Add(new Label { Text = "Brightness (0-15):", AutoSize = true, Margin = new Padding(3, 6, 3, 0) });
            brightPanel.Controls.Add(trkBrightness);
            brightPanel.Controls.Add(lblBrightness);
            brightPanel.Controls.Add(btnBrightnessSend);
            brightPanel.Controls.Add(new Label { Text = "(not the HID board)", AutoSize = true, ForeColor = SystemColors.GrayText, Margin = new Padding(3, 6, 12, 0) });
            brightPanel.Controls.Add(btnBacklightOn);
            brightPanel.Controls.Add(btnBacklightOff);

            var statusPanel = new Panel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 6, 8, 6) };
            statusPanel.Controls.Add(lblStatus);

            var grid = new TableLayoutPanel
            {
                Anchor = AnchorStyles.Top | AnchorStyles.Left,
                AutoSize = true,
                ColumnCount = 5,
                Padding = new Padding(8, 0, 8, 8)
            };
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            grid.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            AddHeader(grid, "#");
            AddHeader(grid, "Gauge");
            AddHeader(grid, "Board");
            AddHeader(grid, "Seconds");
            AddHeader(grid, "");

            for (int i = 0; i < Gauges.Length; i++)
            {
                int index = i;
                var g = Gauges[i];
                grid.Controls.Add(new Label { Text = (i + 1).ToString(), AutoSize = true, Margin = new Padding(3, 8, 8, 3) });
                var name = new Label { Text = g.Name, AutoSize = true, Margin = new Padding(3, 8, 12, 3), Padding = new Padding(2) };
                rowLabels[i] = name;
                grid.Controls.Add(name);
                grid.Controls.Add(new Label { Text = BoardText(g.Board), AutoSize = true, Margin = new Padding(3, 8, 12, 3) });
                var secs = new NumericUpDown
                {
                    Minimum = 1,
                    Maximum = 120,
                    DecimalPlaces = 0,
                    Value = g.DefaultSeconds,
                    Width = 60
                };
                rowSeconds[i] = secs;
                grid.Controls.Add(secs);
                var btn = new Button { Text = "Swing", AutoSize = true, Margin = new Padding(8, 3, 3, 3) };
                btn.Click += async (_, _) => await RunAsync(new[] { index }, oledsDuring: false);
                rowButtons[i] = btn;
                grid.Controls.Add(btn);
            }

            // Warning lamps: one toggle button per lamp, listed in the order
            // Swing ALL turns them off.
            var lampGroup = new GroupBox
            {
                Text = "Warning lamps (turn-off order)",
                Anchor = AnchorStyles.Top | AnchorStyles.Left,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Padding = new Padding(8, 4, 8, 8)
            };
            var lampFlow = new FlowLayoutPanel { Location = new Point(10, 22), AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = false, FlowDirection = FlowDirection.TopDown };
            for (int i = 0; i < Lamps.Length; i++)
            {
                int index = i;
                var chk = new CheckBox
                {
                    Text = $"{i + 1}. {Lamps[i].Name}",
                    Appearance = Appearance.Button,
                    AutoSize = false,
                    Size = new Size(210, 28),
                    TextAlign = ContentAlignment.MiddleLeft,
                    Margin = new Padding(3, 3, 6, 3)
                };
                chk.CheckedChanged += async (_, _) =>
                {
                    if (suppressLampEvents) return;
                    chk.BackColor = chk.Checked ? Color.Gold : Color.Empty;
                    await SendLampAsync(index, chk.Checked);
                };
                lampChecks[i] = chk;
                lampFlow.Controls.Add(chk);
            }
            lampGroup.Controls.Add(lampFlow);

            // Gauges on the left, lamps on the right, side by side so all of
            // them are visible together; the panel still scrolls (nothing in it
            // is docked, so AutoScroll sees the real content size) if the
            // window is made smaller than the content.
            var content = new TableLayoutPanel
            {
                ColumnCount = 2,
                RowCount = 1,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Location = new Point(0, 0)
            };
            content.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            content.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            content.Controls.Add(grid, 0, 0);
            content.Controls.Add(lampGroup, 1, 0);

            var scroll = new Panel { Dock = DockStyle.Fill, AutoScroll = true };
            scroll.Controls.Add(content);

            // Panel LEDs (Radio controller's MAX7219): each one can be lit on its own, all at once, in a chase, or with the board's own test.
            var ledGroup = new GroupBox
            {
                Text = "Panel LEDs (Radio .101, MAX7219)",
                Anchor = AnchorStyles.Top | AnchorStyles.Left,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Padding = new Padding(8, 4, 8, 8)
            };
            var ledOuter = new FlowLayoutPanel { Location = new Point(10, 22), AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = false, FlowDirection = FlowDirection.TopDown };
            var ledButtons = new FlowLayoutPanel { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = true, Width = 300 };
            ledButtons.Controls.Add(btnLedsOn);
            ledButtons.Controls.Add(btnLedsOff);
            ledButtons.Controls.Add(btnLedChase);
            ledButtons.Controls.Add(chkLedHardwareTest);
            ledOuter.Controls.Add(ledButtons);
            var ledColumns = new FlowLayoutPanel { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = false, FlowDirection = FlowDirection.LeftToRight };
            foreach (string panel in new[] { "Auto Pilot", "Audio" })
            {
                var column = new FlowLayoutPanel { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = false, FlowDirection = FlowDirection.TopDown };
                column.Controls.Add(new Label { Text = panel + " panel", AutoSize = true, Font = new Font("Segoe UI", 9f, FontStyle.Bold), Margin = new Padding(3, 6, 3, 3) });
                for (int i = 0; i < PanelLeds.Length; i++)
                {
                    if (PanelLeds[i].Panel != panel) continue;
                    int index = i;
                    var chk = new CheckBox
                    {
                        Text = PanelLeds[i].Name,
                        Appearance = Appearance.Button,
                        AutoSize = false,
                        Size = new Size(130, 28),
                        TextAlign = ContentAlignment.MiddleLeft,
                        Margin = new Padding(3, 3, 6, 3)
                    };
                    chk.CheckedChanged += async (_, _) =>
                    {
                        if (suppressLedEvents) return;
                        chk.BackColor = chk.Checked ? Color.Gold : Color.Empty;
                        await SendPanelLedAsync(index, chk.Checked);
                    };
                    ledChecks[i] = chk;
                    column.Controls.Add(chk);
                }
                ledColumns.Controls.Add(column);
            }
            ledOuter.Controls.Add(ledColumns);
            ledGroup.Controls.Add(ledOuter);
            content.ColumnCount = 3;
            content.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            content.Controls.Add(ledGroup, 2, 0);

            Controls.Add(scroll);
            Controls.Add(statusPanel);
            Controls.Add(brightPanel);
            Controls.Add(lampActions);
            Controls.Add(actions);
            Controls.Add(top);

            btnSwingAll.Click += async (_, _) => await RunAsync(Enumerable.Range(0, Gauges.Length), chkOled.Checked, fullSequence: true);
            btnStop.Click += (_, _) => { cts?.Cancel(); lblStatus.Text = "Stopping after the current wait..."; };
            btnOledOn.Click += async (_, _) => await SendOledAsync(true);
            btnOledOff.Click += async (_, _) => await SendOledAsync(false);
            btnTempSend.Click += async (_, _) => await SendTempAsync();
            trkBrightness.Scroll += async (_, _) => { lblBrightness.Text = trkBrightness.Value.ToString(); await SendBrightnessAsync(); };
            btnBrightnessSend.Click += async (_, _) => await SendBrightnessAsync();
            btnBacklightOn.Click += async (_, _) => await SendBacklightPowerAsync(true);
            btnBacklightOff.Click += async (_, _) => await SendBacklightPowerAsync(false);
            keepAliveTimer.Tick += async (_, _) => await SendKeepAliveAsync();
            btnReset.Click += async (_, _) => await ResetBoardsAsync();
            btnLampsOn.Click += async (_, _) => await RunLampsOnAsync();
            btnLampsOff.Click += async (_, _) => await TurnLampsOffInOrderAsync();
            btnLedsOn.Click += async (_, _) => await SetAllPanelLedsAsync(true);
            btnLedsOff.Click += async (_, _) => await SetAllPanelLedsAsync(false);
            btnLedChase.Click += async (_, _) => await ToggleLedChaseAsync();
            chkLedHardwareTest.CheckedChanged += async (_, _) => await SendLedHardwareTestAsync(chkLedHardwareTest.Checked);
        }

        private static void AddHeader(TableLayoutPanel grid, string text) =>
            grid.Controls.Add(new Label
            {
                Text = text,
                AutoSize = true,
                Font = new Font("Segoe UI", 9f, FontStyle.Bold),
                Margin = new Padding(3, 3, 12, 3)
            });

        private static string BoardText(BoardId b) => b switch
        {
            BoardId.Stepper => "Stepper .105",
            BoardId.DualStepper => "Dual Stepper .106",
            BoardId.Radio => "Radio .101",
            BoardId.Upper => "Upper .103",
            BoardId.Hid => "HID .11",
            _ => "Servo .102"
        };

        private string IpFor(BoardId b) => b switch
        {
            BoardId.Stepper => txtStepperIp.Text.Trim(),
            BoardId.DualStepper => txtDualIp.Text.Trim(),
            BoardId.Radio => txtRadioIp.Text.Trim(),
            BoardId.Upper => txtUpperIp.Text.Trim(),
            BoardId.Hid => txtHidIp.Text.Trim(),
            _ => txtServoIp.Text.Trim()
        };

        /// <summary>The UDP_TO_HID board listens on 4210; every other board on 13136.</summary>
        private static int PortFor(BoardId b) => b == BoardId.Hid ? 4210 : Port;

        /// <summary>Sends "C,&lt;command&gt;:1" (a control packet) to the board.</summary>
        private async Task SendCommandAsync(BoardId board, string command)
        {
            byte[] data = Encoding.ASCII.GetBytes($"C,{command}:1");
            await udp.SendAsync(data, data.Length, IpFor(board), Port);
        }

        /// <summary>Sends a packet (data or control) to a board.</summary>
        private async Task SendPacketAsync(BoardId board, string text)
        {
            byte[] data = Encoding.ASCII.GetBytes(text);
            await udp.SendAsync(data, data.Length, IpFor(board), PortFor(board));
        }

        /// <summary>Sends "C,B:&lt;0-15&gt;" (backlight brightness) to every board except the USB HID board.</summary>
        private async Task SendBrightnessAsync()
        {
            int level = trkBrightness.Value;
            var sent = new List<string>();
            var failed = new List<string>();
            foreach (var board in BrightnessBoards)
            {
                try { await SendPacketAsync(board, $"C,B:{level}"); sent.Add(BoardText(board)); }
                catch (Exception ex) { failed.Add(BoardText(board) + " (" + ex.Message + ")"); }
            }
            lblStatus.Text = $"Brightness {level} sent to: " + string.Join(", ", sent) +
                (failed.Count > 0 ? ". FAILED: " + string.Join(", ", failed) : ".");
        }

        /// <summary>
        /// Sends "D,BPWR:1/0" (backlight power - what the bridges send from the main bus voltage) to the Radio, Servo, Stepper and Dual Stepper.
        /// The boards turn the backlight off at 0 and, on 1, return to the last brightness commanded.
        /// </summary>
        private async Task SendBacklightPowerAsync(bool on)
        {
            var sent = new List<string>();
            var failed = new List<string>();
            foreach (var board in BacklightPowerBoards)
            {
                try { await SendPacketAsync(board, "D,BPWR:" + (on ? "1" : "0")); sent.Add(BoardText(board)); }
                catch (Exception ex) { failed.Add(BoardText(board) + " (" + ex.Message + ")"); }
            }
            lblStatus.Text = "Backlight power " + (on ? "ON" : "OFF") + " sent to: " + string.Join(", ", sent) +
                (failed.Count > 0 ? ". FAILED: " + string.Join(", ", failed) : ".");
        }

        /// <summary>Sends "D,TEMP:&lt;text&gt;" (e.g. 21C) to the Dual Stepper's Temperature OLED.</summary>
        private async Task SendTempAsync()
        {
            string text = txtTemp.Text.Trim();
            if (text.Length == 0)
            {
                lblStatus.Text = "Enter temperature text (e.g. 21C).";
                return;
            }
            try
            {
                byte[] data = Encoding.ASCII.GetBytes("D,TEMP:" + text);
                await udp.SendAsync(data, data.Length, IpFor(BoardId.DualStepper), Port);
                lblStatus.Text = "Temperature OLED set to " + text + ".";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "TEMP send failed: " + ex.Message;
            }
        }

        /// <summary>
        /// Asks which boards to reset (all ticked by default - untick any that are not connected),
        /// then sends "C,RESET:1" to each. A board reboots about 2 s later through its watchdog; the
        /// Dual Stepper then takes about a minute to finish its start-up swings, and the HID board
        /// briefly drops its USB joysticks and keyboard.
        /// </summary>
        private async Task ResetBoardsAsync()
        {
            if (running) return;
            var boards = new[] { BoardId.Radio, BoardId.Servo, BoardId.Upper, BoardId.Stepper, BoardId.DualStepper, BoardId.Hid };
            var chosen = PickBoardsToReset(boards);
            if (chosen.Count == 0) return;

            byte[] data = Encoding.ASCII.GetBytes("C,RESET:1");
            var sent = new List<string>();
            var failed = new List<string>();
            foreach (var board in chosen)
            {
                try
                {
                    await udp.SendAsync(data, data.Length, IpFor(board), PortFor(board));
                    sent.Add(BoardText(board));
                }
                catch (Exception ex)
                {
                    failed.Add(BoardText(board) + " (" + ex.Message + ")");
                }
            }
            lblStatus.Text = "Reset sent to: " + string.Join(", ", sent) +
                (failed.Count > 0 ? ". FAILED: " + string.Join(", ", failed) : ".");
        }

        private List<BoardId> PickBoardsToReset(BoardId[] boards)
        {
            using var dlg = new Form
            {
                Text = "Reset boards",
                FormBorderStyle = FormBorderStyle.FixedDialog,
                StartPosition = FormStartPosition.CenterParent,
                MinimizeBox = false,
                MaximizeBox = false,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Padding = new Padding(12)
            };
            var flow = new FlowLayoutPanel { FlowDirection = FlowDirection.TopDown, AutoSize = true, WrapContents = false, Dock = DockStyle.Fill };
            flow.Controls.Add(new Label
            {
                Text = "Tick the boards to reset. Each reboots about 2 seconds later and runs its" + Environment.NewLine +
                       "start-up again (the Dual Stepper takes about a minute; the HID board" + Environment.NewLine +
                       "drops its USB joysticks and keyboard briefly). Untick any that are not connected.",
                AutoSize = true,
                Margin = new Padding(0, 0, 0, 8)
            });
            var checks = new Dictionary<BoardId, CheckBox>();
            foreach (var b in boards)
            {
                var c = new CheckBox { Text = BoardText(b) + "  (" + IpFor(b) + ")", Checked = true, AutoSize = true };
                checks[b] = c;
                flow.Controls.Add(c);
            }
            var buttons = new FlowLayoutPanel { FlowDirection = FlowDirection.LeftToRight, AutoSize = true, Margin = new Padding(0, 10, 0, 0) };
            var ok = new Button { Text = "Reset", DialogResult = DialogResult.OK, AutoSize = true };
            var cancel = new Button { Text = "Cancel", DialogResult = DialogResult.Cancel, AutoSize = true };
            buttons.Controls.Add(ok);
            buttons.Controls.Add(cancel);
            flow.Controls.Add(buttons);
            dlg.Controls.Add(flow);
            dlg.AcceptButton = null;
            dlg.CancelButton = cancel;

            if (dlg.ShowDialog(this) != DialogResult.OK) return new List<BoardId>();
            return boards.Where(b => checks[b].Checked).ToList();
        }

        private async Task SendOledAsync(bool on, bool quiet = false)
        {
            try
            {
                byte[] data = Encoding.ASCII.GetBytes($"C,OLEDTEST:{(on ? 1 : 0)}");
                await udp.SendAsync(data, data.Length, IpFor(BoardId.DualStepper), Port);
                if (!quiet) lblStatus.Text = on ? "OLEDs lit (OLEDTEST:1 sent)." : "OLEDs restored (OLEDTEST:0 sent).";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "OLED send failed: " + ex.Message;
            }
        }

        private void SetLedCheck(int index, bool on)
        {
            suppressLedEvents = true;
            ledChecks[index].Checked = on;
            ledChecks[index].BackColor = on ? Color.Gold : Color.Empty;
            suppressLedEvents = false;
        }

        /// <summary>
        /// Lights or clears one panel LED: "D,NAVCOM1:1,LED_xxx:1/0" to the Radio controller. NAVCOM1:1 keeps the radio's power flag on
        /// (the LEDs follow it, like the COM and NAV displays); the radio remembers every LED's state, so only the changed one is sent.
        /// </summary>
        private async Task SendPanelLedAsync(int index, bool on)
        {
            try
            {
                await SendPacketAsync(BoardId.Radio, $"D,NAVCOM1:1,{PanelLeds[index].Token}:{(on ? 1 : 0)}");
                lblStatus.Text = $"LED {PanelLeds[index].Panel} panel {PanelLeds[index].Name}: {(on ? "ON" : "OFF")} ({PanelLeds[index].Token}).";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "LED send failed: " + ex.Message;
            }
        }

        /// <summary>All 19 panel LEDs on (or off) in one packet.</summary>
        private async Task SetAllPanelLedsAsync(bool on)
        {
            try
            {
                string body = string.Join(",", PanelLeds.Select(l => $"{l.Token}:{(on ? 1 : 0)}"));
                await SendPacketAsync(BoardId.Radio, "D,NAVCOM1:1," + body);
                for (int i = 0; i < PanelLeds.Length; i++) SetLedCheck(i, on);
                lblStatus.Text = on ? "All panel LEDs ON." : "All panel LEDs OFF.";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "LED send failed: " + ex.Message;
            }
        }

        /// <summary>Lights each LED on its own, in table order, for the "on gap" time from the box (default 1 s), then clears them all.</summary>
        private async Task ToggleLedChaseAsync()
        {
            if (ledChaseCts != null)
            {
                ledChaseCts.Cancel();
                return;
            }
            ledChaseCts = new CancellationTokenSource();
            var token = ledChaseCts.Token;
            btnLedChase.Text = "Stop chase";
            try
            {
                for (int i = 0; i < PanelLeds.Length; i++)
                {
                    string body = string.Join(",", PanelLeds.Select((l, n) => $"{l.Token}:{(n == i ? 1 : 0)}"));
                    await SendPacketAsync(BoardId.Radio, "D,NAVCOM1:1," + body);
                    for (int n = 0; n < PanelLeds.Length; n++) SetLedCheck(n, n == i);
                    lblStatus.Text = $"LED chase: {PanelLeds[i].Panel} panel {PanelLeds[i].Name} ({PanelLeds[i].Token}).";
                    await Task.Delay((int)numLampOnGapMs.Value, token);
                }
                lblStatus.Text = "LED chase finished.";
            }
            catch (OperationCanceledException)
            {
                lblStatus.Text = "LED chase stopped.";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "LED send failed: " + ex.Message;
            }
            finally
            {
                try { await SetAllPanelLedsAsync(false); } catch { }
                ledChaseCts.Dispose();
                ledChaseCts = null;
                btnLedChase.Text = "Chase";
            }
        }

        /// <summary>The board's own test: "C,LEDTEST:1" lights all 19 LEDs at the MAX7219, "C,LEDTEST:0" returns them to the last sent state.</summary>
        private async Task SendLedHardwareTestAsync(bool on)
        {
            try
            {
                await SendPacketAsync(BoardId.Radio, $"C,LEDTEST:{(on ? 1 : 0)}");
                lblStatus.Text = on ? "Board LED test ON (C,LEDTEST:1)." : "Board LED test OFF (C,LEDTEST:0).";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "LED send failed: " + ex.Message;
            }
        }

        private void SetLampCheck(int index, bool on)
        {
            suppressLampEvents = true;
            lampChecks[index].Checked = on;
            lampChecks[index].BackColor = on ? Color.Gold : Color.Empty;
            suppressLampEvents = false;
        }

        /// <summary>Sends "D,&lt;code&gt;:1/0" (a data packet) to the Servo board for one lamp.</summary>
        private async Task SendLampAsync(int index, bool on)
        {
            try
            {
                byte[] data = Encoding.ASCII.GetBytes($"D,{Lamps[index].Code}:{(on ? 1 : 0)}");
                await udp.SendAsync(data, data.Length, IpFor(BoardId.Servo), Port);
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Lamp send failed: " + ex.Message;
            }
        }

        /// <summary>All 15 lamps on (or off) in a single packet.</summary>
        private async Task SetAllLampsAsync(bool on)
        {
            try
            {
                string body = string.Join(",", Lamps.Select(l => $"{l.Code}:{(on ? 1 : 0)}"));
                byte[] data = Encoding.ASCII.GetBytes("D," + body);
                await udp.SendAsync(data, data.Length, IpFor(BoardId.Servo), Port);
                for (int i = 0; i < Lamps.Length; i++) SetLampCheck(i, on);
                lblStatus.Text = on ? "All warning lamps ON." : "All warning lamps OFF.";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Lamp send failed: " + ex.Message;
            }
        }

        /// <summary>Turns each lamp on one at a time in the Lamps[] order, "on gap" (default 1s) per step.</summary>
        private async Task TurnLampsOnInOrderAsync(CancellationToken token = default)
        {
            for (int i = 0; i < Lamps.Length; i++)
            {
                lblStatus.Text = $"Lamp on: {Lamps[i].Name}";
                await SendLampAsync(i, true);
                SetLampCheck(i, true);
                await Task.Delay((int)numLampOnGapMs.Value, token);
            }
            lblStatus.Text = "All warning lamps ON.";
        }

        private async Task RunLampsOnAsync()
        {
            if (running) return;
            SetRunning(true);
            try { await TurnLampsOnInOrderAsync(); }
            catch (Exception ex) { lblStatus.Text = "Error: " + ex.Message; }
            finally { SetRunning(false); }
        }

        /// <summary>Turns each lamp off one at a time in the Lamps[] order, with the gap from the box.</summary>
        private async Task TurnLampsOffInOrderAsync()
        {
            for (int i = 0; i < Lamps.Length; i++)
            {
                lblStatus.Text = $"Lamp off: {Lamps[i].Name}";
                await SendLampAsync(i, false);
                SetLampCheck(i, false);
                await Task.Delay((int)numLampGapMs.Value);
            }
            lblStatus.Text = "All warning lamps OFF.";
        }

        private async Task SendKeepAliveAsync()
        {
            try
            {
                byte[] data = Encoding.ASCII.GetBytes("C,KEEPALIVE:1");
                foreach (var board in new[] { BoardId.Stepper, BoardId.DualStepper, BoardId.Servo })
                    await udp.SendAsync(data, data.Length, IpFor(board), Port);
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Keepalive send failed: " + ex.Message;
            }
        }

        private void SetRunning(bool value)
        {
            running = value;
            keepAliveTimer.Enabled = value;
            if (value) _ = SendKeepAliveAsync();
            btnSwingAll.Enabled = !value;
            btnStop.Enabled = value;
            btnLampsOn.Enabled = !value;
            btnLampsOff.Enabled = !value;
            btnReset.Enabled = !value;
            foreach (var b in rowButtons) b.Enabled = !value;
        }

        /// <summary>Runs the given gauge indexes in order, waiting each row's seconds after sending.</summary>
        private async Task RunAsync(IEnumerable<int> indexes, bool oledsDuring, bool fullSequence = false)
        {
            if (running) return;
            SetRunning(true);
            cts = new CancellationTokenSource();
            var token = cts.Token;
            bool oledsLit = false;
            try
            {
                if (fullSequence && chkLamps.Checked)
                {
                    await TurnLampsOnInOrderAsync(token);
                }

                if (oledsDuring)
                {
                    await SendOledAsync(true, quiet: true);
                    oledsLit = true;
                    await Task.Delay(500, token);
                }

                foreach (int i in indexes)
                {
                    var g = Gauges[i];
                    rowLabels[i].BackColor = Color.Gold;
                    await SendCommandAsync(g.Board, g.Command);
                    int seconds = (int)rowSeconds[i].Value;
                    for (int remaining = seconds * 10; remaining > 0; remaining--)
                    {
                        lblStatus.Text = $"Swinging {g.Name} ({BoardText(g.Board)}, {g.Command}) - {remaining / 10.0:0.0}s left";
                        await Task.Delay(100, token);
                    }
                    rowLabels[i].BackColor = Color.Empty;
                }
                lblStatus.Text = "Done.";
            }
            catch (OperationCanceledException)
            {
                lblStatus.Text = "Stopped. (A swing already started on a board will still finish.)";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Error: " + ex.Message;
            }
            finally
            {
                foreach (var l in rowLabels) l.BackColor = Color.Empty;
                if (oledsLit) await SendOledAsync(false, quiet: true);
                // End of Swing ALL: lamps off, one at a time, in the fixed order.
                if (fullSequence) await TurnLampsOffInOrderAsync();
                SetRunning(false);
                cts.Dispose();
                cts = null;
            }
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing) udp.Dispose();
            base.Dispose(disposing);
        }
    }
}
