using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace F4GPSOut
{
    public partial class frmSelectComPort : Form
    {
        public frmSelectComPort()
        {
            InitializeComponent();
        }

        private void frmSelectComPort_Load(object sender, EventArgs e)
        {
            foreach (string s in SerialPort.GetPortNames())
            {
                listBox1.Items.Add(s);
            }  
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem != null)
                GPSOut_Main.GPSComPort = listBox1.SelectedItem.ToString();
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.GPSComPort = GPSOut_Main.GPSComPort;
            Properties.Settings.Default.Save();
            this.Dispose();
        }

        private void btnEnd_Click(object sender, EventArgs e)
        {
            Environment.Exit(1);
        }
    }
}
