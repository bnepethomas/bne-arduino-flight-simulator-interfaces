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
//      On initial TCP connection to Prosim all values are sent, after initial connect only deltas are sent.
//
//      Three types of values are used:
//      1:  I - indicator either a 1 or 0
//      2:  N - Number generally a float that can be positive or negative
//      3:  B - Binary - used is systems such as overhead backlight
//
//      Values of Interest from Prosim
//      FLT_ALT
//      LAND_ALT
//      APU_STOPPING                    (May map this to a single vlaue APU_SOLENOID)
//      APU_START                       (May map this to a single vlaue APU_SOLENOID)
//      ENGINE_1_PUSHBACK_SOLENOID     (Should fire at N2 56%) 
//      ENGINE_2_PUSHBACK_SOLENOID     (Should fire at N2 56%) 


//      *******************   Both Overheads ***********************

//     "B_OVERHEAD_BACKLIGHT_MASTER = 0"
//     "B_AC_POWER = 0"
//     "B_DC_POWER = 0"
//     "B_DOME_LIGHT_MASTER = 0" - Not initially used
//     "B_LIGHT_TEST = 0" - Hopefully not needed as lights should be driven by indicator variable

//      *******************   Aft Overhead ***********************

//      Slats Panel
//      "I_OH_LEDEVICES_EXT_FLAP1 = 0"
//      "I_OH_LEDEVICES_EXT_FLAP2 = 0"
//      "I_OH_LEDEVICES_EXT_FLAP3 = 0"
//      "I_OH_LEDEVICES_EXT_FLAP4 = 0"
//      "I_OH_LEDEVICES_TRANS_FLAP1 = 0"
//      "I_OH_LEDEVICES_TRANS_FLAP2 = 0"
//      "I_OH_LEDEVICES_TRANS_FLAP3 = 0"
//      "I_OH_LEDEVICES_TRANS_FLAP4 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT1 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT2 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT3 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT4 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT5 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT6 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT7 = 0"
//      "I_OH_LEDEVICES_EXT_SLAT8 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT1 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT2 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT3 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT4 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT5 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT6 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT7 = 0"
//      "I_OH_LEDEVICES_FULLEXT_SLAT8 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT1 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT2 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT3 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT4 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT5 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT6 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT7 = 0"
//      "I_OH_LEDEVICES_TRANS_SLAT8 = 0"

//      Lower Aft Overhead Frame
//      "I_OH_PSEU = 0"
//      "I_OH_GEAR_LEFT_DOWN = 0"
//      "I_OH_GEAR_NOSE_DOWN = 0"
//      "I_OH_GEAR_RIGHT_DOWN = 0"



//      IRS Display and keypad
//      "I_OH_IRS_CLR_KEY = 0" - No Leds in the spot
//      "I_OH_IRS_ENT_KEY = 0" - No Leds in the spot
//      "I_OH_IRS_DOT1 = 0"
//      "I_OH_IRS_DOT2 = 0"
//      "I_OH_IRS_DOT3 = 0"
//      "I_OH_IRS_DOT4 = 0"
//      "I_OH_IRS_DOT5 = 0"
//      "I_OH_IRS_DOT6 = 0"
//      "N_IRS_DISP_LEFT = 37046"
//      "N_IRS_DISP_RIGHT = -76225"
//      "N_IRS_DISP_LEFT = 0"
//      "N_IRS_DISP_RIGHT = 0"

//      IRS System
//      "I_OH_IRS_ALIGN_L = 0"
//      "I_OH_IRS_DCFAIL_L = 0"
//      "I_OH_IRS_FAULT_L = 0"
//      "I_OH_IRS_ONDC_L = 0"
//      "I_OH_IRS_ALIGN_R = 0"
//      "I_OH_IRS_DCFAIL_R = 0"
//      "I_OH_IRS_FAULT_R = 0"
//      "I_OH_IRS_ONDC_R = 0"
//      "I_OH_GPS = 0"

//      Engine Control
//      "I_OH_EEC_ALTN1 = 0" - No Leds in the spot
//      "I_OH_EEC_ALTN2 = 0" - No Leds in the spot
//      "I_OH_EEC1 = 0" - No Leds in the spot
//      "I_OH_EEC2 = 0" - No Leds in the spot
//      "I_OH_ENGINE_CONTROL1 = 0"
//      "I_OH_ENGINE_CONTROL2 = 0"
//      "I_OH_REVERSER1 = 0"
//      "I_OH_REVERSER2 = 0"




//      *******************   Fwd Overhead ***********************

//      Starter 1
//      Starter 2
//      Flight Altitude
//      Landing Altitude
//      "N_OH_FLIGHT_ALT = 0"
//      "N_OH_LAND_ALT = 100"
//      "B_PRESSURISATION_DASHED = 0"
//      "B_PRESSURISATION_DISPLAY_POWER = 0"


