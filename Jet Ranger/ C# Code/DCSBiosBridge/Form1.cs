using System.Globalization;
using System.Net.Sockets;
using System.Text;

namespace DCSBiosBridge
{
    // DCS-BIOS bridge for the Jet Ranger panels (output only for now): listens to the DCS-BIOS export stream, reads the values named in the aircraft's
    // profile and sends them to the panel boards in the same "D,TOKEN:value" packets the FSUIPC and MSFS bridges use.
    public sealed class Form1 : Form
    {
        // The DCS-BIOS metadata module puts the name of the aircraft in a string at the start of the memory image (address 0x0000, 24 characters).
        // This is from the DCS-BIOS Arduino library's _ACFT_NAME; if the name shown here is empty or garbage, pick the profile by hand.
        const int AircraftNameAddress = 0x0000;
        const int AircraftNameLength = 24;
        const double StaleAfterSeconds = 3.0;
        const string GpsHost = "127.0.0.1";
        const int GpsPort = 27002;

        readonly ExportState state = new();
        readonly ExportListener listener;
        readonly Dictionary<string, UdpClient> clients = new();
        readonly UdpClient gpsClient = new();
        readonly Dictionary<string, DateTime> lastSent = new();
        readonly Dictionary<string, string> lastRadioPacket = new();

        readonly string profileDir;
        readonly string jsonDir;
        List<Profile> profiles = new();
        Profile? profile;                  // the profile in use: the aircraft's own values plus the common ones
        Profile? commonProfile;            // profiles\_common.json: what every aircraft exports (airspeed, altitude, heading, position)
        Profile? aircraftProfile;
        List<ValueResult> lastResults = new();
        string calibrationPath = "";
        Dictionary<string, List<double[]>> calibration = new();
        Dictionary<string, BiosControl> controls = new(StringComparer.OrdinalIgnoreCase);
        string aircraft = "";
        string loadedFor = "";
        bool manualProfile;

