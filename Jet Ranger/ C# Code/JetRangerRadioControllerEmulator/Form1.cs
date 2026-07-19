using System.Net;
using System.Net.Sockets;
using System.Text;

namespace JetRangerRadioControllerEmulator
{
    public partial class frmMain : Form
    {
        // Mirrors one COM/NAV radio's tuning state from JET_RANGER_RADIO_CONTROLLER.ino.
        // Major/Minor track the two rotary encoders' detent counts directly
        // (Major = whole-unit steps from BaseMHz, Minor = 5 kHz steps 0-199),
        // matching the encoder ranges configured in the sketch (Comm: 17 major
        // steps from 118, Nav: 10 major steps from 108). Standby is derived the
        // same way the sketch computes convertedchann.
        private sealed class RadioState
        {
            public required string Name;
            public int BaseMHz;
            public int MajorMax;
            public int Major;
            public int Minor;
            public string Active = "";

            public string Standby
            {
                get
                {
                    int freqNum = (BaseMHz + Major) * 1000 + Minor * 5;
                    return (freqNum / 1000).ToString("000") + "." + (freqNum % 1000).ToString("000");
                }
            }

            // Points the dial at the given "XXX.XXX" frequency string, used
            // when swapping active/standby.
            public void SetStandbyFrom(string freq)
            {
                var parts = freq.Split('.');
                if (parts.Length != 2) return;
                if (!int.TryParse(parts[0], out int whole) || !int.TryParse(parts[1], out int thousandths)) return;

                Major = Math.Clamp(whole - BaseMHz, 0, MajorMax - 1);
                Minor = Math.Clamp(thousandths / 5, 0, 199);
            }
        }

        private readonly RadioState com1 = new() { Name = "COM1", BaseMHz = 118, MajorMax = 17, Active = "119.000" };
        private readonly RadioState com2 = new() { Name = "COM2", BaseMHz = 118, MajorMax = 17, Active = "120.000" };
        private readonly RadioState nav1 = new() { Name = "NAV1", BaseMHz = 108, MajorMax = 10, Active = "109.000" };
        private readonly RadioState nav2 = new() { Name = "NAV2", BaseMHz = 108, MajorMax = 10, Active = "110.000" };

        private Label lblCommLine1 = null!;
        private Label lblCommLine2 = null!;
        private Label lblNavLine1 = null!;
        private Label lblNavLine2 = null!;

        private readonly UdpClient reflectorClient = new();

        public frmMain()
        {
            InitializeComponent();

            var (commPanel, commLine1, commLine2) = BuildOledDisplay("COMM DISPLAY");
            lblCommLine1 = commLine1;
            lblCommLine2 = commLine2;
            pnlContent.Controls.Add(commPanel);
            pnlContent.Controls.Add(BuildRadioRow(com1, com2, hasSwap: true));

            var (navPanel, navLine1, navLine2) = BuildOledDisplay("NAV DISPLAY");
            lblNavLine1 = navLine1;
            lblNavLine2 = navLine2;
            pnlContent.Controls.Add(navPanel);
            pnlContent.Controls.Add(BuildRadioRow(nav1, nav2, hasSwap: false));

            pnlContent.Controls.Add(BuildElectricalGroup());

            RefreshDisplays();
        }

        // Builds a monospace black/green panel styled after the character
        // OLEDs updateCOMM()/updateNAV() write to (two lines, two radios each).
        private (GroupBox container, Label line1, Label line2) BuildOledDisplay(string title)
        {
            var container = new GroupBox
            {
                Text = title,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Margin = new Padding(8),
            };

            var screen = new Panel
            {
                BackColor = Color.Black,
                Size = new Size(320, 64),
                Location = new Point(10, 20),
            };

            var font = new Font("Consolas", 16f, FontStyle.Bold);
            var line1 = new Label
            {
                ForeColor = Color.Lime,
                BackColor = Color.Black,
                Font = font,
                AutoSize = false,
                Size = new Size(300, 28),
                Location = new Point(10, 4),
                TextAlign = ContentAlignment.MiddleLeft,
            };
            var line2 = new Label
            {
                ForeColor = Color.Lime,
                BackColor = Color.Black,
                Font = font,
                AutoSize = false,
                Size = new Size(300, 28),
                Location = new Point(10, 32),
                TextAlign = ContentAlignment.MiddleLeft,
            };

            screen.Controls.Add(line1);
            screen.Controls.Add(line2);
            container.Controls.Add(screen);
            container.Size = new Size(screen.Width + 20, screen.Height + 30);

            return (container, line1, line2);
        }

        private FlowLayoutPanel BuildRadioRow(RadioState left, RadioState right, bool hasSwap)
        {
            var row = new FlowLayoutPanel
            {
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                FlowDirection = FlowDirection.LeftToRight,
                WrapContents = false,
            };
            row.Controls.Add(BuildRadioGroup(left, hasSwap));
            row.Controls.Add(BuildRadioGroup(right, hasSwap));
            return row;
        }

