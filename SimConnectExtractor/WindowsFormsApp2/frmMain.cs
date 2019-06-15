using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Timers;

// SimConnect Services
using LockheedMartin.Prepar3D.SimConnect;
using System.Runtime.InteropServices;



//      Based on C# in SDK - which is also found here
//
//
//      Useful URLS
//          https://www.prepar3d.com/SDKv4/sdk/simconnect_api/managed_simconnect_projects.html
//  
//          Variables
//              https://www.prepar3d.com/SDKv4/sdk/references/variables/simulation_variables.html
//          


//      Things slightly different to previous projects
//          Should specify target CPU architecture (x64) versus any.
//          When doing final build change from debug to release
//          Unsure what happens  if running on a remote computer or if SDK isn't already installed
//
// 
//          using  subscription
//          http://www.prepar3d.com/SDKv4/LearningCenter.php
//          http://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/simobject_functions.html


//          Copy name from website, duplicate, add dashs, add to the structure, add  to the request, add to the display
//          Need to note unit od measure eg radians, percent, bool.
//          Generally using Left value if there isn't a glbal value for things such as Falps and SPooiler

//          Generator and fuel pump indexes are 1's based



namespace WindowsFormsApp2
{


    public partial class frmMain : Form
    {

        // User-defined win32 event 
        const int WM_USER_SIMCONNECT = 0x0402;

        // SimConnect object 
        SimConnect simconnect = null;

        enum DEFINITIONS
        {
            Struct1,
        }

        enum DATA_REQUESTS
        {
            REQUEST_1,
        };

        string UDP_Playload;
        string Output_Payload;
        UdpClient udpClient = new UdpClient();
        UdpClient OutputClient = new UdpClient();
        DateTime TimeLastPacketSent;
        TimeSpan span;
        int mS;


        // this is how you declare a data structure so that 
        // simconnect knows how to fill it/read it. 
        // When Adding variables to receive need to add them to this datastructure as well as the request itself ininitDataRequest
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct Struct1
        {
            // this is how you declare a fixed size string 
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double latitude;
            public double longitude;
            public double altitude;
            public double airspeed;
            public double elapsedsimtime;
            public double zulu_time;
            public Int32 time_zone_offset;
            public double absolute_time;
            public double plane_heading_degrees_true;
            public double plane_heading_degrees_magnetic;
            
            public double GENERAL_ENG_MASTER_ALTERNATOR_1;
            public double GENERAL_ENG_MASTER_ALTERNATOR_2;
            public double TRAILING_EDGE_FLAPS_LEFT_ANGLE;
            public double TRAILING_EDGE_FLAPS_LEFT_PERCENT;
            public double SPOILERS_LEFT_POSITION;
            public double ELEVATOR_TRIM_PCT;
            public double BRAKE_PARKING_INDICATOR;
            public double GENERAL_ENG_FUEL_VALVE_1;
            public double GENERAL_ENG_FUEL_VALVE_2;
            public double GENERAL_ENG_STARTER_1;
            public double GENERAL_ENG_STARTER_2;
            public double GENERAL_ENG_OIL_PRESSURE_1;
            public double GENERAL_ENG_OIL_PRESSURE_2;
            public double ENG_ON_FIRE_1;
            public double ENG_ON_FIRE_2;
            public double STALL_WARNING;
            public double ELECTRICAL_MASTER_BATTERY;
            public double LIGHT_CABIN;
            public double LIGHT_STROBE;
            public double LIGHT_NAV;
            public double LIGHT_TAXI;
            public double INNER_MARKER;
            public double MIDDLE_MARKER;
            public double OUTER_MARKER;
            public double GEAR_CENTER_POSITION;
            public double GEAR_LEFT_POSITION;
            public double GEAR_RIGHT_POSITION;


            public double zulu_time_2; // Used to validate we have all data
        };


