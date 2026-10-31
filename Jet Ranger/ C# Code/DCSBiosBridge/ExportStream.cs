using System.Net;
using System.Net.Sockets;
using System.Text;

namespace DCSBiosBridge
{
    // DCS-BIOS keeps the cockpit state as a 64 KB memory image and streams changes to it over UDP: after the four bytes 0x55 0x55 0x55 0x55 come write
    // blocks of <start address, 16 bit> <byte count, 16 bit> <data>, little endian, always an even number of bytes. A control's value is
    // (word at its address & mask) >> shift_by. Same state machine as the DCS-BIOS Arduino library's ExportStreamListener.
    public sealed class ExportState
    {
        readonly byte[] memory = new byte[65536];
        readonly object gate = new();
        public long Frames;                 // number of sync sequences seen
        public long Datagrams;
        public DateTime LastFrameUtc = DateTime.MinValue;

        public void WriteWord(int address, int value)
        {
            lock (gate)
            {
                memory[address & 0xFFFF] = (byte)(value & 0xFF);
                memory[(address + 1) & 0xFFFF] = (byte)((value >> 8) & 0xFF);
            }
        }

        public int ReadWord(int address)
        {
            lock (gate) return memory[address & 0xFFFF] | (memory[(address + 1) & 0xFFFF] << 8);
        }

        public string ReadString(int address, int length)
        {
            var sb = new StringBuilder();
            lock (gate)
            {
                for (int i = 0; i < length; i++)
                {
                    byte b = memory[(address + i) & 0xFFFF];
                    if (b == 0) break;
                    sb.Append((char)b);
                }
            }
            return sb.ToString().Trim();
        }

        // The value of an integer output: (word & mask) >> shift.
        public int ReadValue(int address, int mask, int shift) => (ReadWord(address) & mask) >> shift;

        public double SecondsSinceFrame => LastFrameUtc == DateTime.MinValue ? double.MaxValue : (DateTime.UtcNow - LastFrameUtc).TotalSeconds;
    }

    public sealed class ExportParser
    {
        enum S { AddrLo, AddrHi, CountLo, CountHi, DataLo, DataHi, WaitSync }

        readonly ExportState state;
        S s = S.WaitSync;
        int address, count, data, syncBytes;

        public ExportParser(ExportState state) { this.state = state; }

        public void Feed(ReadOnlySpan<byte> bytes)
        {
            foreach (byte c in bytes) Process(c);
        }

        void Process(byte c)
        {
            switch (s)
            {
                case S.AddrLo: address = c; s = S.AddrHi; break;
                case S.AddrHi:
                    address |= c << 8;
                    s = address != 0x5555 ? S.CountLo : S.WaitSync;
                    break;
                case S.CountLo: count = c; s = S.CountHi; break;
                case S.CountHi:
                    count |= c << 8;
                    s = count == 0 ? S.AddrLo : S.DataLo;
                    break;
                case S.DataLo: data = c; count--; s = S.DataHi; break;
                case S.DataHi:
                    data |= c << 8;
                    count--;
                    state.WriteWord(address, data);
                    address += 2;
                    s = count <= 0 ? S.AddrLo : S.DataLo;
                    break;
                case S.WaitSync: break;
            }
            if (c == 0x55) syncBytes++; else syncBytes = 0;
            if (syncBytes == 4)
            {
                s = S.AddrLo;
                syncBytes = 0;
                state.Frames++;
                state.LastFrameUtc = DateTime.UtcNow;
            }
        }
    }

    // Listens to the DCS-BIOS export stream: UDP multicast 239.255.50.10 port 5010 on the loopback interface (the DCS-BIOS default). The socket must allow
    // address reuse so other programs (the DCS-BIOS Hub, other tools) can listen to the same stream.
    public sealed class ExportListener : IDisposable
    {
        public const string Group = "239.255.50.10";
        public const int Port = 5010;

        readonly ExportState state;
        readonly ExportParser parser;
        UdpClient? client;
        Thread? thread;
        volatile bool running;
        public string Status { get; private set; } = "not started";

        public ExportListener(ExportState state)
        {
            this.state = state;
            parser = new ExportParser(state);
        }

        public void Start()
        {
            try
            {
                client = new UdpClient(AddressFamily.InterNetwork);
                client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                client.Client.Bind(new IPEndPoint(IPAddress.Any, Port));
                // the stream goes out on the loopback interface; a second join on the default interface covers a DCS-BIOS set up to send elsewhere
                client.JoinMulticastGroup(IPAddress.Parse(Group), IPAddress.Loopback);
                Status = "listening on " + Group + ":" + Port;
            }
            catch (Exception ex)
            {
                Status = "cannot listen: " + ex.Message;
                return;
            }
            running = true;
            thread = new Thread(Loop) { IsBackground = true, Name = "DCS-BIOS export listener" };
            thread.Start();
        }

        void Loop()
        {
            var any = new IPEndPoint(IPAddress.Any, 0);
            while (running)
            {
                try
                {
                    byte[] data = client!.Receive(ref any);
                    state.Datagrams++;
                    parser.Feed(data);
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

        public void Dispose()
        {
            running = false;
            try { client?.Close(); } catch { }
        }
    }
}
