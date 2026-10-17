using System;
using FSUIPC;

namespace DS206EventTrigger
{
    class Program
    {
        private static readonly Offset<uint> ControlNumber = new Offset<uint>(0x3110);
        private static readonly Offset<int> ControlParam = new Offset<int>(0x3114);

        private const uint GENERATOR_SWITCH_SET_ID = 32884; // confirmed via FSUIPC6.log

        static void Main(string[] args)
        {
            try
            {
                Log("Connecting to FSUIPC...");
                FSUIPCConnection.Open();
                Log($"Connected. FSUIPC version: {FSUIPCConnection.FSUIPCVersion}, " +
                    $"Sim: {FSUIPCConnection.FlightSimVersionConnected}");

                SetCustomEvent(GENERATOR_SWITCH_SET_ID, 0);
                Log("Generator switch set to ON.");

                System.Threading.Thread.Sleep(2000);

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
            Log($"--- Sending control {controlId} with param={param} ---");

            // Write param FIRST and flush it on its own — guarantees it lands
            // before the control number is written in a separate IPC call.
            ControlParam.Value = param;
            FSUIPCConnection.Process();
            Log($"  Param {param} written and flushed to offset 0x3114.");

            // Now trigger the control in its own call.
            ControlNumber.Value = controlId;
            FSUIPCConnection.Process();
            Log($"  Control {controlId} written and flushed to offset 0x3110 — should fire now.");
        }

        static void Log(string message)
        {
            Console.WriteLine($"[{DateTime.Now:HH:mm:ss.fff}] {message}");
        }
    }
}