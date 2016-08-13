using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Net.Sockets;

namespace EncoderUDPTester
{
   

    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void label4_Click(object sender, EventArgs e)
        {

        }


        static void Sendit(string datapayload)
        {

            UdpClient udpClient = new UdpClient("127.0.0.1", 9902);

            Byte[] sendBytes = Encoding.ASCII.GetBytes(datapayload);

            try
            {
                udpClient.Send(sendBytes, sendBytes.Length);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Sendit("IN_LAND_ALT=1");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Sendit("IN_LAND_ALT=-1");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Sendit("IN_FLT_ALT=1");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            Sendit("IN_FLT_ALT=-1");
        }  

    }
 
}
