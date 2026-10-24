using System.Globalization;
using System.Text;

namespace MSFSWinformsAutoCS
{
    // One sample of the simulator values the panel boards need, in real units. Filled from the
    // SimConnect data definition (Form1.cs) and turned into per-board UDP packets below.
    public sealed class SimSnapshot
    {
        public double AltitudeFeet;
        public double AirspeedKnots;
        public double VerticalSpeedFpm;
        public double RadarAltFeet;
        public double PitchDegrees;          // as the sim reports it (positive = nose down)
        public double BankDegrees;
        public double RotorRpmPct;
        public double EngineRpmPct;          // GENERAL ENG PCT MAX RPM (turbine speed)
        public double TorquePct;
        public double ElectricalLoadAmps;
        public double IttCelsius;
        public double OilTempCelsius;
        public double FuelGallons;
        public double N1Pct;
        public double OilPressurePsi;
        public double TransPressurePsi;
        public double TransTempCelsius;
        public double FuelPressurePsi;
        public double MainBusVolts;
        public double AvionicsBusVolts;
        public double AmbientCelsius;
        public double KohlsmanMillibars;
        public double ZuluSeconds;           // seconds since midnight UTC
        public bool AvionicsMaster;
        public bool FlyInside;               // the FlyInside B206 (aircraft-specific estimates in use)
        public bool CowaSim;                 // the CowaSim Jet Ranger (aircraft-specific estimates in use)
        public double ElectricalLoadFullScaleAmps = Payloads.DefaultElectricalLoadFullScaleAmps;   // amps that read 100 % on the LOAD gauge
        public double Com1Active, Com1Standby, Com2Active, Com2Standby;   // MHz
        public double Nav1Active, Nav1Standby, Nav2Active, Nav2Standby;   // MHz
        public double LatitudeDeg, LongitudeDeg;   // degrees, south and west negative
        public double HeadingTrueDeg;              // degrees true
        public int ZuluYear, ZuluMonth, ZuluDay;   // the simulator's UTC date (the time of day is ZuluSeconds)
        public PanelLedState Leds;                 // autopilot and audio panel LEDs (see PanelLedState)
        public int ApHeading, ApCourse;            // the autopilot heading bug and the NAV1 OBS, degrees (north is 360)
        public int ApAirspeed, ApVerticalSpeed, ApAltitude;   // the autopilot airspeed (knots), vertical speed (feet per minute) and altitude (feet)
    }

    // The 19 LEDs on the Auto Pilot and Audio panels, driven by the Radio controller's MAX7219.
    public struct PanelLedState
    {
        // Auto Pilot panel
        public bool Crs, Hdg, Ias, Alt, Appr, Fd, Bc, ApEngage;
        // Audio panel
        public bool Com1, Both, Nav2, Dme, Outer, Inner, Com2, Nav1, Mkr, Adf, Middle;
    }

    // Builds the "D,CODE:value,..." packets for each board. Same board split and real-unit
    // codes as FSUIPCWinformsAutoCS: each board converts the real units to steps / servo
    // positions with its own calibration tables.
    public static class Payloads
    {
        static readonly CultureInfo Inv = CultureInfo.InvariantCulture;

        // The main bus must be at least this high for the gauges / radar altimeter to be powered.
        public const double GaugePowerVolts = 20.0;
        // The radio bus (avionics bus, or main bus if the avionics bus reads 0) must be at least this
        // high for the COM/NAV displays to be on.
        public const double RadioPowerVolts = 24.0;
        // The main bus must be at least this high for the four Dual Stepper OLEDs to be on.
        public const double OledPowerVolts = 24.0;
        // The TRANSMISSION OIL PRESSURE lamp comes on below this pressure.
        public const double TransmissionPressureWarningPsi = 40.0;
        // The GEN FAIL lamp comes on when the engine speed (turbine, percent) is below this.
        public const double GeneratorFailEngineRpmPct = 10.0;

