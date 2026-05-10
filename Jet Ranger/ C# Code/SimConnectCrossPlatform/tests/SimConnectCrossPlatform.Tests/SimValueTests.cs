// tests/SimConnectCrossPlatform.Tests/SimValueTests.cs

using System;
using SimConnectCrossPlatform.Models;
using Xunit;

namespace SimConnectCrossPlatform.Tests
{
    public class SimValueTests
    {
        [Fact]
        public void FromDouble_StoresAndRetrieves()
        {
            var value = SimValue.FromDouble(12345.6789);
            Assert.Equal(SimDataType.Float64, value.Type);
            Assert.True(value.HasValue);
            Assert.Equal(12345.6789, value.AsDouble(), 4);
        }

        [Fact]
        public void FromFloat_StoresAndRetrieves()
        {
            var value = SimValue.FromFloat(42.5f);
            Assert.Equal(SimDataType.Float32, value.Type);
            Assert.Equal(42.5f, value.AsFloat());
        }

        [Fact]
        public void FromInt_StoresAndRetrieves()
        {
            var value = SimValue.FromInt(999);
            Assert.Equal(SimDataType.Int32, value.Type);
            Assert.Equal(999, value.AsInt());
        }

        [Fact]
        public void FromLong_StoresAndRetrieves()
        {
            var value = SimValue.FromLong(9876543210L);
            Assert.Equal(SimDataType.Int64, value.Type);
            Assert.Equal(9876543210L, value.AsLong());
        }
    }
}

        