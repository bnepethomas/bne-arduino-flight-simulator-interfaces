using System;
using System.Runtime.InteropServices;
using Microsoft.FlightSimulator.SimConnect;

namespace SimConnectFlightControl
{
    class Program
    {
        // User-defined enums for SimConnect tracking
        enum EVENTS
        {
            KEY_MASTER_BATTERY_SET,
            KEY_COM_STBY_RADIO_SET
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
                simconnect = new SimConnect("Flight Controller", IntPtr.Zero, WM_USER_SIMCONNECT, null, 0);

                // 2. Map the Client Events
                // Battery Event
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_MASTER_BATTERY_SET, "MASTER_BATTERY_SET");

                // COM1 Standby Event
                simconnect.MapClientEventToSimEvent(EVENTS.KEY_COM_STBY_RADIO_SET, "COM_STBY_RADIO_SET");

                Console.WriteLine("Connected. Press 'B' to turn OFF Battery, 'C' to set COM1 Standby to 121.50.");

                while (true)
                {
                    var key = Console.ReadKey(true).Key;
                    if (key == ConsoleKey.B)
                    {
                        TurnOffBattery();
                    }
                    else if (key == ConsoleKey.C)
                    {
                        SetCom1Standby();
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
                // Parameter 0 = Off
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

        static void SetCom1Standby()
        {
            if (simconnect != null)
            {
                // Frequency 121.50 MHz in BCD is 0x2150
                // We use uint to ensure the hex value is passed correctly
                uint frequencyBCD = 0x2150;

                simconnect.TransmitClientEvent(
                    SimConnect.SIMCONNECT_OBJECT_ID_USER,
                    EVENTS.KEY_COM_STBY_RADIO_SET,
                    frequencyBCD,
                    GROUP_ID.GROUP0,
                    SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY
                );
                Console.WriteLine("Command Sent: COM1 Standby set to 121.50 MHz");
            }
        }
    }
}