using System;
using Microsoft.FlightSimulator.SimConnect;
using System.Runtime.InteropServices;

namespace SimConnectVersionDemo
{
    class Program
    {
        static SimConnect simConnect;
        static bool isConnected = false;
        const int WM_USER_SIMCONNECT = 0x0402;


        /*
        // --- Data Definitions and Request IDs ---
        // Using enums for clarity and type safety for SimConnect IDs
        enum DEFINITIONS
        {
            Airspeed, AttitudePitch, AttitudeBank, Altimeter, RadarAltimeter, VerticalVelocity,
            TurnRate, SlipSkidAngle, HSIHeading, HSINAV1CDI, HSINAV1Glideslope,
            ADFActiveFreq, ADFBearing, N1Speed, EGT, N2Speed, RotorRPM, EngineTorque,
            EngineOilTemp, EngineOilPressure, TransmissionOilTemp, TransmissionOilPressure,
            FuelLevel, FuelPressure, ElectricalLoad
        }

        enum REQUESTS
        {
            // Using the same enum for Request IDs for simplicity, as they map 1:1 to definitions
            Airspeed = DEFINITIONS.Airspeed, AttitudePitch = DEFINITIONS.AttitudePitch,
            AttitudeBank = DEFINITIONS.AttitudeBank, Altimeter = DEFINITIONS.Altimeter,
            RadarAltimeter = DEFINITIONS.RadarAltimeter, VerticalVelocity = DEFINITIONS.VerticalVelocity,
            TurnRate = DEFINITIONS.TurnRate, SlipSkidAngle = DEFINITIONS.SlipSkidAngle,
            HSIHeading = DEFINITIONS.HSIHeading, HSINAV1CDI = DEFINITIONS.HSINAV1CDI,
            HSINAV1Glideslope = DEFINITIONS.HSINAV1Glideslope, ADFActiveFreq = DEFINITIONS.ADFActiveFreq,
            ADFBearing = DEFINITIONS.ADFBearing, N1Speed = DEFINITIONS.N1Speed, EGT = DEFINITIONS.EGT,
            N2Speed = DEFINITIONS.N2Speed, RotorRPM = DEFINITIONS.RotorRPM, EngineTorque = DEFINITIONS.EngineTorque,
            EngineOilTemp = DEFINITIONS.EngineOilTemp, EngineOilPressure = DEFINITIONS.EngineOilPressure,
            TransmissionOilTemp = DEFINITIONS.TransmissionOilTemp, TransmissionOilPressure = DEFINITIONS.TransmissionOilPressure,
            FuelLevel = DEFINITIONS.FuelLevel, FuelPressure = DEFINITIONS.FuelPressure,
            ElectricalLoad = DEFINITIONS.ElectricalLoad
        }

        // --- SimVar Mapping (Friendly Name to SimConnect Name/Unit) ---
        private static readonly Dictionary<DEFINITIONS, (string SimVarName, string Unit, string FriendlyName)> SimVarMap =
            new Dictionary<DEFINITIONS, (string SimVarName, string Unit, string FriendlyName)>
            {
                { DEFINITIONS.Airspeed, ("AIRSPEED INDICATED", "Knots", "Airspeed Indicator") },
                { DEFINITIONS.AttitudePitch, ("AIRCRAFT PITCH", "Degrees", "Attitude Indicator Pitch") },
                { DEFINITIONS.AttitudeBank, ("AIRCRAFT BANK", "Degrees", "Attitude Indicator Bank") },
                { DEFINITIONS.Altimeter, ("INDICATED ALTITUDE", "Feet", "Altimeter") },
                { DEFINITIONS.RadarAltimeter, ("RADIO ALTITUDE", "Feet", "Radar Altimeter") },
                { DEFINITIONS.VerticalVelocity, ("VERTICAL SPEED", "Feet per second", "Vertical Velocity Indicator") },
                { DEFINITIONS.TurnRate, ("TURN INDICATOR", "Degrees per second", "Turn Rate") },
                { DEFINITIONS.SlipSkidAngle, ("SIDESLIP ANGLE", "Degrees", "Slip/Skid Angle") },
                { DEFINITIONS.HSIHeading, ("HEADING INDICATOR", "Degrees", "HSI Heading") },
                { DEFINITIONS.HSINAV1CDI, ("NAV1 CDI", "Number", "HSI NAV1 CDI") },
                { DEFINITIONS.HSINAV1Glideslope, ("NAV1 GLIDE SLOPE", "Number", "HSI NAV1 Glideslope") },
                { DEFINITIONS.ADFActiveFreq, ("ADF ACTIVE FREQUENCY:1", "Hz", "ADF Active Freq") },
                { DEFINITIONS.ADFBearing, ("ADF BEARING:1", "Degrees", "ADF Bearing") },
                { DEFINITIONS.N1Speed, ("TURB ENG N1:1", "Percent", "N1 Speed") },
                { DEFINITIONS.EGT, ("GENERAL ENG EXHAUST GAS TEMPERATURE:1", "Rankine", "EGT") },
                { DEFINITIONS.N2Speed, ("TURB ENG N2:1", "Percent", "N2 Speed") },
                { DEFINITIONS.RotorRPM, ("ROTOR RPM", "RPM", "Rotor RPM") },
                { DEFINITIONS.EngineTorque, ("GENERAL ENG TORQUE:1", "Foot pounds", "Engine Torque") },
                { DEFINITIONS.EngineOilTemp, ("GENERAL ENG OIL TEMPERATURE:1", "Rankine", "Engine Oil Temp") },
                { DEFINITIONS.EngineOilPressure, ("GENERAL ENG OIL PRESSURE:1", "Psi", "Engine Oil Pressure") },
                { DEFINITIONS.TransmissionOilTemp, ("TRANSMISSION OIL TEMPERATURE:1", "Rankine", "Transmission Oil Temp") },
                { DEFINITIONS.TransmissionOilPressure, ("TRANSMISSION OIL PRESSURE:1", "Psi", "Transmission Oil Pressure") },
                { DEFINITIONS.FuelLevel, ("FUEL TOTAL QUANTITY", "Gallons", "Fuel Level") },
                { DEFINITIONS.FuelPressure, ("GENERAL ENG FUEL PRESSURE:1", "Psi", "Fuel Pressure") },
                { DEFINITIONS.ElectricalLoad, ("ELECTRICAL TOTAL LOAD", "Amps", "Electrical Load") }
            };
        */
        // Dictionary to store the latest values of each SimVar, accessible across threads
        private Dictionary<string, double> currentSimVarValues = new Dictionary<string, double>();
        private object lockObject = new object(); // For thread-safe access to currentSimVarValues


