// src/SimConnectCrossPlatform/Interfaces/ISimInterface.cs

using System;
using System.Collections.Generic;
using SimConnectCrossPlatform.Models;

namespace SimConnectCrossPlatform.Interfaces
{
    /// <summary>
    /// Platform-agnostic interface for simulator communication
    /// </summary>
    public interface ISimInterface : IDisposable
    {
        // ===== Connection =====

        /// <summary>Connect to the simulator</summary>
        bool Connect(string appName);

        /// <summary>Disconnect from the simulator</summary>
        void Disconnect();

        /// <summary>Current connection state</summary>
        ConnectionState ConnectionState { get; }

        /// <summary>Detected simulator platform</summary>
        SimPlatform Platform { get; }

        /// <summary>Human-readable platform name</summary>
        string PlatformName { get; }

        // ===== Data Definitions =====

        /// <summary>Register a data definition group</summary>
        bool RegisterDataDefinition(DataDefinition definition);

        /// <summary>Unregister a data definition group</summary>
        bool UnregisterDataDefinition(uint definitionId);

        // ===== Data Requests =====

        /// <summary>Request data on the user's aircraft</summary>
        bool RequestData(uint requestId, uint definitionId,
            UpdatePeriod period = UpdatePeriod.Once);

        /// <summary>Set data on the user's aircraft</summary>
        bool SetData(uint definitionId,
            IReadOnlyDictionary<string, SimValue> values);

        // ===== Events =====

        /// <summary>Send a simulator event</summary>
        bool SendEvent(string eventName, uint data = 0);

        // ===== Processing =====

        /// <summary>Process pending SimConnect messages</summary>
        void Process();

        // ===== .NET Events =====

        /// <summary>Raised when simulation data is received</summary>
        event EventHandler<SimDataReceivedEventArgs>? DataReceived;

        /// <summary>Raised when connection state changes</summary>
        event EventHandler<ConnectionStateChangedEventArgs>?
            ConnectionStateChanged;

        /// <summary>Raised when a SimConnect exception occurs</summary>
        event EventHandler<SimConnectExceptionEventArgs>?
            ExceptionReceived;
    }
}