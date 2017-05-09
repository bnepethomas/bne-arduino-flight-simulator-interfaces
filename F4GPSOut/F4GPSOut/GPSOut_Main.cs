using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO.MemoryMappedFiles;
using F4GPSOut.Headers;

namespace F4GPSOut
{

    
    public partial class GPSOut_Main : Form
    {

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public class SpeedFanSharedMem
        {
            ushort version;
            ushort flags;
            Int32 size;
            Int32 handle;
            ushort numTemps;
            ushort numFans;
            ushort numVolts;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public Int32[] temps;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public Int32[] fans;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public Int32[] volts;

            public SpeedFanSharedMem()
            {
                temps = new Int32[32];
                fans = new Int32[32];
                volts = new Int32[32];
            }
        }

        public const int PROCESS_ALL_ACCESS = 0x1F0FFF;
        public const int FILE_MAP_READ = 0x0004;

        [DllImport("Kernel32.dll", CharSet = CharSet.Auto)]
        internal static extern IntPtr OpenFileMapping(int dwDesiredAccess, bool bInheritHandle, StringBuilder lpName);

        [DllImport("Kernel32.dll")]
        internal static extern IntPtr MapViewOfFile(IntPtr hFileMapping,
                int dwDesiredAccess, int dwFileOffsetHigh, int dwFileOffsetLow, int dwNumberOfBytesToMap);

        [DllImport("Kernel32.dll")]
        internal static extern bool UnmapViewOfFile(IntPtr map);

        [DllImport("kernel32.dll")]
        internal static extern bool CloseHandle(IntPtr hObject);
        
        public enum ProgramMode
        {
            DemoMode,
            F4Mode,
            LomacMode
        };


        static public F4GPSout.BMS4FlightData F4Data;


        static public string GPSComPort;
        public ProgramMode OperationalMode;
        
        static public double gLat = 0;     // N/S to five decimal places
        static public double gLong = 0;    // E/W to five decimal places

        static public string xoutputstr;
        static public string youtputstr;

        static public String outNorS;
        static public String outEorW;

        static public string outSpeed = "100";
        static public string outAltitude = "1000";
        static public string outLatitude = "44.89";
        static public string outLongitude = "35.39";
        static public string outTrackMadeGood = "000";
        static public string outMagVar = "00.0";

        static public double Lomac_GPS_Altitude = 100;
        static public double Lomac_GPS_Heading = 90;
        static public double Lomac_GPS_Latitude = -27.3873;
        static public double Lomac_GPS_Longitude = 153.1301;
        static public double Lomac_GPS_Speed = 333;


        public GPSOut_Main()
        {
            InitializeComponent();
        }

        private void manualPositionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frmManualPosition fManPos = new frmManualPosition();
            fManPos.Show();
        }

        private void GPSOut_Main_Load(object sender, EventArgs e)
        {



            foreach (string arg in Environment.GetCommandLineArgs())
            {
                if (arg.IndexOf("Falcon") != -1)
                {
                    OperationalMode = ProgramMode.F4Mode;
                    break;
                }
                else if (arg.IndexOf("Lomac") != -1)
                {
                    OperationalMode = ProgramMode.LomacMode;
                    break;
                }

                
            }



            GPSComPort = Properties.Settings.Default.GPSComPort;

            this.serialPort1.PortName = GPSComPort;

            bool bFoundPort = false;
            foreach (string s in SerialPort.GetPortNames())
            {
                if (0 == String.Compare(s, this.serialPort1.PortName, true))
                {
                    bFoundPort = true;
                }
            }

            if (!bFoundPort)
            {
                
                frmSelectComPort frmSelComPort = new frmSelectComPort();
                frmSelComPort.ShowDialog();
                this.serialPort1.PortName = GPSComPort;
            }

            UpdateMainTitleBar();
            // this.Text = "Using Comm Port " + GPSComPort;

            serialPort1.BaudRate = 19200;
  
            serialPort1.Open();
            if (!serialPort1.IsOpen)
            {
                MessageBox.Show("Unable to open Com Port " + GPSComPort, "Unable to open Comm Port");
                return ;
            }


            Lomac_GPS_Altitude = 4000;
            Lomac_GPS_Heading = 0;
            Lomac_GPS_Speed = 400;
            Lomac_GPS_Latitude = -27.3873;
            Lomac_GPS_Longitude = 153.1301;

            this.timer1.Enabled = true;
        }



