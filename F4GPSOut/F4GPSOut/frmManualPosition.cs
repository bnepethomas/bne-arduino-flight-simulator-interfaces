using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;


namespace F4GPSOut
{
    public partial class frmManualPosition : Form
    {
        static public double iPositionMultiplier = 0;
        static public double iNorthOrSouth = 0;         // +1 North -1 South 0 static
        static public double iEastorWest = 0;           // +1 East -1 West 0 static
        static public double iUporDown = 0;             // +1 Inc Alt -1 Dec Alt
        static public double iPortorStarbard = 0;       // +1 Inc Heading - 1 Dec heading

        public frmManualPosition()
        {
            InitializeComponent();

        }

        private void cancelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Dispose();
        }

        private void frmManualPosition_Load(object sender, EventArgs e)
        {
            // this.lblCurrentLat.Text = GPSOut_Main.gLat.ToString("0.#####");
            // this.lblCurrentLong.Text = GPSOut_Main.gLong.ToString("0.#####");
            this.lblCurrentLat.Text = GPSOut_Main.Lomac_GPS_Latitude.ToString("0.#####");
            this.lblCurrentLat.Text = GPSOut_Main.Lomac_GPS_Longitude.ToString("0.#####");
            this.timer1.Enabled = true;

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            GPSOut_Main.gLat = GPSOut_Main.gLat + iPositionMultiplier * iNorthOrSouth / 10000;
            GPSOut_Main.gLong = GPSOut_Main.gLong + iPositionMultiplier * iEastorWest / 10000;
 
            
            // Check we haven't fallen off the earth
            if (GPSOut_Main.gLat > 90)
            {
                GPSOut_Main.gLat = -90;
            }
            else if (GPSOut_Main.gLat < -90)
            {
                GPSOut_Main.gLat = 90;
            }
            // Check we haven't fallen off the earth
            if (GPSOut_Main.gLong > 180)
            {
                GPSOut_Main.gLong = -180;
            }
            else if (GPSOut_Main.gLong < -180)
            {
                GPSOut_Main.gLong = 180;
            }
            this.lblCurrentLat.Text = GPSOut_Main.gLat.ToString("0.#####");
            this.lblCurrentLong.Text = GPSOut_Main.gLong.ToString("0.#####");

            // this.lblCurrentLat.Text = GPSOut_Main.Lomac_GPS_Latitude.ToString("0.#####");
            // this.lblCurrentLong.Text = GPSOut_Main.Lomac_GPS_Longitude.ToString("0.#####");

            GPSOut_Main.Lomac_GPS_Latitude = GPSOut_Main.gLat;
            GPSOut_Main.Lomac_GPS_Longitude = GPSOut_Main.gLong;

            GPSOut_Main.Lomac_GPS_Altitude = GPSOut_Main.Lomac_GPS_Altitude + iUporDown;

            GPSOut_Main.Lomac_GPS_Heading = GPSOut_Main.Lomac_GPS_Heading + iPortorStarbard;
            if (GPSOut_Main.Lomac_GPS_Heading < 0)
            {
                GPSOut_Main.Lomac_GPS_Heading = 359;
            }
            else if (GPSOut_Main.Lomac_GPS_Heading > 359)
            {
                GPSOut_Main.Lomac_GPS_Heading = 0;
            }
        }

        private void rbSpeed0_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = false;
            iPositionMultiplier = 0;
        }

        private void rbSpeed1_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
            iPositionMultiplier = 1;          
        }


        private void rbSpeed10_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
            iPositionMultiplier = 10; 
        }

        private void rbSpeed100_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
            iPositionMultiplier = 100; 
        }

        private void rbSpeed1000_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
            iPositionMultiplier = 1000; 
        }

        private void rbSpeed10000_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Enabled = true;
            iPositionMultiplier = 10000; 
        }

        private void rbNorth_CheckedChanged(object sender, EventArgs e)
        {
            iNorthOrSouth = 1;
        }

        private void rbNorthSouthStatic_CheckedChanged(object sender, EventArgs e)
        {
            iNorthOrSouth = 0;
        }

        private void rbSouth_CheckedChanged(object sender, EventArgs e)
        {
            iNorthOrSouth = -1;
        }

        private void rbWest_CheckedChanged(object sender, EventArgs e)
        {
            iEastorWest = -1;
        }

        private void rbEastWestStatic_CheckedChanged(object sender, EventArgs e)
        {
            iEastorWest = 0;
        }

        private void rbEast_CheckedChanged(object sender, EventArgs e)
        {
            iEastorWest = 1;
        }

        private void rbAltInc_CheckedChanged(object sender, EventArgs e)
        {
            iUporDown = 10;
        }

        private void rbAltStatic_CheckedChanged(object sender, EventArgs e)
        {
            iUporDown = 0;
        }

        private void rbAltDec_CheckedChanged(object sender, EventArgs e)
        {
            iUporDown = -10;
        }

        private void rbHDGPort_CheckedChanged(object sender, EventArgs e)
        {
            iPortorStarbard = -1;
        }

        private void rbHDGStatic_CheckedChanged(object sender, EventArgs e)
        {
            iPortorStarbard = 0;
        }

        private void rbHDGStarboard_CheckedChanged(object sender, EventArgs e)
        {
            iPortorStarbard = 1;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Check both a valid numbers +89 -89, +180 -180
            // assign to lat long
            GPSOut_Main.gLat = double.Parse(this.txtInputLat.Text);
            GPSOut_Main.gLong = double.Parse (this.txtInputLong.Text);
        }

        private void brisbaneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            GPSOut_Main.gLat = -27.3873;
            GPSOut_Main.gLong = 153.1301;
        }

        private void queenstownToolStripMenuItem_Click(object sender, EventArgs e)
        {
            GPSOut_Main.gLat = -45.0218;
            GPSOut_Main.gLong = 168.7472;
        }

        private void uRKAToolStripMenuItem_Click(object sender, EventArgs e)
        {
            GPSOut_Main.gLat = 45.0042;
            GPSOut_Main.gLong = 37.349;
        }
            



    }
}
