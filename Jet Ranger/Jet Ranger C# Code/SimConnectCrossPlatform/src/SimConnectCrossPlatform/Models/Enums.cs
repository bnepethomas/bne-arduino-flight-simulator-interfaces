// src/SimConnectCrossPlatform/Models/Enums.cs

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// Supported simulator platforms
    /// </summary>
    public enum SimPlatform
    {
        Unknown = 0,
        P3D = 1,
        MSFS2024 = 2
    }

    /// <summary>
    /// Connection state machine states
    /// </summary>
    public enum ConnectionState
    {
        Disconnected = 0,
        Connecting = 1,
        Connected = 2,
        Error = 3
    }

    /// <summary>
    /// SimConnect data types
    /// </summary>
    public enum SimDataType
    {
        Float32 = 0,
        Float64 = 1,
        Int32 = 2,
        Int64 = 3,
        String256 = 4
    }

    /// <summary>
    /// Data update frequency
    /// </summary>
    public enum UpdatePeriod
    {
        Once = 0,
        PerVisualFrame = 1,
        PerSimFrame = 2,
        PerSecond = 3
    }
}