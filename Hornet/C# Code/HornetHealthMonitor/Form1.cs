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

        DateTime LeftConsoleLastReceived = DateTime.Now;
        DateTime UIPLastReceived = DateTime.Now;
        DateTime LIPLastReceived = DateTime.Now;
        DateTime LowerInstLastReceived = DateTime.Now;
        DateTime RightConsoleLastReceived = DateTime.Now;
        DateTime GaugesLastReceived = DateTime.Now;
        DateTime UDPToKeyboardLastReceived = DateTime.Now;

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

                            if (receivedData.Contains("Hornet Left Console Combined"))
                            {
                                LeftConsoleLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(LeftConsoleLastReceived.Hour + ":" + LeftConsoleLastReceived.Minute + ":" + LeftConsoleLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");

                                    }));
                                }

                                this.Invoke(new Action(() =>
                                {
                                    lblLeftConsole.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.Contains("UIP Combined Controller"))
                            {
                                UIPLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)

                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(UIPLastReceived.Hour + ":" + UIPLastReceived.Minute + ":" + UIPLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblUIP.BackColor = Color.Green;
                                }));

                            }
                            else if (receivedData.Contains("Hornet LIP CONTROLLER"))
                            {
                                LIPLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(LIPLastReceived.Hour + ":" + LIPLastReceived.Minute + ":" + LIPLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblLIP.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.Contains("Hornet Standy Instruments"))
                            {
                                LowerInstLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(LowerInstLastReceived.Hour + ":" + LowerInstLastReceived.Minute + ":" + LowerInstLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblLowerInst.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.Contains("Hornet Right Combined"))
                            {
                                RightConsoleLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(RightConsoleLastReceived.Hour + ":" + RightConsoleLastReceived.Minute + ":" + RightConsoleLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblRightConsole.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.Contains("Hornet Gauges Instrument Controller"))
                            {
                                GaugesLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(GaugesLastReceived.Hour + ":" + GaugesLastReceived.Minute + ":" + GaugesLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblGauges.BackColor = Color.Green;
                                }));
                            }
                            else if (receivedData.Contains("Hornet UDP to Keyboard"))
                            {
                                UDPToKeyboardLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(UDPToKeyboardLastReceived.Hour + ":" + UDPToKeyboardLastReceived.Minute + ":" + UDPToKeyboardLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblUDPToKeyboard.BackColor = Color.Green;
                                }));
                            }
                            else
                            {
                                // If the received data doesn't match any known type, you can log it or ignore it
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add($" Received unknown data: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
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
            TimeSpan span = DateTime.Now - LeftConsoleLastReceived;
            int mS = (int)span.TotalMilliseconds;
            // displayText("Its been this many mS since sending last packet: " + mS.ToString());

            if (mS >= 30000)
            {
                if (lblLeftConsole.BackColor != Color.Red)
                {
                    AddLog("Left Console connection lost.");
                }
                lblLeftConsole.BackColor = Color.Red;

            }
            else if (mS >= 15000)
            {
                if (lblLeftConsole.BackColor != Color.Orange)
                {
                    AddLog("Left Console connection warning.");
                }
                lblLeftConsole.BackColor = Color.Orange;
            }

            span = DateTime.Now - UIPLastReceived;
            mS = (int)span.TotalMilliseconds;
            // displayText("Its been this many mS since sending last packet: " + mS.ToString());

            if (mS >= 30000)
            {
                if (lblUIP.BackColor != Color.Red)
                {
                    AddLog("UIP connection lost.");
                }
                lblUIP.BackColor = Color.Red;

            }
            else if (mS >= 15000)
            {
                if (lblUIP.BackColor != Color.Orange)
                {
                    AddLog("UIP connection warning.");
                }
                lblUIP.BackColor = Color.Orange;
            }

            span = DateTime.Now - LIPLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblLIP.BackColor != Color.Red)
                {
                    AddLog("LIP connection lost.");
                }
                lblLIP.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblLIP.BackColor != Color.Orange)
                {
                    AddLog("LIP connection warning.");
                }
                lblLIP.BackColor = Color.Orange;
            }

            span = DateTime.Now - LowerInstLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblLowerInst.BackColor != Color.Red)
                {
                    AddLog("Lower Inst connection lost.");
                }
                lblLowerInst.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblLowerInst.BackColor != Color.Orange)
                {
                    AddLog("Lower Inst connection warning.");
                }
                lblLowerInst.BackColor = Color.Orange;
            }

            span = DateTime.Now - RightConsoleLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblRightConsole.BackColor != Color.Red)
                {
                    AddLog("Right Console connection lost.");
                }
                lblRightConsole.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblRightConsole.BackColor != Color.Orange)
                {
                    AddLog("Right Console connection warning.");
                }
                lblRightConsole.BackColor = Color.Orange;
            }

            span = DateTime.Now - GaugesLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblGauges.BackColor != Color.Red)
                {
                    AddLog("Gauges connection lost.");
                }
                lblGauges.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblGauges.BackColor != Color.Orange)
                {
                    AddLog("Gauges connection warning.");
                }
                lblGauges.BackColor = Color.Orange;
            }

            span = DateTime.Now - UDPToKeyboardLastReceived;
            mS = (int)span.TotalMilliseconds;

            if (mS >= 30000)
            {
                if (lblUDPToKeyboard.BackColor != Color.Red)
                {
                    AddLog("UDP To Keyboard connection lost.");
                }
                lblUDPToKeyboard.BackColor = Color.Red;
            }
            else if (mS >= 15000)
            {
                if (lblUDPToKeyboard.BackColor != Color.Orange)
                {
                    AddLog("UDP To Keyboard connection warning.");
                }
                lblUDPToKeyboard.BackColor = Color.Orange;
            }


        }

        private void AddLog(string logMessage)
        {
            if (loggingActive)
            {
                listBoxLogs.Items.Add(DateTime.Now.Hour + ":" + DateTime.Now.Minute + ":" + DateTime.Now.Second + " " + logMessage);
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
                this.Height = this.Height - listBoxLogs.Size.Height - 20;
                this.Width = this.Width - 300;
            }

            else
            {
                listBoxLogs.Visible = true;
                cmdToggleLogs.Text = "Hide Logs";
                this.Height = this.Height + listBoxLogs.Size.Height + 20;
                this.Width = this.Width + 300;

            }
        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
    }

}
