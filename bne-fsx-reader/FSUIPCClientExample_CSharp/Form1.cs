using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.IO.Ports;
using System.Net;
using System.Threading;
using FSUIPC;

namespace FSUIPCClientExample_CSharp
{
    public partial class Form1 : Form
    {
        // Private Static Members
        private static readonly string AppTitle = "FSUIPCClientApplication_CSharp";

        // Create the Offsets we're interested in for this Application
        private Offset<int> airspeed = new Offset<int>(0x02BC);  // Basic integer read example
        private Offset<int> avionics = new Offset<int>(0x2E80);  // Basic integer read and write example
        private Offset<byte[]> fsLocalDateTime = new Offset<byte[]>(0x0238, 10); // Example of reading an arbitary set of bytes. 

        private Offset<string> aircraftType = new Offset<string>("AircraftInfo", 0x3160, 24); // Example of string and use of a group
        private Offset<BitArray> lights = new Offset<BitArray>(0x0D0C, 2);      // Example of BitArray used to manage a bit field type offset.
        private Offset<BitArray> AudioPanel = new Offset<BitArray>(0x3122, 1);  // Example of BitArray used to manage a bit field type offset.
        private Offset<Double> compass = new Offset<double>(0x02CC); // Example for disconnecting/reconnecting
        private Offset<short> pause = new Offset<short>(0x0262, true); // Example of a write only offset.


        private Offset<short> com1bcd = new Offset<short>(0x34E);               // Binary Coded Decimal
        private Offset<short> com1standbybcd = new Offset<short>(0x311A);       // Binary Coded Decimal
        private Offset<short> com2bcd = new Offset<short>(0x3118);              // Binary Coded Decimal
        private Offset<short> com2standbybcd = new Offset<short>(0x311C);       // Binary Coded Decimal


        private Offset<short> nav1bcd = new Offset<short>(0x350);               // Binary Coded Decimal
        private Offset<short> nav1standbybcd = new Offset<short>(0x311E);       // Binary Coded Decimal
        private Offset<short> nav2bcd = new Offset<short>(0x352);               // Binary Coded Decimal
        private Offset<short> nav2standbybcd = new Offset<short>(0x3120);       // Binary Coded Decimal


        private Offset<short> ADF1mainbcd = new Offset<short>(0x34C);           // Binary Coded Decimal
        private Offset<short> ADF1extbcd = new Offset<short>(0x356);            // Binary Coded Decimal


        private Offset<Double> AvionicsVoltage = new Offset<Double>(0x2850);    // 24V when power available

        private Offset<string> DME1Dist = new Offset<string>(0xC29, 5);         // DME1 Dist as string
        private Offset<string> DME1Speed = new Offset<string>(0xC2E, 5);         // DME1 Speed as string
        private Offset<string> DME2Dist = new Offset<string>(0xC33, 5);         // DME1 Dist as string
        private Offset<string> DME2Speed = new Offset<string>(0xC38, 5);         // DME1 Speed as string

        private Offset<short> Transponderbcd = new Offset<short>(0x354);         // Binary Coded Decimal

        private Offset<short> InnerMarker = new Offset<short>(0x0BAC);
        private Offset<short> MiddleMarker = new Offset<short>(0x0BAE);
        private Offset<short> OuterMarker = new Offset<short>(0x0BB0);

        private Offset<int> NoseGearPos = new Offset<int>(0x0BEC);
        private Offset<int> RightGearPos = new Offset<int>(0x0BF0);
        private Offset<int> LeftGearPos = new Offset<int>(0x0BF4);

        private Offset<BitArray> DodoLightBits = new Offset<BitArray>(0x2F28,8);

        private Offset<int> AP_MasterSwitch = new Offset<int>(0x07BC);
        private Offset<int> AP_HeadingLock = new Offset<int>(0x07C4);
        private Offset<int> AP_Nav1Lock = new Offset<int>(0x07C8);
        private Offset<int> AP_AltLock = new Offset<int>(0x07D0);
        private Offset<int> AP_IASLock = new Offset<int>(0x07DC);
        private Offset<int> AP_ApprLock = new Offset<int>(0x0800);
        private Offset<int> AP_BCLock = new Offset<int>(0x0804);
        private Offset<int> AP_FD = new Offset<int>(0x2EE0);

        private Offset<UInt16> AP_HeadingValue = new Offset<UInt16>(0x07CC);  // Heading Value as degrees*65636/360
        private Offset<UInt16> AP_CourseValue = new Offset<UInt16>(0x0C4E);       // Course Value as degrees*65636/360 from Nav1
        private Offset<int> AP_AltValue = new Offset<int>(0x07D4);          // Altitude value as metres*65536
        private Offset<short> AP_IASValue = new Offset<short>(0x07E2);          // Airspeed Value in knots
        private Offset<short> AP_VSValue = new Offset<short>(0x07F2);           // VS as ft/min

        private string str_AP_HeadingValue;
        private string str_AP_CourseValue;
        private string str_AP_AltValue;
        private string str_AP_IASValue;
        private string str_AP_VSValue;

        private Offset<int> Spoiler_Pos = new Offset<int>(0x0BD4);
        private Offset<int> Flaps_Pos = new Offset<int>(0x0BE0);

        private string str_Spoiler_Pos;
        private string str_Flaps_Pos;

        private Offset<long> playerLatitude = new Offset<long>(0x0560); // Offset for Lat/Lon features
        private Offset<long> playerLongitude = new Offset<long>(0x0568); // Offset for Lat/Lon features
        private Offset<short> onGround = new Offset<short>(0x0366); // Offset for Lat/Lon features
        private Offset<short> magVar = new Offset<short>(0x02A0); // Offset for Lat/Lon features
        private Offset<uint> playerHeadingTrue = new Offset<uint>(0x0580); // Offset for moving the plane
        private Offset<long> playerAltitude = new Offset<long>(0x0570); // Offset for moving the plane
        private Offset<short> slewMode = new Offset<short>(0x05DC, true); // Offset for moving the plane
        private Offset<int> sendControl = new Offset<int>(0x3110, true); // Offset for moving the plane

