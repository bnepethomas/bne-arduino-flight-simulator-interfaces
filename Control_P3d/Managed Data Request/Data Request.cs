// Copyright (c) 2010-2018 Lockheed Martin Corporation. All rights reserved.
// Use of this file is bound by the PREPAR3D® SOFTWARE DEVELOPER KIT END USER LICENSE AGREEMENT

//
// Managed Data Request sample
//
// Click on Connect to try and connect to a running version of Prepar3D
// Click on Request Data any number of times
// Click on Disconnect to close the connection, and then you should
// be able to click on Connect and restart the process
//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

// Add these two statements to all SimConnect clients
using LockheedMartin.Prepar3D.SimConnect;
using System.Runtime.InteropServices;

namespace Managed_Data_Request
{
    public partial class Form1 : Form
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

        enum DEFINITIONS2
        {
            HDGINFO,
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

    public Form1()
        {
            InitializeComponent();

            setButtons(true, false, false);
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
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Latitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Longitude", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Plane Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

                // IMPORTANT: register it with the simconnect managed wrapper marshaller
                // if you skip this step, you will only receive a uint in the .dwData field.
                simconnect.RegisterDataDefineStruct<Struct1>(DEFINITIONS.Struct1);

                // catch a simobject data request
                simconnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simconnect_OnRecvSimobjectDataBytype);
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

                    displayText("Title: " + s1.title);
                    displayText("Lat:   " + s1.latitude);
                    displayText("Lon:   " + s1.longitude);
                    displayText("Alt:   " + s1.altitude);
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
             
            simconnect.RequestDataOnSimObjectType(DATA_REQUESTS.REQUEST_1,DEFINITIONS.Struct1, 0, SIMCONNECT_SIMOBJECT_TYPE.USER);
            displayText("Request sent...");
        }

        // Response number
        int response = 1;

        // Output text - display a maximum of 10 lines
        string output = "\n\n\n\n\n\n\n\n\n\n";

        void displayText(string s)
        {
            // remove first string from output
            output = output.Substring(output.IndexOf("\n") + 1);

            // add the new string
            output += "\n" + response++ + ": " + s;

            // display it
            richResponse.Text = output;
        }

        private void button1_Click(object sender, EventArgs e)
        {

            ////Data Definition
            //SimConnect.AddToDataDefinition(Definitions.SetComFrequency, "COM ACTIVE FREQUENCY:1", "Frequency BCD16", SIMCONNECT_DATATYPE.INT32, 0f, SimConnect.SIMCONNECT_UNUSED);
            //SimConnect.RegisterDataDefineStruct<ComFrequency>(Definitions.SetComFrequency);

            //// ComFrequency Class
            //[StructLayout(LayoutKind.Sequential, Pack = 1)]
            //public class ComFrequency
            //{
            //    public uint Frequency;
            //}

            //// Convert to BCD16
            //public static uint ToBcd16(uint n)
            //{
            //    uint result = 0;
            //    var remainder = n % 0x10;
            //    var quotient = n / 0x10;
            //    if (!(quotient == 0 && remainder == 0))
            //        result += ToBcd16(quotient) * 10 + remainder;
            //    return result;
            //}

            //// Set the frequency
            //var radioData = new ComFrequency();
            //double freq;
            //if (double.TryParse(tbCOM1Frequency.Text, out freq) && (freq >= 117.975) && (freq <= 136f))
            //{
            //    radioData.Frequency = ToBcd16((uint) (freq%100*100 + 0.5));
            //    SimConnect.SetDataOnSimObject(Definitions.SetComFrequency, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_DATA_SET_FLAG.DEFAULT,
            //    radioData);
            //}

            //The key to what you want to do is the SetDataOnSimObject function in the SDK.
            //In order to use it you will need to create a structure to hold the value you want to set and an enum that defines the id of that structure.For example:
            // define the data 



            // tell it which var you want to set
            // (the var names are in a different document in the sdk)
            simconnect.AddToDataDefinition(DEFINITIONS2.HDGINFO, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
            // register the var you want to set with SimConnect
            simconnect.RegisterDataDefineStruct<SetHdg>(DEFINITIONS2.HDGINFO);

            SetHdg
            // create a struct so that you can set the varSetHdg 

            hdgdata = new SetHdg();

            // set the var to the value you want it to be
            hdgdata.hdg = 270;
            // and finally, tell SimConnect to set the variable
            //simconnect.SetDataOnSimObject(DEFINITIONS2.HDGINFO, SIMCONNECT_SIMOBJECT_TYPE.USER, SIMCONNECT_DATA_SET_FLAG.DEFAULT, hdgdata);
            simconnect.SetDataOnSimObject(DEFINITIONS2.HDGINFO, 0, SIMCONNECT_DATA_SET_FLAG.DEFAULT, hdgdata);

        }
}
}
// End of sample