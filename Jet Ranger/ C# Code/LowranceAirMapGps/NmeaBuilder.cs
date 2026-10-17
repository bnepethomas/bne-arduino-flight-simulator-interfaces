using System.Globalization;
using System.Text;

namespace LowranceAirMapGps
{
    /// <summary>Builds the NMEA 0183 sentences sent to the GPS.</summary>
    public static class NmeaBuilder
    {
        private const double FeetToMetres = 0.3048;
        private const double KnotsToKmh = 1.852;
        private static readonly CultureInfo Inv = CultureInfo.InvariantCulture;

        /// <summary>
        /// One update: GGA (position, altitude), RMC (position, speed, track, date), GLL, VTG (track, speed), GSA and HDT (heading).
        /// NMEA has no airspeed sentence a GPS accepts, so the airspeed is sent as the GPS speed in RMC and VTG.
        /// </summary>
        public static IReadOnlyList<string> Build(AircraftData d, DateTime utc)
        {
            string time = utc.ToString("HHmmss.00", Inv);
            string date = utc.ToString("ddMMyy", Inv);
            string lat = Latitude(d.LatitudeDeg, out string ns);
            string lon = Longitude(d.LongitudeDeg, out string ew);
            string altM = (d.AltitudeFt * FeetToMetres).ToString("0.0", Inv);
            string kt = d.AirspeedKt.ToString("0.0", Inv);
            string kmh = (d.AirspeedKt * KnotsToKmh).ToString("0.0", Inv);
            string hdg = d.HeadingDeg.ToString("0.0", Inv);

            return new[]
            {
                Sentence($"GPGGA,{time},{lat},{ns},{lon},{ew},1,08,1.0,{altM},M,0.0,M,,"),
                Sentence($"GPRMC,{time},A,{lat},{ns},{lon},{ew},{kt},{hdg},{date},,,A"),
                Sentence($"GPGLL,{lat},{ns},{lon},{ew},{time},A,A"),
                Sentence($"GPVTG,{hdg},T,,M,{kt},N,{kmh},K,A"),
                Sentence("GPGSA,A,3,01,02,03,04,05,06,07,08,,,,,1.5,1.0,1.2"),
                Sentence($"GPHDT,{hdg},T"),
            };
        }

        /// <summary>ddmm.mmmmm</summary>
        public static string Latitude(double deg, out string hemisphere)
        {
            hemisphere = deg >= 0 ? "N" : "S";
            return DegMin(Math.Abs(deg), 2);
        }

        /// <summary>dddmm.mmmmm</summary>
        public static string Longitude(double deg, out string hemisphere)
        {
            hemisphere = deg >= 0 ? "E" : "W";
            return DegMin(Math.Abs(deg), 3);
        }

        private static string DegMin(double absDeg, int degDigits)
        {
            int whole = (int)Math.Floor(absDeg);
            double min = (absDeg - whole) * 60.0;
            // Rounding to 5 decimals can produce 60.00000: carry into the degrees.
            if (Math.Round(min, 5) >= 60.0) { whole++; min = 0; }
            return whole.ToString("D" + degDigits, Inv) + min.ToString("00.00000", Inv);
        }

        /// <summary>Wraps a sentence body as $body*CS followed by CR LF.</summary>
        public static string Sentence(string body)
        {
            int cs = 0;
            foreach (char c in body) cs ^= c;
            return "$" + body + "*" + cs.ToString("X2", Inv) + "\r\n";
        }

        public static byte[] ToBytes(IEnumerable<string> sentences) => Encoding.ASCII.GetBytes(string.Concat(sentences));
    }
}