        private readonly int REFRESH_SCENERY = 65562; // Control number to refresh the scenery

        private FsLatLonPoint EGLL; // Holds the position of London Heathrow (EGLL)
        private FsLatLonQuadrilateral runwayQuad; // defines the four corners of the runway (27L at EGLL)
        private AITrafficServices AI; // Holds a reference to the AI Traffic Services object

        static TcpClient client;
        static NetworkStream stream;
        static Socket newsock;
        static EndPoint Remote;
        static bool bDebugFoundInCommandLine = false;
        static byte[] data;
        static IPEndPoint RemoteEndPoint;
        static IPEndPoint OLEDEndPoint;
        static Socket server;

        string sSOIC_StatusLights;
        
        public Form1()
        {
            InitializeComponent();
            // Setup the example data for London Heathrow
            // 1. The position
            //    This shows an FsLongitude and FsLatitude class made from the Degrees/Minutes/Seconds constructor.
            //    The Timer1_Tick() method shows a different contructor (using the RAW FSUIPC values).
            FsLatitude lat = new FsLatitude(51, 28, 39.0d); 
            FsLongitude lon = new FsLongitude(0, -27, -41.0d); 
            EGLL = new FsLatLonPoint(lat, lon);
            // Now define the Quadrangle for the 27L (09R) runway.
            // We could just define the four corner Lat/Lon points if we knew them.
            // In this example however we're using the helper function to calculate the points
            // from the runway information.  This is the kind of info you can find in the output files
            // from Pete Dowson's MakeRunways program.
            FsLatitude rwyThresholdLat = new FsLatitude(51.464943d);
            FsLongitude rwyThresholdLon = new FsLongitude(-0.434046d);
            double rwyMagHeading = 272.7d;
            double rwyMagVariation = -3d;
            double rwyLength = 11978d;
            double rwyWidth = 164d;
            // Call the static helper on the FsLatLonQuarangle class to generate the Quadrangle for this runway...
            FsLatLonPoint thresholdCentre = new FsLatLonPoint(rwyThresholdLat, rwyThresholdLon);
            double trueHeading = rwyMagHeading + rwyMagVariation;
            runwayQuad = FsLatLonQuadrilateral.ForRunway(thresholdCentre, trueHeading, rwyWidth, rwyLength);
            // Set the default value for the distance units and AI Radar range
            this.cbxDistanceUnits.Text = "Nautical Miles";
            this.cbxRadarRange.Text = "50";


            data = new Byte[256];
            RemoteEndPoint = new IPEndPoint(IPAddress.Loopback, 7777);
            OLEDEndPoint = new IPEndPoint(IPAddress.Loopback, 7781);
            server = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

            for (int i = 100; i < 141; i++)
            {
                string welcome = i.ToString() + "=1:";
                welcome = welcome + (i - 1).ToString() + "=0:";
                Console.WriteLine(welcome);
                data = Encoding.ASCII.GetBytes(welcome);
                server.SendTo(data, data.Length, SocketFlags.None, RemoteEndPoint);
                Thread.Sleep(20);
            }
        }

        // Application started so try to open the connection to FSUIPC
        private void Form1_Load(object sender, EventArgs e)
        {

            openFSUIPC();
        }

        // User pressed connect button so try again...
        private void btnStart_Click(object sender, EventArgs e)
        {
            openFSUIPC();
         }

        // Opens FSUIPC - if all goes well then starts the 
        // timer to drive start the main application cycle.
        // If can't open display the error message.
        private void openFSUIPC()
        {
            try
            {
                // Attempt to open a connection to FSUIPC (running on any version of Flight Sim)
                FSUIPCConnection.Open();
                // Opened OK so disable the Connect button
                this.btnStart.Enabled = false;
                this.chkEnableAIRadar.Enabled = true;
                // Start the timer ticking to drive the rest of the application
                this.timer1.Interval = 200;
                this.timer1.Enabled = true;
                // Set the AI object
                AI = FSUIPCConnection.AITrafficServices;
            }
            catch (Exception ex)
            {
                // Badness occurred - show the error message
                MessageBox.Show(ex.Message, AppTitle, MessageBoxButtons.OK, MessageBoxIcon.Error);
                FSUIPCConnection.Close();
                Debug.WriteLine(ex.Message);
            }
        }

        // Application is unloading so call close to cleanup the 
        // UNMANAGED memory used by FSUIPC. 
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            FSUIPCConnection.Close();
        }

