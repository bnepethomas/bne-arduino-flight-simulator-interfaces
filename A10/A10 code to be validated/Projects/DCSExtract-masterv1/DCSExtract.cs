using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace DCS
{
    class DCSExtract
    {
        [DllImport("DcsExport64.dll")]
        public static extern bool version(ref int mayor, ref int minor);

        [DllImport("DcsExport64.dll")]
        public static extern bool exist_dcs();

        [DllImport("DcsExport64.dll")]
        public static extern bool start_capture();

        [DllImport("DcsExport64.dll")]
        public static extern void stop_capture();

        [DllImport("DcsExport64.dll")]
        public static extern bool exist_ka50();

        [DllImport("DcsExport64.dll")]
        public static extern bool scan_ka50(bool spu9, bool r800, bool r828);

        [DllImport("DcsExport64.dll")]
        public static extern bool exist_a10c();

        [DllImport("DcsExport64.dll")]
        public static extern bool scan_a10c(bool tacan, bool ils, bool vhf_am, bool vhf_fm);

        public static void Info(ref string inf)
        {
            byte[] data = new byte[128];
            int len = 0;

            info(data, ref len);

            inf = Encoding.ASCII.GetString(data, 0, len);
        }

        //KA50
        public static bool ScanUV26(ref string data)
        {
            byte[] uv = new byte[16];
            int len = 0;

            if(!get_uv26(uv, ref len)) return false;

            data = Encoding.UTF8.GetString(uv, 0, len);

            return true;
        }

        public static bool ScanPVI(ref int upleft, ref string up, ref int upright, ref int downleft, ref string down, ref int downright)
        {
            byte[] sup = new byte[16];
            int ulen = 0;
            byte[] sdown = new byte[16];
            int dlen = 0;

            if(!get_pvi(ref upleft, sup, ref ulen, ref upright, ref downleft, sdown, ref dlen, ref downright)) return false;

            up = Encoding.UTF8.GetString(sup, 0, ulen);
            down = Encoding.UTF8.GetString(sdown, 0, dlen);

            return true;
        }

        public static bool ScanPui800(ref string left, ref int middle, ref int right)
        {
            byte[] sleft = new byte[16];
            int len = 0;

            if(!get_pui800(sleft, ref len, ref middle, ref right)) return false;

            left = Encoding.UTF8.GetString(sleft, 0, len);

            return true;
        }

        public static bool ScanEkran(ref string memory, ref string queue, ref string failure,ref string line1, ref string line2, ref string line3, ref string line4)
        {
            byte[] mem = new byte[256];
            int memlen = 0;

            byte[] que = new byte[256];
            int quelen = 0;

            byte[] fail = new byte[256];
            int faillen = 0;

            byte[] ln1 = new byte[256];
            int ln1len = 0;

            byte[] ln2 = new byte[256];
            int ln2len = 0;

            byte[] ln3 = new byte[256];
            int ln3len = 0;

            byte[] ln4 = new byte[256];
            int ln4len = 0;

            if(!get_ekran(mem, ref memlen, que, ref quelen, fail, ref faillen, ln1, ref ln1len, ln2, ref ln2len, ln3, ref ln3len, ln4, ref ln4len)) return false;

            memory = Encoding.UTF8.GetString(mem, 0, memlen);
            queue = Encoding.UTF8.GetString(que, 0, quelen);
            failure = Encoding.UTF8.GetString(fail, 0, faillen);
            line1 = Encoding.UTF8.GetString(ln1, 0, ln1len);
            line2 = Encoding.UTF8.GetString(ln2, 0, ln2len);
            line3 = Encoding.UTF8.GetString(ln3, 0, ln3len);
            line4 = Encoding.UTF8.GetString(ln4, 0, ln4len);

            return true;
        }

        public static bool ScanSpu9(ref bool on, ref int channel)
        {
            return get_spu9(ref on, ref channel);
        }

        public static bool ScanR800(ref bool on, ref bool am, ref string frequency)
        {
            byte[] freq = new byte[256];
            int len = 0;

            if(!get_r800(ref on, ref am, freq, ref len)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, len);

            return true;
        }

        public static bool ScanR828(ref bool on, ref int channel)
        {
            return get_r828(ref on, ref channel);
        }

        //A10C
        public static bool ScanCMSC(ref string chaff, ref string flare, ref string jmr, ref string mws, ref bool unwrapthreats, ref bool unwrapsymbols)
        {
            byte[] ch = new byte[256];
            int chlen = 0;
            byte[] fl = new byte[256];
            int fllen = 0;
            byte[] jm = new byte[256];
            int jmlen = 0;
            byte[] mw = new byte[256];
            int mwlen = 0;

            if(!get_cmsc(ch, ref chlen, fl, ref fllen, jm, ref jmlen, mw, ref mwlen, ref unwrapthreats, ref unwrapsymbols)) return false;

            chaff = Encoding.UTF8.GetString(ch, 0, chlen);
            flare = Encoding.UTF8.GetString(fl, 0, fllen);
            jmr = Encoding.UTF8.GetString(jm, 0, jmlen);
            mws = Encoding.UTF8.GetString(mw, 0, mwlen);

            return true;
        }

        public static bool ScanCMSP(ref string up1, ref string up2, ref string up3, ref string up4, ref string down1, ref string down2, ref string down3, ref string down4)
        {
            byte[] u1 = new byte[256];
            int u1len = 0;
            byte[] u2 = new byte[256];
            int u2len = 0;
            byte[] u3 = new byte[256];
            int u3len = 0;
            byte[] u4 = new byte[256];
            int u4len = 0;
            byte[] d1 = new byte[256];
            int d1len = 0;
            byte[] d2 = new byte[256];
            int d2len = 0;
            byte[] d3 = new byte[256];
            int d3len = 0;
            byte[] d4 = new byte[256];
            int d4len = 0;

            if(!get_cmsp(u1, ref u1len, u2, ref u2len, u3, ref u3len, u4, ref u4len, d1, ref d1len, d2, ref d2len, d3, ref d3len, d4, ref d4len)) return false;

            up1 = Encoding.UTF8.GetString(u1, 0, u1len);
            up2 = Encoding.UTF8.GetString(u2, 0, u2len);
            up3 = Encoding.UTF8.GetString(u3, 0, u3len);
            up4 = Encoding.UTF8.GetString(u4, 0, u4len);
            down1 = Encoding.UTF8.GetString(d1, 0, d1len);
            down2 = Encoding.UTF8.GetString(d2, 0, d2len);
            down3 = Encoding.UTF8.GetString(d3, 0, d3len);
            down4 = Encoding.UTF8.GetString(d4, 0, d4len);

            return true;
        }

        public static bool ScanUHF(ref int channel, ref int functiondial, ref int modefrequency, ref string frequency)
        {
            byte[] freq = new byte[256];
            int freqlen = 0;

            if(!get_uhf(ref channel, ref functiondial, ref modefrequency, freq, ref freqlen)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, freqlen);

            return true;
        }

        public static bool ScanTacan(ref int modedial, ref string frequency)
        {
            byte[] freq = new byte[256];
            int freqlen = 0;

            if(!get_tacan(ref modedial, freq, ref freqlen)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, freqlen);

            return true;
        }

        public static bool ScanILS(ref bool on, ref string frequency)
        {
            byte[] freq = new byte[256];
            int freqlen = 0;

            if(!get_ils(ref on, freq, ref freqlen)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, freqlen);

            return true;
        }

        public static bool ScanVHF_AM(ref bool on, ref int channel, ref int modefrequency, ref int selectfrequency, ref string frequency)
        {
            byte[] freq = new byte[256];
            int freqlen = 0;

            if(!get_vhf_am(ref on, ref channel, ref modefrequency, ref selectfrequency, freq, ref freqlen)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, freqlen);

            return true;
        }

        public static bool ScanVHF_FM(ref bool on, ref int channel, ref int modefrequency, ref int selectfrequency, ref string frequency)
        {
            byte[] freq = new byte[256];
            int freqlen = 0;

            if(!get_vhf_fm(ref on, ref channel, ref modefrequency, ref selectfrequency, freq, ref freqlen)) return false;

            frequency = Encoding.UTF8.GetString(freq, 0, freqlen);

            return true;
        }

        [DllImport("DcsExport64.dll")]
        private static extern void info([Out] byte[] info, ref int length);

        //KA50
        [DllImport("DcsExport64.dll")]
        private static extern bool get_uv26([Out] byte[] data, ref int length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_pvi(ref int upleft, [Out] byte[] up, ref int up_length, ref int upright, ref int downleft, [Out] byte[] down, ref int down_length, ref int downright);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_pui800([Out] byte[] left, ref int left_length, ref int middle, ref int right);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_ekran([Out] byte[] memory, ref int memorylength, [Out] byte[] queue, ref int queuelength, [Out] byte[] failure, ref int failurelength, [Out] byte[] line1, ref int line1length, [Out] byte[] line2, ref int line2length, [Out] byte[] line3, ref int line3length, [Out] byte[] line4, ref int line4length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_spu9(ref bool on, ref int channel);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_r800(ref bool on, ref bool am, [Out] byte[] frequency, ref int frequency_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_r828(ref bool on, ref int channel);

        //A10C
        [DllImport("DcsExport64.dll")]
        private static extern bool get_cmsc([Out] byte[] chaff, ref int chaff_length, [Out] byte[] flare, ref int flare_length, [Out] byte[] jmr, ref int jmr_length, [Out] byte[] mws, ref int mws_length, ref bool unwrapthreats, ref bool unwrapsymbols);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_cmsp([Out] byte[] up1, ref int up1_length, [Out] byte[] up2, ref int up2_length, [Out] byte[] up3, ref int up3_length, [Out] byte[] up4, ref int up4_length, [Out] byte[] down1, ref int down1_length, [Out] byte[] down2, ref int down2_length, [Out] byte[] down3, ref int down3_length, [Out] byte[] down4, ref int down4_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_uhf(ref int channel, ref int functiondial, ref int modefrequency, [Out] byte[] frequency, ref int frequency_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_tacan(ref int modedial, [Out] byte[] frequency, ref int frequency_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_ils(ref bool on, [Out] byte[] frequency, ref int frequency_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_vhf_am(ref bool on, ref int channel, ref int modefrequency, ref int selectfrequency, [Out] byte[] frequency, ref int frequency_length);

        [DllImport("DcsExport64.dll")]
        private static extern bool get_vhf_fm(ref bool on, ref int channel, ref int modefrequency, ref int selectfrequency, [Out] byte[] frequency, ref int frequency_length);
    }
}
