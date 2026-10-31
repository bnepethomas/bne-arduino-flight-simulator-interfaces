using System.Globalization;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace JetRangerPanelEmulator
{
    // One packet as the boards receive it on UDP 13136: "D,TOKEN:value,TOKEN:value" (data) or "C,COMMAND:value" (control).
    public sealed class Packet
    {
        public char Kind;
        public List<(string Key, string Value)> Items = new();
        public string Raw = "";

        public static Packet? Parse(string text)
        {
            string s = text.Trim('\0', '\r', '\n', ' ');
            if (s.Length < 3 || s[1] != ',' || (s[0] != 'D' && s[0] != 'C')) return null;
            var p = new Packet { Kind = s[0], Raw = s };
            foreach (string part in s.Substring(2).Split(','))
            {
                int colon = part.IndexOf(':');
                if (colon <= 0) continue;
                p.Items.Add((part.Substring(0, colon).Trim(), part.Substring(colon + 1).Trim()));
            }
            return p;
        }

        public static bool TryNumber(string value, out double number)
            => double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out number);
    }

    // The UDP end of an emulated board: it listens on <net>.<board number>:13136, the same port the real board uses, on its own loopback address
    // (127.0.0.101 for the Radio, 127.0.0.102 Servo, 127.0.0.105 Stepper, 127.0.0.106 Dual Stepper, 127.0.0.107 ADF and Upper), so a bridge only has to
    // swap 172.16.1 for 127.0.0 to talk to the emulator instead of the board.
    public sealed class PanelLink : IDisposable
    {
        public const int Port = 13136;
        readonly UdpClient udp;
        readonly CancellationTokenSource cts = new();
        public string Address { get; }
        public event Action<string>? Received;

        public PanelLink(string address)
        {
            Address = address;
            udp = new UdpClient(new IPEndPoint(IPAddress.Parse(address), Port));
            Task.Run(async () =>
            {
                while (!cts.IsCancellationRequested)
                {
                    try
                    {
                        var r = await udp.ReceiveAsync(cts.Token);
                        Received?.Invoke(Encoding.ASCII.GetString(r.Buffer));
                    }
                    catch (OperationCanceledException) { return; }
                    catch (ObjectDisposedException) { return; }
                    catch (SocketException) { /* a reset from an earlier send: keep listening */ }
                }
            });
        }

        public void Dispose()
        {
            cts.Cancel();
            try { udp.Close(); } catch { }
        }
    }
}