        // The timer handles the real-time updating of the Form.
        // The default group (ie, no group specified) is 
        // Processed and every Offset in the default group is updated.
        private void timer1_Tick(object sender, EventArgs e)
        {
            // Process the default group
            try
            {
                FSUIPCConnection.Process();


                // IAS - Simple integer returned so just divide as per the 
                // FSUIPC documentation for this offset and display the result.
                double airpeedKnots = ((double)airspeed.Value / 128d);
                this.txtIAS.Text = airpeedKnots.ToString("f1");


                // Avionics Master Switch
                this.chkAvionics.Checked = (avionics.Value > 0);  // 0 = Off, 1 = On.


                // Advanced Concept: Reading Raw Blocks of Data.
                // FS Local Date and Time
                // This demonstrates getting back an arbitrary number of bytes from an offset.
                // Here we're getting 10 back from Offset 0x0328 which contain info about the 
                // Local date and time in FS.
                // Because it's returned as a byte array we need to handle everything ourselves...
                // 1. Year (starts at Byte 8) for 2 bytes. (Int16)
                //    Use the BitConverter class to get it into a native Int16 variable
                short year = BitConverter.ToInt16(fsLocalDateTime.Value, 8);
                //    You could also do it manually if you know about such things...
                //    short year = (short)(fsLocalDateTime.Value[8] + (fsLocalDateTime.Value[9] * 0x100));
                // 2. Make new datetime with the the time value at 01/01 of the year...
                //    Time - in bytes 0,1 and 2. (Hour, Minute, Second):
                DateTime fsTime = new DateTime(year, 1, 1, fsLocalDateTime.Value[0], fsLocalDateTime.Value[1], fsLocalDateTime.Value[2]);
                // 3. Get the Day of the Year back (not given as Day and Month) 
                //    and add this on to the Jan 1 date we created above 
                //    to give the final date:
                short dayNo = BitConverter.ToInt16(fsLocalDateTime.Value, 6);
                fsTime = fsTime.Add(new TimeSpan(dayNo - 1, 0, 0, 0));
                // Now print it out
                this.txtFSDateTime.Text = fsTime.ToString("dddd, MMMM dd yyyy hh:mm:ss");


                // Lights
                // This demonstrates using the BitArray type to handle
                // a bit field type offset.  The lights are a 2 byte (16bit) bit field 
                // starting in offset 0D0C.
                // To make the code clearer and easier to write in the first
                // place - I created a LightType Enum (bottom of this file).
                // You could of course just use the literal values 0-9 if you prefer.
                // For the first three, I've put alternative lines in comments
                // that use a literal indexer instead of the enum.
                // Update each checkbox according to the relevent bit in the BitArray...
                this.chkBeacon.Checked = lights.Value[(int)LightType.Beacon];
                //this.chkBeacon.Checked = lights.Value[1];
                this.chkCabin.Checked = lights.Value[(int)LightType.Cabin];
                //this.chkCabin.Checked = lights.Value[9];
                this.chkInstuments.Checked = lights.Value[(int)LightType.Instruments];
                //this.chkInstuments.Checked = lights.Value[5];
                this.chkLanding.Checked = lights.Value[(int)LightType.Landing];
                this.chkLogo.Checked = lights.Value[(int)LightType.Logo];
                this.chkNavigation.Checked = lights.Value[(int)LightType.Navigation];
                this.chkRecognition.Checked = lights.Value[(int)LightType.Recognition];
                this.chkStrobes.Checked = lights.Value[(int)LightType.Strobes];
                this.chkTaxi.Checked = lights.Value[(int)LightType.Taxi];
                this.chkWing.Checked = lights.Value[(int)LightType.Wing];

                // Compass heading
                // Used to demonstrate disconnecting and reconnecting an Offset.
                // We display the data in the field regardless of whether 
                // it's been updated or not.
                this.txtCompass.Text = compass.Value.ToString("F2");


                // COM1 frequency
                string com1String = BCDToString(com1bcd.Value);
                this.lblDebug.Text = "COM1 " + com1String + ". ";
                // COM1 Standby frequency
                string com1standbyString = BCDToString(com1standbybcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "COM1 Standby " + com1standbyString + ". ";
                // COM2 frequency
                string com2String = BCDToString(com2bcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "COM2 " + com2String + ". ";
                this.txtCOM2.Text = com2String;
                // COM2 Standby frequency
                string com2standbyString = BCDToString(com2standbybcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "COM2 Standby " + com2standbyString + ". ";


                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                // nav1 frequency
                string nav1String = BCDToString(nav1bcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "NAV1 " + nav1String + ". ";
                // nav1 Standby frequency
                string nav1standbyString = BCDToString(nav1standbybcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "NAV1 Standby " + nav1standbyString + ". ";
                // nav2 frequency
                string nav2String = BCDToString(nav2bcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "NAV2 " + nav2String + ". ";
                // nav2 Standby frequency
                string nav2standbyString = BCDToString(nav2standbybcd.Value);
                this.lblDebug.Text = this.lblDebug.Text + "NAV2 Standby " + nav2standbyString + ". ";
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                
                // ADF
                // ADF has different frequency to COM and NAV so convert locally
                // if EXT only returns a single digit then its the decimal
                // otherwise is the highest digit
                string ADF1mainString  = ADF1mainbcd.Value.ToString("X");
                this.lblDebug.Text = this.lblDebug.Text + "ADF2 Main " + ADF1mainString + ". ";
                // nav1 Standby frequency
                string ADF1extString = ADF1extbcd.Value.ToString("X");

                string ADF1decimalString;
                string ADF1msbString;
                if (ADF1extString.Length < 2)
                {
                    ADF1decimalString = ADF1extString;
                    ADF1msbString = "0";
                    this.lblDebug.Text = this.lblDebug.Text + "ADF1 Ext (short) " + ADF1msbString + "." + ADF1decimalString + ". ";
                }
                else
                {
                    ADF1decimalString = ADF1extString.Substring(1,1);
                    ADF1msbString = ADF1extString.Substring(0,1);
                    this.lblDebug.Text = this.lblDebug.Text + "ADF1 Ext (long) " + ADF1msbString + "." + ADF1decimalString + ". ";
                }
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";
                string ADF1totalString = ADF1msbString + ADF1mainString + "." + ADF1decimalString;
                this.lblDebug.Text = this.lblDebug.Text + ADF1totalString;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                // Avionics Voltage
                string sAvionicsVoltage = AvionicsVoltage.Value.ToString();
                this.lblDebug.Text = this.lblDebug.Text + "Avionics Voltage " + sAvionicsVoltage;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";



                string sDME1Dist = DME1Dist.Value.ToString();
                this.lblDebug.Text = this.lblDebug.Text + "DME1 Dist " +  sDME1Dist;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                string sDME1Speed = DME1Speed.Value.ToString();
                this.lblDebug.Text = this.lblDebug.Text + "DME1 Speed " + sDME1Speed;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";


                //; DME 
                // If Values = 0.00 then display ----
                string sDME2Dist = DME2Dist.Value.ToString();
                this.lblDebug.Text = this.lblDebug.Text + "DME2 Dist " + sDME2Dist;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                string sDME2Speed = DME2Speed.Value.ToString();
                this.lblDebug.Text = this.lblDebug.Text + "DME2 Speed " + sDME2Speed;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                // Transponder
                string transponderString = Transponderbcd.Value.ToString("X"); ;
                this.lblDebug.Text = this.lblDebug.Text + "Transponder " + transponderString;
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                // Audio Panel
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - Com1tx " + AudioPanel.Value[(int)AudioLightType.Com1Transmitt].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - Com2tx " + AudioPanel.Value[(int)AudioLightType.Com2Transmitt].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - ComBothRx " + AudioPanel.Value[(int)AudioLightType.COMRxBoth].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - Nav1 " + AudioPanel.Value[(int)AudioLightType.Nav1Sound].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - Nav2 " + AudioPanel.Value[(int)AudioLightType.Nav2Sound].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - Marker " + AudioPanel.Value[(int)AudioLightType.MarkerSound].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - DME " + AudioPanel.Value[(int)AudioLightType.DMESound].ToString() + "\n\r"; 
                this.lblDebug.Text = this.lblDebug.Text + "Audio Panel - ADF1 " + AudioPanel.Value[(int)AudioLightType.ADF1Sound].ToString() + "\n\r";

                
                sSOIC_StatusLights = "";
                string sTempVar = "";

                if (AvionicsVoltage.Value > 24)
                {
                    if (AudioPanel.Value[(int)AudioLightType.Com1Transmitt]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = "100=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.Com2Transmitt]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "101=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.COMRxBoth]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "102=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.Nav1Sound]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "103=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.Nav2Sound]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "104=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.MarkerSound]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "105=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.DMESound]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "106=" + sTempVar + ":";

                    if (AudioPanel.Value[(int)AudioLightType.ADF1Sound]) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "107=" + sTempVar + ":";


                    // Markers 
                    this.lblDebug.Text = this.lblDebug.Text + "Inner Marker " + InnerMarker.Value.ToString() + "\n\r";
                    this.lblDebug.Text = this.lblDebug.Text + "Middle Marker " + MiddleMarker.Value.ToString() + "\n\r";
                    this.lblDebug.Text = this.lblDebug.Text + "Outer Marker " + OuterMarker.Value.ToString() + "\n\r";

                    if (InnerMarker.Value == 1) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "110=" + sTempVar + ":";

                    if (MiddleMarker.Value == 1) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "111=" + sTempVar + ":";

                    if (OuterMarker.Value == 1) sTempVar = "1"; else sTempVar = "0";
                    sSOIC_StatusLights = sSOIC_StatusLights + "112=" + sTempVar + ":";

                }
                else    // No Navionics zero all values
                {
                    sSOIC_StatusLights = sSOIC_StatusLights + "100=0:101=0:102=0:103=0:104=0:105=0:106=0:107=0:"
                        + "110=0:111=0:112=0:";
                }

                // Gear
                this.lblDebug.Text = this.lblDebug.Text + "Nose Gear " +  NoseGearPos.Value.ToString() + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Nose Gear " +  RightGearPos.Value.ToString() + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Nose Gear " +  LeftGearPos.Value.ToString() + "\n\r";

                
                // Dodo Lightbits
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Engine Out " + DodoLightBits.Value[(int)DodoLightType.EngOut].ToString() + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Rotor Low " + DodoLightBits.Value[(int)DodoLightType.RotorLow].ToString()   + "  ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Fuel Low " + DodoLightBits.Value[(int)DodoLightType.FuelLow].ToString()     + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Gen Fail " + DodoLightBits.Value[(int)DodoLightType.GeneratorFail].ToString() + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Batt Warm " + DodoLightBits.Value[(int)DodoLightType.BatteryWarm].ToString()     + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Batt Hot " + DodoLightBits.Value[(int)DodoLightType.BatteryHot].ToString()      + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Fuel Pump " + DodoLightBits.Value[(int)DodoLightType.FuelPumpFail].ToString()     + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Trans Press " + DodoLightBits.Value[(int)DodoLightType.TransmissionPressureFail].ToString()   + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Trans Temp " + DodoLightBits.Value[(int)DodoLightType.TransmissionTemperatureFail].ToString()    + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Engine Chip " + DodoLightBits.Value[(int)DodoLightType.EngineChip].ToString()   + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Tail Rotor Chip " + DodoLightBits.Value[(int)DodoLightType.TailRotorChip].ToString() + "   ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Trans Chip " + DodoLightBits.Value[(int)DodoLightType.TransmissionChip].ToString()    + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Fuel Filter " + DodoLightBits.Value[(int)DodoLightType.FuelFilterFail].ToString()   + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Baggage Door " + DodoLightBits.Value[(int)DodoLightType.BaggageDoor].ToString()  + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo SimmConnect " + DodoLightBits.Value[(int)DodoLightType.SimConnectFail].ToString()   + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Spare 1 " + DodoLightBits.Value[(int)DodoLightType.Spare1].ToString()       + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Spare 2 " + DodoLightBits.Value[(int)DodoLightType.Spare2].ToString()       + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Spare 3" + DodoLightBits.Value[(int)DodoLightType.Spare3].ToString()        + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Spare 4 " + DodoLightBits.Value[(int)DodoLightType.Spare4].ToString()       + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Spare 5 " + DodoLightBits.Value[(int)DodoLightType.Spare5].ToString()       + "    ";
                this.lblDebug.Text = this.lblDebug.Text + "Dodo Turbine Temp " + DodoLightBits.Value[(int)DodoLightType.TurbineOverTemp].ToString()  + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "AP Master " + AP_MasterSwitch.Value.ToString() + "     " ;
                this.lblDebug.Text = this.lblDebug.Text + "AP Nav1 Lock " + AP_Nav1Lock.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP Heading Lock " + AP_HeadingLock.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP Alt Lock " + AP_AltLock.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP IAS Lock " + AP_IASLock.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP Appr Lock " + AP_ApprLock.Value.ToString() + "\n\r";
                this.lblDebug.Text = this.lblDebug.Text + "AP BC Lock " + AP_BCLock.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP FD Lock " + AP_FD.Value.ToString() + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                str_AP_HeadingValue = (AP_HeadingValue.Value / 182).ToString("000");         // Need to pad to three digits
                str_AP_CourseValue = AP_CourseValue.Value.ToString("000");
                str_AP_AltValue = (AP_AltValue.Value / (199753.728)).ToString("0000") + "0";         // Altitude value as metres*65536
                str_AP_IASValue = AP_IASValue.Value.ToString();
                str_AP_VSValue = AP_VSValue.Value.ToString();


                this.lblDebug.Text = this.lblDebug.Text + "AP Heading Value " + str_AP_HeadingValue + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP Course Value " + str_AP_CourseValue + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP Alt Value " + str_AP_AltValue + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP IAS Value " + str_AP_IASValue + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "AP VS Value " + str_AP_VSValue + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "\n\r";

                str_Spoiler_Pos = Spoiler_Pos.Value.ToString();
                str_Flaps_Pos =  Flaps_Pos.Value.ToString();

                this.lblDebug.Text = this.lblDebug.Text + "Spoilers " + str_Spoiler_Pos + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "Flaps " + str_Flaps_Pos + "     ";


                if (DodoLightBits.Value[(int)DodoLightType.EngOut]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "120=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.RotorLow]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "121=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.FuelLow]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "122=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.GeneratorFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "123=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.BatteryWarm]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "124=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.BatteryHot]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "125=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.FuelPumpFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "126=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.TransmissionPressureFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "127=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.TransmissionTemperatureFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "128=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.EngineChip]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "129=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.TailRotorChip]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "130=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.TransmissionChip]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "131=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.FuelFilterFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "132=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.BaggageDoor]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "133=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.SimConnectFail]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "134=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.Spare1]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "135=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.Spare2]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "136=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.Spare3]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "137=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.Spare4]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "138=" + sTempVar + ":";

                if (DodoLightBits.Value[(int)DodoLightType.Spare5]) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "139=" + sTempVar + ":";

                // Auto Pilot

                if (AP_MasterSwitch.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "300=" + sTempVar + ":";

                if (AP_Nav1Lock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "301=" + sTempVar + ":";

                if (AP_HeadingLock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "302=" + sTempVar + ":";

                if (AP_AltLock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "303=" + sTempVar + ":";

                if (AP_IASLock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "304=" + sTempVar + ":";

                if (AP_ApprLock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "305=" + sTempVar + ":";

                if (AP_BCLock.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "306=" + sTempVar + ":";

                if (AP_FD.Value != 0) sTempVar = "1"; else sTempVar = "0";
                sSOIC_StatusLights = sSOIC_StatusLights + "307=" + sTempVar + ":";



                

                // Send the UDP packet towards the SOIC server via the UDP to TCP Bridge
                data = Encoding.ASCII.GetBytes(sSOIC_StatusLights);
                server.SendTo(data, data.Length, SocketFlags.None, RemoteEndPoint);

                this.lblDebug.Text = this.lblDebug.Text + "Spoilers " + str_Spoiler_Pos + "     ";
                this.lblDebug.Text = this.lblDebug.Text + "Flaps " + str_Flaps_Pos + "     ";


                // Send the UDP packet towards oled Display
                // Note use of ; instead of : so we can display hh:mm:ss
                // on cessna 172 nav volts at 24 then 28 when alt is running
                if (AvionicsVoltage.Value > 23)
                {
                    sSOIC_StatusLights = "200=" + com1String + "    " + com2String + ";201=" + com1standbyString + "    " + com2standbyString +
                        ";202=" + nav1String + "    " + nav2String +
                        ";203=" + nav1standbyString + "    " + nav2standbyString + ";204=" + sDME1Dist + " NM   " + sDME1Speed
                        + " KT;205=" + sDME2Dist + " NM   " + sDME2Speed + " KT"
                        + ";206=" + str_AP_HeadingValue + " " + str_AP_VSValue.PadLeft(12) 
                        + ";207=" + str_AP_CourseValue + "   " + str_AP_IASValue.PadLeft(3) + "  " + str_AP_AltValue
                        + ";208=F " + str_Flaps_Pos.PadRight(5)  + "  " +  str_Spoiler_Pos.PadLeft(5) + " S"
                        + ";209=" + fsTime.ToString("HH:mm:ss") + "  " + ADF1totalString;
                    lblDebug.Text = lblDebug.Text + sSOIC_StatusLights;
                }
                else
                {
                    string blanks16 = "                ";
                    sSOIC_StatusLights = "200=" + blanks16 + ";201=" + blanks16 +
                        ";202=" + blanks16 +
                        ";203=" + blanks16 +
                        ";204=" + blanks16 +
                        ";205=" + blanks16 +
                        ";206=" + blanks16 +
                        ";207=" + blanks16 +
                        ";208=" + blanks16 +
                        ";209=" + blanks16;

                }
                data = Encoding.ASCII.GetBytes(sSOIC_StatusLights);
                server.SendTo(data, data.Length, SocketFlags.None, OLEDEndPoint);

                // Latitude and Longitude 
                // Shows using the FsLongitude and FsLatitude classes to easily work with Lat/Lon
                // Create new instances of FsLongitude and FsLatitude using the raw 8-Byte data from the FSUIPC Offsets
                FsLongitude lon = new FsLongitude(playerLongitude.Value);
                FsLatitude lat = new FsLatitude(playerLatitude.Value);
                // Use the ToString() method to output in human readable form:
                // (note that many other properties are avilable to get the Lat/Lon in different numerical formats)
                this.txtLatitude.Text = lat.ToString();
                this.txtLongitude.Text = lon.ToString();

                // Using fsLonLatPoint to calculate distance and bearing between two points
                // First get the point for the current plane position
                FsLatLonPoint currentPosition = new FsLatLonPoint(lat, lon);
                // Get the distance between here and EGLL
                double distance = 0;
                switch (this.cbxDistanceUnits.Text)
                {
                    case "Nautical Miles":
                        distance = currentPosition.DistanceFromInNauticalMiles(EGLL);
                        break;
                    case "Statute Miles":
                        distance = currentPosition.DistanceFromInFeet(EGLL) / 5280d;
                        break;
                    case "Kilometres":
                        distance = currentPosition.DistanceFromInMetres(EGLL) / 1000d;
                        break;
                }
                // Write the distance to the text box formatting to 2 decimal places
                this.txtDistance.Text = distance.ToString("N2");
                // Get the bearing (True) 
                double bearing = currentPosition.BearingTo(EGLL);
                // Get the magnetic variation
                double variation = (double)magVar.Value * 360d / 65536d;
                // convert bearing to magnetic bearing by subtracting the magnetic variation
                bearing -= variation;
                // Display the bearing in whole numbers and tag on a degree symbol
                this.txtBearing.Text = bearing.ToString("F0") + "\u00B0";

                // Now check if the player is on the runway:
                // Test is the plane is on the ground and if the current position is in the bounds of
                // the runway Quadrangle we calculated in the constructor above.
                chkPlaneOnRunway.Checked = (this.onGround.Value == 1 && runwayQuad.ContainsPoint(currentPosition));
            }
            catch (FSUIPCException ex)
            {
                if (ex.FSUIPCErrorCode == FSUIPCError.FSUIPC_ERR_SENDMSG)
                {
                    // Send message error - connection to FSUIPC lost.
                    // Show message, disable the main timer loop and relight the 
                    // connection button:
                    // Also Close the broken connection.
                    this.timer1.Enabled = false;
                    this.btnStart.Enabled = true;
                    this.chkEnableAIRadar.Enabled = false;
                    this.chkEnableAIRadar.Checked = false;
                    this.AIRadarTimer.Enabled = false;
                    FSUIPCConnection.Close();
                    MessageBox.Show("The connection to Flight Sim has been lost.", AppTitle, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                }
                else
                {
                    // not the disonnect error so some other baddness occured.
                    // just rethrow to halt the application
                    throw ex;
                }
            }
            catch (Exception)
            {
                // Sometime when the connection is lost, bad data gets returned 
                // and causes problems with some of the other lines.  
                // This catch block just makes sure the user doesn't see any
                // other Exceptions apart from FSUIPCExceptions.
            }
        }

        private void CheckPlayerIsOnRunway()
        {
            FsLongitude lon = new FsLongitude(playerLongitude.Value);
            FsLatitude lat = new FsLatitude(playerLatitude.Value);
            // Get the point for the current plane position
            FsLatLonPoint currentPosition = new FsLatLonPoint(lat, lon);

            // Now define the Quadrangle for the 27L (09R) runway.
            // We could just define the four corner Lat/Lon points if we knew them.
            // In this example however we're using the helper function to calculate the points
            // from the runway information.  This is the kind of info you can find in the output files
            // from Pete Dowson's MakeRunways program.
            FsLatitude rwyThresholdLat = new FsLatitude(51.464943d);
            FsLongitude rwyThresholdLon = new FsLongitude(-0.434046d);
            double rwyMagHeading = 272.7d;
            double rwyMagVariation = -3d;
            double rwyLength = 11978d;
            double rwyWidth = 164d;
            
            // Call the static helper on the FsLatLonQuarangle class to generate the Quadrangle for this runway...
            FsLatLonPoint thresholdCentre = new FsLatLonPoint(rwyThresholdLat, rwyThresholdLon);
            double trueHeading = rwyMagHeading + rwyMagVariation;
            runwayQuad = FsLatLonQuadrilateral.ForRunway(thresholdCentre, trueHeading, rwyWidth, rwyLength);
            
            // Now check if the player is on the runway:
            // Test is the plane is on the ground and if the current position is in the bounds of
            // the runway Quadrangle we calculated in the constructor above.
            if (this.onGround.Value == 1 && runwayQuad.ContainsPoint(currentPosition))
            {
                // Player is on the runway
                // Do Stuff
            }
        }

        // Demonstrates the Grouping facility and also returning a string.
        // The AircraftType Offset is in a Group called "AircraftInfo".
        // With the Group system you can gain control over which 
        // Offsets are processed.
        private void btnGetAircraftType_Click(object sender, EventArgs e)
        {
            // Aircraft type is in the "AircraftInfo" data group so we only want to proccess that here.
            try
            {
                FSUIPCConnection.Process("AircraftInfo");
                // OK so display the string
                // With strings the DLL automatically handles the 
                // ASCII/Unicode conversion and deals with the 
                // zero terminators.
                this.txtAircraftType.Text = aircraftType.Value;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, AppTitle, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }


        // Next few eventhandlers deal with writing the lights.
        // Again we use the BitArray to manage the individual bits.
        // I've also used the LightType enum again for the bit numbers, although
        // that's not to everyone's taste.  Some alternative lines using literal index 
        // numbers are included as comments for the first two.
        private void chkNavigation_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Navigation] = this.chkNavigation.Checked;
            //this.lights.Value[0] = this.chkNavigation.Checked;
        }

        private void chkBeacon_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Beacon] = this.chkBeacon.Checked;
            //this.lights.Value[1] = this.chkBeacon.Checked;
        }

        private void chkLanding_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Landing] = this.chkLanding.Checked;
        }

        private void chkTaxi_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Taxi] = this.chkTaxi.Checked;
        }

