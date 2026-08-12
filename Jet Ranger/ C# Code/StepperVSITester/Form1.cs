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

            cboNewGauge.SelectedIndex = 0;
            txtNewGaugeSteps.Text = "0";

            trkEgt.Value = 0;
            UpdateValueLabel(lblEgtValue, 0, "C");
            txtEgtInput.Text = "0";

            txtEot.Text = "0";
            txtEop.Text = "0";
            txtXot.Text = "0";
            txtXop.Text = "0";
            txtTs.Text = "0";
            txtRs.Text = "0";
            txtGp.Text = "0";
            txtFa.Text = "0";

            trkIas.Value = 0;
            UpdateValueLabel(lblIasValue, 0, "kt");
            txtIasInput.Text = "0";
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
        // "AGL" case in Jet_Ranger_Driver_Test.ino's
        // HandleOutputValuePair()) - same pattern as ALT above, now that
        // this gauge's real 0/500/2500 ft calibration is known. Wire code
        // renamed from "RALT" to "AGL" to match both
        // JET_RANGER_STEPPER_CONTROLLER.ino and JET_RANGER_SERVO_CONTROLLER.ino
        // (and what FSUIPCWinformsAutoCS actually sends) - control names
        // here are unchanged.
        private void trkRadarAlt_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRadarAltValue, trkRadarAlt.Value, "ft");
            txtRadarAltInput.Text = trkRadarAlt.Value.ToString();
            Send("AGL", trkRadarAlt.Value);
        }

        private void butRadarAltZero_Click(object sender, EventArgs e)
        {
            trkRadarAlt.Value = 0;
            txtRadarAltInput.Text = "0";
            UpdateValueLabel(lblRadarAltValue, 0, "ft");
            Send("AGL", 0);
        }

        private void butSendRadarAlt_Click(object sender, EventArgs e)
        {
            SendManualValue(trkRadarAlt, txtRadarAltInput, lblRadarAltValue, "AGL", "ft");
        }

        private void txtRadarAltInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkRadarAlt, txtRadarAltInput, lblRadarAltValue, "AGL", "ft");
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

        // Raw step pass-through test frame giving JET_RANGER_STEPPER_CONTROLLER.ino's
        // gauges the same "direct step target" capability Stepper-Tuning-Harness
        // offers for every gauge over Serial (see that sketch's HandleOutputValuePair()).
        // Two groups of codes live in this one dropdown:
        //  - TQ/FLAPS/AOA/GFORCE/SPDMAX: gauges with no real calibration at all yet,
        //    so raw steps is their only option.
        //  - IASRAW/ALTRAW/VSIRAW/OILTRAW/OILPRAW/XMSNTRAW/XMSNPRAW/ITTRAW/RPMERAW/
        //    RPMRRAW/N1RAW/FUELRAW: distinct raw-step siblings of gauges that DO have
        //    a real-unit code/section elsewhere in this form (IAS/ALT/VSI trackbars,
        //    the EGT/ITT trackbar, the OILT/OILP/XMSNT/XMSNP/RPME/RPMR/N1/FUEL rows) -
        //    lets the operator bypass that gauge's unit conversion for bench testing
        //    without losing the real-value control.
        // One shared control with a dropdown rather than a duplicate raw-step
        // trackbar/row next to every calibrated gauge - cheaper to keep in sync, and
        // easy to give a gauge its own dedicated raw control later if that turns out
        // to be worth the extra screen space.
        private void butNewGaugeSend_Click(object sender, EventArgs e)
        {
            if (!long.TryParse(txtNewGaugeSteps.Text, out long steps))
            {
                MessageBox.Show("Steps must be a whole number (e.g. 150 or -150)");
                return;
            }

            Send(cboNewGauge.SelectedItem?.ToString() ?? "EOT", steps);
        }

        private void butNewGaugeZero_Click(object sender, EventArgs e)
        {
            txtNewGaugeSteps.Text = "0";
            Send(cboNewGauge.SelectedItem?.ToString() ?? "EOT", 0);
        }

        private void txtNewGaugeSteps_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                butNewGaugeSend_Click(sender, e);
            }
        }

        // EGT (Exhaust Gas Temp), 0-900C: the first of the New Gauges to
        // graduate from raw steps to a real value, now that its unit/range
        // is known. Sends degrees C directly; the board converts to steps
        // via a linear scale (setEGT()/egtCToSteps() in
        // JET_RANGER_STEPPER_CONTROLLER.ino) since EGTstepper has no
        // bench-measured calibration yet - same "good enough for a first
        // pass" approach ALT's feet*5.76 conversion used before VSI/Radar
        // ALT got real per-point tables. Wire code is "ITT" (not "EGT") to
        // match JET_RANGER_SERVO_CONTROLLER.ino's existing code for this
        // same real-world quantity - control names here are unchanged.
        private void trkEgt_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblEgtValue, trkEgt.Value, "C");
            txtEgtInput.Text = trkEgt.Value.ToString();
            Send("ITT", trkEgt.Value);
        }

        private void butEgtZero_Click(object sender, EventArgs e)
        {
            trkEgt.Value = 0;
            txtEgtInput.Text = "0";
            UpdateValueLabel(lblEgtValue, 0, "C");
            Send("ITT", 0);
        }

        private void butSendEgt_Click(object sender, EventArgs e)
        {
            SendManualValue(trkEgt, txtEgtInput, lblEgtValue, "ITT", "C");
        }

        private void txtEgtInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkEgt, txtEgtInput, lblEgtValue, "ITT", "C");
            }
        }

        // Compact rows for 8 more real-value gauges (EOT/EOP/XOT/XOP/TS/RS/GP/FA)
        // - no trackbar, just a value box + Send, since a full trackbar section
        // per gauge (like VSI/ALT/Radar ALT/EGT above) would push the form well
        // past a normal screen height. Each board-side conversion is the same
        // "uncalibrated linear scale" placeholder as EGT's - see
        // JET_RANGER_STEPPER_CONTROLLER.ino's setEOT()/setEOP()/etc.
        private void SendRealValue(TextBox textBox, string code)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                Send(code, value);
            }
            else
            {
                MessageBox.Show("Value must be a number");
            }
        }

        // Wire codes renamed to match JET_RANGER_SERVO_CONTROLLER.ino's existing
        // codes for these same real-world quantities (OILT/OILP/XMSNT/XMSNP/
        // RPME/RPMR/N1/FUEL) - control names here are unchanged (still
        // txtEot/txtEop/etc.), only the string handed to Send() changed.
        private void butSendEot_Click(object sender, EventArgs e) => SendRealValue(txtEot, "OILT");
        private void txtEot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtEot, "OILT"); }

        private void butSendEop_Click(object sender, EventArgs e) => SendRealValue(txtEop, "OILP");
        private void txtEop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtEop, "OILP"); }

        private void butSendXot_Click(object sender, EventArgs e) => SendRealValue(txtXot, "XMSNT");
        private void txtXot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtXot, "XMSNT"); }

        private void butSendXop_Click(object sender, EventArgs e) => SendRealValue(txtXop, "XMSNP");
        private void txtXop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtXop, "XMSNP"); }

        private void butSendTs_Click(object sender, EventArgs e) => SendRealValue(txtTs, "RPME");
        private void txtTs_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtTs, "RPME"); }

        private void butSendRs_Click(object sender, EventArgs e) => SendRealValue(txtRs, "RPMR");
        private void txtRs_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtRs, "RPMR"); }

        private void butSendGp_Click(object sender, EventArgs e) => SendRealValue(txtGp, "N1");
        private void txtGp_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtGp, "N1"); }

        private void butSendFa_Click(object sender, EventArgs e) => SendRealValue(txtFa, "FUEL");
        private void txtFa_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtFa, "FUEL"); }

        // IAS (Current Airspeed), 0-140kt: sends real knots directly; the
        // board converts to steps via setIAS()/iasKtToSteps() in
        // JET_RANGER_STEPPER_CONTROLLER.ino, reusing that stepper's
        // existing DCS-BIOS-derived step range rather than a fresh
        // placeholder. CAUTION: JET_RANGER_SERVO_CONTROLLER.ino's own
        // "IAS" code means something different (a pre-converted Bell 206
        // servo-position number, not knots) - see that sketch's comment on
        // the "IAS" case for the full caveat.
        private void trkIas_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblIasValue, trkIas.Value, "kt");
            txtIasInput.Text = trkIas.Value.ToString();
            Send("IAS", trkIas.Value);
        }

        private void butIasZero_Click(object sender, EventArgs e)
        {
            trkIas.Value = 0;
            txtIasInput.Text = "0";
            UpdateValueLabel(lblIasValue, 0, "kt");
            Send("IAS", 0);
        }

        private void butSendIas_Click(object sender, EventArgs e)
        {
            SendManualValue(trkIas, txtIasInput, lblIasValue, "IAS", "kt");
        }

        private void txtIasInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkIas, txtIasInput, lblIasValue, "IAS", "kt");
            }
        }
    }
}
