// src/SimConnectCrossPlatform/Core/SimConnectInterface.cs

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.Extensions.Logging;
using SimConnectCrossPlatform.Interfaces;
using SimConnectCrossPlatform.Loader;
using SimConnectCrossPlatform.Models;
using SimConnectCrossPlatform.Translation;

namespace SimConnectCrossPlatform.Core
{
    /// <summary>
    /// Core SimConnect interface implementation with dynamic loading
    /// and SimVar translation. Works with both MSFS2024 and P3D.
    /// </summary>
    public class SimConnectInterface : ISimInterface
    {
        private readonly SimConnectDynamicLoader _loader;
        private readonly ILogger? _logger;
        private SimVarTranslator? _translator;
        private IntPtr _hSimConnect = IntPtr.Zero;
        private bool _disposed;

        // State
        private ConnectionState _connectionState
            = ConnectionState.Disconnected;
        private readonly object _lock = new();

        // Registration tracking
        private readonly Dictionary<uint, RegisteredDefinition>
            _definitions = new();
        private readonly Dictionary<uint, uint>
            _requestToDefinition = new();
        private readonly Dictionary<string, uint>
            _eventNameToId = new();
        private uint _nextEventId = 10000;

        /// <summary>
        /// Internal tracking of a registered data definition
        /// </summary>
        private class RegisteredDefinition
        {
            public DataDefinition Definition { get; set; } = new();
            public int TotalDataSize { get; set; }
            public List<int> FieldOffsets { get; set; } = new();
            public List<int> FieldSizes { get; set; } = new();
            public List<SimVarDefinition> ActiveVariables { get; set; } = new();
        }

        // ===== Events =====
        public event EventHandler<SimDataReceivedEventArgs>? DataReceived;
        public event EventHandler<ConnectionStateChangedEventArgs>?
            ConnectionStateChanged;
        public event EventHandler<SimConnectExceptionEventArgs>?
            ExceptionReceived;

        // ===== Properties =====
        public ConnectionState ConnectionState
        {
            get => _connectionState;
            private set
            {
                var old = _connectionState;
                if (old != value)
                {
                    _connectionState = value;
                    _logger?.LogInformation(
                        "Connection state: {Old} → {New}",
                        old, value);
                    ConnectionStateChanged?.Invoke(this,
                        new ConnectionStateChangedEventArgs(
                            old, value, Platform));
                }
            }
        }

        public SimPlatform Platform => _loader.DetectedPlatform;

        public string PlatformName => Platform switch
        {
            SimPlatform.MSFS2024 =>
                "Microsoft Flight Simulator 2024",
            SimPlatform.P3D => "Prepar3D",
            _ => "Unknown"
        };

        // ===== Constructor =====

        public SimConnectInterface(ILogger? logger = null)
        {
            _logger = logger;
            _loader = new SimConnectDynamicLoader(logger);
        }

        // =====================================================
        // Connection
        // =====================================================

        public bool Connect(string appName)
        {
            lock (_lock)
            {
                if (_connectionState == ConnectionState.Connected)
                {
                    _logger?.LogWarning("Already connected");
                    return true;
                }

                ConnectionState = ConnectionState.Connecting;

                _logger?.LogInformation(
                    "Loading SimConnect dynamically...");

                if (!_loader.Load())
                {
                    _logger?.LogError(
                        "Failed to load SimConnect.dll from any source");
                    ConnectionState = ConnectionState.Error;
                    return false;
                }

                _logger?.LogInformation(
                    "SimConnect loaded for {Platform} from {Path}",
                    _loader.DetectedPlatform, _loader.LoadedDllPath);

                // Initialize translator
                _translator = new SimVarTranslator(
                    _loader.DetectedPlatform, _logger);

                // Open connection
                var hr = _loader.Open!(
                    out _hSimConnect,
                    appName,
                    IntPtr.Zero, 0, IntPtr.Zero, 0);

                if (hr != 0)
                {
                    _logger?.LogError(
                        "SimConnect_Open failed with HRESULT: 0x{HR:X8}",
                        hr);
                    _loader.Unload();
                    ConnectionState = ConnectionState.Error;
                    return false;
                }

                _logger?.LogInformation(
                    "Connected to {Platform}", PlatformName);
                ConnectionState = ConnectionState.Connected;
                return true;
            }
        }

