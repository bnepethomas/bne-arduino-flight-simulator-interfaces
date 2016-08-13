using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
//
using System.Net;
using System.Net.Sockets;


//      Connects to Local Instance of Promsim TCP 8091 (give option to specificy it as remote)
//      Both sends and receives traffic to remote Arduino over UDP. Using UDP reduces Arduino load.
//
//      -----------------------------------------------------------------------------------------------
//      Will split into two programs then don't have to worry about threads
//
//      One listens to TCP data from Prosim, filters and sends to Arduino (UDP 9901)
//
//      The other connects to Prosim, ignores all data from Prosim but listens to Arduino (UDP 9902)
//      -----------------------------------------------------------------------------------------------
//
//
//      Values of Interest from Prosim
//      FLT_ALT
//      LAND_ALT
//      APU_STOPPING                    (May map this to a single vlaue APU_SOLENOID)
//      APU_START                       (May map this to a single vlaue APU_SOLENOID)
//      ENGINE_1_PUSHBACK_SOLENOID     (Should fire at N2 56%) 
//      ENGINE_2_PUSHBACK_SOLENOID     (Should fire at N2 56%) 
//
//
//      Values Received from Arduino
//      FLT_ALT_INCREMENT               Sends IN_FLT_ALT=1
//      FLT_ALT_DECREMENT               Sends IN_FLT_ALT=-1
//      LAND_ALT_INCREMENT              Send IN_LAND_ALT=1
//      LAND_ALT_DECREMENT              Send IN_LAND_ALT=-1



//      20160502       PT - Creation


//      Considerations
//      Handle TCP disconnection timeout and disconnections - on disconnect ensure displays and solenoinds are switched off    
//      Make simple UDP listener for testing, perhaps command line option for local test


namespace ProSim_UDP_Receiver
{
    class Program
    {

        public static UdpClient receivingUdpClient = new UdpClient(9902);
        public static IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
        public static string returnData;

        static void Main(string[] args)
        {
            //      Listen for data on UDP 9901, if received pass through to ProSim on TCP
            //      Make UDP Sender for testing - use forms
            Console.WriteLine("ProSim_UDP_Receiver");
            while (true)
            {
                //Creates a UdpClient for reading incoming data.


                //Creates an IPEndPoint to record the IP Address and port number of the sender. 
                // The IPEndPoint will allow you to read datagrams sent from any source.


                Console.WriteLine("Waiting for UdpClient Data");
                Receive();
                Connect("127.0.0.1", returnData);
            }
        }

        static void Receive()
        {


            try
            {

                // Blocks until a message returns on this socket from a remote host.
                Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                returnData = Encoding.ASCII.GetString(receiveBytes);
                Console.WriteLine(DateTime.Now);
                Console.WriteLine("This is the message you received " +
                                            returnData.ToString());
                Console.WriteLine("This message was sent from " +
                                            RemoteIpEndPoint.Address.ToString() +
                                            " on their port number " +
                                            RemoteIpEndPoint.Port.ToString());
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }


        static void Connect(String server, String message)
        {
            try
            {
                // Create a TcpClient.
                // Note, for this client to work you need to have a TcpServer 
                // connected to the same address as specified by the server, port
                // combination.
                Int32 port = 8091;
                TcpClient client = new TcpClient(server, port);

                // Translate the passed message into ASCII and store it as a Byte array.
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

                // Get a client stream for reading and writing.
                //  Stream stream = client.GetStream();

                NetworkStream stream = client.GetStream();

                // Send the message to the connected TcpServer. 
                stream.Write(data, 0, data.Length);

                Console.WriteLine("Sent: {0}", message);

                // Receive the TcpServer.response.

                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                Int32 bytes = stream.Read(data, 0, data.Length);
                responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                Console.WriteLine("Received: {0}", responseData);

                // Close everything.
                stream.Close();
                client.Close();
            }
            catch (ArgumentNullException e)
            {
                Console.WriteLine("ArgumentNullException: {0}", e);
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }

            // Console.WriteLine("\n Press Enter to continue...");
            // Console.Read();
        }
    }


}
