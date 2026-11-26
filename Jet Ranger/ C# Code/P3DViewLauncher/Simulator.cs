using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using LockheedMartin.Prepar3D.SimConnect;

namespace P3DViewLauncher
{
    /// <summary>The SimConnect connection and the view requests.</summary>
    sealed class Simulator : IDisposable
    {
        readonly AutoResetEvent signal = new AutoResetEvent(false);
        SimConnect connection;
        bool open, quit;

        public bool Connect(bool wait)
        {
            bool told = false;
            while (true)
            {
                try
                {
                    connection = new SimConnect("P3D View Launcher", IntPtr.Zero, 0, signal, 0);
                    connection.OnRecvOpen += (s, d) => open = true;
                    connection.OnRecvQuit += (s, d) => quit = true;
                    // Prepar3D reports a refused request (for example an unknown camera) here.
                    connection.OnRecvException += (s, d) => Console.WriteLine("  Prepar3D reported: " + (SIMCONNECT_EXCEPTION)d.dwException);
                    for (int i = 0; i < 20 && !open; i++) Pump(250);
                    if (open) return true;
                    connection.Dispose();
                    throw new InvalidOperationException("Prepar3D did not answer.");
                }
                catch (Exception ex) when (ex is COMException || ex is InvalidOperationException)
                {
                    if (!wait) { Console.WriteLine("Could not connect to Prepar3D. Is it running?"); return false; }
                    if (!told) Console.WriteLine("Waiting for Prepar3D...");
                    told = true;
                    Thread.Sleep(2000);
                }
            }
        }

        public void OpenView(string camera, string title) => connection.OpenView(camera, title);
        public void UndockView(string title) => connection.UndockView(title);
        public void CloseView(string title) => connection.CloseView(title);
        /// <summary>Points a view window at a custom camera (CustomCameras.xml). OpenView does not accept those names.</summary>
        public void PanToCustomCamera(string title, string camera) => connection.CameraPanToView(title, camera, 0f);

        /// <summary>Sets the camera of Prepar3D's main view, then the custom camera if there is one.</summary>
        public void SetMainCamera(string camera, string customCamera)
        {
            connection.ChangeView(camera);
            if (customCamera == null) return;
            Pump(1000);
            connection.MainCameraPanToView(customCamera, 0f);
        }

        /// <summary>Handles messages from the simulator for the given time.</summary>
        public void Pump(int milliseconds)
        {
            var clock = Stopwatch.StartNew();
            do
            {
                int left = (int)Math.Max(0, milliseconds - clock.ElapsedMilliseconds);
                if (signal.WaitOne(left)) connection.ReceiveMessage();
                if (quit) throw new Exception("Prepar3D closed.");
            }
            while (clock.ElapsedMilliseconds < milliseconds);
        }

        public void Dispose()
        {
            connection?.Dispose();
            signal.Dispose();
        }
    }
}
