// src/SimConnectCrossPlatform/Loader/SimConnectDynamicLoader.cs

using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Extensions.Logging;
using Microsoft.Win32;
using SimConnectCrossPlatform.Models;

namespace SimConnectCrossPlatform.Loader
{
    /// <summary>
    /// Dynamically loads SimConnect.dll at runtime, supporting
    /// both MSFS2024 and Prepar3D without compile-time binding.
    /// </summary>
    public class SimConnectDynamicLoader : IDisposable
    {
        private IntPtr _hModule = IntPtr.Zero;
        private bool _disposed;
        private readonly ILogger? _logger;

        /// <summary>Detected simulator platform</summary>
        public SimPlatform DetectedPlatform { get; private set; }
            = SimPlatform.Unknown;

        /// <summary>Path of the loaded DLL</summary>
        public string? LoadedDllPath { get; private set; }

        /// <summary>Whether a DLL is currently loaded</summary>
        public bool IsLoaded => _hModule != IntPtr.Zero;

        // ===== Resolved Function Delegates =====
        internal SimConnectDelegates.SimConnect_Open? Open { get; private set; }
        internal SimConnectDelegates.SimConnect_Close? Close { get; private set; }
        internal SimConnectDelegates.SimConnect_AddToDataDefinition?
            AddToDataDefinition { get; private set; }
        internal SimConnectDelegates.SimConnect_ClearDataDefinition?
            ClearDataDefinition { get; private set; }
        internal SimConnectDelegates.SimConnect_RequestDataOnSimObjectType?
            RequestDataOnSimObjectType { get; private set; }
        internal SimConnectDelegates.SimConnect_RequestDataOnSimObject?
            RequestDataOnSimObject { get; private set; }
        internal SimConnectDelegates.SimConnect_SetDataOnSimObject?
            SetDataOnSimObject { get; private set; }
        internal SimConnectDelegates.SimConnect_MapClientEventToSimEvent?
            MapClientEventToSimEvent { get; private set; }
        internal SimConnectDelegates.SimConnect_TransmitClientEvent?
            TransmitClientEvent { get; private set; }
        internal SimConnectDelegates.SimConnect_GetNextDispatch?
            GetNextDispatch { get; private set; }
        internal SimConnectDelegates.SimConnect_SubscribeToSystemEvent?
            SubscribeToSystemEvent { get; private set; }
        internal SimConnectDelegates.SimConnect_UnsubscribeFromSystemEvent?
            UnsubscribeFromSystemEvent { get; private set; }

        public SimConnectDynamicLoader(ILogger? logger = null)
        {
            _logger = logger;
        }

        /// <summary>
        /// Auto-detect and load SimConnect from any available simulator.
        /// Tries MSFS2024 first, then P3D.
        /// </summary>
        public bool Load()
        {
            _logger?.LogInformation("Auto-detecting SimConnect...");

            if (Load(SimPlatform.MSFS2024))
            {
                _logger?.LogInformation(
                    "Loaded MSFS2024 SimConnect from: {Path}", LoadedDllPath);
                return true;
            }

            if (Load(SimPlatform.P3D))
            {
                _logger?.LogInformation(
                    "Loaded P3D SimConnect from: {Path}", LoadedDllPath);
                return true;
            }

            _logger?.LogError("Failed to load SimConnect from any source");
            return false;
        }

        /// <summary>
        /// Load SimConnect for a specific simulator platform
        /// </summary>
        public bool Load(SimPlatform platform)
        {
            Unload();

            var searchPaths = GetSearchPaths(platform);

            foreach (var path in searchPaths)
            {
                if (!File.Exists(path))
                {
                    _logger?.LogDebug("Path not found: {Path}", path);
                    continue;
                }

                _logger?.LogDebug("Trying to load: {Path}", path);

                _hModule = Win32Native.LoadLibrary(path);
                if (_hModule == IntPtr.Zero)
                {
                    var error = Win32Native.GetLastError();
                    _logger?.LogDebug(
                        "LoadLibrary failed for {Path}: error {Error}",
                        path, error);
                    continue;
                }

                if (ResolveFunctions())
                {
                    DetectedPlatform = platform;
                    LoadedDllPath = path;
                    _logger?.LogInformation(
                        "Successfully loaded SimConnect ({Platform}) " +
                        "from: {Path}", platform, path);
                    return true;
                }

                _logger?.LogDebug(
                    "Function resolution failed for: {Path}", path);
                Win32Native.FreeLibrary(_hModule);
                _hModule = IntPtr.Zero;
            }

            return false;
        }

