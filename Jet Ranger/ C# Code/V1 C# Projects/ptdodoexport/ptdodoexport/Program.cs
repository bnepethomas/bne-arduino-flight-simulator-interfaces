using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;       // Needed for DLL Call


namespace ptdodoexport
{
    class Program
    {
        
        //typedef int (WINAPI *GETLIGHT) (void);
        //[DllImport("DodoSim206FSXDataExport.dll")] static extern Boolean MessageBeep(UInt32 beepType);
        [DllImport("DodoSim206FSXDataExport.dll")]
        public static extern int GetEngOutLightState();
        //HMODULE hLibrary = LoadLibrary(TEXT("DodoSim206FSXDataExport.dll"));
        static void Main(string[] args)
        {
            while (true)
            {
                System.Console.WriteLine("Time is " + System.DateTime.Now.ToLongTimeString());
                System.Console.WriteLine("Sending Request");
                int dodresp;
                dodresp = GetEngOutLightState();
                System.Console.WriteLine("Sent Request " +  dodresp.ToString());
                System.Console.WriteLine("done");
                System.Threading.Thread.Sleep(500);
            }

        }
    }
}
