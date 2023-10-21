using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Text.RegularExpressions;

namespace GetCMSPTextNewAlgorithm
{
    public partial class Form1 : Form
    {
        int CMSP_X, CMSP_Y;
        int CMSPwidth, CMSPheight;
        int CMSC_X, CMSC_Y;
        int CMSCwidth, CMSCheight;
        double CMSCthresholdBrightness = 0.12;
        //IntPtr handle, hdcSrc, hdcDest, hBitmap;
        Regex regexCMSCField = new Regex("([^ ]+) +([^ ]+) +([^ ]+)", RegexOptions.Singleline | RegexOptions.Compiled);

        PictureBox[] pboxChars;
        Label[] lblCharsInfo;
        Bitmap bitmapCMSP, bitmapCMSC;
        Graphics graphCMSP, graphCMSC;
        Dictionary<int, int[]> CharFeatures = new Dictionary<int, int[]>();

        Dictionary<char, int[]> CharFeatureTemplate = new Dictionary<char, int[]>();

        public Form1()
        {
            InitializeComponent();

            bitmapCMSP = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            bitmapCMSC = new Bitmap(pictureBoxCMSC.Width, pictureBoxCMSC.Height);
            graphCMSP = Graphics.FromImage(bitmapCMSP);
            graphCMSC = Graphics.FromImage(bitmapCMSC);

            btnStop.Enabled = false;

            pboxChars = new PictureBox[] { 
                picCMSPl1c01, picCMSPl1c02, picCMSPl1c03, picCMSPl1c04, picCMSPl1c05, picCMSPl1c06, picCMSPl1c07, picCMSPl1c08,
                picCMSPl1c09, picCMSPl1c10, picCMSPl1c11, picCMSPl1c12, picCMSPl1c13, picCMSPl1c14, picCMSPl1c15,
                picCMSPl2c01, picCMSPl2c02, picCMSPl2c03, picCMSPl2c04, picCMSPl2c05, picCMSPl2c06, picCMSPl2c07, picCMSPl2c08,
                picCMSPl2c09, picCMSPl2c10, picCMSPl2c11, picCMSPl2c12, picCMSPl2c13, picCMSPl2c14, picCMSPl2c15, picCMSPl2c16
            };


            lblCharsInfo = new Label[] { 
                lblCMSPl1Char01Msg,
                lblCMSPl1Char02Msg,
                lblCMSPl1Char03Msg,
                lblCMSPl1Char04Msg,
                lblCMSPl1Char05Msg,
                lblCMSPl1Char06Msg,
                lblCMSPl1Char07Msg,
                lblCMSPl1Char08Msg,
                lblCMSPl1Char09Msg,
                lblCMSPl1Char10Msg,
                lblCMSPl1Char11Msg,
                lblCMSPl1Char12Msg,
                lblCMSPl1Char13Msg,
                lblCMSPl1Char14Msg,
                lblCMSPl1Char15Msg,
                lblCMSPl2Char01Msg,
                lblCMSPl2Char02Msg,
                lblCMSPl2Char03Msg,
                lblCMSPl2Char04Msg,
                lblCMSPl2Char05Msg,
                lblCMSPl2Char06Msg,
                lblCMSPl2Char07Msg,
                lblCMSPl2Char08Msg,
                lblCMSPl2Char09Msg,
                lblCMSPl2Char10Msg,
                lblCMSPl2Char11Msg,
                lblCMSPl2Char12Msg,
                lblCMSPl2Char13Msg,
                lblCMSPl2Char14Msg,
                lblCMSPl2Char15Msg,
                lblCMSPl2Char16Msg
            };

            for (int i = 0; i < 15 + 16; i++)
            {
                CharFeatures.Add(i, new int[35]);
            }

            CharFeatureTemplate.Add('0', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('1', new int[] { 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('2', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1 });
            CharFeatureTemplate.Add('3', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('4', new int[] { 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 });

            CharFeatureTemplate.Add('5', new int[] { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('6', new int[] { 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('7', new int[] { 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0 });
            CharFeatureTemplate.Add('8', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('9', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 });

            CharFeatureTemplate.Add('A', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('B', new int[] { 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('C', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('D', new int[] { 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('E', new int[] { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1 });

            CharFeatureTemplate.Add('F', new int[] { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('G', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('H', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('I', new int[] { 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('J', new int[] { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0 });

            CharFeatureTemplate.Add('K', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('L', new int[] { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1 });
            CharFeatureTemplate.Add('M', new int[] { 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('N', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('O', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });

            CharFeatureTemplate.Add('P', new int[] { 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('Q', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('R', new int[] { 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('S', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('T', new int[] { 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 });

            CharFeatureTemplate.Add('U', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0 });
            CharFeatureTemplate.Add('V', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 });
            CharFeatureTemplate.Add('W', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 });
            CharFeatureTemplate.Add('X', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1 });
            CharFeatureTemplate.Add('Y', new int[] { 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 });
            CharFeatureTemplate.Add('Z', new int[] { 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1 });

            //font_CMS.tga 1,1, the square symbol
            CharFeatureTemplate.Add((char)128, new int[] { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 });
            //A Underscore 
            CharFeatureTemplate.Add('a', new int[] { 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 });
            //M Underscore 
            CharFeatureTemplate.Add('m', new int[] { 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 });
            //S Underscore 
            CharFeatureTemplate.Add('s', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 });
            //X Underscore 
            CharFeatureTemplate.Add('x', new int[] { 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 });

            CharFeatureTemplate.Add('!', new int[] { 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 });
            CharFeatureTemplate.Add('#', new int[] { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('(', new int[] { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 });
            CharFeatureTemplate.Add(')', new int[] { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 });
            CharFeatureTemplate.Add('.', new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 });

            CharFeatureTemplate.Add('*', new int[] { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('-', new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('/', new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
            CharFeatureTemplate.Add(':', new int[] { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 });
            CharFeatureTemplate.Add('?', new int[] { 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 });

        }

        private int GetDeviation(int[] sample, int[] template)
        {
            int deviation = 0;
            for (int i = 0; i < sample.Length; i++)
            {
                deviation += Math.Abs(sample[i] - template[i]);
            }
            return deviation;
        }

        private void btnGetScreen_Click(object sender, EventArgs e)
        {
            if (int.TryParse(txtPosX.Text, out CMSP_X) == false || int.TryParse(txtPosY.Text, out CMSP_Y) == false
                || int.TryParse(txtWidth.Text, out CMSPwidth) == false || int.TryParse(txtHeight.Text, out CMSPheight) == false ||
                int.TryParse(txtCMSC_X.Text, out CMSC_X) == false || int.TryParse(txtCMSC_Y.Text, out CMSC_Y) == false ||
                    int.TryParse(txtCMSC_W.Text, out CMSCwidth) == false || int.TryParse(txtCMSC_H.Text, out CMSCheight) == false)
            {
                MessageBox.Show("invalid number detected");
                return;
            }

            timerCMSC.Enabled = true;
            timerCMSP.Enabled = true;

            btnGetScreen.Enabled = false;
            btnStop.Enabled = true;
        }

        private void timerCMSP_Tick(object sender, EventArgs e)
        {
            graphCMSP.CopyFromScreen(CMSP_X, CMSP_Y, 0, 0, new Size(CMSPwidth, CMSPheight));
#if debug
            pictureBox1.Image = bitmapCMSP;
#endif

            AnalyzeCMSP();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            timerCMSP.Enabled = false;
            timerCMSC.Enabled = false;
            btnGetScreen.Enabled = true;
            btnStop.Enabled = false;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            btnStop_Click(sender, e);
        }

        private void AnalyzeCMSP()
        {
#if debug
            using (Graphics g = Graphics.FromImage(bitmapCMSP))
            {
#endif

            //GraphicsPath fillPath = new System.Drawing.Drawing2D.GraphicsPath(
            //        new Point[] {
            //            new Point( CMSPwidth, 0 ), 
            //            new Point( pictureBox1.Width, 0 ),
            //            new Point( pictureBox1.Width, pictureBox1.Height ),
            //            new Point( 0, pictureBox1.Height ),
            //            new Point( 0, CMSPheight ),
            //            new Point( CMSPwidth, CMSPheight ),
            //            new Point( CMSPwidth, 0 )
            //        },
            //        new Byte[] {
            //            (Byte)PathPointType.Start,
            //            (Byte)PathPointType.Line,
            //            (Byte)PathPointType.Line,
            //            (Byte)PathPointType.Line,
            //            (Byte)PathPointType.Line,
            //            (Byte)PathPointType.Line,
            //            (Byte)(PathPointType.Line | PathPointType.CloseSubpath )
            //        }
            //);
            //g.FillPath(new SolidBrush(Color.Gray), fillPath);

            //project Horizontally
            int[] projHori = new int[CMSPheight];
            for (int y = 0; y < CMSPheight; y++)
            {
                for (int x = 0; x < CMSPwidth; x++)
                {
                    if (bitmapCMSP.GetPixel(x, y).GetBrightness() > 0.35)
                        projHori[y]++;
                }
#if debug
                    g.DrawLine(new Pen(Color.Green, 1.0f), CMSPwidth, y, CMSPwidth + projHori[y], y);
#endif
            }

            //search line
            int line1YStart = 0, line1YEnd = 0, line2YStart = 0, line2YEnd = 0;
            for (int y = 0; y < CMSPheight; y++)
            {
                if (line1YStart == 0)
                {
                    if (projHori[y] > 5)
                        line1YStart = y;
                    continue;
                }
                if (line1YEnd == 0)
                {
                    if (projHori[y] < 5)
                        line1YEnd = y;
                    continue;
                }
                if (line2YStart == 0)
                {
                    if (projHori[y] > 5)
                        line2YStart = y;
                    continue;
                }
                if (line2YEnd == 0)
                {
                    if (projHori[y] < 5)
                        line2YEnd = y;
                    continue;
                }
            }

            //g.DrawLine(new Pen(Color.White, 1.0f), CMSPwidth, line1YStart, CMSPwidth + 10, line1YStart);
            //g.DrawLine(new Pen(Color.Blue, 1.0f), CMSPwidth, line1YEnd, CMSPwidth + 10, line1YEnd);
            //g.DrawLine(new Pen(Color.White, 1.0f), CMSPwidth, line2YStart, CMSPwidth + 10, line2YStart);
            //g.DrawLine(new Pen(Color.Blue, 1.0f), CMSPwidth, line2YEnd, CMSPwidth + 10, line2YEnd);

            //analyze chars position for each line

            //project image vertically
            int[] line1XMap = new int[CMSPwidth];
            Dictionary<int, int> line1CharXStart = new Dictionary<int, int>();
            Dictionary<int, int> line1CharXEnd = new Dictionary<int, int>();
            for (int x = 0; x < CMSPwidth; x++)
            {
                for (int y = line1YStart; y <= line1YEnd; y++)
                {
                    if (bitmapCMSP.GetPixel(x, y).GetBrightness() > 0.13)
                        line1XMap[x]++;
                }

                //g.DrawLine(new Pen(Color.Green), x, CMSPheight + 10, x, CMSPheight + 10 + line1XMap[x]);
                if (x == 0) continue;

                if (line1XMap[x - 1] == 0 && line1XMap[x] > 0)
                {
                    line1CharXStart.Add(line1CharXStart.Keys.Count + 1, x);
#if debug
                        g.DrawLine(new Pen(Color.White, 0.5f), x, line1YStart, x, line1YEnd);
#endif
                }
                else if (line1XMap[x - 1] > 0 && line1XMap[x] == 0)
                {
                    line1CharXEnd.Add(line1CharXEnd.Keys.Count + 1, x - 1);
#if debug
                        g.DrawLine(new Pen(Color.Blue, 0.5f), x - 1, line1YStart, x - 1, line1YEnd);
#endif
                }
            }

            int[] line2XMap = new int[CMSPwidth];
            Dictionary<int, int> line2CharXStart = new Dictionary<int, int>();
            Dictionary<int, int> line2CharXEnd = new Dictionary<int, int>();
            for (int x = 0; x < CMSPwidth; x++)
            {
                for (int y = line2YStart; y <= line2YEnd; y++)
                {
                    if (bitmapCMSP.GetPixel(x, y).GetBrightness() > 0.2)
                        line2XMap[x]++;
                }
                //g.DrawLine(new Pen(Color.Green), x, CMSPheight + 50, x, CMSPheight + 50 + line2XMap[x]);
                if (x == 0) continue;

                if (line2XMap[x - 1] == 0 && line2XMap[x] > 0)
                {
                    line2CharXStart.Add(line2CharXStart.Keys.Count + 1, x);
#if debug
                        g.DrawLine(new Pen(Color.White, 0.5f), x, line2YStart, x, line2YEnd);
#endif
                }
                else if (line2XMap[x - 1] > 0 && line2XMap[x] == 0)
                {
                    line2CharXEnd.Add(line2CharXEnd.Keys.Count + 1, x - 1);
#if debug
                        g.DrawLine(new Pen(Color.Blue, 0.5f), x - 1, line2YStart, x - 1, line2YEnd);
#endif
                }
            }


#if debug
                StringBuilder strCharInfo = new StringBuilder();
                strCharInfo.Append("l1 Height:").Append(line1YEnd - line1YStart).Append(" ");

                if (pictureBox2.Width <= 0 || pictureBox2.Height <= 0)
                {
                    return;
                }
                Bitmap bitmap2 = new Bitmap(pictureBox2.Width, pictureBox2.Height);
                using (Graphics gBox2 = Graphics.FromImage(bitmap2))
                {
                    gBox2.Clear(Color.Gray);
#endif
            StringBuilder strbLine1 = new StringBuilder();
            foreach (int key in line1CharXStart.Keys)
            {
                if (line1CharXEnd.ContainsKey(key) == false)
                    return;
#if debug
                        strCharInfo.Append(line1CharXStart[key] - line1CharXEnd[key]).Append(" ");
#endif
                Bitmap charBitmap = GetSubBitmap(bitmapCMSP, line1CharXStart[key], line1YStart, line1CharXEnd[key] - line1CharXStart[key], line1YEnd - line1YStart);
                if (charBitmap != null)
                {
#if debug
                            gBox2.DrawImage(charBitmap, line1CharXStart[key], 5);
#endif
                    if (key == 1)
                    {
                        int spaceCount = (int)Math.Round(line1CharXStart[key] / ((line1YEnd - line1YStart) * 0.714));
                        if (spaceCount > 0)
                            strbLine1.Append(string.Format("{0," + spaceCount + "}", " "));

                    }
                    else
                    {
                        int space = line1CharXStart[key] - line1CharXStart[key - 1];
                        if (space >= 2 * (line1YEnd - line1YStart) * 0.714)
                        {
                            int spaceCount = (int)(space / ((line1YEnd - line1YStart) * 0.714));
                            strbLine1.Append(string.Format("{0," + spaceCount + "}", " "));
                        }
                    }
                    byte[] featureVec = GetFeatureVector(charBitmap,
#if debug
 gBox2,
#else
 null,
#endif
 line1CharXStart[key], 25);
                    if (featureVec == null) return;
                    char matchResult = MatchChar(featureVec);
                    strbLine1.Append(matchResult);
#if debug
                            lblCharsInfo[key].Text = FormatFeatureString(featureVec) + "\n[" + matchResult + "]";
#endif
                    charBitmap.Dispose();
                }
            }
#if debug
                    strCharInfo.Append("\n");
                    strCharInfo.Append("l2 Height:").Append(line2YEnd - line2YStart).Append(" ");
#endif
            StringBuilder strbLine2 = new StringBuilder();
            foreach (int key in line2CharXStart.Keys)
            {
                if (line2CharXEnd.ContainsKey(key) == false)
                    return;
#if debug
                        strCharInfo.Append(line2CharXStart[key] - line2CharXEnd[key]).Append(" ");
#endif
                Bitmap charBitmap = GetSubBitmap(bitmapCMSP, line2CharXStart[key], line2YStart, line2CharXEnd[key] - line2CharXStart[key], line2YEnd - line2YStart);
                if (charBitmap != null)
                {
#if debug
                            gBox2.DrawImage(charBitmap, line2CharXStart[key], 50);
#endif
                    //if the distance between this character and the previous one is larger then 2 times of the normal character width then there should be a space 
                    if (key > 1 && line2CharXStart[key] - line2CharXStart[key - 1] >= 1.7 * (line2YEnd - line2YStart) * 0.714)
                    {
                        strbLine2.Append(" ");
                    }

                    byte[] featureVec = GetFeatureVector(charBitmap,
#if debug
 gBox2,
#else
 null,
#endif
 line2CharXStart[key], 70);
                    if (featureVec == null) return;
                    char matchResult = MatchChar(featureVec);
                    strbLine2.Append(matchResult);
#if debug
                            lblCharsInfo[key + line1CharXStart.Count].Text = FormatFeatureString(featureVec) + "\n[" + matchResult + "]";
#endif
                    charBitmap.Dispose();
                }
            }
            lblCMSPResult.Text = strbLine1.Append("\n").Append(strbLine2).ToString();
#if debug
                }
                pictureBox2.Image = bitmap2;
                lblCharInfos.Text = strCharInfo.ToString();
            }
#endif
        }

        private Bitmap GetSubBitmap(Bitmap img, int x, int y, int width, int height)
        {
            if (width <= 0 || height <= 0) return null;
            Bitmap newImg = new Bitmap(width, height);
            using (Graphics g = Graphics.FromImage(newImg))
            {
                g.DrawImage(img, 0, 0, new Rectangle(x, y, width, height), GraphicsUnit.Pixel);
            }
            return newImg;
        }

        private byte[] GetFeatureVectorCMSC(Bitmap img, Graphics debugG, int debugDrawX, int debugDrawY)
        {
            //padding the image width to standard aspect ratio, Height : Widht = 29 :14 
            // we don't need to change the height, just pad the left / right edge is okay.
            double aspectRatio = (double)img.Height / img.Width;
            if (aspectRatio > 2.4)
            {
                double standardWidth = (double)img.Height / 29 * 14;
                int copyOffsetX = (int)((standardWidth - img.Width) / 2);
                Bitmap correctSizeImg = new Bitmap((int)standardWidth, img.Height);
                using (Graphics g = Graphics.FromImage(correctSizeImg))
                {
                    g.Clear(Color.Black);
                    g.DrawImageUnscaled(img, copyOffsetX, 0);
                }
                img.Dispose();
                img = correctSizeImg;
            }


            byte[] feature = new byte[35];

            //the sample point increament in x and y direction
            double xStep = Convert.ToDouble(img.Width) / 5;
            double yStep = Convert.ToDouble(img.Height) / 7;

            for (int y = 0; y < 7; y++)
            {
                for (int x = 0; x < 5; x++)
                {
                    //int samplePosX = (int)(xStep * x) + (int)(xStep / 2);
                    //int samplePosY = (int)(y * yStep) + (int)(xStep / 2);
                    int samplePosX = (int)Math.Round((xStep * x) + (xStep / 2));
                    int samplePosY = (int)Math.Round((y * yStep) + (xStep / 2));
                    if (samplePosX < 0 || samplePosY < 0 || samplePosX >= img.Width || samplePosY >= img.Height)
                        return null;
                    feature[y * 5 + x] = img.GetPixel(samplePosX, samplePosY).GetBrightness() > CMSCthresholdBrightness ? (byte)1 : (byte)0;
#if debug
                    img.SetPixel(samplePosX, samplePosY, Color.Red);
                    debugG.DrawImage(img, debugDrawX + 350, debugDrawY);
#endif
                }
            }
            return feature;
        }

        private byte[] GetFeatureVector(Bitmap img, Graphics debugG, int debugDrawX, int debugDrawY)
        {
            //padding the image width to standard aspect ratio, Height : Widht = 7 : 5
            // we don't need to change the height, just pad the left / right edge is okay.
            double aspectRatio = (double)img.Height / img.Width;
            if (aspectRatio > 1.45)
            {
                double standardWidth = (double)img.Height / 7 * 5;
                int copyOffsetX = (int)((standardWidth - img.Width) / 2);
                Bitmap correctSizeImg = new Bitmap((int)standardWidth, img.Height);
                using (Graphics g = Graphics.FromImage(correctSizeImg))
                {
                    g.Clear(Color.Black);
                    g.DrawImageUnscaled(img, copyOffsetX, 0);
                }
                img.Dispose();
                img = correctSizeImg;
            }


            byte[] feature = new byte[35];

            //the sample point increament in x and y direction
            double xStep = Convert.ToDouble(img.Width) / 5;
            double yStep = Convert.ToDouble(img.Height) / 7;


            for (int y = 0; y < 7; y++)
            {
                for (int x = 0; x < 5; x++)
                {
                    //int samplePosX = (int)(xStep * x) + (int)(xStep / 2);
                    //int samplePosY = (int)(y * yStep) + (int)(xStep / 2);
                    int samplePosX = (int)Math.Round((xStep * x) + (xStep / 2));
                    int samplePosY = (int)Math.Round((y * yStep) + (xStep / 2));
                    if (samplePosX < 0 || samplePosY < 0 || samplePosX >= img.Width || samplePosY >= img.Height)
                        return null;
                    feature[y * 5 + x] = img.GetPixel(samplePosX, samplePosY).GetBrightness() > 0.33 ? (byte)1 : (byte)0;
#if debug
                    img.SetPixel(samplePosX, samplePosY, Color.Red);
                    debugG.DrawImage(img, debugDrawX, debugDrawY);
#endif
                }
            }
            return feature;
        }

        /// <summary>
        /// Match a character by using it's feature vector
        /// </summary>
        /// <param name="feature"></param>
        /// <returns></returns>
        private char MatchChar(byte[] feature)
        {
            SortedList<int, char> MatchDiffList = new SortedList<int, char>();
            foreach (char key in CharFeatureTemplate.Keys)
            {
                int diffCount = 0;
                for (int i = 0; i < 35; i++)
                {
                    if (feature[i] != CharFeatureTemplate[key][i])
                        diffCount++;
                }
                if (MatchDiffList.ContainsKey(diffCount) == false)
                    MatchDiffList.Add(diffCount, key);
            }
            return MatchDiffList.First().Value;
        }

        private string FormatFeatureString(byte[] feature)
        {
            StringBuilder strBuilder = new StringBuilder();
            foreach (int value in feature)
            {
                strBuilder.AppendLine(value.ToString());
            }
            return strBuilder.ToString();
        }

        private void lblResult_TextChanged(object sender, EventArgs e)
        {
            int port = 0;
            if (int.TryParse(txtPort.Text, out port) == false)
                return;

            System.Net.Sockets.UdpClient udp = new System.Net.Sockets.UdpClient(txtIP.Text, port);

            byte header = 11;
            List<byte> msg = new List<byte>();
            msg.Add(header);
            msg.AddRange(System.Text.Encoding.ASCII.GetBytes(lblCMSPResult.Text));
            udp.Send(msg.ToArray(), msg.Count);
        }

        private void timerCMSC_Tick(object sender, EventArgs e)
        {
            graphCMSC.CopyFromScreen(CMSC_X, CMSC_Y, 0, 0, new Size(CMSCwidth, CMSCheight));
#if debug
            pictureBoxCMSC.Image = bitmapCMSC;
#endif
            AnalyzeCMSC();
        }

        private void AnalyzeCMSC()
        {
            double charWHRatio = 0.48276;
#if debug
            using (Graphics g = Graphics.FromImage(bitmapCMSC))
            {
#endif
            //project Horizontally
            int[] projHori = new int[CMSCheight];
            for (int y = 0; y < CMSCheight; y++)
            {
                for (int x = 0; x < CMSCwidth; x++)
                {
                    if (bitmapCMSC.GetPixel(x, y).GetBrightness() > CMSCthresholdBrightness)
                        projHori[y]++;
                }
#if debug
                    g.DrawLine(new Pen(Color.Green, 1.0f), CMSCwidth, y, CMSCwidth + projHori[y], y);
#endif
            }

            //search line
            int line1YStart = 0, line1YEnd = 0, line2YStart = 0, line2YEnd = 0;
            for (int y = 0; y < CMSCheight; y++)
            {
                if (line1YStart == 0)
                {
                    if (projHori[y] > 5)
                        line1YStart = y;
                    continue;
                }
                if (line1YEnd == 0)
                {
                    if (projHori[y] < 5)
                        line1YEnd = y;
                    continue;
                }
                if (line2YStart == 0)
                {
                    if (projHori[y] > 5)
                        line2YStart = y;
                    continue;
                }
                if (line2YEnd == 0)
                {
                    if (projHori[y] < 5)
                        line2YEnd = y;
                    continue;
                }
            }

            //g.DrawLine(new Pen(Color.White, 1.0f), CMSCwidth, line1YStart, CMSCwidth + 10, line1YStart);
            //g.DrawLine(new Pen(Color.Blue, 1.0f), CMSCwidth, line1YEnd, CMSCwidth + 10, line1YEnd);
            //g.DrawLine(new Pen(Color.White, 1.0f), CMSCwidth, line2YStart, CMSCwidth + 10, line2YStart);
            //g.DrawLine(new Pen(Color.Blue, 1.0f), CMSCwidth, line2YEnd, CMSCwidth + 10, line2YEnd);

            //analyze chars position for each line

            //project image vertically
            int[] line1XMap = new int[CMSCwidth];
            Dictionary<int, int> line1CharXStart = new Dictionary<int, int>();
            Dictionary<int, int> line1CharXEnd = new Dictionary<int, int>();
            //split the line1 character
            for (int x = 0; x < CMSCwidth; x++)
            {
                for (int y = line1YStart; y <= line1YEnd; y++)
                {
                    if (bitmapCMSC.GetPixel(x, y).GetBrightness() > CMSCthresholdBrightness)
                        line1XMap[x]++;
                }

                //g.DrawLine(new Pen(Color.Green), x, CMSCheight + 10, x, CMSCheight + 10 + line1XMap[x]);
                if (x == 0) continue;

                if (line1XMap[x - 1] == 0 && line1XMap[x] > 0)
                {
                    line1CharXStart.Add(line1CharXStart.Keys.Count + 1, x);
#if debug
                        g.DrawLine(new Pen(Color.White, 0.5f), x, line1YStart, x, line1YEnd);
#endif
                }
                else if (line1XMap[x - 1] > 0 && line1XMap[x] == 0)
                {
                    line1CharXEnd.Add(line1CharXEnd.Keys.Count + 1, x - 1);
#if debug
                        g.DrawLine(new Pen(Color.Blue, 0.5f), x - 1, line1YStart, x - 1, line1YEnd);
#endif
                }
            }

            int[] line2XMap = new int[CMSCwidth];
            Dictionary<int, int> line2CharXStart = new Dictionary<int, int>();
            Dictionary<int, int> line2CharXEnd = new Dictionary<int, int>();
            //split the line2 character
            for (int x = 0; x < CMSCwidth; x++)
            {
                for (int y = line2YStart; y <= line2YEnd; y++)
                {
                    if (bitmapCMSC.GetPixel(x, y).GetBrightness() > CMSCthresholdBrightness)
                        line2XMap[x]++;
                }
                //g.DrawLine(new Pen(Color.Green), x, CMSCheight + 50, x, CMSCheight + 50 + line2XMap[x]);
                if (x == 0) continue;

                if (line2XMap[x - 1] == 0 && line2XMap[x] > 0)
                {
                    line2CharXStart.Add(line2CharXStart.Keys.Count + 1, x);
#if debug
                        g.DrawLine(new Pen(Color.White, 0.5f), x, line2YStart, x, line2YEnd);
#endif
                }
                else if (line2XMap[x - 1] > 0 && line2XMap[x] == 0)
                {
                    line2CharXEnd.Add(line2CharXEnd.Keys.Count + 1, x - 1);
#if debug
                        g.DrawLine(new Pen(Color.Blue, 0.5f), x - 1, line2YStart, x - 1, line2YEnd);
#endif
                }
            }

#if debug
                StringBuilder strCharInfo = new StringBuilder();
                strCharInfo.Append("l1 Height:").Append(line1YEnd - line1YStart).Append(" ");

                if (pictureBoxCMSCDebug.Width <= 0 || pictureBoxCMSCDebug.Height <= 0)
                {
                    return;
                }
                Bitmap bitmap2 = new Bitmap(pictureBoxCMSCDebug.Width, pictureBoxCMSCDebug.Height);
                using (Graphics gBox2 = Graphics.FromImage(bitmap2))
                {
                    gBox2.Clear(Color.Gray);
#endif
            StringBuilder strbLine1 = new StringBuilder();
            foreach (int key in line1CharXStart.Keys)
            {
                if (line1CharXEnd.ContainsKey(key) == false)
                    return;
#if debug
                        strCharInfo.Append(line1CharXStart[key] - line1CharXEnd[key]).Append(" ");
#endif
                Bitmap charBitmap = GetSubBitmap(bitmapCMSC, line1CharXStart[key], line1YStart, line1CharXEnd[key] - line1CharXStart[key], line1YEnd - line1YStart);
                if (charBitmap != null)
                {
#if debug
                            gBox2.DrawImage(charBitmap, line1CharXStart[key], 5);
#endif
                    if (key == 1)
                    {
                        int spaceCount = (int)Math.Round(line1CharXStart[key] / ((line1YEnd - line1YStart) * charWHRatio));
                        if (spaceCount > 0)
                            strbLine1.Append(string.Format("{0," + spaceCount + "}", " "));
                    }
                    else
                    {
                        int space = line1CharXStart[key] - line1CharXStart[key - 1];
                        if (space >= 2 * (line1YEnd - line1YStart) * charWHRatio)
                        {
                            int spaceCount = (int)(space / ((line1YEnd - line1YStart) * charWHRatio));
                            strbLine1.Append(string.Format("{0," + spaceCount + "}", " "));
                        }
                    }
                    byte[] featureVec = GetFeatureVectorCMSC(charBitmap,
#if debug
 gBox2,
#else
 null,
#endif
 line1CharXStart[key], 5);
                    if (featureVec == null) return;
                    char matchResult = MatchChar(featureVec);
                    strbLine1.Append(matchResult);
#if debug
                            lblCharsInfo[key].Text = FormatFeatureString(featureVec) + "\n[" + matchResult + "]";
#endif
                    charBitmap.Dispose();
                }
            }
#if debug
                    strCharInfo.Append("\n");
                    strCharInfo.Append("l2 Height:").Append(line2YEnd - line2YStart).Append(" ");
#endif
            StringBuilder strbLine2 = new StringBuilder();
            foreach (int key in line2CharXStart.Keys)
            {
                if (line2CharXEnd.ContainsKey(key) == false)
                    return;
#if debug
                        strCharInfo.Append(line2CharXStart[key] - line2CharXEnd[key]).Append(" ");
#endif
                Bitmap charBitmap = GetSubBitmap(bitmapCMSC, line2CharXStart[key], line2YStart, line2CharXEnd[key] - line2CharXStart[key], line2YEnd - line2YStart);
                if (charBitmap != null)
                {
#if debug
                            gBox2.DrawImage(charBitmap, line2CharXStart[key], 70);
#endif
                    //if the distance between this character and the previous one is larger then 2 times of the normal character width then there should be a space 
                    if (key > 1 && line2CharXStart[key] - line2CharXStart[key - 1] >= 1.7 * (line2YEnd - line2YStart) * charWHRatio)
                    {
                        strbLine2.Append(" ");
                    }

                    byte[] featureVec = GetFeatureVectorCMSC(charBitmap,
#if debug
 gBox2,
#else
 null,
#endif
 line2CharXStart[key], 70);
                    if (featureVec == null) return;
                    char matchResult = MatchChar(featureVec);
                    strbLine2.Append(matchResult);
#if debug
                            lblCharsInfo[key + line1CharXStart.Count].Text = FormatFeatureString(featureVec) + "\n[" + matchResult + "]";
#endif
                    charBitmap.Dispose();
                }
            }
            lblCMSCResult.Text = strbLine1.Append(" ").Append(strbLine2).ToString();
#if debug
                }
                pictureBoxCMSCDebug.Image = bitmap2;
                lblDebugCMSC.Text = strCharInfo.ToString();
            }
#endif
        }

        private void lblCMSCResult_TextChanged(object sender, EventArgs e)
        {
            int port = 0;
            if (int.TryParse(txtPort.Text, out port) == false)
                return;

            Match matchCMSC = regexCMSCField.Match(lblCMSCResult.Text);
            StringBuilder cmscBuilder = new StringBuilder();
            if (matchCMSC != null && matchCMSC.Groups != null && matchCMSC.Groups.Count > 0)
            {
                cmscBuilder.Append(matchCMSC.Groups[1].Value.PadRight(8)).Append(" ").Append(matchCMSC.Groups[2].Value.PadRight(8)).Append(matchCMSC.Groups[3].Value);
            }


            System.Net.Sockets.UdpClient udp = new System.Net.Sockets.UdpClient(txtIP.Text, port);
            byte header = 12;
            List<byte> msg = new List<byte>();
            msg.Add(header);
            msg.AddRange(System.Text.Encoding.ASCII.GetBytes(cmscBuilder.ToString()));
            udp.Send(msg.ToArray(), msg.Count);
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

    }
}
