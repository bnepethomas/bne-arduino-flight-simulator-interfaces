// src/SimConnectCrossPlatform/Translation/TranslatedVar.cs

namespace SimConnectCrossPlatform.Translation
{
    /// <summary>
    /// Result of translating a canonical SimVar name for a platform
    /// </summary>
    public class TranslatedVar
    {
        /// <summary>Platform-specific variable name</summary>
        public string Name { get; set; } = string.Empty;

        /// <summary>Platform-specific units</summary>
        public string Units { get; set; } = string.Empty;

        /// <summary>Whether this variable is supported on the platform</summary>
        public bool IsSupported { get; set; } = true;

        /// <summary>Optional notes about platform differences</summary>
        public string? Notes { get; set; }

        public override string ToString() =>
            IsSupported
                ? $"{Name} ({Units})"
                : $"[NOT SUPPORTED] {Name} - {Notes}";
    }
}