        /// <summary>
        /// Load from an explicit file path
        /// </summary>
        public bool LoadFromPath(string dllPath, SimPlatform platform)
        {
            Unload();

            if (!File.Exists(dllPath))
            {
                _logger?.LogError("DLL not found: {Path}", dllPath);
                return false;
            }

            _hModule = Win32Native.LoadLibrary(dllPath);
            if (_hModule == IntPtr.Zero)
            {
                _logger?.LogError(
                    "Failed to load DLL: {Path}", dllPath);
                return false;
            }

            if (ResolveFunctions())
            {
                DetectedPlatform = platform;
                LoadedDllPath = dllPath;
                return true;
            }

            Win32Native.FreeLibrary(_hModule);
            _hModule = IntPtr.Zero;
            return false;
        }

        /// <summary>
        /// Unload the currently loaded SimConnect DLL
        /// </summary>
        public void Unload()
        {
            if (_hModule != IntPtr.Zero)
            {
                Win32Native.FreeLibrary(_hModule);
                _hModule = IntPtr.Zero;
            }

            DetectedPlatform = SimPlatform.Unknown;
            LoadedDllPath = null;
            ClearDelegates();
        }

        // =====================================================
        // Search Path Discovery
        // =====================================================

        private List<string> GetSearchPaths(SimPlatform platform)
        {
            var paths = new List<string>();

            if (platform == SimPlatform.MSFS2024 ||
                platform == SimPlatform.Unknown)
            {
                AddMSFS2024Paths(paths);
            }

            if (platform == SimPlatform.P3D ||
                platform == SimPlatform.Unknown)
            {
                AddP3DPaths(paths);
            }

            // Fallback: current directory and PATH
            paths.Add(Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory, "SimConnect.dll"));
            paths.Add("SimConnect.dll");

            return paths;
        }