        // Total electrical load that reads 100 % on the LOAD gauge. The Dual Stepper's ELECTRICALLOAD
        // is a percentage; the sim gives amps. 300 A is a round number chosen so the FlyInside B206's
        // 143 A reads about 48 % (its cockpit LOAD gauge shows about 50 %); not checked at other loads.
        public const double DefaultElectricalLoadFullScaleAmps = 300.0;

        public static double ElectricalLoadPercent(SimSnapshot s)
            // The sign of ELECTRICAL TOTAL LOAD AMPS flips with the direction of the current (negative while the
            // battery, not the generator, supplies the bus, e.g. with the engine stopped), so the magnitude is used.
            => Math.Clamp(Math.Abs(s.ElectricalLoadAmps) / s.ElectricalLoadFullScaleAmps * 100.0, 0.0, 100.0);

        static string F0(double v) => Math.Round(v).ToString("F0", Inv);
        static string F1(double v) => v.ToString("F1", Inv);

        public static bool GaugesPowered(SimSnapshot s) => s.MainBusVolts >= GaugePowerVolts;
        // Bus voltage that decides whether the COM/NAV displays are on: the avionics bus, or the
        // main bus when the aircraft does not report the avionics bus (it reads exactly 0, as on
        // the FlyInside B206).
        public static double RadioBusVolts(SimSnapshot s) => s.AvionicsBusVolts > 0 ? s.AvionicsBusVolts : s.MainBusVolts;
        public static bool RadioPowered(SimSnapshot s) => RadioBusVolts(s) >= RadioPowerVolts;
        public static bool OledsPowered(SimSnapshot s) => s.MainBusVolts >= OledPowerVolts;

        // The 15 warning lamps on the Servo board (code, display name). Order is the order they
        // are sent in. Only RLOW, EOUT and LOWF can be derived from standard SimVars; the others
        // are aircraft specific and stay off (see PROGRAM_SUMMARY.md).
        public static readonly (string Code, string Name)[] Lamps =
        {
            ("EOUT", "ENGINE OUT"),
            ("TOPW", "TRANSMISSION OIL PRESSURE"),
            ("BHOT", "BATTERY HOT"),
            ("EC",   "ENGINE CHIP"),
            ("BD",   "BAGGAGE DOOR"),
            ("GENF", "GENERATOR FAIL"),
            ("TOTW", "TRANSMISSION OIL TEMPERATURE"),
            ("RLOW", "ROTOR LOW RPM"),
            ("FFLTR","AFT FUEL FILTER"),
            ("BTMP", "BATTERY TEMPERATURE"),
            ("TC",   "TRANSMISSION CHIP"),
            ("TRC",  "TAIL ROTOR CHIP"),
            ("FPMP", "FUEL PUMP"),
            ("LOWF", "LOW FUEL"),
            ("SCF",  "SC FAIL"),
        };

        public static bool LampState(string code, SimSnapshot s)
        {
            switch (code)
            {
                case "EOUT": return s.EngineRpmPct < 60;      // turbine below 60 percent
                case "RLOW": return s.RotorRpmPct <= 95;      // rotor at or below 95 percent
                case "LOWF": return s.FuelGallons <= 17;      // 17 US gallons or less
                case "GENF": return s.EngineRpmPct < GeneratorFailEngineRpmPct;   // engine speed below 10 percent
                // Transmission oil pressure below 40 psi. Only when a pressure is available: the FlyInside B206's
                // estimate (which falls to 0 with the rotor), or any aircraft whose SimVar reads above 0 (most
                // report 0 for it, which would keep the lamp on all the time).
                case "TOPW": return (s.FlyInside || s.CowaSim || s.TransPressurePsi > 0) && s.TransPressurePsi < TransmissionPressureWarningPsi;
                default: return false;
            }
        }

        // ",CODE:1" / ",CODE:0" for every lamp (all lamps off when the main bus is unpowered).
        public static string LampPayload(SimSnapshot s)
        {
            var sb = new StringBuilder();
            bool powered = GaugesPowered(s);
            foreach (var (code, _) in Lamps)
                sb.Append(',').Append(code).Append(':').Append(powered && LampState(code, s) ? '1' : '0');
            return sb.ToString();
        }

