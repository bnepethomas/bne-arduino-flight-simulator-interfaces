using System.Net;
using System.Net.Sockets;
using System.Text;

namespace JetRangerUpperControllerEmulator
{
    public partial class frmMain : Form
    {
        // Mirrors the row/column matrix indices in JET_RANGER_UPPER_CONTROLLER.ino.
        // Only indices that carry a // comment in that sketch's
        // CreateDcsBiosMessage() are wired to a real switch on the panel, so
        // only those are emulated here.
        private static readonly (int Index, string Label, string Group)[] Switches =
        {
            (0,  "HTR 0",              "Cabin Heat"),
            (1,  "HTR 1",              "Cabin Heat"),
            (2,  "HTR 2",              "Cabin Heat"),
            (3,  "HTR 3",              "Cabin Heat"),
            (4,  "HTR 4",              "Cabin Heat"),
            (5,  "Air Vent",           "Air"),
            (6,  "Air Heat",           "Air"),
            (11, "CB Fuel Fwd Boost",  "Circuit Breakers"),
            (12, "CB Fuel Aft Boost",  "Circuit Breakers"),
            (13, "CB Gen Field",       "Circuit Breakers"),
            (14, "CB Gen",             "Circuit Breakers"),
            (15, "ATT IND / DIR GYRO", "Instruments"),
            (16, "Battery",            "Electrical"),
            (17, "Gen",                "Electrical"),
            (22, "LDG Light",          "Lights"),
            (23, "Inst Light",         "Lights"),
            (24, "CB Caut Light",      "Lights"),
            (25, "Anti Col Light",     "Lights"),
            (26, "Pos Light",          "Lights"),
            (27, "Pitot Heat",         "Lights"),
            (28, "Avionics",           "Lights"),
            (39, "Strobe Light",       "Lights"),
        };

        private static readonly Color SwitchOnColor = Color.LightGreen;
        private readonly Color switchOffColor = SystemColors.ControlLight;

        private readonly Dictionary<int, bool> switchState = new();
        private readonly UdpClient hidClient = new();

        public frmMain()
        {
            InitializeComponent();
            BuildSwitchPanel();
        }

        // Builds one group box per logical group, each containing one toggle
        // button per switch in that group.
        private void BuildSwitchPanel()
        {
            foreach (var group in Switches.GroupBy(s => s.Group))
            {
                var flow = new FlowLayoutPanel
                {
                    AutoSize = true,
                    AutoSizeMode = AutoSizeMode.GrowAndShrink,
                    FlowDirection = FlowDirection.TopDown,
                    Location = new Point(10, 20),
                    WrapContents = false,
                };

                foreach (var sw in group)
                {
                    switchState[sw.Index] = false;

                    var btn = new Button
                    {
                        Text = sw.Label,
                        Tag = sw.Index,
                        Width = 170,
                        Height = 32,
                        Margin = new Padding(3),
                        UseVisualStyleBackColor = false,
                        BackColor = switchOffColor,
                    };
                    btn.Click += SwitchButton_Click;
                    flow.Controls.Add(btn);
                }

                var box = new GroupBox
                {
                    Text = group.Key,
                    AutoSize = true,
                    AutoSizeMode = AutoSizeMode.GrowAndShrink,
                    Margin = new Padding(8),
                };
                box.Controls.Add(flow);

                pnlSwitches.Controls.Add(box);
            }
        }

        // Each button behaves as a physical toggle switch: clicking it flips
        // its stored state and sends the new state to UDP_HID.
        private void SwitchButton_Click(object? sender, EventArgs e)
        {
            var btn = (Button)sender!;
            int idx = (int)btn.Tag!;

            bool newState = !switchState[idx];
            switchState[idx] = newState;
            btn.BackColor = newState ? SwitchOnColor : switchOffColor;

            SendButtonToHid(idx, newState ? 1 : 0);
        }

        // Sends "idx,state" to UDP_HID, matching SendButtonToHid() in
        // JET_RANGER_UPPER_CONTROLLER.ino.
        private void SendButtonToHid(int idx, int state)
        {
            if (!IPAddress.TryParse(txtHost.Text.Trim(), out var hidIp))
            {
                lblStatus.Text = $"Invalid host: {txtHost.Text}";
                return;
            }
            if (!int.TryParse(txtPort.Text.Trim(), out int hidPort))
            {
                lblStatus.Text = $"Invalid port: {txtPort.Text}";
                return;
            }

            string payload = $"{idx},{state}";
            try
            {
                byte[] bytes = Encoding.ASCII.GetBytes(payload);
                hidClient.Send(bytes, bytes.Length, new IPEndPoint(hidIp, hidPort));
                lblStatus.Text = $"Sent: {payload}";
            }
            catch (SocketException ex)
            {
                lblStatus.Text = $"Send failed: {ex.Message}";
            }
        }
    }
}
