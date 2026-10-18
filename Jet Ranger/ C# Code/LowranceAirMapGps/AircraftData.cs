using System.Globalization;

namespace LowranceAirMapGps
{
    /// <summary>One sample of aircraft information. Altitude in feet, airspeed in knots, heading in degrees true.</summary>
    /// <summary>SimUtc is the simulator's UTC date and time when the packet carried one (otherwise null).</summary>
    public readonly record struct AircraftData(double LatitudeDeg, double LongitudeDeg, double AltitudeFt, double AirspeedKt, double HeadingDeg, DateTime? SimUtc = null);

    /// <summary>Parses the UDP packets that carry aircraft information.</summary>
    /// <remarks>
    /// Two layouts are accepted (case-insensitive, optional leading "D,"):
    ///   key/value : D,LAT:-27.383,LON:153.107,ALT:13,IAS:0,HDG:27,UTC:20261018031205.250   (any subset; missing keys keep the previous value)
    ///   csv       : lat,lon,altitude_ft,airspeed_kt,heading_deg[,utc]
    /// UTC is the simulator's UTC date and time as yyyyMMddHHmmss with optional .fff fraction of a second.
    /// Key aliases: LAT; LON, LONG, LNG; ALT, ALTITUDE; IAS, AIRSPEED, SPD, SPEED, GS; HDG, HEADING, TRK, COURSE.
    /// </remarks>
    public static class AircraftParser
    {
        private static readonly string[] LatKeys = { "LAT", "LATITUDE" };
        private static readonly string[] LonKeys = { "LON", "LONG", "LNG", "LONGITUDE" };
        private static readonly string[] AltKeys = { "ALT", "ALTITUDE" };
        private static readonly string[] SpeedKeys = { "IAS", "AIRSPEED", "SPD", "SPEED", "GS" };
        private static readonly string[] HeadingKeys = { "HDG", "HEADING", "TRK", "COURSE" };
        private static readonly string[] UtcKeys = { "UTC", "TIME", "SIMTIME" };
        private static readonly string[] UtcFormats = { "yyyyMMddHHmmss", "yyyyMMddHHmmss.f", "yyyyMMddHHmmss.ff", "yyyyMMddHHmmss.fff" };

        /// <summary>Returns the new sample, or null when the text is not aircraft data. <paramref name="previous"/> supplies values missing from a key/value packet.</summary>
        public static AircraftData? Parse(string text, AircraftData previous)
        {
            if (string.IsNullOrWhiteSpace(text)) return null;
            string s = text.Trim();
            if (s.StartsWith("D,", StringComparison.OrdinalIgnoreCase)) s = s.Substring(2);
            string[] parts = s.Split(new[] { ',', ';' }, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
            if (parts.Length == 0) return null;

            double lat = previous.LatitudeDeg, lon = previous.LongitudeDeg, alt = previous.AltitudeFt,
                   spd = previous.AirspeedKt, hdg = previous.HeadingDeg;
            bool gotLat = false, gotLon = false;
            DateTime? utc = null;

            if (parts.All(p => p.Contains(':')))
            {
                foreach (string p in parts)
                {
                    int i = p.IndexOf(':');
                    string key = p.Substring(0, i).Trim().ToUpperInvariant();
                    if (UtcKeys.Contains(key)) { utc = ParseUtc(p.Substring(i + 1)); continue; }
                    if (!TryNum(p.Substring(i + 1), out double v)) continue;
                    if (LatKeys.Contains(key)) { lat = v; gotLat = true; }
                    else if (LonKeys.Contains(key)) { lon = v; gotLon = true; }
                    else if (AltKeys.Contains(key)) alt = v;
                    else if (SpeedKeys.Contains(key)) spd = v;
                    else if (HeadingKeys.Contains(key)) hdg = v;
                }
                // A packet with neither coordinate is not position data (could be another D, packet on the network).
                if (!gotLat && !gotLon) return null;
            }
            else
            {
                if (parts.Length < 5) return null;
                if (!TryNum(parts[0], out lat) || !TryNum(parts[1], out lon) || !TryNum(parts[2], out alt) ||
                    !TryNum(parts[3], out spd) || !TryNum(parts[4], out hdg)) return null;
                if (parts.Length > 5) utc = ParseUtc(parts[5]);
            }

            if (lat < -90 || lat > 90 || lon < -180 || lon > 180) return null;
            hdg %= 360.0; if (hdg < 0) hdg += 360.0;
            if (spd < 0) spd = 0;
            return new AircraftData(lat, lon, alt, spd, hdg, utc);
        }

        private static DateTime? ParseUtc(string t) =>
            DateTime.TryParseExact(t.Trim(), UtcFormats, CultureInfo.InvariantCulture,
                DateTimeStyles.AssumeUniversal | DateTimeStyles.AdjustToUniversal, out DateTime d) ? d : null;

        private static bool TryNum(string t, out double v) =>
            double.TryParse(t.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out v) && double.IsFinite(v);
    }
}
