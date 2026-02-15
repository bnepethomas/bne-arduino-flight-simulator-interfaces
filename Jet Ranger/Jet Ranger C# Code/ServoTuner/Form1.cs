using Microsoft.VisualBasic;
using System.Net.Sockets;
using System.Text;
using System.Windows.Input;

namespace ServoTuner
{
    public partial class frmMain : Form
    {


        string UDP_Playload;
        UdpClient frontPanelClient = new UdpClient();

        // Must be in the same order as the ServoShortCodes enum
        enum Servos
        {
            AirSpeed,
            VerticalSpeed,
            AttitudeIndicatorBankDegrees,
            AttitudeIndicatorPitchDegrees,
            RotorRpmPct1,
            GeneralEngPctMaxRpm1,
            EngTorquePercent1,
            ElectricalTotalLoadAmps,
            TurbEngItt1,
            EngOilTemperature1,
            FuelTotalQuantity,
            TurbEngCorrectedN11,
            EngOilPressure1,
            EngTransmissionPressure1,
            EngTransmissionTemperature1,
            PlaneAltAboveGround,
        }

        // Short codes for the servos, must be in the same order as the Servos enum
        enum ServoShortCodes
        {
            IAS,
            VSI,
            BANK,
            PITCH,
            RPMR,
            RPME,
            TQ,
            AMPS,
            ITT,
            OILT,
            FUEL,
            N1,
            OILP,
            XMSNP,
            XMSNT,
            AGL,
        }


        //                                    ASP  VSI  BNK  PCH  RPMR RPME TQ   AMPS ITT  OILT FUEL N1  OILP  XMNP XMNT AGL
        long[] ServMinPosition = new long[] { 173, 178, 444, 555, 177, 137, 176, 527, 121, 124, 159, 121,  82,   0,   0, 222 };
        long[] ServMaxPosition = new long[] {  10,  14, 444, 555,  23,   6,  37, 740, 802, 175,  51, 000,  34, 999, 999, 222 };
        long[] ServZeroPosition = new long[]{ 173,  93, 444, 555, 177, 137, 176, 527, 121, 124, 159, 121,  82,   0,   0, 222 };


        private void SendToFrontPanel(string message)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes(message);
            frontPanelClient.Send(sendBytes, sendBytes.Length);
        }

        private long Mapper(long value, long fromLow, long fromHigh, long toLow, long toHigh)
        {
            /// <summary>
            /// Re-maps a number from one range to another, similar to the Arduino map() function.
            /// </summary>
            /// <param name="value">The input value to map.</param>
            /// <param name="fromLow">The lower bound of the value's current range.</param>
            /// <param name="fromHigh">The upper bound of the value's current range.</param>
            /// <param name="toLow">The lower bound of the value's target range.</param>
            /// <param name="toHigh">The upper bound of the value's target range.</param>
            /// <returns>The mapped value in the target range.</returns>


            // The formula for the Arduino map() function
            // Need to check how this handles the from being higher than the to range
            // and whether it handles values outside the from range.
            // It should handle both of those cases but need to test that.
            return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;

        }

        private void UpdateLabels()
        {
            lblShortCode.Text = $"{((ServoShortCodes)lstServos.SelectedIndex).ToString()}";
            lblMinValue.Text = $"Min: {ServMinPosition[lstServos.SelectedIndex]}";
            lblMaxValue.Text = $"Max: {ServMaxPosition[lstServos.SelectedIndex]}";
        }

        private void SendManualValue()

        {

            // Need to check if the value is within the min and max for the selected servo before sending it to the front panel
            long localMin = 0;
            long localMax = 1000;
            if (ServMinPosition[lstServos.SelectedIndex] > ServMaxPosition[lstServos.SelectedIndex])
            {
                localMax = ServMinPosition[lstServos.SelectedIndex];
                localMin = ServMaxPosition[lstServos.SelectedIndex];
            }
            else
            {
                localMin = ServMinPosition[lstServos.SelectedIndex];
                localMax = ServMaxPosition[lstServos.SelectedIndex];
            }


            if (long.TryParse(txtRawInput.Text, out long newValue))
            {
                if (newValue >= localMin && newValue <= localMax)
                {
                    SendToFrontPanel("D," + lblShortCode.Text + ":" + newValue.ToString());
                }
                else
                {
                    MessageBox.Show($"Value must be between {localMin} and {localMax}");
                }
            }
        }