        private void chkStrobes_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Strobes] = this.chkStrobes.Checked;
        }

        private void chkInstuments_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Instruments] = this.chkInstuments.Checked;
        }

        private void chkRecognition_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Recognition] = this.chkRecognition.Checked;
        }

        private void chkWing_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Wing] = this.chkWing.Checked;
        }

        private void chkLogo_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Logo] = this.chkLogo.Checked;
        }

        private void chkCabin_CheckedChanged(object sender, EventArgs e)
        {
            this.lights.Value[(int)LightType.Cabin] = this.chkCabin.Checked;
        }

        // Demonstrates a simple 'write' to an Offset.
        // To send a value to FSUIPC, just change the Value property.
        // The new data will be written during the next Process().
        private void chkAvionics_CheckedChanged(object sender, EventArgs e)
        {
            this.avionics.Value = chkAvionics.Checked ? 1 : 0;
        }

        // This demonstrates disconnecting an individual Offset.
        // After it's disconnected it doesn't get updated from FSUIPC
        // and changed to the value of this Offset do not get written
        // when Process() is called.
        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            // Disconnect immediatley.
            this.compass.Disconnect();
        }

        // Same as disconnect, except the disconnect happens after 
        // the next Process.  So one more read/write is done and then
        // the Offset is disconnected.
        private void btnDisconnectAfterNext_Click(object sender, EventArgs e)
        {
            // Diconnect after the next process.
            this.compass.Disconnect(true);
        }

        // This demonstrates reconnecting an Offset.  It's value 
        // will be read/written during the subsequent Process()
        // calls.
        private void btnReconnect_Click(object sender, EventArgs e)
        {
            // Reconnect
            this.compass.Reconnect();
        }

        // Same as reconnect except the the Offset will be disconnected
        // again after the next Process() call.
        private void btnReconnectOnce_Click(object sender, EventArgs e)
        {
            // Reconnect, but only for one Process().  
            // The Offset is then disconnected again.
            this.compass.Reconnect(true);
        }

        // The pause Offset is Write only.  It's value is never updated from 
        // FSUIPC.  When it's value changes the new value is written
        // to FSUIPC during the next Process().
        private void chkPause_CheckedChanged(object sender, EventArgs e)
        {
            pause.Value = (short)(this.chkPause.Checked ? 1 : 0);
        }

        private enum LightType
        {
            Navigation,
            Beacon,
            Landing,
            Taxi,
            Strobes,
            Instruments,
            Recognition,
            Wing,
            Logo,
            Cabin
        }

        private enum AudioLightType
        {
            ADF1Sound,
            DMESound,
            MarkerSound,
            Nav2Sound,
            Nav1Sound,
            COMRxBoth,
            Com2Transmitt,
            Com1Transmitt    
        }

        private enum DodoLightType
        {
            EngOut,
            RotorLow,
            FuelLow,
            GeneratorFail,
            BatteryWarm,
            BatteryHot,
            FuelPumpFail,
            TransmissionPressureFail,
            TransmissionTemperatureFail,
            EngineChip,
            TailRotorChip,
            TransmissionChip,
            FuelFilterFail,
            BaggageDoor,
            SimConnectFail,
            Spare1,
            Spare2,
            Spare3,
            Spare4,
            Spare5,
            TurbineOverTemp,
        }

        private void chkEnableAIRadar_CheckedChanged(object sender, EventArgs e)
        {
            // Turn on/off the radar timer to update the radar info  (runs every second)
            this.AIRadarTimer.Enabled = this.chkEnableAIRadar.Checked;
            // Force the panel to redraw now rather than wait one second
            this.pnlAIRadar.Invalidate();
        }

        private void AIRadarTimer_Tick(object sender, EventArgs e)
        {
            // Every second we update the Ground and Airborne AI trafic info
            AI.RefreshAITrafficInformation(this.chkShowGroundAI.Checked, this.chkShowAirborneAI.Checked);
            // Apply a filter according to the range set by the user
            // Filtering ground and airborne traffic, no bearing filter (include from 0-360)
            // No altitude filter (passing nulls)
            // Range as set by the combo box.
            AI.ApplyFilter(true, true, 0, 360, null, null, double.Parse(this.cbxRadarRange.Text));
            // Invalidate the radar panel so it redraws.
            this.pnlAIRadar.Invalidate();
        }

        private void pnlAIRadar_Paint(object sender, PaintEventArgs e)
        {
            // This gets called whenever the panel needs to draw itself.
            if (this.chkEnableAIRadar.Checked)
            {
                // First Clear the panel and make a black background
                e.Graphics.Clear(Color.Black);
                // Start by working out the centre of the radar and draw the centre cross
                Point centre = new Point(this.pnlAIRadar.ClientSize.Width / 2, this.pnlAIRadar.ClientSize.Height / 2);
                e.Graphics.DrawLine(Pens.White, centre.X - 4, centre.Y, centre.X + 4, centre.Y);
                e.Graphics.DrawLine(Pens.White, centre.X, centre.Y - 4, centre.X, centre.Y + 4);
                double range = double.Parse(this.cbxRadarRange.Text);
                // work out the scale using the range and the smallest size of the panel
                double scale = range / (double)(this.pnlAIRadar.ClientSize.Width < this.pnlAIRadar.ClientSize.Height ? this.pnlAIRadar.ClientSize.Width : this.pnlAIRadar.ClientSize.Height) * 2d;
                // Go through each plane and draw it on the radar
                // Note: We are using the seperate collections for the ground and airborne
                // There is a collection of all AI traffic called 'AllTraffic' which can be used if
                // you do not want to deal with these seperatley.
                // First, draw the ground AI if required
                if (this.chkShowGroundAI.Checked)
                {
                    // Loop through the collection of plane objects in the GroundTraffic collection.
                    foreach (AIPlaneInfo plane in AI.GroundTraffic)
                    {
                        // Here we just pass the planeInfo off to the draw routine.
                        // There is quite a lot of information available in the AIPlaneInfo object.
                        // See the reference manual or Intellisense for details.
                        drawTarget(e.Graphics, scale, centre, plane);
                    }
                }
                // Next, draw the Airborne AI if required
                if (this.chkShowAirborneAI.Checked)
                {
                    // Loop through the collection of plane objects in the GroundTraffic collection.
                    foreach (AIPlaneInfo plane in AI.AirbourneTraffic)
                    {
                        drawTarget(e.Graphics, scale, centre, plane);
                    }
                }
            }
            else
            {
                // Radar turned off so just clear it with white
                e.Graphics.Clear(Color.White);
            }
        }

        private void drawTarget(Graphics graphics, double scale, Point centre, AIPlaneInfo plane)
        {
            // We are going to use some of the info from the plane object to draw the target.
            // Lots more info is avilable for other application.  
            // See the reference manual or Intellisense for details.
            // Work out the range of the target in pixels by multiplying by the scale
            double distancePixels = plane.DistanceNM / scale;
            // Work out the position from the centre using this distance and the bearing
            double dx = Math.Cos(degreeToRadian(plane.BearingTo)) * distancePixels;
            double dy = Math.Sin(degreeToRadian(plane.BearingTo)) * distancePixels;
            PointF target = new PointF((float)centre.X + (float)dx, (float)centre.Y + (float)dy);
            // Draw the target circle around this point oriented to the plane's heading 
            graphics.DrawEllipse(Pens.LightGreen, target.X - 4f, target.Y - 4f, 8f, 8f);
            // Draw a line from the circle to indicate heading
            double tailHeading = 180d + plane.HeadingDegrees;
            dx = Math.Cos(degreeToRadian(tailHeading)) * 12;
            dy = Math.Sin(degreeToRadian(tailHeading)) * 12;
            PointF tailEnd = new PointF(target.X + (float)dx, target.Y + (float)dy);
            graphics.DrawLine(new Pen(new LinearGradientBrush(target, tailEnd, Color.LightGreen, Color.DarkGreen)), target, tailEnd);
            // Work out the position of the data block
            PointF dataBlock = new PointF(target.X + 20, target.Y - 20);
            // Draw the line to the datablock
            graphics.DrawLine(Pens.LightGreen, new PointF(target.X + 5, target.Y - 5), new PointF(dataBlock.X - 5, dataBlock.Y + 7));
            // Draw the data block
            // Line 1 - the Callsign
            graphics.DrawString(plane.ATCIdentifier, this.pnlAIRadar.Font, Brushes.LightGreen, dataBlock);
            // Line 2 - the Altitude (hundreds of feet) and speed
            string line2 = "";
            line2 += ((int)(plane.AltitudeFeet / 100d)).ToString("d3");
            // Put a +,- or = depending on if the plane is decending, climbing or level
            if (plane.VirticalSpeedFeet < 0)
            {
                line2 += "-";
            }
            else if (plane.VirticalSpeedFeet > 0)
            {
                line2 += "+";
            }
            else
            {
                line2 += "=";
            }
            graphics.DrawString(line2, this.pnlAIRadar.Font, Brushes.LightGreen, new PointF(dataBlock.X, dataBlock.Y + 12));
            // Line 3 - origin, destination and assigned runway
            graphics.DrawString(plane.DepartureICAO + "->" + plane.DestinationICAO + " " + plane.RunwayAssigned.ToString(), this.pnlAIRadar.Font, Brushes.LightGreen, new PointF(dataBlock.X, dataBlock.Y + 24));
        }

        private double degreeToRadian(double angle)
        {
            return Math.PI * angle / 180.0;
        }

        private void btnMoveToEGLL_Click(object sender, EventArgs e)
        {
            // Suspend the timers
            if (this.timer1.Enabled)
            {
                this.timer1.Enabled = false;
                if (this.chkEnableAIRadar.Checked)
                {
                    this.AIRadarTimer.Enabled = false;
                }
                // Put the sim into Slew mode
                slewMode.Value = 1;
                FSUIPCConnection.Process();
                // Make a new point representing the centre of the threshold for 27L
                FsLatitude lat = new FsLatitude(51.464943d);
                FsLongitude lon = new FsLongitude(-0.434046d);
                FsLatLonPoint newPos = new FsLatLonPoint(lat, lon);
                // Now move this point 150 metres up the runway
                // Use one of the OffsetBy methods of the FsLatLonPoint class  
                double rwyTrueHeading =  269.7d;
                newPos = newPos.OffsetByMetres(rwyTrueHeading, 150);
                // Set the new position
                playerLatitude.Value = newPos.Latitude.ToFSUnits8();
                playerLongitude.Value = newPos.Longitude.ToFSUnits8();
                // set the heading and altitude
                playerAltitude.Value = 0;
                playerHeadingTrue.Value = (uint)(rwyTrueHeading * (65536d * 65536d) / 360d);
                FSUIPCConnection.Process();
                // Turn off the slew mode
                slewMode.Value = 0;
                FSUIPCConnection.Process();
                // Refresh the scenery
                sendControl.Value = REFRESH_SCENERY;
                FSUIPCConnection.Process();
                // Reenable the timers
                this.timer1.Enabled = true;
                this.AIRadarTimer.Enabled = this.chkEnableAIRadar.Checked;
            }
        }

        private void tabPage3_Click(object sender, EventArgs e)
        {

        }

        private string BCDToString(short BCDToConvert)
        {
            string wrkString;
            // a. Convert to a string in Hexadecimal format
            wrkString  = BCDToConvert.ToString("X");
            // b. Add the assumed '1' and insert the decimal point
            if (wrkString.Length > 3)
                wrkString = "1" + wrkString.Substring(0, 2) + "." + wrkString.Substring(2, 2);
            else
                wrkString = "10" + wrkString.Substring(0, 1) + "." + wrkString.Substring(1, 2);
            return wrkString;
        }

        private void lblDebug_Click(object sender, EventArgs e)
        {

        }




    }

    // A double buffered panel used for the radar scope.
    // The normal panel that .NET supplies doesn't use double buffering 
    // and therefore suffers from massive flickering issues.
    public class DoubleBufferPanel : Panel
    {
        public DoubleBufferPanel()
        {
            // Set the value of the double-buffering style bits to true.
            this.SetStyle(ControlStyles.DoubleBuffer |
            ControlStyles.UserPaint |
            ControlStyles.AllPaintingInWmPaint,
            true);

            this.UpdateStyles();
        }
    }

    
}
