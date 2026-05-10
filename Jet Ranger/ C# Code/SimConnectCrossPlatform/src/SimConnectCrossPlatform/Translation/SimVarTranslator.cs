// src/SimConnectCrossPlatform/Translation/SimVarTranslator.cs

using System.Collections.Generic;
using Microsoft.Extensions.Logging;
using SimConnectCrossPlatform.Models;

namespace SimConnectCrossPlatform.Translation
{
    /// <summary>
    /// Translates canonical SimVar names to platform-specific equivalents.
    /// Most variables are identical across platforms (passthrough).
    /// Only variables that differ are explicitly mapped.
    /// </summary>
    public class SimVarTranslator
    {
        private readonly SimPlatform _platform;
        private readonly ILogger? _logger;

        private class PlatformMapping
        {
            public string P3DName { get; init; } = string.Empty;
            public string P3DUnits { get; init; } = string.Empty;
            public bool P3DSupported { get; init; } = true;

            public string MSFSName { get; init; } = string.Empty;
            public string MSFSUnits { get; init; } = string.Empty;
            public bool MSFSSupported { get; init; } = true;

            public string? Notes { get; init; }
        }

        private readonly Dictionary<string, PlatformMapping> _mappings = new();

        public SimVarTranslator(SimPlatform platform, ILogger? logger = null)
        {
            _platform = platform;
            _logger = logger;
            InitializeMappings();

            _logger?.LogInformation(
                "SimVarTranslator initialized for {Platform} " +
                "with {Count} special mappings",
                platform, _mappings.Count);
        }

        private void InitializeMappings()
        {
            // ============================================
            // Variables with DIFFERENT behavior/names
            // ============================================

            _mappings["SURFACE_TYPE"] = new PlatformMapping
            {
                P3DName = "SURFACE TYPE",
                P3DUnits = "enum",
                P3DSupported = true,
                MSFSName = "SURFACE TYPE",
                MSFSUnits = "enum",
                MSFSSupported = true,
                Notes = "Enum values differ between platforms"
            };

            _mappings["CAMERA_STATE"] = new PlatformMapping
            {
                P3DName = "CAMERA STATE",
                P3DUnits = "enum",
                P3DSupported = true,
                MSFSName = "CAMERA STATE",
                MSFSUnits = "enum",
                MSFSSupported = true,
                Notes = "Camera state enum values may differ"
            };

            // ============================================
            // MSFS2024-only variables
            // ============================================

            _mappings["INTERIOR_COCKPIT_AMBIENT_LIGHT"] = new PlatformMapping
            {
                P3DName = "",
                P3DUnits = "",
                P3DSupported = false,
                MSFSName = "INTERIOR COCKPIT AMBIENT LIGHT",
                MSFSUnits = "percent",
                MSFSSupported = true,
                Notes = "MSFS2024 only — cockpit ambient lighting"
            };

            _mappings["WHEEL_RPM"] = new PlatformMapping
            {
                P3DName = "",
                P3DUnits = "",
                P3DSupported = false,
                MSFSName = "WHEEL RPM",
                MSFSUnits = "rpm",
                MSFSSupported = true,
                Notes = "MSFS2024 only — wheel rotation speed"
            };

            // ============================================
            // P3D-only variables
            // ============================================

            _mappings["PANEL_LIGHT_SWITCH"] = new PlatformMapping
            {
                P3DName = "PANEL LIGHT SWITCH",
                P3DUnits = "bool",
                P3DSupported = true,
                MSFSName = "",
                MSFSUnits = "",
                MSFSSupported = false,
                Notes = "P3D only — legacy panel light control"
            };

            _mappings["COUNTERMEASURE_CHAFF_COUNT"] = new PlatformMapping
            {
                P3DName = "COUNTERMEASURE CHAFF COUNT",
                P3DUnits = "number",
                P3DSupported = true,
                MSFSName = "",
                MSFSUnits = "",
                MSFSSupported = false,
                Notes = "P3D Professional only — military feature"
            };

            // ============================================
            // Variables with different units handling
            // ============================================

            // Add more mappings as needed...
        }

        /// <summary>
        /// Translate a canonical variable name to the current platform
        /// </summary>
        /// <param name="canonicalName">
        /// The canonical (platform-agnostic) variable name
        /// </param>
        /// <param name="requestedUnits">
        /// Requested units string
        /// </param>
        /// <returns>Translated variable information</returns>
        public TranslatedVar Translate(
            string canonicalName, string requestedUnits)
        {
            // Check for special mapping
            if (_mappings.TryGetValue(canonicalName, out var mapping))
            {
                return _platform switch
                {
                    SimPlatform.MSFS2024 => new TranslatedVar
                    {
                        Name = mapping.MSFSName,
                        Units = string.IsNullOrEmpty(mapping.MSFSUnits)
                            ? requestedUnits : mapping.MSFSUnits,
                        IsSupported = mapping.MSFSSupported,
                        Notes = mapping.MSFSSupported
                            ? mapping.Notes
                            : $"Not supported on MSFS2024. {mapping.Notes}"
                    },
                    SimPlatform.P3D => new TranslatedVar
                    {
                        Name = mapping.P3DName,
                        Units = string.IsNullOrEmpty(mapping.P3DUnits)
                            ? requestedUnits : mapping.P3DUnits,
                        IsSupported = mapping.P3DSupported,
                        Notes = mapping.P3DSupported
                            ? mapping.Notes
                            : $"Not supported on P3D. {mapping.Notes}"
                    },
                    _ => new TranslatedVar
                    {
                        Name = canonicalName,
                        Units = requestedUnits,
                        IsSupported = false,
                        Notes = "Unknown platform"
                    }
                };
            }

            // No special mapping — passthrough (most variables)
            return new TranslatedVar
            {
                Name = canonicalName,
                Units = requestedUnits,
                IsSupported = true
            };
        }

        /// <summary>
        /// Check if a canonical variable name is supported
        /// on the current platform
        /// </summary>
        public bool IsSupported(string canonicalName)
        {
            if (!_mappings.TryGetValue(canonicalName, out var mapping))
                return true; // Passthrough assumed supported

            return _platform switch
            {
                SimPlatform.MSFS2024 => mapping.MSFSSupported,
                SimPlatform.P3D => mapping.P3DSupported,
                _ => false
            };
        }

        /// <summary>
        /// Get all known platform-specific mappings
        /// </summary>
        public IReadOnlyDictionary<string, TranslatedVar>
            GetAllMappings()
        {
            var result = new Dictionary<string, TranslatedVar>();
            foreach (var kvp in _mappings)
            {
                result[kvp.Key] = Translate(kvp.Key, "");
            }
            return result;
        }
    }
}