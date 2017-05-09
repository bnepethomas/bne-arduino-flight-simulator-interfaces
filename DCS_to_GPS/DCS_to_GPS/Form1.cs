using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;
using System.Net;


namespace DCS_to_GPS
{


    public partial class Form1 : Form
    {
        public static bool messageReceived = false;
        public Form1()
        {
           
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int i = 0;
            UdpClient receivingUdpClient = new UdpClient(7783);
            Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            while (true)
            {

                {


                    // Blocks until a message returns on this socket from a remote host.
                    Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);
                    i++;
                    this.Text = i.ToString();
                    string returnData = Encoding.ASCII.GetString(receiveBytes);

                    textBox1.Text =  "Pkt Rx " + DateTime.Now.ToString() + returnData.ToString();
                    textBox1.Refresh();
                }

            }
        }

        public class UdpState
        {



            public IPEndPoint e;



            public UdpClient u;

        }

        public static void ReceiveCallback(IAsyncResult ar)
        {
            UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
            IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

            Byte[] receiveBytes = u.EndReceive(ar, ref e);
            string receiveString = Encoding.ASCII.GetString(receiveBytes);

            MessageBox.Show("Got a Message");
            Console.WriteLine("Received: {0}", receiveString);
            messageReceived = true;
        }

        public static void ReceiveMessages()
        {
            // Receive a message and write it to the console.
            IPEndPoint e = new IPEndPoint(IPAddress.Any, 7783);
            UdpClient u = new UdpClient(e);

            UdpState s = new UdpState();
            s.e = e;
            s.u = u;

            
            
            Console.WriteLine("listening for messages");
            u.BeginReceive(new AsyncCallback(ReceiveCallback), s);

            MessageBox.Show("Call back started");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ReceiveMessages();
        }
    }
}
