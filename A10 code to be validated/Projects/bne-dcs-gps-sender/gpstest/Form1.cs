using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {

        string outcompletestring;
        string outStartOfString;
        string outUTC;
        string outStatus;
        string outDate;
        string outEndOfString;
        string outNorS;
        string outEorW;
        string outMagEorW;
        string xoutputstr = "a";
        string youtputstr = "a";
        static string outSpeed = "100";
        static string outTrackMadeGood = "000";
        string outMagVar = "00.0";
        string outGPSFix = "2";
        string outNoofSatellites = "05";
        string outPrecision;
        static string outAltitude = "1000";
        static string outLatitude = "44.89";
        static string outLongitude = "35.39";
        static string wrkf4alt;
        string wrkf4speed;
        string wrkf4heading;
        //string xdegrees;
        //string xminutes;
        //string xseconds;
        //string ydegrees;
        //string yminutes;
        //string yseconds;
        double dbloutLatitude;
        double dbloutLongitude;




        string lastNetworkStatus;
        public static bool messageReceived = false;
        static Socket newsock;
        static EndPoint Remote;
        static String ThreadStatus;
        Thread Receive_UDP_Thread;


        public Form1()
        {
            InitializeComponent();
            try
            {
                this.serialPort1.BaudRate = 19200;
                this.serialPort1.Open();
                Receive_UDP_Thread = new Thread(new ThreadStart(Receive_DCS_UDP));
                Receive_UDP_Thread.Start();
                Receive_UDP_Thread.IsBackground = true;
                timer2.Interval = 100;
                timer2.Enabled = true;
            }
            catch (Exception caughterror)
            {
                
                MessageBox.Show(caughterror.Message.ToString());

            }

            
        }

        private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                Receive_UDP_Thread.Abort();
                this.serialPort1.Close();
            }

            catch (Exception err)
            {
                MessageBox.Show(err.ToString());
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                //this.serialPort1.BaudRate = 19200;
                //this.serialPort1.Open();

                //this.serialPort1.WriteLine("$GPRMC,115447,A,0000.00,N,00000.00,E,40.000,0.000,220114,5.000,W*53");
                //this.serialPort1.WriteLine("$GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E");
                //MessageBox.Show( getChecksum("$GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*"));
                //MessageBox.Show( getChecksum("$GPRMC,115447,A,0000.00,N,00000.00,E,40.000,0.000,220114,5.000,W*"));
                //MessageBox.Show(getChecksum("$GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*"));

                // Output Altitude information
                // $GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E
                //          1         2       3   4        5 6  7  8   9    10 etc
                //
                // Field Number:
                //  1) Universal Time Coordinated (Utc)
                //  2) Latitude
                //  3) N Or S (North Or South)
                //  4) Longitude
                //  5) E Or W (East Or West)
                //  6) Gps Quality Indicator,
                //     0 - Fix Not Available,
                //     1 - Gps Fix,
                //     2 - Differential Gps Fix
                //  7) Number Of Satellites In View, 00 - 12
                //  8) Horizontal Dilution Of Precision
                //  9) Antenna Altitude Above/Below Mean-Sea-Level (Geoid)
                // 10) Units Of Antenna Altitude, Meters
                // 11) Geoidal Separation, The Difference Between The Wgs-84 Earth
                //     Ellipsoid And Mean-Sea-Level (Geoid), "-" Means Mean-Sea-Level
                //     Below Ellipsoid
                // 12) Units Of Geoidal Separation, Meters
                // 13) Age Of Differential Gps Data, Time In Seconds Since Last Sc104
                //     Type 1 Or 9 Update, Null Field When Dgps Is Not Used
                // 14) Differential Reference Station Id, 0000-1023
                // 15) Checksum


                //string outcompletestring;
                //string outStartOfString;
                //string outUTC;
                //string outStatus;
                //string outDate;
                //string outEndOfString;
                //string outNorS;
                //string outEorW;
                //string outMagEorW;
                //string xoutputstr = "a";
                //string youtputstr = "a";
                //string outSpeed = "1";
                //string outTrackMadeGood = "1";
                //string outMagVar = "1";
                //string outGPSFix = "2";
                //string outNoofSatellites = "05";
                //string outPrecision;
                //string outAltitude = "1";
                //string outLatitude = "1";
                //string outLongitude = "1";
                //string wrkf4alt;
                //string wrkf4speed;
                //string wrkf4heading;
                ////string xdegrees;
                ////string xminutes;
                ////string xseconds;
                ////string ydegrees;
                ////string yminutes;
                ////string yseconds;
                //double dbloutLatitude;
                //double dbloutLongitude;



                //outLongitude = "35.39";
                //outLatitude = "44.89";




                wrkf4alt = outAltitude;
                wrkf4speed = outSpeed;

                wrkf4heading = outTrackMadeGood;

                this.lblAltitude.Text = wrkf4alt;

                this.lblSpeed.Text = wrkf4speed;

                // Lomac

                this.lbllomaclat.Text = outLatitude;
                this.lbllomaclong.Text = outLongitude;

                // Separate out MS - so we went up with DMS
                //xdecdegrees = this.lbllomaclat.Text;
                //xdegrees = Format(Int(xdecdegrees), "00")
                //xminutes = (xdecdegrees - xdegrees) * 60
                //xseconds = Format(((xminutes - Int(xminutes)) * 60) / 100, ".########")
                //xminutes = Format(Int((xdecdegrees - xdegrees) * 60), "00")
                //xoutputstr = xdegrees + xminutes + xseconds;

                double decimal_degrees;

                // set decimal_degrees value here
                dbloutLatitude = Convert.ToDouble(outLatitude);
                double xdegrees = Math.Floor(dbloutLatitude);
                double xminutes = (dbloutLatitude - Math.Floor(dbloutLatitude)) * 60.0;
                double xseconds = (xminutes - Math.Floor(xminutes)) * 60.0;
                double xtenths = (xseconds - Math.Floor(xseconds)) * 10.0;
                // get rid of fractional part
                xminutes = Math.Floor(xminutes);
                xseconds = Math.Floor(xseconds);
                xtenths = Math.Floor(xtenths);


                // Separate out MS for Longitude
                dbloutLongitude = Convert.ToDouble(outLongitude);
                double ydegrees = Math.Floor(dbloutLongitude);
                double yminutes = (dbloutLongitude - Math.Floor(dbloutLongitude)) * 60.0;
                double yseconds = (yminutes - Math.Floor(yminutes)) * 60.0;
                double ytenths = (yseconds - Math.Floor(yseconds)) * 10.0;
                // get rid of fractional part
                xminutes = Math.Floor(xminutes);
                xseconds = Math.Floor(xseconds);
                xtenths = Math.Floor(xtenths);

                //If ydegrees < 100 Then
                //    ydegrees = "0" & ydegrees
                //End If

                xoutputstr = xdegrees.ToString() + xminutes.ToString("00") + "." + xseconds.ToString("00") + xtenths.ToString("00");
                youtputstr = ydegrees.ToString("000") + yminutes.ToString("00") + "." + yseconds.ToString("00") + ytenths.ToString("00");

                //xoutputstr = "4725.8904";
                //youtputstr = "12218.4798";


                // lomac land
                //youtputstr = "03500.00000000"

                this.lblLatLong.Text = xoutputstr + " " + youtputstr;



                // Output values to GPS

                //outLongitude = youtputstr;
                //outLatitude = xoutputstr;
                //outSpeed = wrkf4speed;
                outMagVar = "0";
                //outTrackMadeGood = wrkf4heading;




                outStartOfString = "$GPRMC";
                outUTC = DateTime.Now.ToString("HHmmss") + ".00";
                outStatus = "A";
                outDate = DateTime.Now.ToString("ddmmyy");
                outEndOfString = "*";


                outNorS = "N";
                outEorW = "E";
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

                this.label1.Text = outcompletestring + getChecksum(outcompletestring);
                this.textBox1.Text = outcompletestring + getChecksum(outcompletestring);
                this.serialPort1.WriteLine(outcompletestring + getChecksum(outcompletestring) + "\r\n");




                // Output Altitude information
                //$GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E
                //         1         2       3   4        5 6  7  8   9    10 etc
                //
                // Field Number:
                //  1) Universal Time Coordinated (Utc)
                //  2) Latitude
                //  3) N Or S (North Or South)
                //  4) Longitude
                //  5) E Or W (East Or West)
                //  6) Gps Quality Indicator,
                //     0 - Fix Not Available,
                //     1 - Gps Fix,
                //     2 - Differential Gps Fix
                //  7) Number Of Satellites In View, 00 - 12
                //  8) Horizontal Dilution Of Precision
                //  9) Antenna Altitude Above/Below Mean-Sea-Level (Geoid)
                // 10) Units Of Antenna Altitude, Meters
                // 11) Geoidal Separation, The Difference Between The Wgs-84 Earth
                //     Ellipsoid And Mean-Sea-Level (Geoid), "-" Means Mean-Sea-Level
                //     Below Ellipsoid
                // 12) Units Of Geoidal Separation, Meters
                // 13) Age Of Differential Gps Data, Time In Seconds Since Last Sc104
                //     Type 1 Or 9 Update, Null Field When Dgps Is Not Used
                // 14) Differential Reference Station Id, 0000-1023
                // 15) Checksum

                // Start with fixed values
                outStartOfString = "$GPGGA";
                outGPSFix = "2";
                outNoofSatellites = "05";
                outPrecision = "0.0";



                outcompletestring = outStartOfString + "," + outUTC + ",";
                //outcompletestring = outcompletestring + Format(outLatitude, "##0.00") + "," + outNorS + ",";
                //outcompletestring = outcompletestring + Format(outLongitude, "##0.00") + "," + outEorW + ",";
                outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ",";
                outcompletestring = outcompletestring + youtputstr + "," + outEorW + ",";
                outcompletestring = outcompletestring + outGPSFix + ",";
                outcompletestring = outcompletestring + outNoofSatellites + ",";
                outcompletestring = outcompletestring + outPrecision + ",";
                outcompletestring = outcompletestring + outAltitude + ".0,M,,,,";
                outcompletestring = outcompletestring + outEndOfString;

                this.label2.Text = outcompletestring + getChecksum(outcompletestring);
                this.textBox2.Text = outcompletestring + getChecksum(outcompletestring);
                this.serialPort1.WriteLine(outcompletestring + getChecksum(outcompletestring) + "\r\n");

                // Out Satellite status
                outcompletestring = "$GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*";
                this.label3.Text = outcompletestring + getChecksum(outcompletestring);
                this.textBox3.Text = outcompletestring + getChecksum(outcompletestring);
                this.serialPort1.WriteLine(outcompletestring + getChecksum(outcompletestring) + "\r\n");

                //Console.WriteLine( getChecksum("$GPRMC,115447,A,0000.00,N,00000.00,E,40.000,0.000,220114,5.000,W*");
                //this.serialPort1.WriteLine("$GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*1D");
                //this.serialPort1.WriteLine("$GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E");
                // $GPRMC,115447,A,0000.00,N,00000.00,E,40.000,0.000,220114,5.000,W*53"
                // $GPGGA,053302.00,4725.8904,N,12218.4798,W,1,05,0.0,131.9,M,,,,*2E
                // $GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*1D

                //this.serialPort1.Close();
                //$GPRMC,115447,A,0000.00,N,00000.00,E,40.000,0.000,220114,5.000,W*53
            }

            catch (Exception caughterror)
            {
                
                //MessageBox.Show(caughterror.Message.ToString());
                this.lblProgStatus.Text =  DateTime.Now.ToLongTimeString() + " " + caughterror.Message.ToString();
                Console.WriteLine(caughterror.ToString());
            }


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

        private void timer1_Tick(object sender, EventArgs e)
        {
            button1_Click(sender, e);  
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.timer1.Enabled = !this.timer1.Enabled;
            if (this.timer1.Enabled == true)
                this.button2.Text = "Disable timer";
            else
                this.button2.Text = "Enable timer";
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        /********************************************************
         * Close all sockets and streams on the capture of a    *
         * Control-C.                                           *
         ********************************************************/
        public static void cleanUp()
        {
            Console.WriteLine("Closing Connections!");
            newsock.Close();
        }

        public static void Receive_DCS_UDP()
        {
            // Create the UDP server socket to communicate with Export.LUA
            newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Loopback, 7790);
            IPEndPoint sender = new IPEndPoint(IPAddress.Loopback, 0);

            newsock.Bind(ipep);
            Remote = (EndPoint)(sender);


            // String to store the response ASCII representation.
            String responseData = String.Empty;

            //Creates a UdpClient for reading incoming data.

            UdpClient receivingUdpClient = new UdpClient(7782);

            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);

            // Read the first batch of the TcpServer response bytes.
            while (true)
            {
                try
                {

                    // Blocks until a message returns on this socket from a remote host.
                    Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                    string returnData = Encoding.ASCII.GetString(receiveBytes);

                    if (true)
                    {
                        Console.WriteLine("Payload received from DCS " +
                                                    returnData.ToString());
                        Console.WriteLine("This message was sent from " +
                                                    RemoteIpEndPoint.Address.ToString() +
                                                    " on their port number " +
                                                    RemoteIpEndPoint.Port.ToString());
                    }

                    if (true)
                    // Don't sent anything to SOIC Server if nosend on command line
                    {
                        // MessageBox.Show(returnData.ToString());
                        ThreadStatus = returnData.ToString();
                        ParseUDPString(returnData.ToString());
                    }

                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }

            }

        }


        public static void ParseUDPString(string ReceivedString)
        {
            char[] delimiterChars = { ':' };
            char[] innerDelimiter = { '=' };
            string[] innerWords;
            bool firstValue;
            string valueBeingUpdated = "None";

            string[] words = ReceivedString.Split(delimiterChars);
            // MessageBox.Show ( words.Length + " words in text:");
            foreach (string s in words)
            {
                // MessageBox.Show(s);
                innerWords = s.Split(innerDelimiter);
                firstValue = true;
                foreach (string myvalues in innerWords)
                {
                   
                    // There should be two values returned in the inner loop
                    
                   //  MessageBox.Show(myvalues);
                    if (firstValue)
                    {

                        switch (myvalues)
                        {
                            case "Altitude":
                                {
                                    valueBeingUpdated = "Altitude";
                                    break;
                                }
                            case "Heading":
                                {
                                    valueBeingUpdated = "Heading";
                                    break;
                                }
                            case "Latitude":
                                {
                                    valueBeingUpdated = "Latitude";
                                    break;
                                }
                            case "Longitude":
                                {
                                    valueBeingUpdated = "Longitude";
                                    break;
                                }
                            case "Airspeed":
                                {
                                    valueBeingUpdated = "Airspeed";
                                    break;
                                }
                            default:
                                {
                                    valueBeingUpdated = "None";
                                    break;
                                }


                        }
                        firstValue = false;
                    }
                    else
                        // Dealing with the value being returned
                        
                    {

                        
                        switch (valueBeingUpdated)
                        {

                            case "Airspeed":
                                {

                                    outSpeed = myvalues;
                                    break;

                                }
                            
                            case "Altitude":
                                {

                                    outAltitude = myvalues;
                                    break;

                                }
                            case "Heading":
                                {

                                    outTrackMadeGood = myvalues;
                                    break;

                                }

                            case "Longitude":
                                {

                                    outLongitude = myvalues;
                                    break;

                                }

                            case "Latitude":
                                {

                                    outLatitude = myvalues;
                                    break;

                                }



                            default:
                                {
                                    break;
                                }

                        }
                        valueBeingUpdated = "None";
                    }
                }
            }

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (lastNetworkStatus  != ThreadStatus )
            {
                lblNetworkingThreadStatus.Text = DateTime.Now.ToLongTimeString() + ":" +  ThreadStatus;
                lastNetworkStatus  = ThreadStatus; 
            }
        }



    }
}
