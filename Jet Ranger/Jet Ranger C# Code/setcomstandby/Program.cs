using System;
using System.Runtime.InteropServices;
using Microsoft.FlightSimulator.SimConnect;

namespace SimConnectBatteryControl
{
    class Program
    {
        // User-defined enums for SimConnect tracking
        enum EVENTS
        {
            KEY_MASTER_BATTERY_SET,
        }

        enum GROUP_ID
        {
            GROUP0,
        }

        static SimConnect simconnect = null;
        const int WM_USER_SIMCONNECT = 0x0402;

        static void Main(string[] args)
        {
            try
            {
                // 1. Initialize SimConnect
                // Note: In a WinForms/WPF app, use the window handle. 
                // In a console app, this is more complex as you need a message loop.
                simconnect = new SimConnect("Battery Controller", IntPtr.Zero, WM_USER_SIMCONNECT, null, 0);

                // 2. Map the Client Event to the Simulation Event
                // MASTER_BATTERY_SET allows us to pass 1 for ON and 0 for OFF
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_MASTER_BATTERY_SET, "MASTER_BATTERY_SET");

                Console.WriteLine("Connected to Simulator. Press 'O' to turn OFF Master Battery.");

                while (true)
                {
                    var key = Console.ReadKey(true).Key;
                    if (key == ConsoleKey.O)
                    {
                        TurnOffBattery();
                    }
                    else if (key == ConsoleKey.Escape)
                    {
                        break;
                    }
                }
            }
            catch (COMException ex)
            {
                Console.WriteLine("Connection failed: " + ex.Message);
            }
        }

        static void TurnOffBattery()
        {
            if (simconnect != null)
            {
                // 3. Transmit the event
                // Parameter: 0 = Off, 1 = On
                simconnect.TransmitClientEvent(
                    SimConnect.SIMCONNECT_OBJECT_ID_USER,
                    EVENTS.KEY_MASTER_BATTERY_SET,
                    0,
                    GROUP_ID.GROUP0,
                    SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                );

                Console.WriteLine("Command Sent: Master Battery OFF");
            }
        }
    }
}