using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
//
using System.Net;
using System.Net.Sockets;
//
using System.Threading;
using System.IO;


namespace SendReboot_Shutdown
{
    class Program
    {

        public static UdpClient SendingUdpClient = new UdpClient(7784);

        public static IPAddress serverAddr = IPAddress.Parse("192.168.1.117");
        public static IPEndPoint RemoteIpEndPoint = new IPEndPoint(serverAddr, 7784);

        static void Main(string[] args)
        {


            Console.WriteLine("Sending Command to Raspberry Pi");
            try
            {
                Console.WriteLine(DateTime.Now);
                SendToArduino("999=Reboot");
                //SendToArduino("999=ShutdownAndHalt");
            }

            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

        }




        static void SendToArduino(String UpdatedDisplayValue)
        {

            Boolean ldebugging = false;
            if (ldebugging) Console.Write("Starting Send :");
            if (ldebugging) Console.WriteLine(UpdatedDisplayValue);
            byte[] send_buffer = Encoding.ASCII.GetBytes(UpdatedDisplayValue);

            SendingUdpClient.Send(send_buffer, send_buffer.Length, RemoteIpEndPoint);
            if (ldebugging) Console.WriteLine("Send Complete");


        }

    }

}
