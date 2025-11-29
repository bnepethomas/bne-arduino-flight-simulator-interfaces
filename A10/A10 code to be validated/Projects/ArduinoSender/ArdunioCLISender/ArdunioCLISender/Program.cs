using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;
using System.Net;


namespace ArdunioCLISender
{
    class Program
    {
        static SerialPort _serialPort;

        static Socket newsock;

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
            string lastCMSP_Top = "";
            string lastCMSP_Bottom = "";
            string lastCMSP = "";
            string lastCMSC = "";
            string lastCMSC_Top = "";
            string lastCMSC_Bottom = "";

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


            // Create a new SerialPort object with default settings.
            _serialPort = new SerialPort();

            // Allow the user to set the appropriate properties.
            _serialPort.PortName = "COM4";
            _serialPort.BaudRate = 115200;


            bool getakey = false;
            try 
            {
                if (!_serialPort.IsOpen)
                    _serialPort.Open();
            }
            // Lazy here, catch the COM Port not being available
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                Console.ReadKey(getakey);
                _serialPort.PortName = SetPortName("COM4");
            };

            try
            {

                //Creates a UdpClient for reading incoming data.
                UdpClient receivingUdpClient = new UdpClient(7781);
                Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);


                // Couple of STX to sync up 
                _serialPort.Write(NULL, 0, 1);
                _serialPort.Write(NULL, 0, 1);


                ClearArduinoDisplay(oled1);
                ClearArduinoDisplay(oled2);
                ClearArduinoDisplay(oled3);
                ClearArduinoDisplay(oled4); 
                ClearArduinoDisplay(oled5);
                ClearArduinoDisplay(oled6);
                ClearArduinoDisplay(oled7);


                while (bShowTimeOnlyInCommandLine)
                {
                    int iTimeToWait = 100;
                    Thread.Sleep(80);
                    if (_serialPort.IsOpen)
                    {

                        SendToArduino(oled1, SecondLine, "1 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled2, SecondLine, "2 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled3, SecondLine, "3 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled4, SecondLine, "4 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled5, SecondLine, "5 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled6, SecondLine, "6 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                        SendToArduino(oled7, SecondLine, "7 - " + DateTime.Now.ToLongTimeString() + "  ");
                        Thread.Sleep(iTimeToWait);
                    }

                };



                SendToArduino(oled1, FirstLine, "1 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled2, FirstLine, "2 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled3, FirstLine, "3 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled4, FirstLine, "4 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled5, FirstLine, "5 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled6, FirstLine, "6 Waiting for DCS");
                Thread.Sleep(50);
                SendToArduino(oled7, FirstLine, "7 Waiting for DCS");
                Thread.Sleep(500);
                Thread.Sleep(50);
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

                                // 20141105
                                // Need to check that blanks are being sent as well

                                // CMSP
                                sWorkstr = Get_Value(words, 8033);
                                if (sWorkstr != lastCMSP_Top)
                                {
                                    SendToArduino(oled7, FirstLine, sWorkstr);
                                    Thread.Sleep(50);
                                    lastCMSP_Top = sWorkstr;
                                }

                                sWorkstr = Get_Value(words, 8034);
                                if (sWorkstr != lastCMSP_Bottom)
                                {
                                    SendToArduino(oled7, SecondLine , sWorkstr);
                                    Thread.Sleep(50);
                                    lastCMSP_Bottom = sWorkstr;
                                }


                                sWorkstr = Get_Value(words, 8036).PadRight(12) + Get_Value(words, 8035).Substring(4, 4); ;
                                if (sWorkstr != lastCMSC_Bottom)
                                {
                                    SendToArduino(oled5, SecondLine , sWorkstr);
                                    Thread.Sleep(50);
                                    lastCMSC_Bottom = sWorkstr;
                                }

                                sWorkstr = Get_Value(words, 8037).PadRight(12) + Get_Value(words,8035).Substring(0,4);
                                if (sWorkstr != lastCMSC_Top)
                                {
                                    SendToArduino(oled5, FirstLine, sWorkstr);
                                    Thread.Sleep(50);
                                    lastCMSC_Top = sWorkstr;
                                }

                                    //                                if (returnData != lastCMSC)
                                    //{

                                    //    //ClearArduinoDisplay(oled5);
                                    //    //    //SendToArduino(oled7, FirstLine,"first");
                                    //    //    //SendToArduino(oled7, SecondLine, "second");
                                    //    SendToArduino(oled5, FirstLine, returnData.Substring(12, 16));
                                    //    Thread.Sleep(50);
                                    //    SendToArduino(oled5, SecondLine, returnData.Substring(28));

                                // VHF Radio

                                // VHF Radio values stored in values 157, 158, 159, 160
                                sWorkstr = "VHF AM " + Get_Value(words, 157) + Get_Value(words, 158) + Get_Value(words, 159) + "." + Get_Value(words, 160);
                                if (sWorkstr != lastVHF_AM)
                                {

                                    lastVHF_AM = sWorkstr;
                                    SendToArduino(oled6, SecondLine, lastVHF_AM.PadRight(16));
                                    Thread.Sleep(20);
                                }

                                // VHF 
                                sWorkstr = "VHF FM " + Get_Value(words, 143) + Get_Value(words, 144) + Get_Value(words, 145) + "." + Get_Value(words, 146);
                                if (sWorkstr != lastVHF_FM)
                                {

                                    lastVHF_FM = sWorkstr;
                                    SendToArduino(oled6, FirstLine, lastVHF_FM.PadRight(16));
                                    Thread.Sleep(20);
                                }

                                // ILS
                                sWorkstr = "ILS " + Get_Value(words, 8010) + "." + Get_Value(words, 8011);
                                if (sWorkstr != lastILS)
                                {
                                    //sWorkstr = Get_Value(words, 157) + Get_Value(words, 158) + Get_Value(words, 159) + Get_Value(words, 160);
                                    lastILS = sWorkstr;
                                    SendToArduino(oled4, FirstLine, lastILS.PadRight(16));
                                    Thread.Sleep(250);
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
                                    SendToArduino(oled3, FirstLine, lastTACAN.PadRight(16));
                                    Thread.Sleep(100);
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
                                        SendToArduino(oled7, FirstLine, returnData.Substring(12, 18));
                                        Thread.Sleep(50);
                                        SendToArduino(oled7, SecondLine, returnData.Substring(32));
                                        Thread.Sleep(50);
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
                                        SendToArduino(oled5, FirstLine, returnData.Substring(12, 16));
                                        Thread.Sleep(50);
                                        SendToArduino(oled5, SecondLine, returnData.Substring(28));
                                        Thread.Sleep(50);
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
                Console.ReadKey(getakey);
            };
            
        }


        static void SendToArduino(byte[] oledPtr, byte[] lineNumber, string StrToSend)
        {
            if (_serialPort.IsOpen)
            {
                _serialPort.Write(STX, 0, 1);
                _serialPort.Write(oledPtr, 0, 1);
                _serialPort.Write(lineNumber, 0, 1);
                _serialPort.Write(StrToSend);
                _serialPort.Write(ETX, 0, 1);
            }
        }

        static void ClearArduinoDisplay(byte[] oledPtr)
        {
            if (_serialPort.IsOpen)
            {
                _serialPort.Write(STX, 0, 1);
                _serialPort.Write(oledPtr, 0, 1);
                _serialPort.Write(FirstLine, 0, 1);
                _serialPort.Write(CLS,0,1);
                _serialPort.Write(ETX, 0, 1);
            }
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
