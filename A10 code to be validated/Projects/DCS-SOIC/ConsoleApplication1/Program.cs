using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO.Ports;
using System.Net;
using System.Threading;

namespace Soic_Conv_Output
{
    static class Program
    {
        static TcpClient client;
        static NetworkStream stream;
        static Socket newsock;
        static EndPoint Remote;
        static EndPoint CDURemote;
        
        static bool bDebugFoundInCommandLine = false;
        static bool bNoSendFoundInCommandLine = false;




        /********************************************************************************
         *                            Device Lookup Tables                              *
         ********************************************************************************/

        /*****    CDU    *****/

        /* Because a single button is being used for both + and -, it is necessary
         * to track state of the button. Each press of the button will change the value
         * of this boolean */
        static bool isPlus = true;

        // CDU Button Lookup Table
        static Dictionary<string, string> dicCDU = new Dictionary<string, string>
        {
            // Left Arrow Buttons
            {"64","3001"},{"72","3002"},{"80","3003"},{"88","3004"},
            // Right Arrow Buttons
            {"85","3005"},{"84","3006"},{"83","3007"},{"82","3008"},
            // CDU Page Buttons (e.g. SYS, NAV, WP)
            {"71","3009"},{"70","3010"},{"69","3011"},{"68","3012"},{"67","3013"},{"66","3014"},
            // Numbers 1-9, 0 and Decimal Point
            {"32","3015"},{"31","3016"},{"30","3017"},{"24","3018"},{"23","3019"},{"22","3020"},{"16","3021"},{"15","3022"},{"14","3023"},{"7","3024"},{"8","3025"},{"2","3026"},
            // Letters A-Z
            {"61","3027"},{"60","3028"},{"59","3029"},{"58","3030"},{"57","3031"},
            {"53","3032"},{"52","3033"},{"51","3034"},{"50","3035"},{"49","3036"},
            {"29","3037"},{"28","3038"},{"27","3039"},{"26","3040"},{"25","3041"},
            {"21","3042"},{"20","3043"},{"19","3044"},{"18","3045"},{"17","3046"},
            {"13","3047"},{"12","3048"},{"11","3049"},{"10","3050"},{"9","3051"},
            {"5","3052"},
            // Special Characters
            {"79","3055"}, // Mark Point
            {"3","3056"}, // Back
            {"4","3057"}, // Space
            {"1","3058"}, // Clear
            {"62","3059"},  // Fault Acknowledge
            {"55","3062"},  // Page Up
            {"54","3063"},  // Page Down
            {"76","3064"},  // Scroll WP Up
            {"75","3065"},  // Scroll WP Down
            {"6","3066"}  // Plus and Minus along with the Boolean

        };

        static void Main(string[] args)
        {

            Console.WriteLine("Starting SOIC_Conv_Output");

            //bool bDebugFoundInCommandLine=false;
            string sWorkstr;
            foreach (string s in args)
            {
                sWorkstr = s.ToLower();
                Console.WriteLine(sWorkstr);
                if (sWorkstr.Contains("debug"))
                {
                    bDebugFoundInCommandLine=true;
                }

                if (sWorkstr.Contains("nosend"))
                {
                    bNoSendFoundInCommandLine = true;
                }
            }
            if (bDebugFoundInCommandLine)
                Console.WriteLine("Debug requested, displayed received data");

            
            // Setup call back function to clean up network ports on a CTRL-C
            Console.CancelKeyPress += delegate
            {
                cleanUp();
            };

            // Create the UDP server socket to communicate with Export.LUA
            newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Loopback, 7779);
            IPEndPoint sender = new IPEndPoint(IPAddress.Loopback, 0);
            IPEndPoint CDUSender = new IPEndPoint(IPAddress.Loopback, 7780);
            newsock.Bind(ipep);
            Remote = (EndPoint)(sender);
            CDURemote = (EndPoint)(CDUSender);

            // Connect to the SOIC server to get input from CDU
            Thread ConnectCDUThread = new Thread(new ThreadStart(ConCDU));
            ConnectCDUThread.Start();
            Connect("127.0.0.1", "Arn.Resp:2=0\n");


            // Copied from CDU only code
            //Console.WriteLine("Connecting to SOIC");
            //Connect("127.0.0.1", "Arn.Inicio:1\n");
        }

