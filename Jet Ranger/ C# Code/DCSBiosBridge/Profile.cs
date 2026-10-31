using System.Globalization;
using System.Text.Json;

namespace DCSBiosBridge
{
    // One generic value: a token the panel boards understand (LED_H_ENG_OIL, IAS, PITCH, MAINBUS, ...) and the DCS-BIOS control that supplies it.
    public sealed class ProfileValue
    {
        public string Token { get; set; } = "";
        public string Control { get; set; } = "";      // DCS-BIOS control identifier; empty = not mapped yet
        public string Hint { get; set; } = "";         // words to search the module's controls for ("ENGINE OIL PRESS"), used by Suggest
        public string Type { get; set; } = "lamp";     // "lamp": 1 when the value is at or above Threshold of its maximum; "analog": raw / max * Scale + Offset
        public double Threshold { get; set; } = 0.5;
        public double Scale { get; set; } = 1.0;
        public double Offset { get; set; } = 0.0;
        public int Decimals { get; set; } = 0;
        public bool Invert { get; set; }               // lamp: on when the control is off (a light that is lit by a missing signal)
    }

    // An aircraft profile: which DCS-BIOS module file to read, the aircraft names to recognise it by and the list of generic values.
    public sealed class Profile
    {
        public string Module { get; set; } = "";
        public string JsonFile { get; set; } = "";
        public List<string> Aircraft { get; set; } = new();
        public List<ProfileValue> Values { get; set; } = new();
        public string FileName { get; set; } = "";

        public static Profile Load(string path)
        {
            var p = JsonSerializer.Deserialize<Profile>(File.ReadAllText(path), new JsonSerializerOptions { PropertyNameCaseInsensitive = true, ReadCommentHandling = JsonCommentHandling.Skip, AllowTrailingCommas = true })
                    ?? throw new InvalidDataException("empty profile");
            p.FileName = Path.GetFileName(path);
            if (p.JsonFile.Length == 0) p.JsonFile = p.Module + ".json";
            return p;
        }

        public bool Matches(string aircraftName)
            => aircraftName.Length > 0 && Aircraft.Any(a => string.Equals(a, aircraftName, StringComparison.OrdinalIgnoreCase));

        // The value of every mapped token from the current cockpit state, as the text the boards expect.
        public Dictionary<string, string> Evaluate(ExportState state, Dictionary<string, BiosControl> controls, Dictionary<string, double>? raw = null)
        {
            var result = new Dictionary<string, string>();
            foreach (var v in Values)
            {
                if (v.Control.Length == 0 || !controls.TryGetValue(v.Control, out var c) || c.Output == null) continue;
                int value = state.ReadValue(c.Output.Address, c.Output.Mask, c.Output.Shift);
                double fraction = (double)value / c.Output.MaxValue;
                if (raw != null) raw[v.Token] = value;
                if (v.Type.Equals("analog", StringComparison.OrdinalIgnoreCase))
                    result[v.Token] = (fraction * v.Scale + v.Offset).ToString("F" + v.Decimals, CultureInfo.InvariantCulture);
                else
                {
                    bool on = fraction >= v.Threshold;
                    result[v.Token] = (on != v.Invert) ? "1" : "0";
                }
            }
            return result;
        }

        // Controls of the module whose identifier or description contain the words of the hint, best match first.
        public static List<BiosControl> Suggest(string hint, Dictionary<string, BiosControl> controls, int max = 5)
        {
            var words = hint.Split(new[] { ' ', '_', '-' }, StringSplitOptions.RemoveEmptyEntries).Select(w => w.ToUpperInvariant()).ToArray();
            if (words.Length == 0) return new List<BiosControl>();
            return controls.Values
                .Where(c => c.Output != null)
                .Select(c => (c, score: words.Count(w => (c.Id + " " + c.Description).ToUpperInvariant().Contains(w))))
                .Where(x => x.score > 0)
                .OrderByDescending(x => x.score).ThenBy(x => x.c.Id.Length)
                .Take(max).Select(x => x.c).ToList();
        }
    }
}
