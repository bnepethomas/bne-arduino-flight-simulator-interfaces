using System.Net.Sockets;
using System.Text;

namespace XPlaneBridge
{
    // X-Plane bridge for the Jet Ranger panels (output only): subscribes to the datarefs named in the profile over X-Plane's UDP interface (no plugin), converts them
    // to the units the boards take and sends them in the same "D,TOKEN:value" packets the other bridges use. profiles\_common.json works for any aircraft; an
    // aircraft profile (chosen in the Profile box: X-Plane does not give a dataref that names the aircraft) adds what depends on the aircraft.
    public sealed class Form1 : Form
    {
        const double StaleAfterSeconds = 3.0;
        const string GpsHost = "127.0.0.1";
        const int GpsPort = 27002;

        readonly XPlaneState state = new();
        readonly XPlaneLink link;
        readonly Dictionary<string, UdpClient> clients = new();
        readonly UdpClient gpsClient = new();
        readonly Dictionary<string, DateTime> lastSent = new();
        readonly Dictionary<string, string> lastRadioPacket = new();

        readonly string profileDir;
        readonly string datarefFile;
        List<Profile> profiles = new();
        Profile? commonProfile;
        Profile profile = new();
        List<ValueResult> lastResults = new();
        DatarefCatalog? catalog;
        string loadedFor = "?";
        bool manualProfile;

