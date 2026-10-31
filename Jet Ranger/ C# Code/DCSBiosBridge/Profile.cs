using System.Globalization;
using System.Text.Json;

namespace DCSBiosBridge
{
    // One generic value: a token the panel boards understand (LED_H_ENG_OIL, IAS, PITCH, MAINBUS, ...) and the DCS-BIOS control(s) that supply it.
    //   lamp        1 when the control is at or above Threshold of its maximum (Invert: on when it is off).
    //   analog      the control as a fraction of its maximum (0 to 1), or its raw value when Raw is set, times Scale plus Offset. When the value has calibration
    //               Points (fraction or raw value -> real value, from the Calibrate button) they are used instead, by straight lines between the points.
    //   digits      Controls are the drum digits, most significant first; each reads as a digit when multiplied by 10 (10 is blank): the radar altimeter.
    //   coordinate  Controls are the degrees, minutes and fractional minutes (divide by 65535) of a position and DirControl its N, S, E or W letter: DCS-BIOS CommonData.
    // Provisional marks a needle position that has only a first-guess scale: it is shown as "uncalibrated" until points are added.
    public sealed class ProfileValue
    {
        public string Token { get; set; } = "";
        public string Control { get; set; } = "";
        public List<string> Controls { get; set; } = new();
        public string DirControl { get; set; } = "";
        public string Hint { get; set; } = "";
        public string Type { get; set; } = "lamp";
        public double Threshold { get; set; } = 0.5;
        public double Scale { get; set; } = 1.0;
        public double Offset { get; set; } = 0.0;
        public int Decimals { get; set; } = 0;
        public bool Invert { get; set; }
        public bool Raw { get; set; }
        public bool Provisional { get; set; }
        public List<double[]>? Points { get; set; }
    }

    public sealed class ValueResult
    {
        public string Token = "";
        public string Sent = "";          // the text for the board
        public string Shown = "";         // the raw reading for the table
        public double X;                  // the input a calibration point is made at (fraction, or raw value)
        public bool Analog;
        public string Status = "";
    }

    // An aircraft profile: which DCS-BIOS module file to read, the aircraft names to recognise it by and the list of generic values.
    public sealed class Profile
    {
        public string Module { get; set; } = "";
        public string JsonFile { get; set; } = "";
        public List<string> Aircraft { get; set; } = new();
        public List<ProfileValue> Values { get; set; } = new();
        public string FileName { get; set; } = "";

        static readonly JsonSerializerOptions Options = new() { PropertyNameCaseInsensitive = true, ReadCommentHandling = JsonCommentHandling.Skip, AllowTrailingCommas = true };

        public static Profile Load(string path)
        {
            var p = JsonSerializer.Deserialize<Profile>(File.ReadAllText(path), Options) ?? throw new InvalidDataException("empty profile");
            p.FileName = Path.GetFileName(path);
            if (p.JsonFile.Length == 0) p.JsonFile = p.Module + ".json";
            return p;
        }

        public bool Matches(string aircraftName)
            => aircraftName.Length > 0 && Aircraft.Any(a => string.Equals(a, aircraftName, StringComparison.OrdinalIgnoreCase));

        // The values every aircraft can give (profiles\_common.json: CommonData) plus the aircraft's own; the aircraft's own win for a token both have.
        public static Profile? Merge(Profile? common, Profile? aircraft)
        {
            if (common == null && aircraft == null) return null;
            var merged = new Profile { Module = aircraft?.Module ?? "Generic (CommonData only)", JsonFile = aircraft?.JsonFile ?? "", FileName = aircraft?.FileName ?? common!.FileName };
            if (aircraft != null) merged.Aircraft = aircraft.Aircraft;
            var own = new HashSet<string>(aircraft?.Values.Select(v => v.Token) ?? Enumerable.Empty<string>());
            if (common != null) merged.Values.AddRange(common.Values.Where(v => !own.Contains(v.Token)));
            if (aircraft != null) merged.Values.AddRange(aircraft.Values);
            return merged;
        }

        static bool TryRead(ExportState state, Dictionary<string, BiosControl> controls, string id, out int value, out int max)
        {
            value = 0; max = 1;
            if (id.Length == 0 || !controls.TryGetValue(id, out var c) || c.Output == null) return false;
            value = state.ReadValue(c.Output.Address, c.Output.Mask, c.Output.Shift);
            max = c.Output.MaxValue;
            return true;
        }

