using System.Globalization;
using System.Reflection;

namespace LowranceAirMapGpsBenchTester
{
    public record Airport(string CountryCode, string City, string Name, string Icao, string Iata, double Lat, double Lon, double ElevationFt)
    {
        public override string ToString() => $"{Icao}  {Name}";
    }

    public record Country(string Code, string Name)
    {
        public override string ToString() => Name;
    }

    /// <summary>
    /// Airports with an ICAO code, loaded from the embedded airports.tsv (trimmed from the OurAirports data, public domain).
    /// Positions are each airport's reference point. Columns: country, city, name, ICAO, IATA, latitude, longitude, elevation ft.
    /// </summary>
    public class AirportDb
    {
        private readonly List<Airport> _all = new();

        public static AirportDb Load()
        {
            var db = new AirportDb();
            using Stream? s = Assembly.GetExecutingAssembly().GetManifestResourceStream("airports.tsv");
            if (s == null) return db;
            using var r = new StreamReader(s);
            string? line;
            while ((line = r.ReadLine()) != null)
            {
                string[] f = line.Split('\t');
                if (f.Length < 8) continue;
                if (!double.TryParse(f[5], NumberStyles.Float, CultureInfo.InvariantCulture, out double lat) ||
                    !double.TryParse(f[6], NumberStyles.Float, CultureInfo.InvariantCulture, out double lon)) continue;
                double.TryParse(f[7], NumberStyles.Float, CultureInfo.InvariantCulture, out double el);
                db._all.Add(new Airport(f[0], string.IsNullOrWhiteSpace(f[1]) ? "(no city)" : f[1], f[2], f[3], f[4], lat, lon, el));
            }
            return db;
        }

        public int Count => _all.Count;

        public IReadOnlyList<Country> Countries() =>
            _all.Select(a => a.CountryCode).Distinct()
                .Select(c => new Country(c, CountryName(c)))
                .OrderBy(c => c.Name, StringComparer.CurrentCultureIgnoreCase).ToList();

        public IReadOnlyList<string> Cities(string countryCode) =>
            _all.Where(a => a.CountryCode == countryCode).Select(a => a.City).Distinct()
                .OrderBy(c => c, StringComparer.CurrentCultureIgnoreCase).ToList();

        public IReadOnlyList<Airport> Airports(string countryCode, string city) =>
            _all.Where(a => a.CountryCode == countryCode && a.City == city)
                .OrderBy(a => a.Name, StringComparer.CurrentCultureIgnoreCase).ToList();

        public Airport? Find(string icao) => _all.FirstOrDefault(a => a.Icao.Equals(icao, StringComparison.OrdinalIgnoreCase));

        private static string CountryName(string code)
        {
            try { return new RegionInfo(code).EnglishName; }
            catch (ArgumentException) { return code; }      // codes such as XK that .NET does not know
        }
    }
}
