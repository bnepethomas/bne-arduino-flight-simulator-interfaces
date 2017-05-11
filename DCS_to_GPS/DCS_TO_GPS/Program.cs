using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO.Ports;

namespace DCS_TO_GPS
{
    


    class Program
    {
        
        static public string GPSComPort;
        static public ProgramMode OperationalMode;
        
        static public double gLat = 0;     // N/S to five decimal places
        static public double gLong = 0;    // E/W to five decimal places

        static public string xoutputstr;
        static public string youtputstr;

        static public String outNorS;
        static public String outEorW;

        static public string outSpeed = "100";
        static public string outAltitude = "1000";
        static public string outLatitude = "44.89";
        static public string outLongitude = "35.39";
        static public string outTrackMadeGood = "000";
        static public string outMagVar = "00.0";

        static public double Lomac_GPS_Altitude = 100;
        static public double Lomac_GPS_Heading = 90;
        static public double Lomac_GPS_Latitude = -27.3873;
        static public double Lomac_GPS_Longitude = 153.1301;
        static public double Lomac_GPS_Speed = 333;


        public enum ProgramMode
        {
            DemoMode,
            F4Mode,
            LomacMode
        };
        
        static void Main(string[] args)
        {
            DCS_TO_GPSOut_init();
        }

        static void DCS_TO_GPSOut_init()
        {
            foreach (string arg in Environment.GetCommandLineArgs())
            {
                if (arg.IndexOf("Falcon") != -1)
                {
                    OperationalMode = ProgramMode.F4Mode;
                    break;
                }
                else if (arg.IndexOf("Lomac") != -1)
                {
                    OperationalMode = ProgramMode.LomacMode;
                    break;
                }
                else if (arg.IndexOf("reset") != -1)
                {
                    Console.WriteLine("Reseting Configuration to default");
                    Console.ReadLine();   
                    Properties.Settings.Default.GPSComPort = "Com0";
                    Properties.Settings.Default.Save();
                }

                
            }



            GPSComPort = Properties.Settings.Default.GPSComPort;



            bool bFoundPort = false;
            foreach (string s in SerialPort.GetPortNames())
            {
                if (0 == String.Compare(s, GPSComPort, true))
                {
                    bFoundPort = true;
                    Console.WriteLine("Using ");
                }
            }


            if (!bFoundPort)
            {
                
                string strYorN;
                Console.WriteLine("Unable to find configured Serial Port " + GPSComPort);
                Console.WriteLine("Press Enter to Continue");
                Console.ReadLine();
                Console.WriteLine("Installed Serial Ports");
                foreach (string s in SerialPort.GetPortNames())
                {
                    Console.Write(s);
                    Console.Write(" Use this port (Y/N): ");
                    strYorN = Console.ReadLine();
                    if (strYorN.Contains("Y") || strYorN.Contains("y"))
                    {
                        Console.WriteLine("Saving GPS Serial Port " + s);
                        Properties.Settings.Default.GPSComPort = s;
                        Properties.Settings.Default.Save();
                        Console.WriteLine("Press Enter to Continue");
                        Console.ReadLine();            
                        break;
                    }
 
                }
                Console.WriteLine("Press Enter to Continue");
                Console.ReadLine();
            }
        }
    }
}
