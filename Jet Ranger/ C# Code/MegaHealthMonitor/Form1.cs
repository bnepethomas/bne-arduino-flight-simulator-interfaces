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

                            if (receivedData.StartsWith("COMM_NAV"))
                            {
                                CommNavLastReceived = DateTime.Now;
                                // Update UI with received data (use Invoke to reach UI thread)
                                if (loggingActive)
                                {
                                    this.Invoke(new Action(() =>
                                    {
                                        listBoxLogs.Items.Add(CommNavLastReceived.Hour + ":" + CommNavLastReceived.Minute + ":" + CommNavLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");

                                    }));
                                }

                                this.Invoke(new Action(() =>
                                {
                                    lblCommNav.BackColor = Color.Green;
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
                                        listBoxLogs.Items.Add(ServoLastReceived.Hour + ":" + ServoLastReceived.Minute + ":" + ServoLastReceived.Second + $" Received: {receivedData} from {result.RemoteEndPoint}");
                                    }));
                                }
                                this.Invoke(new Action(() =>
                                {
                                    lblServo.BackColor = Color.Green;
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
        }

    }
