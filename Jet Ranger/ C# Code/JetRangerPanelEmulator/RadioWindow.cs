namespace JetRangerPanelEmulator
{
    // The Radio controller (172.16.1.101): the COM and NAV frequency displays, the Auto Pilot and DME displays, the 19 Auto Pilot and Audio panel LEDs and the 20 Huey
    // Warning Panel LEDs.
    public sealed class RadioWindow : BoardWindow
    {
        public override string BoardName => "radio";
        public override int BoardNumber => 101;
        protected override string Heading => "Radio controller";

        // The frequencies as the displays show them (xxx.xxx), starting with the sketch's own start-up values. The PC sends only COM1 and COM2; the NAV radios keep their start-up values.
        readonly Dictionary<string, string> freq = new()
        {
            ["C1A"] = "119.000", ["C1S"] = "121.000", ["C2A"] = "120.000", ["C2S"] = "122.000",
            ["N1A"] = "109.000", ["N1S"] = "111.000", ["N2A"] = "110.000", ["N2S"] = "112.000",
        };
        readonly Dictionary<string, string> ap = new() { ["APHDG"] = "090", ["APCRS"] = "110", ["APIAS"] = "80", ["APVS"] = "0", ["APALT"] = "3200" };
        readonly Dictionary<string, bool> leds = new();
        bool? radioPower;                       // NAVCOM1: null until the first one arrives
        bool ledTest;

        public RadioWindow(string net) : base(net)
        {
            foreach (var l in Specs.AutoPilotLeds) leds[l.Token] = false;
            foreach (var l in Specs.AudioLeds) leds[l.Token] = false;
            foreach (var t in Specs.HueyLeds) leds[t] = false;
            ClientSize = new Size(940, 600);
        }

        bool DisplaysOn => !NoData && radioPower != false;
        bool LedsOn(string token) => ledTest || (radioPower != false && leds[token]);

        static string Pad(string s)
            => double.TryParse(s, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out double v) ? v.ToString("000.000", System.Globalization.CultureInfo.InvariantCulture) : s;

        protected override void HandleData(string token, string value)
        {
            switch (token)
            {
                case "NAVCOM1": radioPower = value != "0"; break;
                case "C1A": case "C1S": case "C2A": case "C2S": freq[token] = Pad(value); break;
                case "APHDG": case "APCRS": case "APIAS": case "APVS": case "APALT": ap[token] = value; break;
                default:
                    if (leds.ContainsKey(token)) leds[token] = value != "0";
                    break;
            }
        }

        protected override void HandleControl(string name, string value)
        {
            if (name == "LEDTEST") ledTest = value != "0";
        }

        protected override void ResetState()
        {
            radioPower = null; ledTest = false;
            foreach (var k in leds.Keys.ToList()) leds[k] = false;
        }

        // The Auto Pilot display: HDG and VS on the top row, CRS, IAS and ALT on the bottom, in the windows of the faceplate mask.
        string[] AutoPilotLines()
        {
            char[] top = new string(' ', 16).ToCharArray(), bottom = new string(' ', 16).ToCharArray();
            void Left(char[] row, string s) { for (int i = 0; i < s.Length && i < 16; i++) row[i] = s[i]; }
            void Right(char[] row, int end, string s) { for (int i = 0; i < s.Length; i++) { int c = end - s.Length + 1 + i; if (c >= 0 && c < 16) row[c] = s[i]; } }
            Left(top, ap["APHDG"]); Right(top, 15, ap["APVS"]);
            Left(bottom, ap["APCRS"]); Right(bottom, 8, ap["APIAS"]); Right(bottom, 15, ap["APALT"]);
            return new[] { new string(top), new string(bottom) };
        }

        public override Dictionary<string, string> State()
        {
            var d = new Dictionary<string, string>();
            foreach (var kv in freq) d[kv.Key] = DisplaysOn ? kv.Value : "";
            foreach (var kv in ap) d[kv.Key] = DisplaysOn ? kv.Value : "";
            foreach (var kv in leds) d[kv.Key] = LedsOn(kv.Key) ? "1" : "0";
            d["NAVCOM1"] = radioPower == null ? "" : (radioPower == true ? "1" : "0");
            d["displays"] = DisplaysOn ? "on" : "blank";
            return d;
        }

        protected override void Draw(Graphics g, Size size)
        {
            Gfx.Caption(g, "Radio controller .101  (Radio Auto Pilot PCB)", size.Width / 2f, 16, 15, Color.FromArgb(230, 230, 230), true);
            bool on = DisplaysOn;
            float w = (size.Width - 50) / 4f, y = 62, h = 70;
            string[] caps = { "COM", "NAV", "AUTO PILOT", "DME" };
            var lines = new[]
            {
                new[] { freq["C1A"] + " " + freq["C2A"], freq["C1S"] + " " + freq["C2S"] },
                new[] { freq["N1A"] + " " + freq["N2A"], freq["N1S"] + " " + freq["N2S"] },
                AutoPilotLines(),
                new[] { "10M", "" },
            };
            for (int i = 0; i < 4; i++)
            {
                float x = 10 + i * (w + 10);
                Gfx.Caption(g, caps[i], x + w / 2, y - 14, 12, Color.FromArgb(190, 190, 190), true);
                Gfx.Oled(g, new RectangleF(x, y, w, h), lines[i], 16, on);
            }

            float ly = y + h + 38;
            Gfx.Caption(g, "AUTO PILOT", 10 + 4 * 52, ly - 18, 12, Color.FromArgb(190, 190, 190), true);
            for (int i = 0; i < Specs.AutoPilotLeds.Length; i++)
            {
                var (token, label, colour) = Specs.AutoPilotLeds[i];
                Gfx.Led(g, 36 + i * 52, ly + 6, 9, colour, LedsOn(token), label, Light);
            }
            Gfx.Caption(g, "AUDIO", 560, ly - 18, 12, Color.FromArgb(190, 190, 190), true);
            for (int i = 0; i < Specs.AudioLeds.Length; i++)
            {
                var (token, label, colour) = Specs.AudioLeds[i];
                Gfx.Led(g, 446 + i * 44, ly + 6, 9, colour, LedsOn(token), label, Light);
            }

            float hy = ly + 70;
            Gfx.Caption(g, "HUEY WARNING PANEL", size.Width / 2f, hy - 14, 12, Color.FromArgb(190, 190, 190), true);
            float cw = (size.Width - 20) / 5f, chh = (size.Height - hy - 14) / 4f;
            for (int i = 0; i < Specs.HueyLeds.Length; i++)
            {
                string token = Specs.HueyLeds[i];
                string label = token.Substring(6).Replace('_', ' ');
                var rect = new RectangleF(10 + (i % 5) * cw + 3, hy + (i / 5) * chh + 3, cw - 6, chh - 6);
                Gfx.Lamp(g, rect, label, "", Gfx.Amber, LedsOn(token), Light, Math.Min(15f, chh * 0.34f));
            }
        }
    }
}