        static double Interpolate(List<double[]> points, double x)
        {
            var p = points.OrderBy(a => a[0]).ToList();
            if (x <= p[0][0]) return p[0][1];
            if (x >= p[^1][0]) return p[^1][1];
            for (int i = 1; i < p.Count; i++)
            {
                if (x <= p[i][0])
                {
                    double span = p[i][0] - p[i - 1][0];
                    return span <= 0 ? p[i][1] : p[i - 1][1] + (p[i][1] - p[i - 1][1]) * (x - p[i - 1][0]) / span;
                }
            }
            return p[^1][1];
        }

        // The value of every mapped token from the current cockpit state.
        public List<ValueResult> Evaluate(ExportState state, Dictionary<string, BiosControl> controls)
        {
            var results = new List<ValueResult>();
            foreach (var v in Values)
            {
                var r = new ValueResult { Token = v.Token };
                string type = v.Type.ToLowerInvariant();
                if (type == "digits")
                {
                    if (v.Controls.Count == 0) continue;
                    double number = 0;
                    bool ok = true;
                    foreach (var id in v.Controls)
                    {
                        if (!TryRead(state, controls, id, out int raw, out int max)) { ok = false; break; }
                        int digit = (int)Math.Round((double)raw / max * 10.0);
                        number = number * 10 + (digit >= 10 ? 0 : digit);
                    }
                    if (!ok) continue;
                    r.Sent = number.ToString("F0", CultureInfo.InvariantCulture);
                    r.Shown = r.Sent;
                    r.Status = "ok";
                }
                else if (type == "coordinate")
                {
                    if (v.Controls.Count < 3) continue;
                    if (!TryRead(state, controls, v.Controls[0], out int deg, out _) || !TryRead(state, controls, v.Controls[1], out int min, out _) || !TryRead(state, controls, v.Controls[2], out int frac, out int fmax)) continue;
                    double value = deg + (min + (double)frac / fmax) / 60.0;
                    if (v.DirControl.Length > 0 && controls.TryGetValue(v.DirControl, out var dc) && dc.Text != null)
                    {
                        string dir = state.ReadString(dc.Text.Address, dc.Text.MaxLength);
                        if (dir.StartsWith("S", StringComparison.OrdinalIgnoreCase) || dir.StartsWith("W", StringComparison.OrdinalIgnoreCase)) value = -value;
                    }
                    r.Sent = value.ToString("F6", CultureInfo.InvariantCulture);
                    r.Shown = r.Sent;
                    r.Status = "ok";
                }
                else
                {
                    if (!TryRead(state, controls, v.Control, out int raw, out int max)) continue;
                    double fraction = (double)raw / max;
                    if (type == "analog")
                    {
                        double x = v.Raw ? raw : fraction;
                        bool calibrated = v.Points != null && v.Points.Count >= 2;
                        double y = calibrated ? Interpolate(v.Points!, x) : x * v.Scale + v.Offset;
                        r.Sent = y.ToString("F" + v.Decimals, CultureInfo.InvariantCulture);
                        r.Shown = v.Raw ? raw.ToString(CultureInfo.InvariantCulture) : fraction.ToString("F3", CultureInfo.InvariantCulture);
                        r.X = x;
                        r.Analog = true;
                        r.Status = calibrated ? "calibrated (" + v.Points!.Count + " points)" : (v.Provisional ? "uncalibrated" : "ok");
                    }
                    else
                    {
                        bool on = fraction >= v.Threshold;
                        r.Sent = (on != v.Invert) ? "1" : "0";
                        r.Shown = raw.ToString(CultureInfo.InvariantCulture);
                        r.Status = "ok";
                    }
                }
                results.Add(r);
            }
            return results;
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

    // Calibration points made with the Calibrate button, kept next to the profile (UH-1H.calibration.json) so the profile file and its comments are not rewritten:
    // token -> list of [input, real value].
    public static class Calibration
    {
        static readonly JsonSerializerOptions Options = new() { WriteIndented = true };

        public static string PathFor(string profileDir, string profileFile) => System.IO.Path.Combine(profileDir, System.IO.Path.GetFileNameWithoutExtension(profileFile) + ".calibration.json");

        public static Dictionary<string, List<double[]>> Load(string path)
        {
            try
            {
                if (File.Exists(path))
                    return JsonSerializer.Deserialize<Dictionary<string, List<double[]>>>(File.ReadAllText(path)) ?? new();
            }
            catch (Exception) { }
            return new();
        }

        public static void Save(string path, Dictionary<string, List<double[]>> data)
            => File.WriteAllText(path, JsonSerializer.Serialize(data, Options));

        public static void Apply(Profile profile, Dictionary<string, List<double[]>> data)
        {
            foreach (var v in profile.Values)
                if (data.TryGetValue(v.Token, out var pts) && pts.Count > 0) v.Points = pts;
        }
    }
}
