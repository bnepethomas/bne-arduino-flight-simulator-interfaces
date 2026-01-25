using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Threading;

using Microsoft.FlightSimulator.SimConnect; // Make sure to add reference to this DLL

namespace SimConnectUdpSender
{
    class Program
    {
        // --- SimConnect Configuration ---
        private SimConnect? simConnect = null;
        private bool isConnected = false;
        private Timer reconnectTimer; // Timer to periodically attempt reconnection

        // --- UDP Configuration ---
        private const string UdpIp = "127.0.0.1"; // Target IP address (e.g., localhost)
        private const int UdpPort = 5005;       // Target UDP port
        private UdpClient udpClient;

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

        // Dictionary to store the latest values of each SimVar, accessible across threads
        private Dictionary<string, double> currentSimVarValues = new Dictionary<string, double>();
        private object lockObject = new object(); // For thread-safe access to currentSimVarValues

        // Timer to trigger sending UDP packets
        private Timer udpSendTimer;
        private const int UdpSendIntervalMs = 100; // 100ms interval for sending UDP packets

        static void Main(string[] args)
        {
            Program p = new Program();
            p.Run();
        }

        public Program()
        {
            // Initialize UDP client
            udpClient = new UdpClient();
            try
            {
                udpClient.Connect(UdpIp, UdpPort);
                Console.WriteLine($"UDP client connected to {UdpIp}:{UdpPort}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error connecting UDP client: {ex.Message}");
                // In a real application, you might want to handle this more gracefully, e.g., retry.
                // For now, we'll just log and continue, UDP sends will fail silently.
            }

            // Initialize reconnect timer: starts immediately, repeats every 5 seconds
            reconnectTimer = new Timer(TryConnectSimConnect, null, 0, 5000);

            // Initialize UDP send timer: starts in a disabled state (Timeout.Infinite)
            // It will be enabled once SimConnect connection is established.
            udpSendTimer = new Timer(SendUdpPacket, null, Timeout.Infinite, UdpSendIntervalMs);
        }

        public void Run()
        {
            Console.WriteLine("SimConnect UDP Sender started. Press any key to exit.");
            Console.ReadKey(); // Keep the console application running until a key is pressed
            Cleanup(); // Clean up resources before exiting
        }

        private void TryConnectSimConnect(object state)
        {
            if (isConnected) return; // Already connected

            Console.WriteLine("Attempting to connect to SimConnect...");
            try
            {
                // Dispose previous SimConnect instance if it exists to ensure a clean reconnect
                if (simConnect != null)
                {
                    simConnect.Dispose();
                    simConnect = default!;
                }

                // Create a new SimConnect instance
                simConnect = new SimConnect("SimConnect UDP Sender", IntPtr.Zero, 0, null, 0);

                // Register event handlers
                simConnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(simConnect_OnRecvOpen);
                simConnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(simConnect_OnRecvQuit);
                simConnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(simConnect_OnRecvException);
                simConnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simConnect_OnRecvSimobjectDataBytype);

                // If connection is successful, SetupDataDefinitionsAndRequests will be called in OnRecvOpen
                // We don't set isConnected to true here directly, as OnRecvOpen confirms the connection.
            }
            catch (Exception ex)
            {
                Console.WriteLine($"SimConnect connection failed: {ex.Message}");
                isConnected = false;
                udpSendTimer.Change(Timeout.Infinite, Timeout.Infinite); // Ensure UDP sending is stopped
            }
        }

        private void SetupDataDefinitionsAndRequests()
        {
            foreach (DEFINITIONS defId in Enum.GetValues(typeof(DEFINITIONS)))
            {
                var (simVarName, unit, friendlyName) = SimVarMap[defId];

                // Add data definition: This tells SimConnect what variable to read.
                simConnect.AddToDataDefinition(
                    defId, simVarName, unit, SIMCONNECT_DATATYPE.FLOAT64);

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
            }
            Console.WriteLine("SimConnect data definitions and requests set up.");
        }

        // --- SimConnect Event Handlers ---
        private void simConnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            Console.WriteLine("SimConnect: Connected to " + data.szApplicationName);
            isConnected = true;
            SetupDataDefinitionsAndRequests(); // Set up definitions and requests after connection
            udpSendTimer.Change(0, UdpSendIntervalMs); // Start UDP sending immediately, then every 100ms
        }

        private void simConnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            Console.WriteLine("SimConnect: Disconnected from simulator.");
            isConnected = false;
            udpSendTimer.Change(Timeout.Infinite, Timeout.Infinite); // Stop UDP sending
            simConnect.Dispose();
            simConnect = default!;
        }

        private void simConnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            Console.WriteLine($"SimConnect Exception: {data.dwException} (SendID: {data.dwSendID}, Index: {data.dwIndex})");
            // Depending on the exception, you might want to force a disconnect and retry.
            // For now, we'll just log it.
        }

        private void simConnect_OnRecvSimobjectDataBytype(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE data)
        {
            // This event fires for each requested SimVar when its value is updated.
            // The dwRequestID tells us which SimVar's data we're receiving.
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
                // Console.WriteLine($"{friendlyName}: {value:F2} {unit}"); 
            }
        }

        // --- UDP Sending Logic ---
        private void SendUdpPacket(object state)
        {
            if (!isConnected || udpClient == null || !udpClient.Client.Connected) return; // Only send if connected to SimConnect and UDP client is ready

            Dictionary<string, double> dataToSend;
            // Create a snapshot of the current SimVar values for thread safety
            lock (lockObject)
            {
                dataToSend = new Dictionary<string, double>(currentSimVarValues);
            }

            if (dataToSend.Count == 0)
            {
                // No data collected yet, or SimVars are not available.
                // This can happen right after connection before the first data arrives.
                return;
            }

            try
            {
                // Serialize the dictionary to a JSON string
                string jsonString = JsonSerializer.Serialize(dataToSend);
                byte[] sendBytes = Encoding.UTF8.GetBytes(jsonString);

                // Send the JSON string over UDP
                udpClient.Send(sendBytes, sendBytes.Length);
                // Optional: print sent data to console
                // Console.WriteLine($"Sent UDP: {jsonString}"); 
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending UDP packet: {ex.Message}");
                // If UDP client disconnects or has issues, you might want to retry connection or log more details.
            }
        }

        private void Cleanup()
        {
            reconnectTimer?.Dispose(); // Stop and dispose reconnect timer
            udpSendTimer?.Dispose();   // Stop and dispose UDP send timer

            if (simConnect != null)
            {
                simConnect.Dispose(); // Dispose SimConnect instance
                simConnect = null!;
                Console.WriteLine("SimConnect instance disposed.");
            }
            if (udpClient != null)
            {
                udpClient.Close(); // Close UDP client socket
                udpClient = null!;
                Console.WriteLine("UDP client closed.");
            }
        }
    }
}