        private static string getChecksum(string sentence)
        {
            //Start with first Item
            int checksum = Convert.ToByte(sentence[sentence.IndexOf('$') + 1]);
            // Loop through all chars to get a checksum
            for (int i = sentence.IndexOf('$') + 2; i < sentence.IndexOf('*'); i++)
            {
                // No. XOR the checksum with this character's value
                checksum ^= Convert.ToByte(sentence[i]);
            }
            // Return the checksum formatted as a two-character hexadecimal
            return checksum.ToString("X2");
        }



        public void ReadAndConvertSharedMem()
        {
            // This routine reads the F4 Shared Memory File and then outputs it to the GPS
            // Simple data transformation aside for converting the x/y to Lat Long

            double wrkf4alt;
            double wrkf4heading;
            double wrkf4speed;

            double xdecdegrees;
            double xdegrees;
            double xminutes;
            double xseconds;

            double ydecdegrees;
            double ydegrees;
            double yminutes;
            double yseconds;


            /*
            If Me.chkF4.Value > 0 Then
                Me.tmrUpdateFromlomac = True
            Else
                Me.tmrUpdateFromlomac = False
            End If
            */


            wrkf4alt = Lomac_GPS_Altitude;
            wrkf4speed = Lomac_GPS_Speed;

            wrkf4heading = Lomac_GPS_Heading;

            this.lblAltitude.Text = wrkf4alt.ToString();

            this.lblSpeed.Text = wrkf4speed.ToString();

            // Lomac

            this.lbllomaclat.Text = Lomac_GPS_Latitude.ToString();
            this.lbllomaclong.Text = Lomac_GPS_Longitude.ToString();

            GPSOut_Main.gLong = Lomac_GPS_Longitude;
            GPSOut_Main.gLat = Lomac_GPS_Latitude;


            // Separate out MS - so we went up with DMS
            if (!(double.TryParse(this.lbllomaclat.Text, out xdecdegrees)))
            {
                MessageBox.Show("Unable to convert Lomac Lat to double " + this.lbllomaclat.Text, "Conversion to Double Failed");   
            }


            if (xdecdegrees < 0)
            {
                xdecdegrees = xdecdegrees * -1;
                outNorS = "S";
            }
            else
            {
                outNorS = "N";
            }
            xdegrees = (int) xdecdegrees;                          
            xminutes = (xdecdegrees - xdegrees) * 60;
            xseconds = (xminutes - ((int) xminutes)) * 60 /100 ;   
            xminutes = (int) xminutes;                              
            xoutputstr = xdegrees.ToString("00") +  xminutes.ToString("00") + xseconds.ToString(".0000");
            
  
            
            // Separate out MS for Longitude

            if (!(double.TryParse(this.lbllomaclong.Text, out ydecdegrees)))
            {
                MessageBox.Show("Unable to convert Lomac Long to double " + this.lbllomaclong.Text, "Conversion to Double Failed");   
            }


            if (ydecdegrees < 0)
            {
                ydecdegrees = ydecdegrees * -1;
                outEorW = "W";
            }
            else
            {
                outEorW = "E";
            }

            ydegrees = (int) ydecdegrees;                           
            yminutes = (ydecdegrees - ydegrees) * 60;
            yseconds = (yminutes - ((int) yminutes)) * 60 /100;     
            yminutes = (int) yminutes;                              

            youtputstr = ydegrees.ToString("000") + yminutes.ToString("00") + yseconds.ToString(".00000");

            // lomac land
            // youtputstr = "03500.00000000"

            this.lblLatLong.Text = xoutputstr + " " + youtputstr;
            


            // Output values to GPS
            outAltitude = Lomac_GPS_Altitude.ToString();
            outLongitude = Lomac_GPS_Longitude.ToString();
            outLatitude = Lomac_GPS_Latitude.ToString();
            outSpeed = wrkf4speed.ToString();
            outMagVar = "0";
            outTrackMadeGood = wrkf4heading.ToString();

            // Write them out
            CalcAndWriteToSerialPort();


        }

