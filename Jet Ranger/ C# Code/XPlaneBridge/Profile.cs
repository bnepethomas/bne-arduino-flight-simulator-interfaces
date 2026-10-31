using System.Globalization;
using System.Text.Json;

namespace XPlaneBridge
{
    // One generic value: a token the panel boards understand (IAS, VSI, PITCH, MAINBUS, LED_H_ENG_OIL, RLOW, ...) and the X-Plane dataref(s) that supply it.
    // X-Plane datarefs are in real units, so no calibration is needed, only a conversion to the units the boards take.
    //   analog  value / Divisor times Scale plus Offset, Decimals places, Suffix added ("21" + "C"). Divisor is another dataref, for a ratio (torque / maximum torque).
    //   lamp    1 when the dataref is at or above Threshold (a bit field or an int flag counts as on), Invert reverses it.
    //   hhmm    Datarefs are the hours and the minutes; the token is hours * 100 + minutes (the Dual Stepper clock).
    //   utc     Datarefs are the zulu seconds since midnight, the zulu month and the zulu day (X-Plane has no year: the PC's UTC year is used); the token is
    //           yyyyMMddHHmmss.fff, the simulator time the GPS interface follows.
    public sealed class ProfileValue
    {
        public string Token { get; set; } = "";
        public string Dataref { get; set; } = "";
        public List<string> Datarefs { get; set; } = new();
        public string Divisor { get; set; } = "";
        public string Type { get; set; } = "analog";
        public double Scale { get; set; } = 1.0;
        public double Offset { get; set; } = 0.0;
        public int Decimals { get; set; } = 0;
        public string Suffix { get; set; } = "";
        public double Threshold { get; set; } = 0.5;
        public bool Invert { get; set; }
        public string Hint { get; set; } = "";
        public string Note { get; set; } = "";

        public IEnumerable<string> AllDatarefs()
        {
            if (Dataref.Length > 0) yield return Dataref;
            foreach (var d in Datarefs) yield return d;
            if (Divisor.Length > 0) yield return Divisor;
        }
    }

    public sealed class ValueResult
    {
        public string Token = "";
        public string Sent = "";
        public string Shown = "";
        public string Status = "";
    }

    // A profile is a list of generic values. profiles\_common.json is added to every aircraft's profile (an aircraft's own value for the same token wins) and works
    // on its own for any aircraft; a profile for a particular aircraft adds what depends on it (rotor RPM, the bus voltage, ...).
    public sealed class Profile
    {
        public string Module { get; set; } = "";
        public List<ProfileValue> Values { get; set; } = new();
        public string FileName { get; set; } = "";

        static readonly JsonSerializerOptions Options = new() { PropertyNameCaseInsensitive = true, ReadCommentHandling = JsonCommentHandling.Skip, AllowTrailingCommas = true };

        public static Profile Load(string path)
        {
            var p = JsonSerializer.Deserialize<Profile>(File.ReadAllText(path), Options) ?? throw new InvalidDataException("empty profile");
            p.FileName = Path.GetFileName(path);
            return p;
        }

        public static Profile Merge(Profile? common, Profile? aircraft)
        {
            var merged = new Profile { Module = aircraft?.Module ?? "Generic (common values only)", FileName = aircraft?.FileName ?? common?.FileName ?? "" };
            var own = new HashSet<string>(aircraft?.Values.Select(v => v.Token) ?? Enumerable.Empty<string>());
            if (common != null) merged.Values.AddRange(common.Values.Where(v => !own.Contains(v.Token)));
            if (aircraft != null) merged.Values.AddRange(aircraft.Values);
            return merged;
        }

        public IEnumerable<string> AllDatarefs() => Values.SelectMany(v => v.AllDatarefs()).Distinct();

        public List<ValueResult> Evaluate(XPlaneLink link, XPlaneState state)
        {
            var results = new List<ValueResult>();
            bool Read(string path, out double value)
            {
                value = 0;
                if (path.Length == 0 || !state.TryGet(link.IndexOf(path), out float f)) return false;
                value = f;
                return true;
            }
            foreach (var v in Values)
            {
                var r = new ValueResult { Token = v.Token, Status = "ok" };
                string type = v.Type.ToLowerInvariant();
                if (type == "hhmm")
                {
                    if (v.Datarefs.Count < 2 || !Read(v.Datarefs[0], out double h) || !Read(v.Datarefs[1], out double m)) continue;
                    r.Sent = ((int)h * 100 + (int)m).ToString(CultureInfo.InvariantCulture);
                    r.Shown = r.Sent;
                }
                else if (type == "utc")
                {
                    if (v.Datarefs.Count < 3 || !Read(v.Datarefs[0], out double sec) || !Read(v.Datarefs[1], out double month) || !Read(v.Datarefs[2], out double day)) continue;
                    if (month < 1 || month > 12 || day < 1 || day > 31 || sec < 0 || sec >= 86400) continue;
                    try
                    {
                        r.Sent = new DateTime(DateTime.UtcNow.Year, (int)month, (int)day).AddSeconds(Math.Floor(sec * 1000) / 1000).ToString("yyyyMMddHHmmss.fff", CultureInfo.InvariantCulture);
                    }
                    catch (ArgumentOutOfRangeException) { continue; }
                    r.Shown = r.Sent;
                }
                else
                {
                    if (!Read(v.Dataref, out double x)) continue;
                    r.Shown = x.ToString("0.###", CultureInfo.InvariantCulture);
                    if (type == "lamp")
                    {
                        bool on = x >= v.Threshold;
                        r.Sent = (on != v.Invert) ? "1" : "0";
                    }
                    else
                    {
                        if (v.Divisor.Length > 0)
                        {
                            if (!Read(v.Divisor, out double d) || d <= 0) continue;
                            x /= d;
                        }
                        r.Sent = (x * v.Scale + v.Offset).ToString("F" + v.Decimals, CultureInfo.InvariantCulture) + v.Suffix;
                    }
                }
                results.Add(r);
            }
            return results;
        }
    }

    // The datarefs X-Plane publishes, read from <X-Plane>\Resources\plugins\DataRefs.txt: name, type, writable, units, description (tab separated). Used by the
    // Search button to find the dataref for a new value.
    public sealed class DatarefCatalog
    {
        public readonly List<(string Name, string Type, string Units, string Description)> Entries = new();

        public static DatarefCatalog? Load(string path)
        {
            if (!File.Exists(path)) return null;
            var c = new DatarefCatalog();
            foreach (var line in File.ReadLines(path))
            {
                var f = line.Split('\t');
                if (f.Length < 4 || !f[0].StartsWith("sim/")) continue;
                c.Entries.Add((f[0], f[1], f[3], f.Length > 4 ? f[4] : ""));
            }
            return c;
        }

        public List<(string Name, string Type, string Units, string Description)> Search(string hint, int max = 8)
        {
            var words = hint.Split(new[] { ' ', '_', '-', '/' }, StringSplitOptions.RemoveEmptyEntries).Select(w => w.ToUpperInvariant()).ToArray();
            if (words.Length == 0) return new();
            return Entries
                .Select(e => (e, score: words.Count(w => (e.Name + " " + e.Description).ToUpperInvariant().Contains(w))))
                .Where(x => x.score == words.Length)
                .OrderBy(x => x.e.Name.Length)
                .Take(max).Select(x => x.e).ToList();
        }
    }
}
