using System.Diagnostics;
using System.IO.Ports;
using System.Net;
using System.Net.Sockets;

namespace LowranceAirMapGps
{
    /// <summary>
    /// Feeds a Lowrance AirMap 2000C GPS with NMEA 0183 over a serial port (115200 bps, 8, N, 1).
    /// Aircraft information arrives on a UDP port; with no data for 30 seconds the position falls back to the
    /// start of the runway at Brisbane Airport. A new set of sentences is sent every 500 ms.
    /// </summary>
    public class MainForm : Form
    {
        private const int BaudRate = 115200;
        private const int SendIntervalMs = 500;
        private static readonly TimeSpan NoDataTimeout = TimeSpan.FromSeconds(30);

        private readonly AppSettings _settings = AppSettings.Load();

        // ---- serial ----
        private SerialPort? _serial;
        private string _serialStatus = "not open";
        private DateTime _nextOpenAttempt = DateTime.MinValue;

        // ---- UDP (written by the listener task, read by the timer) ----
        private UdpClient? _udp;
        private readonly object _lock = new();
        private AircraftData _latest;
        private long _lastRxTicks;                 // Stopwatch timestamp of the last accepted packet; 0 = never
        private string _udpStatus = "starting";
        private int _packets;
        private TimeSpan? _simClockOffset;         // simulator UTC minus the PC's UTC; null = use the PC clock

        private readonly System.Windows.Forms.Timer _timer = new() { Interval = SendIntervalMs };
        private int _sentUpdates;

        // ---- UI ----
        private readonly Label _lblSerial = new() { AutoSize = true };
        private readonly Label _lblUdp = new() { AutoSize = true };
        private readonly Label _lblSource = new() { AutoSize = true, Font = new Font("Segoe UI", 11F, FontStyle.Bold) };
        private readonly Label _lblTime = new() { AutoSize = true };
        private readonly Label _lblValues = new() { AutoSize = true, Font = new Font("Consolas", 10F) };
        private readonly TextBox _txtNmea = new() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, Font = new Font("Consolas", 9F) };
        private readonly Button _btnPort = new() { Text = "Change serial port...", AutoSize = true };

        public MainForm()
        {
            Text = "Lowrance AirMap 2000C GPS feeder";
            ClientSize = new Size(640, 400);
            MinimumSize = new Size(560, 360);
            StartPosition = FormStartPosition.CenterScreen;

            _latest = _settings.DefaultAircraft;

            var top = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, Padding = new Padding(10) };
            top.Controls.Add(_lblSerial);
            top.Controls.Add(_lblUdp);
            top.Controls.Add(_lblSource);
            top.Controls.Add(_lblValues);
            top.Controls.Add(_lblTime);
            top.Controls.Add(_btnPort);
            _txtNmea.Dock = DockStyle.Fill;
            Controls.Add(_txtNmea);
            Controls.Add(top);

