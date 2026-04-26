// File: src/SimConnectCrossPlatform.WinFormsApp/MainForm.cs

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.Extensions.Logging;
using SimConnectCrossPlatform.Models;
using SimConnectCrossPlatform.Reconnection;
using SimConnectCrossPlatform.WinFormsApp.Logging;

namespace SimConnectCrossPlatform.WinFormsApp
{
    public partial class MainForm : Form
    {
        private ReconnectingSimInterface? _sim;
        private ILogger? _logger;
        private bool _isConnected;

        // Request IDs
        private const uint REQ_FLIGHT_DATA = 100;
        private const uint REQ_ENGINE_DATA = 200;
        private const uint REQ_AUTOPILOT_DATA = 300;

        // Definition IDs
        private const uint DEF_FLIGHT_DATA = 1;
        private const uint DEF_ENGINE_DATA = 2;
        private const uint DEF_AUTOPILOT_DATA = 3;

        public MainForm()
        {
            InitializeComponent();
            WireEvents();
        }

        // =====================================================
        // Initialization
        // =====================================================

        private void WireEvents()
        {
            this.Load += MainForm_Load;
            this.FormClosing += MainForm_FormClosing;

            btnConnect.Click += BtnConnect_Click;
            btnDisconnect.Click += BtnDisconnect_Click;
            btnClearLog.Click += BtnClearLog_Click;

            btnParkBrake.Click += (s, e) =>
                SendSimEvent("PARKING_BRAKES");
            btnGear.Click += (s, e) =>
                SendSimEvent("GEAR_TOGGLE");
            btnLandingLights.Click += (s, e) =>
                SendSimEvent("LANDING_LIGHTS_TOGGLE");
            btnNavLights.Click += (s, e) =>
                SendSimEvent("TOGGLE_NAV_LIGHTS");
            btnBeacon.Click += (s, e) =>
                SendSimEvent("TOGGLE_BEACON_LIGHTS");
            btnStrobes.Click += (s, e) =>
                SendSimEvent("STROBES_TOGGLE");
        }

        private void MainForm_Load(object? sender, EventArgs e)
        {
            // Create logger that writes to the log textbox
            _logger = new TextBoxLogger(
                txtLog, "SimConnect", LogLevel.Debug);

            _logger.LogInformation("Application started");
            _logger.LogInformation(
                "Cross-Platform SimConnect for MSFS2024 & P3D");
            _logger.LogInformation(
                "Click 'Connect' to start");

            UpdateUIState(false);
        }

        // =====================================================
        // Connection
        // =====================================================

        private void BtnConnect_Click(object? sender, EventArgs e)
        {
            try
            {
                btnConnect.Enabled = false;

                _logger?.LogInformation(
                    "Initializing connection...");

                // Create reconnection config
                var config = new ReconnectionConfig
                {
                    InitialRetryDelay = TimeSpan.FromSeconds(2),
                    MaxRetryDelay = TimeSpan.FromSeconds(30),
                    BackoffMultiplier = 2.0,
                    EnableHeartbeat = true,
                    HeartbeatTimeout = TimeSpan.FromSeconds(15),
                    MaxRetryAttempts = -1,
                    AutoReconnect = true,
                    ProcessInterval =
                        TimeSpan.FromMilliseconds(50)
                };

                // Create interface with logger
                _sim = new ReconnectingSimInterface(
                    config, _logger);

                // Wire events
                _sim.ConnectionStateChanged +=
                    Sim_ConnectionStateChanged;
                _sim.DataReceived +=
                    Sim_DataReceived;
                _sim.ReconnectionAttempt +=
                    Sim_ReconnectionAttempt;
                _sim.ExceptionReceived +=
                    Sim_ExceptionReceived;

                // Register data definitions
                RegisterDataDefinitions();

                // Register data requests (persisted)
                _sim.RequestData(
                    REQ_FLIGHT_DATA,
                    DEF_FLIGHT_DATA,
                    UpdatePeriod.PerSecond);
                _sim.RequestData(
                    REQ_ENGINE_DATA,
                    DEF_ENGINE_DATA,
                    UpdatePeriod.PerSecond);
                _sim.RequestData(
                    REQ_AUTOPILOT_DATA,
                    DEF_AUTOPILOT_DATA,
                    UpdatePeriod.PerSecond);

                // Start background worker
                _sim.Start("SimConnectWinFormsApp");

                // Update button states
                btnDisconnect.Enabled = true;

                _logger?.LogInformation(
                    "Connection process started. " +
                    "Waiting for simulator...");
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Failed to start: {Error}", ex.Message);
                btnConnect.Enabled = true;
                btnDisconnect.Enabled = false;
            }
        }

