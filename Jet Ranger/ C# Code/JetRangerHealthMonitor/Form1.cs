using System;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace MegaHealthMonitor
{
    public partial class frmMain : Form
    {
        private UdpClient udpServer;
        private const int listenPort = 13137;

        string UDP_Playload;
        UdpClient udpClient = new UdpClient();

        DateTime CommNavLastReceived = DateTime.Now;
        DateTime ServoLastReceived = DateTime.Now;
        DateTime JoyStickLastReceived = DateTime.Now;
        DateTime UpperInputLastReceived = DateTime.Now;
        DateTime StepperLastReceived = DateTime.Now;
        DateTime DualStepperLastReceived = DateTime.Now;
        DateTime AdfLastReceived = DateTime.Now;

        // The last voltage reading from each board. Every board appends its 5 V rail to its heartbeat (" V5:5.02") and the ADF controller also its
        // 12 V rail (" V12:12.41"), for example "ADF_PANEL V5:5.02 V12:12.41".
        private sealed class BoardVoltage
        {
            public string Prefix = "";
            public string Name = "";
            public bool Measures12V;
            public double? V5;
            public double? V12;
            public DateTime? When;
        }

        private readonly BoardVoltage[] boardVoltages =
        {
            new BoardVoltage { Prefix = "COMM_NAV", Name = "Comm Nav (Radio)" },
            new BoardVoltage { Prefix = "SERVO", Name = "Servo" },
            new BoardVoltage { Prefix = "JOYSTICK", Name = "Joystick (HID)" },
            new BoardVoltage { Prefix = "UPPER_INPUT", Name = "Upper Input" },
            new BoardVoltage { Prefix = "STEPPER", Name = "Stepper" },
            new BoardVoltage { Prefix = "DUAL_STEPPER", Name = "Dual Stepper" },
            new BoardVoltage { Prefix = "ADF_PANEL", Name = "ADF", Measures12V = true },
        };

        // Rails outside these limits are shown in orange.
        private const double V5Low = 4.75, V5High = 5.25, V12Low = 10.8, V12High = 15.0;

        // Records the voltages in a heartbeat ("COMM_NAV V5:5.02") against its board.
        private void RecordVoltages(string heartbeat)
        {
            foreach (var board in boardVoltages)
            {
                if (!heartbeat.StartsWith(board.Prefix)) continue;
                var m5 = System.Text.RegularExpressions.Regex.Match(heartbeat, @"V5:([0-9]+(?:\.[0-9]+)?)");
                var m12 = System.Text.RegularExpressions.Regex.Match(heartbeat, @"V12:([0-9]+(?:\.[0-9]+)?)");
                if (m5.Success) board.V5 = double.Parse(m5.Groups[1].Value, System.Globalization.CultureInfo.InvariantCulture);
                if (m12.Success) board.V12 = double.Parse(m12.Groups[1].Value, System.Globalization.CultureInfo.InvariantCulture);
                if (m5.Success || m12.Success) board.When = DateTime.Now;
                break;
            }
        }

        // Fills the voltage table: one row per board with its last 5 V reading, the ADF's 12 V reading and how long ago it arrived.
        private void UpdateVoltageList()
        {
            if (!lvVoltages.Visible) return;
            lvVoltages.BeginUpdate();
            if (lvVoltages.Items.Count != boardVoltages.Length)
            {
                lvVoltages.Items.Clear();
                foreach (var b in boardVoltages)
                {
                    var item = new ListViewItem(b.Name) { UseItemStyleForSubItems = false };
                    item.SubItems.Add("");
                    item.SubItems.Add("");
                    item.SubItems.Add("");
                    lvVoltages.Items.Add(item);
                }
            }
            for (int i = 0; i < boardVoltages.Length; i++)
            {
                var b = boardVoltages[i];
                var item = lvVoltages.Items[i];
                bool stale = b.When == null || (DateTime.Now - b.When.Value).TotalSeconds > 35;

                item.SubItems[1].Text = b.V5.HasValue ? b.V5.Value.ToString("F2") + " V" : "no reading";
                item.SubItems[1].ForeColor = !b.V5.HasValue || stale ? Color.Gray : (b.V5.Value < V5Low || b.V5.Value > V5High ? Color.OrangeRed : Color.Black);

                if (b.Measures12V)
                {
                    item.SubItems[2].Text = b.V12.HasValue ? b.V12.Value.ToString("F2") + " V" : "no reading";
                    item.SubItems[2].ForeColor = !b.V12.HasValue || stale ? Color.Gray : (b.V12.Value < V12Low || b.V12.Value > V12High ? Color.OrangeRed : Color.Black);
                }
                else
                {
                    item.SubItems[2].Text = "not measured";
                    item.SubItems[2].ForeColor = Color.Gray;
                }

                if (b.When.HasValue)
                {
                    int seconds = (int)(DateTime.Now - b.When.Value).TotalSeconds;
                    item.SubItems[3].Text = b.When.Value.ToString("HH:mm:ss") + " (" + seconds + " s ago)";
                }
                else
                {
                    item.SubItems[3].Text = "none yet";
                }
                item.SubItems[3].ForeColor = stale ? Color.Gray : Color.Black;
            }
            lvVoltages.EndUpdate();
        }

        // Sizes the window for whichever of the log list and the voltage table are showing.
        private void ApplyLayout()
        {
            int top = 67;
            listBoxLogs.Top = top;
            if (listBoxLogs.Visible) top += listBoxLogs.Height + 8;
            lvVoltages.Top = top;
            if (lvVoltages.Visible) top += lvVoltages.Height + 8;
            ClientSize = new Size((listBoxLogs.Visible || lvVoltages.Visible) ? 465 : 165, top + 6);
        }

        bool loggingActive = true;

        private void StartListener()
        {
            Task.Run(async () =>
            {
                try
                {
                    // Create UdpClient on the specified port
                    using (udpServer = new UdpClient(listenPort))
                    {
                        while (true)
                        {
                            // Wait for incoming data
                            UdpReceiveResult result = await udpServer.ReceiveAsync();
                            string receivedData = Encoding.ASCII.GetString(result.Buffer);

                            //if (loggingActive)
                            //{
                            //    this.Invoke(new Action(() =>
                            //    {
                            //        listBoxLogs.Items.Add($" Received: {receivedData} from {result.RemoteEndPoint}");

                            //    }));
                            //}

                            this.Invoke(new Action(() =>
                            {
                                RecordVoltages(receivedData);
                                UpdateVoltageList();
                            }));

                            if (receivedData.StartsWith("COMM_NAV"))
                            {
                                CommNavLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(CommNavLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");

                                    }));
                                }

                                this.Invoke(new Action(() =>
                                {
                                    lblCommNav.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.StartsWith("JOYSTICK"))
                            {
                                JoyStickLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(JoyStickLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblJoyStick.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.StartsWith("SERVO"))
                            {
                                ServoLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)

                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(ServoLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblServo.BackColor = Color.Green;
                                }));

                            }
                            else if (receivedData.StartsWith("UPPER_INPUT"))
                            {
                                UpperInputLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(UpperInputLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblUpperInput.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.StartsWith("STEPPER"))
                            {
                                StepperLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(StepperLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblStepper.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.StartsWith("DUAL_STEPPER"))
                            {
                                DualStepperLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(DualStepperLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblDualStepper.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.StartsWith("ADF_PANEL"))
                            {
                                AdfLastReceived = DateTime.Now;
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(AdfLastReceived.ToString("HH:mm:ss") + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblAdf.BackColor = Color.Green;
                                }));
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    // Handle socket exceptions (e.g., if port is already in use)
                    Console.WriteLine(ex.Message);
                }
            });
        }


        private void CheckHealth()
        {
            TimeSpan span = DateTime.Now - CommNavLastReceived;
            int mS = (int)span.TotalMilliseconds;
            // displayText("Its been this many mS since sending last packet: " + mS.ToString());

            if (mS >= 30000)
            {
                if (lblCommNav.BackColor != Color.Red)
                {
                    AddLog("CommNav connection lost.");
                }
                lblCommNav.BackColor = Color.Red;

            }
            else if (mS >= 15000)
            {
                if (lblCommNav.BackColor != Color.Orange)
                {
                    AddLog("CommNav connection warning.");
                }
                lblCommNav.BackColor = Color.Orange;
            }

            span = DateTime.Now - ServoLastReceived;
            mS = (int)span.TotalMilliseconds;
            // displayText("Its been this many mS since sending last packet: " + mS.ToString());

            if (mS >= 30000)
            {
                if (lblServo.BackColor != Color.Red)
                {
                    AddLog("Servo connection lost.");
                }
                lblServo.BackColor = Color.Red;

            }
            else if (mS >= 15000)
            {
                if (lblServo.BackColor != Color.Orange)
                {
                    AddLog("Servo connection warning.");
                }
                lblServo.BackColor = Color.Orange;
            }

            span = DateTime.Now - JoyStickLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblJoyStick.BackColor != Color.Red)
                {
                    AddLog("JoyStick connection lost.");
                }
                lblJoyStick.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblJoyStick.BackColor != Color.Orange)
                {
                    AddLog("JoyStick connection warning.");
                }
                lblJoyStick.BackColor = Color.Orange;
            }

            span = DateTime.Now - UpperInputLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblUpperInput.BackColor != Color.Red)
                {
                    AddLog("Upper Input connection lost.");
                }
                lblUpperInput.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblUpperInput.BackColor != Color.Orange)
                {
                    AddLog("Upper Input connection warning.");
                }
                lblUpperInput.BackColor = Color.Orange;
            }

            span = DateTime.Now - StepperLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblStepper.BackColor != Color.Red)
                {
                    AddLog("Stepper connection lost.");
                }
                lblStepper.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblStepper.BackColor != Color.Orange)
                {
                    AddLog("Stepper connection warning.");
                }
                lblStepper.BackColor = Color.Orange;
            }

            span = DateTime.Now - DualStepperLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblDualStepper.BackColor != Color.Red)
                {
                    AddLog("Dual Stepper connection lost.");
                }
                lblDualStepper.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblDualStepper.BackColor != Color.Orange)
                {
                    AddLog("Dual Stepper connection warning.");
                }
                lblDualStepper.BackColor = Color.Orange;
            }

            span = DateTime.Now - AdfLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblAdf.BackColor != Color.Red)
                {
                    AddLog("ADF connection lost.");
                }
                lblAdf.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblAdf.BackColor != Color.Orange)
                {
                    AddLog("ADF connection warning.");
                }
                lblAdf.BackColor = Color.Orange;
            }

            UpdateVoltageList();   // refreshes the "seconds ago" column

        }

        private void AddLog(string logMessage)
        {
            if (loggingActive)
            {
                listBoxLogs.Items.Add(DateTime.Now.ToString("HH:mm:ss") + " " + logMessage);
            }
        }

        public frmMain()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            StartListener();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void tmrHealthCheck_Tick(object sender, EventArgs e)
        {
            CheckHealth();
        }

        private void cmdToggleLogs_Click(object sender, EventArgs e)
        {
            if (listBoxLogs.Visible == true)
            {
                listBoxLogs.Items.Clear();
                listBoxLogs.Visible = false;
                cmdToggleLogs.Text = "Show Logs";
            }
            else
            {
                listBoxLogs.Visible = true;
                cmdToggleLogs.Text = "Hide Logs";
            }
            ApplyLayout();
        }

        // Shows or hides the table of the last voltage reading from each board.
        private void cmdToggleVoltages_Click(object sender, EventArgs e)
        {
            lvVoltages.Visible = !lvVoltages.Visible;
            cmdToggleVoltages.Text = lvVoltages.Visible ? "Hide Voltages" : "Show Voltages";
            ApplyLayout();
            UpdateVoltageList();
        }
        }

    }