        // Servo board (.102): attitude pitch/bank in degrees to one decimal plus the lamp codes.
        // Pitch is negated: the sim reports nose-down positive, the servo table is nose-up positive.
        public static string Servo(SimSnapshot s)
            => "D,BPWR:" + (GaugesPowered(s) ? "1" : "0")      // backlight on only while the main bus is 20 V or more
             + ",PITCH:" + F1(-s.PitchDegrees) + ",BANK:" + F1(s.BankDegrees) + LampPayload(s);

        // The radar altimeter is powered from the main bus: 0 when unpowered.
        public static string RadarAltitude(SimSnapshot s)
            => GaugesPowered(s) ? F0(s.RadarAltFeet) : "0";

        // Single-board Stepper (.105): airspeed (one decimal), VSI, radar altitude, fuel, torque,
        // turbine outlet temperature and gas producer.
        public static string Stepper(SimSnapshot s)
            => "D,BPWR:" + (GaugesPowered(s) ? "1" : "0")      // backlight on only while the main bus is 20 V or more
             + ",IAS:" + F1(s.AirspeedKnots)
             + ",VSI:" + ((int)s.VerticalSpeedFpm).ToString(Inv)
             + ",AGL:" + RadarAltitude(s)
             + ",FUEL:" + F0(s.FuelGallons)
             + ",TQ:" + F0(s.TorquePct)
             + ",ITT:" + F0(s.IttCelsius)
             + ",N1:" + F0(s.N1Pct);

        // HHMM for the Clock OLED.
        public static int ZuluHHMM(SimSnapshot s)
        {
            int total = (int)s.ZuluSeconds;
            int hours = (total / 3600) % 24;
            int minutes = (total % 3600) / 60;
            return hours * 100 + minutes;
        }

        // Dual Stepper (.106). PWR leads the packet so the board applies it before ALT/ZULU/TEMP.
        public static string DualStepper(SimSnapshot s)
            => "D,PWR:" + (OledsPowered(s) ? "1" : "0")
             + ",BPWR:" + (GaugesPowered(s) ? "1" : "0")        // backlight on only while the main bus is 20 V or more
             + ",ALT:" + F0(s.AltitudeFeet)
             + ",RPMR:" + s.RotorRpmPct.ToString("F1", Inv)
             + ",RPME:" + s.EngineRpmPct.ToString("F1", Inv)
             + ",ZULU:" + ZuluHHMM(s).ToString(Inv)
             + ",OILT:" + F0(s.OilTempCelsius)
             + ",OILP:" + F0(s.OilPressurePsi)
             + ",XMSNT:" + F0(s.TransTempCelsius)
             + ",XMSNP:" + F0(s.TransPressurePsi)
             + ",FUELLOAD:" + F0(s.FuelPressurePsi)
             + ",ELECTRICALLOAD:" + F0(ElectricalLoadPercent(s))
             + ",TEMP:" + F0(s.AmbientCelsius) + "C";

        // Kollsman setting as inHg x 100 (2992 = 29.92 inHg) from millibars.
        public static int BaroInHgx100(SimSnapshot s) => (int)Math.Round(s.KohlsmanMillibars * 2.953);

        // Lowrance AirMap GPS interface (LowranceAirMapGps, UDP 27002): latitude, longitude, altitude (feet),
        // airspeed (knots) and true heading (degrees).
        public static string Gps(SimSnapshot s)
            => "D,LAT:" + s.LatitudeDeg.ToString("F6", Inv) + ",LON:" + s.LongitudeDeg.ToString("F6", Inv)
             + ",ALT:" + F0(s.AltitudeFeet) + ",IAS:" + F1(s.AirspeedKnots) + ",HDG:" + F1(s.HeadingTrueDeg)
             + SimUtc(s);

