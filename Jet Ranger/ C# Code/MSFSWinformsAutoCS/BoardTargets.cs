using System.Net.Sockets;

namespace MSFSWinformsAutoCS
{
    // Where the board packets go: to the real boards (172.16.1.x), to the Jet Ranger panel emulators (JetRangerPanelEmulator), or to both. Each emulator listens on the
    // board's own port at the same last number on a loopback address (127.0.0.101 for the Radio .101 and so on), so the packet is the one the board would get.
    public sealed class BoardTargets
    {
        public const int Port = 13136;
        static readonly HashSet<int> EmulatedBoards = new() { 101, 102, 105, 106, 107 };

        public bool ToBoards = true;
        public bool ToEmulators;
        public string EmulatorNet = "127.0.0";          // --emulator-net <a.b.c> when the emulators run on another address range

        readonly HashSet<UdpClient> boards = new();
        readonly Dictionary<UdpClient, UdpClient> mirrors = new();

        // Command line: --emulators (send to the emulators as well as the boards), --emulators-only (not to the boards), --emulator-net <a.b.c>.
        public void ReadCommandLine(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                if (args[i].Equals("--emulators", StringComparison.OrdinalIgnoreCase)) ToEmulators = true;
                else if (args[i].Equals("--emulators-only", StringComparison.OrdinalIgnoreCase)) { ToEmulators = true; ToBoards = false; }
                else if (args[i].Equals("--emulator-net", StringComparison.OrdinalIgnoreCase) && i + 1 < args.Length) EmulatorNet = args[++i];
            }
        }

        // boardAddress is the real board's address (172.16.1.105 ...), already connected as boardClient; the emulator's address is EmulatorNet plus its last number.
        public void Register(UdpClient boardClient, string boardAddress)
        {
            boards.Add(boardClient);
            if (!int.TryParse(boardAddress.Substring(boardAddress.LastIndexOf('.') + 1), out int number) || !EmulatedBoards.Contains(number)) return;
            var mirror = new UdpClient();
            mirror.Connect(EmulatorNet + "." + number, Port);
            mirrors[boardClient] = mirror;
        }

        public bool Handles(UdpClient client) => boards.Contains(client);

        // A board or emulator that is not there can raise a socket error on a later send; that must never stop the bridge.
        public void Send(UdpClient boardClient, byte[] data)
        {
            if (ToBoards) TrySend(boardClient, data);
            if (ToEmulators && mirrors.TryGetValue(boardClient, out var mirror)) TrySend(mirror, data);
        }

        static void TrySend(UdpClient client, byte[] data)
        {
            try { client.Send(data, data.Length); }
            catch (SocketException) { }
            catch (ObjectDisposedException) { }
        }
    }
}