//      "B_STARTER_PB_SOLENOID_1 = 0"
//      "B_STARTER_SOLENOID_1 = 0"
//      "B_STARTER_PB_SOLENOID_2 = 0"
//      "B_STARTER_SOLENOID_2 = 0"

//      *********************  MIP  *****************************

//      "B_STICKSHAKER = 0" - not currently used
//      "B_STICKSHAKER_FO = 0" - not currently used
//      "I_MIP_GPWS_INOP = 0" - not currently used
//
//
//      Values Received from Arduino - currently not used, just using joystick interface
//      FLT_ALT_INCREMENT               Sends IN_FLT_ALT=1
//      FLT_ALT_DECREMENT               Sends IN_FLT_ALT=-1
//      LAND_ALT_INCREMENT              Send IN_LAND_ALT=1
//      LAND_ALT_DECREMENT              Send IN_LAND_ALT=-1



//  0 I_OH_LEDEVICES_TRANS_FLAP3  
//  1 I_OH_LEDEVICES_TRANS_FLAP4  
//  2 I_OH_LEDEVICES_TRANS_SLAT5  
//  3 I_OH_LEDEVICES_TRANS_SLAT6  
//  4 I_OH_LEDEVICES_TRANS_SLAT7  
//  5 I_OH_LEDEVICES_TRANS_SLAT8  
//  6 I_OH_IRS_DCFAIL_L  
//  7 I_OH_REVERSER1  
//  8 I_OH_LEDEVICES_EXT_FLAP3  
//  9 I_OH_LEDEVICES_EXT_FLAP4  
//  10 I_OH_LEDEVICES_EXT_SLAT5  
//  11 I_OH_LEDEVICES_EXT_SLAT6  
//  12 I_OH_LEDEVICES_EXT_SLAT7  
//  13 I_OH_LEDEVICES_EXT_SLAT8  
//  14 I_OH_IRS_FAULT_L  
//  15 I_OH_GEAR_NOSE_DOWN  
//  16 I_OH_LEDEVICES_EXT_FLAP3  
//  17 I_OH_LEDEVICES_EXT_FLAP4  
//  18 I_OH_LEDEVICES_FULLEXT_SLAT5  
//  19 I_OH_LEDEVICES_FULLEXT_SLAT6  
//  20 I_OH_LEDEVICES_FULLEXT_SLAT7  
//  21 I_OH_LEDEVICES_FULLEXT_SLAT8  
//  22 I_OH_IRS_ONDC_R  
//  23 I_OH_ENGINE_CONTROL2  
//  24 I_OH_LEDEVICES_TRANS_FLAP2  
//  25 I_OH_LEDEVICES_TRANS_FLAP1  
//  26 I_OH_LEDEVICES_TRANS_SLAT4  
//  27 I_OH_LEDEVICES_TRANS_SLAT3  
//  28 I_OH_LEDEVICES_TRANS_SLAT2  
//  29 I_OH_LEDEVICES_TRANS_SLAT1  
//  30 I_OH_IRS_ALIGN_R  
//  31 I_OH_ENGINE_CONTROL1  
//  32 I_OH_LEDEVICES_EXT_FLAP2  
//  33 I_OH_LEDEVICES_EXT_FLAP1  
//  34 I_OH_LEDEVICES_EXT_SLAT4  
//  35 I_OH_LEDEVICES_EXT_SLAT3  
//  36 I_OH_LEDEVICES_EXT_SLAT2  
//  37 I_OH_LEDEVICES_EXT_SLAT1  
//  38 I_OH_IRS_ALIGN_L  
//  39 I_OH_GEAR_LEFT_DOWN  
//  40 I_OH_LEDEVICES_EXT_FLAP2  
//  41 I_OH_LEDEVICES_EXT_FLAP1  
//  42 I_OH_LEDEVICES_FULLEXT_SLAT4  
//  43 I_OH_LEDEVICES_FULLEXT_SLAT3  
//  44 I_OH_LEDEVICES_FULLEXT_SLAT2  
//  45 I_OH_LEDEVICES_FULLEXT_SLAT1  
//  46 I_OH_IRS_DCFAIL_R  
//  47 I_OH_REVERSER2  
//  54 I_OH_IRS_FAULT_R  
//  55 I_OH_GEAR_RIGHT_DOWN  
//  61 I_OH_GPS  
//  62 I_OH_IRS_ONDC_L  
//  63 I_OH_PSEU  


//      20160502       PT - Creation


//      Considerations
//      Handle TCP disconnection timeout and disconnections - on disconnect ensure displays and solenoinds are switched off    
//      Make simple UDP listener for testing, perhaps command line option for local test