        readonly Label lblStream = new() { AutoSize = true };
        readonly Label lblAircraft = new() { AutoSize = true };
        readonly Label lblProfile = new() { AutoSize = true };
        readonly ComboBox cboProfile = new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 220 };
        readonly CheckBox chkSend = new() { Text = "Send to the boards", Checked = true, AutoSize = true };
        readonly Button btnReload = new() { Text = "Reload profiles", AutoSize = true };
        readonly Button btnSuggest = new() { Text = "Suggest controls", AutoSize = true };
        readonly Button btnCalibrate = new() { Text = "Calibrate selected...", AutoSize = true };
        readonly Button btnClearCal = new() { Text = "Clear its calibration", AutoSize = true };
        readonly ListView lvValues = new() { View = View.Details, FullRowSelect = true, GridLines = true, Dock = DockStyle.Fill };
        readonly TextBox txtLog = new() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Bottom, Height = 160 };
        readonly System.Windows.Forms.Timer timer = new() { Interval = 100 };

        public Form1()
        {
            Text = "DCS-BIOS Bridge";
            ClientSize = new Size(900, 600);
            MinimumSize = new Size(700, 450);

            var args = Program.Args;
            profileDir = args.GetValueOrDefault("profile-dir") ?? Path.Combine(AppContext.BaseDirectory, "profiles");
            jsonDir = args.GetValueOrDefault("json-dir") ?? DefaultJsonDir();
            if (args.ContainsKey("no-send")) chkSend.Checked = false;

            string? boardHost = args.GetValueOrDefault("board-host");
            foreach (var (name, host) in Boards.All)
            {
                var c = new UdpClient();
                c.Connect(boardHost ?? host, Boards.ControlPort);
                clients[name] = c;
            }
            gpsClient.Connect(GpsHost, GpsPort);

            var top = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 8, 8, 0) };
            top.Controls.AddRange(new Control[] { lblStream, lblAircraft, lblProfile });
            top.FlowDirection = FlowDirection.TopDown;
            var bar = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 4, 8, 4) };
            bar.Controls.AddRange(new Control[] { new Label { Text = "Profile", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, cboProfile, btnReload, btnSuggest, btnCalibrate, btnClearCal, chkSend });

            lvValues.Columns.Add("Token", 180);
            lvValues.Columns.Add("DCS-BIOS control", 230);
            lvValues.Columns.Add("Raw", 70);
            lvValues.Columns.Add("Sent to board", 100);
            lvValues.Columns.Add("Board", 90);
            lvValues.Columns.Add("Status", 140);
            Controls.Add(lvValues);
            Controls.Add(txtLog);
            Controls.Add(bar);
            Controls.Add(top);

            cboProfile.SelectionChangeCommitted += (_, _) => { manualProfile = cboProfile.SelectedIndex > 0; loadedFor = ""; };
            btnReload.Click += (_, _) => { LoadProfiles(); loadedFor = ""; };
            btnSuggest.Click += (_, _) => Suggest();
            btnCalibrate.Click += (_, _) => Calibrate();
            btnClearCal.Click += (_, _) => ClearCalibration();
            timer.Tick += (_, _) => Tick();
            FormClosing += (_, _) => { timer.Stop(); listener?.Dispose(); };

            listener = new ExportListener(state);
            listener.Start();
            Log("Profiles: " + profileDir);
            Log("DCS-BIOS module files: " + jsonDir);
            Log("Stream: " + listener.Status + (boardHost != null ? "   (all board packets go to " + boardHost + ")" : ""));
            LoadProfiles();
            if (args.TryGetValue("module", out var forced))
            {
                int i = profiles.FindIndex(p => p.Module.Equals(forced, StringComparison.OrdinalIgnoreCase));
                if (i >= 0) { cboProfile.SelectedIndex = i + 1; manualProfile = true; }
            }
            timer.Start();
        }

        static string DefaultJsonDir()
        {
            string saved = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Saved Games");
            foreach (var dcs in new[] { "DCS", "DCS.openbeta" })
            {
                string d = Path.Combine(saved, dcs, "Scripts", "DCS-BIOS", "doc", "json");
                if (Directory.Exists(d)) return d;
            }
            return Path.Combine(saved, "DCS", "Scripts", "DCS-BIOS", "doc", "json");
        }

        void Log(string text)
        {
            string line = DateTime.Now.ToString("HH:mm:ss") + "  " + text + Environment.NewLine;
            if (Program.Args.TryGetValue("logfile", out var logFile))
            {
                try { File.AppendAllText(logFile, line); } catch (IOException) { }
            }
            txtLog.AppendText(line);
            if (txtLog.Lines.Length > 400) txtLog.Lines = txtLog.Lines.Skip(200).ToArray();
        }

        void LoadProfiles()
        {
            profiles = new List<Profile>();
            commonProfile = null;
            if (Directory.Exists(profileDir))
            {
                foreach (var f in Directory.GetFiles(profileDir, "*.json").OrderBy(x => x))
                {
                    string name = Path.GetFileName(f);
                    if (name.EndsWith(".calibration.json", StringComparison.OrdinalIgnoreCase)) continue;
                    try
                    {
                        var p = Profile.Load(f);
                        if (name.StartsWith("_")) commonProfile = p; else profiles.Add(p);
                    }
                    catch (Exception ex) { Log("Profile " + name + ": " + ex.Message); }
                }
            }
            string keep = cboProfile.SelectedItem as string ?? "";
            cboProfile.Items.Clear();
            cboProfile.Items.Add("Automatic (from the aircraft name)");
            foreach (var p in profiles) cboProfile.Items.Add(p.Module);
            int idx = manualProfile ? cboProfile.Items.IndexOf(keep) : 0;
            cboProfile.SelectedIndex = idx >= 0 ? idx : 0;
            if (idx < 0) manualProfile = false;
            Log("Loaded " + profiles.Count + " aircraft profile(s)" + (commonProfile != null ? " and the common profile (" + commonProfile.Values.Count + " values for every aircraft)" : ""));
        }

        void LoadModuleFile(string file)
        {
            string path = Path.Combine(jsonDir, file);
            if (!File.Exists(path))
            {
                Log("The DCS-BIOS module file " + path + " does not exist. Install DCS-BIOS (its release includes the module files, or DCS writes them when it runs with DCS-BIOS installed).");
                return;
            }
            try { Log(file + ": " + ModuleDoc.LoadInto(path, controls) + " DCS-BIOS controls read"); }
            catch (Exception ex) { Log("Cannot read " + path + ": " + ex.Message); }
        }

        // Picks the aircraft profile (by the aircraft name in the stream, or by hand), adds the common values every aircraft can give and loads the DCS-BIOS definitions
        // (CommonData.json and the aircraft's own file) when either changes. An aircraft with no profile still gets the common values.
        void SelectProfile()
        {
            Profile? want = null;
            if (manualProfile && cboProfile.SelectedIndex > 0) want = profiles[cboProfile.SelectedIndex - 1];
            else want = profiles.FirstOrDefault(p => p.Matches(aircraft));
            string key = (want?.FileName ?? "") + "|" + aircraft;
            if (key == loadedFor) return;
            loadedFor = key;
            aircraftProfile = want;
            controls = new Dictionary<string, BiosControl>(StringComparer.OrdinalIgnoreCase);
            lvValues.Items.Clear();
            lastRadioPacket.Clear();
            lastResults = new();
            profile = Profile.Merge(commonProfile, want);
            if (profile == null) return;
            if (want == null && aircraft.Length > 0) Log("No profile for the aircraft \"" + aircraft + "\": only the common values are used");
            LoadModuleFile("CommonData.json");
            if (want != null) LoadModuleFile(want.JsonFile);
            calibrationPath = Calibration.PathFor(profileDir, want?.FileName ?? commonProfile?.FileName ?? "_common.json");
            calibration = Calibration.Load(calibrationPath);
            Calibration.Apply(profile, calibration);
            Log("Profile " + profile.Module + ": " + profile.Values.Count + " values" + (calibration.Count > 0 ? ", calibration for " + calibration.Count + " of them" : ""));
            foreach (var v in profile.Values)
            {
                var item = new ListViewItem(v.Token);
                string ctl = v.Controls.Count > 0 ? string.Join(" ", v.Controls) : v.Control;
                bool missing = v.Controls.Count > 0 ? v.Controls.Any(c => !controls.ContainsKey(c)) : (v.Control.Length > 0 && !controls.ContainsKey(v.Control));
                item.SubItems.Add(ctl.Length == 0 ? "(not mapped)" : ctl + (missing ? "  (not in the module file)" : ""));
                item.SubItems.Add("");
                item.SubItems.Add("");
                item.SubItems.Add(Boards.BoardFor(v.Token) is { Length: > 0 } b ? b : "(power input)");
                item.SubItems.Add("");
                lvValues.Items.Add(item);
            }
        }

        void Suggest()
        {
            if (profile == null) { Log("Suggest: no profile is selected"); return; }
            if (controls.Count == 0) { Log("Suggest: no DCS-BIOS module file is loaded"); return; }
            foreach (var v in profile.Values.Where(x => x.Controls.Count == 0 && (x.Control.Length == 0 || !controls.ContainsKey(x.Control))))
            {
                var found = Profile.Suggest(v.Hint.Length > 0 ? v.Hint : v.Token.Replace('_', ' '), controls);
                Log(v.Token + " (" + v.Hint + "): " + (found.Count == 0 ? "no candidates" : string.Join("; ", found.Select(c => c.Id + " [" + c.Description + "]"))));
            }
        }

        // Calibrate: the cockpit gauge is read by eye, the real value typed in, and the needle position DCS-BIOS reports at that moment is stored as a point of that value.
        // Two or more points replace the first-guess scale; points are kept in <profile>.calibration.json.
        void Calibrate()
        {
            if (profile == null || lvValues.SelectedIndices.Count == 0) { Log("Calibrate: select a value in the table first"); return; }
            int i = lvValues.SelectedIndices[0];
            var r = lastResults.FirstOrDefault(x => x.Token == profile.Values[i].Token);
            if (r == null || !r.Analog) { Log("Calibrate: " + profile.Values[i].Token + " is not an analog gauge value that is being read"); return; }
            string token = r.Token;
            double x = r.X;
            string? text = Prompt("Calibrate " + token,
                "The cockpit gauge for " + token + " now reads (in the units the panel board uses; the panel is sending " + r.Sent + "):\r\n\r\nDCS-BIOS position now: " + r.Shown + ".\r\nFly to another reading and repeat; two or more points are used.");
            if (text == null) return;
            if (!double.TryParse(text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out double real)) { Log("Calibrate: \"" + text + "\" is not a number"); return; }
            if (!calibration.TryGetValue(token, out var pts)) calibration[token] = pts = new List<double[]>();
            pts.RemoveAll(p => Math.Abs(p[0] - x) < (profile.Values[i].Raw ? 0.5 : 0.003));
            pts.Add(new[] { x, real });
            Calibration.Save(calibrationPath, calibration);
            Calibration.Apply(profile, calibration);
            Log("Calibrate " + token + ": " + x.ToString("0.###", CultureInfo.InvariantCulture) + " = " + real.ToString(CultureInfo.InvariantCulture) + " (" + pts.Count + " point(s), saved to " + Path.GetFileName(calibrationPath) + ")");
        }

        void ClearCalibration()
        {
            if (profile == null || lvValues.SelectedIndices.Count == 0) { Log("Clear calibration: select a value in the table first"); return; }
            var v = profile.Values[lvValues.SelectedIndices[0]];
            if (calibration.Remove(v.Token)) { Calibration.Save(calibrationPath, calibration); v.Points = null; Log("Calibration of " + v.Token + " cleared"); }
            else Log(v.Token + " has no calibration points");
        }

        static string? Prompt(string title, string message)
        {
            using var f = new Form { Text = title, FormBorderStyle = FormBorderStyle.FixedDialog, StartPosition = FormStartPosition.CenterParent, MinimizeBox = false, MaximizeBox = false, ClientSize = new Size(440, 190) };
            var lbl = new Label { Text = message, Left = 12, Top = 10, Width = 416, Height = 90 };
            var box = new TextBox { Left = 12, Top = 108, Width = 200 };
            var ok = new Button { Text = "Add point", Left = 250, Top = 150, Width = 85, DialogResult = DialogResult.OK };
            var cancel = new Button { Text = "Cancel", Left = 343, Top = 150, Width = 85, DialogResult = DialogResult.Cancel };
            f.Controls.AddRange(new Control[] { lbl, box, ok, cancel });
            f.AcceptButton = ok;
            f.CancelButton = cancel;
            return f.ShowDialog() == DialogResult.OK ? box.Text : null;
        }

        void Tick()
        {
            aircraft = state.Frames > 0 ? state.ReadString(AircraftNameAddress, AircraftNameLength) : "";
            double age = state.SecondsSinceFrame;
            bool live = age <= StaleAfterSeconds;
            lblStream.Text = "DCS-BIOS stream: " + (state.Frames == 0 ? listener.Status + " - no data yet" : (live ? "receiving" : "no data for " + age.ToString("F0") + " s - not sending")) + "   (" + state.Frames + " frames)";
            lblStream.ForeColor = live ? Color.Green : Color.DarkOrange;
            lblAircraft.Text = "Aircraft: " + (aircraft.Length > 0 ? aircraft : "(not known yet)");
            SelectProfile();
            lblProfile.Text = "Profile: " + (profile?.Module ?? "none");
            if (profile == null) return;

            lastResults = profile.Evaluate(state, controls);
            var tokens = new Dictionary<string, string>();
            foreach (var r in lastResults) tokens[r.Token] = r.Sent;
            for (int i = 0; i < profile.Values.Count && i < lvValues.Items.Count; i++)
            {
                var r = lastResults.FirstOrDefault(x => x.Token == profile.Values[i].Token);
                lvValues.Items[i].SubItems[2].Text = r?.Shown ?? "";
                lvValues.Items[i].SubItems[3].Text = r?.Sent ?? "";
                lvValues.Items[i].SubItems[5].Text = r?.Status ?? "";
            }
            if (!live || !chkSend.Checked || tokens.Count == 0) return;
            Send(Payloads.Build(tokens));
        }

        // The same cadence as the other bridges: the radio on change (at most every 200 ms) or every 5 s, the other boards about every 90 ms, the GPS twice a second.
        void Send(Dictionary<string, string> packets)
        {
            DateTime now = DateTime.Now;
            foreach (var (board, packet) in packets)
            {
                bool radio = board == "Radio";
                double ms = lastSent.TryGetValue(board, out var t) ? (now - t).TotalMilliseconds : double.MaxValue;
                if (board == "Gps") { if (ms >= 500) { Transmit(gpsClient, packet); lastSent[board] = now; } continue; }
                bool changed = radio && (!lastRadioPacket.TryGetValue(board, out var last) || last != packet);
                if (radio ? ((changed && ms >= 200) || ms >= 5000) : ms >= 90)
                {
                    Transmit(clients[board], packet);
                    lastSent[board] = now;
                    if (radio) lastRadioPacket[board] = packet;
                }
            }
        }

        static void Transmit(UdpClient client, string packet)
        {
            // a board that is switched off can raise a socket error on a later send; that must never stop the bridge
            try { byte[] d = Encoding.ASCII.GetBytes(packet); client.Send(d, d.Length); }
            catch (SocketException) { }
            catch (ObjectDisposedException) { }
        }
    }
}
