// Original based on P3d sample code, modifying for MSFS2024

// Workflow for adding new variables to receive from simconnect and send to the front panel
// 1. Identify the sim variable you want to receive from the sim. Use the SDK documentation to find the variable name and units.
// 2. Add a new variable to the appropriate structure (Struct1, StructRadio, or StructFrontPanel) to
//    hold the value of the sim variable. Make sure to use the correct data type and units.
// 3. Add the sim variable to the data definition in the Setup_DataRequests() method. Make sure to add
//    it in the same order as it is defined in the structure.
// 4. Add a new variable to hold the value of the sim variable in the frmMain class. This variable will
//    be used to store the value received from simconnect and to send to the front panel.
// 5. Add code to the simconnect_OnRecvSimobjectDataBytype event handler to update the value of the new
//    variable with the value received from simconnect.
// 6. Add code to the UpdateFrontPanel() method to include the new variable in the payload sent to the
//    front panel. Make sure to format the value correctly based on the units of the sim variable.
// 7. Test the changes by running the application and verifying that the new variable is being received from
//    simconnect and sent to the front panel correctly.
// 8. If necessary, add code to handle any special cases for the new variable, such as mapping the value to a
//    specific range for a servo or handling boolean values for indicators.

// Outside of this target infrastructure needs to be added to the Arudino project to receive the new variable and
// update the corresponding servo or indicator on the front panel. This will involve adding a new case to the switch
// statement that processes incoming data from the PC and updating the code that controls the servos and indicators
// based on the value of the new variable.
// 1. Use Kicad to determine which pin on the Arduino the new variable will be connected to and connect servo. 
//      Jet Ranger DUE Prod Radio Controller for Radios 
//      Jet Ranger Front Panel Servo for Instruments and Indicators
// 2. The constant should be suffixed with _PORT eg AIRPSEED_PORT
// 3. The Variable should be been updated in the enum Servos and have an entry in the ServoMinPosition, ServoMaxPosition,
//    and ServoZeroPosition arrays if it is a servo. The master location for these arrays should be the Servo Tuner project
//    so that they can be easily updated and the values will be consistent across both the PC application and the Arduino code.
//    Note they Arduino code does not accept the native C# definitions so a little modification is needed for the arrays.
// 4. As the mapping of SimConnect to Servo position is done in the PC application, no mapping is needed in the Arduino code.
// 5. Create the Servo variable with a suffix of _SERVO.
//          Servo ENG_TORQUE_SERVO;
// 6. Create a routine which is prefix with Set eg SetEgnineTorque which receives an integer value. It checks to see if the
//    servo is attached and if not it attaches it. It then updates the last update time for the servo and sets the idle status
//    to false before writing the value to the servo.

//      void SetEngineTorque(int TargetValue) {
//           if (ENG_TORQUE_SERVO.attached() == false){
//               ENG_TORQUE_SERVO.attach(ENG_TORQUE_PORT);
//           }
//           aServoLastupdate[EngTorquePercent1] = millis();
//           aServoIdle[EngTorquePercent1] = false;

//           ENG_TORQUE_SERVO.write(TargetValue);
//         }
// 7.  Add entry to UpdateServoPos(). Which walks through all Servo to see if a movement is needed
//          if (i == EngTorquePercent1) SetEngineTorque(aServoPosition[EngTorquePercent1]);
//
// 8.  Add Entry to CheckServoIdleTime() which checks to see if the servo has been idle for a certain amount of time and if so,
//     detaches the servo to prevent it jittering and reduces power demand.
//          void CheckServoIdleTime() {
//             if (aServoIdle[EngTorquePercent1] == false)
//             {
//                //Need to see if we have hit time to detach
//                if ((millis() - aServoLastupdate[EngTorquePercent1]) >= ServoIdleTime)
//                {
//                    if (ENG_TORQUE_SERVO.attached() == true)
//                    {
//                        ENG_TORQUE_SERVO.detach();
//                    }
//                    aServoIdle[EngTorquePercent1] = true;
//                    SendDebug("Detaching Engine Torque Servo");
//                }
//            }
//
// 9.  Add Entry to HandleOutputValuePair
//            if (ParameterName == "TQ")
//            {
//                SendDebug("Received Engine Torque: " + ParameterValue);
//                aTargetServoPosition[EngTorquePercent1] = ParameterValue.toInt();
//            } 
//
// 10. Add Entry to swing new Servo in setup.
//            SetEngineTorque(aServZeroPosition[EngTorquePercent1]);
//            delay(20);
//            for (int i = 0; i <= 120; i++)
//            {
//                SetEngineTorque((map(i, 0, 120, aServMinPosition[EngTorquePercent1], aServMaxPosition[EngTorquePercent1])));
//                delay(10);
//            }
//            for (int i = 120; i >= 0; i--)
//            {
//                SetEngineTorque((map(i, 0, 120, aServMinPosition[EngTorquePercent1], aServMaxPosition[EngTorquePercent1])));
//                delay(10);
//            }
//
// 11. Compile and upload 
// 12. Update Servo Tuner, add call for the Instrument
//           private int TQ_Process(long newValue)
//           {
//               // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
//               // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
//               // So we need to map the input value (0-100) to the range of 37-176.
//               int mappedvalue = (int)Mapper(newValue, 0, 120,
//                   ServMinPosition[lstServos.SelectedIndex], ServMaxPosition[lstServos.SelectedIndex]);
//               return mappedvalue;
//           }   
// 13. Add entry in SendConverted Value
//                if (lblShortCode.Text == "TQ") valueToSend = TQ_Process(newValue); 
// 14. Check Min/Maxs and if needed introduce ranges if gauge is not linear in the instrument Specific Code     
// 15. Copy the call to SimConnect_to_IP 

// SimConnect Services
// using LockheedMartin.Prepar3D.SimConnect;
//using LockheedMartin.Prepar3D.SimConnect;

// To Use SimConnect in your project, you need to do the following:
// 1. Set the project platform to x64, as SimConnect only works with 64-bit targets. You can do this in the Configuration Manager.
// 2. Add a reference to the SimConnect assembly. You can find this in the SimConnect SDK, typically located at
//    C:\Program Files (x86)\Microsoft SDKs\Flight Simulator\SimConnect SDK\lib\x64\SimConnect.dll.
//    In Visual Studio, right-click on your project in the Solution Explorer, select "Add" > "Reference...", and then
//    browse to the "Microsoft.FlightSimulator.SimConnect.dll" file to add it as a reference.

using Microsoft.FlightSimulator.SimConnect;
using System;
using System.Diagnostics.Eventing.Reader;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Rebar;

//      Based on C# in SDK - which is also found here
//
//
//      Useful URLS
//          https://docs.flightsimulator.com/html/Programming_Tools/SimVars/Aircraft_SimVars/Aircraft_Misc_Variables.htm
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


namespace SimConnect_to_UDP

{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new frmMain());
        }
    }
}