        // One group box per radio: a Major (whole-unit) knob, a Minor (5 kHz)
        // knob, and — for COM1/COM2 only — the active/standby swap button that
        // mirrors the physical panel's XFER key.
        private GroupBox BuildRadioGroup(RadioState state, bool hasSwap)
        {
            var box = new GroupBox
            {
                Text = state.Name,
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Margin = new Padding(8),
            };

            var flow = new FlowLayoutPanel
            {
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                FlowDirection = FlowDirection.LeftToRight,
                Location = new Point(10, 20),
                WrapContents = false,
            };

            var majorKnob = new KnobControl { Caption = "MHz" };
            majorKnob.Turned += (_, dir) =>
            {
                state.Major = Math.Clamp(state.Major + dir, 0, state.MajorMax - 1);
                OnRadioChanged(state);
            };

            var minorKnob = new KnobControl { Caption = "kHz" };
            minorKnob.Turned += (_, dir) =>
            {
                state.Minor = ((state.Minor + dir) % 200 + 200) % 200;
                OnRadioChanged(state);
            };

            flow.Controls.Add(majorKnob);
            flow.Controls.Add(minorKnob);

            if (hasSwap)
            {
                var swapBtn = new Button
                {
                    Text = "⇄ XFER",
                    Width = 70,
                    Height = 40,
                    Margin = new Padding(6, 24, 3, 3),
                };
                swapBtn.Click += (_, _) => SwapRadio(state);
                flow.Controls.Add(swapBtn);
            }

            box.Controls.Add(flow);
            return box;
        }

        private GroupBox BuildElectricalGroup()
        {
            var box = new GroupBox
            {
                Text = "Electrical (panel key row 0/col 0 and row 1/col 0)",
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                Margin = new Padding(8),
            };

            var flow = new FlowLayoutPanel
            {
                AutoSize = true,
                AutoSizeMode = AutoSizeMode.GrowAndShrink,
                FlowDirection = FlowDirection.LeftToRight,
                Location = new Point(10, 20),
            };

            var onBtn = new Button { Text = "MASTER ON", Width = 120, Height = 32, Margin = new Padding(3) };
            onBtn.Click += (_, _) =>
            {
                SendCommand("MASTER_BATTERY_ON");
                SendCommand("ALTERNATOR_ON");
            };

            var offBtn = new Button { Text = "MASTER OFF", Width = 120, Height = 32, Margin = new Padding(3) };
            offBtn.Click += (_, _) =>
            {
                SendCommand("MASTER_BATTERY_OFF");
                SendCommand("ALTERNATOR_OFF");
            };

            flow.Controls.Add(onBtn);
            flow.Controls.Add(offBtn);
            box.Controls.Add(flow);
            return box;
        }

        private void OnRadioChanged(RadioState state)
        {
            RefreshDisplays();
            SendStandby(state);
        }

        // Mirrors handleKeyPress()'s COM1/COM2 swap keys: sends the swap
        // command the sim bridge listens for, and — since this emulator has no
        // live sim feed to report the resulting active frequency back — also
        // swaps the locally displayed active/standby pair so the panel looks
        // right immediately.
        private void SwapRadio(RadioState state)
        {
            string oldActive = state.Active;
            string oldStandby = state.Standby;

            state.Active = oldStandby;
            state.SetStandbyFrom(oldActive);

            RefreshDisplays();

            if (state == com1)
            {
                SendCommand("COM1_RADIO_SWAP");
            }
            else if (state == com2)
            {
                SendCommand("COM2_RADIO_SWAP");
                SendCommand("AVIONICS_MASTER_SET");
            }
        }

        private void RefreshDisplays()
        {
            lblCommLine1.Text = $"{com1.Active,-8} {com2.Active,-8}";
            lblCommLine2.Text = $"{com1.Standby,-8} {com2.Standby,-8}";
            lblNavLine1.Text = $"{nav1.Active,-8} {nav2.Active,-8}";
            lblNavLine2.Text = $"{nav1.Standby,-8} {nav2.Standby,-8}";
        }

        // Sends the radio's own standby frequency as bare text, e.g. "121.005".
        // The sketch's Nav encoder handlers actually resend com1StandbyFrequency
        // here (a copy/paste quirk), which the C# sim-bridge apps would silently
        // treat as a COM1 standby set anyway since they don't distinguish by
        // content; sending each radio's real value is more useful for testing
        // and does not change any currently-working behaviour.
        private void SendStandby(RadioState state) => SendUdp(state.Standby);

        // Mirrors handleKeyPress()'s COM1_RADIO_SWAP / COM2_RADIO_SWAP /
        // AVIONICS_MASTER_SET / MASTER_BATTERY_ON|OFF / ALTERNATOR_ON|OFF commands.
        private void SendCommand(string command) => SendUdp(command);

        private void SendUdp(string payload)
        {
            if (!IPAddress.TryParse(txtHost.Text.Trim(), out var reflectorIp))
            {
                lblStatus.Text = $"Invalid host: {txtHost.Text}";
                return;
            }
            if (!int.TryParse(txtPort.Text.Trim(), out int reflectorPort))
            {
                lblStatus.Text = $"Invalid port: {txtPort.Text}";
                return;
            }

            try
            {
                byte[] bytes = Encoding.ASCII.GetBytes(payload);
                reflectorClient.Send(bytes, bytes.Length, new IPEndPoint(reflectorIp, reflectorPort));
                lblStatus.Text = $"Sent: {payload}";
            }
            catch (SocketException ex)
            {
                lblStatus.Text = $"Send failed: {ex.Message}";
            }
        }
    }
}