        public void Disconnect()
        {
            lock (_lock)
            {
                if (_hSimConnect != IntPtr.Zero)
                {
                    try
                    {
                        _loader.Close?.Invoke(_hSimConnect);
                    }
                    catch (Exception ex)
                    {
                        _logger?.LogWarning(
                            "Error during SimConnect_Close: {Error}",
                            ex.Message);
                    }
                    _hSimConnect = IntPtr.Zero;
                }

                _loader.Unload();
                _definitions.Clear();
                _requestToDefinition.Clear();
                _eventNameToId.Clear();

                ConnectionState = ConnectionState.Disconnected;
            }
        }

        // =====================================================
        // Data Definitions
        // =====================================================

        public bool RegisterDataDefinition(DataDefinition definition)
        {
            lock (_lock)
            {
                if (_hSimConnect == IntPtr.Zero)
                {
                    _logger?.LogError(
                        "Cannot register definition — not connected");
                    return false;
                }

                // Clear existing definition if re-registering
                if (_definitions.ContainsKey(definition.Id))
                {
                    _loader.ClearDataDefinition?.Invoke(
                        _hSimConnect, definition.Id);
                    _definitions.Remove(definition.Id);
                }

                var regDef = new RegisteredDefinition
                {
                    Definition = definition
                };

                foreach (var simVar in definition.Variables)
                {
                    // Translate for current platform
                    var translated = _translator!.Translate(
                        simVar.Name, simVar.Units);

                    if (!translated.IsSupported)
                    {
                        _logger?.LogWarning(
                            "SimVar '{Name}' not supported on " +
                            "{Platform}: {Notes}",
                            simVar.Name, PlatformName,
                            translated.Notes);
                        continue;
                    }

                    int fieldSize = simVar.GetByteSize();
                    regDef.FieldOffsets.Add(regDef.TotalDataSize);
                    regDef.FieldSizes.Add(fieldSize);
                    regDef.TotalDataSize += fieldSize;
                    regDef.ActiveVariables.Add(simVar);

                    var hr = _loader.AddToDataDefinition!(
                        _hSimConnect,
                        definition.Id,
                        translated.Name,
                        translated.Units,
                        simVar.GetSimConnectDataType(),
                        simVar.Epsilon,
                        SimConnectConstants.UNUSED
                    );

                    if (hr != 0)
                    {
                        _logger?.LogError(
                            "AddToDataDefinition failed for " +
                            "'{Name}': HRESULT 0x{HR:X8}",
                            simVar.Name, hr);
                        return false;
                    }

                    _logger?.LogDebug(
                        "Registered: {Name} ({Units}) [{Type}]",
                        translated.Name, translated.Units,
                        simVar.DataType);
                }

                _definitions[definition.Id] = regDef;

                _logger?.LogInformation(
                    "Registered definition {Id} with {Count} variables " +
                    "({Bytes} bytes)",
                    definition.Id, regDef.ActiveVariables.Count,
                    regDef.TotalDataSize);

                return true;
            }
        }

        public bool UnregisterDataDefinition(uint definitionId)
        {
            lock (_lock)
            {
                if (_hSimConnect != IntPtr.Zero &&
                    _loader.ClearDataDefinition != null)
                {
                    _loader.ClearDataDefinition(
                        _hSimConnect, definitionId);
                }

                _definitions.Remove(definitionId);

                // Remove associated requests
                var toRemove = new List<uint>();
                foreach (var kvp in _requestToDefinition)
                {
                    if (kvp.Value == definitionId)
                        toRemove.Add(kvp.Key);
                }
                foreach (var key in toRemove)
                {
                    _requestToDefinition.Remove(key);
                }

                return true;
            }
        }

