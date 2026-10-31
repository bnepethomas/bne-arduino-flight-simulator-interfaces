namespace JetRangerPanelEmulator
{
    // The part the Stepper and Dual Stepper controllers share: a set of dial gauges that follow their tokens, swing on command and fall back to zero without data.
    public abstract class GaugeBoardWindow : BoardWindow
    {
        protected readonly List<Gauge> Gauges = new();
        protected Gauge? Find(string token) => Gauges.FirstOrDefault(g => g.Spec.Token == token);

        protected GaugeBoardWindow(string net) : base(net) { }

        protected override void HandleData(string token, string value)
        {
            var g = Find(token);
            if (g != null && Packet.TryNumber(value, out double v)) { g.Target = v; g.HasValue = true; }
        }

        protected override void HandleControl(string name, string value)
        {
            if (name.StartsWith("SWING", StringComparison.Ordinal)) Find(name.Substring(5))?.Swing();
        }

        protected override void ResetState()
        {
            foreach (var g in Gauges) { g.HasValue = false; g.Target = g.Spec.Rest; }
        }

        protected override void Animate(double dt)
        {
            foreach (var g in Gauges) g.Animate(dt, NoData);
        }

        public override Dictionary<string, string> State()
        {
            var d = new Dictionary<string, string>();
            foreach (var g in Gauges)
            {
                d[g.Spec.Token] = g.HasValue ? Num(g.Target) : "";
                d[g.Spec.Token + ".needle"] = Num(g.Needle);
            }
            return d;
        }

        // Dials laid out in a grid inside a rectangle.
        protected void DialGrid(Graphics g, RectangleF area, int columns, int rows, IList<Gauge> gauges)
        {
            float cw = area.Width / columns, ch = area.Height / rows;
            float r = Math.Min(cw, ch) / 2 * 0.92f;
            for (int i = 0; i < gauges.Count; i++)
            {
                float cx = area.X + cw * (i % columns + 0.5f), cy = area.Y + ch * (i / columns + 0.5f);
                Gfx.Dial(g, cx, cy, r, gauges[i].Spec, gauges[i].Needle, Light);
            }
        }
    }

    // The Stepper controller (172.16.1.105): airspeed, vertical speed, radar altitude, torque, gas producer, turbine out temperature and fuel quantity.
    public sealed class StepperWindow : GaugeBoardWindow
    {
        public override string BoardName => "stepper";
        public override int BoardNumber => 105;
        protected override string Heading => "Stepper controller";

        public StepperWindow(string net) : base(net)
        {
            foreach (var s in Specs.StepperGauges()) Gauges.Add(new Gauge(s));
            ClientSize = new Size(800, 470);
        }

        protected override void Draw(Graphics g, Size size)
        {
            Gfx.Caption(g, "Stepper controller .105  (13 Stepper PCB)", size.Width / 2f, 16, 15, Color.FromArgb(230, 230, 230), true);
            var top = new RectangleF(0, 30, size.Width, (size.Height - 30) / 2);
            DialGrid(g, new RectangleF(0, 30, size.Width, (size.Height - 30) / 2), 4, 1, Gauges.Take(4).ToList());
            float rowH = (size.Height - 30) / 2;
            DialGrid(g, new RectangleF(size.Width / 8f, 30 + rowH, size.Width * 0.75f, rowH), 3, 1, Gauges.Skip(4).ToList());
        }
    }

    // The Dual Stepper controller (172.16.1.106): oil, transmission, rotor and engine speed, electrical load, fuel pressure, the altimeter and its four OLEDs.
    public sealed class DualStepperWindow : GaugeBoardWindow
    {
        public override string BoardName => "dual";
        public override int BoardNumber => 106;
        protected override string Heading => "Dual Stepper controller";

        readonly Gauge altimeter = new(Specs.Altimeter());
        double altitudeFeet;
        bool hasAltitude;
        bool? power;                    // PWR: null until the first one arrives
        string zulu = "";
        string temp = "";
        bool oledTest;

        public DualStepperWindow(string net) : base(net)
        {
            foreach (var s in Specs.DualGauges()) Gauges.Add(new Gauge(s));
            ClientSize = new Size(900, 610);
        }

        bool OledsOn => !NoData && power != false;

        protected override void HandleData(string token, string value)
        {
            switch (token)
            {
                case "PWR": power = value != "0"; break;
                case "ALT":
                    if (Packet.TryNumber(value, out double ft)) { altitudeFeet = ft; hasAltitude = true; altimeter.Target = (ft % 1000 + 1000) % 1000 / 100.0; altimeter.HasValue = true; }
                    break;
                case "ZULU": zulu = value; break;
                case "TEMP": temp = value; break;
                default: base.HandleData(token, value); break;
            }
        }

        protected override void HandleControl(string name, string value)
        {
            if (name == "OLEDTEST") oledTest = value != "0";
            else if (name == "SWINGALT") altimeter.Swing();
            else base.HandleControl(name, value);
        }

        protected override void ResetState()
        {
            base.ResetState();
            power = null; zulu = temp = ""; hasAltitude = false; altimeter.HasValue = false; oledTest = false;
        }

        protected override void Animate(double dt)
        {
            base.Animate(dt);
            // the altimeter turns once per 1000 ft: take the short way round when the needle wraps from 10 to 0
            if (altimeter.HasValue && !NoData)
            {
                double diff = altimeter.Target - altimeter.Needle;
                if (Math.Abs(diff) > 5) altimeter.Needle = altimeter.Target;
            }
            altimeter.Animate(dt, NoData);
        }

        string Clock() => zulu.Length >= 4 && int.TryParse(zulu, out int hhmm) ? (hhmm / 100).ToString("00") + ":" + (hhmm % 100).ToString("00") : (zulu.Length > 0 ? zulu : "--:--");

        public override Dictionary<string, string> State()
        {
            var d = base.State();
            d["ALT"] = hasAltitude ? Num(altitudeFeet) : "";
            d["ALT.needle"] = Num(altimeter.Needle);
            d["PWR"] = power == null ? "" : (power == true ? "1" : "0");
            d["oled.baro"] = OledsOn ? "2992" : "";
            d["oled.alt"] = OledsOn && hasAltitude ? ((int)Math.Round(altitudeFeet)).ToString() : "";
            d["oled.clock"] = OledsOn && zulu.Length > 0 ? Clock() : "";
            d["oled.temp"] = OledsOn ? temp : "";
            return d;
        }

        protected override void Draw(Graphics g, Size size)
        {
            Gfx.Caption(g, "OLED Dual Stepper controller .106", size.Width / 2f, 16, 15, Color.FromArgb(230, 230, 230), true);
            float oledH = 120, top = 30;
            float areaH = size.Height - top - oledH;
            float dialW = size.Width * 0.72f;
            DialGrid(g, new RectangleF(0, top, dialW, areaH), 4, 2, Gauges);
            float ar = Math.Min(size.Width - dialW, areaH) / 2 * 0.92f;
            Gfx.Dial(g, dialW + (size.Width - dialW) / 2, top + areaH / 2, ar, altimeter.Spec, altimeter.Needle, Light);

            string[] titles = { "BARO", "ALTITUDE", "ZULU", "TEMP" };
            string[] values =
            {
                "2992",
                hasAltitude ? ((int)Math.Round(altitudeFeet)).ToString().PadLeft(5, '0') : "00000",
                Clock(),
                temp.Length > 0 ? temp : "--",
            };
            float w = (size.Width - 50) / 4f;
            for (int i = 0; i < 4; i++)
            {
                float x = 10 + i * (w + 10), y = size.Height - oledH + 24;
                Gfx.Caption(g, titles[i], x + w / 2, y - 10, 12, Color.FromArgb(190, 190, 190), true);
                string text = values[i].Length >= 8 ? values[i] : new string(' ', (8 - values[i].Length) / 2) + values[i];
                Gfx.Oled(g, new RectangleF(x, y, w, 62), new[] { text }, 8, OledsOn, oledTest);
            }
        }
    }
}
