// src/SimConnectCrossPlatform/Models/SimValue.cs

using System;

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// Represents a simulation variable value with type information.
    /// Immutable value type for thread safety.
    /// </summary>
    public readonly struct SimValue : IEquatable<SimValue>
    {
        private readonly object? _value;

        /// <summary>
        /// The data type of this value
        /// </summary>
        public SimDataType Type { get; }

        /// <summary>
        /// Whether this value has been set
        /// </summary>
        public bool HasValue => _value != null;

        private SimValue(object value, SimDataType type)
        {
            _value = value ?? throw new ArgumentNullException(nameof(value));
            Type = type;
        }

        // ===== Factory Methods =====

        /// <summary>Create from a 32-bit float</summary>
        public static SimValue FromFloat(float value) =>
            new(value, SimDataType.Float32);

        /// <summary>Create from a 64-bit double</summary>
        public static SimValue FromDouble(double value) =>
            new(value, SimDataType.Float64);

        /// <summary>Create from a 32-bit integer</summary>
        public static SimValue FromInt(int value) =>
            new(value, SimDataType.Int32);

        /// <summary>Create from a 64-bit integer</summary>
        public static SimValue FromLong(long value) =>
            new(value, SimDataType.Int64);

        /// <summary>Create from a string</summary>
        public static SimValue FromString(string value) =>
            new(value ?? string.Empty, SimDataType.String256);

        /// <summary>Create a default/empty value</summary>
        public static SimValue Default => new(0.0, SimDataType.Float64);

        // ===== Type-safe Accessors =====

        /// <summary>Get as float (with conversion)</summary>
        public float AsFloat() => Convert.ToSingle(_value);

        /// <summary>Get as double (with conversion)</summary>
        public double AsDouble() => Convert.ToDouble(_value);

        /// <summary>Get as int (with conversion)</summary>
        public int AsInt() => Convert.ToInt32(_value);

        /// <summary>Get as long (with conversion)</summary>
        public long AsLong() => Convert.ToInt64(_value);

        /// <summary>Get as string</summary>
        public string AsString() => _value?.ToString() ?? string.Empty;

        // ===== Try-Get Accessors =====

        /// <summary>Try to get as double without exceptions</summary>
        public bool TryGetDouble(out double result)
        {
            try
            {
                result = Convert.ToDouble(_value);
                return true;
            }
            catch
            {
                result = 0;
                return false;
            }
        }

        /// <summary>Try to get as int without exceptions</summary>
        public bool TryGetInt(out int result)
        {
            try
            {
                result = Convert.ToInt32(_value);
                return true;
            }
            catch
            {
                result = 0;
                return false;
            }
        }

        // ===== Equality & ToString =====

        public override string ToString()
        {
            if (_value == null) return "null";

            return Type switch
            {
                SimDataType.Float32 => $"{AsFloat():F2}",
                SimDataType.Float64 => $"{AsDouble():F4}",
                SimDataType.Int32 => $"{AsInt()}",
                SimDataType.Int64 => $"{AsLong()}",
                SimDataType.String256 => AsString(),
                _ => _value.ToString() ?? "null"
            };
        }

        public override bool Equals(object? obj) =>
            obj is SimValue other && Equals(other);

        public bool Equals(SimValue other) =>
            Type == other.Type &&
            Equals(_value, other._value);

        public override int GetHashCode() =>
            HashCode.Combine(Type, _value);

        public static bool operator ==(SimValue left, SimValue right) =>
            left.Equals(right);

        public static bool operator !=(SimValue left, SimValue right) =>
            !left.Equals(right);

        // ===== Implicit Conversions =====

        public static implicit operator SimValue(double value) =>
            FromDouble(value);
        public static implicit operator SimValue(float value) =>
            FromFloat(value);
        public static implicit operator SimValue(int value) =>
            FromInt(value);
        public static implicit operator SimValue(long value) =>
            FromLong(value);
        public static implicit operator SimValue(string value) =>
            FromString(value);
    }
}