            _btnPort.Click += (_, _) => ChoosePort(force: true);
            _timer.Tick += (_, _) => Tick();
            Shown += (_, _) => OnStart();
            FormClosing += (_, _) => Shutdown();
        }

        // ------------------------------------------------------------------ start-up / shutdown
        private void OnStart()
        {
            // 2: prompt for a serial port when none is saved (or the saved one no longer exists)
            if (string.IsNullOrEmpty(_settings.SerialPort) || !SerialPort.GetPortNames().Contains(_settings.SerialPort, StringComparer.OrdinalIgnoreCase))
                ChoosePort(force: false);
            StartUdp();
            _timer.Start();
            Tick();
        }

        private void Shutdown()
        {
            _timer.Stop();
            try { _udp?.Close(); } catch { }
            CloseSerial();
        }

        // ------------------------------------------------------------------ serial port
        private void ChoosePort(bool force)
        {
            using var dlg = new SerialPortDialog(_settings.SerialPort);
            if (dlg.ShowDialog(this) == DialogResult.OK && dlg.SelectedPort != null)
            {
                _settings.SerialPort = dlg.SelectedPort;
                _settings.Save();
                CloseSerial();
                _nextOpenAttempt = DateTime.MinValue;
            }
            else if (!force && string.IsNullOrEmpty(_settings.SerialPort))
            {
                _serialStatus = "no serial port selected";
            }
        }

        private void EnsureSerialOpen()
        {
            if (_serial is { IsOpen: true } || string.IsNullOrEmpty(_settings.SerialPort)) return;
            if (DateTime.UtcNow < _nextOpenAttempt) return;
            try
            {
                // 3: 115200 bps, 8 data bits, no parity, 1 stop bit
                var sp = new SerialPort(_settings.SerialPort, BaudRate, Parity.None, 8, StopBits.One)
                {
                    Handshake = Handshake.None,
                    WriteTimeout = 400,
                    Encoding = System.Text.Encoding.ASCII,
                };
                sp.Open();
                _serial = sp;
                _serialStatus = "open";
            }
            catch (Exception ex)
            {
                _serial = null;
                _serialStatus = "cannot open: " + ex.Message;
                _nextOpenAttempt = DateTime.UtcNow.AddSeconds(3);   // retry, e.g. when a USB adapter is plugged in later
            }
        }

        private void CloseSerial()
        {
            try { _serial?.Close(); _serial?.Dispose(); } catch { }
            _serial = null;
            _serialStatus = "closed";
        }

        // ------------------------------------------------------------------ UDP
        private void StartUdp()
        {
            try
            {
                _udp = new UdpClient(new IPEndPoint(IPAddress.Any, _settings.UdpPort));
                _udpStatus = "listening";
            }
            catch (Exception ex)
            {
                _udpStatus = "cannot listen: " + ex.Message;
                return;
            }
            UdpClient udp = _udp;
            Task.Run(async () =>
            {
                while (true)
                {
                    try
                    {
                        UdpReceiveResult r = await udp.ReceiveAsync();
                        string text = System.Text.Encoding.ASCII.GetString(r.Buffer);
                        lock (_lock)
                        {
                            AircraftData? d = AircraftParser.Parse(text, _latest);
                            if (d == null) continue;
                            _latest = d.Value;
                            if (d.Value.SimUtc is DateTime sim) UpdateSimClock(sim); else _simClockOffset = null;   // no time in the packet: use the PC clock
                            _lastRxTicks = Stopwatch.GetTimestamp();
                            _packets++;
                        }
                    }
                    catch (ObjectDisposedException) { return; }       // form closing
                    catch (SocketException) { /* ignore a bad packet or ICMP reset and keep listening */ }
                }
            });
        }

        /// <summary>
        /// Keeps the NMEA time and date in step with the simulator. The offset from the PC clock is taken from the first packet and kept
        /// (so the GPS clock ticks smoothly in real time between packets); it is only re-taken when the simulator time has moved by more
        /// than 1.5 seconds from where the offset predicts, e.g. the user changed the time or the simulation rate is not 1x.
        /// Called with the lock held.
        /// </summary>
        private void UpdateSimClock(DateTime simUtc)
        {
            TimeSpan measured = simUtc - DateTime.UtcNow;
            if (_simClockOffset == null || Math.Abs((measured - _simClockOffset.Value).TotalSeconds) > 1.5)
                _simClockOffset = measured;
        }

        // ------------------------------------------------------------------ 500 ms update
        private void Tick()
        {
            AircraftData data;
            bool live;
            double ageSeconds;
            DateTime gpsTime;
            bool simTime;
            lock (_lock)
            {
                // 5: with no data for 30 seconds the position defaults back to Brisbane Airport
                ageSeconds = _lastRxTicks == 0 ? double.PositiveInfinity : Stopwatch.GetElapsedTime(_lastRxTicks).TotalSeconds;
                live = ageSeconds < NoDataTimeout.TotalSeconds;
                data = live ? _latest : _settings.DefaultAircraft;
                if (!live) { _latest = _settings.DefaultAircraft; _simClockOffset = null; }   // a partial packet after a timeout starts from the default
                // the simulator's date and time while it is supplying them, otherwise the PC clock
                simTime = live && _simClockOffset != null;
                gpsTime = simTime ? DateTime.UtcNow + _simClockOffset!.Value : DateTime.UtcNow;
            }

            IReadOnlyList<string> sentences = NmeaBuilder.Build(data, gpsTime);

            // 6: one set of sentences every 500 ms
            EnsureSerialOpen();
            if (_serial is { IsOpen: true })
            {
                try
                {
                    byte[] bytes = NmeaBuilder.ToBytes(sentences);
                    _serial.Write(bytes, 0, bytes.Length);
                    _sentUpdates++;
                }
                catch (Exception ex)
                {
                    _serialStatus = "write failed: " + ex.Message;
                    CloseSerial();
                    _nextOpenAttempt = DateTime.UtcNow.AddSeconds(3);
                }
            }

            UpdateUi(data, live, ageSeconds, sentences, gpsTime, simTime);
        }

        private void UpdateUi(AircraftData d, bool live, double ageSeconds, IReadOnlyList<string> sentences, DateTime gpsTime, bool simTime)
        {
            string port = string.IsNullOrEmpty(_settings.SerialPort) ? "(none)" : _settings.SerialPort;
            _lblSerial.Text = $"Serial: {port} at {BaudRate} bps 8-N-1 - {_serialStatus}  (updates sent: {_sentUpdates})";
            _lblUdp.Text = $"UDP port {_settings.UdpPort}: {_udpStatus}  (packets accepted: {_packets})";
            _lblSource.Text = live ? $"Source: aircraft data over UDP (last {ageSeconds:0.0} s ago)"
                                   : "Source: default - Brisbane Airport, start of runway";
            _lblSource.ForeColor = live ? Color.DarkGreen : Color.DarkOrange;
            _lblValues.Text =
                $"Lat {d.LatitudeDeg,10:0.00000}   Lon {d.LongitudeDeg,10:0.00000}\n" +
                $"Alt {d.AltitudeFt,7:0} ft   Airspeed {d.AirspeedKt,5:0.0} kt   Heading {d.HeadingDeg,5:0.0}°";
            _lblTime.Text = $"GPS time: {gpsTime:yyyy-MM-dd HH:mm:ss} UTC ({(simTime ? "simulator time" : "PC clock")})";
            _txtNmea.Text = string.Concat(sentences);
        }
    }
}
