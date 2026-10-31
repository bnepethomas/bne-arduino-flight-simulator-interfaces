using System.Diagnostics;
using System.Globalization;
using System.Text.Json;

namespace JetRangerPanelEmulator
{
    // The needle of one gauge: it moves towards the value the board was told at a limited speed (a stepper does not jump), swings to the end of the scale and back
    // when told to, and returns to its calibrated zero when the board has no data.
    public sealed class Gauge
    {
        public DialSpec Spec;
        public double Target;          // the last value sent
        public double Needle;          // what is drawn
        public bool HasValue;
        int swingPhase;                // 1 = going to the top of the scale, 2 = coming back
        public double FractionPerSecond = 0.55;      // of the whole scale per second

        public Gauge(DialSpec spec) { Spec = spec; Target = Needle = spec.Rest; }

        public void Swing() => swingPhase = 1;

        public void Animate(double dt, bool noData)
        {
            double goal = Target;
            if (noData || !HasValue) goal = Spec.Rest;
            if (swingPhase == 1) goal = Spec.Max;
            else if (swingPhase == 2) goal = Spec.Rest;
            double step = (Spec.Max - Spec.Min) * FractionPerSecond * dt;
            if (Math.Abs(goal - Needle) <= step)
            {
                Needle = goal;
                if (swingPhase == 1) swingPhase = 2;
                else if (swingPhase == 2) swingPhase = 0;
            }
            else Needle += Math.Sign(goal - Needle) * step;
        }
    }

    sealed class DoubleBufferedPanel : Panel
    {
        public DoubleBufferedPanel() { DoubleBuffered = true; ResizeRedraw = true; SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer, true); }
    }

    // The window of one emulated board. It binds the board's UDP address, applies what every board does with a packet (the backlight power, the brightness, the
    // reset, the 30 second no-data watchdog) and leaves the board's own tokens and drawing to the subclass.
    public abstract class BoardWindow : Form
    {
        public abstract string BoardName { get; }               // radio, servo, stepper, dual, adf
        public abstract int BoardNumber { get; }                // the last number of the board's address
        protected abstract string Heading { get; }

        public static double WatchdogSeconds = 30.0;
        public static string? DumpDirectory;

        PanelLink? link;
        readonly DoubleBufferedPanel canvas = new() { Dock = DockStyle.Fill, BackColor = Color.FromArgb(38, 40, 44) };
        readonly StatusStrip status = new();
        readonly ToolStripStatusLabel lblAddress = new(), lblPackets = new() { Spring = true, TextAlign = ContentAlignment.MiddleLeft }, lblState = new();
        readonly System.Windows.Forms.Timer timer = new() { Interval = 33 };
        readonly Stopwatch clock = Stopwatch.StartNew();
        double lastTick, lastDataAt, lastDump;
        int packets;
        string lastPacket = "";
        bool? backlightPower;                                   // BPWR: null until the first one arrives
        int? brightness;                                        // C,B:0-15: null until the first one arrives

        protected double Now => clock.Elapsed.TotalSeconds;
        protected bool NoData => Now - lastDataAt > WatchdogSeconds;
        // The panel backlight as 0 to 1: off with BPWR:0 and while the board has no data; otherwise the commanded brightness (a bit under full until one arrives).
        protected double Light => NoData || backlightPower == false ? 0 : (brightness is int b ? b / 15.0 : 0.7);

        protected BoardWindow(string net)
        {
            Text = "Jet Ranger panel emulator";
            StartPosition = FormStartPosition.Manual;
            BackColor = Color.FromArgb(38, 40, 44);
            canvas.Paint += (_, e) => { Gfx.Smooth(e.Graphics); Draw(e.Graphics, canvas.ClientSize); };
            status.Items.AddRange(new ToolStripItem[] { lblAddress, lblPackets, lblState });
            Controls.Add(canvas);
            Controls.Add(status);
            timer.Tick += (_, _) => OnTimer();
            Shown += (_, _) => Start(net);
            FormClosed += (_, _) => { timer.Stop(); link?.Dispose(); };
        }

        void Start(string net)
        {
            string address = net + "." + BoardNumber;
            Text = Heading + "  -  emulator at " + address + ":" + PanelLink.Port;
            lblAddress.Text = address + ":" + PanelLink.Port;
            lastTick = Now;
            lastDataAt = Now;                                    // a real board waits 30 seconds for the first packet before it blanks
            try
            {
                link = new PanelLink(address);
                link.Received += text => { if (!IsDisposed && IsHandleCreated) BeginInvoke(() => OnText(text)); };
                lblPackets.Text = "waiting for packets";
            }
            catch (Exception ex)
            {
                lblPackets.Text = "cannot listen on " + address + ":" + PanelLink.Port + " - " + ex.Message;
            }
            timer.Start();
        }

        void OnText(string text)
        {
            var p = Packet.Parse(text);
            if (p == null) return;
            packets++;
            lastPacket = p.Raw.Length > 160 ? p.Raw.Substring(0, 160) + "..." : p.Raw;
            bool counts = p.Kind == 'D';
            foreach (var (key, value) in p.Items)
            {
                if (p.Kind == 'D')
                {
                    if (key == "BPWR") backlightPower = value != "0";
                    else HandleData(key, value);
                }
                else
                {
                    if (key == "B" && int.TryParse(value, out int level)) { brightness = Math.Clamp(level, 0, 15); counts = true; }
                    else if (key == "RESET") { ResetBoard(); counts = true; }
                    else
                    {
                        if (key != "DISPLAY") counts = true;
                        HandleControl(key, value);
                    }
                }
            }
            if (counts) lastDataAt = Now;
        }

        void ResetBoard()
        {
            backlightPower = null;
            brightness = null;
            ResetState();
        }

        void OnTimer()
        {
            double now = Now, dt = Math.Min(0.25, now - lastTick);
            lastTick = now;
            Animate(dt);
            canvas.Invalidate();
            lblState.Text = (NoData ? "no data for " + (now - lastDataAt).ToString("F0") + " s - blank" : "live") + "   backlight " +
                            (Light <= 0 ? "off" : (brightness is int b ? "level " + b : "on")) + "   ";
            if (packets > 0) lblPackets.Text = packets + " packets, last " + (now - lastDataAt).ToString("F1") + " s ago: " + lastPacket;
            if (DumpDirectory != null && now - lastDump >= 0.25) { lastDump = now; Dump(); }
        }

        void Dump()
        {
            try
            {
                var state = State();
                state["_board"] = BoardName;
                state["_packets"] = packets.ToString(CultureInfo.InvariantCulture);
                state["_last"] = lastPacket;
                state["_noData"] = NoData ? "1" : "0";
                state["_light"] = Light.ToString("F2", CultureInfo.InvariantCulture);
                Directory.CreateDirectory(DumpDirectory!);
                string path = Path.Combine(DumpDirectory!, BoardName + ".json");
                File.WriteAllText(path + ".tmp", JsonSerializer.Serialize(state));
                File.Move(path + ".tmp", path, true);
            }
            catch (IOException) { }
        }

        protected virtual void HandleData(string token, string value) { }
        protected virtual void HandleControl(string name, string value) { }
        protected virtual void ResetState() { }
        protected virtual void Animate(double dt) { }
        protected abstract void Draw(Graphics g, Size size);
        public abstract Dictionary<string, string> State();

        protected static string Num(double v) => v.ToString("0.###", CultureInfo.InvariantCulture);
    }
}