        private void BtnDisconnect_Click(
            object? sender, EventArgs e)
        {
            DisconnectAsync();
        }

        private async void DisconnectAsync()
        {
            try
            {
                btnDisconnect.Enabled = false;
                _logger?.LogInformation("Disconnecting...");

                if (_sim != null)
                {
                    // Unwire events first
                    _sim.ConnectionStateChanged -=
                        Sim_ConnectionStateChanged;
                    _sim.DataReceived -=
                        Sim_DataReceived;
                    _sim.ReconnectionAttempt -=
                        Sim_ReconnectionAttempt;
                    _sim.ExceptionReceived -=
                        Sim_ExceptionReceived;

                    await _sim.StopAsync();
                    _sim.Dispose();
                    _sim = null;
                }

                _isConnected = false;
                UpdateUIState(false);
                ResetDataDisplay();
                UpdateConnectionStatusDisplay(
                    ConnectionState.Disconnected);

                _logger?.LogInformation("Disconnected.");
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Error disconnecting: {Error}", ex.Message);
            }
            finally
            {
                btnConnect.Enabled = true;
            }
        }

        private void RegisterDataDefinitions()
        {
            if (_sim == null) return;

            // Flight Data
            _sim.RegisterDataDefinition(new DataDefinition(
                DEF_FLIGHT_DATA, "FlightData",
                new SimVarDefinition(
                    "PLANE ALTITUDE", "feet"),
                new SimVarDefinition(
                    "AIRSPEED INDICATED", "knots"),
                new SimVarDefinition(
                    "PLANE HEADING DEGREES MAGNETIC", "degrees"),
                new SimVarDefinition(
                    "VERTICAL SPEED", "feet per minute"),
                new SimVarDefinition(
                    "GROUND VELOCITY", "knots"),
                new SimVarDefinition(
                    "PLANE LATITUDE", "degrees"),
                new SimVarDefinition(
                    "PLANE LONGITUDE", "degrees")
            ));

            // Engine Data
            _sim.RegisterDataDefinition(new DataDefinition(
                DEF_ENGINE_DATA, "EngineData",
                new SimVarDefinition(
                    "GENERAL ENG RPM:1", "rpm"),
                new SimVarDefinition(
                    "GENERAL ENG THROTTLE LEVER POSITION:1",
                    "percent"),
                new SimVarDefinition(
                    "ENG FUEL FLOW GPH:1",
                    "gallons per hour"),
                new SimVarDefinition(
                    "FUEL TOTAL QUANTITY WEIGHT", "pounds")
            ));

            // Autopilot Data
            _sim.RegisterDataDefinition(new DataDefinition(
                DEF_AUTOPILOT_DATA, "AutopilotData",
                new SimVarDefinition(
                    "AUTOPILOT MASTER", "bool",
                    SimDataType.Int32),
                new SimVarDefinition(
                    "AUTOPILOT HEADING LOCK DIR", "degrees"),
                new SimVarDefinition(
                    "AUTOPILOT ALTITUDE LOCK VAR", "feet"),
                new SimVarDefinition(
                    "AUTOPILOT AIRSPEED HOLD VAR", "knots")
            ));

            _logger?.LogInformation(
                "Registered 3 data definition groups");
        }

        // =====================================================
        // SimConnect Event Handlers
        // =====================================================

        private void Sim_ConnectionStateChanged(
            object? sender,
            ConnectionStateChangedEventArgs e)
        {
            SafeInvoke(() =>
            {
                _isConnected =
                    e.NewState == ConnectionState.Connected;

                UpdateUIState(_isConnected);
                UpdateConnectionStatusDisplay(e.NewState);

                statusRetryCount.Text =
                    $"Retries: {_sim?.RetryCount ?? 0}";

                if (_isConnected)
                {
                    lblPlatform.Text =
                        _sim?.PlatformName ?? "";
                    statusPlatform.Text =
                        _sim?.PlatformName ?? "";
                }
            });
        }

        private void Sim_DataReceived(
            object? sender,
            SimDataReceivedEventArgs e)
        {
            SafeInvoke(() =>
            {
                switch (e.RequestId)
                {
                    case REQ_FLIGHT_DATA:
                        UpdateFlightData(e.Data);
                        break;
                    case REQ_ENGINE_DATA:
                        UpdateEngineData(e.Data);
                        break;
                    case REQ_AUTOPILOT_DATA:
                        UpdateAutopilotData(e.Data);
                        break;
                }
            });
        }

