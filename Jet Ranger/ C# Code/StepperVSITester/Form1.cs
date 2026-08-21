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

        // JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's own address (172.16.1.106,
        // distinct from the single-board sketch's 172.16.1.105 so both can be
        // on the network at once) - a separate socket since this is a
        // different physical board, not an alternate port on the same one.
        // Only used for that board's Fuel Load / Electrical Load gauges
        // (FUELLOAD/ELECTRICALLOAD), which don't exist on the single-board
        // sketch at all.
        UdpClient dualStepperClient = new UdpClient();

        private void Send(string code, long value)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D," + code + ":" + value.ToString());
            stepperClient.Send(sendBytes, sendBytes.Length);
        }

        private void SendDual(string code, long value)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D," + code + ":" + value.ToString());
            dualStepperClient.Send(sendBytes, sendBytes.Length);
        }

        // double overloads - only used by RPME/RPMR (see their handlers
        // below) so those two match FSUIPCWinformsAutoCS's own one-decimal
        // wire format (e.g. "82.0"/"82.4"), instead of every other code's
        // bare integer. Calls passing an int/long still resolve to the
        // long overloads above (int->long is a better implicit conversion
        // than int->double), so this doesn't change anything for ALT or
        // any other existing caller.
        private void Send(string code, double value)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D," + code + ":" + value.ToString("F1"));
            stepperClient.Send(sendBytes, sendBytes.Length);
        }

        private void SendDual(string code, double value)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D," + code + ":" + value.ToString("F1"));
            dualStepperClient.Send(sendBytes, sendBytes.Length);
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

        // Same as SendManualValue() above, but also broadcasts to
        // dualStepperClient (172.16.1.106) - used for codes both boards
        // respond to: RPME/RPMR (each board has its own separate
        // TS_PCT_TABLE/RS_PCT_TABLE, but the same UDP codes reach both)
        // and ALT (both boards' ALTstepper responds to "ALT" - see
        // JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's now-enabled "ALT"
        // case).
        private void SendManualValueBroadcast(TrackBar trackBar, TextBox textBox, Label label, string code, string unit)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                if (value >= trackBar.Minimum && value <= trackBar.Maximum)
                {
                    trackBar.Value = (int)value;
                    UpdateValueLabel(label, value, unit);
                    Send(code, value);
                    SendDual(code, value);
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

        // Same as SendManualValueBroadcast() above, but sends the value
        // with a single decimal place instead of a bare integer (e.g.
        // "82.0") via the double Send()/SendDual() overloads above - only
        // used by RPME/RPMR's Send button/Enter handlers below, so their
        // wire format matches FSUIPCWinformsAutoCS's own one-decimal
        // RPMR/RPME. The trackbar itself is still whole-percent only
        // (TrackBar has no fractional granularity) - this only affects
        // what's put on the wire, not what the operator can dial in here.
        private void SendPercentManualValueBroadcast(TrackBar trackBar, TextBox textBox, Label label, string code, string unit)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                if (value >= trackBar.Minimum && value <= trackBar.Maximum)
                {
                    trackBar.Value = (int)value;
                    UpdateValueLabel(label, value, unit);
                    Send(code, (double)value);
                    SendDual(code, (double)value);
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
            dualStepperClient.Connect("172.16.1.106", 13136);

            txtFuelLoad.Text = "0";
            txtElectricalLoad.Text = "0";

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

        // Broadcasts to both stepperClient (172.16.1.105) and
        // dualStepperClient (172.16.1.106) - JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's
        // ALTstepper was revived and its "ALT"/"ALTRAW" UDP cases enabled
        // specifically so this board could be driven the same way as the
        // single-board sketch (previously it was DCS-BIOS-only, with no
        // UDP path at all).
        private void trkAlt_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblAltValue, trkAlt.Value, "ft");
            txtAltInput.Text = trkAlt.Value.ToString();
            Send("ALT", trkAlt.Value);
            SendDual("ALT", trkAlt.Value);
        }

        private void butAltZero_Click(object sender, EventArgs e)
        {
            trkAlt.Value = 0;
            txtAltInput.Text = "0";
            UpdateValueLabel(lblAltValue, 0, "ft");
            Send("ALT", 0);
            SendDual("ALT", 0);
        }

        private void butSendAlt_Click(object sender, EventArgs e)
        {
            SendManualValueBroadcast(trkAlt, txtAltInput, lblAltValue, "ALT", "ft");
        }

        private void txtAltInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValueBroadcast(trkAlt, txtAltInput, lblAltValue, "ALT", "ft");
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
        //    RPMRRAW/N1RAW/FUELRAW/FUELLOADRAW/ELECTRICALLOADRAW: distinct raw-step
        //    siblings of gauges that DO have a real-unit code/section elsewhere in
        //    this form (the Radar ALT/IAS/ALT/VSI/RPME/RPMR trackbars, the EGT/ITT
        //    trackbar, the OILT/OILP/XMSNT/XMSNP/N1/FUEL rows, and the FUELLOAD/
        //    ELECTRICALLOAD rows below) - lets the operator bypass that gauge's unit
        //    conversion for bench testing without losing the real-value control.
        // One shared control with a dropdown rather than a duplicate raw-step
        // trackbar/row next to every calibrated gauge - cheaper to keep in sync, and
        // easy to give a gauge its own dedicated raw control later if that turns out
        // to be worth the extra screen space.
        // Broadcasts to both stepperClient (172.16.1.105) and
        // dualStepperClient (172.16.1.106) - JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino
        // accepts every one of these codes too (it's a fork of the same
        // sketch), except AGLRAW/TQ, which no longer exist there (that
        // board's Radar Alt/Torque steppers were repurposed as Fuel
        // Load/Electrical Load - see FUELLOAD/ELECTRICALLOAD below) and are
        // silent no-ops on it, same as FLAPS/AOA/GFORCE/SPDMAX already are
        // on both boards. FUELLOADRAW/ELECTRICALLOADRAW are the mirror image -
        // they only exist on 172.16.1.106 (FuelLoadStepper/ElectricalLoadStepper
        // aren't declared on the single-board sketch at all), so they're silent
        // no-ops on 172.16.1.105.
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
            SendDual(SelectedNewGaugeCode(), steps);
        }

        private void butNewGaugeZero_Click(object sender, EventArgs e)
        {
            txtNewGaugeSteps.Text = "0";
            Send(SelectedNewGaugeCode(), 0);
            SendDual(SelectedNewGaugeCode(), 0);
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
            SendDual(SelectedNewGaugeCode(), steps);
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
        // same "uncalibrated linear scale" placeholder as EGT's for GP/FA - see
        // JET_RANGER_STEPPER_CONTROLLER.ino's setGP()/setFA()/etc. EOT/EOP/XOT/XOP
        // all graduated to real calibration (see SendRealValueBroadcast() below)
        // once JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's EOT_C_TABLE/
        // EOP_PSI_TABLE/XOT_C_TABLE/XOP_PSI_TABLE existed. TS ("RPME")/RS ("RPMR")
        // used to be compact rows here too, but graduated to full trackbar
        // sections (below, near IAS) once real hand-measured TS_PCT_TABLE/
        // RS_PCT_TABLE calibration existed for them - same "graduates once
        // calibration is known" pattern EGT/IAS followed earlier.
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

        // Same as SendRealValue() above, but also broadcasts to
        // dualStepperClient (172.16.1.106) - used for OILT/OILP/XMSNT/XMSNP
        // now that both JET_RANGER_STEPPER_CONTROLLER.ino (172.16.1.105,
        // still the placeholder linear scale) AND
        // JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino (172.16.1.106, real
        // EOT_C_TABLE/EOP_PSI_TABLE/XOT_C_TABLE/XOP_PSI_TABLE calibration -
        // see that sketch's PROGRAM_SUMMARY.md) have their own EOTstepper/
        // EOPstepper/XOTstepper/XOPstepper responding to these codes. See
        // SendManualValueBroadcast() above for the same broadcast reasoning
        // applied to ALT/RPME/RPMR.
        private void SendRealValueBroadcast(TextBox textBox, string code)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                Send(code, value);
                SendDual(code, value);
            }
            else
            {
                MessageBox.Show("Value must be a number");
            }
        }

        // Same as SendRealValue() above, but sent to dualStepperClient
        // (172.16.1.106) instead of stepperClient (172.16.1.105) - for
        // JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's FUELLOAD (PSI)/
        // ELECTRICALLOAD (%) codes, which don't exist on the single-board
        // sketch. Was named SendDualRawValue - renamed now that both codes
        // take real units (FUEL_LOAD_PSI_TABLE/ELECTRICAL_LOAD_PCT_TABLE on
        // the board side) rather than raw steps; this method itself never
        // did any unit conversion either way, so its behaviour is
        // unchanged.
        private void SendDualValue(TextBox textBox, string code)
        {
            if (long.TryParse(textBox.Text, out long value))
            {
                SendDual(code, value);
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
        // the remaining two (N1/FUEL). OILT/OILP/XMSNT/XMSNP all use
        // SendRealValueBroadcast() instead of SendRealValue() (below) now
        // that JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino has its own
        // calibrated EOT/EOP/XOT/XOP (EOT_C_TABLE/EOP_PSI_TABLE/
        // XOT_C_TABLE/XOP_PSI_TABLE) alongside JET_RANGER_STEPPER_CONTROLLER.ino's
        // own (still placeholder) EOT/EOP/XOT/XOP.
        private void butSendEot_Click(object sender, EventArgs e) => SendRealValueBroadcast(txtEot, "OILT");
        private void txtEot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValueBroadcast(txtEot, "OILT"); }

        private void butSendEop_Click(object sender, EventArgs e) => SendRealValueBroadcast(txtEop, "OILP");
        private void txtEop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValueBroadcast(txtEop, "OILP"); }

        private void butSendXot_Click(object sender, EventArgs e) => SendRealValueBroadcast(txtXot, "XMSNT");
        private void txtXot_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValueBroadcast(txtXot, "XMSNT"); }

        private void butSendXop_Click(object sender, EventArgs e) => SendRealValueBroadcast(txtXop, "XMSNP");
        private void txtXop_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendRealValueBroadcast(txtXop, "XMSNP"); }

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
        // table existed - same trackbar layout as IAS/EGT. Broadcasts to
        // both stepperClient (172.16.1.105) and dualStepperClient
        // (172.16.1.106) - see SendManualValueBroadcast() above for why.
        private void trkRpme_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRpmeValue, trkRpme.Value, "%");
            txtRpmeInput.Text = trkRpme.Value.ToString();
            Send("RPME", (double)trkRpme.Value);
            SendDual("RPME", (double)trkRpme.Value);
        }

        private void butRpmeZero_Click(object sender, EventArgs e)
        {
            trkRpme.Value = 0;
            txtRpmeInput.Text = "0";
            UpdateValueLabel(lblRpmeValue, 0, "%");
            Send("RPME", 0.0);
            SendDual("RPME", 0.0);
        }

        private void butSendRpme_Click(object sender, EventArgs e)
        {
            SendPercentManualValueBroadcast(trkRpme, txtRpmeInput, lblRpmeValue, "RPME", "%");
        }

        private void txtRpmeInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendPercentManualValueBroadcast(trkRpme, txtRpmeInput, lblRpmeValue, "RPME", "%");
            }
        }

        // RPMR (Rotor Speed), 0-117%: sends real percent directly; the board
        // converts to steps via setRS()/rsPctToSteps() and its real
        // hand-measured RS_PCT_TABLE in JET_RANGER_STEPPER_CONTROLLER.ino.
        // Graduated here from a compact value-box row (see above) once that
        // table existed - same trackbar layout as IAS/EGT/RPME. Broadcasts
        // to both stepperClient (172.16.1.105) and dualStepperClient
        // (172.16.1.106) - see SendManualValueBroadcast() above for why.
        private void trkRpmr_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(lblRpmrValue, trkRpmr.Value, "%");
            txtRpmrInput.Text = trkRpmr.Value.ToString();
            Send("RPMR", (double)trkRpmr.Value);
            SendDual("RPMR", (double)trkRpmr.Value);
        }

        private void butRpmrZero_Click(object sender, EventArgs e)
        {
            trkRpmr.Value = 0;
            txtRpmrInput.Text = "0";
            UpdateValueLabel(lblRpmrValue, 0, "%");
            Send("RPMR", 0.0);
            SendDual("RPMR", 0.0);
        }

        private void butSendRpmr_Click(object sender, EventArgs e)
        {
            SendPercentManualValueBroadcast(trkRpmr, txtRpmrInput, lblRpmrValue, "RPMR", "%");
        }

        private void txtRpmrInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendPercentManualValueBroadcast(trkRpmr, txtRpmrInput, lblRpmrValue, "RPMR", "%");
            }
        }

        // Dual Stepper (172.16.1.106) real-value test rows - FUELLOAD (PSI)/
        // ELECTRICALLOAD (%) only exist on JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino
        // (that board's Radar Alt/Torque steppers repurposed as Fuel Load/
        // Electrical Load - see that sketch's PROGRAM_SUMMARY.md), so these
        // go through SendDual()/dualStepperClient rather than the
        // single-board Send()/stepperClient every other control here uses.
        // Now that both codes have real calibration (FUEL_LOAD_PSI_TABLE/
        // ELECTRICAL_LOAD_PCT_TABLE on the board), raw-step access is still
        // available via the Raw Step Test dropdown's FUELLOADRAW/
        // ELECTRICALLOADRAW entries below, same as every other calibrated
        // gauge's "<CODE>RAW" sibling.
        private void butSendFuelLoad_Click(object sender, EventArgs e) => SendDualValue(txtFuelLoad, "FUELLOAD");
        private void txtFuelLoad_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendDualValue(txtFuelLoad, "FUELLOAD"); }

        private void butSendElectricalLoad_Click(object sender, EventArgs e) => SendDualValue(txtElectricalLoad, "ELECTRICALLOAD");
        private void txtElectricalLoad_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendDualValue(txtElectricalLoad, "ELECTRICALLOAD"); }

        // Clock OLED (172.16.1.106) manual test - hour/minute boxes are
        // combined into the HHMM-encoded "ZULU" wire format (e.g. 1430 for
        // 14:30) JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino's
        // onZuluTimeChange() expects, then sent via SendDual() the same way
        // as the FUELLOAD/ELECTRICALLOAD rows above (this board only, no
        // equivalent on the single-board Stepper Controller).
        private void SendClockValue()
        {
            if (!int.TryParse(txtClockHour.Text, out int hours) || hours < 0 || hours > 23)
            {
                MessageBox.Show("Hour must be 0-23");
                return;
            }
            if (!int.TryParse(txtClockMinute.Text, out int minutes) || minutes < 0 || minutes > 59)
            {
                MessageBox.Show("Minute must be 0-59");
                return;
            }
            SendDual("ZULU", (hours * 100) + minutes);
        }

        private void butSendClock_Click(object sender, EventArgs e) => SendClockValue();
        private void txtClock_KeyDown(object sender, KeyEventArgs e) { if (e.KeyCode == Keys.Enter) SendClockValue(); }
    }
}
