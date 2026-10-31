using System.Net;
using System.Net.Sockets;
using System.Text;

namespace XPlaneBridge
{
    // The values X-Plane has sent back, by the index each dataref was subscribed with.
    public sealed class XPlaneState
    {
        readonly Dictionary<int, float> values = new();
        readonly object gate = new();
        public long Packets;
        public DateTime LastPacketUtc = DateTime.MinValue;

        public void Set(int index, float value)
        {
            lock (gate) values[index] = value;
        }

        public bool TryGet(int index, out float value)
        {
            lock (gate) return values.TryGetValue(index, out value);
        }

        public void Clear()
        {
            lock (gate) values.Clear();
        }

        public int Count { get { lock (gate) return values.Count; } }
        public double SecondsSincePacket => LastPacketUtc == DateTime.MinValue ? double.MaxValue : (DateTime.UtcNow - LastPacketUtc).TotalSeconds;
    }

    // Talks to X-Plane over its UDP interface (default port 49000), no plugin needed. A dataref is subscribed with an "RREF" request:
    //   "RREF\0", int32 frequency in Hz (0 cancels), int32 index chosen by the client, then the dataref path in a 400 byte field padded with zeros
    //   (an array element is written as "sim/cockpit2/engine/indicators/N1_percent[0]").
    // X-Plane answers to the address and port the request came from with "RREF," and then pairs of int32 index and float32 value, at the requested rate.
    // Subscriptions are sent again every few seconds, so they survive X-Plane being restarted. X-Plane also announces itself on the multicast group 239.255.1.1,
    // port 49707 (the "BECN" beacon), which finds the address of X-Plane on another PC.
    public sealed class XPlaneLink : IDisposable
    {
        public const int BeaconPort = 49707;
        public const string BeaconGroup = "239.255.1.1";

        readonly XPlaneState state;
        readonly UdpClient client = new(new IPEndPoint(IPAddress.Any, 0));
        UdpClient? beacon;
        Thread? receiver, beaconThread;
        volatile bool running;
        IPEndPoint target;
        string[] datarefs = Array.Empty<string>();
        DateTime lastSubscribe = DateTime.MinValue;
        public string Status { get; private set; } = "not started";
        public int FrequencyHz { get; set; } = 10;
        public bool BeaconFound { get; private set; }

        public XPlaneLink(XPlaneState state, string host, int port)
        {
            this.state = state;
            target = new IPEndPoint(Resolve(host), port);
        }

        static IPAddress Resolve(string host)
        {
            if (IPAddress.TryParse(host, out var ip)) return ip;
            return Dns.GetHostAddresses(host).First(a => a.AddressFamily == AddressFamily.InterNetwork);
        }

        public IPEndPoint Target => target;

        public void Start(bool listenForBeacon)
        {
            running = true;
            receiver = new Thread(ReceiveLoop) { IsBackground = true, Name = "X-Plane receiver" };
            receiver.Start();
            Status = "sending requests to " + target;
            if (!listenForBeacon) return;
            try
            {
                beacon = new UdpClient(AddressFamily.InterNetwork);
                beacon.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                beacon.Client.Bind(new IPEndPoint(IPAddress.Any, BeaconPort));
                beacon.JoinMulticastGroup(IPAddress.Parse(BeaconGroup));
                beaconThread = new Thread(BeaconLoop) { IsBackground = true, Name = "X-Plane beacon" };
                beaconThread.Start();
            }
            catch (Exception)
            {
                beacon = null;    // another program owns the port, or no multicast: the address given (or 127.0.0.1) is used
            }
        }

        // The dataref list to subscribe to; they are (re)subscribed with the next Tick.
        public void SetDatarefs(IEnumerable<string> paths)
        {
            datarefs = paths.ToArray();
            state.Clear();
            lastSubscribe = DateTime.MinValue;
        }

        public IReadOnlyList<string> Datarefs => datarefs;

        // Index 1 to n in the order of the list.
        public int IndexOf(string dataref) => Array.IndexOf(datarefs, dataref) + 1;

        // Called by the main timer: sends the subscriptions at the start and then every 5 seconds.
        public void Tick()
        {
            if ((DateTime.UtcNow - lastSubscribe).TotalSeconds < 5) return;
            lastSubscribe = DateTime.UtcNow;
            for (int i = 0; i < datarefs.Length; i++) SendRequest(FrequencyHz, i + 1, datarefs[i]);
        }

        void SendRequest(int frequency, int index, string path)
        {
            var packet = new byte[5 + 4 + 4 + 400];
            Encoding.ASCII.GetBytes("RREF\0", 0, 5, packet, 0);
            BitConverter.TryWriteBytes(packet.AsSpan(5, 4), frequency);
            BitConverter.TryWriteBytes(packet.AsSpan(9, 4), index);
            Encoding.ASCII.GetBytes(path, 0, Math.Min(path.Length, 399), packet, 13);
            try { client.Send(packet, packet.Length, target); }
            catch (SocketException) { }
            catch (ObjectDisposedException) { }
        }

        void ReceiveLoop()
        {
            var any = new IPEndPoint(IPAddress.Any, 0);
            while (running)
            {
                try
                {
                    byte[] data = client.Receive(ref any);
                    if (data.Length < 13 || data[0] != 'R' || data[1] != 'R' || data[2] != 'E' || data[3] != 'F') continue;
                    for (int o = 5; o + 8 <= data.Length; o += 8)
                        state.Set(BitConverter.ToInt32(data, o), BitConverter.ToSingle(data, o + 4));
                    state.Packets++;
                    state.LastPacketUtc = DateTime.UtcNow;
                }
                catch (SocketException)
                {
                    if (!running) break;
                }
                catch (ObjectDisposedException)
                {
                    break;
                }
            }
        }

        // "BECN\0", int8 major, int8 minor, int32 host id, int32 version, uint32 role, uint16 port (little endian), then the computer name.
        void BeaconLoop()
        {
            var any = new IPEndPoint(IPAddress.Any, 0);
            while (running)
            {
                try
                {
                    byte[] data = beacon!.Receive(ref any);
                    if (data.Length < 21 || data[0] != 'B' || data[1] != 'E' || data[2] != 'C' || data[3] != 'N') continue;
                    int port = BitConverter.ToUInt16(data, 5 + 2 + 4 + 4 + 4);
                    if (port == 0) continue;
                    var found = new IPEndPoint(any.Address, port);
                    if (!found.Equals(target))
                    {
                        target = found;
                        lastSubscribe = DateTime.MinValue;
                        Status = "found by its beacon at " + target;
                    }
                    BeaconFound = true;
                }
                catch (SocketException)
                {
                    if (!running) break;
                }
                catch (ObjectDisposedException)
                {
                    break;
                }
            }
        }

        // Cancels the subscriptions (frequency 0), so X-Plane stops sending when the bridge closes.
        public void Dispose()
        {
            running = false;
            for (int i = 0; i < datarefs.Length; i++) SendRequest(0, i + 1, datarefs[i]);
            try { client.Close(); } catch { }
            try { beacon?.Close(); } catch { }
        }
    }
}