        readonly Label lblXPlane = new() { AutoSize = true };
        readonly Label lblProfile = new() { AutoSize = true };
        readonly ComboBox cboProfile = new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 240 };
        readonly CheckBox chkSend = new() { Text = "Send to the boards", Checked = true, AutoSize = true };
        readonly Button btnReload = new() { Text = "Reload profiles", AutoSize = true };
        readonly Button btnSearch = new() { Text = "Search datarefs...", AutoSize = true };
        readonly ListView lvValues = new() { View = View.Details, FullRowSelect = true, GridLines = true, Dock = DockStyle.Fill };
        readonly TextBox txtLog = new() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Bottom, Height = 150 };
        readonly System.Windows.Forms.Timer timer = new() { Interval = 100 };

        public Form1()
        {
            Text = "X-Plane Bridge";
            ClientSize = new Size(980, 620);
            MinimumSize = new Size(760, 450);

            var args = Program.Args;
            profileDir = args.GetValueOrDefault("profile-dir") ?? Path.Combine(AppContext.BaseDirectory, "profiles");
            datarefFile = args.GetValueOrDefault("datarefs") ?? DefaultDatarefFile();
            if (args.ContainsKey("no-send")) chkSend.Checked = false;

            string? boardHost = args.GetValueOrDefault("board-host");
            foreach (var (name, host) in Boards.All)
            {
                var c = new UdpClient();
                c.Connect(boardHost ?? host, Boards.ControlPort);
                clients[name] = c;
            }
            gpsClient.Connect(GpsHost, GpsPort);

            string? xhost = args.GetValueOrDefault("xplane-host");
            int xport = int.TryParse(args.GetValueOrDefault("xplane-port"), out var p) ? p : 49000;
            link = new XPlaneLink(state, xhost ?? "127.0.0.1", xport);

            var top = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 8, 8, 0), FlowDirection = FlowDirection.TopDown };
            top.Controls.AddRange(new Control[] { lblXPlane, lblProfile });
            var bar = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(8, 4, 8, 4) };
            bar.Controls.AddRange(new Control[] { new Label { Text = "Profile", AutoSize = true, Margin = new Padding(0, 6, 3, 0) }, cboProfile, btnReload, btnSearch, chkSend });

            lvValues.Columns.Add("Token", 170);
            lvValues.Columns.Add("X-Plane dataref", 400);
            lvValues.Columns.Add("Raw", 80);
            lvValues.Columns.Add("Sent to board", 100);
            lvValues.Columns.Add("Board", 90);
            lvValues.Columns.Add("Status", 100);
            Controls.Add(lvValues);
            Controls.Add(txtLog);
            Controls.Add(bar);
            Controls.Add(top);

            cboProfile.SelectionChangeCommitted += (_, _) => { manualProfile = cboProfile.SelectedIndex > 0; loadedFor = "?"; };
            btnReload.Click += (_, _) => { LoadProfiles(); loadedFor = "?"; };
            btnSearch.Click += (_, _) => Search();
            timer.Tick += (_, _) => Tick();
            FormClosing += (_, _) => { timer.Stop(); link.Dispose(); };

            link.Start(xhost == null);
            Log("Profiles: " + profileDir);
            Log("X-Plane: " + link.Target + (xhost == null ? " (a different address is taken from X-Plane's beacon if it is heard)" : ""));
            Log("Board packets go to " + (boardHost != null ? boardHost : "the boards") + ", the GPS interface to " + GpsHost + ":" + GpsPort);
            catalog = DatarefCatalog.Load(datarefFile);
            Log(catalog != null ? "DataRefs.txt: " + catalog.Entries.Count + " datarefs read from " + datarefFile : "DataRefs.txt not found at " + datarefFile + " (the Search button needs it; use --datarefs <file>)");
            LoadProfiles();
            if (args.TryGetValue("module", out var forced))
            {
                int i = profiles.FindIndex(x => x.Module.Equals(forced, StringComparison.OrdinalIgnoreCase));
                if (i >= 0) { cboProfile.SelectedIndex = i + 1; manualProfile = true; }
            }
            timer.Start();
        }

        static string DefaultDatarefFile()
        {
            foreach (var d in new[] { @"C:\X-Plane 12", @"C:\X-Plane 11", @"D:\X-Plane 12", @"D:\X-Plane 11" })
            {
                string f = Path.Combine(d, "Resources", "plugins", "DataRefs.txt");
                if (File.Exists(f)) return f;
            }
            return @"C:\X-Plane 12\Resources\plugins\DataRefs.txt";
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
                    try
                    {
                        var pr = Profile.Load(f);
                        if (name.StartsWith("_")) commonProfile = pr; else profiles.Add(pr);
                    }
                    catch (Exception ex) { Log("Profile " + name + ": " + ex.Message); }
                }
            }
            string keep = cboProfile.SelectedItem as string ?? "";
            cboProfile.Items.Clear();
            cboProfile.Items.Add("Generic (common values, any aircraft)");
            foreach (var pr in profiles) cboProfile.Items.Add(pr.Module);
            int idx = manualProfile ? cboProfile.Items.IndexOf(keep) : 0;
            cboProfile.SelectedIndex = idx >= 0 ? idx : 0;
            if (idx < 0) manualProfile = false;
            Log("Loaded " + profiles.Count + " aircraft profile(s)" + (commonProfile != null ? " and the common profile (" + commonProfile.Values.Count + " values for every aircraft)" : ""));
        }

        void SelectProfile()
        {
            Profile? want = manualProfile && cboProfile.SelectedIndex > 0 ? profiles[cboProfile.SelectedIndex - 1] : null;
            string key = want?.FileName ?? "";
            if (key == loadedFor) return;
            loadedFor = key;
            profile = Profile.Merge(commonProfile, want);
            lastRadioPacket.Clear();
            lastResults = new();
            link.SetDatarefs(profile.AllDatarefs());
            lvValues.Items.Clear();
            foreach (var v in profile.Values)
            {
                var item = new ListViewItem(v.Token);
                item.SubItems.Add((v.Type == "hhmm" || v.Type == "utc") ? string.Join(" ", v.Datarefs) : v.Dataref + (v.Divisor.Length > 0 ? "  /  " + v.Divisor : ""));
                item.SubItems.Add("");
                item.SubItems.Add("");
                item.SubItems.Add(Boards.BoardFor(v.Token) is { Length: > 0 } b ? b : "(power input)");
                item.SubItems.Add("");
                lvValues.Items.Add(item);
            }
            Log("Profile " + profile.Module + ": " + profile.Values.Count + " values, " + link.Datarefs.Count + " datarefs subscribed at " + link.FrequencyHz + " Hz");
        }

        void Search()
        {
            if (catalog == null) { Log("Search: DataRefs.txt was not found"); return; }
            string? text = Prompt("Search datarefs", "Words to look for in the dataref names and descriptions (all of them must match), for example  oil pressure  or  rotor:");
            if (string.IsNullOrWhiteSpace(text)) return;
            var found = catalog.Search(text);
            Log("Search \"" + text + "\": " + (found.Count == 0 ? "no datarefs" : found.Count + " best matches"));
            foreach (var e in found) Log("   " + e.Name + "   [" + e.Type + ", " + e.Units + "]   " + (e.Description.Length > 70 ? e.Description.Substring(0, 70) : e.Description));
        }

        static string? Prompt(string title, string message)
        {
            using var f = new Form { Text = title, FormBorderStyle = FormBorderStyle.FixedDialog, StartPosition = FormStartPosition.CenterParent, MinimizeBox = false, MaximizeBox = false, ClientSize = new Size(440, 150) };
            var lbl = new Label { Text = message, Left = 12, Top = 10, Width = 416, Height = 50 };
            var box = new TextBox { Left = 12, Top = 66, Width = 416 };
            var ok = new Button { Text = "Search", Left = 250, Top = 108, Width = 85, DialogResult = DialogResult.OK };
            var cancel = new Button { Text = "Cancel", Left = 343, Top = 108, Width = 85, DialogResult = DialogResult.Cancel };
            f.Controls.AddRange(new Control[] { lbl, box, ok, cancel });
            f.AcceptButton = ok;
            f.CancelButton = cancel;
            return f.ShowDialog() == DialogResult.OK ? box.Text : null;
        }

        void Tick()
        {
            SelectProfile();
            link.Tick();
            double age = state.SecondsSincePacket;
            bool live = age <= StaleAfterSeconds;
            lblXPlane.Text = "X-Plane " + link.Target + ": " + (state.Packets == 0 ? "no data yet (X-Plane running? " + link.Status + ")" : (live ? "receiving, " + state.Count + " datarefs" : "no data for " + age.ToString("F0") + " s - not sending"));
            lblXPlane.ForeColor = live ? Color.Green : Color.DarkOrange;
            lblProfile.Text = "Profile: " + profile.Module;
            if (!live) return;

            lastResults = profile.Evaluate(link, state);
            var tokens = new Dictionary<string, string>();
            foreach (var r in lastResults) tokens[r.Token] = r.Sent;
            for (int i = 0; i < profile.Values.Count && i < lvValues.Items.Count; i++)
            {
                var r = lastResults.FirstOrDefault(x => x.Token == profile.Values[i].Token);
                lvValues.Items[i].SubItems[2].Text = r?.Shown ?? "";
                lvValues.Items[i].SubItems[3].Text = r?.Sent ?? "";
                lvValues.Items[i].SubItems[5].Text = r?.Status ?? "no data";
            }
            if (!chkSend.Checked || tokens.Count == 0) return;
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
