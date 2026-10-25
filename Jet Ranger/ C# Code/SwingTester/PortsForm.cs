using System.ComponentModel;
using System.Diagnostics;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;

namespace SwingTester
{
    /// <summary>
    /// Lists the serial (COM) ports Windows knows about with the USB device behind each one: the board type guessed from the USB vendor and product
    /// ids, the USB serial number (genuine Arduino boards have one, most clones do not), and whether another program has the port open. A USB
    /// device can be restarted from here (pnputil /restart-device, which needs administrator rights so Windows asks first). The list refreshes by
    /// itself when a device is plugged in or removed.
    /// </summary>
    internal sealed class PortsForm : Form
    {
        private sealed class PortInfo
        {
            public string Port = "";
            public string Description = "";
            public string Board = "";
            public string VidPid = "";
            public string Serial = "";
            public string Status = "";
            public string InUse = "";
            public string UsbDeviceId = "";   // the USB device node that gets restarted
            public string PortDeviceId = "";
        }

        private const int WM_DEVICECHANGE = 0x0219;

        private readonly DataGridView grid = new()
        {
            Dock = DockStyle.Fill,
            ReadOnly = true,
            AllowUserToAddRows = false,
            AllowUserToDeleteRows = false,
            AllowUserToResizeRows = false,
            RowHeadersVisible = false,
            SelectionMode = DataGridViewSelectionMode.FullRowSelect,
            MultiSelect = false,
            AutoGenerateColumns = false,
            BackgroundColor = SystemColors.Window
        };
        private readonly Button btnRefresh = new() { Text = "Refresh", AutoSize = true };
        private readonly Button btnRestart = new() { Text = "Restart selected USB device", AutoSize = true, Enabled = false };
        private readonly Button btnCopy = new() { Text = "Copy list", AutoSize = true };
        private readonly Label lblStatus = new() { AutoSize = true, Margin = new Padding(12, 6, 3, 0), Text = "" };
        private readonly System.Windows.Forms.Timer deviceChangeTimer = new() { Interval = 700 };
        private List<PortInfo> ports = new();

        public PortsForm()
        {
            Text = "COM ports and USB devices";
            StartPosition = FormStartPosition.CenterParent;
            ClientSize = new Size(1040, 360);
            MinimumSize = new Size(760, 240);

            AddColumn("Port", "Port", 70);
            AddColumn("Description", "Description", 230);
            AddColumn("Board", "Board (guessed)", 150);
            AddColumn("VidPid", "USB VID:PID", 95);
            AddColumn("Serial", "USB serial number", 170);
            AddColumn("InUse", "Open elsewhere", 100);
            AddColumn("Status", "Status", 70);
            AddColumn("UsbDeviceId", "USB device ID", 220);
            grid.Columns["UsbDeviceId"]!.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

            var buttons = new FlowLayoutPanel { Dock = DockStyle.Bottom, AutoSize = true, Padding = new Padding(8, 6, 8, 6), WrapContents = false };
            buttons.Controls.Add(btnRefresh);
            buttons.Controls.Add(btnRestart);
            buttons.Controls.Add(btnCopy);
            buttons.Controls.Add(lblStatus);

            var note = new Label
            {
                Dock = DockStyle.Top,
                AutoSize = false,
                Height = 40,
                Padding = new Padding(8, 6, 8, 0),
                Text = "Restart stops and starts the selected USB device's driver (the same as disabling and enabling it in Device Manager); it cannot cut the power to " +
                       "the port. Windows asks for administrator rights. Close any program that has the port open (Arduino IDE serial monitor) first."
            };

            Controls.Add(grid);
            Controls.Add(note);
            Controls.Add(buttons);

            btnRefresh.Click += (_, _) => Reload();
            btnRestart.Click += async (_, _) => await RestartSelectedAsync();
            btnCopy.Click += (_, _) => CopyList();
            grid.SelectionChanged += (_, _) => UpdateButtons();
            deviceChangeTimer.Tick += (_, _) => { deviceChangeTimer.Stop(); Reload(); };
            Load += (_, _) => Reload();
        }

        private void AddColumn(string name, string header, int width)
        {
            grid.Columns.Add(new DataGridViewTextBoxColumn { Name = name, HeaderText = header, Width = width, SortMode = DataGridViewColumnSortMode.NotSortable });
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            if (m.Msg == WM_DEVICECHANGE)
            {
                deviceChangeTimer.Stop();
                deviceChangeTimer.Start();
            }
        }

