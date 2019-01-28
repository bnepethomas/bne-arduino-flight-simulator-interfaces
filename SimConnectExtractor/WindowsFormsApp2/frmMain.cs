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
//      Need to add Sockets interface to throw data to Pi
// Currently not using the most efficent way to get data - can simply create a subscription
//          http://www.prepar3d.com/SDKv4/LearningCenter.php
//          http://www.prepar3d.com/SDKv4/sdk/simconnect_api/references/simobject_functions.html

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
        UdpClient udpClient = new UdpClient();
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
            udpClient.Connect("192.168.1.135", 13136);
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
        string output = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

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


        void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {

            displayText("Received something don't know what to do with it but trying");

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

                    UDP_Playload = "latitude:" + s1.latitude;
                    UDP_Playload = UDP_Playload + ",longitude:" + s1.longitude.ToString();
                    UDP_Playload = UDP_Playload + ",altitude:" + s1.altitude.ToString();
                    UDP_Playload = UDP_Playload + ",airspeed:" + s1.airspeed.ToString();
                    UDP_Playload = UDP_Playload + ",zulutime:" + s1.zulu_time.ToString();
                    UDP_Playload = UDP_Playload + ",timezoneoffset:" + s1.time_zone_offset.ToString();
                    UDP_Playload = UDP_Playload + ",trueheading:" + s1.plane_heading_degrees_true.ToString();
                    UDP_Playload = UDP_Playload + ",magheading:" + s1.plane_heading_degrees_magnetic.ToString();



                    span = DateTime.Now - TimeLastPacketSent;
                    mS = (int)span.TotalMilliseconds;
                    displayText("Its been this many mS since sending last packet: " + mS.ToString());

                    if (mS >= 500)
                    { 
                        Byte[] senddata = Encoding.ASCII.GetBytes(UDP_Playload);
                        udpClient.Send(senddata, senddata.Length);

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
    }


}