        public static void Connect(String server, String message)
        {
            try
            {
                // Create a TCP client. 
                Int32 port = 8092;
                client = new TcpClient(server, port);

                // Convert the SOIC initialisation string to a Byte array
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

                // Send the SOIC initialisation to the connected SIOC TCP server
                stream = client.GetStream();
                stream.Write(data, 0, data.Length);


                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                //Creates a UdpClient for reading incoming data.
                UdpClient receivingUdpClient = new UdpClient(7777);

                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);

                // Read the first batch of the TcpServer response bytes.
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

                        if (!bNoSendFoundInCommandLine)
                        // Don't sent anything to SOIC Server if nosend on command line
                        {
                            message = "Arn.Resp:" + returnData.ToString() + "\n";
                            data = System.Text.Encoding.ASCII.GetBytes(message);

                            // Send the SOIC initialisation to the connected SIOC TCP server
                            stream = client.GetStream();
                            stream.Write(data, 0, data.Length);

                            // being new
                            //stream.ReadTimeout = 1;
                            //Int32 bytes = stream.Read(data, 0, data.Length);
                            //Console.WriteLine("Have received bytes");
                            //responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                            //Console.WriteLine("Rx Data - " + responseData);
                            //if (responseData.Contains("Arn.Resp") && !(responseData.Contains("1=0"))) { processCDUInput(responseData); }
                        }

                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.ToString());
                    }

                }
            }
            catch (ArgumentNullException e)
            {
                Console.WriteLine("ArgumentNullException: {0}", e);
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }
        }


        public static void ConCDU()
        {
            ConnectCDU("127.0.0.1", "Arn.Inicio:1\n");
        }   
        public static void ConnectCDU(String server, String message)
        {
            try
            {
                // Create a TCP client. 
                Int32 port = 8092;
                client = new TcpClient(server, port);

                // Convert the SOIC initialisation string to a Byte array
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

                // Send the SOIC initialisation to the connected SIOC TCP server
                stream = client.GetStream();
                stream.Write(data, 0, data.Length);
                Console.WriteLine("Connected to SOIC - Sent init string");
                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                while (true)
                {
                    Int32 bytes = stream.Read(data, 0, data.Length);
                    responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                    Console.WriteLine("Rx Data - " + responseData);
                    if (responseData.Contains("Arn.Resp") && !(responseData.Contains("1=0"))) { processCDUInput(responseData); }
                }

            }
            catch (ArgumentNullException e)
            {
                Console.WriteLine("ArgumentNullException: {0}", e);
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }
        }


        /********************************************************
         * Take the CDU input from the SOIC server, parse it    *
         * and use the CDU lookup table to identify the correct *
         * DCS button ID and send to Export.lua                 *
         ********************************************************/
        public static void processCDUInput(String soicInput)
        {
            String[] splitSOICInput = soicInput.Split(':');
            if (splitSOICInput.Length > 1)
            {
                String[] splitSOICInput2 = splitSOICInput[1].Split('=');
                if (splitSOICInput2.Length > 1)
                {
                    // Check the CDU button press from SOIC is included in the CDU lookup table before sending to Export.lua
                    if (dicCDU.ContainsKey(splitSOICInput2[1]))
                    {
                        // Send the Device and Button to Export.lua
                        cdusendToLUA("C9", dicCDU[splitSOICInput2[1]]);
                    }
                }
            }
        }


        /********************************************************
         * Take a Device ID and DCS button ID and create the    *
         * command format required by Export.lua. Send the      *
         * command to Export.lua over UDP socket                *
         ********************************************************/
        public static void sendToLUA(String luaDevice, String luaButton)
        {
            int recv;
            byte[] data = new byte[1024];
            byte[] data2 = new byte[1024];

            // Send the button press command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + ",1.0");
            recv = newsock.ReceiveFrom(data2, ref Remote);
            newsock.SendTo(data, data.Length, SocketFlags.None, Remote);

            Console.WriteLine("Sent to LUA - " + luaDevice + "," + luaButton + ",1.0");

            // Send the button release command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + ",0.0");
            newsock.SendTo(data, data.Length, SocketFlags.None, Remote );

            Console.WriteLine("Sent to LUA - " + luaDevice + "," + luaButton + ",0.0");
        }

        public static void cdusendToLUA(String luaDevice, String luaButton)
        {
            int recv;
            byte[] data = new byte[1024];
            byte[] data2 = new byte[1024];

            // Send the button press command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + ",1.0");
            //recv = newsock.ReceiveFrom(data2, ref Remote);
            newsock.SendTo(data, data.Length, SocketFlags.None, CDURemote);

            Console.WriteLine("Sent to LUA - " + luaDevice + "," + luaButton + ",1.0");

            // Send the button release command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + ",0.0");
            newsock.SendTo(data, data.Length, SocketFlags.None, CDURemote);

            Console.WriteLine("Sent to LUA - " + luaDevice + "," + luaButton + ",0.0");
        }

        /********************************************************
         * Close all sockets and streams on the capture of a    *
         * Control-C.                                           *
         ********************************************************/
        public static void cleanUp()
        {
            Console.WriteLine("Closing Connections!");
            stream.Close();
            client.Close();
            newsock.Close();
        }
    }
}

