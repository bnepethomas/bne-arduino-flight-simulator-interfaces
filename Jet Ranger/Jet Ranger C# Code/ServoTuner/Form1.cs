

using System;
using System.Diagnostics.Eventing.Reader;
using System.Drawing;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ServoTuner
{
    public partial class Form1 : Form
    {


        string UDP_Playload;
        UdpClient frontPanelClient = new UdpClient();

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


        private void SendToFrontPanel(string message)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes(message);
            frontPanelClient.Send(sendBytes, sendBytes.Length);
        }

        public Form1()
        {
            InitializeComponent();

            Array servoArray = Enum.GetValues(typeof(Servos));
            foreach (Servos servoName in servoArray)
            {
                lstServos.Items.Add($"{servoName}");
            }
            lstServos.SelectedItem = "EngTorquePercent1";

            frontPanelClient.Connect("172.16.1.102", 13136);
            SendToFrontPanel("D,TQ:150");
        }

        private void vScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            this.label1.Text = "Value: " + e.NewValue.ToString();
            SendToFrontPanel("D,TQ:" + e.NewValue.ToString());

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
    }
}
