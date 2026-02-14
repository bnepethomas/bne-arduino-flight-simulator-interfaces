using Microsoft.VisualBasic;
using System.Net.Sockets;
using System.Text;

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
        long[] ServMinPosition = new long[] { 44, 10, 444, 555, 28, 242, 176, 527, 121, 310, 124, 121, 560, 9, 424, 222 };
        long[] ServMaxPosition = new long[] { 955, 952, 444, 555, 895, 986, 37, 740, 802, 20, 736, 000, 864, 288, 107, 222 };
        long[] ServZeroPosition = new long[] { 44, 498, 444, 555, 28, 242, 176, 527, 121, 310, 124, 121, 560, 9, 424, 222 };


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
            if (long.TryParse(txtConvertedInput.Text, out long newValue))
            {
                int mappedvalue = (int)Mapper(newValue, 0, 120,
                    ServMinPosition[lstServos.SelectedIndex], ServMaxPosition[lstServos.SelectedIndex]);
                lblConvertedValue.Text = $"Converted Value: {mappedvalue}";
                SendToFrontPanel("D," + lblShortCode.Text + ":" + mappedvalue.ToString());
            }

        }

        private void label6_Click(object sender, EventArgs e)
        {

        }

        private void txtConvertedInput_Enter(object sender, EventArgs e)
        {

        }


    }
}
