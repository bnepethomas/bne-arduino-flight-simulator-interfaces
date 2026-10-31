using System.Text.Json;

namespace DCSBiosBridge
{
    // One integer output of a DCS-BIOS control: the value is (word at Address & Mask) >> Shift, from 0 to MaxValue.
    public sealed record BiosOutput(int Address, int Mask, int Shift, int MaxValue);

    // One string output (for example the N or S of the latitude): MaxLength characters at Address.
    public sealed record BiosStringOutput(int Address, int MaxLength);

    public sealed record BiosControl(string Id, string Category, string ControlType, string Description, BiosOutput? Output, BiosStringOutput? Text = null);

    // Reads a DCS-BIOS module definition (Saved Games\DCS\Scripts\DCS-BIOS\doc\json\<module>.json, written by DCS-BIOS itself). The file is category -> identifier ->
    // control; the reader looks for any object with an "identifier" and "outputs", so a small change in the layout does not break it. CommonData.json holds the
    // values every aircraft exports (airspeed, altitude, heading, position).
    public static class ModuleDoc
    {
        public static Dictionary<string, BiosControl> Load(string path)
        {
            var result = new Dictionary<string, BiosControl>(StringComparer.OrdinalIgnoreCase);
            LoadInto(path, result);
            return result;
        }

        // Adds the controls of a file to a dictionary (a later file replaces a control of the same identifier).
        public static int LoadInto(string path, Dictionary<string, BiosControl> into)
        {
            using var doc = JsonDocument.Parse(File.ReadAllText(path), new JsonDocumentOptions { AllowTrailingCommas = true });
            int before = into.Count;
            Walk(doc.RootElement, into);
            return into.Count - before;
        }

        static void Walk(JsonElement e, Dictionary<string, BiosControl> into)
        {
            if (e.ValueKind != JsonValueKind.Object) return;
            if (e.TryGetProperty("identifier", out var id) && id.ValueKind == JsonValueKind.String && e.TryGetProperty("outputs", out var outs) && outs.ValueKind == JsonValueKind.Array)
            {
                BiosOutput? first = null;
                BiosStringOutput? text = null;
                foreach (var o in outs.EnumerateArray())
                {
                    string type = o.TryGetProperty("type", out var t) && t.ValueKind == JsonValueKind.String ? t.GetString() ?? "" : "integer";
                    if (!o.TryGetProperty("address", out var a)) continue;
                    if (type == "string")
                    {
                        if (text == null && o.TryGetProperty("max_length", out var ml) && ml.ValueKind == JsonValueKind.Number)
                            text = new BiosStringOutput(a.GetInt32(), ml.GetInt32());
                        continue;
                    }
                    if (type != "integer" || first != null || !o.TryGetProperty("mask", out var m)) continue;
                    int shift = o.TryGetProperty("shift_by", out var sh) && sh.ValueKind == JsonValueKind.Number ? sh.GetInt32() : 0;
                    int max = o.TryGetProperty("max_value", out var mv) && mv.ValueKind == JsonValueKind.Number ? mv.GetInt32() : 1;
                    first = new BiosOutput(a.GetInt32(), m.GetInt32(), shift, Math.Max(1, max));
                }
                string cat = e.TryGetProperty("category", out var c) && c.ValueKind == JsonValueKind.String ? c.GetString() ?? "" : "";
                string ctype = e.TryGetProperty("control_type", out var ct) && ct.ValueKind == JsonValueKind.String ? ct.GetString() ?? "" : "";
                string desc = e.TryGetProperty("description", out var d) && d.ValueKind == JsonValueKind.String ? d.GetString() ?? "" : "";
                into[id.GetString()!] = new BiosControl(id.GetString()!, cat, ctype, desc, first, text);
                return;
            }
            foreach (var p in e.EnumerateObject()) Walk(p.Value, into);
        }
    }
}