        public void CalcAndWriteToSerialPort()
        {

            //Vars Required
            String outStartOfString;        // Always starts with $ rest changes
            String outUTC;                  // hhmmss.ss
            String outStatus;               // A for active V for warning


            String outDate;                 // ddmmyy

            String outMagEorW;              // Mag Variation East or West
            String outEndOfString;          // Always *
            String outcompletestring;


            String outGPSFix;
            String outNoofSatellites;
            String outPrecision;

            

            // Generate GPS out
            // Build string
            // Start with fixed values
            outStartOfString = "$GPRMC";

            DateTime dtnow;
            dtnow = DateTime.Now;
            outUTC = dtnow.ToString("HHmmff") + ".00";
            // outUTC = "160533.00";
            outStatus = "A";
            outDate = dtnow.ToString("ddmmyy");
            // outDate = "010413";
            outEndOfString = "*";

            

            outMagEorW = "E";

            // Assemble string
            outcompletestring = outStartOfString + "," + outUTC + ",";
            outcompletestring = outcompletestring + outStatus + ",";
            outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ",";
            outcompletestring = outcompletestring + youtputstr + "," + outEorW + ",";
            outcompletestring = outcompletestring + outSpeed + ",";
            outcompletestring = outcompletestring + outTrackMadeGood + ",";
            outcompletestring = outcompletestring + outDate + ",";
            outcompletestring = outcompletestring + outMagVar + "," + outMagEorW + outEndOfString;

            // this.textBox1.Text = outcompletestring;

            string chksum;
            chksum = getChecksum(outcompletestring);

            outcompletestring = outcompletestring + chksum + "\x0d\x0a";
            // this.textBox1.Text = this.textBox1.Text + "\x0d\x0a" + outcompletestring; 

            serialPort1.Write(outcompletestring);

            // Test String
            // serialPort1.Write("$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73" + "\x0d\x0a");


            //'Output Altitude information
            //'$GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E
            //'         1         2       3   4        5 6  7  8   9    10 etc
            //'
            //' Field Number:
            //'  1) Universal Time Coordinated (Utc)
            //'  2) Latitude
            //'  3) N Or S (North Or South)
            //'  4) Longitude
            //'  5) E Or W (East Or West)
            //'  6) Gps Quality Indicator,
            //'     0 - Fix Not Available,
            //'     1 - Gps Fix,
            //'     2 - Differential Gps Fix
            //'  7) Number Of Satellites In View, 00 - 12
            //'  8) Horizontal Dilution Of Precision
            //'  9) Antenna Altitude Above/Below Mean-Sea-Level (Geoid)
            //' 10) Units Of Antenna Altitude, Meters
            //' 11) Geoidal Separation, The Difference Between The Wgs-84 Earth
            //'     Ellipsoid And Mean-Sea-Level (Geoid), "-" Means Mean-Sea-Level
            //'     Below Ellipsoid
            //' 12) Units Of Geoidal Separation, Meters
            //' 13) Age Of Differential Gps Data, Time In Seconds Since Last Sc104
            //'     Type 1 Or 9 Update, Null Field When Dgps Is Not Used
            //' 14) Differential Reference Station Id, 0000-1023
            //' 15) Checksum

            // Start with fixed values
            outStartOfString = "$GPGGA";
            outUTC = dtnow.ToString("HHmmff") + ".00";
            outGPSFix = "2";
            outNoofSatellites = "05";
            outPrecision = "0.0";
            outEndOfString = "*";

            outcompletestring = outStartOfString + "," + outUTC + ",";
            outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ",";
            outcompletestring = outcompletestring + youtputstr + "," + outEorW + ",";
            outcompletestring = outcompletestring + outGPSFix + ",";
            outcompletestring = outcompletestring + outNoofSatellites + ",";
            outcompletestring = outcompletestring + outPrecision + ",";
            outcompletestring = outcompletestring + outAltitude + ".0,M,,,,";
            outcompletestring = outcompletestring + outEndOfString;

 
            chksum = getChecksum(outcompletestring);

            outcompletestring = outcompletestring + chksum + "\x0d\x0a";



            // Output the GPGGA sentence
            this.serialPort1.Write(outcompletestring);
 

            // Out Satellite status
            outcompletestring = "$GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*";
            chksum = getChecksum(outcompletestring);

            outcompletestring = outcompletestring + chksum + "\x0d\x0a";


            // Output the GPGGA sentence
            this.serialPort1.Write(outcompletestring);

            /*
            While MSComm2.OutBufferCount > 0
                DoEvents
            Wend
            'Me.MSComm2.PortOpen = False

            If Me.Check1.Value > 0 Then
                Me.Timer1.Enabled = True
            End If

            */

        }


