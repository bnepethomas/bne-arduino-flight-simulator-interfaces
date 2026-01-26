using Microsoft.FlightSimulator.SimConnect;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Threading;

namespace HelloMSFS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const uint WM_USER_SIMCONNECT = 0x0402;                 //
        private const string MSFS_PROCESS_NAME = "FlightSimulator";     // this is MSFS's process name
        private const string PLUGIN_NAME = "Hello MSFS";                // your plugin's name

        private const int SIMCONNECT_TIMER_INTERVAL_MS = 50;            // how often to poll data from the game
        private DispatcherTimer simConnectDispatcherTimer;              // the DispatcherTimer object used to poll data from the game

        private SimConnect simconnect;                                  // the SimConnect SDK object used to subscribe and interact with the game
        private SimVars simvars;                                        // the object we'll store our new data from the game

        public MainWindow()
        {
            InitializeComponent();

            InitializeTimers();
        }

        // this is the structure that will store the data received from the game. note that the number of properties here have to be exactly the same number
        // of AddToDataDefinition(...) calls we are registering, and the type has to match the SIMCONNECT_DATATYPE type.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        private struct SimVars
        {
            public float Altitude;                         // INDICATED ALTITUDE (feet)
            public float KohlsmanSettingHg;                // KOHLSMAN SETTING HG (inHG)
        }

        // we need an enum to register the data structure above. note that if you want to store them in separate structures, you'll need to register with a separate enum value here.
        private enum RequestType
        {
            PerFrameData,
        }

        // initializing our DispatcherTimers
        private void InitializeTimers()
        {
            simConnectDispatcherTimer = new DispatcherTimer() { Interval = new TimeSpan(0, 0, 0, 0, SIMCONNECT_TIMER_INTERVAL_MS) };
            simConnectDispatcherTimer.Tick += SimConnectTimer_Tick;
        }

        // function that will be called by the DispatcherTimer to receive new data from the game
        private void SimConnectTimer_Tick(object? sender, EventArgs e)
        {
            try
            {
                simconnect?.ReceiveMessage();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Exception in {nameof(SimConnectTimer_Tick)}: {ex.Message}");
            }
        }

        // called whenever we receive a new set of SimVar data
        private void Simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {
            switch ((RequestType)data.dwRequestID)
            {
                case RequestType.PerFrameData:
                    simvars = (SimVars)data.dwData[0];

                    Debug.WriteLine($"Received: Altitude: {simvars.Altitude}, Barometer: {simvars.KohlsmanSettingHg}");

                    altimeterTextBox.Text = $"{simvars.Altitude:0.00}";
                    barometerTextBox.Text = $"{simvars.KohlsmanSettingHg:0.00}";

                    break;

                default:
                    Debug.WriteLine($"Unsupported Request Type: {data.dwRequestID}");
                    break;
            }
        }

        // called when the connection throws an exception so we can log it
        private void Simconnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            Debug.WriteLine($"Exception received: {data}");
        }

        // called when the game exits
        private void Simconnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            simConnectDispatcherTimer.Stop();
        }

        // called when we connect to the game for the first time
        private void Simconnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            // the framework is flexible enough to let you specify the units. check the documentation, for example for altitude you could set this to "feet", "meters", etc.
            simconnect.AddToDataDefinition(RequestType.PerFrameData, "INDICATED ALTITUDE", "feet", SIMCONNECT_DATATYPE.FLOAT32, 0, SimConnect.SIMCONNECT_UNUSED);
            simconnect.AddToDataDefinition(RequestType.PerFrameData, "KOHLSMAN SETTING HG", "inHG", SIMCONNECT_DATATYPE.FLOAT32, 0, SimConnect.SIMCONNECT_UNUSED);

            // register our SimVar structure with the PerFrameData enum
            simconnect.RegisterDataDefineStruct<SimVars>(RequestType.PerFrameData);

            // request data from sim
            simconnect.RequestDataOnSimObject(RequestType.PerFrameData, RequestType.PerFrameData, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 0, 0);
        }

        // Connect button event handler
        private void connectButton_Click(object sender, RoutedEventArgs e)
        {
            if (Process.GetProcessesByName(MSFS_PROCESS_NAME).Any())
            {
                try
                {
                    var handle = Process.GetCurrentProcess().MainWindowHandle;

                    simconnect = new SimConnect(PLUGIN_NAME, handle, WM_USER_SIMCONNECT, null, 0);

                    // register a callback for each handler:
                    // called when you connect to the sim for the first time
                    simconnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(Simconnect_OnRecvOpen);
                    // called when the sim exits
                    simconnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(Simconnect_OnRecvQuit);
                    // called when there's an exception when sending us the data
                    simconnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(Simconnect_OnRecvException);
                    // called every time we are sent new simvar data
                    simconnect.OnRecvSimobjectData += new SimConnect.RecvSimobjectDataEventHandler(Simconnect_OnRecvSimobjectData);

                    // start out DispatcherTimer to start polling from the game
                    simConnectDispatcherTimer.Start();

                    // toggle the connect/disconnect buttons
                    connectButton.IsEnabled = false;
                    disconnectButton.IsEnabled = true;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"{ex.Message}");
                }
            }
        }

        // Disconnect button event handler
        private void disconnectButton_Click(object sender, RoutedEventArgs e)
        {
            // stop our DispatcherTimer
            simConnectDispatcherTimer.Stop();

            // toggle the connect/disconnect buttons
            connectButton.IsEnabled = true;
            disconnectButton.IsEnabled = false;
        }
    }
}