        private void AddMSFS2024Paths(List<string> paths)
        {
            // 1. Registry-based detection
            try
            {
                string[] registryPaths = new[]
                {
                    @"SOFTWARE\Microsoft\Microsoft Games\Flight Simulator\SimConnect",
                    @"SOFTWARE\WOW6432Node\Microsoft\Microsoft Games\Flight Simulator\SimConnect"
                };

                foreach (var regPath in registryPaths)
                {
                    using var key = Registry.LocalMachine.OpenSubKey(regPath);
                    if (key == null) continue;

                    // Check multiple possible value names
                    foreach (var valueName in new[] {
                        "SimConnectMSFS", "InstallDir", "" })
                    {
                        var installDir = key.GetValue(valueName) as string;
                        if (!string.IsNullOrEmpty(installDir))
                        {
                            paths.Add(Path.Combine(installDir,
                                "SimConnect.dll"));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                _logger?.LogDebug(
                    "Registry access failed for MSFS: {Error}",
                    ex.Message);
            }

            // 2. Common installation paths
            var commonPaths = new[]
            {
                @"C:\MSFS SDK\SimConnect SDK\lib\SimConnect.dll",
                Path.Combine(
                    Environment.GetFolderPath(
                        Environment.SpecialFolder.LocalApplicationData),
                    @"Packages\Microsoft.Limitless_8wekyb3d8bbwe\" +
                    @"LocalCache\SimConnect.dll"),
                Path.Combine(
                    Environment.GetFolderPath(
                        Environment.SpecialFolder.ProgramFiles),
                    @"Microsoft Games\Microsoft Flight Simulator 2024\" +
                    @"SimConnect.dll"),
            };

            paths.AddRange(commonPaths);
        }

        private void AddP3DPaths(List<string> paths)
        {
            string[] versions = { "v6", "v5", "v4" };

            foreach (var version in versions)
            {
                // 1. Registry
                try
                {
                    var regKey = $@"SOFTWARE\Lockheed Martin\Prepar3D {version}";
                    using var key = Registry.LocalMachine.OpenSubKey(regKey);
                    var installDir = key?.GetValue("SetupPath") as string;
                    if (!string.IsNullOrEmpty(installDir))
                    {
                        paths.Add(Path.Combine(installDir,
                            "SimConnect.dll"));
                    }
                }
                catch (Exception ex)
                {
                    _logger?.LogDebug(
                        "Registry access failed for P3D {Version}: {Error}",
                        version, ex.Message);
                }

                // 2. Common paths
                var programFiles = Environment.GetFolderPath(
                    Environment.SpecialFolder.ProgramFiles);

                paths.Add(Path.Combine(programFiles,
                    $@"Lockheed Martin\Prepar3D {version}\SimConnect.dll"));

                paths.Add(Path.Combine(programFiles,
                    $@"Lockheed Martin\Prepar3D {version}\redist\" +
                    $@"Interface\FSX-SP2-XPACK\retail\lib\SimConnect.dll"));
            }
        }

        // =====================================================
        // Function Resolution
        // =====================================================

        private bool ResolveFunctions()
        {
            try
            {
                // Required functions
                Open = ResolveRequired
                    <SimConnectDelegates.SimConnect_Open>(
                    "SimConnect_Open");
                Close = ResolveRequired
                    <SimConnectDelegates.SimConnect_Close>(
                    "SimConnect_Close");
                AddToDataDefinition = ResolveRequired
                    <SimConnectDelegates.SimConnect_AddToDataDefinition>(
                    "SimConnect_AddToDataDefinition");
                RequestDataOnSimObjectType = ResolveRequired
                    <SimConnectDelegates.SimConnect_RequestDataOnSimObjectType>(
                    "SimConnect_RequestDataOnSimObjectType");
                RequestDataOnSimObject = ResolveRequired
                    <SimConnectDelegates.SimConnect_RequestDataOnSimObject>(
                    "SimConnect_RequestDataOnSimObject");
                SetDataOnSimObject = ResolveRequired
                    <SimConnectDelegates.SimConnect_SetDataOnSimObject>(
                    "SimConnect_SetDataOnSimObject");
                MapClientEventToSimEvent = ResolveRequired
                    <SimConnectDelegates.SimConnect_MapClientEventToSimEvent>(
                    "SimConnect_MapClientEventToSimEvent");
                TransmitClientEvent = ResolveRequired
                    <SimConnectDelegates.SimConnect_TransmitClientEvent>(
                    "SimConnect_TransmitClientEvent");
                GetNextDispatch = ResolveRequired
                    <SimConnectDelegates.SimConnect_GetNextDispatch>(
                    "SimConnect_GetNextDispatch");
                SubscribeToSystemEvent = ResolveRequired
                    <SimConnectDelegates.SimConnect_SubscribeToSystemEvent>(
                    "SimConnect_SubscribeToSystemEvent");

                // Optional functions (may not exist on all platforms)
                ClearDataDefinition = ResolveOptional
                    <SimConnectDelegates.SimConnect_ClearDataDefinition>(
                    "SimConnect_ClearDataDefinition");
                UnsubscribeFromSystemEvent = ResolveOptional
                    <SimConnectDelegates.SimConnect_UnsubscribeFromSystemEvent>(
                    "SimConnect_UnsubscribeFromSystemEvent");

                return true;
            }
            catch (Exception ex)
            {
                _logger?.LogError(
                    "Failed to resolve SimConnect functions: {Error}",
                    ex.Message);
                ClearDelegates();
                return false;
            }
        }

        private T ResolveRequired<T>(string functionName) where T : Delegate
        {
            var ptr = Win32Native.GetProcAddress(_hModule, functionName);
            if (ptr == IntPtr.Zero)
            {
                throw new EntryPointNotFoundException(
                    $"Required function '{functionName}' not found " +
                    $"in SimConnect.dll");
            }
            return Marshal.GetDelegateForFunctionPointer<T>(ptr);
        }

        private T? ResolveOptional<T>(string functionName) where T : Delegate
        {
            var ptr = Win32Native.GetProcAddress(_hModule, functionName);
            if (ptr == IntPtr.Zero)
            {
                _logger?.LogDebug(
                    "Optional function '{Function}' not found",
                    functionName);
                return null;
            }
            return Marshal.GetDelegateForFunctionPointer<T>(ptr);
        }

        private void ClearDelegates()
        {
            Open = null;
            Close = null;
            AddToDataDefinition = null;
            ClearDataDefinition = null;
            RequestDataOnSimObjectType = null;
            RequestDataOnSimObject = null;
            SetDataOnSimObject = null;
            MapClientEventToSimEvent = null;
            TransmitClientEvent = null;
            GetNextDispatch = null;
            SubscribeToSystemEvent = null;
            UnsubscribeFromSystemEvent = null;
        }

        // =====================================================
        // Dispose
        // =====================================================

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                Unload();
                _disposed = true;
            }
        }

        ~SimConnectDynamicLoader()
        {
            Dispose(false);
        }
    }
}