        private void UpdateMainTitleBar()
        {
            string wrkstr;
            switch (OperationalMode)
            {
                case ProgramMode.DemoMode:
                {
                    wrkstr = "Demo Mode";
                    break;
                }
                
                case ProgramMode.F4Mode: 
                {
                    wrkstr = "F4 Mode";
                    break;
                }

                case ProgramMode.LomacMode:
                {
                    wrkstr = "Lomac Mode";
                    break;
                }
                default:
                {
                    wrkstr = "Unknown Mode";
                    break;
                }

            }

            this.Text = "Using Comm Port " + GPSComPort + " " + wrkstr ;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.timer1.Enabled = (!this.timer1.Enabled);
            
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            ReadAndConvertSharedMem();

            if (OperationalMode == ProgramMode.F4Mode)
            {
                GetF4Data();
            }
        }

        private void selectComPortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.timer1.Enabled = false;
            serialPort1.Close();
            frmSelectComPort frmSelComPort = new frmSelectComPort();
            frmSelComPort.ShowDialog();
            UpdateMainTitleBar();
            // this.Text = "Using Comm Port " + GPSComPort;
            serialPort1.PortName = GPSComPort;
            serialPort1.Open();
            this.timer1.Enabled = true;
        }

        private void GetF4Data()
        {

            
            StringBuilder sharedMemFile = new StringBuilder("FalconSharedMemoryArea");
            IntPtr handle = OpenFileMapping(FILE_MAP_READ, false, sharedMemFile);
            SpeedFanSharedMem sm;
            IntPtr mem = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, Marshal.SizeOf((Type)typeof(F4GPSout.BMS4FlightData)));
            if (mem == IntPtr.Zero)
            {
                // throw new Exception("Unable to read shared memory.");
                // Console.WriteLine("What happened");
                this.Text = "Unable to open Falcon";
                return;
            }
            this.Text = "Opened Falcon";
            F4Data = (F4GPSout.BMS4FlightData)Marshal.PtrToStructure(mem, typeof(F4GPSout.BMS4FlightData));

            double Feet_in_lat = 334000;
            double XFeet_In_Lat = 337000;


            double Lat_Zero_Point = 123;
            double Long_Zero_Point = 33.851;
            //' Determine reference zero point
            //If Me.optKorea = True Then
            //    ' Korea
            //    Lat_Zero_Point = 123
            //    Long_Zero_Point = 33.851
            //Else
            //' Balkans
            //    Lat_Zero_Point = 10.98
            //    Long_Zero_Point = 36.658
            //End If



            Lomac_GPS_Altitude = F4Data.z / -3.2808399;
            Lomac_GPS_Heading = F4Data.currentHeading;
            Lomac_GPS_Speed = F4Data.kias;

            Lomac_GPS_Latitude = Long_Zero_Point +  F4Data.x / (XFeet_In_Lat + 10);
            Lomac_GPS_Longitude = Lat_Zero_Point + F4Data.y / Feet_in_lat;

            UnmapViewOfFile(handle);
            CloseHandle(handle);

            ReadAndConvertSharedMem();

        }

    }
}
