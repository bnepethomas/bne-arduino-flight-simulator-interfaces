using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using DCS;

namespace DCSExtractGui
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private string m_Text = "DCSExtract";
        private bool m_Tracking = false;
        private bool m_Ka50 = false;
        private bool m_A10c = false;
        private Thread m_Search = null;
        private Object m_Lock = new Object();

        private void OnLoad(object sender, EventArgs e)
        {
            ClearKa50();
            ClearA10c();

            m_Search = new Thread(new ThreadStart(Search));
            m_Search.Start();
            main.Start();
        }

        private void OnClosing(object sender, FormClosingEventArgs e)
        {
            if (m_Search != null) m_Search.Abort();
        }

        private void Search()
        {
            while (true)
            {
                lock (m_Lock)
                {
                    m_Ka50 = false;
                    m_A10c = false;
                    m_Tracking = false;
                }

                while (!DCSExtract.exist_dcs())
                {
                    Thread.Sleep(50);
                }

                DCSExtract.start_capture();

                if (DCSExtract.exist_ka50())
                {
                    lock (m_Lock)
                    {
                        m_Ka50 = true;
                        m_Tracking = true;
                    }

                    if (!DCSExtract.scan_ka50(true, true, true))
                    {
                        lock(m_Lock)
                        {
                            m_Ka50 = false;
                            m_Tracking = false;
                        }

                        continue;
                    }
                }
                else
                if (DCSExtract.exist_a10c())
                {
                    lock (m_Lock)
                    {
                        m_A10c = true;
                        m_Tracking = true;
                    }

                    if (!DCSExtract.scan_a10c(true, true, true, true))
                    {
                        lock (m_Lock)
                        {
                            m_A10c = false;
                            m_Tracking = false;
                        }

                        continue;
                    }
                }

                lock(m_Lock)
                {
                    m_Tracking = false;
                }

                while(DCSExtract.exist_dcs())
                {
                    Thread.Sleep(50);
                }
            }
        }

        private void OnTick(object sender, EventArgs e)
        {
            if(!DCSExtract.exist_dcs())
            {
                this.Text = m_Text + ": Game not detected";
                return;
            }

            bool ka50 = false;
            bool a10c = false;
            bool track = false;

            lock(m_Lock)
            {
                ka50 = m_Ka50;
                a10c = m_A10c;
                track = m_Tracking;
            }

            if (ka50)
            {
                if (!m_Tracking)
                {
                    this.Text = m_Text + ": Ka50 detected";
                    ExtractKa50();
                }
                else
                {
                    this.Text = m_Text + ": Ka50 searching data";
                    ClearKa50();
                }
            }
            else
            {
                ClearKa50();
            }

            if (a10c)
            {
                if (!m_Tracking)
                {
                    this.Text = m_Text + ": A10c detected";
                    ExtractA10c();
                }
                else
                {
                    this.Text = m_Text + ": A10c searching data";
                    ClearA10c();
                }
            }
            else
            {
                ClearA10c();
            }
        }

        private void ExtractKa50()
        {
            string uv = "";
            DCSExtract.ScanUV26(ref uv);
            uv26.Text = uv;

            string queue = "";
            string memory = "";
            string failure = "";
            string line1 = "";
            string line2 = "";
            string line3 = "";
            string line4 = "";
            DCSExtract.ScanEkran(ref queue, ref memory, ref failure, ref line1, ref line2, ref line3, ref line4);
            ekran_queue.Text = queue;
            ekran_memory.Text = memory;
            ekran_failure.Text = failure;
            ekran_line1.Text = line1;
            ekran_line2.Text = line2;
            ekran_line3.Text = line3;
            ekran_line4.Text = line4;

            int pviul = 0;
            string pviu = "";
            int pviur = 0;
            int pvidl = 0;
            string pvid = "";
            int pvidr = 0;
            DCSExtract.ScanPVI(ref pviul, ref pviu, ref pviur, ref pvidl, ref pvid, ref pvidr);
            pvi_sign_up.Text = pviul == 1 ? "-" : "";
            pvi_up.Text = pviu;
            pvi_up_right.Text = pviur == -1 ? "" : pviur.ToString();
            pvi_sign_down.Text = pvidl == 1 ? "-" : "";
            pvi_down.Text = pvid;
            pvi_down_right.Text = pvidr == -1 ? "" : pvidr.ToString();

            bool r828on = false;
            int r828ch = 0;
            DCSExtract.ScanR828(ref r828on, ref r828ch);
            r828_on.Text = r828on ? "ON" : "OFF";
            r828_channel.Text = r828ch.ToString();

            bool spu9on = false;
            int spu9ch = 0;
            DCSExtract.ScanSpu9(ref spu9on, ref spu9ch);
            spu9_on.Text = spu9on ? "ON" : "OFF";

            switch(spu9ch)
            {
                case 0: spu9_channel.Text = "VHF2"; break;
                case 1: spu9_channel.Text = "VHF1"; break;
                case 2: spu9_channel.Text = "SW"; break;
                case 3: spu9_channel.Text = "GND CREW"; break;
            }

            bool r800on = false;
            bool r800am = false;
            string r800freq = "";
            DCSExtract.ScanR800(ref r800on, ref r800am, ref r800freq);
            r800_on.Text = r800on ? "ON" : "OFF";
            r800_am.Text = r800am ? "AM" : "FM";
            r800_frequency.Text = r800freq;

            string left = "";
            int middle = 0;
            int right = 0;
            DCSExtract.ScanPui800(ref left, ref middle, ref right);
            pui_left.Text = left;
            pui_middle.Text = middle == -1 ? "" : middle.ToString();
            pui_right.Text = right == -1 ? "" : right.ToString();
        }

        private void ExtractA10c()
        {
            string chaff = "";
            string flare = "";
            string jmr = "";
            string mws = "";
            bool uwt = false;
            bool uws = false;
            DCSExtract.ScanCMSC(ref chaff, ref flare, ref jmr, ref mws, ref uwt, ref uws);
            cmsc_chaff.Text = chaff;
            cmsc_flare.Text = flare;
            cmsc_jammer.Text = jmr;
            cmsc_mws.Text = mws;
            cmsc_uwthreats.Text = uwt ? "ON" : "OFF";
            cmsc_uwsymbols.Text = uws ? "ON" : "OFF";

            string u1 = "";
            string u2 = "";
            string u3 = "";
            string u4 = "";
            string d1 = "";
            string d2 = "";
            string d3 = "";
            string d4 = "";
            DCSExtract.ScanCMSP(ref u1, ref u2, ref u3, ref u4, ref d1, ref d2, ref d3, ref d4);
            cmsp_up1.Text = u1;
            cmsp_up2.Text = u2;
            cmsp_up3.Text = u3;
            cmsp_up4.Text = u4;
            cmsp_down1.Text = d1;
            cmsp_down2.Text = d2;
            cmsp_down3.Text = d3;
            cmsp_down4.Text = d4;

            int modedial = 0;
            string tcfreq = "";
            DCSExtract.ScanTacan(ref modedial, ref tcfreq);
            tacan_frequency.Text = tcfreq;

            switch(modedial)
            {
                case 0: tacan_modedial.Text = "OFF"; break;
                case 1: tacan_modedial.Text = "REC"; break;
                case 2: tacan_modedial.Text = "T/R"; break;
                case 3: tacan_modedial.Text = "A/A REC"; break;
                case 4: tacan_modedial.Text = "A/A T/R"; break;
            }

            bool ilson = false;
            string ilsfreq = "";
            DCSExtract.ScanILS(ref ilson, ref ilsfreq);
            ils_on.Text = ilson ? "ON" : "OFF";
            ils_frequency.Text = ilsfreq;

            int uhfchan = 0;
            int uhfdial = 0;
            int uhfmode = 0;
            string uhfreq = "";
            DCSExtract.ScanUHF(ref uhfchan, ref uhfdial, ref uhfmode, ref uhfreq);
            uhf_frequency.Text = uhfreq;
            uhf_channel.Text = uhfchan.ToString();

            switch(uhfdial)
            {
                case 0: uhf_functiondial.Text = "OFF"; break;
                case 1: uhf_functiondial.Text = "MAIN"; break;
                case 2: uhf_functiondial.Text = "BOTH"; break;
                case 3: uhf_functiondial.Text = "ADF"; break;
            }

            switch(uhfmode)
            {
                case 0: uhf_modefrequency.Text = "MNL"; break;
                case 1: uhf_modefrequency.Text = "PRESET"; break;
                case 2: uhf_modefrequency.Text = "GRD"; break;
            }

            bool vamon = false;
            int vamchan = 0;
            int vammode = 0;
            int vamselect = 0;
            string vamfreq = "";
            DCSExtract.ScanVHF_AM(ref vamon, ref vamchan, ref vammode, ref vamselect, ref vamfreq);
            vhf_am_on.Text = vamon ? "ON" : "OFF";
            vhf_am_channel.Text = vamchan.ToString();
            vhf_am_frequency.Text = vamfreq;

            switch(vammode)
            {
                case 0: vhf_am_modefrequency.Text = "OFF"; break;
                case 1: vhf_am_modefrequency.Text = "TK"; break;
                case 2: vhf_am_modefrequency.Text = "DN"; break;
            }

            switch (vamselect)
            {
                case 0: vhf_am_selectfrequency.Text = "FM"; break;
                case 1: vhf_am_selectfrequency.Text = "AM"; break;
                case 2: vhf_am_selectfrequency.Text = "MAN"; break;
                case 3: vhf_am_selectfrequency.Text = "PRE"; break;
            }

            bool vfmon = false;
            int vfmchan = 0;
            int vfmmode = 0;
            int vfmselect = 0;
            string vfmfreq = "";
            DCSExtract.ScanVHF_FM(ref vfmon, ref vfmchan, ref vfmmode, ref vfmselect, ref vfmfreq);
            vhf_fm_on.Text = vfmon ? "ON" : "OFF";
            vhf_fm_channel.Text = vfmchan.ToString();
            vhf_fm_frequency.Text = vfmfreq;

            switch (vfmmode)
            {
                case 0: vhf_fm_modefrequency.Text = "OFF"; break;
                case 1: vhf_fm_modefrequency.Text = "TK"; break;
                case 2: vhf_fm_modefrequency.Text = "DN"; break;
            }

            switch (vfmselect)
            {
                case 0: vhf_fm_selectfrequency.Text = "FM"; break;
                case 1: vhf_fm_selectfrequency.Text = "AM"; break;
                case 2: vhf_fm_selectfrequency.Text = "MAN"; break;
                case 3: vhf_fm_selectfrequency.Text = "PRE"; break;
            }
        }

        private void ClearKa50()
        {
            uv26.Text = "";
            ekran_queue.Text = "";
            ekran_memory.Text = "";
            ekran_failure.Text = "";
            ekran_line1.Text = "";
            ekran_line2.Text = "";
            ekran_line3.Text = "";
            ekran_line4.Text = "";
            pvi_sign_up.Text = "";
            pvi_up.Text = "";
            pvi_up_right.Text = "";
            pvi_sign_down.Text = "";
            pvi_down.Text = "";
            pvi_down_right.Text = "";
            r828_on.Text = "";
            r828_channel.Text = "";
            pui_left.Text = "";
            pui_middle.Text = "";
            pui_right.Text = "";
            spu9_on.Text = "";
            spu9_channel.Text = "";
            r800_on.Text = "";
            r800_am.Text = "";
            r800_frequency.Text = "";
        }

        private void ClearA10c()
        {
            cmsc_jammer.Text = "";
            cmsc_chaff.Text = "";
            cmsc_flare.Text = "";
            cmsc_mws.Text = "";
            cmsc_uwthreats.Text = "";
            cmsc_uwsymbols.Text = "";
            cmsp_up1.Text = "";
            cmsp_up2.Text = "";
            cmsp_up3.Text = "";
            cmsp_up4.Text = "";
            cmsp_down1.Text = "";
            cmsp_down2.Text = "";
            cmsp_down3.Text = "";
            cmsp_down4.Text = "";
            tacan_modedial.Text = "";
            tacan_frequency.Text = "";
            ils_on.Text = "";
            ils_frequency.Text = "";
            uhf_functiondial.Text = "";
            uhf_modefrequency.Text = "";
            uhf_frequency.Text = "";
            uhf_channel.Text = "";
            vhf_am_on.Text = "";
            vhf_am_modefrequency.Text = "";
            vhf_am_selectfrequency.Text = "";
            vhf_am_frequency.Text = "";
            vhf_am_channel.Text = "";
            vhf_fm_on.Text = "";
            vhf_fm_modefrequency.Text = "";
            vhf_fm_selectfrequency.Text = "";
            vhf_fm_frequency.Text = "";
            vhf_fm_channel.Text = "";
        }
    }
}
