using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.IO.Ports;
using System.Net;
using System.Threading;


namespace GPS_Test_Sender
{
    public partial class Form1 : Form
    {

        static string strAltitude = "Altitude=0";
        static string strHeading = "Heading=0";
        static string strLatitude = "";
        static string strLongitude = "";

        public Form1()
        {
            InitializeComponent();
        }




        private void timer1_Tick(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {


            for (int i = 0; i < 10; i++)
            {


                strAltitude = "Altitude=" + (i * 1000).ToString();
                SendDataToServer();


                WaitAWhile(100);
            }

            for (int i = 0; i < 36; i++)
            {


                strHeading  = "Heading=" + (i * 10).ToString();
                SendDataToServer();


                WaitAWhile(100);
            }

            strHeading = "Heading=0";
            SendDataToServer();

            for (int i = 0; i < 180; i++)
            {


                strLongitude = "Longitude=" + i.ToString();
                SendDataToServer();


                WaitAWhile(80);
            }

            strLongitude = "Longitude=10";
            SendDataToServer();

            for (int i = 0; i < 90; i++)
            {


                strLatitude = "Latitude=" + i.ToString();
                SendDataToServer();


                WaitAWhile(100);
            }

        }

        public void SendDataToServer()
        {

            byte[] data = new Byte[256];
            string strToSend = "";

            IPEndPoint RemoteEndPoint = new IPEndPoint(IPAddress.Loopback, 7777);
            Socket server = new Socket(AddressFamily.InterNetwork,
                                       SocketType.Dgram, ProtocolType.Udp);

            strToSend = strAltitude + ":" + strHeading + ":" +  strLongitude + ":" +  strLatitude  ;
            this.lblDataSent.Text = strToSend;
            this.Refresh();
            data = Encoding.ASCII.GetBytes(strToSend);
            server.SendTo(data, data.Length, SocketFlags.None, RemoteEndPoint);
        }

        public void WaitAWhile(int msToWait)
        {
            for (int i = 0; i < msToWait; i++)
            {

                Thread.Sleep(1);
                Application.DoEvents();
            }

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                Environment.Exit(0);
            }

            catch (Exception err)
            {
                MessageBox.Show(err.ToString());
            }

        }



    }
}
