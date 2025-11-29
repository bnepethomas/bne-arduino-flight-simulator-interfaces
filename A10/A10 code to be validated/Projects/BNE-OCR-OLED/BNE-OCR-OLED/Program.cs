using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;
using System.Net;

namespace BNE_OCR_OLED
{

    class Program
    {

        static Socket newsock;
        static EndPoint Remote;
        static EndPoint CDURemote;


        static byte[] NULL = new byte[1] { 0x00 };
        static byte[] STX = new byte[1] { 0x02 };
        static byte[] ETX = new byte[1] { 0x03 };
        static byte[] CR = new byte[1] { 0x0d };


        static byte[] oled1 = new byte[1] { 0x01 };
        static byte[] oled2 = new byte[1] { 0x02 };
        static byte[] oled3 = new byte[1] { 0x03 };
        static byte[] oled4 = new byte[1] { 0x04 };
        static byte[] oled5 = new byte[1] { 0x05 };
        static byte[] oled6 = new byte[1] { 0x06 };
        static byte[] oled7 = new byte[1] { 0x07 };

        static byte[] CLS = new byte[1] { 0x7F };

        // Note that 0x02 and 0x03 are reserved for STX and ETX
        static byte[] FirstLine = new byte[1] { 0x01 };
        static byte[] SecondLine = new byte[1] { 0x02 };



        static bool bDebugFoundInCommandLine = false;
        static bool bShowTimeOnlyInCommandLine = false;


        static void Main(string[] args)
        {


            var sha1 = System.Security.Cryptography.SHA1.Create();
            byte[] buf;
            byte[] hash;
            string lastHash = "";
            string[] words;

            string lastILS = "";
            string lastChaff = "";
            string lastFlare = "";
            string lastTACAN = "";
            string lastVHF_FM = "";
            string lastVHF_AM = "";


            string lastReturnData = "";
            Console.WriteLine("Starting OCR to OLED");

            //bool bDebugFoundInCommandLine=false;
            string sWorkstr;
            foreach (string s in args)
            {
                sWorkstr = s.ToLower();
                Console.WriteLine("Command-Line: " + sWorkstr);
                if (sWorkstr.Contains("debug"))
                {
                    bDebugFoundInCommandLine = true;
                }

                if (sWorkstr.Contains("time"))
                {
                    bShowTimeOnlyInCommandLine = true;
                }

            }
            if (bDebugFoundInCommandLine)
                Console.WriteLine("Debug requested, displayed received data");

            if (bShowTimeOnlyInCommandLine)
                Console.WriteLine("Time passed via Command Line - only updating clocks");


            // Setup call back function to clean up network ports on a CTRL-C
            Console.CancelKeyPress += delegate
            {
                cleanUp();
            };


            newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

            IPEndPoint CDUSender = new IPEndPoint(IPAddress.Loopback, 7781);
            CDURemote = (EndPoint)(CDUSender);

            bool getakey = false;

            try {

                //Creates a UdpClient for reading incoming data.
                //Use the default port used by OverPro
                UdpClient receivingUdpClient = new UdpClient(33051);
                Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);





                while (bShowTimeOnlyInCommandLine)
                {
                    int iTimeToWait = 100;
                    Thread.Sleep(80);



                };




                sWorkstr = "";

                while (true)
                {
                    try
                    {

                        // Blocks until a message returns on this socket from a remote host.
                        Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                        string returnData = Encoding.ASCII.GetString(receiveBytes);

                        if (bDebugFoundInCommandLine)
                        {
                            Console.WriteLine("Payload received from DCS " +
                                                        returnData.ToString());
                            Console.WriteLine("This message was sent from " +
                                                        RemoteIpEndPoint.Address.ToString() +
                                                        " on their port number " +
                                                        RemoteIpEndPoint.Port.ToString());
                        }

                        // 11 CSMP
                        // 12 CSMC
                        //if (receiveBytes[0] == 12)
                        if (returnData != lastReturnData)
                            ForwardToArduinoSender(returnData);
                        lastReturnData = returnData; 

                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                Console.ReadKey(getakey);
            };
            
        }



        public static void ForwardToArduinoSender(string stringtosend)
        {

            int recv;
            byte[] data = new byte[1024];
            byte[] data2 = new byte[1024];

            // Send the button press command to Export.lua
            data = Encoding.ASCII.GetBytes("9999=1;" + stringtosend);
            newsock.SendTo(data, data.Length, SocketFlags.None, CDURemote);

            Console.WriteLine("Sent to Arduino Sender - " + stringtosend);

        }



        public static string Get_Value(string[] DCSData, int ValueToReturn)
        {
            
            int TestPos = -1;
            string wrkstring;
            for (int i = 0; i < DCSData.Length; i++)
            {
                TestPos = DCSData[i].IndexOf(ValueToReturn.ToString());
                if ( TestPos != -1) 
                {
                    wrkstring = DCSData[i].Substring(ValueToReturn.ToString().Length + 1);
                    Console.WriteLine(wrkstring );
                    if (wrkstring.Length == 0)
                        wrkstring = " ";
                    return wrkstring;

                }
            }
            // If value not found return a space, better than a crash
            return " ";

        }

        public static void cleanUp()
        {
            Console.WriteLine("Closing Connections!");
            newsock.Close();
        }
    }
}
