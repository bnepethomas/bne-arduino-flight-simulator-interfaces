using Microsoft.FlightSimulator.SimConnect;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace SimConnect_to_UDP
{
    public partial class frmMain : Form
    {
        private UdpClient udpServer;
        private const int listenPort = 27001;

        // Variables to hold radio frequencies
        // Need to add new variables here as well as in the structure and the request and the display
        // Using StructRadio 
        bool radioFrequencyChanged = false;
        private String com1ActiveFrequency = "";
        private String com2ActiveFrequency = "";
        private String com1StandbyFrequency = "";
        private String com2StandbyFrequency = "";
        private String MainBusVoltage = "";
        private String CIRCUIT_NAVCOM1_ON = "";

        // Variables to hold front panel data
        // Using StructFrontPanel
        bool frontPanelDataChanged = false;
        // Used to determine if we need tp update certain elements are power has become available.
        bool CIRCUIT_NAVCOM1_CHANGED_ON = false;

        private String ALTITUDE = "";                           // ALT
        private String AIRSPEED = "";                           // IAS
        private String VERTICAL_SPEED = "";                     // VSI
        private String PLANE_ALT_ABOVE_GROUND = "";             // AGL
        private String ATTITUDE_INDICATOR_BANK_DEGREES = "";   // BANK
        private String ATTITUDE_INDICATOR_PITCH_DEGREES = "";  // PITCH
        private String ROTOR_RPM_PCT_1 = "";                    // RPMR
        private String GENERAL_ENG_PCT_MAX_RPM_1 = "";          // RPME
        private String ENG_TORQUE_PERCENT_1 = "";               // TQ
        private String ELECTRICAL_TOTAL_LOAD_AMPS = "";         // AMPS
        private String TURB_ENG_ITT_1 = "";                     // ITT
        private String ENG_OIL_TEMPERATURE_1 = "";              // OILT
        private String FUEL_TOTAL_QUANTITY = "";                // FUEL
        private String TURB_ENG_CORRECTED_N1_1 = "";            // N1
        private String ENG_OIL_PRESSURE_1 = "";                 // OILP 
        private String ENG_TRANSMISSION_PRESSURE_1 = "";        // XMSNP
        private String ENG_TRANSMISSION_TEMPERATURE_1 = "";     // XMSNT
        private String FUEL_LOAD = "";                          // FLOAD
        private String ELECTRICAL_LOAD = "";                    // ELOAD
        private String ELECTRICAL_MASTER_BATTERY = "";          // BATSW
        private String AIRSPEED_2 = "";


        private bool Rotor_RPM_Low = false;         //RLOW
        private bool Engine_Out = false;            //EOUT
        private bool Trans_Oil_Pressure = false;    //TOPW
        private bool Trans_Oil_Temp = false;        //TOTW
        private bool Battery_Temp = false;          //BTMP
        private bool Battery_Hot = false;           //BHOT
        private bool Trans_Chip = false;            //TC
        private bool Baggage_Door = false;          //BD
        private bool Engine_Chip = false;           //EC
        private bool TR_Chip = false;               //TRC
        private bool Fuel_Pump = false;             //FPMP
        private bool AFT_Fuel_Filter = false;       //FFLTR
        private bool Gen_Fail = false;              //GENF
        private bool Low_Fuel = false;              //LOWF
        private bool SC_Fail = false;               //SCF

        // User-defined win32 event 
        const int WM_USER_SIMCONNECT = 0x0402;

        // SimConnect object 
        SimConnect simconnect = null;

        enum DEFINITIONS
        {
            Struct1,
            structRadio,
            StructFrontPanel,
        }

        enum DATA_REQUESTS
        {
            REQUEST_1,
            RADIOS,
            REQUEST_FRONTPANEL,
        };

        enum EVENTS
        {
            KEY_COM1_STBY_RADIO_SET,
            KEY_MASTER_BATTERY_SET,
            KEY_AVIONICS_MASTER_SET,
            KEY_COM_STBY_RADIO_SET,
            KEY_COM1_RADIO_SWAP,
            KEY_COM2_RADIO_SWAP,
            KEY_KEY_AVIONICS_MASTER_1_SET,
            KEY_AVIONICS_MASTER_2_SET,
            KEY_ALTERNATOR_SET,
            KEY_ELECTRICAL_BUS_1_AVIONICS_BUS_1_SET,
            KEY_COM_RADIO_SET_HZ,
            KEY_COM2_RADIO_SET_HZ,
            KEY_COM_STBY_RADIO_SET_HZ,
            KEY_COM2_STBY_RADIO_SET_HZ,
            KEY_NAV1_RADIO_SET_HZ,
            KEY_NAV2_RADIO_SET_HZ,
            KEY_NAV1_STBY_SET_HZ,
            KEY_NAV2_STBY_SET_HZ,
            KEY_GPS_DIRECTTO_BUTTON,
        }

        enum Servos
        {
            AirSpeed,
            VerticalSpeed,
            AttitudeIndicatorBankDegrees,
            AttitudeIndicatorPitchDegrees,
            RotorRpmPct1,
            GeneralEngPctMaxRpm1,
            EngTorquePercent1,
            ElectricalTotalLoadAmps,
            TurbEngItt1,
            EngOilTemperature1,
            FuelTotalQuantity,
            TurbEngCorrectedN11,
            EngOilPressure1,
            EngTransmissionPressure1,
            EngTransmissionTemperature1,
            PlaneAltAboveGround,
            FuelLoad,
            ElectricalLoad,
            Number_Of_Servos,
        }

        enum ServoShortCodes
        {
            IAS,
            VSI,
            BANK,
            PITCH,
            RPMR,
            RPME,
            TQ,
            AMPS,
            ITT,
            OILT,
            FUEL,
            N1,
            OILP,
            XMSNP,
            XMSNT,
            AGL,
            FLOAD,
            ELOAD,
        }
        enum GROUP_ID
        {
            GROUP0,
        }

        string UDP_Playload;
        string Output_Payload;
        UdpClient udpClient = new UdpClient();
        UdpClient frontPanelClient = new UdpClient();
        UdpClient OutputClient = new UdpClient();
        DateTime RadioTimeLastPacketSent;
        DateTime FrontPanelTimeLastPacketSent = DateTime.Now;
        TimeSpan span;
        int mS;

        // Will calaculate the position of the servo based on the value received from simconnect and the min and max positions defined for each servo.
        // instead of asking the Mega to do that.  This is because the servos have different ranges and it is easier to manage that in code than on the Mega.
        // The ServoZeroPosition array is to deal with gauges such as VSI and Pitch and Bank which have a zero position in the middle of the range.
        // This allows the code to calculate the position of the servo based on the value received from simconnect and the min and max positions defined
        // for each servo, as well as the zero position for those servos that have a zero position in the middle of the range.


        //                                    ASP  VSI  BNK  PCH  RPMR RPME TQ   AMPS ITT  OILT FUEL N1  OILP  XMNP XMNT  AGL FLOAD ELOAD
        long[] ServMinPosition = new long[]  { 173, 178, 444, 555, 177, 137, 176, 527, 121, 310, 159,   0, 560,   9, 424, 222, 222, 222 };
        long[] ServMaxPosition = new long[]  {  10,  14, 444, 555,  23,   6,  37, 740, 802,  20,  51, 999, 864, 288, 107, 222, 222, 222 };
        long[] ServZeroPosition = new long[] { 173,  93, 444, 555, 177, 137, 176, 527, 121, 310, 159,   0, 560,   9, 424, 222, 222, 222 };
        long[] ServoPosition = new long[]    { 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000 };

        // this is how you declare a data structure so that 
        // simconnect knows how to fill it/read it. 
        // When Adding variables to receive need to add them to this datastructure as well as the request itself ininitDataRequest
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct Struct1
        {
            // this is how you declare a fixed size string 
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double latitude;
            public double longitude;
            public double altitude;
            public double PLANE_ALT_ABOVE_GROUND;
            public double VERTICAL_SPEED;
            public double airspeed;
            public double ATTITUDE_INDICATOR_BANK_DEGREES;
            public double ATTITUDE_INDICATOR_PITCH_DEGREES;
            public double ROTOR_RPM_PCT_1;
            public double GENERAL_ENG_PCT_MAX_RPM_1;
            public double ENG_TORQUE_PERCENT_1;
            public double ELECTRICAL_TOTAL_LOAD_AMPS;
            public double TURB_ENG_ITT_1;
            public double ENG_OIL_TEMPERATURE_1;
            public double FUEL_TOTAL_QUANTITY;
            public double TURB_ENG_CORRECTED_N1_1;
            public double ENG_OIL_PRESSURE_1;
            public double ENG_TRANSMISSION_PRESSURE_1;
            public double ENG_TRANSMISSION_TEMPERATURE_1;
            public double ELECTRICAL_MASTER_BATTERY;
            public double com1ActiveFrequency;
            public double com2ActiveFrequency;
            public double com1StandbyFrequency;
            public double com2StandbyFrequency;
            public double Avionics_Master_Switch;
            public double airspeed_2;
            public double ELECTRICAL_AVIONICS_BUS_VOLTAGE;
            public double ELECTRICAL_MAIN_BUS_VOLTAGE;
            public double CIRCUIT_AVIONICS_ON;
            public double CIRCUIT_NAVCOM1_ON;
            public double ELECTRICAL_MASTER_BATTERY_2;
            //public double ELECTRICAL_BATTERY_BUS_VOLTAGE;




            public double elapsedsimtime;
            public double zulu_time;
            public Int32 time_zone_offset;
            public double absolute_time;
            public double plane_heading_degrees_true;
            public double plane_heading_degrees_magnetic;

            public double GENERAL_ENG_MASTER_ALTERNATOR_1;
            public double GENERAL_ENG_MASTER_ALTERNATOR_2;
            public double TRAILING_EDGE_FLAPS_LEFT_ANGLE;
            public double TRAILING_EDGE_FLAPS_LEFT_PERCENT;
            public double SPOILERS_LEFT_POSITION;
            public double ELEVATOR_TRIM_PCT;
            public double BRAKE_PARKING_INDICATOR;
            public double GENERAL_ENG_FUEL_VALVE_1;
            public double GENERAL_ENG_FUEL_VALVE_2;
            public double GENERAL_ENG_STARTER_1;
            public double GENERAL_ENG_STARTER_2;
            public double GENERAL_ENG_OIL_PRESSURE_1;
            public double GENERAL_ENG_OIL_PRESSURE_2;
            public double ENG_ON_FIRE_1;
            public double ENG_ON_FIRE_2;
            public double STALL_WARNING;

            public double LIGHT_CABIN;
            public double LIGHT_STROBE;
            public double LIGHT_NAV;
            public double LIGHT_TAXI;
            public double INNER_MARKER;
            public double MIDDLE_MARKER;
            public double OUTER_MARKER;
            public double GEAR_CENTER_POSITION;
            public double GEAR_LEFT_POSITION;
            public double GEAR_RIGHT_POSITION;


            public double zulu_time_2; // Used to validate we have all data
        };


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct StructRadio
        {
            // this is how you declare a fixed size string 
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double com1ActiveFrequency;
            public double com2ActiveFrequency;
            public double com1StandbyFrequency;
            public double com2StandbyFrequency;
            public double ELECTRICAL_MAIN_BUS_VOLTAGE;
            public double CIRCUIT_NAVCOM1_ON;
            public double zulu_time_2; // Used to validate we have all data
        };

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct StructFrontPanel
        {
            // this is how you declare a fixed size string 
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double ALTITUDE;                                     // ALT
            public double AIRSPEED;                                     // IAS
            public double VERTICAL_SPEED;                               // VSI
            public double PLANE_ALT_ABOVE_GROUND;                       // AGL
            public double ATTITUDE_INDICATOR_BANK_DEGREES;              // BANK
            public double ATTITUDE_INDICATOR_PITCH_DEGREES;             // PITCH
            public double ROTOR_RPM_PCT_1;                              // RPMR
            public double GENERAL_ENG_PCT_MAX_RPM_1;                    // RPME
            public double ENG_TORQUE_PERCENT_1;                         // TQ
            public double ELECTRICAL_TOTAL_LOAD_AMPS;                   // AMPS
            public double TURB_ENG_ITT_1;                               // ITT
            public double ENG_OIL_TEMPERATURE_1;                        // OILT
            public double FUEL_TOTAL_QUANTITY;                          // FUEL
            public double TURB_ENG_CORRECTED_N1_1;                      // N1
            public double ENG_OIL_PRESSURE_1;                           // OILP 
            public double ENG_TRANSMISSION_PRESSURE_1;                  // XMSNP
            public double ENG_TRANSMISSION_TEMPERATURE_1;               // XMSNT
            public double ELECTRICAL_MASTER_BATTERY;                    // BATSW
            public double CIRCUIT_NAVCOM1_ON;                           //NAVCOM1
            public double AIRSPEED_2;                                   // IAS
        };

        private long Mapper(long value, long fromLow, long fromHigh, long toLow, long toHigh)
        {
            /// <summary>
            /// Re-maps a number from one range to another, similar to the Arduino map() function.
            /// </summary>
            /// <param name="value">The input value to map.</param>
            /// <param name="fromLow">The lower bound of the value's current range.</param>
            /// <param name="fromHigh">The upper bound of the value's current range.</param>
            /// <param name="toLow">The lower bound of the value's target range.</param>
            /// <param name="toHigh">The upper bound of the value's target range.</param>
            /// <returns>The mapped value in the target range.</returns>


            // The formula for the Arduino map() function
            // Need to check how this handles the from being higher than the to range
            // and whether it handles values outside the from range.
            // It should handle both of those cases but need to test that.
            return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;

        }

        private int TQ_Process(int newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(int)Servos.EngTorquePercent1], ServMaxPosition[(int)Servos.EngTorquePercent1]);
            return mappedvalue;
        }


        private int IAS_Process(long newValue)
        {
            // 0  - 173
            // 20 - 165
            // 40 - 142
            // 60 - 115
            // 80 - 84
            // 90 - 74
            //100 - 55
            //110 - 50
            //120 - 40
            //130 - 29
            //140 - 22
            //150 - 10
            switch (newValue)
            {
                case <= 20:
                    return (int)Mapper(newValue, 0, 20, 173, 165);
                case <= 40:
                    return (int)Mapper(newValue, 20, 40, 165, 142);
                case <= 60:
                    return (int)Mapper(newValue, 40, 60, 142, 115);
                case <= 80:
                    return (int)Mapper(newValue, 60, 80, 115, 84);
                case <= 90:
                    return (int)Mapper(newValue, 80, 90, 84, 74);
                case <= 100:
                    return (int)Mapper(newValue, 90, 100, 74, 55);
                case <= 110:
                    return (int)Mapper(newValue, 100, 110, 55, 50);
                case <= 120:
                    return (int)Mapper(newValue, 110, 120, 50, 40);
                case <= 130:
                    return (int)Mapper(newValue, 120, 130, 40, 29);
                case <= 140:
                    return (int)Mapper(newValue, 130, 140, 29, 22);
                case <= 150:
                    return (int)Mapper(newValue, 140, 150, 22, 10);
                case > 150:
                    return 10; ;

            }
        }

        private int RPMR_Process(long newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.RotorRpmPct1)], ServMaxPosition[(uint)(Servos.RotorRpmPct1)]);
            return mappedvalue;
        }

        private int RPME_Process(long newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.GeneralEngPctMaxRpm1)], ServMaxPosition[(uint)(Servos.GeneralEngPctMaxRpm1)]);
            return mappedvalue;
        }

        private int FUEL_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 75,
                ServMinPosition[(uint)(Servos.FuelTotalQuantity)], ServMaxPosition[(uint)(Servos.FuelTotalQuantity)]);
            return mappedvalue;
        }

        private int VSI_Process(long newValue)
        {
            //-6000 - 178
            //-4000 - 161
            //-2000 - 134
            //-1000 - 115
            // 0    - 93
            // 1000 - 70
            // 2000 - 52
            // 4000 - 28
            // 6000 - 15
            switch (newValue)
            {
                case <= -6000:
                    return 178;
                case <= -4000:
                    return (int)Mapper(newValue, -6000, -4000, 178, 161);
                case <= -2000:
                    return (int)Mapper(newValue, -4000, -2000, 161, 134);
                case <= -1000:
                    return (int)Mapper(newValue, -2000, -1000, 134, 115);
                case <= 0:
                    return (int)Mapper(newValue, -1000, 0, 115, 93);
                case <= 1000:
                    return (int)Mapper(newValue, 0, 1000, 93, 70);
                case <= 2000:
                    return (int)Mapper(newValue, 1000, 2000, 70, 52);
                case <= 4000:
                    return (int)Mapper(newValue, 2000, 4000, 52, 28);
                case <= 6000:
                    return (int)Mapper(newValue, 4000, 6000, 28, 15);
                case > 6000:
                    return 15;

                default:
                    return 93;
            }


        }

        private int OILP_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilPressure1)], ServMaxPosition[(uint)(Servos.EngOilPressure1)]);
            return mappedvalue;
        }
        private int OILT_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilTemperature1)], ServMaxPosition[(uint)(Servos.EngOilTemperature1)]);
            return mappedvalue;
        }

        private int XMSNP_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngTransmissionPressure1)], ServMaxPosition[(uint)(Servos.EngTransmissionPressure1)]);
            return mappedvalue;
        }
        private int XMSNT_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngTransmissionTemperature1)], ServMaxPosition[(uint)(Servos.EngTransmissionTemperature1)]);
            return mappedvalue;
        }

        private int ITT_Process(long newValue)
        {
            // 0 - 159
            // 1 - 159
            // 600 - 134
            // 700 - 101
            // 800 - 65
            // 900 - 44
            switch (newValue)
            {
                case <= 100:
                    return (int)(159);
                case <= 600:
                    return (int)(Mapper(newValue, 100, 600, 159, 134));
                case <= 700:
                    return (int)(Mapper(newValue, 600, 700, 134, 101));
                case <= 800:
                    return (int)(Mapper(newValue, 700, 800, 101, 65));
                case <= 900:
                    return (int)(Mapper(newValue, 800, 900, 65, 44));
                default:
                    return (int)(44);
            }

        }
        private int PITCH_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, -90, +90,
                ServMinPosition[(uint)(Servos.AttitudeIndicatorPitchDegrees)], ServMaxPosition[(uint)(Servos.AttitudeIndicatorPitchDegrees)]);
            return mappedvalue;
        }

        private int ROLL_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, -90, 90,
                ServMinPosition[(uint)(Servos.AttitudeIndicatorBankDegrees)], ServMaxPosition[(uint)(Servos.AttitudeIndicatorBankDegrees)]);
            return mappedvalue;
        }

        private int N1_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 105,
                ServMinPosition[(uint)(Servos.TurbEngCorrectedN11)], ServMaxPosition[(uint)(Servos.TurbEngCorrectedN11)]);
            return mappedvalue;
        }

        private int FLOAD_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 30,
                ServMinPosition[(uint)(Servos.FuelLoad)], ServMaxPosition[(uint)(Servos.FuelLoad)]);
            return mappedvalue;
        }

        private int ELOAD_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 100,
                ServMinPosition[(uint)(Servos.ElectricalLoad)], ServMaxPosition[(uint)(Servos.ElectricalLoad)]);
            return mappedvalue;
        }
        private void StartListener()
        {
            Task.Run(async () =>
            {
                try
                {
                    // Create UdpClient on the specified port
                    using (udpServer = new UdpClient(listenPort))
                    {
                        while (true)
                        {
                            // Wait for incoming data
                            UdpReceiveResult result = await udpServer.ReceiveAsync();
                            string receivedData = Encoding.ASCII.GetString(result.Buffer);



                            // Update UI with received data (use Invoke to reach UI thread)
                            this.Invoke(new Action(() =>
                            {
                                listBoxLogs.Items.Add($"Received: {receivedData} from {result.RemoteEndPoint}");
                            }));

                            this.Invoke(new Action(() =>
                            {
                                UpdateRadios(receivedData);
                            }));

                        }
                    }
                }
                catch (Exception ex)
                {
                    // Handle socket exceptions (e.g., if port is already in use)
                    Console.WriteLine(ex.Message);
                }
            });
        }



        int response = 1;

        // Output text - display a maximum of X lines 
        string output = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        void displayText(string s)
        {
            // remove first string from output 
            output = output.Substring(output.IndexOf("\n") + 1);

            // add the new string 
            output += "\n" + response++ + ": " + s;

            // display it 
            richResponse.Text = output;
        }

        private void closeConnection()
        {
            if (simconnect != null)
            {
                // Dispose serves the same purpose as SimConnect_Close() 
                simconnect.Dispose();
                simconnect = null;
                displayText("Connection closed");
            }
        }

        void simconnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            displayText("Connected to MSFS");
        }

        // The case where the user closes Prepar3D 
        void simconnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            displayText("MSFS has exited");
            closeConnection();
        }

        void simconnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            displayText("Exception received: " + data.dwException);
        }
        private void Setup_DataRequests()
        {

            displayText("Pleading for data...");
            //m_scConnection.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.PERIODIQUE, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SECOND, 0, 0, 0, 0);
            //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 1, 0);

            //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 0, 0);
            try
            {
                // working for a once off
                //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.ONCE, 0, 0, 0, 0);

                //  The second to last parameter is the number of frames iterated before a data packet is sent. There isn't a parameter
                //  which maps to events per second so this will variable with frame rate of sim which could range from 15fpc to north of 100fps.
                // To assist in reducing range of pps set upper limit of Sim FPS tp 50 under Graphic Target FPS
                // Interestingly - with a target FPS of 50, seeing 100pps 


                //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SECOND, 0, 0, 1, 0);
                //simconnect.RequestDataOnSimObject(DATA_REQUESTS.RADIOS, DEFINITIONS.structRadio, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 20, 0);
                simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_FRONTPANEL, DEFINITIONS.StructFrontPanel, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 10, 0);


                //simconnect.MapClientEventToSimEvent(EVENTS.KEY_MASTER_BATTERY_SET, "MASTER_BATTERY_SET");
                //simconnect.MapClientEventToSimEvent(EVENTS.KEY_AVIONICS_MASTER_SET, "AVIONICS_MASTER_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_AVIONICS_MASTER_2_SET, "AVIONICS_MASTER_2_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM_STBY_RADIO_SET, "COM_STBY_RADIO_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM1_RADIO_SWAP, "COM1_RADIO_SWAP");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM2_RADIO_SWAP, "COM2_RADIO_SWAP");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_ALTERNATOR_SET, "ALTERNATOR_SET");
                // Electrical Bus 1 to Avionics Bus 1 set
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_ELECTRICAL_BUS_1_AVIONICS_BUS_1_SET, "ELECTRICAL_BUS_1_AVIONICS_BUS_1_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM_STBY_RADIO_SET_HZ, "COM_STBY_RADIO_SET_HZ");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM2_STBY_RADIO_SET_HZ, "COM2_STBY_RADIO_SET_HZ");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_MASTER_BATTERY_SET, "MASTER_BATTERY_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_AVIONICS_MASTER_SET, "AVIONICS_MASTER_SET");
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_GPS_DIRECTTO_BUTTON, "GPS_DIRECTTO_BUTTON");
            }
            catch
            {
                displayText("Unhandled error in data plead ...");
            }


        }


        private void initDataRequest()
        {
            try
            {
                // listen to connect and quit msgs 
                simconnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(simconnect_OnRecvOpen);
                simconnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(simconnect_OnRecvQuit);

                // listen to exceptions 
                simconnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(simconnect_OnRecvException);

                // define a data structure 
                // This data structure will be used to receive data from the sim.
                // The two data structures defined in this example are Struct1 and StructRadio.
                // Struct1 is used to receive a wide variety of data from the sim including position, attitude,
                // engine performance, electrical system status, and more.
                // StructRadio is used to receive radio frequency and electrical bus voltage data.
                // It is important to note that the names of the variables in the structure do not need to match the sim variable names,
                // but they do need to be in the same order as the sim variables are added to the data definition and they need to be of the same type.
                // Variable Reference https://www.prepar3d.com/SDKv4/sdk/references/variables/simulation_variables.html
                // Unless the Units column in the following table identifies the units as a structure or a string,
                // the data will be returned by default in a signed 64 bit floating point value


                // Different aircraft support present the electrical systems differently
                // The Flyside Jet Ranger presents the main bus voltage as "ELECTRICAL MAIN BUS VOLTAGE" with good providing 28V
                // The Aboso Cargo 172 returns 0V when "ELECTRICAL MAIN BUS VOLTAGE" is used
                // The Cowasim JetRanger also presents electical bus 
                // CIRCUIT NAVCOM1 ON is a smple boolean that alo can be used, it is alwas set to one in the 172 but is more likely to be supported across a wider variety of aircraft.


                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Latitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Longitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "PLANE ALT ABOVE GROUND", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "VERTICAL SPEED", "feet per minute", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Airspeed True", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ATTITUDE INDICATOR BANK DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ATTITUDE INDICATOR PITCH DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ROTOR RPM PCT:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG PCT MAX RPM:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG TORQUE PERCENT:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL TOTAL LOAD AMPS", "Amps", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TURB ENG ITT:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG OIL TEMPERATURE:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "FUEL TOTAL QUANTITY", "Gallons", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TURB ENG CORRECTED N1:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG OIL PRESSURE:1", "psi", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG TRANSMISSION PRESSURE:1", "psi", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG TRANSMISSION TEMPERATURE:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MASTER BATTERY", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM ACTIVE FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM ACTIVE FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM STANDBY FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM STANDBY FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Avionics Master Switch", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Airspeed True", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL AVIONICS BUS VOLTAGE", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MAIN BUS VOLTAGE:1", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "CIRCUIT AVIONICS ON", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "CIRCUIT NAVCOM1 ON", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MASTER BATTERY", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                // simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL AVIONICS BUS VOLTAGE", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);  
                // simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MAIN BUS VOLTAGE", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);


                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Sim Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Zulu Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Time Zone Offset", "hours", SIMCONNECT_DATATYPE.INT32, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Absolute Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Heading Degrees Magnetic", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG MASTER ALTERNATOR:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG MASTER ALTERNATOR:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TRAILING EDGE FLAPS LEFT ANGLE", "Radians", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TRAILING EDGE FLAPS LEFT PERCENT", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "SPOILERS LEFT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELEVATOR TRIM PCT", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "BRAKE PARKING INDICATOR", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG FUEL VALVE:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG FUEL VALVE:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG STARTER:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG STARTER:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG OIL PRESSURE:1", "Psf", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG OIL PRESSURE:2", "Psf", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG ON FIRE:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG ON FIRE:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "STALL WARNING", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MASTER BATTERY", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT CABIN", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT STROBE", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT NAV", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT TAXI", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "INNER MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "MIDDLE MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "OUTER MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR CENTER POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR LEFT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR RIGHT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Zulu Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);



                // The StructRadio data definition is focused on radio frequencies and electrical bus voltage, which are critical for avionics operation.
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "COM ACTIVE FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "COM ACTIVE FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "COM STANDBY FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "COM STANDBY FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "ELECTRICAL MAIN BUS VOLTAGE", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.structRadio, "CIRCUIT NAVCOM1 ON", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);


                //ALTITUDE;                           // ALT
                //AIRSPEED;                           // IAS
                //VERTICAL_SPEED;                     // VSI
                //PLANE_ALT_ABOVE_GROUND;             // AGL
                //ATTITUDE_INDICATOR_BANK_DEGREES;   // BANK
                //ATTITUDE_INDICATOR_PITCH_DEGREES;  // PITCH
                //ROTOR_RPM_PCT_1;                    // RPMR
                //GENERAL_ENG_PCT_MAX_RPM_1;          // RPME
                //ENG_TORQUE_PERCENT_1;               // TQ
                //ELECTRICAL_TOTAL_LOAD_AMPS;         // AMPS
                //TURB_ENG_ITT_1;                     // ITT
                //ENG_OIL_TEMPERATURE_1;              // OILT
                //FUEL_TOTAL_QUANTITY;                // FUEL
                //TURB_ENG_CORRECTED_N1_1;            // N1
                //ENG_OIL_PRESSURE_1;                 // OILP 
                //ENG_TRANSMISSION_PRESSURE_1;        // XMSNP
                //ENG_TRANSMISSION_TEMPERATURE_1;    // XMSNT
                //ELECTRICAL_MASTER_BATTERY;          // BATSW
                //AIRSPEED;                           // IAS

                //  The StructFrontPanel data definition is designed to capture a comprehensive set of flight parameters that
                //  are typically displayed on the aircraft's front panel. This includes critical flight information such as
                //  altitude, airspeed, vertical speed, attitude, engine performance metrics, and electrical system status.
                //  By defining this data structure, we can efficiently receive and process a wide range of flight data from the simulator,
                //  which can be used for various applications such as telemetry, flight analysis, or custom instrumentation.
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "Plane Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "Airspeed True", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "VERTICAL SPEED", "feet per minute", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "PLANE ALT ABOVE GROUND", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ATTITUDE INDICATOR BANK DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ATTITUDE INDICATOR PITCH DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ROTOR RPM PCT:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "GENERAL ENG PCT MAX RPM:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ENG TORQUE PERCENT:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ELECTRICAL TOTAL LOAD AMPS", "Amps", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "TURB ENG ITT:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ENG OIL TEMPERATURE:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "FUEL TOTAL QUANTITY", "Gallons", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "TURB ENG CORRECTED N1:1", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ENG OIL PRESSURE:1", "psi", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ENG TRANSMISSION PRESSURE:1", "psi", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ENG TRANSMISSION TEMPERATURE:1", "Celsius", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ELECTRICAL MASTER BATTERY", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "ELECTRICAL MAIN BUS VOLTAGE", "Volts", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.StructFrontPanel, "Airspeed True", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);


                // IMPORTANT: register it with the simconnect managed wrapper marshaller 
                // if you skip this step, you will only receive a uint in the .dwData field. 
                simconnect.RegisterDataDefineStruct<Struct1>(DEFINITIONS.Struct1);
                simconnect.RegisterDataDefineStruct<StructRadio>(DEFINITIONS.structRadio);
                simconnect.RegisterDataDefineStruct<StructFrontPanel>(DEFINITIONS.StructFrontPanel);

                // catch a simobject data request 
                // Simconnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simconnect_OnRecvSimobjectDataBytype);

                simconnect.OnRecvSimobjectData += new SimConnect.RecvSimobjectDataEventHandler(simconnect_OnRecvSimobjectData);
            }
            catch (COMException ex)
            {
                displayText(ex.Message);

            }
        }


        // This is the one receiving streaming data
        void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {



            switch ((DATA_REQUESTS)data.dwRequestID)
            {
                case DATA_REQUESTS.RADIOS:
                    StructRadio sRadio = (StructRadio)data.dwData[0];


                    if (com1ActiveFrequency != sRadio.com1ActiveFrequency.ToString("F3"))
                    {
                        radioFrequencyChanged = true;
                        com1ActiveFrequency = sRadio.com1ActiveFrequency.ToString("F3");
                    }
                    ;

                    if (com1StandbyFrequency != sRadio.com1StandbyFrequency.ToString("F3"))
                    {
                        radioFrequencyChanged = true;
                        com1StandbyFrequency = sRadio.com1StandbyFrequency.ToString("F3");
                    }
                    ;

                    if (com2ActiveFrequency != sRadio.com2ActiveFrequency.ToString("F3"))
                    {
                        radioFrequencyChanged = true;
                        com2ActiveFrequency = sRadio.com2ActiveFrequency.ToString("F3");
                    }
                    ;

                    if (com2StandbyFrequency != sRadio.com2StandbyFrequency.ToString("F3"))
                    {
                        radioFrequencyChanged = true;
                        com2StandbyFrequency = sRadio.com2StandbyFrequency.ToString("F3");
                    }
                    ;

                    if (MainBusVoltage != sRadio.ELECTRICAL_MAIN_BUS_VOLTAGE.ToString("F1"))
                    {
                        radioFrequencyChanged = true;
                        MainBusVoltage = sRadio.ELECTRICAL_MAIN_BUS_VOLTAGE.ToString("F1");
                    }
                    ;

                    if (MainBusVoltage != sRadio.ELECTRICAL_MAIN_BUS_VOLTAGE.ToString("F1"))
                    {
                        radioFrequencyChanged = true;
                        MainBusVoltage = sRadio.ELECTRICAL_MAIN_BUS_VOLTAGE.ToString("F1");
                    }
                    ;
                    if (CIRCUIT_NAVCOM1_ON != sRadio.CIRCUIT_NAVCOM1_ON.ToString())
                    {
                        radioFrequencyChanged = true;
                        CIRCUIT_NAVCOM1_ON = sRadio.CIRCUIT_NAVCOM1_ON.ToString();
                    }
                    ;



                    span = DateTime.Now - RadioTimeLastPacketSent;
                    mS = (int)span.TotalMilliseconds;
                    // displayText("Its been this many mS since sending last packet: " + mS.ToString());

                    if (mS >= 200 && radioFrequencyChanged == true)
                    {
                        radioFrequencyChanged = false;
                        UDP_Playload = "D,C1A:" + com1ActiveFrequency;
                        UDP_Playload += ",C1S:" + com1StandbyFrequency;
                        UDP_Playload += ",C2A:" + com2ActiveFrequency;
                        UDP_Playload += ",C2S:" + com2StandbyFrequency;
                        UDP_Playload += ",MAINBUS:" + MainBusVoltage;
                        UDP_Playload += ",NAVCOM1:" + CIRCUIT_NAVCOM1_ON;
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        udpClient.Send(senddata, senddata.Length);

                        RadioTimeLastPacketSent = DateTime.Now;
                    }
                    // if its been a while since changes send an update anyways
                    if (mS >= 5000)

                    {
                        radioFrequencyChanged = false;
                        UDP_Playload = "D,C1A:" + com1ActiveFrequency;
                        UDP_Playload += ",C1S:" + com1StandbyFrequency;
                        UDP_Playload += ",C2A:" + com2ActiveFrequency;
                        UDP_Playload += ",C2S:" + com2StandbyFrequency;
                        UDP_Playload += ",MAINBUS:" + MainBusVoltage;
                        UDP_Playload += ",NAVCOM1:" + CIRCUIT_NAVCOM1_ON;
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        udpClient.Send(senddata, senddata.Length);
                        RadioTimeLastPacketSent = DateTime.Now;
                    }


                    break;


                case DATA_REQUESTS.REQUEST_FRONTPANEL:

                    // Cowasim has most of these variables working ok
                    // Fly Inside missing Rotor RPM and others
                    StructFrontPanel sFrontPanel = (StructFrontPanel)data.dwData[0];

                    displayText("Aircraft Model:    " + sFrontPanel.title);
                    displayText("Alt:               " + sFrontPanel.ALTITUDE.ToString("F0"));
                    displayText("Radar Alt:         " + sFrontPanel.PLANE_ALT_ABOVE_GROUND.ToString("F0"));
                    displayText("Vertical Speed:    " + sFrontPanel.VERTICAL_SPEED.ToString("F0"));
                    displayText("Airspeed           " + sFrontPanel.AIRSPEED.ToString("F0"));
                    displayText("Bank Degrees       " + sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3"));
                    displayText("Pitch Degrees      " + sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3"));
                    displayText("ROTOR RPM          " + sFrontPanel.ROTOR_RPM_PCT_1);
                    displayText("ENG ROTOR RPM      " + sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1);
                    displayText("ENG TORQUE PERCENT " + sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9);
                    displayText("ELECTRICAL TOTAL LOAD AMPS " + sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56);
                    // ENG EXHAUST is in Rankine need to convert to Celsius - TOT not directly available
                    // So some more calculations needed - for now just send ITT
                    displayText("TURBINE ENG ITT    " + sFrontPanel.TURB_ENG_ITT_1);
                    displayText("ENG OIL TEMP       " + sFrontPanel.ENG_OIL_TEMPERATURE_1);
                    displayText("FUEL TOTAL QUANTITY        " + sFrontPanel.FUEL_TOTAL_QUANTITY);
                    displayText("TURB ENG N1        " + sFrontPanel.TURB_ENG_CORRECTED_N1_1);
                    displayText("ENG OIL PRESSURE   " + sFrontPanel.ENG_OIL_PRESSURE_1);
                    displayText("ENG TRANSMISSION PRESSURE  " + sFrontPanel.ENG_TRANSMISSION_PRESSURE_1);
                    displayText("ENG TRANSMISSION TEMPERATURE " + sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1);
                    displayText("ELECTRICAL MASTER BATTERY   " + sFrontPanel.ELECTRICAL_MASTER_BATTERY);
                    displayText("CIRCUIT_NAVCOM1_ON          " + sFrontPanel.CIRCUIT_NAVCOM1_ON);
                    displayText("Airspeed           " + sFrontPanel.AIRSPEED_2.ToString("F0"));

                    // Clear the UDP payload
                    UDP_Playload = "D";
                    CIRCUIT_NAVCOM1_CHANGED_ON = false;
                    if (CIRCUIT_NAVCOM1_ON != sFrontPanel.CIRCUIT_NAVCOM1_ON.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        UDP_Playload = UDP_Playload + ",NAVCOM1:" + CIRCUIT_NAVCOM1_ON;
                        CIRCUIT_NAVCOM1_ON = sFrontPanel.CIRCUIT_NAVCOM1_ON.ToString();
                        if (CIRCUIT_NAVCOM1_ON == "False")
                        {
                            displayText("NAVCOM1 is OFF");
                            // Need to turn off all Warning indicators and select instruments that are powered by NAVCOM1
                            // Known exclusions are AirSpeed, Altitude and possibly VSI

                            UDP_Playload += ",RLOW:0";
                            UDP_Playload += ",EOUT:0";
                            UDP_Playload += ",TOPW:0";
                            UDP_Playload += ",TOWT:0";
                            UDP_Playload += ",BTMP0:";
                            UDP_Playload += ",BHOT0";
                            UDP_Playload += ",TC:0";
                            UDP_Playload += ",BD0:";
                            UDP_Playload += ",EC:0";
                            UDP_Playload += ",TRC:0"; ;
                            UDP_Playload += ",FPMP:0";
                            UDP_Playload += ",FFLTR:0";
                            UDP_Playload += ",GENF0:";
                            UDP_Playload += ",LOWF:0";
                            UDP_Playload += ",SCF:0";
                        }
                        else
                        {
                            // Need to update impac ted instruments with current values which would not
                            // have ben updated as the instrument itself did not change value
                            CIRCUIT_NAVCOM1_CHANGED_ON = true;
                        }
                    }  //end if NAVCOM1 state changed


                    if (ALTITUDE != sFrontPanel.ALTITUDE.ToString("F0")) ;
                    {
                        frontPanelDataChanged = true;
                        ALTITUDE = sFrontPanel.ALTITUDE.ToString("F0");
                        UDP_Playload = "D,ALT:" + ALTITUDE;
                    }
                    ;

                    if (AIRSPEED != sFrontPanel.AIRSPEED.ToString("F0")) ;
                    {
                        frontPanelDataChanged = true;
                        AIRSPEED = sFrontPanel.AIRSPEED.ToString("F0");
                        int a = (int)(sFrontPanel.AIRSPEED);
                        UDP_Playload = UDP_Playload + ",IAS:" + IAS_Process(a).ToString();
                    }
                    ;

                    if (ROTOR_RPM_PCT_1 != sFrontPanel.ROTOR_RPM_PCT_1.ToString()
                        || CIRCUIT_NAVCOM1_CHANGED_ON == true) ;
                    //While Rotor RPM is not directly affected by NAVCOM1 power state, need to updae the warning indicator state
                    {
                        frontPanelDataChanged = true;
                        ROTOR_RPM_PCT_1 = sFrontPanel.ROTOR_RPM_PCT_1.ToString();
                        int a = (int)(sFrontPanel.ROTOR_RPM_PCT_1);
                        UDP_Playload = UDP_Playload + ",RPMR:" + RPMR_Process(a).ToString();

                        if (sFrontPanel.ROTOR_RPM_PCT_1 <= 95)
                        {
                            UDP_Playload = UDP_Playload + ",RLOW:0";
                            Rotor_RPM_Low = false;
                        }
                        else
                        {
                            UDP_Playload = UDP_Playload + ",RLOW:1";
                            Rotor_RPM_Low = true;
                        }
                    }
                    ; // End Rotor RPM 


                    if (GENERAL_ENG_PCT_MAX_RPM_1 != sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1.ToString()
                        || CIRCUIT_NAVCOM1_CHANGED_ON == true) ;
                    {
                        frontPanelDataChanged = true;
                        GENERAL_ENG_PCT_MAX_RPM_1 = sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1.ToString();
                        int a = (int)(sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1);
                        UDP_Playload = UDP_Playload + ",RPME:" + RPME_Process(a).ToString();


                        if (sFrontPanel.GENERAL_ENG_PCT_MAX_RPM_1 >= 60)
                        {
                            UDP_Playload = UDP_Playload + ",EOUT:0";
                            Engine_Out = false;
                        }
                        else
                        {
                            UDP_Playload = UDP_Playload + ",EOUT:1";
                            Engine_Out = true;
                        }
                    }
                    ;  // End Engine RPM


                    if (FUEL_TOTAL_QUANTITY != sFrontPanel.FUEL_TOTAL_QUANTITY.ToString()
                        || CIRCUIT_NAVCOM1_CHANGED_ON == true) ;
                    {
                        frontPanelDataChanged = true;
                        FUEL_TOTAL_QUANTITY = sFrontPanel.FUEL_TOTAL_QUANTITY.ToString();
                        int a = (int)(sFrontPanel.FUEL_TOTAL_QUANTITY);
                        UDP_Playload = UDP_Playload + ",FUEL:" + FUEL_Process(a).ToString();
                        UDP_Playload = UDP_Playload + ",LOWF:" + Low_Fuel.ToString();

                        if (sFrontPanel.FUEL_TOTAL_QUANTITY <= 17)
                        {
                            Low_Fuel = true;
                            UDP_Playload = UDP_Playload + ",LOWF:1";
                        }
                        else
                        {
                            Low_Fuel = false;
                            UDP_Playload = UDP_Playload + ",LOWF:0";
                        }
                    }
                    ;

                    if (VERTICAL_SPEED != sFrontPanel.VERTICAL_SPEED.ToString("F0")) ;
                    {
                        frontPanelDataChanged = true;
                        VERTICAL_SPEED = sFrontPanel.VERTICAL_SPEED.ToString("F0");
                        int a = (int)(sFrontPanel.VERTICAL_SPEED);
                        UDP_Playload = UDP_Playload + ",VSI:" + VSI_Process(a).ToString();
                    }
                    ;


                    if (PLANE_ALT_ABOVE_GROUND != sFrontPanel.PLANE_ALT_ABOVE_GROUND.ToString("F0")
                        || CIRCUIT_NAVCOM1_CHANGED_ON == true) ;
                    // Radar Altitude is affected by NAVCOM1 power state as the radar altimeter is powered by NAVCOM1. If NAVCOM1 was turned on then we need to update the radar altitude value even if it did not change
                    // as it would not have been updated while NAVCOM1 was off
                    {
                        frontPanelDataChanged = true;
                        if (CIRCUIT_NAVCOM1_ON == "True")
                        {

                            PLANE_ALT_ABOVE_GROUND = sFrontPanel.PLANE_ALT_ABOVE_GROUND.ToString("F0");

                        }
                        else
                        {
                            PLANE_ALT_ABOVE_GROUND = "0";
                        }
                        ;
                        UDP_Playload = UDP_Playload + ",AGL:" + PLANE_ALT_ABOVE_GROUND;
                    }
                    ;

                    if (ATTITUDE_INDICATOR_BANK_DEGREES != sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3")) ;
                    {
                        frontPanelDataChanged = true;
                        ATTITUDE_INDICATOR_BANK_DEGREES = sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3");
                        int a = (int)(sFrontPanel.ATTITUDE_INDICATOR_BANK_DEGREES);
                        UDP_Playload = UDP_Playload + ",BANK:" + ROLL_Process(a).ToString();
                    }
                    ;
                    if (ATTITUDE_INDICATOR_PITCH_DEGREES != sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3")) ;
                    {
                        frontPanelDataChanged = true;
                        ATTITUDE_INDICATOR_PITCH_DEGREES = sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3");
                        int a = (int)(sFrontPanel.ATTITUDE_INDICATOR_PITCH_DEGREES);
                        UDP_Playload = UDP_Playload + ",PITCH:" + PITCH_Process(a).ToString();
                    }
                    ;


                    if (ENG_TORQUE_PERCENT_1 != (sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9).ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ENG_TORQUE_PERCENT_1 = (sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9).ToString();
                        int a = (int)(sFrontPanel.ENG_TORQUE_PERCENT_1 * 4 / 9);

                        UDP_Playload = UDP_Playload + ",TQ:" + TQ_Process(a);
                        ;
                    }
                    ;

                    if (ELECTRICAL_TOTAL_LOAD_AMPS != (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ELECTRICAL_TOTAL_LOAD_AMPS = (sFrontPanel.ELECTRICAL_TOTAL_LOAD_AMPS * 40 / 56).ToString();
                        UDP_Playload = UDP_Playload + ",AMPS:" + ELECTRICAL_TOTAL_LOAD_AMPS;
                    }
                    ;

                    if (TURB_ENG_ITT_1 != sFrontPanel.TURB_ENG_ITT_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        TURB_ENG_ITT_1 = sFrontPanel.TURB_ENG_ITT_1.ToString();
                        int a = (int)(sFrontPanel.TURB_ENG_ITT_1);
                        UDP_Playload = UDP_Playload + ",ITT:" + ITT_Process(a).ToString();
                    }
                    ;

                    if (ENG_OIL_TEMPERATURE_1 != sFrontPanel.ENG_OIL_TEMPERATURE_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ENG_OIL_TEMPERATURE_1 = sFrontPanel.ENG_OIL_TEMPERATURE_1.ToString();
                        int a = (int)(sFrontPanel.ENG_OIL_TEMPERATURE_1);
                        UDP_Playload = UDP_Playload + ",OILT:" + OILT_Process(a).ToString();
                    }
                    ;



                    if (TURB_ENG_CORRECTED_N1_1 != sFrontPanel.TURB_ENG_CORRECTED_N1_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        TURB_ENG_CORRECTED_N1_1 = sFrontPanel.TURB_ENG_CORRECTED_N1_1.ToString();
                        int a = (int)(sFrontPanel.TURB_ENG_CORRECTED_N1_1);
                        UDP_Playload = UDP_Playload + ",N1:" + N1_Process(a).ToString();

                    }
                    ;

                    if (ENG_OIL_PRESSURE_1 != sFrontPanel.ENG_OIL_PRESSURE_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ENG_OIL_PRESSURE_1 = sFrontPanel.ENG_OIL_PRESSURE_1.ToString();
                        int a = (int)(sFrontPanel.ENG_OIL_PRESSURE_1);
                        UDP_Playload = UDP_Playload + ",OILP:" + OILP_Process(a).ToString();
                    }
                    ;

                    if (ENG_TRANSMISSION_PRESSURE_1 != sFrontPanel.ENG_TRANSMISSION_PRESSURE_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ENG_TRANSMISSION_PRESSURE_1 = sFrontPanel.ENG_TRANSMISSION_PRESSURE_1.ToString();
                        int a = (int)(sFrontPanel.ENG_TRANSMISSION_PRESSURE_1);
                        UDP_Playload = UDP_Playload + ",XMSNP:" + XMSNP_Process(a).ToString();   
                    }
                    ;

                    if (ENG_TRANSMISSION_TEMPERATURE_1 != sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ENG_TRANSMISSION_TEMPERATURE_1 = sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1.ToString();
                        int a = (int)(sFrontPanel.ENG_TRANSMISSION_TEMPERATURE_1);
                        UDP_Playload = UDP_Playload + ",XMSNT:" + XMSNT_Process(a).ToString();
                    }
                    ;

                    if (ELECTRICAL_MASTER_BATTERY != sFrontPanel.ELECTRICAL_MASTER_BATTERY.ToString()) ;
                    {
                        frontPanelDataChanged = true;
                        ELECTRICAL_MASTER_BATTERY = sFrontPanel.ELECTRICAL_MASTER_BATTERY.ToString();
                        UDP_Playload = UDP_Playload + ",BATSW:" + ELECTRICAL_MASTER_BATTERY;
                    }
                    ;



                    span = DateTime.Now - FrontPanelTimeLastPacketSent;
                    mS = (int)span.TotalMilliseconds;
                    // displayText("Its been this many mS since sending last packet: " + mS.ToString());

                    if (mS >= 200 && frontPanelDataChanged == true)
                    {
                        frontPanelDataChanged = false;



                        UDP_Playload += ",TOPW:" + Trans_Oil_Pressure.ToString();
                        UDP_Playload += ",TOWT:" + Trans_Oil_Temp.ToString();
                        UDP_Playload += ",BTMP:" + Battery_Temp.ToString();
                        UDP_Playload += ",BHOT:" + Battery_Hot.ToString();
                        UDP_Playload += ",TC:" + Trans_Chip.ToString();
                        UDP_Playload += ",BD:" + Baggage_Door.ToString();
                        UDP_Playload += ",EC:" + Engine_Chip.ToString();
                        UDP_Playload += ",TRC:" + TR_Chip.ToString();
                        UDP_Playload += ",FPMP:" + Fuel_Pump.ToString();
                        UDP_Playload += ",FFLTR:" + AFT_Fuel_Filter.ToString();
                        UDP_Playload += ",GENF:" + Gen_Fail.ToString();

                        UDP_Playload += ",SCF:" + SC_Fail.ToString();


                        if (UDP_Playload != "D")
                        {
                            // Something has changed so send it
                            Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                            frontPanelClient.Send(senddata, senddata.Length);
                            FrontPanelTimeLastPacketSent = DateTime.Now;
                        }

                    }
                    // if its been a while since changes send an update anyways
                    if (mS >= 5000)

                    {
                        // NEED TO REBUILD THIS LIST FOR FRONT PANEL OR FIND ANOTHER WAY TO FLAG A MAJOR CHANGE
                        frontPanelDataChanged = false;
                        UDP_Playload = "D,C1A:" + com1ActiveFrequency;
                        UDP_Playload += ",C1S:" + com1StandbyFrequency;
                        UDP_Playload += ",C2A:" + com2ActiveFrequency;
                        UDP_Playload += ",C2S:" + com2StandbyFrequency;
                        UDP_Playload += ",MAINBUS:" + MainBusVoltage;
                        UDP_Playload += ",NAVCOM1:" + CIRCUIT_NAVCOM1_ON;
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        udpClient.Send(senddata, senddata.Length);
                        FrontPanelTimeLastPacketSent = DateTime.Now;
                    }


                    break;

                case DATA_REQUESTS.REQUEST_1:
                    Struct1 s1 = (Struct1)data.dwData[0];

                    /*
                    displayText("Aircraft Model:    " + s1.title);
                    displayText("Lat:               " + s1.latitude.ToString("F4"));
                    displayText("Lon:               " + s1.longitude.ToString("F4"));
                    displayText("Alt:               " + s1.altitude.ToString("F0"));
                    displayText("Radar Alt:         " + s1.PLANE_ALT_ABOVE_GROUND.ToString("F0"));
                    displayText("Vertical Speed:    " + s1.VERTICAL_SPEED.ToString("F0"));
                    displayText("Airspeed           " + s1.airspeed.ToString("F0"));
                    displayText("Bank Degrees       " + s1.ATTITUDE_INDICATOR_BANK_DEGREES.ToString("F3"));
                    displayText("Pitch Degrees      " + s1.ATTITUDE_INDICATOR_PITCH_DEGREES.ToString("F3"));
                    //displayText("ROTOR RPM          " + s1.ROTOR_RPM_PCT_1);
                    //displayText("ENG ROTOR RPM      " + s1.GENERAL_ENG_PCT_MAX_RPM_1);
                    //displayText("ENG TORQUE PERCENT " + s1.ENG_TORQUE_PERCENT_1 * 4 /9);
                    //displayText("ELECTRICAL TOTAL LOAD AMPS " + s1.ELECTRICAL_TOTAL_LOAD_AMPS * 40/56);
                    // ENG EXHAUST is in Rankine need to convert to Celsius - TOT not directly available
                    // So some more calculations needed - for now just send ITT
                    //displayText("TURBINE ENG ITT    " + s1.TURB_ENG_ITT_1);
                    //displayText("ENG OIL TEMP       " + s1.ENG_OIL_TEMPERATURE_1); 
                    //displayText("FUEL TOTAL QUANTITY        " + s1.FUEL_TOTAL_QUANTITY);
                    //displayText("TURB ENG N1        " + s1.TURB_ENG_CORRECTED_N1_1);
                    //displayText("ENG OIL PRESSURE   " + s1.ENG_OIL_PRESSURE_1);
                    //displayText("ENG TRANSMISSION PRESSURE  " + s1.ENG_TRANSMISSION_PRESSURE_1);
                    //displayText("ENG TRANSMISSION TEMPERATURE " + s1.ENG_TRANSMISSION_TEMPERATURE_1);
                    displayText("ELECTRICAL MASTER BATTERY   " + s1.ELECTRICAL_MASTER_BATTERY);
                    displayText("COM1 Active Freq   " + s1.com1ActiveFrequency);
                    displayText("COM2 Active Freq   " + s1.com2ActiveFrequency);
                    displayText("COM1 Standby Freq  " + s1.com1StandbyFrequency);
                    displayText("COM2 Standby Freq  " + s1.com2StandbyFrequency);
                    displayText("ELECTRICAL MASTER BATTERY   " + s1.ELECTRICAL_MASTER_BATTERY);
                    displayText("Avionics Master Switch      " + s1.Avionics_Master_Switch);
                    displayText("Airspeed 2          " + s1.airspeed_2.ToString("F0"));
                    displayText("ELECTRICAL AVIONICS BUS VOLTAGE " + s1.ELECTRICAL_AVIONICS_BUS_VOLTAGE.ToString("F2"));
                    displayText("ELECTRICAL MAIN BUS VOLTAGE  " + s1.ELECTRICAL_MAIN_BUS_VOLTAGE.ToString("F2"));
                    displayText("CIRCUIT AVIONICS ON " + s1.CIRCUIT_AVIONICS_ON);
                    displayText("CIRCUIT NAVCOM1 ON  " + s1.CIRCUIT_NAVCOM1_ON);
                    displayText("ELECTRICAL MASTER BATTERY   " + s1.ELECTRICAL_MASTER_BATTERY_2);

                    //displayText("Sim Time           " + s1.elapsedsimtime);
                    //displayText("Zulu Time          " + s1.zulu_time);
                    //displayText("Time Zone Offset   " + s1.time_zone_offset);
                    //displayText("Absolute Time      " + s1.absolute_time);
                    //displayText("Plane Heading True " + s1.plane_heading_degrees_true);
                    //displayText("Plane Heading Mag  " + s1.plane_heading_degrees_magnetic);
                    //displayText("GENERAL ENG MASTER ALTERNATOR:1  " + s1.GENERAL_ENG_MASTER_ALTERNATOR_1);
                    //displayText("GENERAL ENG MASTER ALTERNATOR:2  " + s1.GENERAL_ENG_MASTER_ALTERNATOR_2);
                    //displayText("TRAILING EDGE FLAPS LEFT ANGLE  " + s1.TRAILING_EDGE_FLAPS_LEFT_ANGLE);
                    //displayText("TRAILING EDGE FLAPS LEFT PERCENT  " + s1.TRAILING_EDGE_FLAPS_LEFT_PERCENT);
                    //displayText("SPOILERS LEFT POSITION      " + s1.SPOILERS_LEFT_POSITION);
                    //displayText("ELEVATOR TRIM PCT           " + s1.ELEVATOR_TRIM_PCT);
                    //displayText("BRAKE PARKING INDICATOR     " + s1.BRAKE_PARKING_INDICATOR);
                    //displayText("GENERAL ENG FUEL VALVE:1    " + s1.GENERAL_ENG_FUEL_VALVE_1);
                    //displayText("GENERAL ENG FUEL VALVE:2    " + s1.GENERAL_ENG_FUEL_VALVE_2);
                    //displayText("GENERAL ENG STARTER:1       " + s1.GENERAL_ENG_STARTER_1);
                    //displayText("GENERAL ENG STARTER:2       " + s1.GENERAL_ENG_STARTER_2);
                    //displayText("GENERAL ENG OIL PRESSURE:1  " + s1.GENERAL_ENG_OIL_PRESSURE_1);
                    //displayText("GENERAL ENG OIL PRESSURE:2  " + s1.GENERAL_ENG_OIL_PRESSURE_2);
                    //displayText("ENG ON FIRE:2               " + s1.ENG_ON_FIRE_1);
                    //displayText("ENG ON FIRE:2               " + s1.ENG_ON_FIRE_2);
                    //displayText("STALL WARNING               " + s1.STALL_WARNING);

                    //displayText("LIGHT CABIN                 " + s1.LIGHT_CABIN);
                    //displayText("LIGHT STROBE                " + s1.LIGHT_STROBE);
                    //displayText("LIGHT NAV                   " + s1.LIGHT_NAV);
                    //displayText("LIGHT TAXI                  " + s1.LIGHT_TAXI);

                    //displayText("INNER MARKER                " + s1.INNER_MARKER);
                    //displayText("MIDDLE MARKER               " + s1.MIDDLE_MARKER);
                    //displayText("OUTER MARKER                " + s1.OUTER_MARKER);
                    //displayText("GEAR CENTRE POSITION        " + s1.GEAR_CENTER_POSITION);
                    //displayText("GEAR LEFT POSITION          " + s1.GEAR_LEFT_POSITION);
                    //displayText("GEAR RIGHT POSITION         " + s1.GEAR_RIGHT_POSITION);

                    //displayText("Zulu Time 2        " + s1.zulu_time);

                    //UDP_Playload = "D,C1A:" + s1.com1ActiveFrequency.ToString("F3");
                    //UDP_Playload = UDP_Playload + ",longitude:" + s1.longitude.ToString();
                    //UDP_Playload = UDP_Playload + ",altitude:" + s1.altitude.ToString();
                    //UDP_Playload = UDP_Playload + ",airspeed:" + s1.airspeed.ToString();
                    //UDP_Playload = UDP_Playload + ",zulutime:" + s1.zulu_time.ToString();
                    //UDP_Playload = UDP_Playload + ",timezoneoffset:" + s1.time_zone_offset.ToString();
                    //UDP_Playload = UDP_Playload + ",trueheading:" + s1.plane_heading_degrees_true.ToString();
                    //UDP_Playload = UDP_Playload + ",magheading:" + s1.plane_heading_degrees_magnetic.ToString();

                    //Output_Payload = "TRAILING_EDGE_FLAPS_LEFT_ANGLE:" + Math.Floor(s1.TRAILING_EDGE_FLAPS_LEFT_ANGLE * 100);
                    //Output_Payload = Output_Payload + ",SPOILERS_LEFT_POSITION:" + Math.Floor(s1.SPOILERS_LEFT_POSITION * 100);
                    //Output_Payload = Output_Payload + ",GEAR_CENTER_POSITION:" + Math.Floor(s1.GEAR_CENTER_POSITION/100);
                    //Output_Payload = Output_Payload + ",GEAR_LEFT_POSITION:" + Math.Floor(s1.GEAR_LEFT_POSITION/100);
                    //Output_Payload = Output_Payload + ",GEAR_RIGHT_POSITION:" + Math.Floor(s1.GEAR_RIGHT_POSITION/100);
                    //Output_Payload = Output_Payload + ",BRAKE_PARKING_INDICATOR:" + Math.Floor(s1.BRAKE_PARKING_INDICATOR);
                    */
                    break;

                default:
                    displayText("Unknown request ID: " + data.dwRequestID);
                    break;
            }



        }

        private void SendEvent(EVENTS EVENT_SEND_SEND, uint PARAMETER_TO_SEND)
        {
            // Not sure if parameter should be signed int or unsigned int
            if (simconnect != null)
            {
                simconnect.TransmitClientEvent(
                    SimConnect.SIMCONNECT_OBJECT_ID_USER,
                    EVENT_SEND_SEND,
                    PARAMETER_TO_SEND,
                    GROUP_ID.GROUP0,
                    SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                );
            }
        }


        protected override void DefWndProc(ref Message m)
        {
            if (m.Msg == WM_USER_SIMCONNECT)
            {
                if (simconnect != null)
                {
                    simconnect.ReceiveMessage();
                }
            }
            else
            {
                base.DefWndProc(ref m);
            }
        }

        private void setButtons(bool bConnect, bool bGet, bool bDisconnect)
        {
            buttonConnect.Enabled = bConnect;
            buttonRequestData.Enabled = bGet;
            buttonDisconnect.Enabled = bDisconnect;
        }
        private void StartSimConnect()
        {

            if (simconnect == null)
            {
                try
                {
                    // the constructor is similar to SimConnect_Open in the native API 
                    simconnect = new SimConnect("Managed Data Request", this.Handle, WM_USER_SIMCONNECT, null, 0);

                    setButtons(false, true, true);

                    initDataRequest();

                    Setup_DataRequests();

                }
                catch (COMException ex)
                {
                    displayText("Unable to connect to MSFS:\n\n" + ex.Message);
                    // Failed to connect - start timer to try again in 5 seconds
                    timer1.Interval = 5000;
                    timer1.Enabled = true;
                }
            }
            else
            {
                displayText("Error - try again");
                closeConnection();

                setButtons(true, false, false);

                // Failed to connect - start timer to try again in 5 seconds
                timer1.Interval = 5000;
                timer1.Enabled = true;
            }
        }




        private void UpdateRadios(string SIMCONNECT_COMMAND)
        {
            if (simconnect != null)
            {


                if (SIMCONNECT_COMMAND.Contains("COM1_RADIO_SWAP") == true)
                {
                    SendEvent(EVENTS.KEY_COM1_RADIO_SWAP, 0);
                }
                else if (SIMCONNECT_COMMAND.Contains("COM2_RADIO_SWAP") == true)
                {
                    SendEvent(EVENTS.KEY_COM2_RADIO_SWAP, 0);

                }
                else if (SIMCONNECT_COMMAND.Contains("AVIONICS_MASTER_SET") == true)

                {
                    //SendEvent(EVENTS.AVIONICS_MASTER_1_SET, 1);

                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_AVIONICS_MASTER_SET,
                        1,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                     );

                }
                else if (SIMCONNECT_COMMAND.Contains("MASTER_BATTERY_ON") == true)
                {

                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_MASTER_BATTERY_SET,
                        1,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
                else if (SIMCONNECT_COMMAND.Contains("MASTER_BATTERY_OFF") == true)
                {

                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_MASTER_BATTERY_SET,
                        0,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
                else if (SIMCONNECT_COMMAND.Contains("ALTERNATOR_ON") == true)
                {
                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_ALTERNATOR_SET,
                        1,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
                else if (SIMCONNECT_COMMAND.Contains("ALTERNATOR_OFF") == true)
                {
                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_ALTERNATOR_SET,
                        0,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
                else if (SIMCONNECT_COMMAND.Contains("AVIONICS_MASTER_SET") == true)
                {
                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_AVIONICS_MASTER_SET,
                        0,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
                else
                {


                    const float GOTO_FREQUENCY = 121.50F;
                    // Check to see if the received string is a number
                    float FLT_COMM1_STANDBY_FREQUENCY;

                    bool isNumeric = float.TryParse(SIMCONNECT_COMMAND, out FLT_COMM1_STANDBY_FREQUENCY);

                    if (!isNumeric)
                    {
                        listBoxLogs.Items.Add($"'{SIMCONNECT_COMMAND}' is not a valid float. Value: {SIMCONNECT_COMMAND}");
                        SIMCONNECT_COMMAND = "121.50";
                        FLT_COMM1_STANDBY_FREQUENCY = GOTO_FREQUENCY;

                    }

                    // Should have a numberic value at this point
                    // Now check bounds
                    if (FLT_COMM1_STANDBY_FREQUENCY < 118.00 || FLT_COMM1_STANDBY_FREQUENCY > 136.975)
                    {
                        FLT_COMM1_STANDBY_FREQUENCY = GOTO_FREQUENCY;
                    }



                    uint frequencyHz = 118000000; // 118 MHz in Hz
                    frequencyHz = (uint)(FLT_COMM1_STANDBY_FREQUENCY * 1000000);
                    simconnect.TransmitClientEvent(
                        SimConnect.SIMCONNECT_OBJECT_ID_USER,
                        EVENTS.KEY_COM_STBY_RADIO_SET_HZ,
                        frequencyHz,
                        GROUP_ID.GROUP0,
                        SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                    );
                }
            }

        }

        public frmMain()
        {
            InitializeComponent();

            StartListener();


            setButtons(true, false, false);
            udpClient.Connect("172.16.1.101", 13136);
            frontPanelClient.Connect("172.16.1.102", 13136);
            OutputClient.Connect("172.16.1.2", 26028);
            RadioTimeLastPacketSent = DateTime.Now;

            // Initialise the arrays
            // Set them the zero position (not min position as ww have some like VSI
            for (int i = 0; i < ServoPosition.Length; i++)
            {
                ServoPosition[i] = ServZeroPosition[i];
            }
        }

        private void frmMain_Load(object sender, EventArgs e)
        {

        }

        private void frmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            closeConnection();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            StartSimConnect();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            closeConnection();
            setButtons(true, false, false);
        }

        private void buttonRequestData_Click(object sender, EventArgs e)
        {
            // The following call returns identical information to: 
            // simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.ONCE); 

            simconnect.RequestDataOnSimObjectType(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, 0, SIMCONNECT_SIMOBJECT_TYPE.USER);
            // simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 0, 0);


            displayText("Request sent...");
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            StartSimConnect();
        }
    }
}