        // =====================================================
        // Data Requests
        // =====================================================

        public bool RequestData(uint requestId, uint definitionId,
            UpdatePeriod period = UpdatePeriod.Once)
        {
            lock (_lock)
            {
                if (_hSimConnect == IntPtr.Zero) return false;

                _requestToDefinition[requestId] = definitionId;

                int hr;
                if (period == UpdatePeriod.Once)
                {
                    hr = _loader.RequestDataOnSimObjectType!(
                        _hSimConnect,
                        requestId,
                        definitionId,
                        0, // radius meters
                        SimConnectConstants.SIMOBJECT_TYPE_USER
                    );
                }
                else
                {
                    uint scPeriod = period switch
                    {
                        UpdatePeriod.PerVisualFrame =>
                            SimConnectConstants.PERIOD_VISUAL_FRAME,
                        UpdatePeriod.PerSimFrame =>
                            SimConnectConstants.PERIOD_SIM_FRAME,
                        UpdatePeriod.PerSecond =>
                            SimConnectConstants.PERIOD_SECOND,
                        _ => SimConnectConstants.PERIOD_ONCE
                    };

                    hr = _loader.RequestDataOnSimObject!(
                        _hSimConnect,
                        requestId,
                        definitionId,
                        SimConnectConstants.OBJECT_ID_USER,
                        scPeriod,
                        0, 0, 0, 0
                    );
                }

                if (hr != 0)
                {
                    _logger?.LogError(
                        "RequestData failed: HRESULT 0x{HR:X8}", hr);
                    return false;
                }

                _logger?.LogDebug(
                    "Requested data: req={ReqId} def={DefId} " +
                    "period={Period}",
                    requestId, definitionId, period);

                return true;
            }
        }

        public bool SetData(uint definitionId,
            IReadOnlyDictionary<string, SimValue> values)
        {
            lock (_lock)
            {
                if (_hSimConnect == IntPtr.Zero) return false;

                if (!_definitions.TryGetValue(
                    definitionId, out var regDef))
                {
                    _logger?.LogError(
                        "Definition {Id} not registered", definitionId);
                    return false;
                }

                // Marshal data into unmanaged buffer
                IntPtr buffer = IntPtr.Zero;
                try
                {
                    buffer = Marshal.AllocHGlobal(regDef.TotalDataSize);

                    for (int i = 0;
                         i < regDef.ActiveVariables.Count; i++)
                    {
                        var varDef = regDef.ActiveVariables[i];
                        IntPtr fieldPtr = IntPtr.Add(buffer,
                            regDef.FieldOffsets[i]);

                        if (values.TryGetValue(
                            varDef.Name, out var value))
                        {
                            WriteValue(fieldPtr, varDef.DataType, value);
                        }
                        else
                        {
                            // Zero-fill missing values
                            for (int b = 0;
                                 b < regDef.FieldSizes[i]; b++)
                            {
                                Marshal.WriteByte(
                                    fieldPtr, b, 0);
                            }
                        }
                    }

                    var hr = _loader.SetDataOnSimObject!(
                        _hSimConnect,
                        definitionId,
                        SimConnectConstants.OBJECT_ID_USER,
                        0, // flags
                        0, // array count (0 = single)
                        (uint)regDef.TotalDataSize,
                        buffer
                    );

                    if (hr != 0)
                    {
                        _logger?.LogError(
                            "SetData failed: HRESULT 0x{HR:X8}", hr);
                        return false;
                    }

                    return true;
                }
                finally
                {
                    if (buffer != IntPtr.Zero)
                        Marshal.FreeHGlobal(buffer);
                }
            }
        }