        // ",UTC:yyyyMMddHHmmss.fff" - the simulator's UTC date and time - or nothing when the date is not valid (the
        // GPS interface then uses the PC clock).
        public static string SimUtc(SimSnapshot s)
        {
            if (s.ZuluYear < 1 || s.ZuluYear > 9999 || s.ZuluMonth < 1 || s.ZuluMonth > 12 || s.ZuluDay < 1 || s.ZuluDay > DateTime.DaysInMonth(s.ZuluYear, s.ZuluMonth))
                return "";
            DateTime utc = new DateTime(s.ZuluYear, s.ZuluMonth, s.ZuluDay, 0, 0, 0, DateTimeKind.Utc).AddSeconds(Math.Clamp(s.ZuluSeconds, 0, 86399.999));
            return ",UTC:" + utc.ToString("yyyyMMddHHmmss.fff", Inv);
        }

        // Frequencies as the radio board expects them: MHz with three decimals.
        public static string Freq(double mhz) => mhz.ToString("F3", Inv);

        // The panel LEDs as ",LED_CRS:1,LED_HDG:0,...". All 0 while the radio bus is below 24 V, like the radio displays.
        public static string LedTokens(PanelLedState l, bool powered)
        {
            string B(bool v) => (powered && v) ? "1" : "0";
            return ",LED_CRS:" + B(l.Crs)
                 + ",LED_HDG:" + B(l.Hdg)
                 + ",LED_IAS:" + B(l.Ias)
                 + ",LED_ALT:" + B(l.Alt)
                 + ",LED_APPR:" + B(l.Appr)
                 + ",LED_FD:" + B(l.Fd)
                 + ",LED_BC:" + B(l.Bc)
                 + ",LED_APENG:" + B(l.ApEngage)
                 + ",LED_COM1:" + B(l.Com1)
                 + ",LED_BOTH:" + B(l.Both)
                 + ",LED_NAV2:" + B(l.Nav2)
                 + ",LED_DME:" + B(l.Dme)
                 + ",LED_OUTER:" + B(l.Outer)
                 + ",LED_INNER:" + B(l.Inner)
                 + ",LED_COM2:" + B(l.Com2)
                 + ",LED_NAV1:" + B(l.Nav1)
                 + ",LED_MKR:" + B(l.Mkr)
                 + ",LED_ADF:" + B(l.Adf)
                 + ",LED_MID:" + B(l.Middle);
        }

        // A bearing as a whole number of degrees from 1 to 360 (north is 360, as on an autopilot display).
        public static int Bearing(double degrees)
        {
            int d = (int)Math.Round(degrees) % 360;
            if (d < 0) d += 360;
            return d == 0 ? 360 : d;
        }

        // The five autopilot values for the Auto Pilot display: ",APHDG:090,APCRS:110,APIAS:80,APVS:0,APALT:3200".
        public static string ApValueTokens(SimSnapshot s)
            => ",APHDG:" + s.ApHeading.ToString("000", Inv)
             + ",APCRS:" + s.ApCourse.ToString("000", Inv)
             + ",APIAS:" + s.ApAirspeed.ToString(Inv)
             + ",APVS:" + s.ApVerticalSpeed.ToString(Inv)
             + ",APALT:" + s.ApAltitude.ToString(Inv);

        // Radio board (.101).
        public static string Radio(SimSnapshot s)
            => "D,C1A:" + Freq(s.Com1Active)
             + ",C1S:" + Freq(s.Com1Standby)
             + ",C2A:" + Freq(s.Com2Active)
             + ",C2S:" + Freq(s.Com2Standby)
             + ",MAINBUS:" + F1(s.MainBusVolts)
             + ",NAVCOM1:" + (RadioPowered(s) ? "1" : "0")
             + ",BPWR:" + (GaugesPowered(s) ? "1" : "0")        // backlight on only while the main bus is 20 V or more
             + LedTokens(s.Leds, RadioPowered(s))
             + ApValueTokens(s);
    }
}
