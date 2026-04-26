// src/SimConnectCrossPlatform/Models/SimVarDefinition.cs

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// Defines a single simulation variable to be requested
    /// </summary>
    public class SimVarDefinition
    {
        /// <summary>
        /// SimConnect variable name (e.g., "PLANE ALTITUDE")
        /// </summary>
        public string Name { get; set; } = string.Empty;

        /// <summary>
        /// Units string (e.g., "feet", "knots", "degrees")
        /// </summary>
        public string Units { get; set; } = string.Empty;

        /// <summary>
        /// Data type for this variable
        /// </summary>
        public SimDataType DataType { get; set; } = SimDataType.Float64;

        /// <summary>
        /// Epsilon value for change detection (0 = report all changes)
        /// </summary>
        public float Epsilon { get; set; } = 0.0f;

        public SimVarDefinition() { }

        public SimVarDefinition(string name, string units,
            SimDataType dataType = SimDataType.Float64,
            float epsilon = 0.0f)
        {
            Name = name;
            Units = units;
            DataType = dataType;
            Epsilon = epsilon;
        }

        /// <summary>
        /// Get the byte size of this variable's data type
        /// </summary>
        public int GetByteSize() => DataType switch
        {
            SimDataType.Float32 => 4,
            SimDataType.Float64 => 8,
            SimDataType.Int32 => 4,
            SimDataType.Int64 => 8,
            SimDataType.String256 => 256,
            _ => 8
        };

        /// <summary>
        /// Get the SimConnect DATATYPE constant
        /// </summary>
        internal uint GetSimConnectDataType() => DataType switch
        {
            SimDataType.Float32 => SimConnectConstants.DATATYPE_FLOAT32,
            SimDataType.Float64 => SimConnectConstants.DATATYPE_FLOAT64,
            SimDataType.Int32 => SimConnectConstants.DATATYPE_INT32,
            SimDataType.Int64 => SimConnectConstants.DATATYPE_INT64,
            SimDataType.String256 => SimConnectConstants.DATATYPE_STRING256,
            _ => SimConnectConstants.DATATYPE_FLOAT64
        };

        public override string ToString() =>
            $"{Name} ({Units}) [{DataType}]";
    }
}