using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
//
using System.Net;
using System.Net.Sockets;
//
using System.Threading;
using System.IO;


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


namespace ProSimSplitter
{
    class Program
    {

        public static UdpClient SendingUdpClient = new UdpClient(9910);

        public static IPAddress serverAddr = IPAddress.Parse("192.168.1.205");
        public static IPEndPoint RemoteIpEndPoint = new IPEndPoint(serverAddr, 9920);

        public static string returnData;
        
        static void Main(string[] args)
        {
            //      Listen for data on UDP 9901, if received pass through to ProSim on TCP
            //      Make UDP Sender for testing - use forms
            while (true) 
            {
                //Creates a UdpClient for reading incoming data.
               

                //Creates an IPEndPoint to record the IP Address and port number of the sender. 
                // The IPEndPoint will allow you to read datagrams sent from any source.

                Console.WriteLine("ProSimSplitter");
                Console.WriteLine("Connecting to ProSim TCP Server");
                try
                {
                    Console.WriteLine(DateTime.Now);
                    Connect("127.0.0.1", "test");
                    Thread.Sleep(100);
                    Console.WriteLine("Awake from Sleep");
                    
                }

                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
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


            var reader = new StreamReader(stream);
            String responseData = String.Empty;
            
            while (true) {
                responseData = reader.ReadLine();
                ProcessStream(responseData);
                Console.WriteLine("Async Read Received \"" + responseData + "\"");
                
            }

            // Send the message to the connected TcpServer. 
            //stream.Write(data, 0, data.Length);

            //Console.WriteLine("Sent: {0}", message);         

            // Receive the TcpServer.response.

            // Buffer to store the response bytes.
            data = new Byte[256];

            // String to store the response ASCII representation.
            

            // Read the first batch of the TcpServer response bytes.
            Int32 bytes = stream.Read(data, 0, data.Length);
            responseData = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
            Console.WriteLine("Received: {0}", responseData);

            ProcessStream(responseData);

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

        static void ProcessStream(String StringToBeProcessed)
        {

            Boolean ldebugging = false;

            if (ldebugging) Console.WriteLine("Start ProcessStream");
            if (ldebugging) Console.WriteLine("Processing Stream {0}", StringToBeProcessed);

            char[] delimiterChars = { ' ', ',', '.', ':', '\r' };

            
            if (ldebugging) System.Console.WriteLine("Original text: '{0}'", StringToBeProcessed);

            string[] words = StringToBeProcessed.Split(delimiterChars);
            if (ldebugging) System.Console.WriteLine("{0} words in text:", words.Length);

            foreach (string s in words)
            {
                if (ldebugging) System.Console.WriteLine(s);
                int first = s.IndexOf("FLT_ALT");
                if (ldebugging) Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found FLT_ALT");
                    SendToArduino(s);
                }

                first = s.IndexOf("LAND_ALT");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found LAND_ALT");
                    SendToArduino(s);
                }

                first = s.IndexOf("STARTER_1_SOLENOID");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found STARTER_1_SOLENOID");
                    SendToArduino(s);
                }

                first = s.IndexOf("STARTER_2_SOLENOID");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found STARTER_2_SOLENOID");
                    SendToArduino(s);
                }

                
                first = s.IndexOf("APU_STARTING");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found APU_STARTING");
                    SendToArduino(s);
                }


                first = s.IndexOf("APU_STOPPING");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("APU_STOPPING");
                    SendToArduino(s);
                }

                first = s.IndexOf("AC_POWER");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("AC_POWER");
                    SendToArduino(s);
                }

                first = s.IndexOf("DC_POWER");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("DC_POWER");
                    SendToArduino(s);
                }

                // Begin IRS Indicators

                first = s.IndexOf("I_OH_IRS_ALIGN_L");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found I_OH_IRS_ALIGN_L");
                    SendToArduino(s);
                }

                first = s.IndexOf("I_OH_IRS_ONDC_L");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("Hey found I_OH_IRS_ONDC_L");
                    SendToArduino(s);
                }


            }
            if (ldebugging) Console.WriteLine("End ProcessStream");
        }

        static void SendToArduino(String UpdatedDisplayValue)
        {

            Boolean ldebugging = false;
            if (ldebugging) Console.Write("Starting Send :");
            if (ldebugging) Console.WriteLine(UpdatedDisplayValue);
            byte[] send_buffer = Encoding.ASCII.GetBytes(UpdatedDisplayValue);

            SendingUdpClient.Send(send_buffer, send_buffer.Length, RemoteIpEndPoint);
            if (ldebugging) Console.WriteLine("Send Complete");
            

        }
    }

 
}
