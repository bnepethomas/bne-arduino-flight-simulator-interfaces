using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;

using System.Threading;

using System.Net;
using System.Net.Sockets;

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
            COM1_RADIO_HZ_SET,
            SPOILERS_ON,
            SPOILERS_OFF,
            FLAPS_INCR, 
            FLAPS_DECR,
            BRAKES,
            PARKING_BRAKES, 
            ELEV_TRIM_DN,
            ELEV_TRIM_UP,
            AVIONICS_MASTER_SET,
            TOGGLE_MASTER_BATTERY,
            GENALT_BUS1_CONTACT_SET, 
            GENALT_BUS2_CONTACT_SET, 
            TOGGLE_STARTER1, 
            TOGGLE_STARTER2, 
            TOGGLE_FUEL_VALVE_ENG1, 
            TOGGLE_FUEL_VALVE_ENG2, 
            LANDING_LIGHTS_ON, 
            LANDING_LIGHTS_OFF, 
            TOGGLE_CABIN_LIGHTS,
            TOGGLE_TAXI_LIGHTS,
            STROBES_TOGGLE,
            TOGGLE_NAV_LIGHTS, 
            TOGGLE_AIRCRAFT_EXIT,
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


        public static UdpClient receivingUdpClient = new UdpClient(9902);
        public static IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
        public static string returnData;


        static void Main(string[] args)
        {
            // SimConnect object
            SimConnect simconnect = null;
            SimConnect simulation_connection = new SimConnect("Managed Change Vehicle", IntPtr.Zero, 0,
                                                   null, 0);

            string myString;

            try
            {



                if (simconnect == null)
                {


                    Console.WriteLine("Connecting to P3d a second time");
                    try
                    {
                        // the constructor is similar to SimConnect_Open in the native API
                        // Event References http://www.prepar3d.com/SDKv3/LearningCenter/utilities/variables/event_ids.html
                        // Use the second column String_Name as parameter
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
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.COM1_RADIO_HZ_SET, "COM1_RADIO_HZ_SET");


                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.SPOILERS_ON, "SPOILERS_ON");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.SPOILERS_OFF, "SPOILERS_OFF");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.FLAPS_INCR, "FLAPS_INCR");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.FLAPS_DECR, "FLAPS_DECR");
                        // BRAKES emujlates a spring loaded brake and decays over time
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.BRAKES, "BRAKES");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.PARKING_BRAKES, "PARKING_BRAKES");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.ELEV_TRIM_DN, "ELEV_TRIM_DN");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.ELEV_TRIM_UP, "ELEV_TRIM_UP");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.AVIONICS_MASTER_SET, "AVIONICS_MASTER_SET");
                        // TOGGLE_MASTER_BATTERY doesn't seem to have a set value
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_MASTER_BATTERY, "TOGGLE_MASTER_BATTERY");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.GENALT_BUS1_CONTACT_SET, "GENALT_BUS1_CONTACT_SET");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.GENALT_BUS2_CONTACT_SET, "GENALT_BUS2_CONTACT_SET");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_STARTER1, "TOGGLE_STARTER1");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_STARTER2, "TOGGLE_STARTER2");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_FUEL_VALVE_ENG1, "TOGGLE_FUEL_VALVE_ENG1");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_FUEL_VALVE_ENG2, "TOGGLE_FUEL_VALVE_ENG2");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.LANDING_LIGHTS_ON, "LANDING_LIGHTS_ON");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.LANDING_LIGHTS_OFF, "LANDING_LIGHTS_OFF");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_TAXI_LIGHTS, "TOGGLE_TAXI_LIGHTS");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_CABIN_LIGHTS, "TOGGLE_CABIN_LIGHTS");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.STROBES_TOGGLE, "STROBES_TOGGLE");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_NAV_LIGHTS, "TOGGLE_NAV_LIGHTS");
                        simconnect.MapClientEventToSimEvent(PAUSE_EVENTS.TOGGLE_AIRCRAFT_EXIT, "TOGGLE_AIRCRAFT_EXIT");




                    }
                    catch (COMException ex)
                    {
                        Console.WriteLine("Unable to connect to Prepar3D");
                    }

                    Console.WriteLine("Starting control test");
                    try
                    {
                        //simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.TOGGLE_GEAR, 1, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                        simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.NAV1_RADIO_SET, 0x11230, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                        simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.COM1_RADIO_HZ_SET, 121600000, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);

                        //simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.THROTTLE_SET, 16000, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);


                        while (true)
                        {
                            //Creates a UdpClient for reading incoming data.


                            //Creates an IPEndPoint to record the IP Address and port number of the sender. 
                            // The IPEndPoint will allow you to read datagrams sent from any source.


                            Console.WriteLine("Waiting for new UdpClient Data");
                            myString = Receive();
                            if (myString != null)
                            {
                                myString = myString.Trim();
                                Console.WriteLine("Main code return is " + myString);
                                if (myString == "Full")
                                {
                                    Console.WriteLine("Going to Full Throttle");
                                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.THROTTLE_SET, 16000, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                                }

                                switch (myString)
                                {
                                    case ("PARKING_BRAKES"):
                                        simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, PAUSE_EVENTS.PARKING_BRAKES, 1, GROUP.ID_PRIORITY_STANDARD, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                                        break;
                                    default:
                                        Console.WriteLine("Unable to find matching command in data " + myString);
                                        break;
                                }


/*
                                PAUSE = 0,
                                UNPAUSE,
                                SET_ALT,
                                AUTOPILOT_ON,
                                GEAR_UP,
                                GEAR_DOWN,
                                TOGGLE_GEAR,
                                THROTTLE_SET,
                                NAV1_RADIO_SET,
                                COM1_RADIO_HZ_SET,
                                SPOILERS_ON,
                                SPOILERS_OFF,
                                FLAPS_INCR, 
                                FLAPS_DECR,
                                BRAKES,
                                PARKING_BRAKES, 
                                ELEV_TRIM_DN,
                                ELEV_TRIM_UP,
                                AVIONICS_MASTER_SET,
                                TOGGLE_MASTER_BATTERY,
                                GENALT_BUS1_CONTACT_SET, 
                                GENALT_BUS2_CONTACT_SET, 
                                TOGGLE_STARTER1, 
                                TOGGLE_STARTER2, 
                                TOGGLE_FUEL_VALVE_ENG1, 
                                TOGGLE_FUEL_VALVE_ENG2, 
                                LANDING_LIGHTS_ON, 
                                LANDING_LIGHTS_OFF, 
                                TOGGLE_CABIN_LIGHTS,
                                TOGGLE_TAXI_LIGHTS,
                                STROBES_TOGGLE,
                                TOGGLE_NAV_LIGHTS, 
                                TOGGLE_AIRCRAFT_EXIT,
*/
                            }


                            Console.WriteLine("receive parameters from receive code");

                        }
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

            while (true)
            {
                //Creates a UdpClient for reading incoming data.


                //Creates an IPEndPoint to record the IP Address and port number of the sender. 
                // The IPEndPoint will allow you to read datagrams sent from any source.


                Console.WriteLine("Waiting for UdpClient Data");
                myString = Receive();
                if (myString != null)
                {
                    Console.WriteLine("Main code return is " + myString);
                }


                Console.WriteLine("receive parameters from receive code");

            }

            Console.WriteLine("Exiting - Press a key to continue");
            Console.ReadKey();

        }

        static string Receive()
        {


            try
            {

                // Blocks until a message returns on this socket from a remote host.
                Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                returnData = Encoding.ASCII.GetString(receiveBytes);
                Console.WriteLine(DateTime.Now);
                Console.WriteLine("This is the message you received " +
                                            returnData.ToString());
                Console.WriteLine("This message was sent from " +
                                            RemoteIpEndPoint.Address.ToString() +
                                            " on their port number " +
                                            RemoteIpEndPoint.Port.ToString());

                Console.WriteLine("Trim off leading and trailing spaces");
                Console.WriteLine("Break into segments, comma separated");
                Console.WriteLine("Do a look up for first paramter");
                Console.WriteLine("If second paraetmer doesn't exist or isn't a number, set it to 1");
                Console.WriteLine("Pass these parameters to P3d");
                return (returnData.ToString());

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                return (null);
            }
        }
    }
}