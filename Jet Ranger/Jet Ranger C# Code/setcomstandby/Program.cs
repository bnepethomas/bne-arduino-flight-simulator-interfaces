using System;
using System.Runtime.InteropServices;
using Microsoft.FlightSimulator.SimConnect;

namespace SimConnectConsole
{
    class Program
    {
        static SimConnect simConnect;
        static IntPtr hWnd;

        const int WM_USER_SIMCONNECT = 0x0402;

        enum DEFINITIONS
        {
            Com1Standby
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Starting SimConnect console app...");

            // Create a hidden message window
            hWnd = CreateMessageWindow();

            if (hWnd == IntPtr.Zero)
            {
                Console.WriteLine("Failed to create message window.");
                return;
            }

            try
            {
                simConnect = new SimConnect(
                    "COM1 Standby Console",
                    hWnd,
                    WM_USER_SIMCONNECT,
                    null,
                    0
                );

                Console.WriteLine("SimConnect connected.");

                // Define COM1 standby frequency (Hz)
                simConnect.AddToDataDefinition(
                    DEFINITIONS.Com1Standby,
                    "COM STANDBY FREQUENCY:1",
                    "Hz",
                    SIMCONNECT_DATATYPE.FLOAT64,
                    0,
                    SimConnect.SIMCONNECT_UNUSED
                );

                simConnect.RegisterDataDefineStruct<double>(DEFINITIONS.Com1Standby);

                // 118.50 MHz = 120,500,000 Hz
                double com1StandbyHz = 124500000;

                simConnect.SetDataOnSimObject(
                    DEFINITIONS.Com1Standby,
                    SimConnect.SIMCONNECT_OBJECT_ID_USER,
                    SIMCONNECT_DATA_SET_FLAG.DEFAULT,
                    com1StandbyHz
                );

                Console.WriteLine("COM1 standby set to " + com1StandbyHz);
            }
            catch (COMException ex)
            {
                Console.WriteLine("SimConnect COM error:");
                Console.WriteLine(ex);
            }
            catch (Exception ex)
            {
                Console.WriteLine("General error:");
                Console.WriteLine(ex);
            }

            Console.WriteLine("Press ENTER to exit.");
            Console.ReadLine();

            simConnect?.Dispose();
        }

        // -----------------------------
        // Win32 message-only window
        // -----------------------------

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern IntPtr CreateWindowEx(
            int dwExStyle,
            string lpClassName,
            string lpWindowName,
            int dwStyle,
            int x,
            int y,
            int nWidth,
            int nHeight,
            IntPtr hWndParent,
            IntPtr hMenu,
            IntPtr hInstance,
            IntPtr lpParam
        );

        static IntPtr CreateMessageWindow()
        {
            return CreateWindowEx(
                0,
                "STATIC",
                "",
                0,
                0, 0, 0, 0,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero
            );
        }
    }
}
