using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading;
using System.Net.Sockets;
using System.Net;
using System.IO.Ports;
using F4GPSOut.Headers;

namespace DCS_TO_GPS
{
    


    class Program
    {


        static Boolean ldebugging = false;
        static public string GPSComPort;
        static public ProgramMode OperationalMode;
        
        static public double gLat = 0;     // N/S to five decimal places
        static public double gLong = 0;    // E/W to five decimal places

        static public string xoutputstr;
        static public string youtputstr;

        static public String outNorS;
        static public String outEorW;

        // Start in Brisbane
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

        static Byte[] receiveBytes;

        static DateTime LastUpdateToGPS;
        static int GPSTimeout = 100;
        static int UDPSocketTimeout = 1000;

        static SerialPort serialPort1;

        public enum ProgramMode
        {
            DemoMode,
            F4Mode,
            LomacMode
        };



        static public F4GPSout.BMS4FlightData F4Data;



        
        static void Main(string[] args)
        {

            Console.WriteLine("DCS_to_GPS");

             // Setup call back function to clean up network ports on a CTRL-C
            Console.CancelKeyPress += delegate
            {
                cleanUp();
            };


            // Init Network
            UdpClient receivingUdpClient = new UdpClient(7783);
            receivingUdpClient.Client.ReceiveTimeout = UDPSocketTimeout;
            Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            Thread.Sleep(500);

            // Init Time
            LastUpdateToGPS = DateTime.Now;


            Lomac_GPS_Altitude = 4000;
            Lomac_GPS_Heading = 0;
            Lomac_GPS_Speed = 400;
            Lomac_GPS_Latitude = -27.3873;
            Lomac_GPS_Longitude = 153.1301;

            DCS_TO_GPSOut_init();
            while (true)
            {


                
       

                // Blocks until a message returns on this socket from a remote host.
                try
                {
                    receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);
                    string returnData = Encoding.ASCII.GetString(receiveBytes);

                    //Console.WriteLine("Pkt Rx " + returnData.ToString());
                    //Console.WriteLine(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"));
                    //Console.WriteLine(LastUpdateToGPS.ToString("yyyy-MM-dd HH:mm:ss.fff"));
                    if (LastUpdateToGPS.AddMilliseconds(GPSTimeout) < DateTime.Now)
                    {
                        if (ldebugging) Console.WriteLine("Its been a while update GPS");



                        // Time to convert received String
                        // Latitude=44.084018453225:Longitude=43.087468616326:Altitude=1719.7390136719:Airspeed=271.3784286499:Heading=318.92603060603:


                        ProcessReceivedString(returnData);


                        ReadAndConvertSharedMem();
                        LastUpdateToGPS = DateTime.Now;
                    }

                   
                }
                catch (SocketException ex)
                {

                    if (ldebugging)  Console.WriteLine(ex.ErrorCode.ToString());

                    if (ex.SocketErrorCode != SocketError.TimedOut)

                        throw new Exception("Unexpected Socket error", ex);

                    else
                    {
                        Console.WriteLine("No packet received within timeout period");
                        Console.WriteLine("Updating GPS during idle period ");
                        ReadAndConvertSharedMem();
                    }
                }  



                
            }
        }

        static void DCS_TO_GPSOut_init()
        {
            foreach (string arg in Environment.GetCommandLineArgs())
            {
                Console.WriteLine("Parameters passed " + arg);
                if (arg.IndexOf("/?") != -1)
                {
                    Console.WriteLine("DCS");
                    Console.WriteLine("reset - clear config settings");
                    Console.WriteLine("debug");
                    Environment.Exit(0);
                }
                else if (arg.IndexOf("debug") != -1)
                {
                    ldebugging = true;
                }
                else if (arg.IndexOf("Falcon") != -1)
                {
                    OperationalMode = ProgramMode.F4Mode;
                    
                }
                else if (arg.IndexOf("DCS") != -1)
                {
                    OperationalMode = ProgramMode.LomacMode;
                    
                }

                else if (arg.IndexOf("reset") != -1)
                {
                    Console.WriteLine("Reseting Configuration to default");
                    Console.ReadLine();   
                    Properties.Settings.Default.GPSComPort = "Com0";
                    Properties.Settings.Default.Save();
                    Environment.Exit(0);

                }

                
            }



            GPSComPort = Properties.Settings.Default.GPSComPort;



            bool bFoundPort = false;
            foreach (string s in SerialPort.GetPortNames())
            {
                if (0 == String.Compare(s, GPSComPort, true))
                {
                    bFoundPort = true;
                    Console.WriteLine("Using " + GPSComPort);
                }
            }


            if (!bFoundPort)
            {

                string strYorN;
                Console.WriteLine("Unable to find configured Serial Port " + GPSComPort);
                Console.WriteLine("Press Enter to Continue");
                Console.ReadLine();
                Console.WriteLine("Installed Serial Ports");
                foreach (string s in SerialPort.GetPortNames())
                {
                    Console.Write(s);
                    Console.Write(" Use this port (Y/N): ");
                    strYorN = Console.ReadLine();
                    if (strYorN.Contains("Y") || strYorN.Contains("y"))
                    {
                        Console.WriteLine("Saving GPS Serial Port " + s);
                        Properties.Settings.Default.GPSComPort = s;
                        Properties.Settings.Default.Save();
                        Console.WriteLine("Press Enter to Continue");
                        Console.ReadLine();
                        Environment.Exit(0);
                        break;
                    }

                }
                Console.WriteLine("Press Enter to Continue");
                Console.ReadLine();
            }
            else
            {

                Console.WriteLine("Opening " + GPSComPort);
                serialPort1 = new SerialPort();
                serialPort1.PortName = GPSComPort;
                serialPort1.BaudRate = 19200;

                serialPort1.Open();
                if (!serialPort1.IsOpen)
                {
                    Console.WriteLine("Unable to open Com Port " + GPSComPort, "Unable to open Comm Port");
                    Console.ReadLine();
                    Environment.Exit(0);
                }
                else
                {
                    Console.WriteLine("Opened " + GPSComPort + " @ " + serialPort1.BaudRate.ToString() );
                }
            }




        }


