namespace JetRangerPanelEmulator
{
    // The Servo controller (172.16.1.102): the attitude indicator's pitch and bank servos and the 15 caution and warning lamps.
    public sealed class ServoWindow : BoardWindow
    {
        public override string BoardName => "servo";
        public override int BoardNumber => 102;
        protected override string Heading => "Servo controller";

        readonly Gauge pitch = new(new DialSpec { Token = "PITCH", Min = -30, Max = 30, Rest = 0 }) { FractionPerSecond = 1.5 };
        readonly Gauge bank = new(new DialSpec { Token = "BANK", Min = -90, Max = 90, Rest = 0 }) { FractionPerSecond = 1.5 };
        readonly Dictionary<string, bool> lamps = Specs.ServoLamps.ToDictionary(l => l.Token, _ => false);

        public ServoWindow(string net) : base(net) { ClientSize = new Size(820, 470); }

        protected override void HandleData(string token, string value)
        {
            if (token == "PITCH" && Packet.TryNumber(value, out double p)) { pitch.Target = p; pitch.HasValue = true; }
            else if (token == "BANK" && Packet.TryNumber(value, out double b)) { bank.Target = b; bank.HasValue = true; }
            else if (lamps.ContainsKey(token)) lamps[token] = value != "0";
        }

        protected override void HandleControl(string name, string value)
        {
            if (name == "SWINGPITCH") pitch.Swing();
            else if (name == "SWINGBANK") bank.Swing();
        }

        protected override void ResetState()
        {
            pitch.HasValue = bank.HasValue = false;
            foreach (var k in lamps.Keys.ToList()) lamps[k] = false;
        }

        protected override void Animate(double dt)
        {
            pitch.Animate(dt, NoData);
            bank.Animate(dt, NoData);
        }

        public override Dictionary<string, string> State()
        {
            var d = new Dictionary<string, string>
            {
                ["PITCH"] = pitch.HasValue ? Num(pitch.Target) : "", ["PITCH.needle"] = Num(pitch.Needle),
                ["BANK"] = bank.HasValue ? Num(bank.Target) : "", ["BANK.needle"] = Num(bank.Needle),
            };
            foreach (var l in Specs.ServoLamps) d[l.Token] = lamps[l.Token] && !NoData ? "1" : "0";
            return d;
        }

        protected override void Draw(Graphics g, Size size)
        {
            Gfx.Caption(g, "Servo controller .102  (Front Panel Servo PCB)", size.Width / 2f, 16, 15, Color.FromArgb(230, 230, 230), true);
            float r = Math.Min(size.Width * 0.36f, size.Height - 60f) / 2 * 0.95f;
            Gfx.Attitude(g, 20 + r * 1.05f, 40 + (size.Height - 40) / 2f, r, pitch.Needle, bank.Needle, Light);
            Gfx.Caption(g, "PITCH " + pitch.Needle.ToString("0.0") + "   BANK " + bank.Needle.ToString("0.0"), 20 + r * 1.05f, size.Height - 14, 12, Color.FromArgb(190, 190, 190));

            float x0 = size.Width * 0.45f, w = (size.Width - x0 - 20) / 3f, h = (size.Height - 60) / 5f;
            for (int i = 0; i < Specs.ServoLamps.Length; i++)
            {
                var (token, l1, l2, colour) = Specs.ServoLamps[i];
                int col = i / 5, row = i % 5;
                Gfx.Lamp(g, new RectangleF(x0 + col * w + 4, 44 + row * h + 4, w - 8, h - 8), l1, l2, colour, lamps[token] && !NoData, Light);
            }
        }
    }
}
