using System.Net.Sockets;
using System.Text;

namespace StepperVSITester
{
    public partial class frmMain : Form
    {
        // JET_RANGER_STEPPER_CONTROLLER.ino's MSFSport - this tool stands in
        // for FSUIPCWinformsAutoCS's stepperClient while testing the
        // stepper board's "VSI" UDP handler in isolation, without needing
        // FSUIPC/a flight sim running.
        UdpClient stepperClient = new UdpClient();

        private void SendVsiFpm(long fpm)
        {
            byte[] sendBytes = Encoding.ASCII.GetBytes("D,VSI:" + fpm.ToString());
            stepperClient.Send(sendBytes, sendBytes.Length);
        }

        private void SendManualValue()
        {
            if (long.TryParse(txtRawInput.Text, out long fpm))
            {
                if (fpm >= trkVsi.Minimum && fpm <= trkVsi.Maximum)
                {
                    trkVsi.Value = (int)fpm;
                    UpdateValueLabel(fpm);
                    SendVsiFpm(fpm);
                }
                else
                {
                    MessageBox.Show($"Value must be between {trkVsi.Minimum} and {trkVsi.Maximum}");
                }
            }
            else
            {
                MessageBox.Show("Value must be a number");
            }
        }

        private void UpdateValueLabel(long fpm)
        {
            lblValue.Text = $"Value: {fpm} fpm";
        }

        public frmMain()
        {
            InitializeComponent();

            stepperClient.Connect("172.16.1.105", 13136);

            trkVsi.Value = 0;
            UpdateValueLabel(0);
            txtRawInput.Text = "0";
        }

        private void trkVsi_Scroll(object sender, EventArgs e)
        {
            UpdateValueLabel(trkVsi.Value);
            txtRawInput.Text = trkVsi.Value.ToString();
            SendVsiFpm(trkVsi.Value);
        }

        private void butZero_Click(object sender, EventArgs e)
        {
            trkVsi.Value = 0;
            txtRawInput.Text = "0";
            UpdateValueLabel(0);
            SendVsiFpm(0);
        }

        private void butSendRaw_Click(object sender, EventArgs e)
        {
            SendManualValue();
        }

        private void txtRawInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SendManualValue();
            }
        }
    }
}
