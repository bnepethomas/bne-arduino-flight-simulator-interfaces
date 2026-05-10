using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {

        Boolean exitNow = false;
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {


            byte[] NULL = new byte[1] { 0x00 };
            byte[] STX = new byte[1] { 0x02 };
            byte[] ETX = new byte[1] { 0x03 };
            byte[] CR = new byte[1] { 0x0d };


            byte[] oled1 = new byte[1] { 0x01 };
            byte[] oled2 = new byte[1] { 0x02 };
            byte[] oled3 = new byte[1] { 0x03 };
            byte[] oled4 = new byte[1] { 0x04 };
            byte[] oled5 = new byte[1] { 0x05 };
            byte[] oled6 = new byte[1] { 0x06 };
            byte[] oled7 = new byte[1] { 0x07 };
 
            byte[] CLS = new byte[1] { 0x7F };

            // Note that 0x02 and 0x03 are reserved for STX and ETX
            byte[] FirstLine = new byte[1] { 0x01 };
            byte[] SecondLine = new byte[1] { 0x02 };

            if (!this.serialPort1.IsOpen)
                  this.serialPort1.Open();

            // Couple of STX to sync up 
            serialPort1.Write(NULL, 0, 1);
            serialPort1.Write(NULL, 0, 1);

            serialPort1.Write(STX, 0, 1);
            serialPort1.Write(oled1, 0, 1);
            serialPort1.Write(FirstLine, 0, 1);
            serialPort1.Write(CLS, 0, 1);
            serialPort1.Write(ETX, 0, 1);

            serialPort1.Write(STX, 0, 1);
            serialPort1.Write(oled2, 0, 1);
            serialPort1.Write(FirstLine, 0, 1);
            serialPort1.Write(CLS, 0, 1);
            serialPort1.Write(ETX, 0, 1);



            while (!exitNow)
            {
                Application.DoEvents();
                Thread.Sleep(50);
                if (this.serialPort1.IsOpen)
                {
                    serialPort1.Write(STX, 0, 1);
                    serialPort1.Write(oled1, 0, 1);
                    serialPort1.Write(FirstLine, 0, 1);
                    serialPort1.Write(CR, 0, 1);
                    serialPort1.Write("1 - " + DateTime.Now.ToLongTimeString());
                    serialPort1.Write(ETX, 0, 1);


                    serialPort1.Write(STX, 0, 1);
                    serialPort1.Write(oled2, 0, 1);
                    serialPort1.Write(FirstLine, 0, 1);
                    serialPort1.Write(CR, 0, 1);
                    serialPort1.Write("2 - " + DateTime.Now.ToLongTimeString());
                    serialPort1.Write(ETX, 0, 1);
                }

            };


            

        }

        private void button2_Click(object sender, EventArgs e)
        {

            exitNow = true;
            serialPort1.Close();
            System.Windows.Forms.Application.Exit();
        }
    }
}
