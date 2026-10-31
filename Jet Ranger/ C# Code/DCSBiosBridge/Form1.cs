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
        Profile? profile;
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
            bar.Controls.AddRange(new Control[] { new Label { Text = "Profile", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, cboProfile, btnReload, btnSuggest, chkSend });

            lvValues.Columns.Add("Token", 180);
            lvValues.Columns.Add("DCS-BIOS control", 260);
            lvValues.Columns.Add("Raw", 70);
            lvValues.Columns.Add("Sent to board", 110);
            lvValues.Columns.Add("Board", 100);
            Controls.Add(lvValues);
            Controls.Add(txtLog);
            Controls.Add(bar);
            Controls.Add(top);

            cboProfile.SelectionChangeCommitted += (_, _) => { manualProfile = cboProfile.SelectedIndex > 0; loadedFor = ""; };
            btnReload.Click += (_, _) => { LoadProfiles(); loadedFor = ""; };
            btnSuggest.Click += (_, _) => Suggest();
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
            if (Directory.Exists(profileDir))
            {
                foreach (var f in Directory.GetFiles(profileDir, "*.json").OrderBy(x => x))
                {
                    try { profiles.Add(Profile.Load(f)); }
                    catch (Exception ex) { Log("Profile " + Path.GetFileName(f) + ": " + ex.Message); }
                }
            }
            string keep = cboProfile.SelectedItem as string ?? "";
            cboProfile.Items.Clear();
            cboProfile.Items.Add("Automatic (from the aircraft name)");
            foreach (var p in profiles) cboProfile.Items.Add(p.Module);
            int idx = manualProfile ? cboProfile.Items.IndexOf(keep) : 0;
            cboProfile.SelectedIndex = idx >= 0 ? idx : 0;
            if (idx < 0) manualProfile = false;
            Log("Loaded " + profiles.Count + " profile(s)");
        }

        // Picks the profile (by the aircraft name in the stream, or by hand) and loads the module's DCS-BIOS definition when either changes.
        void SelectProfile()
        {
            Profile? want = null;
            if (manualProfile && cboProfile.SelectedIndex > 0) want = profiles[cboProfile.SelectedIndex - 1];
            else want = profiles.FirstOrDefault(p => p.Matches(aircraft));
            string key = (want?.FileName ?? "") + "|" + aircraft;
            if (key == loadedFor) return;
            loadedFor = key;
            profile = want;
            controls = new Dictionary<string, BiosControl>(StringComparer.OrdinalIgnoreCase);
            lvValues.Items.Clear();
            lastRadioPacket.Clear();
            if (profile == null)
            {
                if (aircraft.Length > 0) Log("No profile for the aircraft \"" + aircraft + "\"");
                return;
            }
            string path = Path.Combine(jsonDir, profile.JsonFile);
            if (!File.Exists(path))
            {
                Log("Profile " + profile.Module + ": the DCS-BIOS module file " + path + " does not exist. Install DCS-BIOS and start DCS once so it writes the module files.");
            }
            else
            {
                try
                {
                    controls = ModuleDoc.Load(path);
                    Log("Profile " + profile.Module + ": " + controls.Count + " DCS-BIOS controls read from " + profile.JsonFile);
                }
                catch (Exception ex) { Log("Cannot read " + path + ": " + ex.Message); }
            }
            foreach (var v in profile.Values)
            {
                var item = new ListViewItem(v.Token);
                item.SubItems.Add(v.Control.Length == 0 ? "(not mapped)" : v.Control + (controls.ContainsKey(v.Control) ? "" : "  (not in the module file)"));
                item.SubItems.Add("");
                item.SubItems.Add("");
                item.SubItems.Add(Boards.BoardFor(v.Token) is { Length: > 0 } b ? b : "(power input)");
                lvValues.Items.Add(item);
            }
        }

        void Suggest()
        {
            if (profile == null) { Log("Suggest: no profile is selected"); return; }
            if (controls.Count == 0) { Log("Suggest: no DCS-BIOS module file is loaded"); return; }
            foreach (var v in profile.Values.Where(x => x.Control.Length == 0 || !controls.ContainsKey(x.Control)))
            {
                var found = Profile.Suggest(v.Hint.Length > 0 ? v.Hint : v.Token.Replace('_', ' '), controls);
                Log(v.Token + " (" + v.Hint + "): " + (found.Count == 0 ? "no candidates" : string.Join("; ", found.Select(c => c.Id + " [" + c.Description + "]"))));
            }
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

            var raw = new Dictionary<string, double>();
            var tokens = profile.Evaluate(state, controls, raw);
            for (int i = 0; i < profile.Values.Count && i < lvValues.Items.Count; i++)
            {
                string t = profile.Values[i].Token;
                lvValues.Items[i].SubItems[2].Text = raw.TryGetValue(t, out var r) ? r.ToString("F0") : "";
                lvValues.Items[i].SubItems[3].Text = tokens.TryGetValue(t, out var s) ? s : "";
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
