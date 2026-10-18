using Microsoft.FlightSimulator.SimConnect;
using System.Globalization;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace MSFSWinformsAutoCS
{
    // Microsoft Flight Simulator (2020 / 2024) bridge for the Jet Ranger panel boards.
    // Same shape as FSUIPCWinformsAutoCS: a 1 s connection timer looks for the simulator, a
    // 100 ms main timer turns the latest simulator values into one real-unit packet per board,
    // and a listener on UDP 27001 turns the radio board's commands into simulator events.
    // The data comes from SimConnect: SimVars are streamed into _latest as they arrive and the
    // main timer samples it, so the send rate does not depend on the simulator frame rate.
    public partial class Form1 : Form
    {
        // ----- UDP ------------------------------------------------------------------------
        private UdpClient? udpServer;
        private const int listenPort = 27001;
        private const int boardPort = 13136;

        private readonly UdpClient radioClient = new UdpClient();         // 172.16.1.101 Radio Controller
        private readonly UdpClient servoClient = new UdpClient();         // 172.16.1.102 Servo Controller
        // Aircraft information for the Lowrance AirMap GPS interface (LowranceAirMapGps): sent twice a second.
        // Change GpsHost if the GPS interface runs on another PC.
        private readonly UdpClient gpsClient = new UdpClient();
        private const string GpsHost = "127.0.0.1";
        private const int GpsPort = 27002;
        private DateTime gpsTimeLastPacketSent = DateTime.MinValue;
        private readonly UdpClient stepperClient = new UdpClient();       // 172.16.1.105 Stepper Controller
        private readonly UdpClient dualStepperClient = new UdpClient();   // 172.16.1.106 OLED Dual Stepper Controller

        // ----- SimConnect -----------------------------------------------------------------
        private const int WM_USER_SIMCONNECT = 0x0402;
        private SimConnect? simconnect = null;

        enum DEFINITIONS { PanelData }
        enum DATA_REQUESTS { PanelData }
        enum GROUP_ID { GROUP0 }

        enum EVENTS
        {
            COM1_RADIO_SWAP,
            COM2_RADIO_SWAP,
            NAV1_RADIO_SWAP,
            NAV2_RADIO_SWAP,
            COM1_STBY_SET_HZ,
            COM2_STBY_SET_HZ,
            NAV1_STBY_SET_HZ,
            NAV2_STBY_SET_HZ,
            AVIONICS_MASTER_SET,
            MASTER_BATTERY_SET,
            ALTERNATOR_SET,
        }

        // The SimVars requested, in the order of the PanelData fields below (after "title").
        // SimConnect fills the structure field by field in this order, so the two lists must
        // stay in step; CheckDefinitionMatchesStruct() verifies the counts at connect time.
        private static readonly (string Name, string Units)[] PanelVars =
        {
            ("PLANE ALTITUDE", "feet"),
            ("AIRSPEED INDICATED", "knots"),
            ("VERTICAL SPEED", "feet per minute"),
            ("PLANE ALT ABOVE GROUND", "feet"),
            ("ATTITUDE INDICATOR PITCH DEGREES", "degrees"),
            ("ATTITUDE INDICATOR BANK DEGREES", "degrees"),
            ("ROTOR RPM PCT:1", "percent"),
            ("GENERAL ENG PCT MAX RPM:1", "percent"),
            ("ENG TORQUE PERCENT:1", "percent"),
            ("TURB ENG MAX TORQUE PERCENT:1", "percent"),
            ("ENG EXHAUST GAS TEMPERATURE:1", "celsius"),
            ("GENERAL ENG THROTTLE LEVER POSITION:1", "percent"),
            ("ELECTRICAL TOTAL LOAD AMPS", "amps"),
            ("TURB ENG ITT:1", "celsius"),
            ("ENG OIL TEMPERATURE:1", "celsius"),
            ("FUEL TOTAL QUANTITY", "gallons"),
            ("TURB ENG CORRECTED N1:1", "percent"),
            ("ENG OIL PRESSURE:1", "psi"),
            ("ENG TRANSMISSION PRESSURE:1", "psi"),
            ("ENG TRANSMISSION TEMPERATURE:1", "celsius"),
            ("GENERAL ENG FUEL PRESSURE:1", "psi"),
            ("ELECTRICAL MAIN BUS VOLTAGE", "volts"),
            ("ELECTRICAL AVIONICS BUS VOLTAGE", "volts"),
            ("AVIONICS MASTER SWITCH", "bool"),
            ("AMBIENT TEMPERATURE", "celsius"),
            ("KOHLSMAN SETTING MB", "millibars"),
            ("ZULU TIME", "seconds"),
            ("COM ACTIVE FREQUENCY:1", "MHz"),
            ("COM STANDBY FREQUENCY:1", "MHz"),
            ("COM ACTIVE FREQUENCY:2", "MHz"),
            ("COM STANDBY FREQUENCY:2", "MHz"),
            ("NAV ACTIVE FREQUENCY:1", "MHz"),
            ("NAV STANDBY FREQUENCY:1", "MHz"),
            ("NAV ACTIVE FREQUENCY:2", "MHz"),
            ("NAV STANDBY FREQUENCY:2", "MHz"),
            // Aircraft-specific L:vars (global local variables). The FlyInside B206 does not feed the
            // standard rotor / engine speed SimVars (they read 0 and a slowly decaying idle); its
            // cockpit gauges use these. On an aircraft without them they read 0 and the standard
            // SimVars are used instead (see ToSnapshot).
            ("L:Aircraft.Rotor.RPM.Percent", "number"),
            ("L:Aircraft.Engine.1.Turbine.N2.Smooth", "number"),
            // Position and true heading for the Lowrance AirMap GPS interface (see Payloads.Gps).
            ("PLANE LATITUDE", "degrees"),
            ("PLANE LONGITUDE", "degrees"),
            ("PLANE HEADING DEGREES TRUE", "degrees"),
            // The simulator's UTC date (the time of day is ZULU TIME above), for the GPS time and date.
            ("ZULU YEAR", "number"),
            ("ZULU MONTH OF YEAR", "number"),
            ("ZULU DAY OF MONTH", "number"),
        };

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct PanelData
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string title;
            public double PLANE_ALTITUDE;
            public double AIRSPEED_INDICATED;
            public double VERTICAL_SPEED;
            public double PLANE_ALT_ABOVE_GROUND;
            public double ATTITUDE_INDICATOR_PITCH_DEGREES;
            public double ATTITUDE_INDICATOR_BANK_DEGREES;
            public double ROTOR_RPM_PCT_1;
            public double GENERAL_ENG_PCT_MAX_RPM_1;
            public double ENG_TORQUE_PERCENT_1;
            public double TURB_ENG_MAX_TORQUE_PERCENT_1;
            public double ENG_EXHAUST_GAS_TEMPERATURE_1;
            public double GENERAL_ENG_THROTTLE_LEVER_POSITION_1;
            public double ELECTRICAL_TOTAL_LOAD_AMPS;
            public double TURB_ENG_ITT_1;
            public double ENG_OIL_TEMPERATURE_1;
            public double FUEL_TOTAL_QUANTITY;
            public double TURB_ENG_CORRECTED_N1_1;
            public double ENG_OIL_PRESSURE_1;
            public double ENG_TRANSMISSION_PRESSURE_1;
            public double ENG_TRANSMISSION_TEMPERATURE_1;
            public double GENERAL_ENG_FUEL_PRESSURE_1;
            public double ELECTRICAL_MAIN_BUS_VOLTAGE;
            public double ELECTRICAL_AVIONICS_BUS_VOLTAGE;
            public double AVIONICS_MASTER_SWITCH;
            public double AMBIENT_TEMPERATURE;
            public double KOHLSMAN_SETTING_MB;
            public double ZULU_TIME;
            public double COM_ACTIVE_FREQUENCY_1;
            public double COM_STANDBY_FREQUENCY_1;
            public double COM_ACTIVE_FREQUENCY_2;
            public double COM_STANDBY_FREQUENCY_2;
            public double NAV_ACTIVE_FREQUENCY_1;
            public double NAV_STANDBY_FREQUENCY_1;
            public double NAV_ACTIVE_FREQUENCY_2;
            public double NAV_STANDBY_FREQUENCY_2;
            public double L_AIRCRAFT_ROTOR_RPM_PERCENT;
            public double L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH;
            public double PLANE_LATITUDE;
            public double PLANE_LONGITUDE;
            public double PLANE_HEADING_DEGREES_TRUE;
            public double ZULU_YEAR;
            public double ZULU_MONTH_OF_YEAR;
            public double ZULU_DAY_OF_MONTH;
        }

        // ----- State ----------------------------------------------------------------------
        private PanelData _latest;
        private bool _haveData = false;
        private DateTime _lastDataUtc = DateTime.MinValue;
        private bool _stale = false;
        private string _aircraftTitle = "";
        private bool _suppressAvionicsEvent = false;
        private const double StaleAfterSeconds = 3.0;
        // Gas producer (N1) as a fraction of the turbine N2 L:var, from one cockpit reading: 92 % gas
        // producer with N2 at 100. Only a two-point guess (0 -> 0, 100 -> 92); not checked at other powers.
        private const double FlyInsideGasProducerScale = 0.92;
        // Turbine out temperature (deg C). The cockpit dial rises with power: about 600 at idle (torque 21 %),
        // 650-660 at torque 74 % and about 700 at torque 115 % (read from screenshots), so it is 6 x the turbine N2
        // percent plus about 1.05 deg C per torque percent above the idle torque. Rough fit to three readings.
        private const double FlyInsideTurbineOutTempPerPercent = 6.0;
        private const double FlyInsideTurbineOutTempPerTorque = 1.05;
        private const double FlyInsideIdleTorque = 21.0;
        // Fuel pressure: the SimVar reads on the dial's own 0-3 scale (x10), so psi = 10 x SimVar. One dial reading.
        private const double FlyInsideFuelPressureScale = 10.0;

        // ----- CowaSim Jet Ranger ("206B3 ...") estimates, from dial readings during a collective sweep (2026-10-18) -----
        // Torque: the cockpit dial is a smoothed copy of ENG TORQUE PERCENT:1: about 0.5 x the SimVar (dial 38 at the steady
        // idle value of 83, about 120 at the peak of about 260) with a smoothing time constant of about 2.5 s. Fitted to nine
        // readings, RMS error about 18 points on a signal swinging between 0 and 120.
        private const double CowaSimTorqueScale = 0.42;   // 0.48 from the sweep fit; a steady flight check read the bridge about 15 % high
        private const double CowaSimTorqueSmoothSeconds = 2.5;
        // Turbine out temperature: the dial follows exhaust gas temperature, not ITT (which reads 2,000+): dial = EGT - 200 C
        // within about 30 C over nine readings (EGT 903 reads 700, EGT 727 reads about 525).
        private const double CowaSimTurbineOutTempOffset = 200.0;
        // Electrical load: the LOAD dial sat at about 50 % while the total load was 69.5 A (one reading), so about 139 A is 100 %.
        private const double CowaSimElectricalLoadFullScaleAmps = 139.0;
        // Transmission oil pressure: no variable; the dial read about 45-50 psi at normal rotor speed.
        private const double CowaSimTransmissionPressureNormal = 48.0;
        // Transmission oil temperature: the dial read about 55 C with the engine oil at 60 C shortly after start, and did not
        // follow a slow warm-up, so it is taken as 0.92 x the engine oil temperature (one reading).
        private const double CowaSimTransmissionTempOfOilTemp = 0.92;
        private double _cowaTorque = double.NaN;
        private DateTime _cowaTorqueTime = DateTime.UtcNow;

        private double SmoothCowaSimTorque(double rawTorquePct)
        {
            DateTime now = DateTime.UtcNow;
            double dt = Math.Min(1.0, (now - _cowaTorqueTime).TotalSeconds);
            _cowaTorqueTime = now;
            double target = rawTorquePct * CowaSimTorqueScale;
            if (double.IsNaN(_cowaTorque)) _cowaTorque = target;
            _cowaTorque += (target - _cowaTorque) * (1.0 - Math.Exp(-dt / CowaSimTorqueSmoothSeconds));
            return Math.Clamp(_cowaTorque, 0.0, 120.0);
        }

        // Transmission oil pressure, estimated (no variable exists; the dial reads about 35 psi at normal rotor speed):
        // 35 psi scaled by the rotor speed percent, so it falls to 0 as the rotor stops.
        private const double FlyInsideTransmissionPressureNormal = 35.0;

        private static double FlyInsideTransmissionPressure(double rotorPct)
            => FlyInsideTransmissionPressureNormal * Math.Clamp(rotorPct / 100.0, 0.0, 1.0);

        // Transmission oil temperature, estimated (no variable exists; the dial reads about 10-20 C, cold, with the engine
        // idling): it starts at the outside air temperature and moves slowly toward a target of ambient + 0.8 C per
        // torque percent while the rotor turns (about 33 C at idle, 75 at 74 % torque, 112 at 120 %), and back toward
        // ambient when it stops. Warming takes about 5 minutes (time constant), cooling about 10.
        private const double FlyInsideTransmissionTempPerTorque = 0.8;
        private const double FlyInsideTransmissionWarmSeconds = 300.0;
        private const double FlyInsideTransmissionCoolSeconds = 600.0;
        private double _xmsnTemp = double.NaN;
        private DateTime _xmsnTempTime = DateTime.UtcNow;

        private double UpdateTransmissionTemp(SimSnapshot s)
        {
            DateTime now = DateTime.UtcNow;
            double dt = Math.Min(1.0, (now - _xmsnTempTime).TotalSeconds);
            _xmsnTempTime = now;
            if (double.IsNaN(_xmsnTemp)) _xmsnTemp = s.AmbientCelsius;
            double target = s.RotorRpmPct >= 50 ? s.AmbientCelsius + FlyInsideTransmissionTempPerTorque * s.TorquePct : s.AmbientCelsius;
            double tau = target >= _xmsnTemp ? FlyInsideTransmissionWarmSeconds : FlyInsideTransmissionCoolSeconds;
            _xmsnTemp += (target - _xmsnTemp) * (1.0 - Math.Exp(-dt / tau));
            return _xmsnTemp;
        }

        private static double FlyInsideTurbineOutTemp(double n2, double torque)
            => n2 * FlyInsideTurbineOutTempPerPercent + Math.Max(0.0, torque - FlyInsideIdleTorque) * FlyInsideTurbineOutTempPerTorque;
        // Cockpit torque from TURB ENG MAX TORQUE PERCENT, through two cockpit readings:
        //   raw 5.0  (ground, collective down) -> dial about 20 %
        //   raw 27.5 (in flight)               -> dial 70 %
        // piecewise linear from 0 (0 -> 0, 5 -> 20, 27.5 -> 70, same slope beyond), clamped to 0-120.
        // A straight x4 gave about 110 % in flight. Estimated from two rough readings; recheck at other powers.
        private const double FlyInsideTorqueRawLow = 5.0, FlyInsideTorqueDialLow = 20.0;
        private const double FlyInsideTorqueRawHigh = 27.5, FlyInsideTorqueDialHigh = 70.0;

        // At normal rotor speed (N2 at or above this) the cockpit dial follows the collective, which on the
        // test setup arrives as GENERAL ENG THROTTLE LEVER POSITION (the real COLLECTIVE POSITION reads 0):
        // the dial reads about 21 % with the lever at 0 and about 74 % with it steady at 73.6 (0.72 per percent).
        // The dial reacts at once to the collective, while the sim's torque variable lags it by several seconds.
        // Estimated from two steady readings (so about 93 % at lever 100; the dial swung to about 115 %
        // there for a moment, the over-torque that breaks the engine). Depends on the controller mapping.
        // Highest torque percentage sent (both estimates are limited to 0 - this).
        private const double FlyInsideTorqueMax = 120.0;
        private const double FlyInsideTorqueNormalN2 = 95.0;
        private const double FlyInsideTorqueLeverIdle = 21.0;
        private const double FlyInsideTorquePerLeverPercent = 0.72;
        // Above this lever position the line steepens so full lever reads FlyInsideTorqueMax (120 %). Dial
        // readings: 21 at lever 0, 74 at 73.6, about 100 at lever 89 (read from a blurry screenshot), and 120
        // at full lever; the last stretch is about 1.74 dial points per lever percent.
        private const double FlyInsideTorqueLeverKnee = 73.6;

        private static double FlyInsideTorque(double raw, double lever, double n2)
        {
            if (n2 >= FlyInsideTorqueNormalN2)
            {
                double leverDial = FlyInsideTorqueLeverIdle + lever * FlyInsideTorquePerLeverPercent;
                if (lever > FlyInsideTorqueLeverKnee)
                {
                    // Top of the lever travel: the dial climbs steeply to the maximum at full lever.
                    double kneeDial = FlyInsideTorqueLeverIdle + FlyInsideTorqueLeverKnee * FlyInsideTorquePerLeverPercent;
                    leverDial = kneeDial + (lever - FlyInsideTorqueLeverKnee) * ((FlyInsideTorqueMax - kneeDial) / (100.0 - FlyInsideTorqueLeverKnee));
                }
                return Math.Clamp(leverDial, 0.0, FlyInsideTorqueMax);
            }

            // Rotor not at normal speed: estimate from the turbine's max-torque percent instead.
            double dial;
            if (raw <= FlyInsideTorqueRawLow)
                dial = raw * (FlyInsideTorqueDialLow / FlyInsideTorqueRawLow);
            else
                dial = FlyInsideTorqueDialLow + (raw - FlyInsideTorqueRawLow)
                       * ((FlyInsideTorqueDialHigh - FlyInsideTorqueDialLow) / (FlyInsideTorqueRawHigh - FlyInsideTorqueRawLow));
            return Math.Clamp(dial, 0.0, FlyInsideTorqueMax);
        }

        private bool radioFrequencyChanged = false;
        private string com1ActiveFrequency = "";
        private string com1StandbyFrequency = "";
        private string com2ActiveFrequency = "";
        private string com2StandbyFrequency = "";
        private string mainBusText = "";
        private string navCom1On = "";
        private DateTime radioTimeLastPacketSent = DateTime.Now;
        private DateTime frontPanelTimeLastPacketSent = DateTime.Now;

        // A ListView showing the warning lamp states
        private ListView? _lightList;

        public Form1()
        {
            InitializeComponent();
            ConnectUdpClients();
            configureForm();
            BuildLightList();
            // Start the connection timer to look for the flight sim
            this.timerConnection.Start();
            // Start listening for radio / battery / alternator commands from the Radio / Upper controller boards
            StartListener();
        }

        private void ConnectUdpClients()
        {
            radioClient.Connect("172.16.1.101", boardPort);
            servoClient.Connect("172.16.1.102", boardPort);
            stepperClient.Connect("172.16.1.105", boardPort);
            dualStepperClient.Connect("172.16.1.106", boardPort);
            gpsClient.Connect(GpsHost, GpsPort);
        }

        // UDP to a board that is switched off can raise a socket error on a later send (ICMP
        // port unreachable); that must never stop the bridge.
        private static void SendPacket(UdpClient client, string payload)
        {
            try
            {
                byte[] data = Encoding.ASCII.GetBytes(payload);
                client.Send(data, data.Length);
            }
            catch (SocketException)
            {
            }
            catch (ObjectDisposedException)
            {
            }
        }

        // ----- Connection -----------------------------------------------------------------

        // Called every 1 second by the connection timer until MSFS is found.
        private void timerConnection_Tick(object? sender, EventArgs e)
        {
            try
            {
                OpenSimConnect();
                this.timerConnection.Stop();
                this.timerMain.Start();
                configureForm();
            }
            catch (Exception ex)
            {
                // MSFS not running (COMException) or SimConnect not available: close the
                // half-open connection and keep trying.
                CloseSimConnect();
                System.Diagnostics.Debug.WriteLine("SimConnect open failed: " + ex.Message);
            }
        }

        private void OpenSimConnect()
        {
            simconnect = new SimConnect("MSFS Winforms Bridge", this.Handle, WM_USER_SIMCONNECT, null, 0);

            simconnect.OnRecvOpen += simconnect_OnRecvOpen;
            simconnect.OnRecvQuit += simconnect_OnRecvQuit;
            simconnect.OnRecvException += simconnect_OnRecvException;
            simconnect.OnRecvSimobjectData += simconnect_OnRecvSimobjectData;

            CheckDefinitionMatchesStruct();
            simconnect.AddToDataDefinition(DEFINITIONS.PanelData, "title", null!, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            foreach (var (name, units) in PanelVars)
                simconnect.AddToDataDefinition(DEFINITIONS.PanelData, name, units, SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            simconnect.RegisterDataDefineStruct<PanelData>(DEFINITIONS.PanelData);

            // Every 2nd sim frame; the main timer samples the newest values at 10 Hz.
            simconnect.RequestDataOnSimObject(DATA_REQUESTS.PanelData, DEFINITIONS.PanelData,
                SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME,
                SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 2, 0);

            // Client events used by the commands that come back from the radio board.
            simconnect.MapClientEventToSimEvent(EVENTS.COM1_RADIO_SWAP, "COM1_RADIO_SWAP");
            simconnect.MapClientEventToSimEvent(EVENTS.COM2_RADIO_SWAP, "COM2_RADIO_SWAP");
            simconnect.MapClientEventToSimEvent(EVENTS.NAV1_RADIO_SWAP, "NAV1_RADIO_SWAP");
            simconnect.MapClientEventToSimEvent(EVENTS.NAV2_RADIO_SWAP, "NAV2_RADIO_SWAP");
            simconnect.MapClientEventToSimEvent(EVENTS.COM1_STBY_SET_HZ, "COM_STBY_RADIO_SET_HZ");
            simconnect.MapClientEventToSimEvent(EVENTS.COM2_STBY_SET_HZ, "COM2_STBY_RADIO_SET_HZ");
            simconnect.MapClientEventToSimEvent(EVENTS.NAV1_STBY_SET_HZ, "NAV1_STBY_SET_HZ");
            simconnect.MapClientEventToSimEvent(EVENTS.NAV2_STBY_SET_HZ, "NAV2_STBY_SET_HZ");
            simconnect.MapClientEventToSimEvent(EVENTS.AVIONICS_MASTER_SET, "AVIONICS_MASTER_SET");
            simconnect.MapClientEventToSimEvent(EVENTS.MASTER_BATTERY_SET, "MASTER_BATTERY_SET");
            simconnect.MapClientEventToSimEvent(EVENTS.ALTERNATOR_SET, "ALTERNATOR_SET");
        }

        private static void CheckDefinitionMatchesStruct()
        {
            int fields = typeof(PanelData).GetFields().Length;
            if (fields != PanelVars.Length + 1)
                throw new InvalidOperationException("PanelData has " + fields + " fields but PanelVars lists " + PanelVars.Length + " SimVars (+ title).");
        }

        private void CloseSimConnect()
        {
            if (simconnect != null)
            {
                try { simconnect.Dispose(); } catch { }
                simconnect = null;
            }
            _haveData = false;
            _stale = false;
        }

        private void simconnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            configureForm();
        }

        // MSFS has closed: drop back to looking for it.
        private void simconnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            CloseSimConnect();
            this.timerMain.Stop();
            configureForm();
            this.timerConnection.Start();
        }

        private void simconnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            System.Diagnostics.Debug.WriteLine("SimConnect exception: " + data.dwException);
        }

        // Streaming SimVar data: keep the newest sample for the main timer.
        private void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {
            if ((DATA_REQUESTS)data.dwRequestID != DATA_REQUESTS.PanelData) return;
            _latest = (PanelData)data.dwData[0];
            _lastDataUtc = DateTime.UtcNow;
            string title = _latest.title ?? "";
            if (!_haveData || title != _aircraftTitle)
            {
                // First data, or a different aircraft was loaded: show its name and restart the
                // aircraft-specific models (the transmission temperature estimate warms from ambient).
                _haveData = true;
                _aircraftTitle = title;
                _xmsnTemp = double.NaN;
                _cowaTorque = double.NaN;
                configureForm();
            }
        }

        // SimConnect delivers its messages through a window message.
        protected override void DefWndProc(ref Message m)
        {
            if (m.Msg == WM_USER_SIMCONNECT)
            {
                if (simconnect != null)
                {
                    try
                    {
                        simconnect.ReceiveMessage();
                    }
                    catch (COMException)
                    {
                        // The connection dropped under us; the next connection timer pass reconnects.
                        CloseSimConnect();
                        this.timerMain.Stop();
                        configureForm();
                        this.timerConnection.Start();
                    }
                }
            }
            else
            {
                base.DefWndProc(ref m);
            }
        }

        // ----- Main timer -----------------------------------------------------------------

        private static SimSnapshot ToSnapshot(PanelData d)
        {
            // The FlyInside B206 is recognised by its turbine L:var, or by its title so that it is still
            // recognised with the engine stopped (the L:var reads 0 then).
            bool flyInside = d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH > 0
                || (d.title != null && d.title.Contains("FlyInside", StringComparison.OrdinalIgnoreCase));
            // The CowaSim Jet Ranger (title "206B3 ...") reports the standard SimVars, but its cockpit dials read
            // differently (see the CowaSim constants). Its estimates are applied here and in the main timer.
            bool cowaSim = !flyInside && d.title != null && d.title.Contains("206B3", StringComparison.OrdinalIgnoreCase);
            double torque = flyInside
                ? FlyInsideTorque(d.TURB_ENG_MAX_TORQUE_PERCENT_1, d.GENERAL_ENG_THROTTLE_LEVER_POSITION_1, d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH)
                : d.ENG_TORQUE_PERCENT_1;
            double rotor = d.L_AIRCRAFT_ROTOR_RPM_PERCENT > 0 ? d.L_AIRCRAFT_ROTOR_RPM_PERCENT : d.ROTOR_RPM_PCT_1;
            return new SimSnapshot
        {
            FlyInside = flyInside,
            CowaSim = cowaSim,
            ElectricalLoadFullScaleAmps = cowaSim ? CowaSimElectricalLoadFullScaleAmps : Payloads.DefaultElectricalLoadFullScaleAmps,
            AltitudeFeet = d.PLANE_ALTITUDE,
            AirspeedKnots = d.AIRSPEED_INDICATED,
            VerticalSpeedFpm = d.VERTICAL_SPEED,
            RadarAltFeet = d.PLANE_ALT_ABOVE_GROUND,
            PitchDegrees = d.ATTITUDE_INDICATOR_PITCH_DEGREES,
            BankDegrees = d.ATTITUDE_INDICATOR_BANK_DEGREES,
            // The aircraft's own L:var when it has one (non-zero), else the standard SimVar.
            RotorRpmPct = d.L_AIRCRAFT_ROTOR_RPM_PERCENT > 0 ? d.L_AIRCRAFT_ROTOR_RPM_PERCENT : d.ROTOR_RPM_PCT_1,
            EngineRpmPct = d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH > 0 ? d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH : d.GENERAL_ENG_PCT_MAX_RPM_1,
            // FlyInside B206: ENG TORQUE PERCENT reads 0, so the cockpit dial's value is estimated from the
            // turbine's max-torque percent (FlyInside only: when its N2 L:var is present). See FlyInsideTorque.
            TorquePct = torque,
            ElectricalLoadAmps = d.ELECTRICAL_TOTAL_LOAD_AMPS,
            // FlyInside B206: the standard ITT reads about 540 C while the cockpit shows 600 with the turbine
            // at 100, so it is scaled from the turbine N2 L:var like the gas producer.
            IttCelsius = flyInside ? FlyInsideTurbineOutTemp(d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH, torque)
                : cowaSim ? Math.Max(d.ENG_EXHAUST_GAS_TEMPERATURE_1 - CowaSimTurbineOutTempOffset, d.AMBIENT_TEMPERATURE)
                : d.TURB_ENG_ITT_1,
            OilTempCelsius = d.ENG_OIL_TEMPERATURE_1,
            FuelGallons = d.FUEL_TOTAL_QUANTITY,
            // FlyInside B206: no readable gas producer variable (the standard N1 reads about 52 while the
            // cockpit shows 92 with the turbine at 100), so it is scaled from the turbine N2 L:var.
            N1Pct = d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH > 0 ? d.L_AIRCRAFT_ENGINE_1_TURBINE_N2_SMOOTH * FlyInsideGasProducerScale : d.TURB_ENG_CORRECTED_N1_1,
            OilPressurePsi = d.ENG_OIL_PRESSURE_1,
            // FlyInside B206: no transmission pressure variable exists (the SimVar reads 0 while the cockpit dial shows
            // about 35 psi), so it is estimated from the rotor speed. See FlyInsideTransmissionPressure.
            TransPressurePsi = flyInside ? FlyInsideTransmissionPressure(rotor)
                : cowaSim ? CowaSimTransmissionPressureNormal * Math.Clamp(rotor / 100.0, 0.0, 1.0)
                : d.ENG_TRANSMISSION_PRESSURE_1,
            TransTempCelsius = d.ENG_TRANSMISSION_TEMPERATURE_1,
            // FlyInside B206: the cockpit FUEL dial is a 0-3 scale marked x10 and the SimVar reads in dial units
            // (1.7 with the needle at about 1.7), so the pressure in psi (30 psi full scale) is 10 x the SimVar.
            FuelPressurePsi = flyInside ? d.GENERAL_ENG_FUEL_PRESSURE_1 * FlyInsideFuelPressureScale : d.GENERAL_ENG_FUEL_PRESSURE_1,
            MainBusVolts = d.ELECTRICAL_MAIN_BUS_VOLTAGE,
            AvionicsBusVolts = d.ELECTRICAL_AVIONICS_BUS_VOLTAGE,
            AvionicsMaster = d.AVIONICS_MASTER_SWITCH > 0.5,
            AmbientCelsius = d.AMBIENT_TEMPERATURE,
            KohlsmanMillibars = d.KOHLSMAN_SETTING_MB,
            ZuluSeconds = d.ZULU_TIME,
            Com1Active = d.COM_ACTIVE_FREQUENCY_1,
            Com1Standby = d.COM_STANDBY_FREQUENCY_1,
            Com2Active = d.COM_ACTIVE_FREQUENCY_2,
            Com2Standby = d.COM_STANDBY_FREQUENCY_2,
            Nav1Active = d.NAV_ACTIVE_FREQUENCY_1,
            Nav1Standby = d.NAV_STANDBY_FREQUENCY_1,
            Nav2Active = d.NAV_ACTIVE_FREQUENCY_2,
            Nav2Standby = d.NAV_STANDBY_FREQUENCY_2,
            LatitudeDeg = d.PLANE_LATITUDE,
            LongitudeDeg = d.PLANE_LONGITUDE,
            HeadingTrueDeg = d.PLANE_HEADING_DEGREES_TRUE,
            ZuluYear = (int)d.ZULU_YEAR,
            ZuluMonth = (int)d.ZULU_MONTH_OF_YEAR,
            ZuluDay = (int)d.ZULU_DAY_OF_MONTH,
        };
        }

        // Runs every 100 ms once connected.
        private void timerMain_Tick(object? sender, EventArgs e)
        {
            if (simconnect == null || !_haveData) return;

            // No new data (simulator paused in a menu, or loading): stop sending so the boards'
            // 30 second no-data watchdogs can return the gauges to zero.
            bool stale = (DateTime.UtcNow - _lastDataUtc).TotalSeconds > StaleAfterSeconds;
            if (stale != _stale)
            {
                _stale = stale;
                configureForm();
            }
            if (stale) return;

            SimSnapshot s = ToSnapshot(_latest);
            if (s.CowaSim) s.TorquePct = SmoothCowaSimTorque(s.TorquePct);
            if (s.FlyInside) s.TransTempCelsius = UpdateTransmissionTemp(s);
            if (s.CowaSim) s.TransTempCelsius = s.OilTempCelsius * CowaSimTransmissionTempOfOilTemp;

            // --- On-screen readout and lamps ---
            textBox1.Text = BuildReadout(s);
            UpdateLights(s);
            _suppressAvionicsEvent = true;
            chkAvionicsMaster.Checked = s.AvionicsMaster;
            _suppressAvionicsEvent = false;

            // --- Radio data to the Radio Controller board: on change (at most every 200 ms) or every 5 s ---
            TrackRadioChange(ref com1ActiveFrequency, Payloads.Freq(s.Com1Active));
            TrackRadioChange(ref com1StandbyFrequency, Payloads.Freq(s.Com1Standby));
            TrackRadioChange(ref com2ActiveFrequency, Payloads.Freq(s.Com2Active));
            TrackRadioChange(ref com2StandbyFrequency, Payloads.Freq(s.Com2Standby));
            TrackRadioChange(ref mainBusText, s.MainBusVolts.ToString("F1", CultureInfo.InvariantCulture));
            TrackRadioChange(ref navCom1On, Payloads.RadioPowered(s) ? "1" : "0");

            int radioMs = (int)(DateTime.Now - radioTimeLastPacketSent).TotalMilliseconds;
            if ((radioMs >= 200 && radioFrequencyChanged) || radioMs >= 5000)
            {
                radioFrequencyChanged = false;
                SendPacket(radioClient, Payloads.Radio(s));
                radioTimeLastPacketSent = DateTime.Now;
            }

            // --- One packet per board, about 10 per second ---
            int frontMs = (int)(DateTime.Now - frontPanelTimeLastPacketSent).TotalMilliseconds;
            if (frontMs >= 90)
            {
                SendPacket(servoClient, Payloads.Servo(s));
                SendPacket(stepperClient, Payloads.Stepper(s));
                SendPacket(dualStepperClient, Payloads.DualStepper(s));
                frontPanelTimeLastPacketSent = DateTime.Now;
            }

            // --- Aircraft information for the GPS interface, twice a second ---
            if ((DateTime.Now - gpsTimeLastPacketSent).TotalMilliseconds >= 500)
            {
                SendPacket(gpsClient, Payloads.Gps(s));
                gpsTimeLastPacketSent = DateTime.Now;
            }
        }

        private void TrackRadioChange(ref string current, string latest)
        {
            if (current != latest)
            {
                radioFrequencyChanged = true;
                current = latest;
            }
        }

        private static string BuildReadout(SimSnapshot s)
        {
            var inv = CultureInfo.InvariantCulture;
            string nl = Environment.NewLine;
            return
                "Turbine Out Temperature: " + s.IttCelsius.ToString("F1", inv) + " C" + nl +
                "Gas Producer: " + s.N1Pct.ToString("F1", inv) + "%" + nl +
                "Engine Oil Pressure: " + s.OilPressurePsi.ToString("F0", inv) + " PSI" + nl +
                "Engine Oil Temperature: " + s.OilTempCelsius.ToString("F0", inv) + " C" + nl +
                "Transmission Oil Pressure: " + s.TransPressurePsi.ToString("F1", inv) + " PSI" + nl +
                "Transmission Oil Temperature: " + s.TransTempCelsius.ToString("F0", inv) + " C" + nl +
                "Fuel Quantity: " + s.FuelGallons.ToString("F0", inv) + " Gallons" + nl +
                "Electrical Load: " + s.ElectricalLoadAmps.ToString("F1", inv) + " A (" + Payloads.ElectricalLoadPercent(s).ToString("F0", inv) + " %)" + nl +
                "Fuel Pressure: " + s.FuelPressurePsi.ToString("F1", inv) + " PSI" + nl +
                "Torque Percent: " + s.TorquePct.ToString("F1", inv) + " %" + nl +
                "Air Speed: " + s.AirspeedKnots.ToString("F1", inv) + " Knots" + nl +
                "Rotor RPM: " + s.RotorRpmPct.ToString("F1", inv) + " %" + nl +
                "Engine RPM: " + s.EngineRpmPct.ToString("F1", inv) + " %" + nl +
                "Altitude: " + s.AltitudeFeet.ToString("F0", inv) + " Feet" + nl +
                "Vertical Speed Indicator: " + s.VerticalSpeedFpm.ToString("F0", inv) + " Feet/Minute" + nl +
                "Radio Altimeter: " + s.RadarAltFeet.ToString("F2", inv) + " Feet" + nl +
                "Attitude Pitch: " + s.PitchDegrees.ToString("F2", inv) + " Degrees" + nl +
                "Attitude Bank: " + s.BankDegrees.ToString("F2", inv) + " Degrees" + nl +
                "Main Bus Voltage: " + s.MainBusVolts.ToString("F1", inv) + " V" + nl +
                "Avionics Bus Voltage: " + s.AvionicsBusVolts.ToString("F1", inv) + " V" + nl +
                "Outside Air Temperature: " + s.AmbientCelsius.ToString("F0", inv) + " C" + nl +
                "Altimeter Setting: " + (Payloads.BaroInHgx100(s) / 100.0).ToString("F2", inv) + " inHg" + nl +
                "Zulu Time: " + Payloads.ZuluHHMM(s).ToString("D4", inv) + nl +
                "Comm 1: " + Payloads.Freq(s.Com1Active) + nl +
                "Comm 1 Standby: " + Payloads.Freq(s.Com1Standby) + nl +
                "Comm 2: " + Payloads.Freq(s.Com2Active) + nl +
                "Comm 2 Standby: " + Payloads.Freq(s.Com2Standby) + nl +
                "NAV 1: " + Payloads.Freq(s.Nav1Active) + nl +
                "NAV 1 Standby: " + Payloads.Freq(s.Nav1Standby) + nl +
                "NAV 2: " + Payloads.Freq(s.Nav2Active) + nl +
                "NAV 2 Standby: " + Payloads.Freq(s.Nav2Standby) + nl +
                "GPS position: " + s.LatitudeDeg.ToString("F5", inv) + ", " + s.LongitudeDeg.ToString("F5", inv) +
                "  heading " + s.HeadingTrueDeg.ToString("F0", inv);
        }

        // ----- Warning lamp list ------------------------------------------------------------

        private void BuildLightList()
        {
            _lightList = new ListView
            {
                View = View.Details,
                Location = new System.Drawing.Point(320, 20),
                Size = new System.Drawing.Size(300, 460),
                FullRowSelect = true,
                GridLines = true,
                HeaderStyle = ColumnHeaderStyle.Nonclickable
            };
            _lightList.Columns.Add("Annunciator", 220);
            _lightList.Columns.Add("State", 70);

            foreach (var (code, name) in Payloads.Lamps)
            {
                var item = new ListViewItem(name);
                item.SubItems.Add("OFF");
                item.Tag = code;
                _lightList.Items.Add(item);
            }

            this.Controls.Add(_lightList);
            this.ClientSize = new System.Drawing.Size(330, 600);
        }

        private void UpdateLights(SimSnapshot s)
        {
            if (_lightList == null) return;
            bool powered = Payloads.GaugesPowered(s);
            _lightList.BeginUpdate();
            foreach (ListViewItem item in _lightList.Items)
            {
                bool isOn = powered && Payloads.LampState((string)item.Tag!, s);
                item.SubItems[1].Text = isOn ? "ON" : "OFF";
                item.BackColor = isOn
                    ? System.Drawing.Color.FromArgb(255, 90, 90)   // red-ish when active
                    : System.Drawing.Color.White;
            }
            _lightList.EndUpdate();
        }

        // ----- Commands from the Radio / Upper controller boards (UDP 27001) --------------------

        private void StartListener()
        {
            Task.Run(async () =>
            {
                try
                {
                    using (udpServer = new UdpClient(listenPort))
                    {
                        while (true)
                        {
                            UdpReceiveResult result = await udpServer.ReceiveAsync();
                            string receivedData = Encoding.ASCII.GetString(result.Buffer);

                            System.Diagnostics.Debug.WriteLine($"Radio command received: {receivedData} from {result.RemoteEndPoint}");

                            this.Invoke(new Action(() =>
                            {
                                UpdateRadios(receivedData);
                            }));
                        }
                    }
                }
                catch (Exception ex)
                {
                    // e.g. the port is already in use (the FSUIPC bridge also listens on 27001)
                    Console.WriteLine(ex.Message);
                }
            });
        }

        private void SendEvent(EVENTS evt, uint parameter)
        {
            if (simconnect == null) return;
            try
            {
                simconnect.TransmitClientEvent(
                    SimConnect.SIMCONNECT_OBJECT_ID_USER,
                    evt,
                    parameter,
                    GROUP_ID.GROUP0,
                    SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
            }
            catch (COMException)
            {
            }
        }

        // MHz to the Hz value the *_SET_HZ events take.
        private static uint ToHz(decimal mhz) => (uint)Math.Round(mhz * 1_000_000m);

        // Same command set as FSUIPCWinformsAutoCS.UpdateRadios(): swaps, labelled COM2 / NAV1 /
        // NAV2 standby frequencies, avionics master, battery, alternator, and a bare number for
        // the COM1 standby frequency.
        private void UpdateRadios(string command)
        {
            if (simconnect == null) return;

            if (command.Contains("COM1_RADIO_SWAP"))
            {
                SendEvent(EVENTS.COM1_RADIO_SWAP, 0);
            }
            else if (command.Contains("COM2_RADIO_SWAP"))
            {
                SendEvent(EVENTS.COM2_RADIO_SWAP, 0);
            }
            else if (command.StartsWith("COM2_STANDBY:") || command.StartsWith("NAV1_STANDBY:") || command.StartsWith("NAV2_STANDBY:"))
            {
                // Labelled standby frequency from the COM2 / NAV1 / NAV2 knobs. A bare number
                // always means COM1, so these need their own handler.
                int colon = command.IndexOf(':');
                string radio = command.Substring(0, colon);
                if (decimal.TryParse(command.Substring(colon + 1).Trim(), NumberStyles.Number, CultureInfo.InvariantCulture, out decimal freq))
                {
                    if (radio == "COM2_STANDBY" && freq >= 118.00m && freq <= 136.975m)
                        SendEvent(EVENTS.COM2_STBY_SET_HZ, ToHz(freq));
                    else if (radio == "NAV1_STANDBY" && freq >= 108.00m && freq <= 117.95m)
                        SendEvent(EVENTS.NAV1_STBY_SET_HZ, ToHz(freq));
                    else if (radio == "NAV2_STANDBY" && freq >= 108.00m && freq <= 117.95m)
                        SendEvent(EVENTS.NAV2_STBY_SET_HZ, ToHz(freq));
                }
            }
            else if (command.Contains("NAV2_RADIO_SWAP"))
            {
                SendEvent(EVENTS.NAV2_RADIO_SWAP, 0);
            }
            else if (command.Contains("NAV1_RADIO_SWAP"))
            {
                SendEvent(EVENTS.NAV1_RADIO_SWAP, 0);
            }
            else if (command.Contains("AVIONICS_MASTER_SET"))
            {
                SendEvent(EVENTS.AVIONICS_MASTER_SET, 1);
            }
            else if (command.Contains("MASTER_BATTERY_ON"))
            {
                SendEvent(EVENTS.MASTER_BATTERY_SET, 1);
            }
            else if (command.Contains("MASTER_BATTERY_OFF"))
            {
                SendEvent(EVENTS.MASTER_BATTERY_SET, 0);
            }
            else if (command.Contains("ALTERNATOR_ON"))
            {
                SendEvent(EVENTS.ALTERNATOR_SET, 1);
            }
            else if (command.Contains("ALTERNATOR_OFF"))
            {
                SendEvent(EVENTS.ALTERNATOR_SET, 0);
            }
            else
            {
                // A bare number: the COM1 standby frequency. Anything unreadable or outside the
                // COM band becomes 121.50.
                const decimal GOTO_FREQUENCY = 121.50m;
                if (!decimal.TryParse(command, NumberStyles.Number, CultureInfo.InvariantCulture, out decimal standby))
                    standby = GOTO_FREQUENCY;
                if (standby < 118.00m || standby > 136.975m)
                    standby = GOTO_FREQUENCY;
                SendEvent(EVENTS.COM1_STBY_SET_HZ, ToHz(standby));
            }
        }

        // The Avionics Master tick box: user changes go to the simulator, the box follows the simulator.
        private void chkAvionicsMaster_CheckedChanged(object? sender, EventArgs e)
        {
            if (_suppressAvionicsEvent) return;
            SendEvent(EVENTS.AVIONICS_MASTER_SET, (uint)(this.chkAvionicsMaster.Checked ? 1 : 0));
        }

        // ----- Form ---------------------------------------------------------------------------

        // Configures the status label depending on whether we're connected or not
        private void configureForm()
        {
            if (simconnect != null)
            {
                if (!_haveData)
                {
                    this.lblConnectionStatus.Text = "Connected to MSFS. Waiting for aircraft data...";
                    this.lblConnectionStatus.ForeColor = Color.DarkOrange;
                }
                else if (_stale)
                {
                    this.lblConnectionStatus.Text = "Connected to MSFS. No data (paused or loading) - not sending";
                    this.lblConnectionStatus.ForeColor = Color.DarkOrange;
                }
                else
                {
                    this.lblConnectionStatus.Text = "Connected to MSFS: " + _aircraftTitle;
                    this.lblConnectionStatus.ForeColor = Color.Green;
                }
            }
            else
            {
                this.lblConnectionStatus.Text = "Disconnected. Looking for Flight Simulator...";
                this.lblConnectionStatus.ForeColor = Color.Red;
            }
        }

        // Form is closing so stop all the timers and close the SimConnect connection
        private void frmMain_FormClosing(object? sender, FormClosingEventArgs e)
        {
            this.timerConnection.Stop();
            this.timerMain.Stop();
            CloseSimConnect();
        }
    }
}