    //{ "ABSOLUTE TIME",              "Seconds",              SIMCONNECT_DATATYPE_FLOAT64},
   
    //{ "ZULU DAY OF YEAR",           "Number",               SIMCONNECT_DATATYPE_INT32       },
    //{ "ZULU YEAR",                  "Number",               SIMCONNECT_DATATYPE_INT32       },
    //{ "ZULU MONTH OF YEAR",         "Number",               SIMCONNECT_DATATYPE_INT32       },
    //{ "ZULU DAY OF MONTH",          "Number",               SIMCONNECT_DATATYPE_INT32       },
    //{ "ZULU DAY OF WEEK",           "Number",               SIMCONNECT_DATATYPE_INT32       },


        public frmMain()
        {
            InitializeComponent();


            setButtons(true, false, false);
            udpClient.Connect("172.16.1.2", 13136);
            OutputClient.Connect("172.16.1.2", 26028);
            TimeLastPacketSent = DateTime.Now;


        }
        // Simconnect client will send a win32 message when there is 
        // a packet to process. ReceiveMessage must be called to 
        // trigger the events. This model keeps simconnect processing on the main thread. 

        protected override void DefWndProc(ref Message m)
        {
            if (m.Msg == WM_USER_SIMCONNECT)
            {
                if (simconnect != null)
                {
                    simconnect.ReceiveMessage();
                }
            }
            else
            {
                base.DefWndProc(ref m);
            }
        }

        int response = 1;

        // Output text - display a maximum of X lines 
        string output = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

        private void setButtons(bool bConnect, bool bGet, bool bDisconnect)
        {
            buttonConnect.Enabled = bConnect;
            buttonRequestData.Enabled = bGet;
            buttonDisconnect.Enabled = bDisconnect;
        }

        private void closeConnection()
        {
            if (simconnect != null)
            {
                // Dispose serves the same purpose as SimConnect_Close() 
                simconnect.Dispose();
                simconnect = null;
                displayText("Connection closed");
            }
        }

        // Set up all the SimConnect related data definitions and event handlers 
        private void initDataRequest()
        {
            try
            {
                // listen to connect and quit msgs 
                simconnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(simconnect_OnRecvOpen);
                simconnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(simconnect_OnRecvQuit);

                // listen to exceptions 
                simconnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(simconnect_OnRecvException);

                // define a data structure 
                // Variable Reference https://www.prepar3d.com/SDKv4/sdk/references/variables/simulation_variables.html
                // Unless the Units column in the following table identifies the units as a structure or a string, the data will be returned by default in a signed 64 bit floating point value
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Latitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Longitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Altitude", "meters", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Airspeed True", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Sim Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Zulu Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Time Zone Offset", "hours", SIMCONNECT_DATATYPE.INT32, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Absolute Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Heading Degrees Magnetic", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG MASTER ALTERNATOR:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG MASTER ALTERNATOR:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TRAILING EDGE FLAPS LEFT ANGLE", "Radians", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "TRAILING EDGE FLAPS LEFT PERCENT", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "SPOILERS LEFT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELEVATOR TRIM PCT", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "BRAKE PARKING INDICATOR", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG FUEL VALVE:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG FUEL VALVE:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG STARTER:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED); 
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG STARTER:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG OIL PRESSURE:1", "Psf", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GENERAL ENG OIL PRESSURE:2", "Psf", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG ON FIRE:1", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ENG ON FIRE:2", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "STALL WARNING", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "ELECTRICAL MASTER BATTERY", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT CABIN", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT STROBE", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT NAV", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "LIGHT TAXI", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "INNER MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "MIDDLE MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "OUTER MARKER", "Bool", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR CENTER POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR LEFT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "GEAR RIGHT POSITION", "Percent", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);



                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Zulu Time", "seconds", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);



                // IMPORTANT: register it with the simconnect managed wrapper marshaller 
                // if you skip this step, you will only receive a uint in the .dwData field. 
                simconnect.RegisterDataDefineStruct<Struct1>(DEFINITIONS.Struct1);

                // catch a simobject data request 
                simconnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simconnect_OnRecvSimobjectDataBytype);

                simconnect.OnRecvSimobjectData += new SimConnect.RecvSimobjectDataEventHandler(simconnect_OnRecvSimobjectData);
            }
            catch (COMException ex)
            {
                displayText(ex.Message);
            }
        }

