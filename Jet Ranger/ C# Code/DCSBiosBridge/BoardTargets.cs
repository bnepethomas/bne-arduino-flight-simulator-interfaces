using System.Net.Sockets;
using System.Text;

namespace DCSBiosBridge
{
    // One packet as it was sent: the payload text, the board (or the GPS interface) it is for and where it went.
    public sealed class SentPacket
    {
        public DateTime Time;
        public string Board = "";
        public string Payload = "";
        public string Destination = "";     // board, emulator, board + emulator, or local (the GPS interface)
    }

    // Where the board packets go: to the real boards (172.16.1.x), to the Jet Ranger panel emulators (JetRangerPanelEmulator), or to both. Each emulator listens on the
    // board's own port at the same last number on a loopback address (127.0.0.101 for the Radio .101 and so on), so the packet is the one the board would get.
    // Every packet sent is also reported through PacketSent, which the packet monitor window shows.
    public sealed class BoardTargets
    {
        public const int Port = 13136;
        public const string GpsName = "GPS interface";
        static readonly HashSet<int> EmulatedBoards = new() { 101, 102, 105, 106, 107 };

        // The names the packet monitor filters on, in the order of the boards.
        public static readonly string[] Names = { "Radio .101", "Servo .102", "Stepper .105", "Dual Stepper .106", "ADF and Upper .107", "Upper .103 (retired)", GpsName };

        public bool ToBoards = true;
        public bool ToEmulators;
        public string EmulatorNet = "127.0.0";          // --emulator-net <a.b.c> when the emulators run on another address range
        public bool ShowPacketsAtStart;                 // --show-packets
        public string PacketFilter = "";                // --packet-board <part of a board's name or its last number>

        public event Action<SentPacket>? PacketSent;

        readonly HashSet<UdpClient> boards = new();
        readonly Dictionary<UdpClient, UdpClient> mirrors = new();
        readonly Dictionary<UdpClient, string> names = new();

        // Command line: --emulators (send to the emulators as well as the boards), --emulators-only (not to the boards), --emulator-net <a.b.c>,
        // --show-packets (open the packet monitor at start) and --packet-board <radio, servo, stepper, dual, adf, gps, or a last number such as 105> (filter it).
        public void ReadCommandLine(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                if (args[i].Equals("--emulators", StringComparison.OrdinalIgnoreCase)) ToEmulators = true;
                else if (args[i].Equals("--emulators-only", StringComparison.OrdinalIgnoreCase)) { ToEmulators = true; ToBoards = false; }
                else if (args[i].Equals("--emulator-net", StringComparison.OrdinalIgnoreCase) && i + 1 < args.Length) EmulatorNet = args[++i];
                else if (args[i].Equals("--show-packets", StringComparison.OrdinalIgnoreCase)) ShowPacketsAtStart = true;
                else if (args[i].Equals("--packet-board", StringComparison.OrdinalIgnoreCase) && i + 1 < args.Length) { PacketFilter = args[++i]; ShowPacketsAtStart = true; }
            }
        }

        static string NameOf(int number) => number switch
        {
            101 => Names[0], 102 => Names[1], 105 => Names[2], 106 => Names[3], 107 => Names[4], 103 => Names[5], _ => "Board ." + number,
        };

        // boardAddress is the real board's address (172.16.1.105 ...), already connected as boardClient; the emulator's address is EmulatorNet plus its last number.
        public void Register(UdpClient boardClient, string boardAddress)
        {
            boards.Add(boardClient);
            if (!int.TryParse(boardAddress.Substring(boardAddress.LastIndexOf('.') + 1), out int number)) return;
            names[boardClient] = NameOf(number);
            if (!EmulatedBoards.Contains(number)) return;
            var mirror = new UdpClient();
            mirror.Connect(EmulatorNet + "." + number, Port);
            mirrors[boardClient] = mirror;
        }

        public bool Handles(UdpClient client) => boards.Contains(client);

        // A board or emulator that is not there can raise a socket error on a later send; that must never stop the bridge.
        public void Send(UdpClient boardClient, byte[] data)
        {
            bool toBoard = ToBoards;
            bool toEmulator = ToEmulators && mirrors.TryGetValue(boardClient, out _);
            if (toBoard) TrySend(boardClient, data);
            if (toEmulator) TrySend(mirrors[boardClient], data);
            Report(names.TryGetValue(boardClient, out var name) ? name : "Board", data, toBoard ? (toEmulator ? "board + emulator" : "board") : (toEmulator ? "emulator" : "nowhere"));
        }

        // A packet for something that is not a board (the GPS interface on this PC).
        public void SendDirect(UdpClient client, string name, byte[] data)
        {
            TrySend(client, data);
            Report(name, data, "local");
        }

        void Report(string board, byte[] data, string destination)
        {
            var handler = PacketSent;
            if (handler == null) return;
            handler(new SentPacket { Time = DateTime.Now, Board = board, Payload = Encoding.ASCII.GetString(data), Destination = destination });
        }

        static void TrySend(UdpClient client, byte[] data)
        {
            try { client.Send(data, data.Length); }
            catch (SocketException) { }
            catch (ObjectDisposedException) { }
        }
    }
}
