using System.Text;

namespace XPlaneBridge
{
    // The panel boards, their addresses and which generic tokens each one consumes. The tokens are the same ones the FSUIPC and MSFS bridges send, so the
    // board firmware does not change for DCS: a profile only has to name a DCS-BIOS control for each token it can supply.
    public static class Boards
    {
        public const int ControlPort = 13136;

        public static readonly (string Name, string Host)[] All =
        {
            ("Radio", "172.16.1.101"),
            ("Servo", "172.16.1.102"),
            ("Stepper", "172.16.1.105"),
            ("DualStepper", "172.16.1.106"),
            ("AdfUpper", "172.16.1.107"),
        };

        public static readonly string[] ServoLamps = { "EOUT", "TOPW", "BHOT", "EC", "BD", "GENF", "TOTW", "RLOW", "FFLTR", "BTMP", "TC", "TRC", "FPMP", "LOWF", "SCF" };
        static readonly string[] ServoTokens = { "PITCH", "BANK" };
        static readonly string[] StepperTokens = { "IAS", "VSI", "AGL", "FUEL", "TQ", "ITT", "N1" };
        static readonly string[] DualTokens = { "ALT", "RPMR", "RPME", "OILT", "OILP", "XMSNT", "XMSNP", "FUELLOAD", "ELECTRICALLOAD", "TEMP", "ZULU" };
        static readonly string[] RadioTokens = { "C1A", "C1S", "C2A", "C2S", "APHDG", "APCRS", "APIAS", "APVS", "APALT" };

        public static bool IsRadioToken(string t) => t.StartsWith("LED_", StringComparison.Ordinal) || RadioTokens.Contains(t);

        // The board that takes a token ("" for the power inputs MAINBUS and AVIONICSBUS, which the bridge turns into BPWR, PWR and NAVCOM1 itself).
        public static string BoardFor(string token)
        {
            if (IsRadioToken(token)) return "Radio";
            if (ServoTokens.Contains(token) || ServoLamps.Contains(token)) return "Servo";
            if (StepperTokens.Contains(token)) return "Stepper";
            if (DualTokens.Contains(token)) return "DualStepper";
            if (token == "LAT" || token == "LON" || token == "HDG" || token == "UTC") return "Gps";
            return "";
        }
    }

    public static class Payloads
    {
        // Same thresholds as the other bridges: the main bus must be 20 V for the gauges and the backlights, 24 V for the radio and the Dual Stepper OLEDs.
        public const double GaugePowerVolts = 20.0;
        public const double RadioPowerVolts = 24.0;

        static bool TryVolts(Dictionary<string, string> t, string token, out double volts)
            => double.TryParse(t.GetValueOrDefault(token), System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out volts);

        // No bus voltage mapped means the aircraft is treated as powered while data is flowing.
        public static bool GaugesPowered(Dictionary<string, string> t) => !TryVolts(t, "MAINBUS", out double v) || v >= GaugePowerVolts;

        public static bool RadioPowered(Dictionary<string, string> t)
        {
            if (TryVolts(t, "AVIONICSBUS", out double a) && a > 0) return a >= RadioPowerVolts;
            if (TryVolts(t, "MAINBUS", out double m)) return m >= RadioPowerVolts;
            return true;
        }

        // One "D,TOKEN:value,..." packet per board that has something to say; the backlight packet to the ADF and Upper board goes out always.
        public static Dictionary<string, string> Build(Dictionary<string, string> tokens)
        {
            bool gauges = GaugesPowered(tokens);
            bool radio = RadioPowered(tokens);
            string bpwr = "BPWR:" + (gauges ? "1" : "0");
            var packets = new Dictionary<string, string>();
            var byBoard = new Dictionary<string, List<KeyValuePair<string, string>>>();
            foreach (var kv in tokens)
            {
                string board = Boards.BoardFor(kv.Key);
                if (board.Length == 0) continue;
                if (!byBoard.TryGetValue(board, out var list)) byBoard[board] = list = new();
                list.Add(kv);
            }

            if (byBoard.TryGetValue("Radio", out var rl))
            {
                var sb = new StringBuilder("D,NAVCOM1:" + (radio ? "1" : "0") + "," + bpwr);
                foreach (var kv in rl)
                    sb.Append(',').Append(kv.Key).Append(':').Append(kv.Key.StartsWith("LED_", StringComparison.Ordinal) && !radio ? "0" : kv.Value);
                packets["Radio"] = sb.ToString();
            }
            if (byBoard.TryGetValue("Servo", out var sl))
            {
                var sb = new StringBuilder("D," + bpwr);
                foreach (var kv in sl)
                    sb.Append(',').Append(kv.Key).Append(':').Append(Boards.ServoLamps.Contains(kv.Key) && !gauges ? "0" : kv.Value);
                packets["Servo"] = sb.ToString();
            }
            if (byBoard.TryGetValue("Stepper", out var stl))
                // the radar altimeter is powered from the main bus: 0 when unpowered, as in the other bridges
                packets["Stepper"] = "D," + bpwr + string.Concat(stl.Select(kv => "," + kv.Key + ":" + (kv.Key == "AGL" && !gauges ? "0" : kv.Value)));
            if (byBoard.TryGetValue("DualStepper", out var dl))
                packets["DualStepper"] = "D,PWR:" + (gauges ? "1" : "0") + "," + bpwr + string.Concat(dl.Select(kv => "," + kv.Key + ":" + kv.Value));
            packets["AdfUpper"] = "D," + bpwr;

            if (tokens.TryGetValue("LAT", out var lat) && tokens.TryGetValue("LON", out var lon))
            {
                var sb = new StringBuilder("D,LAT:" + lat + ",LON:" + lon);
                foreach (var k in new[] { "ALT", "IAS", "HDG", "UTC" })
                    if (tokens.TryGetValue(k, out var v)) sb.Append(',').Append(k).Append(':').Append(v);
                packets["Gps"] = sb.ToString();
            }
            return packets;
        }
    }
}
