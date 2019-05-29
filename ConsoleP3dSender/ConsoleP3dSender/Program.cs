using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;

using System.Threading;

using LockheedMartin.Prepar3D.SimConnect;
using System.Runtime.InteropServices;

namespace ManagedChangeVehicle
{
    class ManagedChangeVehicle
    {

        enum DEFINITIONS
        {
            Struct1,
        }

        enum DATA_REQUESTS
        {
            REQUEST_1,
        };


        enum GROUP
        {
            ID_PRIORITY_STANDARD = 1900000000,
        };

        enum PAUSE_EVENTS
        {
            PAUSE = 0,
            UNPAUSE,
            SET_ALT,
            AUTOPILOT_ON,
            GEAR_UP,
            GEAR_DOWN,
            TOGGLE_GEAR,
            THROTTLE_SET,
            NAV1_RADIO_SET,
        };


        enum GROUPID
        {
            FLAG = 2000000000,
        };



        // this is how you declare a data structure so that
        // simconnect knows how to fill it/read it.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct Struct1
        {
            // this is how you declare a fixed size string
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double latitude;
            public double longitude;
            public double altitude;
        };



        // define the structure for the var you want to set
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct SetHdg
        {
            public double hdg;
        };

        static void Main(string[] args)
        {
            // SimConnect object
            SimConnect simconnect = null;
            SimConnect simulation_connection = new SimConnect("Managed Change Vehicle", IntPtr.Zero, 0,
                                                   null, 0);


            try
            {



                if (simconnect == null)
                {


                    Console.WriteLine("Connecting to P3d a second time");
                    try
                    {
                        // the constructor is similar to SimConnect_Open in the native API
                        // Event References http://www.prepar3d.com/SDKv3/LearningCenter/utilities/variables/event_ids.html
                        simconnect = new SimConnect("Managed Data Request", IntPtr.Zero, 0, null, 0);
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.PAUSE, "PAUSE_ON");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.UNPAUSE, "PAUSE_OFF");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.AUTOPILOT_ON, "AUTOPILOT_ON");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.SET_ALT, "AP_ALT_VAR_SET_METRIC");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.GEAR_UP, "GEAR_UP");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.GEAR_DOWN, "GEAR_DOWN");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_GEAR, "GEAR_TOGGLE");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.THROTTLE_SET, "THROTTLE_SET");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.NAV1_RADIO_SET, "NAV1_RADIO_SET");


                    }
                    catch (COMException ex)
                    {
                        Console.WriteLine("Unable to connect to Prepar3D");
                    }

                    Console.WriteLine("Starting control test");
                    try
                    {
                        //simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.TOGGLE_GEAR, 1, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                        simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.NAV1_RADIO_SET, 0x11130, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                        simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.THROTTLE_SET, 16000, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                    }
                    catch (COMException ex)
                    {
                        Console.WriteLine("Unable to execute control test");
                    }
                    Console.WriteLine("Ended control test");
                }
                else
                {
                    Console.WriteLine("Error - try again");

                }

                // Sending the title of the Vehicle 
                //simulation_connection.ChangeVehicle("Mooney Bravo Retro");

                simulation_connection.Dispose();
                simulation_connection = null;
                Console.WriteLine("Vechicle Changed");


            }
            catch (Exception ex)
            {
                // We were unable to connect so let the user know why. 
                System.Console.WriteLine("Sim Connect unable to connect to Prepar3D!\n\n{0}\n\n{1}",
                                         ex.Message, ex.StackTrace);
            }



            // Clean up SomConnect Objects
            if (simulation_connection != null)
            {
                simulation_connection.Dispose();
                simulation_connection = null;
            }

            if (simconnect != null)
            {
                simconnect.Dispose();
                simconnect = null;
            }
            


            Console.WriteLine("Exiting - Press a key to continue");
            Console.ReadKey();

        }
    }
}