        static void ProcessReceivedString(string StringToBeProcessed)
        {

            {

                
                String sWrkstr = "";
                String[] strvariablename;
                char[] secondDelimiterChars =  {'='};

   

                if (ldebugging) Console.WriteLine("Start ProcessStream");
                if (ldebugging) Console.WriteLine("Processing Stream {0}", StringToBeProcessed);

                char[] delimiterChars = { ' ', ',', ':', '\r' };


                if (ldebugging) System.Console.WriteLine("Original text: '{0}'", StringToBeProcessed);

                string[] words = StringToBeProcessed.Split(delimiterChars);
                if (ldebugging) System.Console.WriteLine("{0} words in text:", words.Length);

                // There should only be three things in each line
                // attribute - = - value
                // If there is more than that ignore

                if (words.Length != 6)
                {
                    Console.WriteLine("Invalid number of parameters in " + StringToBeProcessed);
                    Console.WriteLine("Should be 6, but " + words.Length + " returned");
                    return;
                }


                for (int i = 0; i < 6; i++)
                {
                    if (ldebugging) Console.WriteLine(words[i]);

                    
                    strvariablename = words[i].Split(secondDelimiterChars);

                    switch (strvariablename[0])
                    {


                        case "Airspeed":
                            {
                                if (ldebugging) Console.WriteLine("Airspeed" + strvariablename[1]);
                                float workfloat = float.Parse(strvariablename[1]);
                                Lomac_GPS_Speed = workfloat;
                                break;
                            }

                        case "Altitude":
                            {
                                if (ldebugging) Console.WriteLine("Altitude " + strvariablename[1]);
                                float workfloat = float.Parse(strvariablename[1]);
                                Lomac_GPS_Altitude = workfloat;
                                break;
                            }

                        case "Heading":
                            {
                                if (ldebugging) Console.WriteLine("Heading " + strvariablename[1]);
                                float workfloat = float.Parse(strvariablename[1]);
                                Lomac_GPS_Heading = workfloat;
                                break;

                            }


                        case "Latitude":
                            {
                                if (ldebugging) Console.WriteLine("Latitude " + strvariablename[1]);
                                float workfloat = float.Parse(strvariablename[1]);
                                Lomac_GPS_Latitude = workfloat;
                                
                                break;
                            }

                        case "Longitude":
                            {
                                if (ldebugging) Console.WriteLine("Longitude " + strvariablename[1]);
                                float workfloat = float.Parse(strvariablename[1]);
                                Lomac_GPS_Longitude = workfloat;
                                break;
                            }



                        //default:
                        //     {
                        //         // Haven't found an attribute of interest - throw it out
                        //         Console.WriteLine("Not Processing " + words[1]);

                        //     }
                        }   

                    }

                // At this point should only be handling the special cases needing state management


                if (ldebugging) Console.WriteLine("End ProcessStream");

            }
        }

        static void ReadAndConvertSharedMem()
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


            string lblAltitude;
            string lblSpeed;
            string lbllomaclat;
            string lbllomaclong;
            string gLong;
            string gLat;

            wrkf4alt = Lomac_GPS_Altitude;
            wrkf4speed = Lomac_GPS_Speed;

            wrkf4heading = Lomac_GPS_Heading;

            lblAltitude = wrkf4alt.ToString();

            lblSpeed = wrkf4speed.ToString();

            // Lomac

            lbllomaclat = Lomac_GPS_Latitude.ToString();
            lbllomaclong = Lomac_GPS_Longitude.ToString();

            gLong = Lomac_GPS_Longitude.ToString();
            gLat = Lomac_GPS_Latitude.ToString();


            // Separate out MS - so we went up with DMS
            if (!(double.TryParse(lbllomaclat, out xdecdegrees)))
            {
                Console.WriteLine("Unable to convert Lomac Lat to double " + lbllomaclat, "Conversion to Double Failed");
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
            xdegrees = (int)xdecdegrees;
            xminutes = (xdecdegrees - xdegrees) * 60;
            xseconds = (xminutes - ((int)xminutes)) * 60 / 100;
            xminutes = (int)xminutes;
            xoutputstr = xdegrees.ToString("00") + xminutes.ToString("00") + xseconds.ToString(".0000");



            // Separate out MS for Longitude

            if (!(double.TryParse(lbllomaclong, out ydecdegrees)))
            {
                Console.WriteLine("Unable to convert Lomac Long to double " + lbllomaclong, "Conversion to Double Failed");
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

            ydegrees = (int)ydecdegrees;
            yminutes = (ydecdegrees - ydegrees) * 60;
            yseconds = (yminutes - ((int)yminutes)) * 60 / 100;
            yminutes = (int)yminutes;

            youtputstr = ydegrees.ToString("000") + yminutes.ToString("00") + yseconds.ToString(".00000");

            // lomac land
            // youtputstr = "03500.00000000"

            


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

        static void CalcAndWriteToSerialPort()
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
            serialPort1.Write(outcompletestring);


            // Out Satellite status
            outcompletestring = "$GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*";
            chksum = getChecksum(outcompletestring);

            outcompletestring = outcompletestring + chksum + "\x0d\x0a";


            // Output the GPGGA sentence
            serialPort1.Write(outcompletestring);

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

        public static void cleanUp()
        {
            Console.WriteLine("Closing Connections!");

        }

    }
}
