using System;
using FSUIPC; // Paul Henty's FSUIPCClient.dll (NuGet: FSUIPCClient)

namespace DS206EventTrigger
{
    class Program
    {
        // Offsets FSUIPC uses to accept an FS control / custom event trigger
        private static readonly Offset<uint> ControlNumber = new Offset<uint>(0x3110);
        private static readonly Offset<int> ControlParam = new Offset<int>(0x3114);

        // Confirmed via FSUIPC6.log: "FS Control Sent: Ctrl=32888 :DS206.GENERATOR_SWITCH_SET"
        private const uint GENERATOR_SWITCH_SET_ID = 32888; // 0x00008078

        static void Main(string[] args)
        {
            try
            {
                Log("Connecting to FSUIPC...");
                FSUIPCConnection.Open();
                Log($"Connected. FSUIPC version: {FSUIPCConnection.FSUIPCVersion}, " +
                    $"Sim: {FSUIPCConnection.FlightSimVersionConnected}");

                // Param 0 = off (per your file's comment: "param value 0 or 1")
                SetCustomEvent(GENERATOR_SWITCH_SET_ID, 1);

                Log("Generator switch set to OFF.");
            }
            catch (FSUIPCException ex)
            {
                Log($"FSUIPC error: {ex.Message}");
            }
            finally
            {
                FSUIPCConnection.Close();
                Log("Connection closed.");
            }
        }

        static void SetCustomEvent(uint controlId, int param)
        {
            Log($"--- Sending control {controlId} (0x{controlId:X4}) with param={param} ---");

            ControlParam.Value = param;
            ControlNumber.Value = controlId;

            FSUIPCConnection.Process();
            Log("  Process() returned successfully — write reached FSUIPC.");
        }

        static void Log(string message)
        {
            Console.WriteLine($"[{DateTime.Now:HH:mm:ss.fff}] {message}");
        }
    }
}