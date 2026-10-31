namespace JetRangerPanelEmulator
{
    // The faces of the panel's gauges and the legends of its lamps: the same as the instrument map (Appendix G of the master document).
    public static class Specs
    {
        static Func<double, string> Every(double step, double div = 1)
            => v => Math.Abs(v / step - Math.Round(v / step)) < 1e-6 ? (v / div).ToString("0.##", System.Globalization.CultureInfo.InvariantCulture) : "";

        static DialSpec D(string token, string l1, string l2, double min, double max, double major, int minor, Func<double, string> label, float a0 = -135, float a1 = 135, double? rest = null)
            => new() { Token = token, Legend = new[] { l1, l2 }, Min = min, Max = max, Major = major, Minor = minor, Label = label, A0 = a0, A1 = a1, Rest = rest ?? min };

        public static List<DialSpec> StepperGauges() => new()
        {
            D("IAS", "AIRSPEED", "KNOTS", 0, 140, 20, 2, Every(20)),
            D("VSI", "VERT SPEED", "x100 FT/MIN", -1750, 1750, 250, 1,
              v => v switch { 0 => "0", 500 or -500 => "5", 1000 or -1000 => "10", 1500 or -1500 => "15", _ => "" }, -150, 150, 0),
            D("AGL", "RADAR ALT", "x100 FT", 0, 2500, 500, 5, Every(500, 100)),
            D("TQ", "TORQUE", "PERCENT", 0, 120, 20, 2, Every(20)),
            D("N1", "GAS PROD", "N1 %", 0, 110, 10, 2, Every(20)),
            D("ITT", "TURBINE OUT", "TEMP x100 C", 100, 900, 100, 2, Every(200, 100)),
            D("FUEL", "FUEL", "US GAL", 0, 75, 25, 5, Every(25)),
        };

        public static List<DialSpec> DualGauges() => new()
        {
            D("OILP", "ENG OIL", "PSI", 0, 150, 50, 5, Every(50)),
            D("OILT", "ENG OIL", "TEMP C", 0, 150, 50, 5, Every(50)),
            D("XMSNP", "XMSN OIL", "PSI", 0, 150, 50, 5, Every(50)),
            D("XMSNT", "XMSN OIL", "TEMP C", 0, 150, 50, 5, Every(50)),
            D("RPMR", "ROTOR", "RPM %", 0, 110, 10, 2, Every(50)),
            D("RPME", "ENGINE", "N2 %", 0, 110, 10, 2, Every(50)),
            D("ELECTRICALLOAD", "ELEC LOAD", "PERCENT", 0, 100, 25, 5, Every(25)),
            D("FUELLOAD", "FUEL", "PRESS PSI", 0, 30, 10, 2, Every(10)),
        };

        public static DialSpec Altimeter() => new()
        {
            Token = "ALT", Legend = new[] { "ALT" }, LegendTop = -0.28f, Min = 0, Max = 10, Major = 1, Minor = 5, A0 = 0, A1 = 360, Rest = 0,
            Label = v => v >= 10 ? "" : ((int)v).ToString(),
        };

        // Servo lamps in the order of the panel: token, legend, colour.
        public static readonly (string Token, string L1, string L2, Color Colour)[] ServoLamps =
        {
            ("EOUT", "ENGINE", "OUT", Gfx.Red), ("RLOW", "ROTOR", "LOW", Gfx.Red), ("TOPW", "XMSN OIL", "PRESS", Gfx.Amber),
            ("TOTW", "XMSN OIL", "TEMP", Gfx.Amber), ("BHOT", "BATTERY", "HOT", Gfx.Amber), ("BTMP", "BATTERY", "TEMP", Gfx.Amber),
            ("EC", "ENGINE", "CHIP", Gfx.Amber), ("TRC", "TAIL ROTOR", "CHIP", Gfx.Amber), ("TC", "XMSN", "CHIP", Gfx.Amber),
            ("BD", "BAGGAGE", "DOOR", Gfx.Amber), ("FPMP", "FUEL", "PUMP", Gfx.Amber), ("FFLTR", "AFT FUEL", "FILTER", Gfx.Amber),
            ("GENF", "GEN", "FAIL", Gfx.Amber), ("LOWF", "LOW", "FUEL", Gfx.Amber), ("SCF", "SC", "FAIL", Gfx.Amber),
        };

        // The Radio's panel LEDs: token, label, colour.
        public static readonly (string Token, string Label, Color Colour)[] AutoPilotLeds =
        {
            ("LED_CRS", "CRS", Gfx.Green), ("LED_HDG", "HDG", Gfx.Green), ("LED_IAS", "IAS", Gfx.Green), ("LED_ALT", "ALT", Gfx.Green),
            ("LED_APPR", "APPR", Gfx.Green), ("LED_FD", "FD", Gfx.Green), ("LED_BC", "BC", Gfx.Green), ("LED_APENG", "AP", Gfx.Green),
        };

        public static readonly (string Token, string Label, Color Colour)[] AudioLeds =
        {
            ("LED_COM1", "COM1", Gfx.Green), ("LED_BOTH", "BOTH", Gfx.Green), ("LED_NAV2", "NAV2", Gfx.Green), ("LED_DME", "DME", Gfx.Green),
            ("LED_OUTER", "OUTER", Gfx.Blue), ("LED_INNER", "INNER", Gfx.White), ("LED_COM2", "COM2", Gfx.Green), ("LED_NAV1", "NAV1", Gfx.Green),
            ("LED_MKR", "MKR", Gfx.Green), ("LED_ADF", "ADF", Gfx.Green), ("LED_MID", "MID", Gfx.Amber),
        };

        public static readonly string[] HueyLeds =
        {
            "LED_H_ENG_OIL", "LED_H_ENG_ICE", "LED_H_ENG_ICE_DETECT", "LED_H_ENG_CHIP", "LED_H_LEFT_FUEL_BOOST", "LED_H_ENG_FUEL_PUMP", "LED_H_RIGHT_FUEL_BOOST",
            "LED_H_20_MINUTE_FUEL", "LED_H_AUX_FUEL_LOW", "LED_H_XMSN_OIL_PRESS", "LED_H_XMSN_OIL_HOT", "LED_H_HYD_PRESS", "LED_H_ENG_INLET_AIR", "LED_H_INST_INVERTER",
            "LED_H_DC_GENERATOR", "LED_H_EXT_POWER", "LED_H_FUEL_FILTER", "LED_H_GOV_EMER", "LED_H_CHIP_DETECTOR", "LED_H_IFF",
        };
    }
}
