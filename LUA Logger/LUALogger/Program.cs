using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;
using System.Net;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {

            Console.WriteLine("LUA Logger");

            // Setup call back function to clean up network ports on a CTRL-C
            Console.CancelKeyPress += delegate
            {
                cleanUp();
            };

            UdpClient receivingUdpClient = new UdpClient(13135);
            Console.WriteLine("Listening on UDP Port " + receivingUdpClient.Client.LocalEndPoint);
            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            Thread.Sleep(1000);

            while (true)
            {

                {

                    // Blocks until a message returns on this socket from a remote host.
                    Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                    string returnData = Encoding.ASCII.GetString(receiveBytes);

                    Console.WriteLine("Pkt Rx " +
                                                returnData.ToString());
                }

            }



        }

        public static void cleanUp()
        {
            Console.WriteLine("Closing Connections!");

        }
    }
}
