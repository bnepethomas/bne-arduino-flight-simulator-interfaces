// File: tests/SimConnectCrossPlatform.Tests/SimValueTests.cs

using System;
using SimConnectCrossPlatform.Models;
using Xunit;

namespace SimConnectCrossPlatform.Tests
{
    public class SimValueTests
    {
        // =====================================================
        // Factory Method Tests
        // =====================================================

        [Fact]
        public void FromDouble_StoresCorrectly()
        {
            var v = SimValue.FromDouble(12345.6789);

            Assert.Equal(SimDataType.Float64, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal(12345.6789, v.AsDouble(), 4);
        }

        [Fact]
        public void FromDouble_Zero()
        {
            var v = SimValue.FromDouble(0.0);

            Assert.Equal(SimDataType.Float64, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal(0.0, v.AsDouble());
        }

        [Fact]
        public void FromDouble_Negative()
        {
            var v = SimValue.FromDouble(-500.25);

            Assert.Equal(-500.25, v.AsDouble(), 2);
        }

        [Fact]
        public void FromDouble_VeryLarge()
        {
            var v = SimValue.FromDouble(99999999.99);

            Assert.Equal(99999999.99, v.AsDouble(), 2);
        }

        [Fact]
        public void FromFloat_StoresCorrectly()
        {
            var v = SimValue.FromFloat(42.5f);

            Assert.Equal(SimDataType.Float32, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal(42.5f, v.AsFloat());
        }

        [Fact]
        public void FromFloat_Zero()
        {
            var v = SimValue.FromFloat(0.0f);

            Assert.Equal(0.0f, v.AsFloat());
        }

        [Fact]
        public void FromFloat_Negative()
        {
            var v = SimValue.FromFloat(-99.9f);

            Assert.Equal(-99.9f, v.AsFloat(), 1);
        }

        [Fact]
        public void FromInt_StoresCorrectly()
        {
            var v = SimValue.FromInt(999);

            Assert.Equal(SimDataType.Int32, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal(999, v.AsInt());
        }

        [Fact]
        public void FromInt_Zero()
        {
            var v = SimValue.FromInt(0);

            Assert.Equal(0, v.AsInt());
        }

        [Fact]
        public void FromInt_Negative()
        {
            var v = SimValue.FromInt(-42);

            Assert.Equal(-42, v.AsInt());
        }

        [Fact]
        public void FromInt_MaxValue()
        {
            var v = SimValue.FromInt(int.MaxValue);

            Assert.Equal(int.MaxValue, v.AsInt());
        }

        [Fact]
        public void FromInt_MinValue()
        {
            var v = SimValue.FromInt(int.MinValue);

            Assert.Equal(int.MinValue, v.AsInt());
        }

        [Fact]
        public void FromLong_StoresCorrectly()
        {
            var v = SimValue.FromLong(9876543210L);

            Assert.Equal(SimDataType.Int64, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal(9876543210L, v.AsLong());
        }

        [Fact]
        public void FromLong_Zero()
        {
            var v = SimValue.FromLong(0L);

            Assert.Equal(0L, v.AsLong());
        }

        [Fact]
        public void FromLong_Negative()
        {
            var v = SimValue.FromLong(-9876543210L);

            Assert.Equal(-9876543210L, v.AsLong());
        }

        [Fact]
        public void FromString_StoresCorrectly()
        {
            var v = SimValue.FromString("Hello World");

            Assert.Equal(SimDataType.String256, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal("Hello World", v.AsString());
        }

        [Fact]
        public void FromString_Empty()
        {
            var v = SimValue.FromString("");

            Assert.Equal(SimDataType.String256, v.Type);
            Assert.True(v.HasValue);
            Assert.Equal("", v.AsString());
        }

        [Fact]
        public void FromString_Null_BecomesEmpty()
        {
            var v = SimValue.FromString(null!);

            Assert.Equal(SimDataType.String256, v.Type);
            Assert.Equal("", v.AsString());
        }

        [Fact]
        public void FromString_LongString()
        {
            var longStr = new string('A', 300);
            var v = SimValue.FromString(longStr);

            Assert.Equal(longStr, v.AsString());
        }

        // =====================================================
        // Default Value Tests
        // =====================================================

        [Fact]
        public void Default_HasValue()
        {
            var v = SimValue.Default;

            Assert.True(v.HasValue);
            Assert.Equal(SimDataType.Float64, v.Type);
            Assert.Equal(0.0, v.AsDouble());
        }

        [Fact]
        public void Default_AsInt_ReturnsZero()
        {
            var v = SimValue.Default;

            Assert.Equal(0, v.AsInt());
        }

        [Fact]
        public void Default_AsString_ReturnsZeroString()
        {
            var v = SimValue.Default;

            Assert.Equal("0", v.AsString());
        }

        // =====================================================
        // Cross-Type Conversion Tests
        // =====================================================

        [Fact]
        public void DoubleToFloat_Converts()
        {
            var v = SimValue.FromDouble(42.5);

            Assert.Equal(42.5f, v.AsFloat(), 1);
        }

        [Fact]
        public void DoubleToInt_Truncates()
        {
            var v = SimValue.FromDouble(42.9);

            Assert.Equal(42, v.AsInt());
        }

        [Fact]
        public void IntToDouble_Converts()
        {
            var v = SimValue.FromInt(42);

            Assert.Equal(42.0, v.AsDouble());
        }

        [Fact]
        public void IntToFloat_Converts()
        {
            var v = SimValue.FromInt(42);

            Assert.Equal(42.0f, v.AsFloat());
        }

        [Fact]
        public void IntToLong_Converts()
        {
            var v = SimValue.FromInt(42);

            Assert.Equal(42L, v.AsLong());
        }

        [Fact]
        public void LongToDouble_Converts()
        {
            var v = SimValue.FromLong(42L);

            Assert.Equal(42.0, v.AsDouble());
        }

        [Fact]
        public void FloatToDouble_Converts()
        {
            var v = SimValue.FromFloat(42.5f);

            Assert.Equal(42.5, v.AsDouble(), 1);
        }

        [Fact]
        public void IntToString_Converts()
        {
            var v = SimValue.FromInt(42);

            Assert.Equal("42", v.AsString());
        }

        [Fact]
        public void DoubleToString_Converts()
        {
            var v = SimValue.FromDouble(42.5);
            var s = v.AsString();

            Assert.Contains("42", s);
        }

        // =====================================================
        // TryGet Tests
        // =====================================================

        [Fact]
        public void TryGetDouble_FromDouble_Succeeds()
        {
            var v = SimValue.FromDouble(42.5);

            Assert.True(v.TryGetDouble(out double result));
            Assert.Equal(42.5, result);
        }

        [Fact]
        public void TryGetDouble_FromInt_Succeeds()
        {
            var v = SimValue.FromInt(42);

            Assert.True(v.TryGetDouble(out double result));
            Assert.Equal(42.0, result);
        }

        [Fact]
        public void TryGetDouble_FromFloat_Succeeds()
        {
            var v = SimValue.FromFloat(42.5f);

            Assert.True(v.TryGetDouble(out double result));
            Assert.Equal(42.5, result, 1);
        }

        [Fact]
        public void TryGetDouble_FromLong_Succeeds()
        {
            var v = SimValue.FromLong(42L);

            Assert.True(v.TryGetDouble(out double result));
            Assert.Equal(42.0, result);
        }

        [Fact]
        public void TryGetDouble_FromNonNumericString_Fails()
        {
            var v = SimValue.FromString("hello");

            Assert.False(v.TryGetDouble(out double result));
            Assert.Equal(0, result);
        }

        [Fact]
        public void TryGetDouble_FromNumericString_Succeeds()
        {
            var v = SimValue.FromString("42.5");

            Assert.True(v.TryGetDouble(out double result));
            Assert.Equal(42.5, result);
        }

        [Fact]
        public void TryGetInt_FromInt_Succeeds()
        {
            var v = SimValue.FromInt(42);

            Assert.True(v.TryGetInt(out int result));
            Assert.Equal(42, result);
        }

        [Fact]
        public void TryGetInt_FromDouble_Succeeds()
        {
            var v = SimValue.FromDouble(42.0);

            Assert.True(v.TryGetInt(out int result));
            Assert.Equal(42, result);
        }

        [Fact]
        public void TryGetInt_FromNonNumericString_Fails()
        {
            var v = SimValue.FromString("hello");

            Assert.False(v.TryGetInt(out int result));
            Assert.Equal(0, result);
        }

        [Fact]
        public void TryGetInt_FromNumericString_Succeeds()
        {
            var v = SimValue.FromString("42");

            Assert.True(v.TryGetInt(out int result));
            Assert.Equal(42, result);
        }

        // =====================================================
        // Implicit Conversion Tests
        // =====================================================

        [Fact]
        public void ImplicitConversion_Double()
        {
            SimValue v = 123.456;

            Assert.Equal(SimDataType.Float64, v.Type);
            Assert.Equal(123.456, v.AsDouble(), 3);
        }

        [Fact]
        public void ImplicitConversion_Float()
        {
            SimValue v = 42.5f;

            Assert.Equal(SimDataType.Float32, v.Type);
            Assert.Equal(42.5f, v.AsFloat());
        }

        [Fact]
        public void ImplicitConversion_Int()
        {
            SimValue v = 42;

            Assert.Equal(SimDataType.Int32, v.Type);
            Assert.Equal(42, v.AsInt());
        }

        [Fact]
        public void ImplicitConversion_Long()
        {
            SimValue v = 9876543210L;

            Assert.Equal(SimDataType.Int64, v.Type);
            Assert.Equal(9876543210L, v.AsLong());
        }

        [Fact]
        public void ImplicitConversion_String()
        {
            SimValue v = "test string";

            Assert.Equal(SimDataType.String256, v.Type);
            Assert.Equal("test string", v.AsString());
        }

        // =====================================================
        // Equality Tests
        // =====================================================

        [Fact]
        public void Equality_SameDoubleValues_AreEqual()
        {
            var a = SimValue.FromDouble(1.0);
            var b = SimValue.FromDouble(1.0);

            Assert.Equal(a, b);
            Assert.True(a == b);
            Assert.False(a != b);
        }

        [Fact]
        public void Equality_DifferentDoubleValues_AreNotEqual()
        {
            var a = SimValue.FromDouble(1.0);
            var b = SimValue.FromDouble(2.0);

            Assert.NotEqual(a, b);
            Assert.True(a != b);
            Assert.False(a == b);
        }

        [Fact]
        public void Equality_SameIntValues_AreEqual()
        {
            var a = SimValue.FromInt(42);
            var b = SimValue.FromInt(42);

            Assert.Equal(a, b);
            Assert.True(a == b);
        }

        [Fact]
        public void Equality_DifferentIntValues_AreNotEqual()
        {
            var a = SimValue.FromInt(42);
            var b = SimValue.FromInt(99);

            Assert.NotEqual(a, b);
            Assert.True(a != b);
        }

        [Fact]
        public void Equality_SameStringValues_AreEqual()
        {
            var a = SimValue.FromString("hello");
            var b = SimValue.FromString("hello");

            Assert.Equal(a, b);
            Assert.True(a == b);
        }

        [Fact]
        public void Equality_DifferentStringValues_AreNotEqual()
        {
            var a = SimValue.FromString("hello");
            var b = SimValue.FromString("world");

            Assert.NotEqual(a, b);
            Assert.True(a != b);
        }

        [Fact]
        public void Equality_DifferentTypes_SameNumericValue_NotEqual()
        {
            // Even though both represent 42, they have different types
            var a = SimValue.FromInt(42);
            var b = SimValue.FromDouble(42.0);

            // These should NOT be equal because the types differ
            Assert.NotEqual(a, b);
        }

        [Fact]
        public void Equality_SameLongValues_AreEqual()
        {
            var a = SimValue.FromLong(9876543210L);
            var b = SimValue.FromLong(9876543210L);

            Assert.Equal(a, b);
        }

        [Fact]
        public void Equality_SameFloatValues_AreEqual()
        {
            var a = SimValue.FromFloat(42.5f);
            var b = SimValue.FromFloat(42.5f);

            Assert.Equal(a, b);
        }

        // =====================================================
        // GetHashCode Tests
        // =====================================================

        [Fact]
        public void GetHashCode_SameValues_SameHash()
        {
            var a = SimValue.FromDouble(1.0);
            var b = SimValue.FromDouble(1.0);

            Assert.Equal(a.GetHashCode(), b.GetHashCode());
        }

        [Fact]
        public void GetHashCode_DifferentValues_DifferentHash()
        {
            var a = SimValue.FromDouble(1.0);
            var b = SimValue.FromDouble(2.0);

            // Not guaranteed but extremely likely
            Assert.NotEqual(a.GetHashCode(), b.GetHashCode());
        }

        [Fact]
        public void GetHashCode_DifferentTypes_DifferentHash()
        {
            var a = SimValue.FromInt(42);
            var b = SimValue.FromDouble(42.0);

            Assert.NotEqual(a.GetHashCode(), b.GetHashCode());
        }

        // =====================================================
        // Equals(object) Tests
        // =====================================================

        [Fact]
        public void EqualsObject_SameValue_True()
        {
            var a = SimValue.FromDouble(1.0);
            object b = SimValue.FromDouble(1.0);

            Assert.True(a.Equals(b));
        }

        [Fact]
        public void EqualsObject_DifferentValue_False()
        {
            var a = SimValue.FromDouble(1.0);
            object b = SimValue.FromDouble(2.0);

            Assert.False(a.Equals(b));
        }

        [Fact]
        public void EqualsObject_Null_False()
        {
            var a = SimValue.FromDouble(1.0);

            Assert.False(a.Equals(null));
        }

        [Fact]
        public void EqualsObject_DifferentType_False()
        {
            var a = SimValue.FromDouble(1.0);

            Assert.False(a.Equals("not a SimValue"));
        }

        // =====================================================
        // ToString Tests
        // =====================================================

        [Fact]
        public void ToString_Int_FormatsCorrectly()
        {
            var v = SimValue.FromInt(42);

            Assert.Equal("42", v.ToString());
        }

        [Fact]
        public void ToString_Double_ContainsDecimal()
        {
            var v = SimValue.FromDouble(123.4567);
            var s = v.ToString();

            Assert.Contains("123.4567", s);
        }

        [Fact]
        public void ToString_Float_ContainsValue()
        {
            var v = SimValue.FromFloat(42.50f);
            var s = v.ToString();

            Assert.Contains("42.50", s);
        }

        [Fact]
        public void ToString_String_ReturnsString()
        {
            var v = SimValue.FromString("hello");

            Assert.Equal("hello", v.ToString());
        }

        [Fact]
        public void ToString_Long_FormatsCorrectly()
        {
            var v = SimValue.FromLong(9876543210L);

            Assert.Equal("9876543210", v.ToString());
        }

        [Fact]
        public void ToString_NegativeInt_IncludesMinus()
        {
            var v = SimValue.FromInt(-42);

            Assert.Equal("-42", v.ToString());
        }

        [Fact]
        public void ToString_NegativeDouble_IncludesMinus()
        {
            var v = SimValue.FromDouble(-1.5);
            var s = v.ToString();

            Assert.StartsWith("-", s);
            Assert.Contains("1.5", s);
        }

        [Fact]
        public void ToString_Zero_ReturnsZero()
        {
            var v = SimValue.FromInt(0);

            Assert.Equal("0", v.ToString());
        }

        // =====================================================
        // Flight Sim Specific Scenario Tests
        // =====================================================

        [Fact]
        public void Scenario_Altitude_TypicalValue()
        {
            // Typical cruise altitude
            var v = SimValue.FromDouble(35000.0);

            Assert.Equal(35000.0, v.AsDouble());
            Assert.Equal(35000, v.AsInt());
        }

        [Fact]
        public void Scenario_Heading_Wrap()
        {
            // Heading at 359 degrees
            var v = SimValue.FromDouble(359.5);

            Assert.Equal(359.5, v.AsDouble(), 1);
            Assert.Equal(359, v.AsInt());
        }

        [Fact]
        public void Scenario_VerticalSpeed_Negative()
        {
            // Descending at 1500 fpm
            var v = SimValue.FromDouble(-1500.0);

            Assert.Equal(-1500.0, v.AsDouble());
            Assert.True(v.AsDouble() < 0);
        }

        [Fact]
        public void Scenario_Latitude_HighPrecision()
        {
            // Brisbane, Australia latitude
            var v = SimValue.FromDouble(-27.469771);

            Assert.Equal(-27.469771, v.AsDouble(), 6);
        }

        [Fact]
        public void Scenario_Longitude_HighPrecision()
        {
            // Brisbane, Australia longitude
            var v = SimValue.FromDouble(153.025124);

            Assert.Equal(153.025124, v.AsDouble(), 6);
        }

        [Fact]
        public void Scenario_AutopilotMaster_Boolean()
        {
            // AP ON = 1, OFF = 0
            var apOn = SimValue.FromInt(1);
            var apOff = SimValue.FromInt(0);

            Assert.Equal(1, apOn.AsInt());
            Assert.Equal(0, apOff.AsInt());
            Assert.NotEqual(apOn, apOff);
        }

        [Fact]
        public void Scenario_RPM_LargeValue()
        {
            // Typical piston engine RPM
            var v = SimValue.FromDouble(2450.0);

            Assert.Equal(2450.0, v.AsDouble());
            Assert.Equal(2450, v.AsInt());
        }

        [Fact]
        public void Scenario_FuelQuantity_Fractional()
        {
            // Fuel weight in pounds
            var v = SimValue.FromDouble(1234.56);

            Assert.Equal(1234.56, v.AsDouble(), 2);
        }

        [Fact]
        public void Scenario_ThrottlePercent_Range()
        {
            var idle = SimValue.FromDouble(0.0);
            var full = SimValue.FromDouble(100.0);
            var mid = SimValue.FromDouble(50.0);

            Assert.Equal(0.0, idle.AsDouble());
            Assert.Equal(100.0, full.AsDouble());
            Assert.Equal(50.0, mid.AsDouble());
        }

        [Fact]
        public void Scenario_AircraftTitle_String()
        {
            var v = SimValue.FromString(
                "Cessna Skyhawk G1000 Asobo");

            Assert.Equal(
                "Cessna Skyhawk G1000 Asobo", v.AsString());
        }

        // =====================================================
        // Edge Cases
        // =====================================================

        [Fact]
        public void EdgeCase_DoubleMaxValue()
        {
            var v = SimValue.FromDouble(double.MaxValue);

            Assert.Equal(double.MaxValue, v.AsDouble());
        }

        [Fact]
        public void EdgeCase_DoubleMinValue()
        {
            var v = SimValue.FromDouble(double.MinValue);

            Assert.Equal(double.MinValue, v.AsDouble());
        }

        [Fact]
        public void EdgeCase_DoubleNaN()
        {
            var v = SimValue.FromDouble(double.NaN);

            Assert.True(double.IsNaN(v.AsDouble()));
        }

        [Fact]
        public void EdgeCase_DoublePositiveInfinity()
        {
            var v = SimValue.FromDouble(double.PositiveInfinity);

            Assert.True(
                double.IsPositiveInfinity(v.AsDouble()));
        }

        [Fact]
        public void EdgeCase_DoubleNegativeInfinity()
        {
            var v = SimValue.FromDouble(double.NegativeInfinity);

            Assert.True(
                double.IsNegativeInfinity(v.AsDouble()));
        }

        [Fact]
        public void EdgeCase_FloatEpsilon()
        {
            var v = SimValue.FromFloat(float.Epsilon);

            Assert.Equal(float.Epsilon, v.AsFloat());
        }

        [Fact]
        public void EdgeCase_StringWithSpecialChars()
        {
            var v = SimValue.FromString(
                "Test\twith\nnewlines & <special> chars");

            Assert.Contains("newlines", v.AsString());
            Assert.Contains("<special>", v.AsString());
        }

        [Fact]
        public void EdgeCase_StringWithUnicode()
        {
            var v = SimValue.FromString("Ünïcödé ✈️");

            Assert.Equal("Ünïcödé ✈️", v.AsString());
        }

        // =====================================================
        // Collection / Dictionary Usage Tests
        // =====================================================

        [Fact]
        public void CanBeUsedAsDictionaryValue()
        {
            var dict = new System.Collections.Generic.Dictionary
                <string, SimValue>
            {
                ["PLANE ALTITUDE"] = SimValue.FromDouble(35000),
                ["AIRSPEED INDICATED"] = SimValue.FromDouble(250),
                ["HEADING"] = SimValue.FromDouble(270),
                ["AP MASTER"] = SimValue.FromInt(1),
                ["TITLE"] = SimValue.FromString("Boeing 747")
            };

            Assert.Equal(5, dict.Count);
            Assert.Equal(35000, dict["PLANE ALTITUDE"].AsDouble());
            Assert.Equal(250, dict["AIRSPEED INDICATED"].AsDouble());
            Assert.Equal(270, dict["HEADING"].AsDouble());
            Assert.Equal(1, dict["AP MASTER"].AsInt());
            Assert.Equal("Boeing 747", dict["TITLE"].AsString());
        }

        [Fact]
        public void CanBeUsedInList()
        {
            var list = new System.Collections.Generic.List<SimValue>
            {
                SimValue.FromDouble(1.0),
                SimValue.FromInt(2),
                SimValue.FromString("three"),
                SimValue.FromFloat(4.0f),
                SimValue.FromLong(5L)
            };

            Assert.Equal(5, list.Count);
            Assert.Equal(SimDataType.Float64, list[0].Type);
            Assert.Equal(SimDataType.Int32, list[1].Type);
            Assert.Equal(SimDataType.String256, list[2].Type);
            Assert.Equal(SimDataType.Float32, list[3].Type);
            Assert.Equal(SimDataType.Int64, list[4].Type);
        }

        [Fact]
        public void CanBeUsedAsHashSetElement()
        {
            var set = new System.Collections.Generic.HashSet<SimValue>
            {
                SimValue.FromDouble(1.0),
                SimValue.FromDouble(1.0),  // Duplicate
                SimValue.FromDouble(2.0),
                SimValue.FromInt(1)        // Different type
            };

            // Set should have 3 unique elements
            // (1.0 double, 2.0 double, 1 int)
            Assert.Equal(3, set.Count);
        }
    }
}