namespace LowranceAirMapGpsBenchTester
{
    /// <summary>The eight compass directions of the cursor pad. Bearing is degrees clockwise from north.</summary>
    public enum Compass { N, NE, E, SE, S, SW, W, NW }

    public static class GeoMove
    {
        private const double MetresPerDegreeLat = 111_320.0;

        public static double Bearing(Compass c) => (int)c * 45.0;

        private const double EarthRadiusM = 6_371_008.8;

        /// <summary>Moves a position by <paramref name="metres"/> along a compass direction.</summary>
        public static (double Lat, double Lon) Move(double lat, double lon, Compass dir, double metres) =>
            MoveBearing(lat, lon, Bearing(dir), metres);

        /// <summary>Position reached after travelling <paramref name="metres"/> on a constant true bearing (great-circle destination point).</summary>
        public static (double Lat, double Lon) MoveBearing(double lat, double lon, double bearingDeg, double metres)
        {
            double d = metres / EarthRadiusM;
            double br = bearingDeg * Math.PI / 180.0;
            double phi1 = lat * Math.PI / 180.0, lam1 = lon * Math.PI / 180.0;
            double phi2 = Math.Asin(Math.Sin(phi1) * Math.Cos(d) + Math.Cos(phi1) * Math.Sin(d) * Math.Cos(br));
            double lam2 = lam1 + Math.Atan2(Math.Sin(br) * Math.Sin(d) * Math.Cos(phi1), Math.Cos(d) - Math.Sin(phi1) * Math.Sin(phi2));
            double newLon = (lam2 * 180.0 / Math.PI + 540.0) % 360.0 - 180.0;      // normalise to -180..180
            return (Math.Clamp(phi2 * 180.0 / Math.PI, -90, 90), newLon);
        }

        public const double KnotsToMetresPerSecond = 0.514444;

        /// <summary>Direction for the up/down/left/right state (north = up). Null when nothing, or opposite keys cancel out.</summary>
        public static Compass? FromKeys(bool up, bool down, bool left, bool right)
        {
            int dy = (up ? 1 : 0) - (down ? 1 : 0);
            int dx = (right ? 1 : 0) - (left ? 1 : 0);
            return (dx, dy) switch
            {
                (0, 1) => Compass.N,
                (1, 1) => Compass.NE,
                (1, 0) => Compass.E,
                (1, -1) => Compass.SE,
                (0, -1) => Compass.S,
                (-1, -1) => Compass.SW,
                (-1, 0) => Compass.W,
                (-1, 1) => Compass.NW,
                _ => null,
            };
        }
    }
}
