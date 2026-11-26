using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Windows.Forms;

namespace P3DViewLauncher
{
    class ViewSetting
    {
        public string Camera, CustomCamera, Title;   // CustomCamera is null unless the line says  camera > custom camera
        public int Screen;
        public Size? Size;                            // null: keep the size Prepar3D gave the window
        public bool FullScreen;                       // no frame, covering the whole screen
        public bool Main;                             // Prepar3D's own main view: only its camera is set
    }

    /// <summary>Opens Prepar3D views with chosen cameras, undocks each one, and moves it to a screen, as an ordinary
    /// window or (only when asked) full screen. It never shows a dialog box: everything is written to the console and the exit code is 0 only if all worked.</summary>
    static class Program
    {
        const string Usage =
@"P3DViewLauncher [options] [views file]

  (no options)   open the views listed in views.txt (next to the program)
  --wait         keep trying until Prepar3D is running
  --close        close the listed views
  --screens      list the screen numbers
  --identify     show each screen's number on that screen for 30 seconds
  --windows      list Prepar3D's window titles";

        static int Main(string[] args)
        {
            try
            {
                NativeWindows.UseRealPixels();
                var options = args.Where(a => a.StartsWith("--")).Select(a => a.ToLowerInvariant()).ToList();
                var unknown = options.Except(new[] { "--wait", "--close", "--screens", "--windows", "--identify" }).ToList();
                var files = args.Where(a => !a.StartsWith("--")).ToList();
                if (unknown.Count > 0 || files.Count > 1) { Console.WriteLine(Usage); return 2; }

                if (options.Contains("--screens"))
                {
                    var screens = Screens();
                    for (int i = 0; i < screens.Length; i++)
                        Console.WriteLine("Screen {0}: {1} x {2} at {3},{4}{5}  ({6})", i + 1, screens[i].Bounds.Width, screens[i].Bounds.Height,
                            screens[i].Bounds.X, screens[i].Bounds.Y, screens[i].Primary ? "  primary" : "", screens[i].DeviceName);
                    return 0;
                }
                if (options.Contains("--identify"))
                {
                    Identify(Screens(), 30);
                    return 0;
                }
                if (options.Contains("--windows"))
                {
                    var windows = NativeWindows.SimulatorWindows();
                    foreach (var w in windows) Console.WriteLine(w.Value);
                    if (windows.Count == 0) Console.WriteLine("No Prepar3D windows found.");
                    return 0;
                }

                string file = files.Count == 1 ? files[0] : Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "views.txt");
                var views = ReadViews(file, Screens().Length);
                return Run(views, options.Contains("--wait"), options.Contains("--close"));
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.Message);
                return 1;
            }
        }

        /// <summary>The screens in the order of their Windows device names (DISPLAY1, DISPLAY2, ...).</summary>
        static Screen[] Screens()
            => Screen.AllScreens.OrderBy(s => s.DeviceName.Length).ThenBy(s => s.DeviceName).ToArray();

        /// <summary>A large number in the middle of each screen, on top of everything. It closes by itself.</summary>
        static void Identify(Screen[] screens, int seconds)
        {
            var forms = new List<Form>();
            for (int i = 0; i < screens.Length; i++)
            {
                Rectangle area = screens[i].Bounds;
                var form = new Form
                {
                    FormBorderStyle = FormBorderStyle.None, StartPosition = FormStartPosition.Manual, TopMost = true,
                    ShowInTaskbar = false, BackColor = Color.Black,
                    Bounds = new Rectangle(area.X + area.Width / 4, area.Y + area.Height / 4, area.Width / 2, area.Height / 2)
                };
                form.Controls.Add(new Label
                {
                    Dock = DockStyle.Fill, TextAlign = ContentAlignment.MiddleCenter, ForeColor = Color.Yellow,
                    Font = new Font("Segoe UI", area.Height / 8f, FontStyle.Bold, GraphicsUnit.Pixel),
                    Text = "Screen " + (i + 1)
                });
                form.Show();
                forms.Add(form);
            }
            var clock = Stopwatch.StartNew();
            while (clock.Elapsed.TotalSeconds < seconds)
            {
                Application.DoEvents();
                Thread.Sleep(50);
            }
            foreach (var form in forms) form.Close();
        }

        static List<ViewSetting> ReadViews(string file, int screenCount)
        {
            var views = new List<ViewSetting>();
            string[] lines = File.ReadAllLines(file);
            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i].Trim();
                if (line.Length == 0 || line.StartsWith("#")) continue;
                string[] parts = line.Split('|').Select(p => p.Trim()).ToArray();
                string where = file + " line " + (i + 1) + ": ";
                bool main = parts.Length == 2 && string.Equals(parts[1], "main", StringComparison.OrdinalIgnoreCase);
                if ((!main && (parts.Length < 3 || parts.Length > 4)) || parts[0].Length == 0 || parts[1].Length == 0)
                    throw new FormatException(where + "expected  camera name | window title | screen number | size (optional),  or  camera name | main");
                int screen = 0;
                if (!main && (!int.TryParse(parts[2], out screen) || screen < 1 || screen > screenCount))
                    throw new FormatException(where + "the screen number must be 1 to " + screenCount);
                Size? size = null;
                bool fullScreen = parts.Length == 4 && string.Equals(parts[3], "fullscreen", StringComparison.OrdinalIgnoreCase);
                if (parts.Length == 4 && parts[3].Length > 0 && !fullScreen)
                {
                    string[] numbers = parts[3].ToLowerInvariant().Split('x');
                    if (numbers.Length != 2 || !int.TryParse(numbers[0], out int width) || !int.TryParse(numbers[1], out int height)
                        || width < 100 || height < 100)
                        throw new FormatException(where + "the last field must be fullscreen, a size such as 1280x720, or left out");
                    size = new Size(width, height);
                }
                if (views.Any(v => string.Equals(v.Title, parts[1], StringComparison.OrdinalIgnoreCase)))
                    throw new FormatException(where + "the window title \"" + parts[1] + "\" is used twice");
                string[] cameras = parts[0].Split('>').Select(p => p.Trim()).ToArray();
                if (cameras.Length > 2 || cameras.Any(c => c.Length == 0))
                    throw new FormatException(where + "expected a camera name, or  camera name > custom camera name");
                views.Add(new ViewSetting { Camera = cameras[0], CustomCamera = cameras.Length == 2 ? cameras[1] : null,
                                            Title = parts[1], Screen = screen, Size = size, FullScreen = fullScreen, Main = main });
            }
            if (views.Count == 0) throw new FormatException(file + " lists no views");
            return views;
        }

        // Kept out of Main so that --screens and --windows work even if the SimConnect library cannot be loaded.
        [MethodImpl(MethodImplOptions.NoInlining)]
        static int Run(List<ViewSetting> views, bool wait, bool close)
        {
            using (var sim = new Simulator())
            {
                if (!sim.Connect(wait)) return 1;

                if (close)
                {
                    foreach (var view in views.Where(v => !v.Main))
                    {
                        Console.WriteLine("Closing " + view.Title);
                        sim.CloseView(view.Title);
                    }
                    sim.Pump(1000);
                    return 0;
                }

                int failures = 0;
                Screen[] screens = Screens();
                foreach (var view in views)
                {
                    if (view.Main)
                    {
                        Console.WriteLine("Main view: camera \"{0}\"", view.CustomCamera ?? view.Camera);
                        sim.SetMainCamera(view.Camera, view.CustomCamera);
                        sim.Pump(500);
                        Console.WriteLine("  sent");
                        continue;
                    }
                    Console.WriteLine("{0}: camera \"{1}\" to screen {2}", view.Title, view.CustomCamera ?? view.Camera, view.Screen);
                    IntPtr window = NativeWindows.FindSimulatorWindow(view.Title);
                    if (window == IntPtr.Zero)
                    {
                        sim.OpenView(view.Camera, view.Title);
                        sim.Pump(2000);
                        sim.UndockView(view.Title);
                        for (int i = 0; i < 20 && window == IntPtr.Zero; i++)
                        {
                            sim.Pump(500);
                            window = NativeWindows.FindSimulatorWindow(view.Title);
                        }
                    }
                    else Console.WriteLine("  already open; moving it");

                    string problem = window == IntPtr.Zero
                        ? "no window called \"" + view.Title + "\" appeared. Check the camera name, that a flight is loaded and that this program runs on the simulator PC"
                        : view.FullScreen ? NativeWindows.FillScreen(window, screens[view.Screen - 1].Bounds)
                        : NativeWindows.Place(window, screens[view.Screen - 1].WorkingArea, view.Size);
                    if (problem == null && view.CustomCamera != null)
                    {
                        sim.PanToCustomCamera(view.Title, view.CustomCamera);
                        sim.Pump(500);
                    }
                    if (problem == null) Console.WriteLine("  done");
                    else { Console.WriteLine("  FAILED: " + problem); failures++; }
                    sim.Pump(500);
                }
                return failures == 0 ? 0 : 1;
            }
        }
    }
}
