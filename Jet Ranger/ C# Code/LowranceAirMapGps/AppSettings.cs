using System.Text.Json;
using System.Text.Json.Serialization;

namespace LowranceAirMapGps
{
    /// <summary>Saved settings, stored as JSON in %AppData%\LowranceAirMapGps\settings.json.</summary>
    public class AppSettings
    {
        /// <summary>Saved serial port, e.g. COM7. Null until the user has chosen one.</summary>
        public string? SerialPort { get; set; }

        /// <summary>UDP port the aircraft information arrives on.</summary>
        public int UdpPort { get; set; } = 27002;

        // Default position: the threshold of runway 01L at Brisbane Airport (YBBN), on the ground.
        // Heading is degrees TRUE (the runway is 016 magnetic, 11 degrees east variation).
        public double DefaultLatitude { get; set; } = -27.3830;
        public double DefaultLongitude { get; set; } = 153.1068;
        public double DefaultAltitudeFt { get; set; } = 13;
        public double DefaultHeadingDeg { get; set; } = 27;

        private static string FilePath =>
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "LowranceAirMapGps", "settings.json");

        [JsonIgnore]
        public AircraftData DefaultAircraft => new(DefaultLatitude, DefaultLongitude, DefaultAltitudeFt, 0, DefaultHeadingDeg);

        public static AppSettings Load()
        {
            try
            {
                if (File.Exists(FilePath))
                    return JsonSerializer.Deserialize<AppSettings>(File.ReadAllText(FilePath)) ?? new AppSettings();
            }
            catch { /* a damaged file falls back to the defaults */ }
            return new AppSettings();
        }

        public void Save()
        {
            try
            {
                Directory.CreateDirectory(Path.GetDirectoryName(FilePath)!);
                File.WriteAllText(FilePath, JsonSerializer.Serialize(this, new JsonSerializerOptions { WriteIndented = true }));
            }
            catch { /* not fatal: the user is simply asked again next time */ }
        }
    }
}