        private void UpdateButtons()
        {
            btnRestart.Enabled = SelectedPort()?.UsbDeviceId.Length > 0;
        }

        private PortInfo? SelectedPort()
        {
            if (grid.CurrentRow == null) return null;
            int i = grid.CurrentRow.Index;
            return i >= 0 && i < ports.Count ? ports[i] : null;
        }

        private void Reload()
        {
            string? keep = SelectedPort()?.Port;
            try
            {
                ports = ReadPorts();
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Could not read the device list: " + ex.Message;
                return;
            }
            grid.Rows.Clear();
            foreach (var p in ports)
                grid.Rows.Add(p.Port, p.Description, p.Board, p.VidPid, p.Serial, p.InUse, p.Status, p.UsbDeviceId);
            if (keep != null)
            {
                int idx = ports.FindIndex(p => p.Port == keep);
                if (idx >= 0) grid.CurrentCell = grid.Rows[idx].Cells[0];
            }
            lblStatus.Text = ports.Count + " port" + (ports.Count == 1 ? "" : "s") + " found at " + DateTime.Now.ToString("HH:mm:ss") + ".";
            UpdateButtons();
        }

        private void CopyList()
        {
            var sb = new StringBuilder("Port\tDescription\tBoard\tVID:PID\tSerial number\tOpen elsewhere\tStatus\tUSB device ID\r\n");
            foreach (var p in ports)
                sb.Append(string.Join("\t", p.Port, p.Description, p.Board, p.VidPid, p.Serial, p.InUse, p.Status, p.UsbDeviceId)).Append("\r\n");
            try { Clipboard.SetText(sb.ToString()); lblStatus.Text = "List copied to the clipboard."; }
            catch (Exception ex) { lblStatus.Text = "Copy failed: " + ex.Message; }
        }

