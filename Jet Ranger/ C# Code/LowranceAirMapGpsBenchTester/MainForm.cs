using System.Globalization;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace LowranceAirMapGpsBenchTester
{
    /// <summary>
    /// Bench tester for LowranceAirMapGps: sends aircraft information over UDP (default 127.0.0.1:27002, twice a second)
    /// so the GPS interface can be exercised without a simulator. Latitude, longitude, altitude, speed and heading can be
    /// typed in, and an 8-way cursor (buttons or the arrow keys) moves the position.
    /// </summary>
    public class MainForm : Form
    {
        [DllImport("user32.dll")] private static extern short GetAsyncKeyState(int vKey);

        // Brisbane Airport, threshold of runway 01L - the same default the GPS interface uses
        private const double HomeLat = -27.3830, HomeLon = 153.1068, HomeAlt = 13, HomeHdg = 27;
        private const int SendIntervalMs = 500;
        private const int CursorTickMs = 100;
        private const int RepeatDelayMs = 400;

        private double _lat = HomeLat, _lon = HomeLon, _alt = HomeAlt, _spd = 0, _hdg = HomeHdg;

        private readonly UdpClient _udp = new();
        private readonly System.Windows.Forms.Timer _sendTimer = new() { Interval = SendIntervalMs };
        private readonly System.Windows.Forms.Timer _cursorTimer = new() { Interval = CursorTickMs };
        private int _sent;
        private string _status = "";

        // cursor state
        private Compass? _buttonDir;            // direction held with the mouse
        private Compass? _lastDir;              // direction on the previous cursor tick
        private DateTime _holdStart;

        private readonly TextBox _host = new() { Text = "127.0.0.1", Width = 110 };
        private readonly TextBox _port = new() { Text = "27002", Width = 60 };
        private readonly CheckBox _chkSend = new() { Text = "Send twice a second", Checked = true, AutoSize = true };
        private readonly Label _lblSent = new() { AutoSize = true };
        private readonly TextBox _txtLat = Field(), _txtLon = Field(), _txtAlt = Field(), _txtSpd = Field(), _txtHdg = Field();
        private readonly NumericUpDown _step = new() { Minimum = 1, Maximum = 100000, Value = 100, Width = 80 };
        // simulator date/time (UTC): a clock that runs in real time from the value typed in
        private readonly TextBox _txtTime = new() { Width = 150 };
        private readonly CheckBox _chkTime = new() { Text = "Send date/time", Checked = true, AutoSize = true };
        private TimeSpan _clockOffset = TimeSpan.Zero;       // simulator UTC minus the PC's UTC
        private const string TimeFormat = "yyyy-MM-dd HH:mm:ss";
        private readonly CheckBox _chkFly = new() { Text = "Fly: move along the heading at the current speed", AutoSize = true };
        private readonly NumericUpDown _timeX = new() { Minimum = 1, Maximum = 100, Value = 1, Width = 60 };
        private long _lastFlyTicks;
        private readonly CheckBox _chkHeading = new() { Text = "Point heading in direction of travel", Checked = true, AutoSize = true };
        private readonly Label _lblPacket = new() { AutoSize = true, Font = new Font("Consolas", 9F) };

        // ---- airport selection ----
        private readonly AirportDb _db = AirportDb.Load();
        private readonly ComboBox _cmbCountry = AirportCombo(), _cmbCity = AirportCombo(), _cmbAirport = AirportCombo();
        private Airport? _airport;               // the airport currently selected (Reset goes back to it)
        private bool _filling;                   // true while the combo boxes are being filled by code

        private static ComboBox AirportCombo() => new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 300, DropDownHeight = 300 };

        private static TextBox Field() => new() { Width = 110 };

        public MainForm()
        {
            Text = "Lowrance AirMap GPS bench tester";
            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            FormBorderStyle = FormBorderStyle.FixedSingle;
            MaximizeBox = false;
            StartPosition = FormStartPosition.CenterScreen;
            Padding = new Padding(10);

            var root = new TableLayoutPanel { AutoSize = true, ColumnCount = 2, Dock = DockStyle.Fill };

            // --- target ---
            var target = new FlowLayoutPanel { AutoSize = true, FlowDirection = FlowDirection.LeftToRight, WrapContents = false };
            target.Controls.AddRange(new Control[] { new Label { Text = "Send to UDP", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, _host, new Label { Text = ":", AutoSize = true, Margin = new Padding(0, 6, 0, 0) }, _port, _chkSend });
            root.Controls.Add(target); root.SetColumnSpan(target, 2);

            // --- location: country / city / airport ---
            var loc = new TableLayoutPanel { AutoSize = true, ColumnCount = 2, Margin = new Padding(0, 6, 0, 6) };
            AddLocationRow(loc, "Country", _cmbCountry);
            AddLocationRow(loc, "City", _cmbCity);
            AddLocationRow(loc, "Airport", _cmbAirport);
            root.Controls.Add(loc); root.SetColumnSpan(loc, 2);
            _cmbCountry.SelectedIndexChanged += (_, _) => { if (!_filling) FillCities(null); };
            _cmbCity.SelectedIndexChanged += (_, _) => { if (!_filling) FillAirports(null); };
            _cmbAirport.SelectedIndexChanged += (_, _) => { if (!_filling && _cmbAirport.SelectedItem is Airport a) GoToAirport(a); };

            // --- values ---
            var grid = new TableLayoutPanel { AutoSize = true, ColumnCount = 3 };
            AddRow(grid, "Latitude (deg, S negative)", _txtLat, "");
            AddRow(grid, "Longitude (deg, W negative)", _txtLon, "");
            AddRow(grid, "Altitude", _txtAlt, "ft");
            AddRow(grid, "Speed (airspeed)", _txtSpd, "kt");
            AddRow(grid, "Heading (true)", _txtHdg, "deg");
            g_AddTimeRow(grid);
            var home = new Button { Text = "Back to the selected airport", AutoSize = true };
            home.Click += (_, _) =>
            {
                if (_airport != null) { _lat = _airport.Lat; _lon = _airport.Lon; _alt = _airport.ElevationFt; }
                else { _lat = HomeLat; _lon = HomeLon; _alt = HomeAlt; }
                _spd = 0; RefreshFields(force: true);
            };
            grid.Controls.Add(home); grid.SetColumnSpan(home, 3);
            var hint = new Label { Text = "Type a value and press Enter.", AutoSize = true, ForeColor = SystemColors.GrayText };
            grid.Controls.Add(hint); grid.SetColumnSpan(hint, 3);
            root.Controls.Add(grid);

            // --- 8-way cursor ---
            var pad = new TableLayoutPanel { AutoSize = true, ColumnCount = 3, RowCount = 3, Margin = new Padding(20, 0, 0, 0) };
            Compass?[,] layout = { { Compass.NW, Compass.N, Compass.NE }, { Compass.W, null, Compass.E }, { Compass.SW, Compass.S, Compass.SE } };
            string[,] arrows = { { "↖", "↑", "↗" }, { "←", "●", "→" }, { "↙", "↓", "↘" } };
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                {
                    var btn = new Button { Text = arrows[r, c], Width = 54, Height = 54, Font = new Font("Segoe UI Symbol", 16F), Margin = new Padding(2), TabStop = false };
                    Compass? dir = layout[r, c];
                    if (dir == null) btn.Enabled = false;
                    else
                    {
                        btn.MouseDown += (_, e) => { if (e.Button == MouseButtons.Left) _buttonDir = dir; };
                        btn.MouseUp += (_, _) => _buttonDir = null;
                        btn.LostFocus += (_, _) => _buttonDir = null;
                    }
                    pad.Controls.Add(btn, c, r);
                }
            root.Controls.Add(pad);

            // --- step / options ---
            var opts = new FlowLayoutPanel { AutoSize = true, FlowDirection = FlowDirection.LeftToRight, WrapContents = false };
            opts.Controls.AddRange(new Control[] { new Label { Text = "Cursor step", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, _step, new Label { Text = "m per move (hold to repeat; arrow keys work too)", AutoSize = true, Margin = new Padding(3, 6, 0, 0) } });
            root.Controls.Add(opts); root.SetColumnSpan(opts, 2);
            var fly = new FlowLayoutPanel { AutoSize = true, FlowDirection = FlowDirection.LeftToRight, WrapContents = false };
            fly.Controls.AddRange(new Control[] { _chkFly, new Label { Text = "Time x", AutoSize = true, Margin = new Padding(10, 6, 3, 0) }, _timeX });
            root.Controls.Add(fly); root.SetColumnSpan(fly, 2);
            _chkFly.CheckedChanged += (_, _) => _lastFlyTicks = System.Diagnostics.Stopwatch.GetTimestamp();
            root.Controls.Add(_chkHeading); root.SetColumnSpan(_chkHeading, 2);
            root.Controls.Add(_lblSent); root.SetColumnSpan(_lblSent, 2);
            root.Controls.Add(_lblPacket); root.SetColumnSpan(_lblPacket, 2);
            Controls.Add(root);

            foreach (var (tb, apply) in new (TextBox, Action<double>)[] {
                (_txtLat, v => _lat = Math.Clamp(v, -90, 90)), (_txtLon, v => _lon = Math.Clamp(v, -180, 180)),
                (_txtAlt, v => _alt = v), (_txtSpd, v => _spd = Math.Max(0, v)),
                (_txtHdg, v => { _hdg = ((v % 360) + 360) % 360; }) })
            {
                TextBox box = tb; Action<double> set = apply;
                box.KeyDown += (_, e) => { if (e.KeyCode == Keys.Enter) { Commit(box, set); e.SuppressKeyPress = true; ActiveControl = null; } };
                box.Leave += (_, _) => Commit(box, set);
            }

            _sendTimer.Tick += (_, _) => SendNow();
            _cursorTimer.Tick += (_, _) => CursorTick();
            Shown += (_, _) => { InitAirports(); RefreshFields(force: true); ShowClock(force: true); _sendTimer.Start(); _cursorTimer.Start(); };
            FormClosing += (_, _) => { _sendTimer.Stop(); _cursorTimer.Stop(); _udp.Dispose(); };
        }

        // ------------------------------------------------------------ airport selection
        private static void AddLocationRow(TableLayoutPanel g, string label, ComboBox box)
        {
            g.Controls.Add(new Label { Text = label, AutoSize = true, Margin = new Padding(0, 6, 8, 0) });
            g.Controls.Add(box);
        }

        /// <summary>Starts on Brisbane Airport (YBBN), the same default the GPS interface uses.</summary>
        private void InitAirports()
        {
            _filling = true;
            _cmbCountry.Items.Clear();
            foreach (Country c in _db.Countries()) _cmbCountry.Items.Add(c);
            _filling = false;
            Airport? start = _db.Find("YBBN");
            if (start == null) return;                      // list missing: the typed values still work
            SelectCountry(start.CountryCode);
            FillCities(start.City, start.Icao, goTo: false);   // select only: the position stays at the runway start
        }

        private void SelectCountry(string code)
        {
            _filling = true;
            foreach (Country c in _cmbCountry.Items) if (c.Code == code) { _cmbCountry.SelectedItem = c; break; }
            _filling = false;
        }

        private void FillCities(string? select, string? selectIcao = null, bool goTo = true)
        {
            if (_cmbCountry.SelectedItem is not Country country) return;
            _filling = true;
            _cmbCity.Items.Clear();
            foreach (string city in _db.Cities(country.Code)) _cmbCity.Items.Add(city);
            _cmbCity.SelectedItem = select != null && _cmbCity.Items.Contains(select) ? select : (_cmbCity.Items.Count > 0 ? _cmbCity.Items[0] : null);
            _filling = false;
            FillAirports(selectIcao, goTo);
        }

        private void FillAirports(string? selectIcao, bool goTo = true)
        {
            if (_cmbCountry.SelectedItem is not Country country || _cmbCity.SelectedItem is not string city) return;
            _filling = true;
            _cmbAirport.Items.Clear();
            foreach (Airport a in _db.Airports(country.Code, city)) _cmbAirport.Items.Add(a);
            Airport? pick = _cmbAirport.Items.Cast<Airport>().FirstOrDefault(a => selectIcao != null && a.Icao == selectIcao)
                            ?? _cmbAirport.Items.Cast<Airport>().FirstOrDefault();
            _cmbAirport.SelectedItem = pick;
            _filling = false;
            if (pick != null && goTo) GoToAirport(pick);
            else if (pick != null) _airport = pick;
        }

        /// <summary>Puts the aircraft on the ground at the airport. The heading is left as it is.</summary>
        private void GoToAirport(Airport a)
        {
            _airport = a;
            _lat = a.Lat; _lon = a.Lon; _alt = a.ElevationFt; _spd = 0;
            RefreshFields(force: true);
        }

        private void g_AddTimeRow(TableLayoutPanel g)
        {
            var now = new Button { Text = "PC clock", AutoSize = true };
            now.Click += (_, _) => { _clockOffset = TimeSpan.Zero; ShowClock(force: true); };
            g.Controls.Add(new Label { Text = "Date / time (UTC)", AutoSize = true, Margin = new Padding(0, 6, 6, 0) });
            g.Controls.Add(_txtTime);
            var right = new FlowLayoutPanel { AutoSize = true, FlowDirection = FlowDirection.LeftToRight, WrapContents = false, Margin = new Padding(0) };
            right.Controls.AddRange(new Control[] { now, _chkTime });
            g.Controls.Add(right);
            _txtTime.KeyDown += (_, e) => { if (e.KeyCode == Keys.Enter) { CommitClock(); e.SuppressKeyPress = true; ActiveControl = null; } };
            _txtTime.Leave += (_, _) => CommitClock();
        }

        /// <summary>Sets the clock to the typed UTC date/time; it then keeps running in real time.</summary>
        private void CommitClock()
        {
            if (DateTime.TryParseExact(_txtTime.Text.Trim(), TimeFormat, CultureInfo.InvariantCulture,
                    DateTimeStyles.AssumeUniversal | DateTimeStyles.AdjustToUniversal, out DateTime t))
            {
                _clockOffset = t - DateTime.UtcNow;
                _txtTime.BackColor = SystemColors.Window;
            }
            else _txtTime.BackColor = Color.MistyRose;
            ShowClock(force: false);
        }

        private DateTime SimUtc => DateTime.UtcNow + _clockOffset;

        private void ShowClock(bool force)
        {
            if (!force && (_txtTime.Focused || _txtTime.BackColor == Color.MistyRose)) return;
            _txtTime.Text = SimUtc.ToString(TimeFormat, CultureInfo.InvariantCulture);
            _txtTime.BackColor = SystemColors.Window;
        }

        private static void AddRow(TableLayoutPanel g, string label, TextBox box, string unit)
        {
            g.Controls.Add(new Label { Text = label, AutoSize = true, Margin = new Padding(0, 6, 6, 0) });
            g.Controls.Add(box);
            g.Controls.Add(new Label { Text = unit, AutoSize = true, Margin = new Padding(4, 6, 0, 0) });
        }

        // ------------------------------------------------------------ typed values
        private void Commit(TextBox box, Action<double> apply)
        {
            if (double.TryParse(box.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out double v) && double.IsFinite(v))
            {
                apply(v);
                box.BackColor = SystemColors.Window;
            }
            else box.BackColor = Color.MistyRose;      // keep the typed text visible so it can be corrected
            RefreshFields(force: false);
        }

        /// <summary>Writes the current values into the boxes (skipping a box being edited, unless <paramref name="force"/>).</summary>
        private void RefreshFields(bool force)
        {
            Set(_txtLat, _lat.ToString("0.00000", CultureInfo.InvariantCulture), force);
            Set(_txtLon, _lon.ToString("0.00000", CultureInfo.InvariantCulture), force);
            Set(_txtAlt, _alt.ToString("0.#", CultureInfo.InvariantCulture), force);
            Set(_txtSpd, _spd.ToString("0.#", CultureInfo.InvariantCulture), force);
            Set(_txtHdg, _hdg.ToString("0.#", CultureInfo.InvariantCulture), force);
        }

        private void Set(TextBox box, string text, bool force)
        {
            if (!force && (box.Focused || box.BackColor == Color.MistyRose)) return;
            box.Text = text;
            box.BackColor = SystemColors.Window;
        }

        // ------------------------------------------------------------ 8-way cursor
        private static bool IsKeyDown(Keys k) => (GetAsyncKeyState((int)k) & 0x8000) != 0;

        private Compass? CurrentDirection()
        {
            if (_buttonDir != null) return _buttonDir;
            // arrow keys only count while this window is active and no value is being typed
            if (!ContainsFocus || ActiveControl is TextBox) return null;
            return GeoMove.FromKeys(IsKeyDown(Keys.Up), IsKeyDown(Keys.Down), IsKeyDown(Keys.Left), IsKeyDown(Keys.Right));
        }

        private void CursorTick()
        {
            Compass? dir = CurrentDirection();
            bool move = false;
            if (dir == null) _lastDir = null;
            else if (dir != _lastDir) { _lastDir = dir; _holdStart = DateTime.UtcNow; move = true; }                 // a press: one step at once
            else if ((DateTime.UtcNow - _holdStart).TotalMilliseconds >= RepeatDelayMs) move = true;                    // held: repeat

            if (move && dir != null)
            {
                (_lat, _lon) = GeoMove.Move(_lat, _lon, dir.Value, (double)_step.Value);
                if (_chkHeading.Checked) _hdg = GeoMove.Bearing(dir.Value);
            }
            Fly();
            RefreshFields(force: false);
            ShowClock(force: false);
        }

        /// <summary>
        /// Fly mode: advances the position along the heading at the current speed (altitude, speed and heading stay constant).
        /// Distance comes from the real time since the last tick, multiplied by the Time x setting.
        /// </summary>
        private void Fly()
        {
            long now = System.Diagnostics.Stopwatch.GetTimestamp();
            double dt = System.Diagnostics.Stopwatch.GetElapsedTime(_lastFlyTicks, now).TotalSeconds;
            _lastFlyTicks = now;
            if (!_chkFly.Checked || _spd <= 0) return;
            double metres = _spd * GeoMove.KnotsToMetresPerSecond * Math.Min(dt, 1.0) * (double)_timeX.Value;
            (_lat, _lon) = GeoMove.MoveBearing(_lat, _lon, _hdg, metres);
        }

        // ------------------------------------------------------------ UDP
        /// <summary>The packet the GPS interface understands: D,LAT:..,LON:..,ALT:..(ft),IAS:..(kt),HDG:..(deg)</summary>
        public static string BuildPacket(double lat, double lon, double altFt, double spdKt, double hdg, DateTime? utc = null)
        {
            var inv = CultureInfo.InvariantCulture;
            string packet = string.Create(inv, $"D,LAT:{lat:0.000000},LON:{lon:0.000000},ALT:{altFt:0.#},IAS:{spdKt:0.#},HDG:{hdg:0.#}");
            return utc == null ? packet : packet + ",UTC:" + utc.Value.ToString("yyyyMMddHHmmss.fff", inv);
        }

        private void SendNow()
        {
            string packet = BuildPacket(_lat, _lon, _alt, _spd, _hdg, _chkTime.Checked ? SimUtc : null);
            _lblPacket.Text = packet;
            if (_chkSend.Checked)
            {
                try
                {
                    if (!int.TryParse(_port.Text, out int port) || port < 1 || port > 65535) throw new FormatException("bad port");
                    byte[] b = Encoding.ASCII.GetBytes(packet);
                    _udp.Send(b, b.Length, _host.Text.Trim(), port);
                    _sent++;
                    _status = "sending";
                }
                catch (Exception ex) { _status = "error: " + ex.Message; }
            }
            else _status = "paused";
            _lblSent.Text = $"Packets sent: {_sent}  ({_status})";
        }
    }
}
