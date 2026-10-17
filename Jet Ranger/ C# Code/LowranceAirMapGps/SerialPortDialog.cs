using System.IO.Ports;

namespace LowranceAirMapGps
{
    /// <summary>Asks the user to pick the serial port the GPS is connected to.</summary>
    public class SerialPortDialog : Form
    {
        private readonly ComboBox _combo = new() { DropDownStyle = ComboBoxStyle.DropDownList, Left = 16, Top = 44, Width = 280 };
        private readonly Button _ok = new() { Text = "OK", DialogResult = DialogResult.OK, Left = 140, Top = 88, Width = 75 };
        private readonly Button _cancel = new() { Text = "Cancel", DialogResult = DialogResult.Cancel, Left = 221, Top = 88, Width = 75 };
        private readonly Button _refresh = new() { Text = "Refresh", Left = 16, Top = 88, Width = 75 };

        public string? SelectedPort => _combo.SelectedItem as string;

        public SerialPortDialog(string? current)
        {
            Text = "Select GPS serial port";
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = MinimizeBox = false;
            StartPosition = FormStartPosition.CenterParent;
            ClientSize = new Size(312, 130);
            AcceptButton = _ok;
            CancelButton = _cancel;

            Controls.Add(new Label { Text = "Serial port the Lowrance AirMap 2000C is connected to\n(115200 bps, 8 data bits, 1 stop bit, no parity):", Left = 16, Top = 8, Width = 290, Height = 32 });
            Controls.AddRange(new Control[] { _combo, _ok, _cancel, _refresh });
            _refresh.Click += (_, _) => Fill(SelectedPort ?? current);
            Fill(current);
        }

        private void Fill(string? select)
        {
            _combo.Items.Clear();
            foreach (string p in SerialPort.GetPortNames().OrderBy(n => n.Length).ThenBy(n => n, StringComparer.OrdinalIgnoreCase))
                _combo.Items.Add(p);
            if (select != null && _combo.Items.Contains(select)) _combo.SelectedItem = select;
            else if (_combo.Items.Count > 0) _combo.SelectedIndex = 0;
            _ok.Enabled = _combo.Items.Count > 0;
        }
    }
}