        void simconnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            displayText("Connected to Prepar3D");
        }

        // The case where the user closes Prepar3D 
        void simconnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            displayText("Prepar3D has exited");
            closeConnection();
        }

        void simconnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            displayText("Exception received: " + data.dwException);
        }

        // The case where the user closes the client 
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            closeConnection();
        }

        void simconnect_OnRecvSimobjectDataBytype(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE data)
        {

            switch ((DATA_REQUESTS)data.dwRequestID)
            {
                case DATA_REQUESTS.REQUEST_1:
                    Struct1 s1 = (Struct1)data.dwData[0];

                    richResponse.Clear();

                    displayText("title:             " + s1.title);
                    displayText("Lat:               " + s1.latitude);
                    displayText("Lon:               " + s1.longitude);
                    displayText("Alt:               " + s1.altitude);
                    displayText("Airspeed           " + s1.airspeed);
                    displayText("Sim Time           " + s1.elapsedsimtime);
                    displayText("Zulu Time          " + s1.zulu_time);
                    displayText("Time Zone Offset   " + s1.time_zone_offset);
                    displayText("Absolute Time      " + s1.absolute_time);
                    displayText("Heading True " + s1.plane_heading_degrees_true);
                    displayText("Heading Mag  " + s1.plane_heading_degrees_magnetic);
                    

                    UDP_Playload = "latitude:" + s1.latitude;
                    UDP_Playload = UDP_Playload + ",longitude:" + s1.longitude.ToString();
                    UDP_Playload = UDP_Playload + ",altitude:" + s1.altitude.ToString();
                    UDP_Playload = UDP_Playload + ",airspeed:" + s1.airspeed.ToString();
                    UDP_Playload = UDP_Playload + ",zulutime:" + s1.zulu_time.ToString();
                    UDP_Playload = UDP_Playload + ",timezoneoffset:" + s1.time_zone_offset.ToString();
                    UDP_Playload = UDP_Playload + ",trueheading:" + s1.plane_heading_degrees_true.ToString();
                    UDP_Playload = UDP_Playload + ",magheading:" + s1.plane_heading_degrees_magnetic.ToString();

 


                    break;

                default:
                    displayText("Unknown request ID: " + data.dwRequestID);
                    break;
            }
        }


        // This is the one receiving streaming data
        void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {

            

            switch ((DATA_REQUESTS)data.dwRequestID)
            {
                case DATA_REQUESTS.REQUEST_1:
                    Struct1 s1 = (Struct1)data.dwData[0];

                    displayText("titles:             " + s1.title);
                    displayText("Lat:               " + s1.latitude);
                    displayText("Lon:               " + s1.longitude);
                    displayText("Alt:               " + s1.altitude);
                    displayText("Airspeed           " + s1.airspeed);
                    displayText("Sim Time           " + s1.elapsedsimtime);
                    displayText("Zulu Time          " + s1.zulu_time);
                    displayText("Time Zone Offset   " + s1.time_zone_offset);
                    displayText("Absolute Time      " + s1.absolute_time);
                    displayText("Plane Heading True " + s1.plane_heading_degrees_true);
                    displayText("Plane Heading Mag  " + s1.plane_heading_degrees_magnetic);
                    displayText("GENERAL ENG MASTER ALTERNATOR:1  " + s1.GENERAL_ENG_MASTER_ALTERNATOR_1);
                    displayText("GENERAL ENG MASTER ALTERNATOR:2  " + s1.GENERAL_ENG_MASTER_ALTERNATOR_2);
                    displayText("TRAILING EDGE FLAPS LEFT ANGLE  " + s1.TRAILING_EDGE_FLAPS_LEFT_ANGLE);
                    displayText("TRAILING EDGE FLAPS LEFT PERCENT  " + s1.TRAILING_EDGE_FLAPS_LEFT_PERCENT);
                    displayText("SPOILERS LEFT POSITION      " + s1.SPOILERS_LEFT_POSITION);
                    displayText("ELEVATOR TRIM PCT           " + s1.ELEVATOR_TRIM_PCT);
                    displayText("BRAKE PARKING INDICATOR     " + s1.BRAKE_PARKING_INDICATOR);
                    displayText("GENERAL ENG FUEL VALVE:1    " + s1.GENERAL_ENG_FUEL_VALVE_1);
                    displayText("GENERAL ENG FUEL VALVE:2    " + s1.GENERAL_ENG_FUEL_VALVE_2);
                    displayText("GENERAL ENG STARTER:1       " + s1.GENERAL_ENG_STARTER_1);
                    displayText("GENERAL ENG STARTER:2       " + s1.GENERAL_ENG_STARTER_2);
                    displayText("GENERAL ENG OIL PRESSURE:1  " + s1.GENERAL_ENG_OIL_PRESSURE_1);
                    displayText("GENERAL ENG OIL PRESSURE:2  " + s1.GENERAL_ENG_OIL_PRESSURE_2);
                    displayText("ENG ON FIRE:2               " + s1.ENG_ON_FIRE_1);
                    displayText("ENG ON FIRE:2               " + s1.ENG_ON_FIRE_2);
                    displayText("STALL WARNING               " + s1.STALL_WARNING);
                    displayText("ELECTRICAL MASTER BATTERY   " + s1.ELECTRICAL_MASTER_BATTERY);
                    displayText("LIGHT CABIN                 " + s1.LIGHT_CABIN);
                    displayText("LIGHT STROBE                " + s1.LIGHT_STROBE);
                    displayText("LIGHT NAV                   " + s1.LIGHT_NAV);
                    displayText("LIGHT TAXI                  " + s1.LIGHT_TAXI);

                    displayText("INNER MARKER                " + s1.INNER_MARKER);
                    displayText("MIDDLE MARKER               " + s1.MIDDLE_MARKER);
                    displayText("OUTER MARKER                " + s1.OUTER_MARKER);
                    displayText("GEAR CENTRE POSITION        " + s1.GEAR_CENTER_POSITION);
                    displayText("GEAR LEFT POSITION          " + s1.GEAR_LEFT_POSITION);
                    displayText("GEAR RIGHT POSITION         " + s1.GEAR_RIGHT_POSITION);

                    displayText("Zulu Time 2        " + s1.zulu_time);

                    UDP_Playload = "latitude:" + s1.latitude;
                    UDP_Playload = UDP_Playload + ",longitude:" + s1.longitude.ToString();
                    UDP_Playload = UDP_Playload + ",altitude:" + s1.altitude.ToString();
                    UDP_Playload = UDP_Playload + ",airspeed:" + s1.airspeed.ToString();
                    UDP_Playload = UDP_Playload + ",zulutime:" + s1.zulu_time.ToString();
                    UDP_Playload = UDP_Playload + ",timezoneoffset:" + s1.time_zone_offset.ToString();
                    UDP_Playload = UDP_Playload + ",trueheading:" + s1.plane_heading_degrees_true.ToString();
                    UDP_Playload = UDP_Playload + ",magheading:" + s1.plane_heading_degrees_magnetic.ToString();

                    Output_Payload = "TRAILING_EDGE_FLAPS_LEFT_ANGLE:" + Math.Floor(s1.TRAILING_EDGE_FLAPS_LEFT_ANGLE * 100);
                    Output_Payload = "SPOILERS LEFT POSITION:" + Math.Floor(s1.SPOILERS_LEFT_POSITION * 100);
                    Output_Payload = Output_Payload + ",GEAR_CENTER_POSITION:" + Math.Floor(s1.GEAR_CENTER_POSITION/100);
                    Output_Payload = Output_Payload + ",GEAR_LEFT_POSITION:" + Math.Floor(s1.GEAR_LEFT_POSITION/100);
                    Output_Payload = Output_Payload + ",GEAR_RIGHT_POSITION:" + Math.Floor(s1.GEAR_RIGHT_POSITION/100);
                    Output_Payload = Output_Payload + ",BRAKE_PARKING_INDICATOR:" + Math.Floor(s1.BRAKE_PARKING_INDICATOR);


                    span = DateTime.Now - TimeLastPacketSent;
                    mS = (int)span.TotalMilliseconds;
                    displayText("Its been this many mS since sending last packet: " + mS.ToString());

                    if (mS >= 200)
                    { 
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        udpClient.Send(senddata, senddata.Length);

                        senddata = Encoding.ASCII.GetBytes(Output_Payload);
                        OutputClient.Send(senddata, senddata.Length);

                        TimeLastPacketSent = DateTime.Now;
                    }   
                    break;

                default:
                    displayText("Unknown request ID: " + data.dwRequestID);
                    break;
            }



        }


        private void buttonConnect_Click(object sender, EventArgs e)
        {
            if (simconnect == null)
            {
                try
                {
                    // the constructor is similar to SimConnect_Open in the native API 
                    simconnect = new SimConnect("Managed Data Request", this.Handle, WM_USER_SIMCONNECT, null, 0);

                    setButtons(false, true, true);

                    initDataRequest();

                }
                catch (COMException ex)
                {
                    displayText("Unable to connect to Prepar3D:\n\n" + ex.Message);
                }
            }
            else
            {
                displayText("Error - try again");
                closeConnection();

                setButtons(true, false, false);
            }
        }

        private void buttonDisconnect_Click(object sender, EventArgs e)
        {
            closeConnection();
            setButtons(true, false, false);
        }

        private void buttonRequestData_Click(object sender, EventArgs e)
        {
            // The following call returns identical information to: 
            // simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.ONCE); 

            simconnect.RequestDataOnSimObjectType(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, 0, SIMCONNECT_SIMOBJECT_TYPE.USER);
           // simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 0, 0);


            displayText("Request sent...");
        }



        void displayText(string s)
        {
            // remove first string from output 
            output = output.Substring(output.IndexOf("\n") + 1);

            // add the new string 
            output += "\n" + response++ + ": " + s;

            // display it 
            richResponse.Text = output;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }





        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                simconnect.RequestDataOnSimObjectType(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, 0, SIMCONNECT_SIMOBJECT_TYPE.USER);
            }
            catch
            {

                displayText("Unable to get data from SimConnect...");
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.timer1.Enabled = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {

            displayText("Pleading for data...");
            //m_scConnection.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.PERIODIQUE, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SECOND, 0, 0, 0, 0);
            //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 1, 0);

            //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.DEFAULT, 0, 0, 0);
            try
            {
                // working for a once off
                //simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.ONCE, 0, 0, 0, 0);

                //  The second to last parameter is the number of frames iterated before a data packet is sent. There isn't a parameter
                //  which maps to events per second so this will variable with frame rate of sim which could range from 15fpc to north of 100fps.
                // To assist in reducing range of pps set upper limit of Sim FPS tp 50 under Graphic Target FPS
                // Interestingly - with a target FPS of 50, seeing 100pps 

                simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 2, 0);
            }
            catch
            {
                displayText("Unhandled error in data plead ...");
            }
            

        }

        private void btn_SendUDP_Click(object sender, EventArgs e)
        {





        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void richResponse_TextChanged(object sender, EventArgs e)
        {

        }
    }


}