        static void Main(string[] args)
        {
            try
            {
                simConnect = new SimConnect(
                    "Version Demo",
                    IntPtr.Zero,
                    WM_USER_SIMCONNECT,
                    null,
                    0);

                simConnect.OnRecvOpen += SimConnect_OnRecvOpen;
                simConnect.OnRecvQuit += SimConnect_OnRecvQuit;
                simConnect.OnRecvException += SimConnect_OnRecvException;
                simConnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simconnect_OnRecvSimobjectData);


                Console.WriteLine("Connected to Flight Simulator.");
                Console.WriteLine("Waiting for version info...");
                Console.WriteLine("Press Enter to exit.");

                while (!Console.KeyAvailable)
                {
                    simConnect.ReceiveMessage();
                    System.Threading.Thread.Sleep(50);
                }
            }
            catch (COMException ex)
            {
                Console.WriteLine("Unable to connect to Flight Simulator.");
                Console.WriteLine(ex.Message);

            }
            finally
            {
                simConnect?.Dispose();
            }
        }

        private static void SimConnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            Console.WriteLine("=== Flight Simulator Version Info ===");
            Console.WriteLine($"Application Name: {data.szApplicationName}");
            Console.WriteLine($"Application Version: {data.dwApplicationVersionMajor}.{data.dwApplicationVersionMinor}");
            Console.WriteLine($"SimConnect Version: {data.dwSimConnectVersionMajor}.{data.dwSimConnectVersionMinor}");
            Console.WriteLine("=====================================");
            isConnected = true;
            SetupDataDefinitionsAndRequests(); // Set up definitions and requests after connection

        }

        private static void SimConnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            Console.WriteLine("Flight Simulator has exited.");
        }

        private static void SimConnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            Console.WriteLine("Not Happy Jan");
            Console.WriteLine($"SimConnect Exception: {data.dwException}");
        }

        // this is the structure that will store the data received from the game. note that the number of properties here have to be exactly the same number
        // of AddToDataDefinition(...) calls we are registering, and the type has to match the SIMCONNECT_DATATYPE type.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        private  struct SimVars
        {
            public float Altitude;                         // INDICATED ALTITUDE (feet)
            public float KohlsmanSettingHg;                // KOHLSMAN SETTING HG (inHG)
        }

        // we need an enum to register the data structure above. note that if you want to store them in separate structures, you'll need to register with a separate enum value here.
        private  enum RequestType
        {
            PerFrameData,
        }


        enum DEFINITIONS
        {
            Struct1,
        }

        enum DATA_REQUESTS
        {
            Request1,
        }

        // 2. Define the structure to hold the data
        // Ensure the order matches AddToDataDefinition calls
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        
        
        struct FlightData
        {
            public double latitude;
            public double longitude;
            public double altitude;
            public double heading;
        }

        private static void RequestData()
        {
            // 5. Request the data

        }

        private static void SetupDataDefinitionsAndRequests()
        {
            /*
            foreach (DEFINITIONS defId in Enum.GetValues(typeof(DEFINITIONS)))
            {
               
                 * Console.WriteLine("Looping through");
                var (simVarName, unit, friendlyName) = SimVarMap[defId];

                // Add data definition: This tells SimConnect what variable to read.
                simConnect.AddToDataDefinition(
                    defId, simVarName, unit, SIMCONNECT_DATATYPE.FLOAT64, 0.0f, 2);

                // Request data on SimObject: This tells SimConnect to send the data.
                // We request data every second (SIMCONNECT_PERIOD.SECOND) but with an update rate of 10.
                // This means SimConnect will send 10 updates per second, effectively a 100ms interval.
                simConnect.RequestDataOnSimObject(
                    (REQUESTS)defId, // Request ID
                    defId,           // Definition ID
                    SimConnect.SIMCONNECT_OBJECT_ID_USER, // Request for the user's aircraft
                    SIMCONNECT_PERIOD.SECOND, // Period for updates
                    SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, // Default flags
                    0, // No index for this type of request
                    10, // Update rate: 10 times per second (100ms interval)
                    0 // No index for this type of request
                );
                Console.WriteLine(  
                
            }
            */

            simConnect.RegisterDataDefineStruct<FlightData>(DEFINITIONS.Struct1);

            // 4. Add data to the definition (Name, Units, Type)
            // Syntax: AddToDataDefinition(DefineID, SimVarName, Units, DataType, epsilon, id)
            simConnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Latitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            simConnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Longitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            simConnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            simConnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

            /*
            simConnect.AddToDataDefinition(RequestType.PerFrameData, "INDICATED ALTITUDE", "feet", SIMCONNECT_DATATYPE.FLOAT32, 0, SimConnect.SIMCONNECT_UNUSED);
            simConnect.AddToDataDefinition(RequestType.PerFrameData, "KOHLSMAN SETTING HG", "inHG", SIMCONNECT_DATATYPE.FLOAT32, 0, SimConnect.SIMCONNECT_UNUSED);

            // register our SimVar structure with the PerFrameData enum
            simConnect.RegisterDataDefineStruct<SimVars>(RequestType.PerFrameData);

            // request data from sim
            simConnect.RequestDataOnSimObject(RequestType.PerFrameData, RequestType.PerFrameData, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SECOND, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 10, 0);
            */
            simConnect.RequestDataOnSimObject(
                DATA_REQUESTS.Request1,
                DEFINITIONS.Struct1,
                0, // User aircraft
                SIMCONNECT_PERIOD.SECOND,
                SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT,
                0, 10, 1);

            Console.WriteLine("SimConnect data definitions and requests set up.");
        }


        private static void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {
            Console.WriteLine("Data received");
            if (data.dwRequestID == (uint)DATA_REQUESTS.Request1)
            {
                FlightData s1 = (FlightData)data.dwData[0];
                Console.WriteLine($"Lat: {s1.latitude}, Lon: {s1.longitude}");
            }
        }

        /*
        private static void simConnect_OnRecvSimobjectDataBytype(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE data)
        {
            Console.WriteLine("Data Received in simConnect_OnRecvSimobjectDataBytype");
            // This event fires for each requested SimVar when its value is updated.
            // The dwRequestID tells us which SimVar's data we're receiving.
            /*
            if (data.dwRequestID >= (uint)REQUESTS.Airspeed && data.dwRequestID <= (uint)REQUESTS.ElectricalLoad)
            {
                DEFINITIONS defId = (DEFINITIONS)data.dwRequestID;
                var (simVarName, unit, friendlyName) = SimVarMap[defId];

                // Data is typically in dwData[0] for single values requested with FLOAT64 type.
                double value = (double)data.dwData[0];

                // Store the latest value in a thread-safe manner
                
                lock (lockObject)
                {
                    currentSimVarValues[friendlyName] = Math.Round(value, 2);
                }
                

                // Optional: print to console for debugging each individual update
                Console.WriteLine($"{friendlyName}: {value:F2} {unit}"); 
            }
       
        }
        */

    }
}
