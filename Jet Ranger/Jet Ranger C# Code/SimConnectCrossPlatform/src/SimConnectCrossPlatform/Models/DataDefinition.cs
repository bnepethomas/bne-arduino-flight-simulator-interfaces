// src/SimConnectCrossPlatform/Models/DataDefinition.cs

using System.Collections.Generic;
using System.Linq;

namespace SimConnectCrossPlatform.Models
{
    /// <summary>
    /// A group of simulation variables to be requested together
    /// </summary>
    public class DataDefinition
    {
        /// <summary>
        /// Unique identifier for this definition group
        /// </summary>
        public uint Id { get; set; }

        /// <summary>
        /// The variables in this definition group
        /// </summary>
        public List<SimVarDefinition> Variables { get; set; } = new();

        /// <summary>
        /// Optional friendly name for logging/debugging
        /// </summary>
        public string? Name { get; set; }

        public DataDefinition() { }

        public DataDefinition(uint id, params SimVarDefinition[] variables)
        {
            Id = id;
            Variables = new List<SimVarDefinition>(variables);
        }

        public DataDefinition(uint id, string name,
            params SimVarDefinition[] variables)
        {
            Id = id;
            Name = name;
            Variables = new List<SimVarDefinition>(variables);
        }

        /// <summary>
        /// Calculate total byte size of all variables
        /// </summary>
        public int GetTotalByteSize() =>
            Variables.Sum(v => v.GetByteSize());

        /// <summary>
        /// Get byte offsets for each variable within the data block
        /// </summary>
        public List<int> GetFieldOffsets()
        {
            var offsets = new List<int>();
            int offset = 0;
            foreach (var v in Variables)
            {
                offsets.Add(offset);
                offset += v.GetByteSize();
            }
            return offsets;
        }

        public override string ToString() =>
            $"DataDefinition[{Id}] '{Name ?? "unnamed"}' " +
            $"({Variables.Count} vars, {GetTotalByteSize()} bytes)";
    }
}