namespace JetRangerPanelEmulator
{
    // The ADF and Upper controller (172.16.1.107): the three landing gear lamps, the two-line character OLED (ADF frequency and radar follow ID) and the backlight.
    // The Upper switch matrix and the keypads are inputs, so there is nothing of them to show here.
    public sealed class AdfWindow : BoardWindow
    {
        public override string BoardName => "adf";
        public override int BoardNumber => 107;
        protected override string Heading => "ADF and Upper controller";

        readonly Dictionary<string, bool> gear = new() { ["LDGN"] = false, ["LDGL"] = false, ["LDGR"] = false };
        bool displayOn = true;                  // C,DISPLAY from the Radio: starts on, so a Radio that is off or missing never blanks it
        bool oledTest;

        public AdfWindow(string net) : base(net) { ClientSize = new Size(640, 360); }

        bool Lit => !NoData && displayOn;

        protected override void HandleData(string token, string value)
        {
            if (gear.ContainsKey(token)) gear[token] = value != "0";
        }

        protected override void HandleControl(string name, string value)
        {
            if (name == "DISPLAY") displayOn = value != "0";
            else if (name == "OLEDTEST") oledTest = value != "0";
        }

        protected override void ResetState()
        {
            displayOn = true; oledTest = false;
            foreach (var k in gear.Keys.ToList()) gear[k] = false;
        }

        public override Dictionary<string, string> State()
        {
            var d = new Dictionary<string, string>
            {
                ["display"] = Lit ? "on" : "blank",
                ["oled.line1"] = Lit ? "ADF  340 kHz" : "",
                ["oled.line2"] = Lit ? "FOLLOW OFF" : "",
            };
            foreach (var kv in gear) d[kv.Key] = Lit && kv.Value ? "1" : "0";
            return d;
        }

        protected override void Draw(Graphics g, Size size)
        {
            Gfx.Caption(g, "ADF and Upper controller .107  (Upper ADF Controller PCB)", size.Width / 2f, 16, 15, Color.FromArgb(230, 230, 230), true);
            Gfx.Caption(g, "ADF", size.Width / 2f, 50, 12, Color.FromArgb(190, 190, 190), true);
            Gfx.Oled(g, new RectangleF(60, 64, size.Width - 120, 90), new[] { "ADF  340 kHz".PadRight(16), "FOLLOW OFF" }, 16, Lit, oledTest);

            Gfx.Caption(g, "LANDING GEAR", size.Width / 2f, 196, 12, Color.FromArgb(190, 190, 190), true);
            string[] names = { "LDGL", "LDGN", "LDGR" };
            string[] labels = { "LEFT", "NOSE", "RIGHT" };
            for (int i = 0; i < 3; i++)
                Gfx.Led(g, size.Width / 2f + (i - 1) * 120, 250, 22, Gfx.Green, Lit && gear[names[i]], labels[i], Light);
            Gfx.Caption(g, "The Upper switch matrix and keypads are inputs and are not shown.", size.Width / 2f, size.Height - 22, 11, Color.FromArgb(150, 150, 150));
        }
    }
}