        private void Sim_ReconnectionAttempt(
            object? sender,
            ReconnectionAttemptEventArgs e)
        {
            SafeInvoke(() =>
            {
                statusRetryCount.Text =
                    $"Retries: {e.AttemptNumber}";
                statusLabel.Text =
                    $"Reconnecting (#{e.AttemptNumber}) " +
                    $"in {e.NextRetryDelay.TotalSeconds:F1}s...";
            });
        }

        private void Sim_ExceptionReceived(
            object? sender,
            SimConnectExceptionEventArgs e)
        {
            _logger?.LogWarning(
                "SimConnect Exception: {Message} " +
                "(SendID={SendId}, Index={Index})",
                e.Message, e.SendId, e.Index);
        }

        // =====================================================
        // Connection Status Display
        // =====================================================

        private void UpdateConnectionStatusDisplay(
            ConnectionState state)
        {
            switch (state)
            {
                case ConnectionState.Connected:
                    lblConnectionStatus.Text = "● CONNECTED";
                    lblConnectionStatus.ForeColor =
                        Color.FromArgb(80, 255, 80);
                    statusLabel.Text =
                        $"Connected to {_sim?.PlatformName}";
                    break;

                case ConnectionState.Disconnected:
                    lblConnectionStatus.Text = "● DISCONNECTED";
                    lblConnectionStatus.ForeColor =
                        Color.FromArgb(255, 80, 80);
                    statusLabel.Text = "Disconnected";
                    statusPlatform.Text = "";
                    lblPlatform.Text = "";
                    break;

                case ConnectionState.Connecting:
                    lblConnectionStatus.Text = "● CONNECTING...";
                    lblConnectionStatus.ForeColor =
                        Color.FromArgb(255, 255, 80);
                    statusLabel.Text = "Connecting...";
                    break;

                case ConnectionState.Error:
                    lblConnectionStatus.Text = "● ERROR";
                    lblConnectionStatus.ForeColor =
                        Color.FromArgb(255, 120, 0);
                    statusLabel.Text = "Connection error — " +
                        "will retry automatically";
                    break;
            }
        }

        // =====================================================
        // Data Display Updates
        // =====================================================

