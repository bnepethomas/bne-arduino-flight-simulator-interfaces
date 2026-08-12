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
            txtGp.Text = "0";
            txtFa.Text = "0";

            trkIas.Value = 0;
            UpdateValueLabel(lblIasValue, 0, "kt");
            txtIasInput.Text = "0";

            trkRpme.Value = 0;
            UpdateValueLabel(lblRpmeValue, 0, "%");
            txtRpmeInput.Text = "0";

            trkRpmr.Value = 0;
            UpdateValueLabel(lblRpmrValue, 0, "%");
            txtRpmrInput.Text = "0";
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
        // board's side - same pattern as ALT above. Both sketches now treat
        // "AGL" as calibrated feet (previously only Jet_Ranger_Driver_Test.ino
        // did; JET_RANGER_STEPPER_CONTROLLER.ino was raw steps until its own
        // AGL_FT_TABLE was added), so this slider produces real feet
        // regardless of which sketch is flashed - but the two sketches use
        // separate, differently-measured tables (RADAR_ALT_FT_TABLE vs
        // AGL_FT_TABLE), so the same typed feet value may land the needle
        // at a different physical position depending on which is on the
        // board. Raw-step testing for this gauge now goes through the
        // "AGLRAW" entry in the Raw Step Test dropdown instead of this
        // control. Wire code renamed from "RALT" to "AGL" to match both
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
        //  - AGLRAW/IASRAW/ALTRAW/VSIRAW/OILTRAW/OILPRAW/XMSNTRAW/XMSNPRAW/ITTRAW/RPMERAW/
        //    RPMRRAW/N1RAW/FUELRAW: distinct raw-step siblings of gauges that DO have
        //    a real-unit code/section elsewhere in this form (the Radar ALT/IAS/ALT/VSI/
        //    RPME/RPMR trackbars, the EGT/ITT trackbar, the OILT/OILP/XMSNT/XMSNP/N1/FUEL
        //    rows) - lets the operator bypass that gauge's unit conversion for bench
        //    testing without losing the real-value control.
        // One shared control with a dropdown rather than a duplicate raw-step
        // trackbar/row next to every calibrated gauge - cheaper to keep in sync, and
        // easy to give a gauge its own dedicated raw control later if that turns out
        // to be worth the extra screen space.
        // Fallback for when nothing is selected yet - "EOT" doesn't exist in
        // this dropdown any more (it graduated to its own row), so this
        // matches cboNewGauge's actual first item instead of a stale one.
        private string SelectedNewGaugeCode() => cboNewGauge.SelectedItem?.ToString() ?? "TQ";

        private void butNewGaugeSend_Click(object sender, EventArgs e)
        {
            if (!long.TryParse(txtNewGaugeSteps.Text, out long steps))
            {
                MessageBox.Show("Steps must be a whole number (e.g. 150 or -150)");
                return;
            }

            Send(SelectedNewGaugeCode(), steps);
        }

        private void butNewGaugeZero_Click(object sender, EventArgs e)
        {
            txtNewGaugeSteps.Text = "0";
            Send(SelectedNewGaugeCode(), 0);
        }

        private void txtNewGaugeSteps_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                butNewGaugeSend_Click(sender, e);
            }
        }

        // Single-step jog buttons: since every raw code here is an absolute
        // .moveTo() target on the board (not a relative move), "one step"
        // means nudging the textbox's own value by +/-1 and resending it -
        // only meaningful as a true single step if the stepper actually
        // reached the last target sent (AccelStepper's acceleration ramp
        // takes some time), so give it a moment between clicks.
        private void StepNewGauge(long delta)
        {
            if (!long.TryParse(txtNewGaugeSteps.Text, out long steps))
            {
                steps = 0;
            }

            steps += delta;
            txtNewGaugeSteps.Text = steps.ToString();
            Send(SelectedNewGaugeCode(), steps);
        }

        private void butNewGaugeStepBack_Click(object sender, EventArgs e) => StepNewGauge(-1);
        private void butNewGaugeStepFwd_Click(object sender, EventArgs e) => StepNewGauge(1);

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

        // Compact rows for 6 more real-value gauges (EOT/EOP/XOT/XOP/GP/FA) -
        // no trackbar, just a value box + Send, since a full trackbar section
        // per gauge (like VSI/ALT/Radar ALT/EGT above) would push the form well
        // past a normal screen height. Each board-side conversion is still the
        // same "uncalibrated linear scale" placeholder as EGT's - see
        // JET_RANGER_STEPPER_CONTROLLER.ino's setEOT()/setEOP()/etc. TS
        // ("RPME")/RS ("RPMR") used to be compact rows here too, but graduated
        // to full trackbar sections (below, near IAS) once real hand-measured
        // TS_PCT_TABLE/RS_PCT_TABLE calibration existed for them - same
        // "graduates once calibration is known" pattern EGT/IAS followed
        // earlier.
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
        // N1/FUEL) - control names here are unchanged (still txtEot/txtEop/
        // etc.), only the string handed to Send() changed. RPME/RPMR followed
        // this same rename but have since graduated to their own trackbar
        // sections below (near IAS) - see SendRealValue's callers here for
        // the remaining six.
        private void butSendEot_Click(object sender, EventArgs e) => SendRealValue(txtEot, "OILT");
        private void txtEot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtEot, "OILT"); }

        private void butSendEop_Click(object sender, EventArgs e) => SendRealValue(txtEop, "OILP");
        private void txtEop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtEop, "OILP"); }

        private void butSendXot_Click(object sender, EventArgs e) => SendRealValue(txtXot, "XMSNT");
        private void txtXot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtXot, "XMSNT"); }

        private void butSendXop_Click(object sender, EventArgs e) => SendRealValue(txtXop, "XMSNP");
        private void txtXop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValue(txtXop, "XMSNP"); }

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

        // RPME (Turbine/Engine Speed), 0-117%: sends real percent directly;
        // the board converts to steps via setTS()/tsPctToSteps() and its
        // real hand-measured TS_PCT_TABLE in JET_RANGER_STEPPER_CONTROLLER.ino.
        // Graduated here from a compact value-box row (see above) once that
        // table existed - same trackbar layout as IAS/EGT.
        private void trkRpme_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRpmeValue, trkRpme.Value, "%");
            txtRpmeInput.Text = trkRpme.Value.ToString();
            Send("RPME", trkRpme.Value);
        }

        private void butRpmeZero_Click(object sender, EventArgs e)
        {
            trkRpme.Value = 0;
            txtRpmeInput.Text = "0";
            UpdateValueLabel(lblRpmeValue, 0, "%");
            Send("RPME", 0);
        }

        private void butSendRpme_Click(object sender, EventArgs e)
        {
            SendManualValue(trkRpme, txtRpmeInput, lblRpmeValue, "RPME", "%");
        }

        private void txtRpmeInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkRpme, txtRpmeInput, lblRpmeValue, "RPME", "%");
            }
        }

        // RPMR (Rotor Speed), 0-117%: sends real percent directly; the board
        // converts to steps via setRS()/rsPctToSteps() and its real
        // hand-measured RS_PCT_TABLE in JET_RANGER_STEPPER_CONTROLLER.ino.
        // Graduated here from a compact value-box row (see above) once that
        // table existed - same trackbar layout as IAS/EGT/RPME.
        private void trkRpmr_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRpmrValue, trkRpmr.Value, "%");
            txtRpmrInput.Text = trkRpmr.Value.ToString();
            Send("RPMR", trkRpmr.Value);
        }

        private void butRpmrZero_Click(object sender, EventArgs e)
        {
            trkRpmr.Value = 0;
            txtRpmrInput.Text = "0";
            UpdateValueLabel(lblRpmrValue, 0, "%");
            Send("RPMR", 0);
        }

        private void butSendRpmr_Click(object sender, EventArgs e)
        {
            SendManualValue(trkRpmr, txtRpmrInput, lblRpmrValue, "RPMR", "%");
        }

        private void txtRpmrInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue(trkRpmr, txtRpmrInput, lblRpmrValue, "RPMR", "%");
            }
        }
    }
}