        private static void WriteValue(IntPtr ptr,
            SimDataType type, SimValue value)
        {
            switch (type)
            {
                case SimDataType.Float32:
                {
                    var bytes = BitConverter.GetBytes(value.AsFloat());
                    Marshal.Copy(bytes, 0, ptr, 4);
                    break;
                }
                case SimDataType.Float64:
                {
                    var bytes = BitConverter.GetBytes(value.AsDouble());
                    Marshal.Copy(bytes, 0, ptr, 8);
                    break;
                }
                case SimDataType.Int32:
                    Marshal.WriteInt32(ptr, value.AsInt());
                    break;
                case SimDataType.Int64:
                    Marshal.WriteInt64(ptr, value.AsLong());
                    break;
                case SimDataType.String256:
                {
                    var str = value.AsString();
                    var bytes = System.Text.Encoding.ASCII.GetBytes(str);
                    var len = System.Math.Min(bytes.Length, 255);
                    Marshal.Copy(bytes, 0, ptr, len);
                    Marshal.WriteByte(ptr, len, 0); // null terminate
                    break;
                }
            }
        }

        // =====================================================
        // Events
        // =====================================================

        public bool SendEvent(string eventName, uint data = 0)
        {
            lock (_lock)
            {
                if (_hSimConnect == IntPtr.Zero) return false;

                // Get or create event ID
                if (!_eventNameToId.TryGetValue(
                    eventName, out uint eventId))
                {
                    eventId = _nextEventId++;
                    _eventNameToId[eventName] = eventId;

                    // Map it
                    var mapHr = _loader.MapClientEventToSimEvent!(
                        _hSimConnect, eventId, eventName);
                    if (mapHr != 0)
                    {
                        _logger?.LogError(
                            "MapClientEventToSimEvent failed for " +
                            "'{Event}': HRESULT 0x{HR:X8}",
                            eventName, mapHr);
                        _eventNameToId.Remove(eventName);
                        return false;
                    }
                }

                // Transmit
                var hr = _loader.TransmitClientEvent!(
                    _hSimConnect,
                    SimConnectConstants.OBJECT_ID_USER,
                    eventId,
                    data,
                    SimConnectConstants.GROUP_PRIORITY_HIGHEST,
                    SimConnectConstants.EVENT_FLAG_GROUPID_IS_PRIORITY
                );

                if (hr != 0)
                {
                    _logger?.LogError(
                        "TransmitClientEvent failed for '{Event}': " +
                        "HRESULT 0x{HR:X8}",
                        eventName, hr);
                    return false;
                }

                return true;
            }
        }

        // =====================================================
        // Message Processing
        // =====================================================

        public void Process()
        {
            lock (_lock)
            {
                if (_hSimConnect == IntPtr.Zero) return;

                int maxMessages = 100; // prevent infinite loop
                int processed = 0;

                while (processed < maxMessages)
                {
                    var hr = _loader.GetNextDispatch!(
                        _hSimConnect,
                        out IntPtr pData,
                        out uint cbData);

                    if (hr != 0) break; // No more messages

                    try
                    {
                        HandleDispatch(pData, cbData);
                    }
                    catch (Exception ex)
                    {
                        _logger?.LogError(
                            "Error handling dispatch: {Error}",
                            ex.Message);
                    }

                    processed++;
                }
            }
        }

        private void HandleDispatch(IntPtr pData, uint cbData)
        {
            uint dwID = (uint)Marshal.ReadInt32(pData,
                SimConnectConstants.RECV_OFFSET_DWID);

            switch (dwID)
            {
                case SimConnectConstants.RECV_ID_SIMOBJECT_DATA:
                case SimConnectConstants.RECV_ID_SIMOBJECT_DATA_BYTYPE:
                    HandleSimObjectData(pData);
                    break;

                case SimConnectConstants.RECV_ID_EXCEPTION:
                    HandleException(pData);
                    break;

                case SimConnectConstants.RECV_ID_QUIT:
                    _logger?.LogInformation(
                        "Simulator quit message received");
                    ConnectionState = ConnectionState.Disconnected;
                    break;

                case SimConnectConstants.RECV_ID_OPEN:
                    _logger?.LogInformation(
                        "SimConnect OPEN message received");
                    break;

                default:
                    _logger?.LogDebug(
                        "Unhandled dispatch ID: 0x{ID:X8}", dwID);
                    break;
            }
        }