        private void UpdateFlightData(
            IReadOnlyDictionary<string, SimValue> data)
        {
            if (data.TryGetValue(
                "PLANE ALTITUDE", out var alt))
            {
                txtAltitude.Text = $"{alt.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "AIRSPEED INDICATED", out var spd))
            {
                txtAirspeed.Text = $"{spd.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "PLANE HEADING DEGREES MAGNETIC", out var hdg))
            {
                txtHeading.Text = $"{hdg.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "VERTICAL SPEED", out var vs))
            {
                double vsVal = vs.AsDouble();
                txtVerticalSpeed.Text = $"{vsVal:N0}";

                // Color-code: green=climbing, red=descending
                if (vsVal > 50)
                {
                    txtVerticalSpeed.ForeColor =
                        Color.FromArgb(0, 255, 128);
                }
                else if (vsVal < -50)
                {
                    txtVerticalSpeed.ForeColor =
                        Color.FromArgb(255, 100, 100);
                }
                else
                {
                    txtVerticalSpeed.ForeColor =
                        Color.FromArgb(200, 200, 200);
                }
            }

            if (data.TryGetValue(
                "GROUND VELOCITY", out var gs))
            {
                txtGroundSpeed.Text = $"{gs.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "PLANE LATITUDE", out var lat))
            {
                txtLatitude.Text = $"{lat.AsDouble():F6}";
            }

            if (data.TryGetValue(
                "PLANE LONGITUDE", out var lon))
            {
                txtLongitude.Text = $"{lon.AsDouble():F6}";
            }
        }

        private void UpdateEngineData(
            IReadOnlyDictionary<string, SimValue> data)
        {
            if (data.TryGetValue(
                "GENERAL ENG RPM:1", out var rpm))
            {
                txtRPM.Text = $"{rpm.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "GENERAL ENG THROTTLE LEVER POSITION:1",
                out var thr))
            {
                txtThrottle.Text = $"{thr.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "ENG FUEL FLOW GPH:1", out var ff))
            {
                txtFuelFlow.Text = $"{ff.AsDouble():N1}";
            }

            if (data.TryGetValue(
                "FUEL TOTAL QUANTITY WEIGHT", out var fuel))
            {
                txtFuel.Text = $"{fuel.AsDouble():N0}";
            }
        }

        private void UpdateAutopilotData(
            IReadOnlyDictionary<string, SimValue> data)
        {
            if (data.TryGetValue(
                "AUTOPILOT MASTER", out var apMaster))
            {
                bool isOn = apMaster.AsInt() != 0;
                txtAPMaster.Text = isOn ? "ON" : "OFF";
                txtAPMaster.ForeColor = isOn
                    ? Color.FromArgb(80, 255, 80)
                    : Color.FromArgb(255, 80, 80);
            }

            if (data.TryGetValue(
                "AUTOPILOT HEADING LOCK DIR", out var apHdg))
            {
                txtAPHeading.Text =
                    $"{apHdg.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "AUTOPILOT ALTITUDE LOCK VAR", out var apAlt))
            {
                txtAPAltitude.Text =
                    $"{apAlt.AsDouble():N0}";
            }

            if (data.TryGetValue(
                "AUTOPILOT AIRSPEED HOLD VAR", out var apSpd))
            {
                txtAPSpeed.Text =
                    $"{apSpd.AsDouble():N0}";
            }
        }

        private void ResetDataDisplay()
        {
            const string def = "---";

            // Flight data
            txtAltitude.Text = def;
            txtAirspeed.Text = def;
            txtHeading.Text = def;
            txtVerticalSpeed.Text = def;
            txtVerticalSpeed.ForeColor =
                Color.FromArgb(0, 255, 128);
            txtGroundSpeed.Text = def;
            txtLatitude.Text = def;
            txtLongitude.Text = def;

            // Engine data
            txtRPM.Text = def;
            txtThrottle.Text = def;
            txtFuelFlow.Text = def;
            txtFuel.Text = def;

            // Autopilot
            txtAPMaster.Text = def;
            txtAPMaster.ForeColor =
                Color.FromArgb(150, 200, 255);
            txtAPHeading.Text = def;
            txtAPAltitude.Text = def;
            txtAPSpeed.Text = def;

            // Labels
            lblPlatform.Text = "";
            statusPlatform.Text = "";
        }

        // =====================================================
        // UI State Management
        // =====================================================

        private void UpdateUIState(bool connected)
        {
            btnConnect.Enabled = !connected && _sim == null;
            btnDisconnect.Enabled = _sim != null;

            // Sim event buttons only active when connected
            btnParkBrake.Enabled = connected;
            btnGear.Enabled = connected;
            btnLandingLights.Enabled = connected;
            btnNavLights.Enabled = connected;
            btnBeacon.Enabled = connected;
            btnStrobes.Enabled = connected;
        }

        // =====================================================
        // Event Sending
        // =====================================================

        private void SendSimEvent(string eventName)
        {
            if (_sim == null || !_isConnected)
            {
                _logger?.LogWarning(
                    "Cannot send event '{Event}' — not connected",
                    eventName);
                return;
            }

            _logger?.LogInformation(
                "Sending event: {Event}", eventName);

            if (!_sim.SendEvent(eventName))
            {
                _logger?.LogError(
                    "Failed to send event: {Event}", eventName);
            }
        }

        // =====================================================
        // Clear Log
        // =====================================================

        private void BtnClearLog_Click(
            object? sender, EventArgs e)
        {
            txtLog.Clear();
            _logger?.LogInformation("Log cleared");
        }

        // =====================================================
        // Form Closing — Clean Shutdown
        // =====================================================

        private async void MainForm_FormClosing(
            object? sender, FormClosingEventArgs e)
        {
            if (_sim != null)
            {
                _logger?.LogInformation(
                    "Application closing — shutting down...");

                try
                {
                    // Unwire events to prevent callbacks
                    // during shutdown
                    _sim.ConnectionStateChanged -=
                        Sim_ConnectionStateChanged;
                    _sim.DataReceived -=
                        Sim_DataReceived;
                    _sim.ReconnectionAttempt -=
                        Sim_ReconnectionAttempt;
                    _sim.ExceptionReceived -=
                        Sim_ExceptionReceived;

                    await _sim.StopAsync();
                    _sim.Dispose();
                    _sim = null;
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(
                        $"Shutdown error: {ex.Message}");
                }
            }
        }

        // =====================================================
        // Thread-Safe UI Helper
        // =====================================================

        /// <summary>
        /// Safely invoke an action on the UI thread.
        /// Handles cross-thread calls from the SimConnect
        /// background worker.
        /// </summary>
        private void SafeInvoke(Action action)
        {
            try
            {
                if (this.IsDisposed || !this.IsHandleCreated)
                    return;

                if (this.InvokeRequired)
                {
                    this.BeginInvoke(action);
                }
                else
                {
                    action();
                }
            }
            catch (ObjectDisposedException)
            {
                // Form was disposed during invoke — safe to ignore
            }
            catch (InvalidOperationException)
            {
                // Handle was not created or form is closing
            }
        }
    }
}