        private async Task RestartSelectedAsync()
        {
            var p = SelectedPort();
            if (p == null || p.UsbDeviceId.Length == 0) return;
            var answer = MessageBox.Show(this,
                "Restart " + p.Port + " (" + (p.Board.Length > 0 ? p.Board : p.Description) + ")?\r\n\r\n" + p.UsbDeviceId +
                "\r\n\r\nThe board's USB connection drops for a moment (a running sketch keeps running if it has its own power). Windows will ask for administrator rights.",
                "Restart USB device", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (answer != DialogResult.OK) return;

            btnRestart.Enabled = false;
            lblStatus.Text = "Restarting " + p.Port + "...";
            try
            {
                int code = await Task.Run(() =>
                {
                    var psi = new ProcessStartInfo("pnputil.exe", "/restart-device \"" + p.UsbDeviceId + "\"")
                    {
                        Verb = "runas",
                        UseShellExecute = true,
                        WindowStyle = ProcessWindowStyle.Hidden
                    };
                    using var proc = Process.Start(psi);
                    if (proc == null) return -1;
                    proc.WaitForExit(30000);
                    return proc.HasExited ? proc.ExitCode : -2;
                });
                lblStatus.Text = code switch
                {
                    0 => p.Port + " restarted.",
                    3010 => p.Port + " restarted, but Windows wants a reboot to finish.",
                    -2 => "pnputil did not finish within 30 seconds.",
                    _ => "pnputil returned error " + code + " (the device may be in use, or not restartable)."
                };
            }
            catch (Win32Exception ex) when (ex.NativeErrorCode == 1223)
            {
                lblStatus.Text = "Cancelled: administrator rights were not granted.";
            }
            catch (Exception ex)
            {
                lblStatus.Text = "Restart failed: " + ex.Message;
            }
            await Task.Delay(1500);
            Reload();
        }

        // ---------------------------------------------------------------------------------------------------------------------------------
        private static List<PortInfo> ReadPorts()
        {
            var result = new List<PortInfo>();
            var comRegex = new Regex(@"\((COM\d+)\)", RegexOptions.IgnoreCase);
            using var searcher = new ManagementObjectSearcher("SELECT Name, PNPDeviceID, Manufacturer, ConfigManagerErrorCode FROM Win32_PnPEntity WHERE Name LIKE '%(COM%)'");
            foreach (ManagementObject mo in searcher.Get())
            {
                string name = mo["Name"] as string ?? "";
                var m = comRegex.Match(name);
                if (!m.Success) continue;
                string deviceId = mo["PNPDeviceID"] as string ?? "";
                uint err = mo["ConfigManagerErrorCode"] is uint e ? e : 0;
                var info = new PortInfo
                {
                    Port = m.Groups[1].Value.ToUpperInvariant(),
                    Description = name.Replace(m.Value, "").Trim(),
                    PortDeviceId = deviceId,
                    Status = err == 0 ? "OK" : "Error " + err
                };
                string usb = FindUsbDevice(deviceId);
                info.UsbDeviceId = usb;
                if (usb.Length > 0)
                {
                    var vp = Regex.Match(usb, @"VID_([0-9A-F]{4})&PID_([0-9A-F]{4})", RegexOptions.IgnoreCase);
                    if (vp.Success)
                    {
                        info.VidPid = vp.Groups[1].Value.ToUpperInvariant() + ":" + vp.Groups[2].Value.ToUpperInvariant();
                        info.Board = BoardGuess(info.VidPid);
                    }
                    // Windows makes up an instance id containing '&' (for example 6&870CE29&0&1) when the device has no serial number of its own.
                    string last = usb.Substring(usb.LastIndexOf('\\') + 1);
                    info.Serial = last.Contains('&') ? "(none)" : last;
                }
                else
                {
                    info.Board = "not a USB port";
                }
                info.InUse = IsPortOpenElsewhere(info.Port);
                result.Add(info);
            }
            result.Sort((a, b) => PortNumber(a.Port).CompareTo(PortNumber(b.Port)));
            return result;
        }

        private static int PortNumber(string port) => int.TryParse(port.Substring(3), out int n) ? n : 0;

        private static string BoardGuess(string vidPid) => vidPid switch
        {
            "2341:0042" or "2341:0010" or "2A03:0042" or "2A03:0010" => "Arduino Mega 2560",
            "2341:8036" or "2341:0036" or "2A03:8036" => "Arduino Leonardo",
            "2341:0043" or "2341:0001" => "Arduino Uno",
            "2341:003D" or "2341:003E" => "Arduino Due",
            "1A86:7523" or "1A86:7522" => "CH340 serial (clone board)",
            "0403:6001" => "FTDI FT232R",
            "10C4:EA60" => "CP210x serial",
            _ => ""
        };

        /// <summary>Walks up the device tree from the COM port device to the USB device node (VID_xxxx&amp;PID_xxxx\serial, not a composite function MI_xx).</summary>
        private static string FindUsbDevice(string deviceId)
        {
            if (deviceId.Length == 0) return "";
            string current = deviceId;
            for (int depth = 0; depth < 6; depth++)
            {
                if (current.StartsWith("USB\\VID_", StringComparison.OrdinalIgnoreCase) && !current.Contains("&MI_", StringComparison.OrdinalIgnoreCase))
                    return current;
                string? parent = ParentOf(current);
                if (parent == null) return "";
                current = parent;
            }
            return "";
        }

        [DllImport("cfgmgr32.dll", CharSet = CharSet.Unicode)]
        private static extern int CM_Locate_DevNodeW(out uint devInst, string deviceId, uint flags);
        [DllImport("cfgmgr32.dll")]
        private static extern int CM_Get_Parent(out uint parent, uint devInst, uint flags);
        [DllImport("cfgmgr32.dll", CharSet = CharSet.Unicode)]
        private static extern int CM_Get_Device_IDW(uint devInst, StringBuilder buffer, int length, uint flags);

        private static string? ParentOf(string deviceId)
        {
            if (CM_Locate_DevNodeW(out uint inst, deviceId, 0) != 0) return null;
            if (CM_Get_Parent(out uint parent, inst, 0) != 0) return null;
            var sb = new StringBuilder(512);
            return CM_Get_Device_IDW(parent, sb, sb.Capacity, 0) == 0 ? sb.ToString() : null;
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern IntPtr CreateFileW(string name, uint access, uint share, IntPtr security, uint creation, uint flags, IntPtr template);
        [DllImport("kernel32.dll")]
        private static extern bool CloseHandle(IntPtr handle);

        /// <summary>Opens the port exclusively for an instant: "access denied" means another program has it open.</summary>
        private static string IsPortOpenElsewhere(string port)
        {
            IntPtr h = CreateFileW(@"\\.\" + port, 0xC0000000 /* read + write */, 0, IntPtr.Zero, 3 /* OPEN_EXISTING */, 0, IntPtr.Zero);
            if (h != new IntPtr(-1))
            {
                CloseHandle(h);
                return "no";
            }
            return Marshal.GetLastWin32Error() == 5 ? "YES" : "?";
        }
    }
}
