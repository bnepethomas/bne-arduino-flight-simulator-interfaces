using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;
using System.Net;


namespace bnecmspconvertor
{
    class Program
    {
 

        static Socket newsock;


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
            string lastCMSP_Top = "";
            string lastCMSP_Bottom = "";
            string lastCMSP = "";
            string lastCMSC = "";

            Console.WriteLine("Starting Arduino CLI Sender");

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





            try
            {

                //Creates a UdpClient for reading incoming data.
                UdpClient receivingUdpClient = new UdpClient(7787);
                Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);





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

                        // Basic test looking for differences - quick checksum - if different then split
                        buf = System.Text.Encoding.UTF8.GetBytes(returnData);
                        hash = sha1.ComputeHash(buf, 0, buf.Length);
                        sWorkstr = System.BitConverter.ToString(hash).Replace("-", "");
                        if (sWorkstr != lastHash)
                        {

                            lastHash = sWorkstr;

                            words = returnData.Split(new char[] { ':' }, 100, StringSplitOptions.RemoveEmptyEntries);
                            Console.WriteLine(words[0].ToString());

                            // CMSC/CMSP has a tag attrbitue of 9999
                            if (Get_Value(words,9999) == " ")
                            {

                                // VHF Radio

                                // VHF Radio values stored in values 157, 158, 159, 160
                                sWorkstr = "VHF AM " + Get_Value(words, 157) + Get_Value(words, 158) + Get_Value(words, 159) + "." + Get_Value(words, 160);
                                if (sWorkstr != lastVHF_AM)
                                {

                                    lastVHF_AM = sWorkstr;
                                    //SendToArduino(oled6, SecondLine, lastVHF_AM.PadRight(16));
                                    //Thread.Sleep(20);
                                }

                                // VHF 
                                sWorkstr = "VHF FM " + Get_Value(words, 143) + Get_Value(words, 144) + Get_Value(words, 145) + "." + Get_Value(words, 146);
                                if (sWorkstr != lastVHF_FM)
                                {

                                    lastVHF_FM = sWorkstr;
                                    //SendToArduino(oled6, FirstLine, lastVHF_FM.PadRight(16));
                                    //Thread.Sleep(20);
                                }

                                // ILS
                                sWorkstr = "ILS " + Get_Value(words, 8010) + "." + Get_Value(words, 8011);
                                if (sWorkstr != lastILS)
                                {
                                    //sWorkstr = Get_Value(words, 157) + Get_Value(words, 158) + Get_Value(words, 159) + Get_Value(words, 160);
                                    lastILS = sWorkstr;
                                    //SendToArduino(oled4, FirstLine, lastILS.PadRight(16));
                                    //Thread.Sleep(250);
                                }

                                // TACAN
                                // Deal with some weirdness on the major channel value
                                if (Get_Value(words, 8020) == "1")
                                    sWorkstr = "1";
                                else
                                    sWorkstr = " ";

                                sWorkstr = "TACAN " + sWorkstr + Get_Value(words, 8021)+ Get_Value(words,8022) ;
                                if (Get_Value(words, 8023) == "0")
                                    sWorkstr = sWorkstr + "X";
                                else
                                    sWorkstr = sWorkstr + "Y";
                                if (sWorkstr != lastTACAN)
                                {
                                    //sWorkstr = Get_Value(words, 157) + Get_Value(words, 158) + Get_Value(words, 159) + Get_Value(words, 160);
                                    lastTACAN = sWorkstr;
                                    //SendToArduino(oled3, FirstLine, lastTACAN.PadRight(16));
                                    //Thread.Sleep(100);
                                }


                                //// Chaff Flare
                                //sWorkstr = "Chaff " + Get_Value(words, 8030);
                                //if (sWorkstr != lastChaff)
                                //{
                                //    lastChaff = sWorkstr;
                                //    SendToArduino(oled5, FirstLine, lastChaff.PadRight(16));
                                //    Thread.Sleep(20);
                                //}

                            
                                //sWorkstr = "Flare " + Get_Value(words, 8031);
                                //if (sWorkstr != lastFlare)
                                //{
                                //    lastFlare = sWorkstr;
                                //    SendToArduino(oled5, SecondLine, lastFlare.PadRight(16));
                                //    Thread.Sleep(20);
                                //}
                            }
                            else
                            {

                               // CMSP
                                if (returnData.IndexOf("CMSP") > 0 )
                                {

                                    if (returnData != lastCMSP)
                                    {
                                        //ClearArduinoDisplay(oled7);
                                        //SendToArduino(oled7, FirstLine,"first");
                                        //SendToArduino(oled7, SecondLine, "second");
                                        //SendToArduino(oled7, FirstLine, returnData.Substring(12, 18)
                                        //   + "                      "
                                         //  + returnData.Substring(32));
                                        //SendToArduino(oled7, SecondLine, "hi");
                                        //SendToArduino(oled7, FirstLine, returnData.Substring(12, 18));
                                        //Thread.Sleep(50);
                                        //SendToArduino(oled7, SecondLine, returnData.Substring(32));
                                        //Thread.Sleep(50);
                                        lastCMSP = returnData;
                                    }

                                }
                                else if (returnData.IndexOf("CMSC") > 0)
                                {
                                    //CMSC
                                    if (returnData != lastCMSC)
                                    {

                                        //ClearArduinoDisplay(oled5);
                                        //    //SendToArduino(oled7, FirstLine,"first");
                                        //    //SendToArduino(oled7, SecondLine, "second");
                                        //SendToArduino(oled5, FirstLine, returnData.Substring(12, 16));
                                        //Thread.Sleep(50);
                                        //SendToArduino(oled5, SecondLine, returnData.Substring(28));
                                        //Thread.Sleep(50);
                                        lastCMSC = returnData;
                                    }
                                }
                                //SendToArduino(oled7, SecondLine, returnData.Substring(31, 10));

                                //sWorkstr = returnData.Substring(12, 16);
                                //if (sWorkstr.Length > 16)
                                //    sWorkstr = sWorkstr.Substring(2, 16);
                                //if (sWorkstr != lastCMSP_Top)
                                //{
                                //    ClearArduinoDisplay(oled7);
                                //    lastCMSP_Top = sWorkstr;
                                //    SendToArduino(oled7, SecondLine, lastCMSP_Top.PadRight(16));
                                //    Thread.Sleep(50);
                                //}

                                //sWorkstr = " " + Get_Value(words, 8041);
                                //if (sWorkstr.Length > 16)
                                //    sWorkstr = sWorkstr.Substring(2, 16);
                                //if (sWorkstr != lastCMSP_Bottom)
                                //{
                                //    lastCMSP_Bottom = sWorkstr;
                                //    SendToArduino(oled7, SecondLine, lastCMSP_Bottom.PadRight(16));
                                //    Thread.Sleep(20);
                                //}
                            }

                        }



                        

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
                //Console.ReadKey(getakey);
            };
            
        }

        // Display Port values and prompt user to enter a port. 
        static string SetPortName(string defaultPortName)
        {
            string portName;

            Console.WriteLine("Available Ports:");
            foreach (string s in SerialPort.GetPortNames())
            {
                Console.WriteLine("   {0}", s);
            }

            Console.Write("Enter COM port value (Default: {0}): ", defaultPortName);
            portName = Console.ReadLine();

            if (portName == "" || !(portName.ToLower()).StartsWith("com"))
            {
                portName = defaultPortName;
            }
            return portName;
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
