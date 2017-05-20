using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO.Ports;
using System.Net;
using System.Threading;

namespace CDU_Test2
{
    static class Program
    {
        static TcpClient client;
        static NetworkStream stream;
        static Socket newsock;
        static EndPoint Remote;


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


        /********************************************************************************
         *                                   Functions                                  *       
         ********************************************************************************/

        static void Main(string[] args)
        {
            // Setup call back function to clean up network ports on a CTRL-C
            Console.CancelKeyPress += delegate
            {
                cleanUp();
            };

            // Create the UDP server socket to communicate with Export.LUA
            newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Loopback, 7777);
            IPEndPoint sender = new IPEndPoint(IPAddress.Loopback, 0);
            newsock.Bind(ipep);
            Remote = (EndPoint)(sender);

            // Connect to the SOIC server to get input from CDU
            Connect("127.0.0.1", "Arn.Resp:2=0\n");
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

                // PTs destruction
                message = "Arn.Resp:2=1:509=1:525=1\n";
                data = System.Text.Encoding.ASCII.GetBytes(message);

                // Send the SOIC initialisation to the connected SIOC TCP server
                stream = client.GetStream();
                stream.Write(data, 0, data.Length);

                // end of PTs destruction
                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                while (true)
                {
                    //Int32 bytes = stream.Read(data, 0, data.Length);
                    //responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                    //if (responseData.Contains("Arn.Resp") && !(responseData.Contains("1=0"))) { processCDUInput(responseData); }
                    Thread.Sleep(1000);

                    // PTs destruction
                    message = "Arn.Resp:480=0:481=0:482=0:483=0:484=0:485=0:486=0:487=0:509=0:525=0:662=0:663=0:1662=0\n";
                    data = System.Text.Encoding.ASCII.GetBytes(message);

                    // Send the SOIC initialisation to the connected SIOC TCP server
                    stream = client.GetStream();
                    stream.Write(data, 0, data.Length);

                    Thread.Sleep(1000);
                    message = "Arn.Resp:480=1:481=1:482=1:483=1:484=1:485=1:486=1:487=1:509=1:525=1:662=1:663=1:1662=1\n";
                    data = System.Text.Encoding.ASCII.GetBytes(message);

                    // Send the SOIC initialisation to the connected SIOC TCP server
                    stream = client.GetStream();
                    stream.Write(data, 0, data.Length);

                    // end of PTs destruction

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
                        sendToLUA("C9", dicCDU[splitSOICInput2[1]]);
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

            // Send the button release command to Export.lua
            data = Encoding.ASCII.GetBytes(luaDevice + "," + luaButton + ",0.0");
            newsock.SendTo(data, data.Length, SocketFlags.None, Remote);
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