        private void HandleSimObjectData(IntPtr pData)
        {
            uint requestId = (uint)Marshal.ReadInt32(pData,
                SimConnectConstants.DATA_OFFSET_REQUEST_ID);

            if (!_requestToDefinition.TryGetValue(
                requestId, out uint defId))
            {
                _logger?.LogDebug(
                    "Unknown request ID: {Id}", requestId);
                return;
            }

            if (!_definitions.TryGetValue(defId, out var regDef))
            {
                _logger?.LogDebug(
                    "Unknown definition ID: {Id}", defId);
                return;
            }

            IntPtr dataPtr = IntPtr.Add(pData,
                SimConnectConstants.DATA_OFFSET_DATA_START);
            var values = new Dictionary<string, SimValue>();

            for (int i = 0;
                 i < regDef.ActiveVariables.Count
                 && i < regDef.FieldOffsets.Count;
                 i++)
            {
                var varDef = regDef.ActiveVariables[i];
                IntPtr fieldPtr = IntPtr.Add(dataPtr,
                    regDef.FieldOffsets[i]);

                try
                {
                    values[varDef.Name] = ReadValue(
                        fieldPtr, varDef.DataType);
                }
                catch (Exception ex)
                {
                    _logger?.LogError(
                        "Error reading '{Name}': {Error}",
                        varDef.Name, ex.Message);
                    values[varDef.Name] = SimValue.Default;
                }
            }

            DataReceived?.Invoke(this,
                new SimDataReceivedEventArgs(requestId, values));
        }

        private static SimValue ReadValue(IntPtr ptr, SimDataType type)
        {
            return type switch
            {
                SimDataType.Float32 => SimValue.FromFloat(
                    BitConverter.ToSingle(ReadBytes(ptr, 4), 0)),
                SimDataType.Float64 => SimValue.FromDouble(
                    BitConverter.ToDouble(ReadBytes(ptr, 8), 0)),
                SimDataType.Int32 => SimValue.FromInt(
                    Marshal.ReadInt32(ptr)),
                SimDataType.Int64 => SimValue.FromLong(
                    Marshal.ReadInt64(ptr)),
                SimDataType.String256 => SimValue.FromString(
                    Marshal.PtrToStringAnsi(ptr) ?? string.Empty),
                _ => SimValue.Default
            };
        }

        private static byte[] ReadBytes(IntPtr ptr, int count)
        {
            var bytes = new byte[count];
            Marshal.Copy(ptr, bytes, 0, count);
            return bytes;
        }

        private void HandleException(IntPtr pData)
        {
            uint exception = (uint)Marshal.ReadInt32(pData,
                SimConnectConstants.EXCEPTION_OFFSET_EXCEPTION);
            uint sendId = (uint)Marshal.ReadInt32(pData,
                SimConnectConstants.EXCEPTION_OFFSET_SENDID);
            uint index = (uint)Marshal.ReadInt32(pData,
                SimConnectConstants.EXCEPTION_OFFSET_INDEX);

            var args = new SimConnectExceptionEventArgs(
                exception, sendId, index);

            _logger?.LogWarning(
                "SimConnect exception: {Message} " +
                "(SendID={SendId}, Index={Index})",
                args.Message, sendId, index);

            ExceptionReceived?.Invoke(this, args);
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
                if (disposing)
                {
                    Disconnect();
                    _loader.Dispose();
                }
                _disposed = true;
            }
        }
    }
}