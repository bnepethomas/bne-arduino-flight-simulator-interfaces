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
        static bool bDebugFoundInCommandLine = false;


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
            IPEndPoint ipep = new IPEndPoint(IPAddress.Loopback, 7778);
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
                        message = "Arn.Resp:" + returnData.ToString() + "\n";
                        data = System.Text.Encoding.ASCII.GetBytes(message);

                        // Send the SOIC initialisation to the connected SIOC TCP server
                        stream = client.GetStream();
                        stream.Write(data, 0, data.Length);

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