        private void SendConvertedValue()
        {
            // This is for testing the conversion of the input value to the corresponding value
            // for the front panel, without sending it to the front panel

            if (long.TryParse(txtConvertedInput.Text, out long newValue))
            {
                int valueToSend = 0;
                if (lblShortCode.Text == "TQ") valueToSend = TQ_Process(newValue);
                if (lblShortCode.Text == "IAS") valueToSend = IAS_Process(newValue);
                if (lblShortCode.Text == "RPMR") valueToSend = RPMR_Process(newValue);
                if (lblShortCode.Text == "RPME") valueToSend = RPME_Process(newValue);
                if (lblShortCode.Text == "FUEL") valueToSend = FUEL_Process(newValue);
                if (lblShortCode.Text == "VSI") valueToSend = VSI_Process(newValue);
                if (lblShortCode.Text == "OILP") valueToSend = OILP_Process(newValue);
                if (lblShortCode.Text == "OILT") valueToSend = OILt_Process(newValue);  


                lblConvertedValue.Text = $"Converted Value: {valueToSend}";
                SendToFrontPanel("D," + lblShortCode.Text + ":" + valueToSend.ToString());
            }
            else
            {
                MessageBox.Show($"Value must be a number");

            }
        }

        private int TQ_Process(long newValue)
        {
            // If the short code is TQ then we need to convert the value from a percentage to the corresponding value for the front panel
            // The front panel expects a value between 37 and 176 for the torque servo, which corresponds to 0% and 100% respectively.
            // So we need to map the input value (0-100) to the range of 37-176.
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[lstServos.SelectedIndex], ServMaxPosition[lstServos.SelectedIndex]);
            return mappedvalue;
        }

        private int IAS_Process(long newValue)
        {
            // 0  - 173
            // 20 - 165
            // 40 - 142
            // 60 - 115
            // 80 - 84
            // 90 - 74
            //100 - 55
            //110 - 50
            //120 - 40
            //130 - 29
            //140 - 22
            //150 - 10
            switch (newValue)
            {
                case <= 20:
                    return (int)Mapper(newValue, 0, 20, 173, 165);
                case <= 40:
                    return (int)Mapper(newValue, 20, 40, 165, 142);
                case <= 60:
                    return (int)Mapper(newValue, 40, 60, 142, 115);
                case <=80:
                    return (int)Mapper(newValue, 60, 80, 115, 84);
                case <=90:
                    return (int)Mapper(newValue, 80, 90, 84, 74);
                case <=100:
                    return (int)Mapper(newValue, 90, 100, 74, 55);
                case <=110:
                    return (int)Mapper(newValue, 100, 110, 55, 50);
                case <= 120:
                    return (int)Mapper(newValue, 110, 120, 50, 40);
                case <=130:
                    return (int)Mapper(newValue, 120, 130, 40, 29);
                case <=140:
                    return (int)Mapper(newValue, 130, 140, 29, 22);
                case <= 150:
                    return (int)Mapper(newValue, 140, 150, 22, 10);
                case > 150:
                    return 10;

                default:
                    int mappedvalue = (int)Mapper(newValue, 0, 150,
                        ServMinPosition[(uint)(Servos.AirSpeed)], ServMaxPosition[(uint)(Servos.AirSpeed)]);
                    return mappedvalue;
            }
        }

        private int RPMR_Process(long newValue)
        {

            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.RotorRpmPct1)], ServMaxPosition[(uint)(Servos.RotorRpmPct1)]);
            return mappedvalue;
        }

        private int RPME_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 120,
                ServMinPosition[(uint)(Servos.GeneralEngPctMaxRpm1)], ServMaxPosition[(uint)(Servos.GeneralEngPctMaxRpm1)]);
            return mappedvalue;
        }

        private int FUEL_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 75,
                ServMinPosition[(uint)(Servos.FuelTotalQuantity)], ServMaxPosition[(uint)(Servos.FuelTotalQuantity)]);
            return mappedvalue;
        }

        private int VSI_Process(long newValue)
        {
            //-6000 - 178
            //-4000 - 161
            //-2000 - 134
            //-1000 - 115
            // 0    - 93
            // 1000 - 70
            // 2000 - 52
            // 4000 - 28
            // 6000 - 15
            switch (newValue)
            {
                case <= -6000:
                    return 178;
                case <= -4000:
                    return (int)Mapper(newValue, -6000, -4000, 178, 161);
                case <= -2000:
                    return (int)Mapper(newValue, -4000, -2000, 161, 134);
                case <= -1000:
                    return (int)Mapper(newValue, -2000, -1000, 134, 115);
                case <= 0:
                    return (int)Mapper(newValue, -1000, 0, 115, 93);
                case <= 1000:
                    return (int)Mapper(newValue, 0, 1000, 93, 70);
                case <= 2000:
                    return (int)Mapper(newValue, 1000, 2000, 70, 52);
                case <= 4000:
                    return (int)Mapper(newValue, 2000, 4000, 52, 28);
                case <= 6000:
                    return (int)Mapper(newValue, 4000, 6000, 28, 15);
                    case > 6000:
                    return 15;

                default:
                    return 93;
            }

            
        }

        private int OILP_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilPressure1)], ServMaxPosition[(uint)(Servos.EngOilPressure1)]);
            return mappedvalue;
        }
        private int OILt_Process(long newValue)
        {
            int mappedvalue = (int)Mapper(newValue, 0, 150,
                ServMinPosition[(uint)(Servos.EngOilTemperature1)], ServMaxPosition[(uint)(Servos.EngOilTemperature1)]);
            return mappedvalue;
        }


        public frmMain()
        {
            InitializeComponent();

            Array servoArray = Enum.GetValues(typeof(Servos));
            foreach (Servos servoName in servoArray)
            {
                lstServos.Items.Add($"{servoName}");
            }
            lstServos.SelectedItem = "EngTorquePercent1";
            UpdateLabels();

            frontPanelClient.Connect("172.16.1.102", 13136);
        }

        private void vScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            // Need to check if the value is within the min and max for the selected servo before sending it to the front panel
            long localMin = 0;
            long localMax = 1000;
            if (ServMinPosition[lstServos.SelectedIndex] > ServMaxPosition[lstServos.SelectedIndex])
            {
                localMax = ServMinPosition[lstServos.SelectedIndex];
                localMin = ServMaxPosition[lstServos.SelectedIndex];
            }
            else
            {
                localMin = ServMinPosition[lstServos.SelectedIndex];
                localMax = ServMaxPosition[lstServos.SelectedIndex];
            }

            if (e.NewValue >= localMin && e.NewValue <= localMax)
            {
                lblMinValue.Text = $"Min: {ServMinPosition[lstServos.SelectedIndex]}";
                lblMaxValue.Text = $"Max: {ServMaxPosition[lstServos.SelectedIndex]}";
                this.label1.Text = "Value: " + e.NewValue.ToString();
                SendToFrontPanel("D," + lblShortCode.Text + ":" + e.NewValue.ToString());
            }


        }

        private void Form1_Load(object sender, EventArgs e)
        {

            this.vScrollBar1.Value = 120;
            this.label1.Text = "Value: " + this.vScrollBar1.Value.ToString();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            UDP_Playload = "D,";
            //UDP_Playload += this.vScrollBar1.Value.ToString();
            UDP_Playload += ",TQ:" + 150;
            SendToFrontPanel(UDP_Playload);

        }

        private void lstServos_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateLabels();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SendManualValue();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SendConvertedValue();
        }


        private void txtConvertedInput_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Enter:
                    {
                        SendConvertedValue();
                        break;
                    }
                default:
                    break;
            }
        }

        private void txtConvertedInput_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