//      Need to determine which of the values writing to Arudino need to be sanity tested, the individual leds should be direct, ones to check:
//      1: Flight and Landing Altitudes, need to collect value, check value, and then see if dashes are present, and maybe if power is available
//      2: The INS status display, again check power availablity
//      3: For the Starter Switches look at timing on the Servo - so after firing the servo then set a flag, and check for millis to reset it


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

                // Check entire line is passed, and for indicators validate value to be either 0 or 1
                // Packet format to Arduino 
                // Indicators I06=1 - I-Indicator, XX, valid options 1 or 0, 0-63 generic indicators, 
                // Relays R06-1 - R-Relay, valid options 1 or 0, 80-90
                // INS Display - O-Oled, "XXXXXXXXXXXXXXX" - fixed length string
                // Seven Segment displays - S-Seven Segment - valid options - all blank, all dashes, or numeric value


                int first = s.IndexOf("I_OH_LEDEVICES_TRANS_FLAP3");
                //Console.WriteLine(first);
                if (first >= 0)
                {
                    Console.WriteLine("I_OH_LEDEVICES_TRANS_FLAP3");
                    SendToArduino(s);
                }
 
                //  1 I_OH_LEDEVICES_TRANS_FLAP4  
                //  2 I_OH_LEDEVICES_TRANS_SLAT5  
                //  3 I_OH_LEDEVICES_TRANS_SLAT6  
                //  4 I_OH_LEDEVICES_TRANS_SLAT7  
                //  5 I_OH_LEDEVICES_TRANS_SLAT8  
                //  6 I_OH_IRS_DCFAIL_L  
                //  7 I_OH_REVERSER1  
                //  8 I_OH_LEDEVICES_EXT_FLAP3  
                //  9 I_OH_LEDEVICES_EXT_FLAP4  
                //  10 I_OH_LEDEVICES_EXT_SLAT5  
                //  11 I_OH_LEDEVICES_EXT_SLAT6  
                //  12 I_OH_LEDEVICES_EXT_SLAT7  
                //  13 I_OH_LEDEVICES_EXT_SLAT8  
                //  14 I_OH_IRS_FAULT_L  
                //  15 I_OH_GEAR_NOSE_DOWN  
                //  16 I_OH_LEDEVICES_EXT_FLAP3  
                //  17 I_OH_LEDEVICES_EXT_FLAP4  
                //  18 I_OH_LEDEVICES_FULLEXT_SLAT5  
                //  19 I_OH_LEDEVICES_FULLEXT_SLAT6  
                //  20 I_OH_LEDEVICES_FULLEXT_SLAT7  
                //  21 I_OH_LEDEVICES_FULLEXT_SLAT8  
                //  22 I_OH_IRS_ONDC_R  
                //  23 I_OH_ENGINE_CONTROL2  
                //  24 I_OH_LEDEVICES_TRANS_FLAP2  
                //  25 I_OH_LEDEVICES_TRANS_FLAP1  
                //  26 I_OH_LEDEVICES_TRANS_SLAT4  
                //  27 I_OH_LEDEVICES_TRANS_SLAT3  
                //  28 I_OH_LEDEVICES_TRANS_SLAT2  
                //  29 I_OH_LEDEVICES_TRANS_SLAT1  
                //  30 I_OH_IRS_ALIGN_R  
                //  31 I_OH_ENGINE_CONTROL1  
                //  32 I_OH_LEDEVICES_EXT_FLAP2  
                //  33 I_OH_LEDEVICES_EXT_FLAP1  
                //  34 I_OH_LEDEVICES_EXT_SLAT4  
                //  35 I_OH_LEDEVICES_EXT_SLAT3  
                //  36 I_OH_LEDEVICES_EXT_SLAT2  
                //  37 I_OH_LEDEVICES_EXT_SLAT1  
                //  38 I_OH_IRS_ALIGN_L  
                //  39 I_OH_GEAR_LEFT_DOWN  
                //  40 I_OH_LEDEVICES_EXT_FLAP2  
                //  41 I_OH_LEDEVICES_EXT_FLAP1  
                //  42 I_OH_LEDEVICES_FULLEXT_SLAT4  
                //  43 I_OH_LEDEVICES_FULLEXT_SLAT3  
                //  44 I_OH_LEDEVICES_FULLEXT_SLAT2  
                //  45 I_OH_LEDEVICES_FULLEXT_SLAT1  
                //  46 I_OH_IRS_DCFAIL_R  
                //  47 I_OH_REVERSER2  
                //  54 I_OH_IRS_FAULT_R  
                //  55 I_OH_GEAR_RIGHT_DOWN  
                //  61 I_OH_GPS  
                //  62 I_OH_IRS_ONDC_L  
                //  63 I_OH_PSEU


                
                
                if (ldebugging) System.Console.WriteLine(s);
                first = s.IndexOf("FLT_ALT");
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
