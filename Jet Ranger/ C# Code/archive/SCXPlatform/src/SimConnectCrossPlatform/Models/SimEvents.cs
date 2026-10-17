// src/SimConnectCrossPlatform/Models/SimEvents.cs

using System;
using System.Collections.Generic;

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// Event args for simulation data updates
    /// </summary>
    public class SimDataReceivedEventArgs : EventArgs
    {
        /// <summary>Request ID that triggered this data</summary>
        public uint RequestId { get; }

        /// <summary>Map of variable name → value</summary>
        public IReadOnlyDictionary<string, SimValue> Data { get; }

        /// <summary>Timestamp when data was received</summary>
        public DateTime Timestamp { get; }

        public SimDataReceivedEventArgs(
            uint requestId,
            IReadOnlyDictionary<string, SimValue> data)
        {
            RequestId = requestId;
            Data = data;
            Timestamp = DateTime.UtcNow;
        }

        /// <summary>
        /// Get a specific value by variable name, or default
        /// </summary>
        public SimValue GetValue(string variableName) =>
            Data.TryGetValue(variableName, out var value)
                ? value : SimValue.Default;
    }

    /// <summary>
    /// Event args for connection state transitions
    /// </summary>
    public class ConnectionStateChangedEventArgs : EventArgs
    {
        public ConnectionState OldState { get; }
        public ConnectionState NewState { get; }
        public SimPlatform Platform { get; }
        public DateTime Timestamp { get; }

        public ConnectionStateChangedEventArgs(
            ConnectionState oldState,
            ConnectionState newState,
            SimPlatform platform)
        {
            OldState = oldState;
            NewState = newState;
            Platform = platform;
            Timestamp = DateTime.UtcNow;
        }
    }

    /// <summary>
    /// Event args for reconnection attempts
    /// </summary>
    public class ReconnectionAttemptEventArgs : EventArgs
    {
        public int AttemptNumber { get; }
        public TimeSpan NextRetryDelay { get; }
        public DateTime Timestamp { get; }

        public ReconnectionAttemptEventArgs(
            int attemptNumber, TimeSpan nextRetryDelay)
        {
            AttemptNumber = attemptNumber;
            NextRetryDelay = nextRetryDelay;
            Timestamp = DateTime.UtcNow;
        }
    }

    /// <summary>
    /// Event args for SimConnect exceptions
    /// </summary>
    public class SimConnectExceptionEventArgs : EventArgs
    {
        public uint ExceptionCode { get; }
        public uint SendId { get; }
        public uint Index { get; }
        public string Message { get; }

        public SimConnectExceptionEventArgs(
            uint exceptionCode, uint sendId, uint index)
        {
            ExceptionCode = exceptionCode;
            SendId = sendId;
            Index = index;
            Message = GetExceptionMessage(exceptionCode);
        }

        private static string GetExceptionMessage(uint code) => code switch
        {
            0 => "None",
            1 => "Error",
            2 => "Size mismatch",
            3 => "Unrecognized ID",
            4 => "Unopened",
            5 => "Version mismatch",
            6 => "Too many groups",
            7 => "Name unrecognized",
            8 => "Too many event names",
            9 => "Event ID duplicate",
            10 => "Too many maps",
            11 => "Too many objects",
            12 => "Too many requests",
            13 => "Weather invalid airport",
            14 => "Weather invalid metar",
            15 => "Weather unable to get observation",
            16 => "Weather unable to create station",
            17 => "Weather unable to remove station",
            18 => "Invalid data type",
            19 => "Invalid data size",
            20 => "Data error",
            21 => "Invalid array",
            22 => "Create object failed",
            23 => "Load flight plan failed",
            24 => "Operation invalid for object type",
            25 => "Illegal operation",
            26 => "Already subscribed",
            27 => "Invalid enum",
            28 => "Definition error",
            29 => "Duplicate ID",
            30 => "Datum ID",
            31 => "Out of bounds",
            32 => "Already created",
            33 => "Object outside reality bubble",
            34 => "Object container",
            35 => "Object AI",
            36 => "Object ATC",
            37 => "Object schedule",
            _ => $"Unknown exception ({code})"
        };
    }
}