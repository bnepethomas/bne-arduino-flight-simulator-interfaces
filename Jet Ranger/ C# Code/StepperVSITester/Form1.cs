using System.Net.Sockets;
using System.Text;

namespace StepperVSITester
{
    public partial class frmMain : Form
    {
        // JET_RANGER_STEPPER_CONTROLLER.ino's MSFSport - this tool stands in
        // for FSUIPCWinformsAutoCS's stepperClient while testing the
        // stepper board's "VSI"/"ALT" UDP handlers in isolation, without
        // needing FSUIPC/a flight sim running.
        UdpClient stepperClient = new UdpClient();

        private void Send(string code, long value)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D," + code + ":" + value.ToString());
            stepperClient.Send(sendBytes, sendBytes.Length);
        }

        private void SendManualValue(TrackBar trackBar, TextBox textBox, Label label, string code, string unit)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                if (value >= trackBar.Minimum && value <= trackBar.Maximum)
                {
                    trackBar.Value = (int)value;
                    UpdateValueLabel(label, value, unit);
                    Send(code, value);
                }
                else
                {
                    MessageBox.Show($"Value must be between {trackBar.Minimum} and {trackBar.Maximum}");
                }
            }
            else
            {
                MessageBox.Show("Value must be a number");
            }
        }

        private void UpdateValueLabel(Label label, long value, string unit)
        {
            label.Text = $"Value: {value} {unit}";
        }

        public frmMain()
        {
            InitializeComponent();

            stepperClient.Connect("172.16.1.105", 13136);

            trkVsi.Value = 0;
            UpdateValueLabel(lblValue, 0, "fpm");
            txtRawInput.Text = "0";

            trkAlt.Value = 0;
            UpdateValueLabel(lblAltValue, 0, "ft");
            txtAltInput.Text = "0";

            trkRadarAlt.Value = 0;
            UpdateValueLabel(lblRadarAltValue, 0, "ft");
            txtRadarAltInput.Text = "0";
        }

        private void trkVsi_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblValue, trkVsi.Value, "fpm");
            txtRawInput.Text = trkVsi.Value.ToString();
            Send("VSI", trkVsi.Value);
        }

        private void butZero_Click(object sender, EventArgs e)
        {
            trkVsi.Value = 0;
            txtRawInput.Text = "0";
            UpdateValueLabel(lblValue, 0, "fpm");
            Send("VSI", 0);
        }

        private void butSendRaw_Click(object sender, EventArgs e)
        {
            SendManualValue(trkVsi, txtRawInput, lblValue, "VSI", "fpm");
        }

        private void txtRawInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkVsi, txtRawInput, lblValue, "VSI", "fpm");
            }
        }

        private void trkAlt_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblAltValue, trkAlt.Value, "ft");
            txtAltInput.Text = trkAlt.Value.ToString();
            Send("ALT", trkAlt.Value);
        }

        private void butAltZero_Click(object sender, EventArgs e)
        {
            trkAlt.Value = 0;
            txtAltInput.Text = "0";
            UpdateValueLabel(lblAltValue, 0, "ft");
            Send("ALT", 0);
        }

        private void butSendAlt_Click(object sender, EventArgs e)
        {
            SendManualValue(trkAlt, txtAltInput, lblAltValue, "ALT", "ft");
        }

        private void txtAltInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkAlt, txtAltInput, lblAltValue, "ALT", "ft");
            }
        }

        // Radar Altimeter test frame: sends feet, converted to steps on the
        // board's side by RADAR_ALT_FT_TABLE/radarAltFtToSteps() (see the
        // "RALT" case in Jet_Ranger_Driver_Test.ino's
        // HandleOutputValuePair()) - same pattern as ALT above, now that
        // this gauge's real 0/500/2500 ft calibration is known.
        private void trkRadarAlt_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRadarAltValue, trkRadarAlt.Value, "ft");
            txtRadarAltInput.Text = trkRadarAlt.Value.ToString();
            Send("RALT", trkRadarAlt.Value);
        }

        private void butRadarAltZero_Click(object sender, EventArgs e)
        {
            trkRadarAlt.Value = 0;
            txtRadarAltInput.Text = "0";
            UpdateValueLabel(lblRadarAltValue, 0, "ft");
            Send("RALT", 0);
        }

        private void butSendRadarAlt_Click(object sender, EventArgs e)
        {
            SendManualValue(trkRadarAlt, txtRadarAltInput, lblRadarAltValue, "RALT", "ft");
        }

        private void txtRadarAltInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkRadarAlt, txtRadarAltInput, lblRadarAltValue, "RALT", "steps");
            }
        }

        // Direct-step jog for the Altimeter: sends a signed relative step
        // count and the delay (ms) to hold between each raw step pulse, for
        // Jet_Ranger_Driver_Test.ino's jogAltimeterSteps(), which bit-bangs
        // the ALT step/dir pins directly rather than going through
        // AccelStepper's acceleration ramp - lets an operator find exact
        // step timing on the bench. Packed as "<steps>/<intervalMs>" in a
        // single "ASTEP" value since the board's "D,CODE:value" packet
        // parser only reads one value per code.
        private void SendAltJog(long steps, long intervalMs)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes($"D,ASTEP:{steps}/{intervalMs}");
            stepperClient.Send(sendBytes, sendBytes.Length);
        }

        private void butJogSend_Click(object sender, EventArgs e)
        {
            if (!long.TryParse(txtJogSteps.Text, out long steps))
            {
                MessageBox.Show("Steps must be a whole number (e.g. 150 or -150)");
                return;
            }

            if (!long.TryParse(txtJogInterval.Text, out long intervalMs) || intervalMs <= 0)
            {
                MessageBox.Show("Interval must be a positive number of milliseconds");
                return;
            }

            SendAltJog(steps, intervalMs);
        }

        private void txtJogSteps_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                butJogSend_Click(sender, e);
            }
        }

        private void txtJogInterval_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                butJogSend_Click(sender, e);
            }
        }
    }
}
