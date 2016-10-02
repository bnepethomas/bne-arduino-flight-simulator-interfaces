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


// Pressurisation 
// Start up
// Async Read Received "B_PRESSURISATION_DASHED = 0"
// Async Read Received "B_PRESSURISATION_DISPLAY_POWER = 0"
//
// Async Read Received "B_PRESSURISATION_DASHED = 1"
// Async Read Received "B_PRESSURISATION_DISPLAY_POWER = 1"
//
// Async Read Received "N_OH_LAND_ALT = 0"
// Async Read Received "N_OH_FLIGHT_ALT = 0"
// Async Read Received "N_OH_FLIGHT_ALT = 10000"
// Async Read Received "N_OH_LAND_ALT = 100"


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

        public static IPAddress serverAddr = IPAddress.Parse("192.168.2.205");
        public static IPEndPoint RemoteIpEndPoint = new IPEndPoint(serverAddr, 9920);


        // For some devices need to maintain state which will be reset on TCP connection establishment.
        // So initialise to blank state and if any of the related attributes change then look at collective state then fire off to Arudino
        // 
        // These devices include:
        //  Flight and Altitude displays
        //  INS Display
        //  Starter - stil lyet to determine if Arudino should watch millis to flick back
        //
        
        public static bool bHandle_IRS_DISP = false;
        public static bool bHandle_PRESSURISATION_DISPLAY = false;
        public static bool bHandle_STARTER = false;
        public static bool bHandle_OH_Backlight = false;

        public static string sN_IRS_DISP_LEFT;
        public static string sN_IRS_DISP_RIGHT;
        public static string IRS_ENT_LED = "   ";
        public static string IRS_CLR_LED = "   ";

        public static string sN_OH_FLIGHT_ALT;
        public static string sN_OH_LAND_ALT;
        public static string sB_PRESSURISATION_DASHED;
        public static string sB_PRESSURISATION_DISPLAY_POWER;

        public static string sB_STARTER_SOLENOID_1;
        public static string sB_STARTER_PB_SOLENOID_2;


        public static string returnData;
        
        static void Main(string[] args)
        {
            //      Listen for data on UDP 9901, if received pass through to ProSim on TCP
            //      Make UDP Sender for testing - use forms
            while (true) 
            {


                // Test routine that reads in previously sent datastream
                //DirectoryWalk();
                
                
                //Creates a UdpClient for reading incoming data.


                ProcessStream("hhggh");

                //Creates an IPEndPoint to record the IP Address and port number of the sender. 
                // The IPEndPoint will allow you to read datagrams sent from any source.

                Console.WriteLine("ProSimSplitter");
                Console.WriteLine("Connecting to ProSim TCP Server");
                try
                {
                    Console.WriteLine(DateTime.Now);
                    Connect("127.0.0.1", "test");
                    Thread.Sleep(1000);
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


            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // StringToBeProcessed = "I_OH_ENGINE_CONTROL1 = 1";
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++
            
            Boolean ldebugging = false;
            String sWrkstr = "";

            // Reset change flags for devices needing state

            bHandle_IRS_DISP = false;
            bHandle_PRESSURISATION_DISPLAY = false;
            bHandle_STARTER = false;
            bHandle_OH_Backlight = false;

            if (ldebugging) Console.WriteLine("Start ProcessStream");
            if (ldebugging) Console.WriteLine("Processing Stream {0}", StringToBeProcessed);

            char[] delimiterChars = { ' ', ',', ':', '\r' };

            
            if (ldebugging) System.Console.WriteLine("Original text: '{0}'", StringToBeProcessed);

            string[] words = StringToBeProcessed.Split(delimiterChars);
            if (ldebugging) System.Console.WriteLine("{0} words in text:", words.Length);

            // There should only be three things in each line
            // attribute - = - value
            // If there is more than that ignore

            if (words.Length != 3)
            {
                Console.WriteLine("Invalid number of parameters in " + StringToBeProcessed);
                Console.WriteLine("Should be 3, but " + words.Length + " returned");
                return;
            }

            // Check entire line is passed, and for indicators validate value to be either 0 or 1
            // Packet format to Arduino 
            // Indicators I06=1 - I-Indicator, XX, valid options 1 or 0, 0-63 generic indicators, 
            // Relays R06=1 - R-Relay, valid options 1 or 0, 80-90
            // INS Display - O-Oled, "XXXXXXXXXXXXXXX" - fixed length string
            // Seven Segment displays - S-Seven Segment - valid options - all blank, all dashes, or numeric value

            switch (words[0])
            {

                //I_OH_ENGINE_CONTROL1              // Port  31
                case "I_OH_ENGINE_CONTROL1":
                    {
                        SendToArduinoAVPair("I31", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_ENGINE_CONTROL2  			// Port  23
                case "I_OH_ENGINE_CONTROL2":
                    {
                        SendToArduinoAVPair("I23", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_GEAR_LEFT_DOWN  			    // Port  39
                case "I_OH_GEAR_LEFT_DOWN":
                    {
                        SendToArduinoAVPair("I39", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_GEAR_NOSE_DOWN  			    // Port  15
                case "I_OH_GEAR_NOSE_DOWN":
                    {
                        SendToArduinoAVPair("I15", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_GEAR_RIGHT_DOWN  			// Port  55
                case "I_OH_GEAR_RIGHT_DOWN":
                    {
                        SendToArduinoAVPair("I55", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_GPS  						// Port  61
                case "I_OH_GPS":
                    {
                        SendToArduinoAVPair("I61", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_IRS_ALIGN_L  				// Port  38
                case "I_OH_IRS_ALIGN_L":
                    {
                        SendToArduinoAVPair("I38", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_ALIGN_R  				// Port  30
                case "I_OH_IRS_ALIGN_R":
                    {
                        SendToArduinoAVPair("I30", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_DCFAIL_L  				// Port  6 
                case "I_OH_IRS_DCFAIL_L":
                    {
                        SendToArduinoAVPair("I06", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_DCFAIL_R  				// Port  46
                case "I_OH_IRS_DCFAIL_R":
                    {
                        SendToArduinoAVPair("I46", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_FAULT_L  				// Port  14
                case "I_OH_IRS_FAULT_L":
                    {
                        SendToArduinoAVPair("I14", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_IRS_FAULT_R  				// Port  54
                case "I_OH_IRS_FAULT_R":
                    {
                        SendToArduinoAVPair("I54", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_ONDC_L  				    // Port  62
                case "I_OH_IRS_ONDC_L":
                    {
                        SendToArduinoAVPair("I62", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                //I_OH_IRS_ONDC_R  				    // Port  22
                case "I_OH_IRS_ONDC_R":
                    {
                        SendToArduinoAVPair("I22", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_FLAP1  		// Port  33 and 41
                case "I_OH_LEDEVICES_EXT_FLAP1":
                    {
                        // Special Case two leds to manage
                        SendToArduinoAVPair("I33", words[2], "I");
                        SendToArduinoAVPair("I41", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_LEDEVICES_EXT_FLAP2  		// Port  32 and 40
                case "I_OH_LEDEVICES_EXT_FLAP2":
                    {
                        // Special Case two leds to manage
                        SendToArduinoAVPair("I32", words[2], "I");
                        SendToArduinoAVPair("I40", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_LEDEVICES_EXT_FLAP3  		// Port  16 and 8
                case "I_OH_LEDEVICES_EXT_FLAP3":
                    {
                        // Special Case two leds to manage
                        SendToArduinoAVPair("I16", words[2], "I");
                        SendToArduinoAVPair("I08", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_LEDEVICES_EXT_FLAP4  		// Port  17 and 9
                case "I_OH_LEDEVICES_EXT_FLAP4":
                    {
                        // Special Case two leds to manage
                        SendToArduinoAVPair("I17", words[2], "I");
                        SendToArduinoAVPair("I09", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }




                //I_OH_LEDEVICES_EXT_SLAT1  		// Port  37
                case "I_OH_LEDEVICES_EXT_SLAT1":
                    {
                        SendToArduinoAVPair("I37", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT2  		// Port  36
                case "I_OH_LEDEVICES_EXT_SLAT2":
                    {
                        SendToArduinoAVPair("I36", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT3  		// Port  35
                case "I_OH_LEDEVICES_EXT_SLAT3":
                    {
                        SendToArduinoAVPair("I35", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_LEDEVICES_EXT_SLAT4  		// Port  34
                case "I_OH_LEDEVICES_EXT_SLAT4":
                    {
                        SendToArduinoAVPair("I34", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT5  		// Port  10
                case "I_OH_LEDEVICES_EXT_SLAT5":
                    {
                        SendToArduinoAVPair("I10", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT6  		// Port  11
                case "I_OH_LEDEVICES_EXT_SLAT6":
                    {
                        SendToArduinoAVPair("I11", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT7  		// Port  12
                case "I_OH_LEDEVICES_EXT_SLAT7":
                    {
                        SendToArduinoAVPair("I12", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_EXT_SLAT8  		// Port  13
                case "I_OH_LEDEVICES_EXT_SLAT8":
                    {
                        SendToArduinoAVPair("I13", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }



                //I_OH_LEDEVICES_FULLEXT_SLAT1  	// Port  45
                case "I_OH_LEDEVICES_FULLEXT_SLAT1":
                    {
                        SendToArduinoAVPair("I45", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT2  	// Port  44
                case "I_OH_LEDEVICES_FULLEXT_SLAT2":
                    {
                        SendToArduinoAVPair("I44", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT3  	// Port  43
                case "I_OH_LEDEVICES_FULLEXT_SLAT3":
                    {
                        SendToArduinoAVPair("I43", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT4  	// Port  42
                case "I_OH_LEDEVICES_FULLEXT_SLAT4":
                    {
                        SendToArduinoAVPair("I42", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT5  	// Port  18
                case "I_OH_LEDEVICES_FULLEXT_SLAT5":
                    {
                        SendToArduinoAVPair("I18", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT6  	// Port  19
                case "I_OH_LEDEVICES_FULLEXT_SLAT6":
                    {
                        SendToArduinoAVPair("I19", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT7  	// Port  20
                case "I_OH_LEDEVICES_FULLEXT_SLAT7":
                    {
                        SendToArduinoAVPair("I20", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_FULLEXT_SLAT8  	// Port  21
                case "I_OH_LEDEVICES_FULLEXT_SLAT8":
                    {
                        SendToArduinoAVPair("I21", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_FLAP1  	    // Port  25
                case "I_OH_LEDEVICES_TRANS_FLAP1":
                    {
                        SendToArduinoAVPair("I25", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_FLAP2  	    // Port  24
                case "I_OH_LEDEVICES_TRANS_FLAP2":
                    {
                        SendToArduinoAVPair("I24", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_FLAP3  	    // Port  0 
                case "I_OH_LEDEVICES_TRANS_FLAP3":
                    {
                        SendToArduinoAVPair("I00", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_FLAP4  	    // Port  1 
                case "I_OH_LEDEVICES_TRANS_FLAP4":
                    {
                        SendToArduinoAVPair("I01", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT1    	// Port  29
                case "I_OH_LEDEVICES_TRANS_SLAT1":
                    {
                        SendToArduinoAVPair("I29", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT2  	    // Port  28
                case "I_OH_LEDEVICES_TRANS_SLAT2":
                    {
                        SendToArduinoAVPair("I28", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT3  	    // Port  27
                case "I_OH_LEDEVICES_TRANS_SLAT3":
                    {
                        SendToArduinoAVPair("I27", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT4  	    // Port  26
                case "I_OH_LEDEVICES_TRANS_SLAT4":
                    {
                        SendToArduinoAVPair("I26", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT5  	    // Port  2 
                case "I_OH_LEDEVICES_TRANS_SLAT5":
                    {
                        SendToArduinoAVPair("I02", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT6  	    // Port  3 
                case "I_OH_LEDEVICES_TRANS_SLAT6":
                    {
                        SendToArduinoAVPair("I03", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT7  	    // Port  4 
                case "I_OH_LEDEVICES_TRANS_SLAT7":
                    {
                        SendToArduinoAVPair("I04", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_LEDEVICES_TRANS_SLAT8  	    // Port  5 
                case "I_OH_LEDEVICES_TRANS_SLAT8":
                    {
                        SendToArduinoAVPair("I05", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_PSEU 						// Port  63
                case "I_OH_PSEU":
                    {
                        SendToArduinoAVPair("I63", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_REVERSER1  				    // Port  7 
                case "I_OH_REVERSER1":
                    {
                        SendToArduinoAVPair("I07", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }


                //I_OH_REVERSER2  				    // Port  47
                case "I_OH_REVERSER2":
                    {
                        SendToArduinoAVPair("I47", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                // Remaining values do require some state management - flag value has been updated and then break out of 
                // switch state (as opposed to return/exit from subroutine

                // "B_OVERHEAD_BACKLIGHT_MASTER = 0"
                // "B_AC_POWER = 0"
                // "B_DC_POWER = 0"
                // "B_DOME_LIGHT_MASTER = 0" - Not initially used
                // "B_LIGHT_TEST = 0" - Hopefully not needed as lights should be driven by indicator variable


                //  IRS Display and keypad
                //  "I_OH_IRS_CLR_KEY = 0" - No Leds in the spot
                //  "I_OH_IRS_ENT_KEY = 0" - No Leds in the spot
                //  "I_OH_IRS_DOT1 = 0"
                //  "I_OH_IRS_DOT2 = 0"
                //  "I_OH_IRS_DOT3 = 0"
                //  "I_OH_IRS_DOT4 = 0"
                //  "I_OH_IRS_DOT5 = 0"
                //  "I_OH_IRS_DOT6 = 0"
                //  "N_IRS_DISP_LEFT = 37046"
                //  "N_IRS_DISP_RIGHT = -76225"
                //  "N_IRS_DISP_LEFT = 0"
                //  "N_IRS_DISP_RIGHT = 0"

                case "I_OH_IRS_ENT_KEY":
                    {

                        if (words[2] == "0")
                        {
                            IRS_ENT_LED = "   ";
                        }
                        else
                        {
                            IRS_ENT_LED = "ENT";
                        }

                        SendToArduinoAVPair("OL2", IRS_ENT_LED + "        " + IRS_CLR_LED + " ", "S");
                        //Thread.Sleep(25);
                        return;

                    }

                case "I_OH_IRS_CLR_KEY":
                    {


                        if (words[2] == "0")
                        {
                            IRS_CLR_LED = "   ";
                        }
                        else
                        {
                            IRS_CLR_LED = "CLR";
                        }

                        SendToArduinoAVPair("OL2", IRS_ENT_LED + "        " + IRS_CLR_LED + " " , "S");
                        //Thread.Sleep(25);
                        return;
                    }



                case "B_OVERHEAD_BACKLIGHT_MASTER":
                    {
                        SendToArduinoAVPair("R06", words[2], "I");
                        // Nothing more to do fall out
                        return;
                    }

                case "N_IRS_DISP_LEFT":
                    {
                        bHandle_IRS_DISP = true;
                        sN_IRS_DISP_LEFT = words[2];
                        break;
                    }
                case "N_IRS_DISP_RIGHT":
                    {
                        bHandle_IRS_DISP = true;
                        sN_IRS_DISP_RIGHT = words[2];
                        break;
                    }



                //  "N_OH_FLIGHT_ALT = 0"
                //  "N_OH_LAND_ALT = 100"
                //  "B_PRESSURISATION_DASHED = 0"
                //  "B_PRESSURISATION_DISPLAY_POWER = 0"

                case "N_OH_FLIGHT_ALT":
                    {
                        bHandle_PRESSURISATION_DISPLAY = true;
                        sN_OH_FLIGHT_ALT = words[2];
                        break;
                    }
                case "N_OH_LAND_ALT":
                    {
                        bHandle_PRESSURISATION_DISPLAY = true;
                        sN_OH_LAND_ALT = words[2];
                        break;
                    }
                case "B_PRESSURISATION_DASHED":
                    {
                        bHandle_PRESSURISATION_DISPLAY = true;
                        sB_PRESSURISATION_DASHED = words[2];
                        break;
                    }
                case "B_PRESSURISATION_DISPLAY_POWER":
                    {
                        bHandle_PRESSURISATION_DISPLAY = true;
                        sB_PRESSURISATION_DISPLAY_POWER = words[2];
                        break;
                    }


                //  "B_STARTER_PB_SOLENOID_1 = 0"
                //  "B_STARTER_SOLENOID_1 = 0"
                //  "B_STARTER_PB_SOLENOID_2 = 0"
                //  "B_STARTER_SOLENOID_2 = 0"  

                case "B_STARTER_PB_SOLENOID_1":
                    {
                        bHandle_STARTER = true;
                        sB_STARTER_SOLENOID_1 = words[2];
                        SendToArduinoAVPair("S01", sB_STARTER_SOLENOID_1, "I");
                        return;
                    }
                case "B_STARTER_PB_SOLENOID_2":
                    {
                        bHandle_STARTER = true;
                        sB_STARTER_PB_SOLENOID_2 = words[2];
                        SendToArduinoAVPair("S02", sB_STARTER_PB_SOLENOID_2, "I");
                        return;
                    }


                default:
                    {
                        // Haven't found an attribute of interest - throw it out
                        Console.WriteLine("Not Processing " + words[1]);
                        return;
                    }

            }

            // At this point should only be handling the special cases needing state management


            if (bHandle_IRS_DISP)
            {

                // excluding while isolating Arduino hang
                //return;

                // Need to worry about AC or DC power to flag display
                // will handle later

                // IRS Display has two lines of 16 Characters - returns values look like they are 6 Digits
                // and may be prefix by a negative sign
                // Update by creating string of 16 characters and then insert values in string
                // It is possible between receiving updates values are 0 or blank, but this should be 
                // very brief.


                sWrkstr = "                 ";
                string sNorthWrkStr = "";
                string sEastWkrStr = "";

                sWrkstr = sWrkstr.PadRight(16);                

                if (sN_IRS_DISP_LEFT != null)
                {
                    // As we are working with a fresh string each time, just insert characters
                    // Don't need to worry about pushing characters next next statement
                    // stomps over spaces that have been pushed

                    sNorthWrkStr = sN_IRS_DISP_LEFT;
                    if (sNorthWrkStr != "-1")
                    {
                        // If longer than 3 characters hopefully we have full co-ordinates to 3 decimal places
                        if (sNorthWrkStr.Length > 3)
                            sNorthWrkStr = sNorthWrkStr.Insert(sNorthWrkStr.Length - 3, ".");

                        // Need to determine if we are East or West
                        if (sNorthWrkStr.StartsWith("-"))
                            sNorthWrkStr = sNorthWrkStr.Replace("-", "S");
                        else
                            sNorthWrkStr = "N" + sNorthWrkStr;
                    }
                    else
                        sNorthWrkStr = "        ";

                    sWrkstr = sWrkstr.Insert(0, sNorthWrkStr);

                    
                    
                    
                }

                if (sN_IRS_DISP_RIGHT != null)
                {

                    
                    sEastWkrStr = sN_IRS_DISP_RIGHT;
                    if (sEastWkrStr != "-1")
                    {
                        // If longer than 3 characters hopefully we have full co-ordinates to 3 decimal places
                        if (sEastWkrStr.Length > 3)
                            sEastWkrStr = sEastWkrStr.Insert(sEastWkrStr.Length - 3, ".");

                        // Need to determine if we are north or south
                        if (sEastWkrStr.StartsWith("-"))
                            sEastWkrStr = sEastWkrStr.Replace("-", "W");
                        else
                            sEastWkrStr = "E" + sEastWkrStr;
                    }
                    else
                    {
                        sEastWkrStr = "        ";
                    }
                    sWrkstr = sWrkstr.Insert(8, sEastWkrStr);

                }


                // Prune excess characters
                sWrkstr = sWrkstr.Substring(0, 17);

                SendToArduinoAVPair("OL1", sWrkstr, "S");

                return;

            }


            if (bHandle_PRESSURISATION_DISPLAY)
            {
                // Value has changed need to send something
                // Need to consider which display is being updated, so more than just string
                // Use PD1 as prefix for Flight, and PD2 for Landing altitudes 

                if (sB_PRESSURISATION_DISPLAY_POWER == "0")
                {
                    // Power is off send 8 blanks
                    SendToArduinoAVPair("PD1", "        ", "S");
                    SendToArduinoAVPair("PD2", "        ", "S");
                    return;
                }

                if (sB_PRESSURISATION_DASHED == "1")
                {
                    // Power is off send 8 blanks
                    SendToArduinoAVPair("PD1", "  -------", "S");
                    SendToArduinoAVPair("PD2", "  -------", "S");
                    return;
                }

                // At this point power is on and displays not dashed, so throw data
                // PD1 is flight

                if (sN_OH_FLIGHT_ALT != null)
                {
                    sWrkstr = sN_OH_FLIGHT_ALT.PadLeft(8);
                    SendToArduinoAVPair("PD1", sWrkstr, "S");
                }
                // PD2 is landing
                if (sN_OH_LAND_ALT != null)
                {
                    sWrkstr = " " + sN_OH_LAND_ALT.PadLeft(7);
                    SendToArduinoAVPair("PD2", sWrkstr, "S");
                }

                return;

            }       // End Handle Pressurisation




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

        static void SendToArduinoAVPair(String attributeName, String value, string valueType)
        {

            String valueToSend = "";
            Boolean ldebugging = false;
            Double testNumber;

            // Check valueType is known
            //      Three types of values are used:
            //      1:  I - indicator either a 1 or 0
            //      2:  N - Number generally a float that can be positive or negative
            //      3:  B - Binary - used is systems such as overhead backlight
            //      4:  S - String
            switch (valueType)
            {


                case "B":
                    {
                        // Is an indicator - now check if a 0 or 1
                        if (value == "0" || value == "1")
                            break;
                        else
                        {
                            Console.WriteLine("Invalid value passed in " + attributeName + ":" + value + ":" + valueType);
                            // Leave the routine
                            return;
                        }
                    }


                case "I":
                    {
                        // Is an indicator - now check if a 0 or 1
                        if (value == "0" || value == "1")
                            break;
                        else
                        {
                            Console.WriteLine("Invalid value passed in " + attributeName + ":" + value + ":" + valueType);
                            // Leave the routine
                            return;
                        }                    
                    }

                case "N":
                    {
                        // Is an indicator - now check if a 0 or 1
                        if (double.TryParse(value, out testNumber))
                            break;
                        else
                        {
                            Console.WriteLine("Invalid number passed in " + attributeName + ":" + value + ":" + valueType);
                            // Leave the routine
                            return;
                        }
                    }

                case "S":
                    {
                        // Nothing to really check here
                        break;
                    }

                default:
                    {
                        Console.WriteLine("Invalid valuetype passed in " + attributeName + ":" + value + ":" + valueType);
                        return;
                    }

            }


            valueToSend = attributeName + "=" + value;
            Console.WriteLine("Sending " + valueToSend);


            if (ldebugging) Console.Write("Starting Send :");
            if (ldebugging) Console.WriteLine(valueToSend);
            byte[] send_buffer = Encoding.ASCII.GetBytes(valueToSend);

            SendingUdpClient.Send(send_buffer, send_buffer.Length, RemoteIpEndPoint);
            if (ldebugging) Console.WriteLine("Send Complete");

            //Thread.Sleep(5);


        }

        static void TestFramework()
        {
            string StringToBeProcessed = "N_FUEL_FLOW_2 = -76225";

            Boolean ldebugging = true;

            if (ldebugging) Console.WriteLine("Start ProcessStream");
            if (ldebugging) Console.WriteLine("Processing Stream {0}", StringToBeProcessed);

            char[] delimiterChars = { ' ', ',', ':', '\r' };

            
            if (ldebugging) System.Console.WriteLine("Original text: '{0}'", StringToBeProcessed);

            string[] words = StringToBeProcessed.Split(delimiterChars);
            if (ldebugging) System.Console.WriteLine("{0} words in text:", words.Length);

            foreach (string s in words)
            {
                int first = s.IndexOf("I_OH_LEDEVICES_TRANS_FLAP3");
                Console.WriteLine(StringToBeProcessed);
                Console.WriteLine("s vlaue is " +  s);
                Console.WriteLine("Now picking the value of the attribute");
                Console.WriteLine(words[2]); // value 3rd position but it is 0 based so 2


            }

            System.Console.WriteLine("Hit a key");
            System.Console.ReadLine();


        }

        static void ReadFile()
        {
            int counter = 0;
            string line;

            // Read the file and display it line by line.
            System.IO.StreamReader file =
               new System.IO.StreamReader("c:\\test.txt");
            while ((line = file.ReadLine()) != null)
            {
                Console.WriteLine(line);
                counter++;
            }

            file.Close();

            // Suspend the screen.
            Console.ReadLine();

        }

        static void DirectoryWalk()
        {

            //try
            //{
            //    // Only get files that begin with the letter "c."
            //    string[] dirs = Directory.GetFiles(@".", "iam*");
            //    Console.WriteLine("The number of files starting with c is {0}.", dirs.Length);
            //    foreach (string dir in dirs)
            //    {
            //        Console.WriteLine(dir);
            //    }
            //}
            //catch (Exception e)
            //{
            //    Console.WriteLine("The process failed: {0}", e.ToString());
            //}



            int counter = 0;
            string line;
            string sWrkString;

            int iLeadingCharacters=21;

            // Read the file and display it line by line.
            System.IO.StreamReader file =
               new System.IO.StreamReader(@".\Values_Returned_From_Prosim.txt");
            while ((line = file.ReadLine()) != null)
            {
                if (line.IndexOf("Async Read Received ") == 0)
                {
                    Console.WriteLine(line);
                    sWrkString = line.Substring(iLeadingCharacters, (line.Length - (iLeadingCharacters+1)));
                    Console.WriteLine(sWrkString);
                    ProcessStream(sWrkString);
                }
                
                counter++;
            }
            Console.WriteLine("file read complete, press the enter key");
            file.Close();

            // Suspend the screen.
            Console.ReadLine();
            
            
            try
            {
                string dirPath = @"c:\";
                dirPath = @"..";
                List<string> dirs = new List<string>(Directory.EnumerateDirectories(dirPath));

                foreach (var dir in dirs)
                {
                    Console.WriteLine("{0}", dir.Substring(dir.LastIndexOf("\\") + 1));
                }
                Console.WriteLine("{0} directories found.", dirs.Count);
            }
            catch (UnauthorizedAccessException UAEx)
            {
                Console.WriteLine(UAEx.Message);
            }
            catch (PathTooLongException PathEx)
            {
                Console.WriteLine(PathEx.Message);
            }




        }


    }
 
}
